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
 
boolean ExecuteEventlist(unsigned long Event, byte Port, byte Type, unsigned long PreviousContent, byte PreviousPort, byte PreviousType)
  {
  unsigned long MacroEvent, MacroAction;
  byte x,y,z;

  if(depth++>=MACRO_EXECUTION_DEPTH)
    {
    Serial.print(Text(Text_50));
    PrintTerm();
    return false; // bij geneste loops ervoor zorgen dat er niet meer dan MACRO_EXECUTION_DEPTH niveaus diep macro's uitgevoerd worden
    }
  
  for(x=1; x<=Eventlist_MAX && Eventlist_Read(x,&MacroEvent,&MacroAction); x++)
    {
    Type=EventType(Event);

    // kijk of deze regel een match heeft, zo ja, dan uitvoeren
    y=false;
    if(CheckEvent(Event,MacroEvent))y=true;

    // als de Eventlist regel een wildcard is
    if(((MacroEvent>>16)&0xff)==CMD_WILDCARD_EVENT)
      {
      y=true; 
      z=(MacroEvent>>8)&0xff;
      // Debug: Serial.print("z=");Serial.print(cmd2str(z));Serial.print(", Port=");Serial.print(cmd2str(E.Port));PrintTerm();
      if(z>0 && z!=Port)y=false;
      z=MacroEvent&0xff;
      // Debug: Serial.print("z=");Serial.print(cmd2str(z));Serial.print(", Type=");Serial.print(cmd2str(E.Type));PrintTerm();
      if(z>0 && z!=Type)y=false;
      // Debug: Serial.print("y=");Serial.print(y,DEC);PrintTerm();
      }
    
    if(y)
      {
      y=IsCommand(MacroAction);

      if(Trace)PrintEventlistEntry(x,depth);
      
      if(y>0 && y<RANGE_EVENT) // is de ontvangen code een uitvoerbaar commando?
        {
        if(!ExecuteCommand(MacroAction,CMD_SOURCE_MACRO,EventType(MacroAction),Event,Port,Type))
          {
          depth--;
          return false;
          }
        }
      else
        {// het is een ander soort event;
        if(MacroEvent!=command2event(CMD_WILDCARD_EVENT,0,0))
          {
          if(Trace)
            {
            PrintEvent(DIRECTION_IN,CMD_SOURCE_MACRO,EventType(MacroAction),MacroAction);
            PrintTerm();
            }

          if(!ExecuteEventlist(MacroAction,CMD_SOURCE_MACRO,EventType(MacroAction),Event,Port,Type))
            {
            depth--;
            return false;
            }
          }
        }
      }
    }
  depth--;
  return true;
  }

 /**********************************************************************************************\
 * Maak de Eventlist leeg.
 \*********************************************************************************************/
void FactoryEventlist(void)
  {
  unsigned long Event;
  
  Eventlist_Write(1,0L,0L); // maak de eventlist leeg.
  Event=command2event(CMD_BOOT_EVENT,0,0);
  Eventlist_Write(0,Event,command2event(CMD_SOUND,7,0)); // Kort geluidssignaal bij ieder binnenkomend event
  Event=command2event(CMD_WILDCARD_EVENT,0,0);
  Eventlist_Write(0,Event,command2event(CMD_SOUND,0,0)); // Kort geluidssignaal bij ieder binnenkomend event
  Eventlist_Write(0,Event,command2event(CMD_SEND_RF,5,0)); // Alle KAKU events doorsturen naar RF
  Eventlist_Write(0,Event,command2event(CMD_SEND_IR,5,0)); // Alle events doorsturen naar IR
  }

 /**********************************************************************************************\
 * Toetst of de Code voorkomt in de Eventlist. Geeft False als de opgegeven code niet bestaat.
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
  if(MacroEvent==Event)return true; 
  
  // kijk of het een matching CLOCK_EVENT is:
  if(((MacroEvent>>16)&0xff)==CMD_CLOCK_EVENT_ALL)// in de Eventlist betrof het een CLOCK_EVENT_ALL wildcard.
    {
    if((MacroEvent&0xff00ffff)==(Event&0xff00ffff))// home, unit en tijd komen overeen
      {
      if(((Event>>16)&0xff)>=CMD_CLOCK_EVENT_MON && ((Event>>16)&0xff)<=CMD_CLOCK_EVENT_SUN)//en het te verwerken event was een CLOCK_EVENT
        return true;
      }
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

 
    
    
