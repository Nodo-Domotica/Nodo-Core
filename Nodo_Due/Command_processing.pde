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



/*********************************************************************************************\
 * Deze functie checked of de code die ontvangen is een uitvoerbare opdracht is/
 * Als het een correct commando is wordt deze uitgevoerd en 
 * true teruggegeven. Zo niet dan wordt er een 'false' retour gegeven.
 * 
 \*********************************************************************************************/
boolean ExecuteCommand(unsigned long Content, int Src, unsigned long PreviousContent, int PreviousSrc)
  {
  unsigned long Event, Action;  
  int x,y,error=false;
  
  int Command      = EventPart(Content,EVENT_PART_COMMAND);
  int Par1         = EventPart(Content,EVENT_PART_PAR1);
  int Par2         = EventPart(Content,EVENT_PART_PAR2);
  int Type         = EventType(Content);
  int PreviousType = EventType(PreviousContent);
  
  if(Command) // is de ontvangen code een uitvoerbaar commando?
    {
    if(S.Trace&1)
      PrintEvent(Content,Src,DIRECTION_EXECUTE);
      
    switch(Command)
      {   
      case CMD_SEND_KAKU:
        KAKU_2_RawSignal(Content);
        PrintEvent(command2event(CMD_KAKU,Par1,Par2),CMD_PORT_RF,DIRECTION_OUT);
        RawSendRF();      
        break;
        
      case CMD_SEND_KAKU_NEW:
        NewKAKU_2_RawSignal(Content);
        PrintEvent(command2event(CMD_KAKU_NEW,Par1,Par2),CMD_PORT_RF, DIRECTION_OUT);
        RawSendRF();      
        break;
        
      case CMD_VARIABLE_INC: 
        if(Par1<=USER_VARIABLES_MAX && (S.UserVar[Par1-1]+Par2)<=255) // alleen ophogen als variabele nog niet de maximale waarde heeft
          {      
          S.UserVar[Par1-1]+=Par2;
          SaveSettings();
          }
        break;        
  
      case CMD_VARIABLE_DEC: 
        if(Par1<=USER_VARIABLES_MAX && (S.UserVar[Par1-1]-Par2)>=0) // alleen decrement als variabele hierdoor niet negatief wordt
          {
          S.UserVar[Par1-1]-=Par2;
          SaveSettings();
          }
        break;        
  
      case CMD_VARIABLE_SET:   
        if(Par1>0 && Par1<=USER_VARIABLES_MAX) // alleen een geldige variabele setten.
         {
         S.UserVar[Par1-1]=Par2;
         SaveSettings();
         }
        break;        
  
      case CMD_VARIABLE_DAYLIGHT: // alleen een geldige variabele setten.
        if(Par1>0 && Par1<=USER_VARIABLES_MAX)
          {
          S.UserVar[Par1-1]=Time.Daylight;
          SaveSettings();
          }
        break;        
  
      case CMD_VARIABLE_VARIABLE: // alleen een geldige variabele setten.
        if(Par1>=1 && Par1<=USER_VARIABLES_MAX && Par2>=1 && Par2<=USER_VARIABLES_MAX) 
          {
          S.UserVar[Par1-1]=S.UserVar[Par2-1];
          SaveSettings();
          }
        break;        
  
      case CMD_VARIABLE_WIRED_ANALOG: // alleen een geldige variabele setten met geldige WIRED-IN poort
        if(Par1>=1 && Par1<=USER_VARIABLES_MAX && Par2>=1 && Par2<=4) 
          {
          S.UserVar[Par1-1]=analogRead(WiredAnalogInputPin_1+(Par2-1))>>2;
          SaveSettings();
          }
        break;        
  
      case CMD_BREAK_ON_VAR_EQU: // alleen een geldige variabele vergelijken
        if(Par1>=1 && Par1<=USER_VARIABLES_MAX) 
          {
          if(S.UserVar[Par1-1]==Par2)
            {
            if(S.Trace&1)
              {
              Serial.print((Text_09));PrintTerm();
              }
            return false;
            }
          }
        break;        
  
      case CMD_BREAK_ON_VAR_NEQU: // alleen een geldige variabele
        if(Par1>=1 && Par1<=USER_VARIABLES_MAX) 
          {
          if(S.UserVar[Par1-1]!=Par2)
            {
            if(S.Trace&1)
              {
              Serial.print(Text(Text_09));PrintTerm();
              }
            return false;
            }
          }
        break;        
  
      case CMD_BREAK_ON_VAR_MORE:// alleen een geldige variabele
        if(Par1>=1 && Par1<=USER_VARIABLES_MAX) 
          {
          if(S.UserVar[Par1-1]>Par2)
            {
            if(S.Trace&1)
              {
              Serial.print(Text(Text_09));PrintTerm();
              }
            return false;
            }
          }
        break;        
  
      case CMD_BREAK_ON_VAR_LESS:// alleen een geldige variabele
        if(Par1>=1 && Par1<=USER_VARIABLES_MAX) 
          {
          if(S.UserVar[Par1-1]<Par2)
            {
            if(S.Trace&1)
              {
              Serial.print(Text(Text_09));PrintTerm();
              }
            return false;
            }
          }
        break;  
  
      case CMD_TIMER_RESET:// Deze heeft betrekking op alle timers: cancellen.
        if(Par1==0)
           {
           for(x=0;x<USER_TIMER_MAX;x++)
              UserTimer[x]=0L;
           }
        else
          {
          if(Par1>0 && Par1<=USER_TIMER_MAX)// voor de gebruiker beginnen de timers bij één.
            UserTimer[Par1-1]=0L;
          }
        break;
  
    case CMD_SEND_USEREVENT:
      // Voeg Home toe en Maak Unit=0 want een UserEvent is ALTIJD voor ALLE Nodo's. Verzend deze vervolgens.
      Event=command2event(CMD_USER_EVENT,Par1,Par2)&0x00ffffff | ((unsigned long)S.Home)<<28; // Voeg Home toe en Maak Unit=0 want een UserEvent is ALTIJD voor ALLE Nodo's.;
      Nodo_2_RawSignal(Event);
      if(PreviousSrc!=CMD_PORT_IR && (S.DivertPort==DIVERT_PORT_IR || S.DivertPort==DIVERT_PORT_IR_RF))
        {
        PrintEvent(Event,CMD_PORT_IR,DIRECTION_OUT);
        RawSendIR();
        } 
      if(PreviousSrc!=CMD_PORT_RF && (S.DivertPort==DIVERT_PORT_RF || S.DivertPort==DIVERT_PORT_IR_RF))
        {
        PrintEvent(Event,CMD_PORT_RF,DIRECTION_OUT);
        RawSendRF();
        }
      break;      

    case CMD_SIMULATE_DAY:
       if(Par1==0)Par1=1;
       if(Par1!=1)Par1=7;
       SimulateDay(Par1); 
       break;     

    case CMD_SEND_RAW:
      if(PreviousSrc!=CMD_PORT_IR && (Par1==1 || Par1==0))
        {
        PrintEvent(0,CMD_PORT_IR,DIRECTION_OUT);
        RawSendIR();
        }
  
      if(PreviousSrc!=CMD_PORT_RF && (Par1==2 || Par1==0))
        {
        PrintEvent(0,CMD_PORT_RF,DIRECTION_OUT);
        RawSendRF();      
        }
      PrintRawSignal();PrintTerm();
      break;        
  
    case CMD_CLOCK_YEAR:
       x=Par1*100+Par2;
       if(x>=2000 && x<=2090)// nieuwe millenium bug, maar dan ben ik al 130 jaar ;-)
         {
         Time.Year=x;
         ClockSet();
         break;
         }
  
    case CMD_CLOCK_DLS: 
       if(S.DaylightSaving!=Par1)// alleen verzetten als de setting veranderd, anders wordt de klok twee uur verzet!
          {
          S.DaylightSaving=Par1;
          if(S.DaylightSaving==1)
            Time.Hour=Time.Hour<23?Time.Hour+1:0;
          else
            Time.Hour=Time.Hour==0?23:Time.Hour-1;
          ClockSet();
          SaveSettings();
          }
       break;        
    
    case CMD_CLOCK_TIME:
       Time.Hour=Par1;
       Time.Minutes=Par2;
       Time.Seconds=0;
       ClockSet();
       break;
  
    case CMD_CLOCK_DATE: // data1=datum, data2=maand
       Time.Date=Par1;
       Time.Month=Par2;
       ClockSet();
       break;
  
    case CMD_CLOCK_DOW:
       Time.Day=Par1;
       ClockSet();
       break;
         
    case CMD_TIMER_SET:
      if(Par1>0 && Par1<=USER_TIMER_MAX)// alleen geldige timer. Voor de gebruiker beginnen de timers bij één.
        UserTimer[Par1-1]=millis()+(unsigned long)(Par2)*60000L;// Par1=timer, Par2=minuten
      break;
  
    case CMD_TIMER_RANDOM:
      if(Par1>0 && Par1<=USER_TIMER_MAX)// alleen een geldige timer
        UserTimer[Par1-1]=millis()+(unsigned long)random(Par2)*60000L;// Par1=timer, Par2=maximaal aantal minuten
      break;
  
    case CMD_DELAY:
       delay(Par1*1000);     
       break;        
        
    case CMD_SOUND: 
       Alarm(Par1,Par2);
       break;     
    
    case CMD_WIRED_PULLUP:
       if(Par1>0 && Par1<5)
          {
          S.WiredInputPullUp[Par1-1]=Par2; // Par1 is de poort[1..4], Par2 is de waarde [0..1]
          digitalWrite(14+WiredAnalogInputPin_1+Par1-1,S.WiredInputPullUp[Par1-1]?HIGH:LOW);// Zet de pull-up weerstand van 20K voor analoge ingangen. Analog-0 is gekoppeld aan Digital-14
          SaveSettings();
          }
       break;
             
    case CMD_VARIABLE_CLEAR:
       error=VariableClear(Par1);
       break;
       
    case CMD_WIRED_THRESHOLD:
       if(Par1>=1 && Par1<=4)
         {
         S.WiredInputThreshold[Par1-1]=Par2; // Par1 is de poort[1..4], Par2 is de waarde [0..255]
         SaveSettings();
         }
       break;
  
    case CMD_WIRED_OUT:
       if(Par1>=1 && Par1<=4)
          {
          digitalWrite(WiredDigitalOutputPin_1+Par1-1,Par2&1);
          WiredOutputStatus[Par1-1]=Par2&1;
          }
       break;
                    
    case CMD_WIRED_SMITTTRIGGER:
       if(Par1>0 && Par1<5)
         {
         S.WiredInputSmittTrigger[Par1-1]=Par2; // Par1 is de poort[1..4], Par2 is de waarde [0..255]
         SaveSettings();
         }
       break;
       
    case CMD_WAITFREERF: 
      {
      S.WaitFreeRFAction=Par1;
      S.WaitFreeRFWindow=Par2==0?S.Unit*500:Par2*100;
      SaveSettings();
      break;
      }

    case CMD_DIVERT:   
      DivertUnit=Par1&0xf;
      break;

    case CMD_UNIT:
      S.Unit=Par1&0xf; // Par1 &0xf omdat alleen waarden van 0..15 geldig zijn.
      DivertUnit=S.Unit;   // Alle commando's en events zijn voor de Nodo zelf.
      SaveSettings();
      break;    

    case CMD_RESET:
       VariableClear(0);// wis alle variabelen
       delay(500);// kleine pauze, anders kans fout bij seriële communicatie
       Reset();
       break;        

    case CMD_COPYSIGNAL:
       if(Par1)
         CopySignalRF2IR(Par2);      
       else
         CopySignalIR2RF(Par2);
       break;        

    case CMD_SEND_STATUS:
      x=Par1;// bevat het commando waarvoor de status opgehaald moet worden
      Par1=Par2;
      if(GetStatus(&x,&Par1,&Par2))// let op: call by reference. Gegevens komen terug in Par1 en Par2
        {
        Event=command2event(x,Par1,Par2);
        Nodo_2_RawSignal(Event);
        if(S.DivertPort==DIVERT_PORT_IR || S.DivertPort==DIVERT_PORT_IR_RF)
          {
          PrintEvent(Event,CMD_PORT_IR,DIRECTION_OUT);
          RawSendIR();
          } 
        if(S.DivertPort==DIVERT_PORT_RF || S.DivertPort==DIVERT_PORT_IR_RF)
          {
          PrintEvent(Event,CMD_PORT_RF,DIRECTION_OUT);
          RawSendRF();
          }
        }
      break;
        
        
      default:
        error=true;
      }
    }   
  else
    {
    error=true;
    }
  
  if(error)
    {
    Serial.print(Text(Text_06));
    PrintEventCode(Content);PrintTerm();
    return false;
    }
  return true;
  }


