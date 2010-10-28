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
 
boolean ProcessEvent(unsigned long IncommingEvent, byte Port, unsigned long PreviousContent, byte PreviousPort)
  {
  unsigned long Event_1, Event_2;
  byte Type=EventType(IncommingEvent);
  byte Command=EventPart(IncommingEvent,EVENT_PART_COMMAND);
  byte x,y,z;
  static byte depth=0;  // teller die bijhoudt hoe vaak er binnen een macro weer een macro wordt uitgevoerd. Voorkomt tevens vastlopers a.g.v. loops die door een gebruiker zijn gemaakt met macro's

  // Uitvoeren voorafgaand aan een reeks
  if(depth==0)
   {
   if(S.WaitForFreeRF==WAITFREERF_SERIES)WaitForFreeRF();
   if(S.Trace&1)PrintLine();
   }

//  debugging:
//    Serial.print("??? ProcessEvent(): IncommingEvent=");
//    Serial.print(IncommingEvent,HEX);
//    Serial.print(", Command=");
//    Serial.print(Command,HEX);
//    Serial.print(", Type=");
//    Serial.print(Type,HEX);
//    PrintTerm();

  PrintEvent(IncommingEvent,Port,DIRECTION_IN);  // geef event weer op Serial
  digitalWrite(MonitorLedPin,HIGH);          // LED aan om aan te geven dat er wat ontvangen is en verwerkt wordt
  
  if(depth++>=MACRO_EXECUTION_DEPTH)
    {
    Serial.print(Text(Text_50));PrintTerm();
    depth=0;
    return false; // bij geneste loops ervoor zorgen dat er niet meer dan MACRO_EXECUTION_DEPTH niveaus diep macro's uitgevoerd worden
    }

    // ############# Divert event ################  

    // Check of het type binnengekomen event in aanmerking moet komen voor forwarding
    x=false;       
    if(DivertUnit!=S.Unit)
      {
      if(S.DivertType==DIVERT_TYPE_USEREVENT && Command==CMD_USER_EVENT)x=true;
      if(S.DivertType==DIVERT_TYPE_EVENTS && (Type==CMD_TYPE_EVENT || Type==CMD_TYPE_UNKNOWN))x=true;
      if(S.DivertType==DIVERT_TYPE_ALL)x=true;
      }
       
    if(x)
      {      
      //Maak Raw signaal gereed met het te forwarden event
      if(EventPart(IncommingEvent,EVENT_PART_HOME)==S.Home)// Behoort event tot huidige Home?
        {// Huidig home adres
        switch(Command)
          {
          case CMD_USER_EVENT:
            Event_1=(IncommingEvent&0x00ffffff | ((unsigned long)S.Home)<<28); // Voeg Home toe en Maak Unit=0 want een UserEvent is ALTIJD voor ALLE Nodo's.
            Nodo_2_RawSignal(Event_1);  
            break;
  
          case CMD_KAKU:
            // KAKU ontvangers kunnen Nodo formaat niet ontvangen. Als het een KAKU event is, dan verzenden met SendKAKU
            Event_1=(IncommingEvent&0xff00ffff) | ((unsigned long)CMD_KAKU)<<16;
            KAKU_2_RawSignal(Event_1);// nu kan er worden verzonden volgens KAKU formaat
            break;
  
          case CMD_KAKU_NEW:
            // KAKU ontvangers kunnen Nodo formaat niet ontvangen. Als het een NewKAKU event is, dan verzenden met SendNewKAKU
            Event_1=(IncommingEvent&0xff00ffff) | ((unsigned long)CMD_KAKU_NEW)<<16;
            NewKAKU_2_RawSignal(Event_1);// nu kan er worden verzonden volgens NewKAKU formaat
            break;
            
          default:
            {
            // Te forwarden event voorzien van nieuwe bestemming unit
            Event_1=(IncommingEvent&0xf0ffffff) | ((unsigned long)(DivertUnit))<<24; // Event_1 is het te forwarden event voorzien van nieuwe bestemming unit
            Nodo_2_RawSignal(Event_1);
            }
          }// switch
        }// Huidig home adres
      else 
        {// Ander Home adres of onbekend signaal
        Event_1=IncommingEvent; // onveranderd forwarden
        Nodo_2_RawSignal(Event_1);
        }
  
      // RawSignal buffer is gevuld en kan worden verzonden.      
      if(S.Trace&1)
        {
        Serial.print(Text(Text_07));
        Serial.print(DivertUnit,DEC);
        PrintTerm();
        }
  
      if(Port!=CMD_PORT_IR && (S.DivertPort==DIVERT_PORT_IR || S.DivertPort==DIVERT_PORT_IR_RF))
        {
        PrintEvent(Event_1,CMD_PORT_IR,DIRECTION_OUT);
        RawSendIR();
        } 
        
      if(Port!=CMD_PORT_RF && (S.DivertPort==DIVERT_PORT_RF || S.DivertPort==DIVERT_PORT_IR_RF))
        {
        PrintEvent(Event_1,CMD_PORT_RF,DIRECTION_OUT);
        RawSendRF();
        }
  
      if(DivertUnit!=0)// als event alleen maar voor een andere Nodo was.
        {
        depth--;
        return true;
        }
      }// in aanmerking voor forwarding

  // ############# Verwerk event ################  

  // als het een Unitnummer nul dan is deze voor ALLE Nodo's en moet deze OOK worden behandeld.
  if(EventPart(IncommingEvent,EVENT_PART_UNIT)==0 && (Type==CMD_TYPE_COMMAND || Type==CMD_TYPE_EVENT))
    IncommingEvent=(IncommingEvent&0xf0ffffff) | (((unsigned long)S.Unit)<<24); // Unitnummer van verzendende Nodo vervangen voor interne verwerking door eigen unitnummer.

  // Verwerk binnengekomen event.  
  if(Type==CMD_TYPE_COMMAND)
    { // Er is een Commando binnengekomen 
    ExecuteCommand(IncommingEvent,Port,PreviousContent,PreviousPort);
    }
  else
    { // Er is een ander soort Event binnengekomen  
    // loop de gehele eventlist langs om te kijken of er een treffer is.    
    for(x=1; x<=Eventlist_MAX && Eventlist_Read(x,&Event_1,&Event_2); x++)
      {
      // kijk of deze regel een match heeft, zo ja, dan uitvoeren
      y=CheckEvent(IncommingEvent,Event_1);
  
      // als de Eventlist regel een wildcard is
      if(((Event_1>>16)&0xff)==CMD_WILDCARD_EVENT)
        {
        y=true; 
        z=(Event_1>>8)&0xff;
        if(z>0 && z!=Port)y=false;
        z=Event_1&0xff;
        if(z>0 && z!=Type)y=false;
        }
      
      if(y)
        {
        if(S.Trace&1)
          PrintEventlistEntry(x,depth);
        
        if(EventPart(Event_2,EVENT_PART_COMMAND)) // is de ontvangen code een uitvoerbaar commando?
          {
          if(!ExecuteCommand(Event_2,CMD_SOURCE_MACRO,IncommingEvent,Port))
            {
            depth--;
            return true;
            }
          }
        else
          {// het is een ander soort event;
          if(Event_1!=command2event(CMD_WILDCARD_EVENT,0,0))
            {
            if(S.Trace&1)
              PrintEvent(Event_2,CMD_SOURCE_MACRO,DIRECTION_IN);
  
            if(!ProcessEvent(Event_2,CMD_SOURCE_MACRO,IncommingEvent,Port))
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
  // kijk of het huidige event exact voorkomt met het event in de regel uit de Eventlist:
  if(MacroEvent==Event)
    return true; // event is gelijk aan die uit de macrolist, dus er is een match.

  // is er een match met een CLOCK_EVENT_ALL event?
  if((MacroEvent&0xff00ffff)==(Event&0xff00ffff)) // home, unit en tijdstip kloppen
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

  // Eerste volgende Event vullen met een 0. Dit markeert het einde van de Eventlist
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

