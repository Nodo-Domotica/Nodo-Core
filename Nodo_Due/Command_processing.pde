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
#define ERROR_COMMAND  4

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
  int Command      = EventPart(Content,EVENT_PART_COMMAND);
  int Par1         = EventPart(Content,EVENT_PART_PAR1);
  int Par2         = EventPart(Content,EVENT_PART_PAR2);
  byte Type         = EventType(Content);
  byte PreviousType = EventType(PreviousContent);
  
  if(error=CommandError(Content))// als er een error is, dan een error-event genereren en verzenden.
    {
    ErrorHandling(Command,error);
    return false;
    }
  else // geen fouten, dan verwerken
    {      
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
        if((S.UserVar[Par1-1]+Par2)<=255) // alleen ophogen als variabele nog niet de maximale waarde heeft.
          {
          if(!Par2)Par2=1;      
          S.UserVar[Par1-1]+=Par2;
          SaveSettings();
          }
        break;        
  
      case CMD_VARIABLE_DEC: 
        if((S.UserVar[Par1-1]-Par2)>=0) // alleen decrement als variabele hierdoor niet negatief wordt
          {
          if(!Par2)Par2=1;      
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
        S.UserVar[Par1-1]=analogRead(WiredAnalogInputPin_1+(Par2-1))>>2;
        SaveSettings();
        break;        
  
      case CMD_BREAK_ON_VAR_EQU:
        if(S.UserVar[Par1-1]==Par2)
          {
          if(S.Trace&1)
            PrintText(Text_09,true);
          error=true;
          }
  
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
        if(Par1==0)
          for(x=0;x<USER_TIMER_MAX;x++)
            UserTimer[x]=0L;
        else
            UserTimer[Par1-1]=0L;
        break;
  
      case CMD_SEND_USEREVENT:
        // Voeg Home toe en Maak Unit=0 want een UserEvent is ALTIJD voor ALLE Nodo's. Verzend deze vervolgens.
        SendEvent(command2event(CMD_USER_EVENT,Par1,Par2)&0x00ffffff | ((unsigned long)S.Home)<<28);// Voeg Home toe en Maak Unit=0 want een UserEvent is ALTIJD voor ALLE Nodo's.;
        break;
  
      case CMD_SIMULATE_DAY:
        if(Par1==0)Par1=1;
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
        Time.Year=x;
        ClockSet();
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
        UserTimer[Par1-1]=millis()+(unsigned long)(Par2)*60000L;// Par1=timer, Par2=minuten
        break;
  
      case CMD_TIMER_RANDOM:
        UserTimer[Par1-1]=millis()+(unsigned long)random(Par2)*60000L;// Par1=timer, Par2=maximaal aantal minuten
        break;
  
      case CMD_DELAY:
        delay(Par1*1000);     
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
  
      case CMD_TRANSMIT_SETTINGS:
        S.TransmitPort=Par1;
        SaveSettings();
        break;
  
      case CMD_SEND_STATUS:
        x=Par1;// bevat het commando waarvoor de status opgehaald moet worden
        Par1=Par2;
        if(error=GetStatus(&x,&Par1,&Par2))// let op: call by reference. Gegevens komen terug in Par1 en Par2
          SendEvent(command2event(x,Par1,Par2));
        break;      
       }
    }
  return error?false:true;
  }

/*********************************************************************************************\
 * Deze functie checked of de code die ontvangen is een uitvoerbare opdracht is/
 * Als het een correct commando is wordt een false teruggegeven 
 * in andere gevallen een foutcode
 \*********************************************************************************************/

byte CommandError(unsigned long Content)
  {
  byte Command      = EventPart(Content,EVENT_PART_COMMAND);
  byte Par1         = EventPart(Content,EVENT_PART_PAR1);
  byte Par2         = EventPart(Content,EVENT_PART_PAR2);

  switch(Command)
    {
    //test; geen, altijd goed
    case CMD_SEND_STATUS://??? foutive opties nog niet voldoende afgevangen
    case CMD_STATUS://??? foutive opties nog niet voldoende afgevangen
    case CMD_DELAY:
    case CMD_SOUND: 
    case CMD_SEND_RAW:
    case CMD_SEND_USEREVENT:
    case CMD_SEND_KAKU:
    case CMD_EVENTLIST_SHOW:
    case CMD_SEND_KAKU_NEW:
    case CMD_EVENTLIST_WRITE:
    case CMD_ANALYSE_SETTINGS:
    case CMD_RAWSIGNAL_GET:
    case CMD_RESET_FACTORY:
    case CMD_RAWSIGNAL_PUT:
    case CMD_EVENTLIST_ERASE: 
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
    case CMD_VARIABLE_VARIABLE:
      if(Par1<1 || Par1>USER_VARIABLES_MAX)return ERROR_PAR1;
      if(Par2<1 || Par2<USER_VARIABLES_MAX)return ERROR_PAR2;
      return false;
        
    // test:Par1 binnen bereik maximaal beschikbare variabelen, Par2 is een geldige WIRED_IN
    case CMD_VARIABLE_WIRED_ANALOG:
      if(Par1<1 || Par1>USER_VARIABLES_MAX)return ERROR_PAR1;
      if(Par2<1 || Par2>4)return ERROR_PAR2;
      return false;
 
    // test:Par1 binnen bereik maximaal beschikbare timers
    case CMD_TIMER_SET:
    case CMD_TIMER_RANDOM:
      if(Par1<1 || Par1>USER_TIMER_MAX)return ERROR_PAR1;
      return false;

    // test:Par1 binnen bereik maximaal beschikbare variabelen,0 mag ook (=alle variabelen)
    case CMD_VARIABLE_CLEAR:
      if(Par1>USER_VARIABLES_MAX)return ERROR_PAR1;
      return false;

    // test:Par1 binnen bereik maximaal beschikbare timers,0 mag ook (=alle timers)
    case CMD_TIMER_RESET:
      if(Par1>USER_TIMER_MAX)return ERROR_PAR1;
      return false;

    // alleen 0,1 of 7
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

     // par1 alleen 0 of 1.
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
 * Handel een foutmelding af
 \*********************************************************************************************/
void ErrorHandling(byte P1, byte P2)
  {
  unsigned long Event;

  Event=command2event(CMD_ERROR,P1,P2);
  ProcessEvent(Event,VALUE_DIRECTION_INTERNAL,0,0,0);
  SendEvent(Event);
  }

  
  
  
  
  
