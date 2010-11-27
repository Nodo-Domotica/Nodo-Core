/**************************************************************************\

    This file is part of Nodo Due, © Copyright Paul Tonkes.

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


/*********************************************************************************************\
* 
\*********************************************************************************************/
#define SERIAL_BUFFER_SIZE        25  // kleine String waar de karakters van de seriele poort in worden opgeslagen. Max. grootte voor HEX getal. 
char SerialBuffer[SERIAL_BUFFER_SIZE];

unsigned long Receive_Serial(void)
  {
  unsigned long Event,Action;
  byte x,y;
  int Par1,Par2,Cmd;
  boolean error=false;
  
  Event=SerialReadEvent();
  
  if(EventType(Event)==VALUE_TYPE_UNKNOWN)// het is zeker geen commando, maar een ander type event
      return Event;
  else
    { // het was een geldig uitvoerbaar commando. Dit wordt geborgd door SerialReadEvent();
    Cmd=EventPart(Event,EVENT_PART_COMMAND);
    Par1=EventPart(Event,EVENT_PART_PAR1);
    Par2=EventPart(Event,EVENT_PART_PAR2);
      
    switch(Cmd)
      {
      case CMD_EVENTLIST_SHOW:
        PrintLine();
        for(x=1;x<=Eventlist_MAX && Eventlist_Read(x,&Event,&Action);x++)
          {
          PrintEventlistEntry(x,0);PrintTerm();
          }   
        PrintLine();
        break;
     
      case CMD_EVENTLIST_WRITE:
        // haal event encommando op
        Event=SerialReadEvent();
        Action=SerialReadEvent();

        if(Event==0 || (EventType(Event)==VALUE_TYPE_COMMAND && (error=CommandError(Event ))))
          {
          GenerateEvent(CMD_ERROR,CMD_EVENTLIST_WRITE,1);
          break;
          }
        if(Action==0 || (EventType(Action)==VALUE_TYPE_COMMAND && (error=CommandError(Action ))))
          {
          GenerateEvent(CMD_ERROR,CMD_EVENTLIST_WRITE,2);
          break;
          }
          
        x=EventType(Action);
        if(x==VALUE_TYPE_COMMAND || x==VALUE_TYPE_EVENT)Event&=0xf0ffffff; // als het een commando of een event is, dan unit er uitfilteren alvorens weg te schrijven
        x=EventType(Action);
        if(x==VALUE_TYPE_COMMAND || x==VALUE_TYPE_EVENT)Action&=0xf0ffffff;
        
        // schrijf weg in eventlist
        if(!Eventlist_Write(0,Event,Action)) // Unit er uit filteren, anders na wijzigen unit geen geldige eventlist.
          {
          error=true;
          Par1=CMD_EVENTLIST_WRITE;
          Par2=VALUE_SOURCE_EVENTLIST;
          }
        break;        
  
      case CMD_ANALYSE_SETTINGS:
        S.AnalyseTimeOut=Par1;
        S.AnalyseSharpness=Par2*1000;
        SaveSettings();
        break;
  
      case CMD_TRACE:
        S.Trace=Par1&1 | (Par2&1)<<1;
        SaveSettings();
        break;        
    
      case CMD_DIVERT:   
        Action=(SerialReadEvent()&0xf0ffffff) | ((unsigned long)(Par1))<<24; // Event_1 is het te forwarden event voorzien van nieuwe bestemming unit
        SendEventCode(Action);
        break;        
  
      case CMD_RAWSIGNAL_GET:
        RawsignalGet=true;
        break;        
  
       case CMD_SIMULATE:
         Simulate=Par1;
         break;        
   
       case CMD_UNIT:
         S.Unit=Par1;
         if(Par2>0)S.Home=Par2;
         SaveSettings();
         FactoryEventlist();
         Reset();
         break;    
    
       case CMD_RESET_FACTORY:
          ResetFactory();
          
       case CMD_RAWSIGNAL_PUT:
          y=1;
          do
            {
            x=SerialReadBlock(SerialBuffer);
            // PrintTerm();//??? wat doet deze hier? kan weg?
            RawSignal[y++]=str2val(SerialBuffer);
            }while(x && y<RAW_BUFFER_SIZE);
          RawSignal[0]=y-1;
          Event=AnalyzeRawSignal();
          SendEventCode(Event);
          break;

        case CMD_STATUS:
           Cmd=Par1;
           Par1=Par2;
           if(GetStatus(&Cmd,&Par1,&Par2)) // let op: call by reference !
             {
             Event=command2event(Cmd,Par1,Par2);// event wordt samengesteld...
             PrintEventCode(Event);// ...maar alleen weergegeven.
             PrintTerm();
             }
           else
             {
             error=true;
             Par1=CMD_STATUS;
             Par2=VALUE_PARAMETER;
             }
           break;
    
        case CMD_EVENTLIST_ERASE:
           VariableClear(0); // alle variabelen op nul zetten
           TimerClear(0); // reset de timers
           Eventlist_Write(1,0L,0L); // maak de eventlist leeg.
           break;        
            
        default:// alle andere commando's hebben max. twee parameters. 
          return Event;       
        }
      }
   return 0L;
   }


/*********************************************************************************************\
* 
\*********************************************************************************************/
unsigned long SerialReadEvent()
  {
  unsigned long Event;
  byte x,y,z;
  int Par1,Par2;

  Par1=0;
  Par2=0;
  
  // haal 1e blok gegevens op.
  x=SerialReadBlock(SerialBuffer);

  // is het gewoon een getal dat als event moet worden verwerkt? Deze zijn altijd groter dan 255 want 0..255 zijn commando's
  Event=str2val(SerialBuffer);
  if(Event>0xff)return Event; // alle waarden groter dan 255 mogen gelijk verwerkt worden als een event.

  if(Event==0)
    y=str2cmd(SerialBuffer);  // invoer was geen getal. Haal uit de invoer de code van het tekst commando
  else
    y=Event; 

  if(y==CMD_KAKU || y==CMD_SEND_KAKU)
    {// de string is een KAKU commando. Haal bij het commando behorende parameters op.
    if(x)
      {
      x=SerialReadBlock(SerialBuffer);
      z=0;
      Par1=HA2address(SerialBuffer,&z); // Parameter-1 bevat [A1..P16]. Omzetten naar absolute waarde.
      if(x)
        {
        x=SerialReadBlock(SerialBuffer);
        Par2=str2val(SerialBuffer) | (z<<1); // Parameter-2 bevat [On,Off]. Omzetten naar 1,0. tevens op bit-2 het groepcommando zetten.
        }
      }
    Event=command2event(y,Par1,Par2);
    return Event;
    }

  if(y==CMD_KAKU_NEW || y==CMD_SEND_KAKU_NEW)
    {// de string is een KAKU commando. Haal bij het commando behorende parameters op.
    if(x)
      {
      x=SerialReadBlock(SerialBuffer);
      z=0;
      Par1=HA2address(SerialBuffer,&z); // Parameter-1 bevat adres als tekst. // omzetten met andere functie
      if(x)
        {
        x=SerialReadBlock(SerialBuffer);
        Par2=DL2par(SerialBuffer); // Parameter-2 bevat [Dim1..Dim16]. Omzetten naar code.
        if(!Par2)Par2=str2val(SerialBuffer);
        }
      }
    Event=command2event(y,Par1,Par2);
    return Event;
    }
  
  if(y>RANGE_VALUE && y<=COMMAND_MAX)
    {// de string is een bestaand commando of event. Haal bij het commando behorende parameters op.
    if(x)
      {
      x=SerialReadBlock(SerialBuffer);
      Par1=str2val(SerialBuffer);
      if(x)
        {
        x=SerialReadBlock(SerialBuffer);
        Par2=str2val(SerialBuffer);
        }
      }
    Event=command2event(y,Par1,Par2);
    x=CommandError(Event);
    if(x)
      { // het was geen geldig uitvoerbaar commando  
      if(y==0)
        GenerateEvent(CMD_ERROR,VALUE_TYPE_UNKNOWN,0);
      else
        GenerateEvent(CMD_ERROR,y,x);
      return 0L;
      }
    return Event;
    }

  if(strlen(SerialBuffer)!=0)
    PrintText(Text_06,true);
  return 0L;
  }
   
/**********************************************************************************************\
 * Haalt uit een seriële reeks teken met formaat 'aaaa,bbbb,cccc,dddd;' de blokken tekst 
 * die gescheiden worden door komma's of spaties. 
 * geeft een 1 terug als blok afgesloten met een komma of spatie (en er nog meerdere volgen)
 * geeft een 0 terug als hele reeks is afgesloten met een '\n' of een ';'
 \*********************************************************************************************/
#define SERIAL_TIMEOUT 5000
byte SerialReadBlock(char *SerialString)
  {
  byte StringLength,SerialByte,x;
  unsigned long TimeOutTimer;
  
  // op moment hier aangekomen staat er minimaal een byte klaar in de seriele buffer  
  SerialHold(false);
  x=0;
  StringLength=0;
  TimeOutTimer=millis()+SERIAL_TIMEOUT;

  do
    {// lees de seriële poort totdat afsluitteken of een teken voor volgende parameter
    if(Serial.available()>0)
      {
      SerialByte=Serial.read();
      if(StringLength==0 && SerialByte==32)continue; // voorloop spaties overslaan
      if(SerialByte=='(' || SerialByte==')')continue; // deze tekens zijn comment.
      if(SerialByte==';' || SerialByte==',' || SerialByte=='\n' || SerialByte==32)// tekens die parameters of commando's van elkaar onderscheiden
        x=SerialByte;
      else
        SerialString[StringLength++]=tolower(SerialByte); // voeg karakter toe aan de SerialString (lower-case)
      }
    }while(StringLength<SERIAL_BUFFER_SIZE && x==0 && millis()<TimeOutTimer);
  SerialString[StringLength]=0; // sluit SerialString af met een \0

  SerialHold(true);
  if(x==',' || x==32)return true;// er komen nog meer blokken
  return false;// afgesloten met '\n' of een ';', dus commando is compleet
  }
  
#define XON                       0x11
#define XOFF                      0x13

void SerialHold(boolean x)
  {
  static boolean previous=true;
  
  if(x==previous)
    return;
  else
    {
    if(x)
      Serial.print(XOFF,BYTE);
    else
      Serial.print(XON,BYTE);
    previous=x;
    }
  }
