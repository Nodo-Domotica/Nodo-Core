/**************************************************************************\

    This file is part of Nodo Due, © Copyright Paul Tonkes

    Nodo Due is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Nodo Due is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Nodo Due.  If not, see <http://www.gnu.org/licenses/>.

\**************************************************************************/



 /**********************************************************************************************\
 * Voert alle events uit die correspenderen met de Code
 * Revision 01, 09-12-2009, P.K.Tonkes@gmail.com
 \*********************************************************************************************/
#define MACRO_EXECUTION_DEPTH 10 // maximale nesting van macro's.
 
boolean ProcessEvent(unsigned long IncommingEvent, byte Direction, byte Port, unsigned long PreviousContent, byte PreviousPort)
  {
  unsigned long Event_1, Event_2;
  byte Type=EventType(IncommingEvent);
  byte Command=EventPart(IncommingEvent,EVENT_PART_COMMAND);
  byte x,y,z;
  static byte depth=0;  // teller die bijhoudt hoe vaak er binnen een macro weer een macro wordt uitgevoerd. Voorkomt tevens vastlopers a.g.v. loops die door een gebruiker zijn gemaakt met macro's

  // Uitvoeren voorafgaand aan een reeks uitvoeren
  if(depth==0)
   {
   if(S.Trace&1)PrintLine();  
   if(S.WaitFreeRFAction==VALUE_SERIES)WaitFreeRF(S.WaitFreeRFWindow);
   }

  //??? issue 123 if(depth==0 || S.Trace&1)PrintEvent(IncommingEvent,Port,VALUE_DIRECTION_INPUT);  // geef event weer op Serial
  PrintEvent(IncommingEvent,Port,Direction);  // geef event weer op Serial
  digitalWrite(MonitorLedPin,HIGH);          // LED aan om aan te geven dat er wat ontvangen is en verwerkt wordt
  
  if(depth++>=MACRO_EXECUTION_DEPTH)
    {
    PrintText(Text_50,true);
    depth=0;
    return false; // bij geneste loops ervoor zorgen dat er niet meer dan MACRO_EXECUTION_DEPTH niveaus diep macro's uitgevoerd worden
    }

  // ############# Verwerk event ################  
  // als het een commando of event is voor deze unit, dan t.b.v. interne verwerking unit op 0 zetten.
  if(Type==VALUE_TYPE_COMMAND)
    { // Er is een Commando binnengekomen 
    if(!ExecuteCommand(IncommingEvent,Port,PreviousContent,PreviousPort))
      {
      depth--;//??? Issue 141 hier wel een return. Toegevoegd voor Issue 141
      return false;//??? 
      }
    }
  else
    { // Er is een Event binnengekomen  
    // loop de gehele eventlist langs om te kijken of er een treffer is.    
    for(x=1; x<=Eventlist_MAX && Eventlist_Read(x,&Event_1,&Event_2); x++)
      {
      // kijk of deze regel een match heeft, zo ja, dan set y=vlag voor uitvoeren
      
      // als de Eventlist regel een wildcard is, zo ja, dan set y=vlag voor uitvoeren
      if(((Event_1>>16)&0xff)==CMD_WILDCARD_EVENT) // commando deel van het event.
        {
        y=true; 
        z=(Event_1>>8)&0xff; // Par1 deel bevat de poort
        if(z!=VALUE_ALL && z!=Port)y=false;
        z=Event_1&0xff; // Par2 deel bevat type event
        if(z!=VALUE_ALL && z!=Type)y=false;
        }
      else
        y=CheckEvent(IncommingEvent,Event_1);      
      
      if(y)
        {
        if(S.Trace&1)
          PrintEventlistEntry(x,depth);
          
        if(EventPart(Event_2,EVENT_PART_COMMAND)==EVENT_PART_COMMAND) // is de ontvangen code een uitvoerbaar commando?
          {
          if(!ExecuteCommand(Event_2, VALUE_SOURCE_EVENTLIST,IncommingEvent,Port))
            {
            depth--;
            return false;//??? moet dit false worden? Issue 141
            }
          }
        else
          {// het is een ander soort event;
          if(Event_1!=command2event(CMD_WILDCARD_EVENT,0,0))
            {
            if(!ProcessEvent(Event_2,VALUE_DIRECTION_INTERNAL,VALUE_SOURCE_EVENTLIST,IncommingEvent,Port))
              {
              depth--;
              return true;
              }
            }
          }
        }
      }
    }
  depth--;
  return true;
  }


 /**********************************************************************************************\
 * Toetst of de Code ergens voorkomt in de Eventlist. Geeft False als de opgegeven code niet bestaat.
 \*********************************************************************************************/
boolean CheckEventlist(unsigned long Code)
  {
  unsigned long Event, Action;

  for(byte x=1; x<=Eventlist_MAX;x++)
    {
    Eventlist_Read(x,&Event,&Action);
    if(CheckEvent(Code,Event))return true; // match gevonden
    if(Event==0L)break;
    }
  return false;
  }


 /**********************************************************************************************\
 * Vergelijkt twee events op matching voor uitvoering Eventlist
 \*********************************************************************************************/
boolean CheckEvent(unsigned long Event, unsigned long MacroEvent)
  {  
  // als huidige event exact overeenkomt met het event in de regel uit de Eventlist, dan een match
  if(MacroEvent==Event)return true; 
  
  // als Home niet overeenkomt, dan geen match
  if(EventPart(Event,EVENT_PART_HOME)!=S.Home)return false; 
  
  // Als unit ongelijk aan 0 of ongelijk aan huidige unit, dan is er ook geen match
  int x=EventPart(Event,EVENT_PART_UNIT);
  if(x!=0 && x!=S.Unit)return false; 

  // als huidige event (met wegfilterde home en unit ) gelijk is aan MacroEvent, dan een match
  Event&=0x00ffffff;
  MacroEvent&=0x00ffffff;
  if(MacroEvent==Event)return true; 

  // is er een match met een CLOCK_EVENT_ALL event?
  if((MacroEvent&0x0000ffff)==(Event&0x0000ffff)) // tijdstippen kloppen
    if(EventPart(MacroEvent,EVENT_PART_COMMAND)==CMD_CLOCK_EVENT_ALL) // En het is een ClockAll event.
      return true;
 
  return false;
  }


 /**********************************************************************************************\
 * Schrijft een event in de Eventlist. Deze Eventlist bevindt zich in het EEPROM geheugen.
 \*********************************************************************************************/
boolean Eventlist_Write(int address, unsigned long Event, unsigned long Action)// LET OP: eerste adres=1
  {
  unsigned long TempEvent,TempAction;

  // als adres=0, zoek dan de eerste vrije plaats.
  if(address==0)
    {
    address++;
    while(Eventlist_Read(address,&TempEvent,&TempAction))address++;
    }
  if(address>Eventlist_MAX)return false;// geen geldig adres meer c.q. alles is vol.

  address--;// echte adressering begint vanaf nul. voor de user vanaf 1.
  address=address*8+Eventlist_OFFSET;

  EEPROM.write(address++,(Event>>24  & 0xFF));
  EEPROM.write(address++,(Event>>16  & 0xFF));
  EEPROM.write(address++,(Event>> 8  & 0xFF));
  EEPROM.write(address++,(Event      & 0xFF));
  EEPROM.write(address++,(Action>>24 & 0xFF));
  EEPROM.write(address++,(Action>>16 & 0xFF));
  EEPROM.write(address++,(Action>> 8 & 0xFF));
  EEPROM.write(address++,(Action     & 0xFF));

  // Eerste volgende Event vullen met een 0. Dit markeert het einde van de Eventlist.
  EEPROM.write(address++,0);
  EEPROM.write(address++,0);
  EEPROM.write(address++,0);
  EEPROM.write(address  ,0);
  
  return true;
  }
  
 /**********************************************************************************************\
 * 
 * Revision 01, 09-12-2009, P.K.Tonkes@gmail.com
 \*********************************************************************************************/
boolean Eventlist_Read(int address, unsigned long *Event, unsigned long *Action)// LET OP: eerste adres=1
  {
  if(address>Eventlist_MAX)return(false);
  address--;// echte adressering begint vanaf nul. voor de user vanaf 1.
  address=address*8+Eventlist_OFFSET;

  *Event  =((unsigned long)(EEPROM.read(address++))) << 24;
  *Event|= ((unsigned long)(EEPROM.read(address++))) << 16;
  *Event|= ((unsigned long)(EEPROM.read(address++))) <<  8;
  *Event|= ((unsigned long)(EEPROM.read(address++)))      ;
  *Action =((unsigned long)(EEPROM.read(address++))) << 24;
  *Action|=((unsigned long)(EEPROM.read(address++))) << 16;
  *Action|=((unsigned long)(EEPROM.read(address++))) <<  8;
  *Action|=((unsigned long)(EEPROM.read(address  )))      ;
  if(*Event==0L)
    return(false);
  else
    return(true);
  }


 /**********************************************************************************************\
 * verzendt een event en geeft dit tevens weer op SERAIL
 \**********************************************************************************************/
 boolean SendEvent(unsigned long Event)
   {

   if(S.TransmitPort== VALUE_SOURCE_IR || S.TransmitPort== VALUE_SOURCE_IR_RF)
     {
     Nodo_2_RawSignal(Event);
     PrintEvent(Event, VALUE_SOURCE_IR,VALUE_DIRECTION_OUTPUT);
     RawSendIR();
     } 
   if(S.TransmitPort== VALUE_SOURCE_RF || S.TransmitPort== VALUE_SOURCE_IR_RF)
     {
     if(S.WaitFreeRFAction==VALUE_ALL)WaitFreeRF(S.WaitFreeRFWindow);
     Nodo_2_RawSignal(Event);
     PrintEvent(Event, VALUE_SOURCE_RF,VALUE_DIRECTION_OUTPUT);
     RawSendRF();
     }
   }
 
 
