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

 
boolean ProcessEvent(unsigned long IncommingEvent, byte Direction, byte Port, unsigned long PreviousContent, byte PreviousPort)
  {
  unsigned long Event_1, Event_2;
  byte Command=(IncommingEvent>>16)&0xff;
  byte w,x,y,z;

  digitalWrite(MonitorLedPin,HIGH);          // LED aan om aan te geven dat er wat ontvangen is en verwerkt wordt

  // Als de RAW pulsen worden opgevraagd door de gebruiker...
  if(RawsignalGet)
    {
    PrintRawSignal();
    RawsignalGet=false;
    return true;
    }

  // Uitvoeren voorafgaand aan een reeks uitvoeren
  if(EventlistDepth==0 && S.Trace)
      PrintLine(); 

  // als de Nodo in de hold is gezet, verzamel dan relevante events in de queue
  if(HoldTimer>millis())
    {
    // als het event de opdracht betreft om de hold uit te zetten, dan dit doen.
    if(IncommingEvent==command2event(CMD_DELAY,0,0))
      HoldTimer=0L; //  HoldTimer is NU, dus wachttijd is afgelopen;
    else
      {
      // als het event voorkomt in de eventlist of het is een geldig commando voor deze Nodo, dan is het relevant om op te slaan
      if(CheckEventlist(IncommingEvent) ||   (IncommingEvent>>28)&0xf==EVENT_TYPE_NODO)
        {
        // als er nog plek is in de queue...
        if(QueuePos<EVENT_QUEUE_MAX)
          {
          PrintEvent(IncommingEvent,0,VALUE_DIRECTION_QUEUE);  // geef event weer op Serial
          QueueEvent[QueuePos]=IncommingEvent;
          QueuePort[QueuePos]=Port;
          QueuePos++;           
          return true;
          }
        else
          GenerateEvent(CMD_ERROR,CMD_DELAY,EVENT_QUEUE_MAX);
        }
      return true;
      }
    }

  if(EventlistDepth==0 || S.Trace)PrintEvent(IncommingEvent,Port,Direction);  // geef event weer op Serial

  // Als de 'Confirm' optie aan staat, dan een 'Ok'-event verzenden
  // Dit alleen als het een commando of event voor deze nodo is.
  if(EventlistDepth==0 && S.Confirm && ((IncommingEvent>>28)&0xf==EVENT_TYPE_NODO))
    TransmitCode(command2event(CMD_OK,S.Unit,0));
    
  if(EventlistDepth++>=MACRO_EXECUTION_DEPTH)
    {
    GenerateEvent(CMD_ERROR,VALUE_NESTING,MACRO_EXECUTION_DEPTH);
    EventlistDepth=0;
    return false; // bij geneste loops ervoor zorgen dat er niet meer dan MACRO_EXECUTION_DEPTH niveaus diep macro's uitgevoerd worden
    }

  // ############# Verwerk event ################  
  // als het een commando of event is voor deze unit, dan t.b.v. interne verwerking unit op 0 zetten.
  if(!CommandError(IncommingEvent))
    { // Er is een Commando binnengekomen 
    if(!ExecuteCommand(IncommingEvent,Port,PreviousContent,PreviousPort))
      {
      EventlistDepth--;
      return false;
      }
    }
  else
    { // Er is een Event binnengekomen  
    // loop de gehele eventlist langs om te kijken of er een treffer is.    
    for(x=1; x<=Eventlist_MAX && Eventlist_Read(x,&Event_1,&Event_2); x++)
      {
      // kijk of deze regel een match heeft, zo ja, dan set y=vlag voor uitvoeren
      // de vlag y houdt bij of de wildcard een match heeft met zowel poort als type event. Als deze vlag staat, dan uitvoeren.
      
      // als de Eventlist regel een wildcard is, zo ja, dan set y=vlag voor uitvoeren
      if(((Event_1>>16)&0xff)==CMD_COMMAND_WILDCARD) // commando deel van het event.
        {        
        switch(Command) // Command deel van binnengekomen event.
          {
          case CMD_KAKU:
          case CMD_KAKU_NEW:
          case CMD_ERROR:
          case CMD_OK:
          case CMD_USER_EVENT:
            w=Command;
            break;
//          default:??? nog uitzoeken!
//            //??? was: w=EventType(IncommingEvent);
//            w=EventType(IncommingEvent);
          }

        y=true;// vlag wildcard match.         

        z=(Event_1>>8)&0xff; // Par1 deel van de Wildcard bevat de poort
        if(z!=VALUE_ALL && z!=Port)
          y=false;          

        z=Event_1&0xff; // Par2 deel Wildcard bevat type event
        if(z!=VALUE_ALL && z!=w)
          y=false;
        }
      else
        y=CheckEvent(IncommingEvent,Event_1);      
      
      if(y)
        {
        if(S.Trace)
          PrintEventlistEntry(x,EventlistDepth);
          
        if(!CommandError(Event_2)) // is de ontvangen code een geldig uitvoerbaar commando?
          {
          if(!ExecuteCommand(Event_2, VALUE_SOURCE_EVENTLIST,IncommingEvent,Port))
            {
            EventlistDepth--;
            return false;// false terug als het commando niet geldig of niet uitvoerbaar was
            }
          }
        else
          {// het is een ander soort event;
          if(Event_1!=command2event(CMD_COMMAND_WILDCARD,0,0))
            {
            if(!ProcessEvent(Event_2,VALUE_DIRECTION_INTERNAL,VALUE_SOURCE_EVENTLIST,IncommingEvent,Port))
              {
              EventlistDepth--;
              return true;
              }
            }
          }
        }
      }
    }
  EventlistDepth--;
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
  byte x;
  
  
  Serial.print("CheckEvent() > Event=0x");Serial.print(Event,HEX);Serial.print(", MacroEvent=0x");Serial.print(MacroEvent,HEX);PrintTerm();//???

  
  // als huidige event exact overeenkomt met het event in de regel uit de Eventlist, dan een match
  if(MacroEvent==Event)return true; 
    
  // Als unit ongelijk aan 0 of ongelijk aan huidige unit, dan is er ook geen match
  x=(Event>>24)&0x0f; // unit
  if(x!=0 && x!=S.Unit)return false; 

  // als huidige event (met wegfilterde unit) gelijk is aan MacroEvent, dan een match
  Event&=0xf0ffffff;
  MacroEvent&=0xf0ffffff;
  if(MacroEvent==Event)return true; 

  // beschouw bij een UserEvent een 0 voor Par1 of Par2 als een wildcard.
  if(((Event>>16)&0xff)==CMD_USER_EVENT)// Command
    {
    if(((Event>>8)&0xff)==0 || ((MacroEvent>>8)&0xff)==0){Event&=0xffff00ff;MacroEvent&=0xffff00ff;}
    if(((Event   )&0xff)==0 || ((MacroEvent   )&0xff)==0){Event&=0xffffff00;MacroEvent&=0xffffff00;}
    if(MacroEvent==Event)return true; 
    }

  // is er een match met een CLOCK_EVENT_ALL event?
  x=(Event>>16)&0xff;
  if(x>=CMD_CLOCK_EVENT_SUN && x<=CMD_CLOCK_EVENT_SAT) // het binnengekomen event is een clock event.
    {
    if(((MacroEvent>>16)&0xff)==CMD_CLOCK_EVENT_ALL) // als het event uit de eventlist is een ClockAll event.
      if((MacroEvent&0x0000ffff)==(Event&0x0000ffff)) // en tijdstippen kloppen
        return true;
    }
  
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

/*********************************************************************************************\
 * Handel een foutmelding af
 \*********************************************************************************************/
void GenerateEvent(byte Cmd, byte P1, byte P2)
  {
  unsigned long Event;
  Event=command2event(Cmd,P1,P2);
  TransmitCode(Event);
  }
  

