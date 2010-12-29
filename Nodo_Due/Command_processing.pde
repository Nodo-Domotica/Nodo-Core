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


#define ERROR_PAR1     1
#define ERROR_PAR2     2
#define ERROR_COMMAND  3

/*********************************************************************************************\
 * Deze functie checked of de code die ontvangen is een uitvoerbare opdracht is/
 * Als het een correct commando is wordt een false teruggegeven 
 * in andere gevallen een foutcode
 \*********************************************************************************************/

byte CommandError(unsigned long Content)
  {
  byte Command      = (Content>>16)&0xff;
  byte Par1         = (Content>>8)&0xff;
  byte Par2         = Content&0xff;

  switch(Command)
    {
    //test; geen, altijd goed
    case CMD_VARIABLE_EVENT:    
    case CMD_KAKU:
    case CMD_DLS_EVENT:
    case CMD_CLOCK_EVENT_DAYLIGHT:
    case CMD_CLOCK_EVENT_ALL:
    case CMD_CLOCK_EVENT_SUN:
    case CMD_CLOCK_EVENT_MON:
    case CMD_CLOCK_EVENT_TUE:
    case CMD_CLOCK_EVENT_WED:
    case CMD_CLOCK_EVENT_THU:
    case CMD_CLOCK_EVENT_FRI:
    case CMD_CLOCK_EVENT_SAT:
    case CMD_SEND_STATUS:
    case CMD_STATUS:
    case CMD_DELAY:
    case CMD_SOUND: 
    case CMD_SEND_SIGNAL:
    case CMD_USER_EVENT:
    case CMD_SEND_USEREVENT:
    case CMD_SEND_KAKU:
    case CMD_SEND_KAKU_NEW:
    case CMD_ANALYSE_SETTINGS:
    case CMD_RAWSIGNAL_GET:
    case CMD_RAWSIGNAL_PUT:
      return false;
 
    // geen par1 of par2 ingevuld.
    case CMD_BOOT_EVENT:
    case CMD_EVENTLIST_WRITE:
    case CMD_EVENTLIST_SHOW:
    case CMD_RESET_FACTORY:
    case CMD_EVENTLIST_ERASE: 
      if(Par1!=0)return ERROR_PAR1;    
      if(Par2!=0)return ERROR_PAR2;    
      return false; 
      
    // test:Par1 binnen bereik maximaal beschikbare variabelen
    case CMD_VARIABLE_INC: 
    case CMD_VARIABLE_DEC: 
    case CMD_VARIABLE_SET:   
    case CMD_BREAK_ON_VAR_NEQU:
    case CMD_BREAK_ON_VAR_MORE:
    case CMD_BREAK_ON_VAR_LESS:
    case CMD_BREAK_ON_VAR_EQU:
      if(Par1<1 || Par1>USER_VARIABLES_MAX)return VALUE_PARAMETER;
      return false;
      
    // test:Par1 en Par2 binnen bereik maximaal beschikbare variabelen
    case CMD_SEND_VAR_USEREVENT:
    case CMD_VARIABLE_VARIABLE:
      if(Par1<1 || Par1>USER_VARIABLES_MAX)return ERROR_PAR1;
      if(Par2<1 || Par2>USER_VARIABLES_MAX)return ERROR_PAR2;
      return false;
        
    // test:Par1 binnen bereik maximaal beschikbare variabelen, Par2 is een geldige WIRED_IN
    case CMD_VARIABLE_WIRED_ANALOG:
      if(Par1<1 || Par1>USER_VARIABLES_MAX)return ERROR_PAR1;
      if(Par2<1 || Par2>4)return ERROR_PAR2;
      return false;
 
    // test:Par1 binnen bereik maximaal beschikbare timers
    case CMD_TIMER_EVENT:
    case CMD_TIMER_SET:
    case CMD_TIMER_RANDOM:
      if(Par1<1 || Par1>TIMER_MAX)return ERROR_PAR1;
      return false;

    // test:Par1 binnen bereik maximaal beschikbare variabelen,0 mag ook (=alle variabelen)
    case CMD_VARIABLE_CLEAR:
      if(Par1>USER_VARIABLES_MAX)return ERROR_PAR1;
      return false;

    // test:Par1 binnen bereik maximaal beschikbare timers,0 mag ook (=alle timers)
    case CMD_TIMER_RESET:
      if(Par1>TIMER_MAX)return ERROR_PAR1;
      return false;

    case CMD_WIRED_RANGE:
      if(Par1<1 || Par1>4)return ERROR_PAR1; // poort
      if(Par2>4)return ERROR_PAR2; // range
      return false;

    // Par1 alleen 0,1 of 7
    case CMD_SIMULATE_DAY:
      if(Par1!=0 && Par1!=1 && Par1!=7)return ERROR_PAR1;
      return false;
      
    // geldig jaartal
    case CMD_CLOCK_YEAR:
      if(Par1>21)return ERROR_PAR1;
      return false;
    
    // geldige tijd    
    case CMD_CLOCK_TIME:
      if(Par1>23)return ERROR_PAR1;
      if(Par2>59)return ERROR_PAR2;
      return false;

    // geldige datum
    case CMD_CLOCK_DATE: // data1=datum, data2=maand
      if(Par1>31)return ERROR_PAR1;
      if(Par2>12)return ERROR_PAR2;
      return false;

    case CMD_CLOCK_DOW:
      if(Par1<1 || Par1>7)return ERROR_PAR1;
      return false;
       
    // test:Par1 binnen bereik maximaal beschikbare wired poorten, Par2 [0..255]
    case CMD_WIRED_IN_EVENT:
    case CMD_WIRED_ANALOG:
    case CMD_WIRED_THRESHOLD:
    case CMD_WIRED_SMITTTRIGGER:
      if(Par1<1 || Par1>4)return ERROR_PAR1;
      return false;

    // test:Par1 binnen bereik maximaal beschikbare wired poorten EN Par2 is 0 of 1
    case CMD_WIRED_OUT:
    case CMD_WIRED_PULLUP:
      if(Par1<1 || Par1>4)return ERROR_PAR1;
      if(Par2!=0 && Par2!=1)return ERROR_PAR2;
      return false;

    case CMD_WAITFREERF: 
      if(Par1!=VALUE_OFF && Par1!=VALUE_SERIES && Par1!=VALUE_ALL)return ERROR_PAR1;
      return false;

    case CMD_COPYSIGNAL:
      if(Par1!=VALUE_RF_2_IR && Par1!=VALUE_IR_2_RF)return ERROR_PAR1;
      return false;

    case CMD_TRANSMIT_SETTINGS:
      if(Par1!=VALUE_SOURCE_IR && Par1!=VALUE_SOURCE_IR_RF && Par1!=VALUE_SOURCE_RF)return ERROR_PAR1;
      return false;

    case CMD_COMMAND_WILDCARD:
      switch(Par1)
        {
        case VALUE_ALL:
        case VALUE_SOURCE_IR:
        case VALUE_SOURCE_RF:
        case VALUE_SOURCE_SERIAL:
        case VALUE_SOURCE_WIRED:
        case VALUE_SOURCE_EVENTLIST:
        case VALUE_SOURCE_SYSTEM:
        case VALUE_SOURCE_TIMER:
        case VALUE_SOURCE_VARIABLE:
        case VALUE_SOURCE_CLOCK:
          break;
        default:
          return ERROR_PAR1;
        }

      switch(Par2)
        {
        case VALUE_ALL:
        case CMD_KAKU:
        case CMD_KAKU_NEW:
        case CMD_USER_EVENT:
        case VALUE_TYPE_OTHERUNIT:
        case VALUE_TYPE_UNKNOWN:
          break;
        default:
          return ERROR_PAR2;
        } 
      return false;

     // par1 alleen 0 of 1.
    case CMD_CONFIRM:
    case CMD_SIMULATE:
      if(Par1!=0 && Par1!=1)return ERROR_PAR1;
      return false;

    case CMD_TRACE:
      if(Par1>1)return ERROR_PAR1;
      if(Par1>2)return ERROR_PAR2;
      return false;

    case CMD_DIVERT:   
     if(Par1>UNIT_MAX)return ERROR_PAR1;
     return false;

    case CMD_UNIT:
      if(Par1<1 || Par1>UNIT_MAX)return ERROR_PAR1;
      if(Par2>HOME_MAX)return ERROR_PAR2;
      return false;

    default:
      return ERROR_COMMAND;
    }
  }

 
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

  byte error        = false;
  byte Command      = (Content>>16)&0xff;
  byte Par1         = (Content>>8)&0xff;
  byte Par2         = Content&0xff;
  byte Type         = EventType(Content);
  byte PreviousType = EventType(PreviousContent);
  
  if(error=CommandError(Content))// als er een error is, dan een error-event genereren en verzenden.
    {
    GenerateEvent(CMD_ERROR,Command,error);
    return false;
    }
  else // geen fouten, dan verwerken
    {      
    switch(Command)
      {   
      case CMD_SEND_KAKU:
        SendEventCode(command2event(CMD_KAKU,Par1,Par2));
        break;
        
      case CMD_SEND_KAKU_NEW:
        SendEventCode(command2event(CMD_KAKU_NEW,Par1,Par2));
        break;
        
      case CMD_VARIABLE_INC: 
        if((S.UserVar[Par1-1]+Par2)<=255) // alleen ophogen als variabele nog niet de maximale waarde heeft.
          {
          S.UserVar[Par1-1]+=Par2;
          SaveSettings();
          }
        break;        
  
      case CMD_VARIABLE_DEC: 
        if((S.UserVar[Par1-1]-Par2)>=0) // alleen decrement als variabele hierdoor niet negatief wordt
          {
          S.UserVar[Par1-1]-=Par2;
          SaveSettings();
          }
        break;        
  
      case CMD_VARIABLE_SET:   
        S.UserVar[Par1-1]=Par2;
        SaveSettings();
        break;        
    
      case CMD_VARIABLE_VARIABLE:
        S.UserVar[Par1-1]=S.UserVar[Par2-1];
        SaveSettings();
        break;        
  
      case CMD_VARIABLE_WIRED_ANALOG:
        S.UserVar[Par1-1]=WiredAnalog(Par2-1);
        SaveSettings();
        break;        
  
      case CMD_BREAK_ON_VAR_EQU:
        if(S.UserVar[Par1-1]==Par2)
          {
          if(S.Trace&1)
            PrintText(Text_09,true);
          error=true;
          }
        break;
        
      case CMD_BREAK_ON_VAR_NEQU:
        if(S.UserVar[Par1-1]!=Par2)
          {
          if(S.Trace&1)PrintText(Text_09,true);
          error=true;
          }
        break;        
  
      case CMD_BREAK_ON_VAR_MORE:
        if(S.UserVar[Par1-1]>Par2)
          {
          if(S.Trace&1)
            PrintText(Text_09,true);
          error=true;
          }
        break;        
  
      case CMD_BREAK_ON_VAR_LESS:
        if(S.UserVar[Par1-1]<Par2)
          {
          if(S.Trace&1)
            PrintText(Text_09,true);
          error=true;
          }
        break;  
  
      case CMD_TIMER_RESET:
        TimerClear(Par1);
        break;
  
      case CMD_SEND_USEREVENT:
        // Voeg Home toe en Maak Unit=0 want een UserEvent is ALTIJD voor ALLE Nodo's. Verzend deze vervolgens.
        SendEventCode(command2event(CMD_USER_EVENT,Par1,Par2)&0x00ffffff | ((unsigned long)S.Home)<<28);// Voeg Home toe en Maak Unit=0 want een UserEvent is ALTIJD voor ALLE Nodo's.;
        break;
  
      case CMD_SEND_VAR_USEREVENT:
        // Voeg Home toe en Maak Unit=0 want een UserEvent is ALTIJD voor ALLE Nodo's. Verzend deze vervolgens.
        SendEventCode(command2event(CMD_USER_EVENT,S.UserVar[Par1-1],S.UserVar[Par2-1])&0x00ffffff | ((unsigned long)S.Home)<<28);// Voeg Home toe en Maak Unit=0 want een UserEvent is ALTIJD voor ALLE Nodo's.;
        break;

      case CMD_SIMULATE_DAY:
        if(Par1==0)Par1=1;
        SimulateDay(Par1); 
        break;     
  
      case CMD_SEND_SIGNAL:
        SendEventCode(0L);
        break;        
  
      case CMD_CLOCK_YEAR:
        x=Par1*100+Par2;
        Time.Year=x;
        ClockSet();
        ClockRead();
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
        ClockRead();
        break;
  
      case CMD_CLOCK_DOW:
        Time.Day=Par1;
        ClockSet();
        break;
         
      case CMD_TIMER_SET:
        // timers werken op een resolutie van seconden maar worden door de gebruiker ingegeven in minuten
        UserTimer[Par1-1]=millis()+Par2*60000;// Par1=timer, Par2=minuten
        break;
  
      case CMD_TIMER_RANDOM:
        UserTimer[Par1-1]=millis()+random(Par2)*60000;// Par1=timer, Par2=maximaal aantal minuten
        break;
  
      case CMD_DELAY:
        if(Par1)
          DelayTimer=millis()+((unsigned long)(Par1))*1000;     
        else
          DelayTimer=0L;
        break;        
        
      case CMD_SOUND: 
        Alarm(Par1,Par2);
        break;     
    
      case CMD_WIRED_PULLUP:
        S.WiredInputPullUp[Par1-1]=Par2; // Par1 is de poort[1..4], Par2 is de waarde [0..1]
        digitalWrite(14+WiredAnalogInputPin_1+Par1-1,S.WiredInputPullUp[Par1-1]?HIGH:LOW);// Zet de pull-up weerstand van 20K voor analoge ingangen. Analog-0 is gekoppeld aan Digital-14
        SaveSettings();
        break;
             
      case CMD_VARIABLE_CLEAR:
        VariableClear(Par1);
        break;
       
      case CMD_WIRED_THRESHOLD:
        S.WiredInputThreshold[Par1-1]=Par2; // Par1 is de poort[1..4], Par2 is de waarde [0..255]
        SaveSettings();
        break;
  
      case CMD_WIRED_RANGE:
        S.WiredInputRange[Par1-1]=Par2; // Par1 is de poort[1..4], Par2 is de range [0..4]
        SaveSettings();
        break;

      case CMD_WIRED_OUT:
        digitalWrite(WiredDigitalOutputPin_1+Par1-1,Par2&1);
        WiredOutputStatus[Par1-1]=Par2&1;
        break;
                    
      case CMD_WIRED_SMITTTRIGGER:
        S.WiredInputSmittTrigger[Par1-1]=Par2; // Par1 is de poort[1..4], Par2 is de waarde [0..255]
        SaveSettings();
        break;
       
      case CMD_WAITFREERF: 
        S.WaitFreeRFAction=Par1;
        S.WaitFreeRFWindow=Par2==0?S.Unit*500:Par2*100;
        SaveSettings();
        break;
  
      case CMD_COPYSIGNAL:
        if(Par1==VALUE_RF_2_IR)CopySignalRF2IR(Par2);      
        if(Par1==VALUE_IR_2_RF)CopySignalIR2RF(Par2);
        break;        
  
      case CMD_CONFIRM:
        S.Confirm=Par1;
        SaveSettings();
        break;
  
      case CMD_TRANSMIT_SETTINGS:
        S.TransmitPort=Par1;
        SaveSettings();
        break;
  
      case CMD_SEND_STATUS:
        Command=Par1;// bevat het commando waarvoor de status opgehaald moet worden
        Par1=Par2;
        if(GetStatus(&Command,&Par1,&Par2))// let op: call by reference. Gegevens komen terug in Par1 en Par2
          GenerateEvent(Command,Par1,Par2);
        break;
       }
    }
  return error?false:true;
  }



  
  
  
  
  
