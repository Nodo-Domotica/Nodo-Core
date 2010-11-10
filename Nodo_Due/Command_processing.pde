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
  int x,y;
  byte error=false;
  
  int Command      = EventPart(Content,EVENT_PART_COMMAND);
  int Par1         = EventPart(Content,EVENT_PART_PAR1);
  int Par2         = EventPart(Content,EVENT_PART_PAR2);
  int Type         = EventType(Content);
  int PreviousType = EventType(PreviousContent);
  
  if(Command) // is de ontvangen code een uitvoerbaar commando?
    {
//    if(S.Trace&1) ???
      PrintEvent(Content,0,VALUE_DIRECTION_EXECUTE);
      
    switch(Command)
      {   
      case CMD_SEND_KAKU:
        if(S.WaitFreeRFAction==VALUE_ALL)WaitFreeRF(S.WaitFreeRFWindow);
        KAKU_2_RawSignal(Content);
        PrintEvent(command2event(CMD_KAKU,Par1,Par2), VALUE_SOURCE_RF,VALUE_DIRECTION_OUTPUT);
        RawSendRF();      
        break;
        
      case CMD_SEND_KAKU_NEW:
        if(S.WaitFreeRFAction==VALUE_ALL)WaitFreeRF(S.WaitFreeRFWindow);
        NewKAKU_2_RawSignal(Content);
        PrintEvent(command2event(CMD_KAKU_NEW,Par1,Par2), VALUE_SOURCE_RF, VALUE_DIRECTION_OUTPUT);
        RawSendRF();      
        break;
        
      case CMD_VARIABLE_INC: 
        if(Par1<=USER_VARIABLES_MAX)
           {
           if((S.UserVar[Par1-1]+Par2)<=255) // alleen ophogen als variabele nog niet de maximale waarde heeft.
               {
              if(!Par2)Par2=1;      
              S.UserVar[Par1-1]+=Par2;
              SaveSettings();
               }
            }
        else
          error=VALUE_PARAMETER;
        break;        
  
      case CMD_VARIABLE_DEC: 
        if(Par1<=USER_VARIABLES_MAX)
           {
           if((S.UserVar[Par1-1]-Par2)>=0) // alleen decrement als variabele hierdoor niet negatief wordt
              {
              if(!Par2)Par2=1;      
              S.UserVar[Par1-1]-=Par2;
              SaveSettings();
              }
           }
        else
          error=VALUE_PARAMETER;
        break;        
  
      case CMD_VARIABLE_SET:   
        if(Par1>0 && Par1<=USER_VARIABLES_MAX) // alleen een geldige variabele setten.
         {
         S.UserVar[Par1-1]=Par2;
         SaveSettings();
         }
        else
          error=VALUE_PARAMETER;
        break;        
    
      case CMD_VARIABLE_VARIABLE: // alleen een geldige variabele setten.
        if(Par1>=1 && Par1<=USER_VARIABLES_MAX && Par2>=1 && Par2<=USER_VARIABLES_MAX) 
          {
          S.UserVar[Par1-1]=S.UserVar[Par2-1];
          SaveSettings();
          }
        else
          error=VALUE_PARAMETER;
        break;        
  
      case CMD_VARIABLE_WIRED_ANALOG: // alleen een geldige variabele setten met geldige WIRED-IN poort
        if(Par1>=1 && Par1<=USER_VARIABLES_MAX && Par2>=1 && Par2<=4) 
          {
          S.UserVar[Par1-1]=analogRead(WiredAnalogInputPin_1+(Par2-1))>>2;
          SaveSettings();
          }
        else
          error=VALUE_PARAMETER;
        break;        
  
      case CMD_BREAK_ON_VAR_EQU: // alleen een geldige variabele vergelijken
        if(Par1>=1 && Par1<=USER_VARIABLES_MAX) 
          {
          if(S.UserVar[Par1-1]==Par2)
            {
            if(S.Trace&1)
              PrintText(Text_09,true);
            return false;
            }
          }
        else
          error=VALUE_PARAMETER;
        break;        
  
      case CMD_BREAK_ON_VAR_NEQU: // alleen een geldige variabele
        if(Par1>=1 && Par1<=USER_VARIABLES_MAX) 
          {
          if(S.UserVar[Par1-1]!=Par2)
            {
            if(S.Trace&1)
              PrintText(Text_09,true);
            return false;
            }
          }
        else
          error=VALUE_PARAMETER;
        break;        
  
      case CMD_BREAK_ON_VAR_MORE:// alleen een geldige variabele
        if(Par1>=1 && Par1<=USER_VARIABLES_MAX) 
          {
          if(S.UserVar[Par1-1]>Par2)
            {
            if(S.Trace&1)
              PrintText(Text_09,true);
            return false;
            }
          }
        else
          error=VALUE_PARAMETER;
        break;        
  
      case CMD_BREAK_ON_VAR_LESS:// alleen een geldige variabele
        if(Par1>=1 && Par1<=USER_VARIABLES_MAX) 
          {
          if(S.UserVar[Par1-1]<Par2)
            {
            if(S.Trace&1)
              PrintText(Text_09,true);
            return false;
            }
          }
        else
          error=VALUE_PARAMETER;
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
          else
            error=VALUE_PARAMETER;
          }
        break;
  
    case CMD_SEND_USEREVENT:
      // Voeg Home toe en Maak Unit=0 want een UserEvent is ALTIJD voor ALLE Nodo's. Verzend deze vervolgens.
      SendEvent(command2event(CMD_USER_EVENT,Par1,Par2)&0x00ffffff | ((unsigned long)S.Home)<<28);// Voeg Home toe en Maak Unit=0 want een UserEvent is ALTIJD voor ALLE Nodo's.;
      break;

    case CMD_SIMULATE_DAY:
       if(Par1==0)Par1=1;
       if(Par1!=1)Par1=7;
       SimulateDay(Par1); 
       break;     

    case CMD_SEND_RAW:
      // verzend de de inhoud van de RAW buffer ??? DivertSettings als settings?
      if(PreviousSrc!= VALUE_SOURCE_IR && (Par1== VALUE_SOURCE_IR || Par1== VALUE_SOURCE_IR_RF))
        {
        PrintEvent(0, VALUE_SOURCE_IR,VALUE_DIRECTION_OUTPUT);
        RawSendIR();
        }
      if(PreviousSrc!= VALUE_SOURCE_RF && (Par1== VALUE_SOURCE_RF || Par1== VALUE_SOURCE_IR_RF))
        {
        PrintEvent(0, VALUE_SOURCE_RF,VALUE_DIRECTION_OUTPUT);
        RawSendRF();      
        }
      PrintRawSignal();PrintTerm();
      break;        
  
    case CMD_CLOCK_YEAR:
       x=Par1*100+Par2;
       if(x>=2000 && x<=2090)// nieuwe millenium bug, maar dan ben ik 130 jaar ;-)
         {
         Time.Year=x;
         ClockSet();
         break;
         }
       else
         error=VALUE_PARAMETER;
      
    case CMD_CLOCK_TIME:
       if(Par1<=23 && Par2<=59)
         {
         Time.Hour=Par1;
         Time.Minutes=Par2;
         Time.Seconds=0;
         ClockSet();
         }
       else
         error=VALUE_PARAMETER;
       break;
  
    case CMD_CLOCK_DATE: // data1=datum, data2=maand
      if(Par1<=31 && Par2<=12)
        {      
        Time.Date=Par1;
        Time.Month=Par2;
        ClockSet();
        }
      else
        error=VALUE_PARAMETER;
      break;
  
    case CMD_CLOCK_DOW:
      if(Par1>0 && Par1<8)
        {
        Time.Day=Par1;
        ClockSet();
        }
      else
        error=VALUE_PARAMETER;
      break;
         
    case CMD_TIMER_SET:
      if(Par1>0 && Par1<=USER_TIMER_MAX)// alleen geldige timer. Voor de gebruiker beginnen de timers bij één.
        UserTimer[Par1-1]=millis()+(unsigned long)(Par2)*60000L;// Par1=timer, Par2=minuten
      else
        error=VALUE_PARAMETER;
      break;
  
    case CMD_TIMER_RANDOM:
      if(Par1>0 && Par1<=USER_TIMER_MAX)// alleen een geldige timer
        UserTimer[Par1-1]=millis()+(unsigned long)random(Par2)*60000L;// Par1=timer, Par2=maximaal aantal minuten
      else
        error=VALUE_PARAMETER;
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
       else
         error=VALUE_PARAMETER;
       break;
             
    case CMD_VARIABLE_CLEAR:
       if(Par1>0 && Par1<=15)
         VariableClear(Par1);
       else
         error=VALUE_PARAMETER;
       break;
       
    case CMD_WIRED_THRESHOLD:
       if(Par1>=1 && Par1<=4)
         {
         S.WiredInputThreshold[Par1-1]=Par2; // Par1 is de poort[1..4], Par2 is de waarde [0..255]
         SaveSettings();
         }
       else
         error=VALUE_PARAMETER;
       break;
  
    case CMD_WIRED_OUT:
       if(Par1>=1 && Par1<=4)
          {
          digitalWrite(WiredDigitalOutputPin_1+Par1-1,Par2&1);
          WiredOutputStatus[Par1-1]=Par2&1;
          }
       else
         error=VALUE_PARAMETER;
       break;
                    
    case CMD_WIRED_SMITTTRIGGER:
       if(Par1>0 && Par1<5)
         {
         S.WiredInputSmittTrigger[Par1-1]=Par2; // Par1 is de poort[1..4], Par2 is de waarde [0..255]
         SaveSettings();
         }
       else
         error=VALUE_PARAMETER;
       break;
       
    case CMD_WAITFREERF: 
      if(Par1==VALUE_OFF || Par1==VALUE_SERIES || Par1==VALUE_ALL)
        {
        S.WaitFreeRFAction=Par1;
        S.WaitFreeRFWindow=Par2==0?S.Unit*500:Par2*100;
        SaveSettings();
        }
      else
         error=VALUE_PARAMETER;  
      break;


    case CMD_COPYSIGNAL:
      if(Par1==VALUE_RF_2_IR || Par1==VALUE_IR_2_RF)
        {          
        if(Par1==VALUE_RF_2_IR)
          CopySignalRF2IR(Par2);      
        if(Par1==VALUE_IR_2_RF)
          CopySignalIR2RF(Par2);
        }
      else
        error=VALUE_PARAMETER;  
      break;        

    case CMD_TRANSMIT_SETTINGS:
      if(Par1== VALUE_SOURCE_IR || Par1== VALUE_SOURCE_IR_RF || Par1== VALUE_SOURCE_RF)
        {
        S.TransmitPort=Par1;
        SaveSettings();
        }
      else
        error=VALUE_PARAMETER;
      break;

    case CMD_SEND_STATUS://??? foutive opties voldoende afgevangen
      x=Par1;// bevat het commando waarvoor de status opgehaald moet worden
      Par1=Par2;
      if(GetStatus(&x,&Par1,&Par2))// let op: call by reference. Gegevens komen terug in Par1 en Par2
        SendEvent(command2event(x,Par1,Par2));
      else
        error=VALUE_PARAMETER;
      break;
                
      default:
        error=true;
      }
    }   
  else
    error=VALUE_TYPE_COMMAND;
  
  if(error)
    {// als er een error is, dan een error-event genereren en verzenden.
    if(error!=VALUE_PARAMETER)Command=0;
    Event=command2event(CMD_ERROR,Command,error);
    SendEvent(Event);
    ProcessEvent(Event,VALUE_DIRECTION_INTERNAL,0,0,0);
    return false;
    }
  return true;
  }


