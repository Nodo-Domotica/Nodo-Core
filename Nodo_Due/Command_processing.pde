  /**************************************************************************\
    This file is part of Nodo Due, Â© Copyright Paul Tonkes

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
 * Eenvoudige check of event een Nodo commando is die voor deze Nodo bestemd is.
 * Test NIET op geldigheid van de parameters
 \*********************************************************************************************/
byte NodoType(unsigned long Content)
  {
  byte x;

  // als het geen Nodo event of commando was dan zowieso geen commando
  if(((Content>>28)&0xf)!=SIGNAL_TYPE_NODO)
    return false;
  
  // als het een UserEvent was, dan is die altijd voor deze Nodo
  if(((Content>>16)&0xff)==CMD_USER_EVENT)
    return NODO_TYPE_EVENT;

  // als het voor een andere Nodo bestemd was Unit deel ongelijk aan eigen adres en ongelijk aan wildcard unit=0
  x=(Content>>24)&0xf;
  if(x!=S.Unit && x!=0)
    return false;
 
  x=(Content>>16)&0xff;
  if(x<=RANGE_VALUE)
    return false;

  if(x>=RANGE_EVENT)
    return NODO_TYPE_EVENT;

  return NODO_TYPE_COMMAND;
  }  
  

/*********************************************************************************************\
 * Deze functie checked of de code die ontvangen is een geldige uitvoerbare opdracht is
 * Als het een correct commando is wordt een false teruggegeven 
 * in andere gevallen een foutcode
 \*********************************************************************************************/
#define ERROR_PAR1     1
#define ERROR_PAR2     2
#define ERROR_COMMAND  3

byte CommandError(unsigned long Content)
  {
  byte x;
  
  x=NodoType(Content);
  if(x!=NODO_TYPE_COMMAND && x!=NODO_TYPE_EVENT)
    return ERROR_COMMAND;
  
  byte Command      = (Content>>16)&0xff;
  byte Par1         = (Content>>8)&0xff;
  byte Par2         = Content&0xff;
  
  switch(Command)
    {
    //test; geen, altijd goed
    case CMD_ERROR:
    case CMD_WAITFREERF: 
    case CMD_VARIABLE_EVENT:    
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
    case CMD_STATUS:
    case CMD_DISPLAY: // ??? foutieve invoer nog afvangen
    case CMD_DELAY:
    case CMD_SOUND: 
    case CMD_SEND_SIGNAL:
    case CMD_USER_EVENT:
    case CMD_SEND_USEREVENT:
    case CMD_ANALYSE_SETTINGS:
    case CMD_RAWSIGNAL_GET:
    case CMD_RAWSIGNAL_PUT:
    case CMD_KAKU:
    case CMD_SEND_KAKU:
      return false;
 
    case CMD_SEND_KAKU_NEW:
    case CMD_KAKU_NEW:    
      if(Par2==VALUE_ON || Par2==VALUE_OFF || Par2<=16)return false;
      return ERROR_PAR2;

    // geen par1 of par2 ingevuld.
    case CMD_BOOT_EVENT:
    case CMD_EVENTLIST_WRITE:
    case CMD_EVENTLIST_SHOW:
    case CMD_RESET:
    case CMD_EVENTLIST_ERASE: 
      if(Par1!=0)return ERROR_PAR1;    
      if(Par2!=0)return ERROR_PAR2;    
      return false; 
      
    case CMD_VARIABLE_SET:   
    case CMD_TIMER_SET_SEC:
    case CMD_TIMER_SET_MIN:
      if(Par1>USER_VARIABLES_MAX)return ERROR_PAR1;
      return false;

    // test:Par1 binnen bereik maximaal beschikbare variabelen
    case CMD_VARIABLE_INC: 
    case CMD_VARIABLE_DEC: 
    case CMD_BREAK_ON_VAR_NEQU:
    case CMD_BREAK_ON_VAR_MORE:
    case CMD_BREAK_ON_VAR_LESS:
    case CMD_BREAK_ON_VAR_EQU:
      if(Par1<1 || Par1>USER_VARIABLES_MAX)return ERROR_PAR1;
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
    case CMD_TIMER_RANDOM:
      if(Par1<1 || Par1>TIMER_MAX)return ERROR_PAR1;
      return false;

    // test:Par1 binnen bereik maximaal beschikbare variabelen,0 mag ook (=alle variabelen)

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
      if(Par1>31 || Par1<1)return ERROR_PAR1;
      if(Par2>12 || Par2<1)return ERROR_PAR2;
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

    // test:Par1 binnen bereik maximaal beschikbare wired poorten EN Par2 is ON of OFF
    case CMD_WIRED_OUT:
    case CMD_WIRED_PULLUP:
      if(Par1<1 || Par1>4)return ERROR_PAR1;
      if(Par2!=VALUE_ON && Par2!=VALUE_OFF)return ERROR_PAR2;
      return false;

    case CMD_COPYSIGNAL:
      if(Par1!=VALUE_RF_2_IR && Par1!=VALUE_IR_2_RF)
      return false;

    case CMD_WAITBUSY:
    case CMD_SENDBUSY:
      if(Par1!=VALUE_OFF && Par1!=VALUE_ON && Par1!=VALUE_ALL)return ERROR_PAR1;
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
        case CMD_ERROR:
        case CMD_KAKU:
        case CMD_KAKU_NEW:
        case CMD_USER_EVENT:
          break;
        default:
          return ERROR_PAR2;
        } 
      return false;

     // par1 alleen On of Off.
    case CMD_SIMULATE:
      if(Par1!=VALUE_OFF && Par1!=VALUE_ON)return ERROR_PAR1;
      return false;

    case CMD_DIVERT:   
     if(Par1>UNIT_MAX)return ERROR_PAR1;
     return false;

    case CMD_UNIT:
      if(Par1<1 || Par1>UNIT_MAX)return ERROR_PAR1;
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
  byte Type         = (Content>>28)&0x0f;
  byte PreviousType = (PreviousContent>>28)&0x0f;
  
  if(error=CommandError(Content))// als er een error is, dan een error-event genereren en verzenden.
    {
    TransmitCode(command2event(CMD_ERROR,Command,error),SIGNAL_TYPE_NODO);
    return false;
    }
  else // geen fouten, dan verwerken
    {        
    switch(Command)
      {   
      case CMD_SEND_KAKU:
        TransmitCode(command2event(CMD_KAKU,Par1,Par2),SIGNAL_TYPE_KAKU);
        break;
        
      case CMD_SEND_KAKU_NEW:
        TransmitCode(command2event(CMD_KAKU_NEW,Par1,Par2),SIGNAL_TYPE_NEWKAKU);
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
        if(Par1==0)
          for(byte x=0;x<USER_VARIABLES_MAX;x++)
            S.UserVar[x]=Par2;
        else
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
          error=true;
        break;
        
      case CMD_BREAK_ON_VAR_NEQU:
        if(S.UserVar[Par1-1]!=Par2)
          error=true;
        break;        
  
      case CMD_BREAK_ON_VAR_MORE:
        if(S.UserVar[Par1-1]>Par2)
          error=true;
        break;        
  
      case CMD_BREAK_ON_VAR_LESS:
        if(S.UserVar[Par1-1]<Par2)
          error=true;
        break;  
  
      case CMD_SEND_USEREVENT:
        // Voeg Unit=0 want een UserEvent is ALTIJD voor ALLE Nodo's. Verzend deze vervolgens.
        TransmitCode(command2event(CMD_USER_EVENT,Par1,Par2),SIGNAL_TYPE_NODO);// Maak Unit=0 want een UserEvent is ALTIJD voor ALLE Nodo's.;
        break;
  
      case CMD_SEND_VAR_USEREVENT:
        // Maak Unit=0 want een UserEvent is ALTIJD voor ALLE Nodo's. Verzend deze vervolgens.
        TransmitCode(command2event(CMD_USER_EVENT,S.UserVar[Par1-1],S.UserVar[Par2-1])&0xf0ffffff,SIGNAL_TYPE_NODO);// Maak Unit=0 want een UserEvent is ALTIJD voor ALLE Nodo's.;
        break;

      case CMD_WIRED_ANALOG:
        // Lees de analoge waarde uit en verzend deze
        TransmitCode(command2event(CMD_WIRED_ANALOG,Par1,WiredAnalog(Par1-1)),SIGNAL_TYPE_NODO);
        break;

      case CMD_SIMULATE_DAY:
        if(Par1==0)Par1=1;
        SimulateDay(Par1); 
        break;     
  
      case CMD_SEND_SIGNAL:
        TransmitCode(0L,SIGNAL_TYPE_UNKNOWN);
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

      case CMD_TIMER_SET_MIN:
        // Par1=timer, Par2=minuten. Timers werken op een resolutie van seconden maar worden door de gebruiker ingegeven in minuten        
        TimerSet(Par1,int(Par2)*60);
        break;
        
      case CMD_TIMER_SET_SEC:
        // Par1=timer, Par2=seconden. Timers werken op een resolutie van seconden.            
        TimerSet(Par1,Par2);
        break;

      case CMD_TIMER_RANDOM:
        UserTimer[Par1-1]=millis()+random(Par2)*60000;// Par1=timer, Par2=maximaal aantal minuten
        break;
  
      case CMD_DELAY:
        if(Par1)
          {
          HoldTimer=millis()+((unsigned long)(Par1))*1000;
          if(Par2==VALUE_OFF) // Niet opslaan in de queue, maar direct een 'dode' pause uitvoeren.
            {
            while(HoldTimer>millis())        
              digitalWrite(MonitorLedPin,(millis()>>7)&0x01);
            }
          else
            {
            // start een nieuwe recursieve loop() om zo de events die voorbij komen te plaatsen in de queue.
            // deze recursieve aanroep wordt beëindigd als HoldTimer==0L
            Hold=CMD_DELAY;
            loop();
            }
          }        
        else
          HoldTimer=0L; //  Wachttijd is afgelopen;
        break;        
        
      case CMD_SOUND: 
        Alarm(Par1,Par2);
        break;     
    
      case CMD_WIRED_PULLUP:
        S.WiredInputPullUp[Par1-1]=Par2==VALUE_ON; // Par1 is de poort[1..4], Par2 is de waarde [0..1]
        digitalWrite(14+WiredAnalogInputPin_1+Par1-1,S.WiredInputPullUp[Par1-1]==VALUE_ON);// Zet de pull-up weerstand van 20K voor analoge ingangen. Analog-0 is gekoppeld aan Digital-14
        SaveSettings();
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
        digitalWrite(WiredDigitalOutputPin_1+Par1-1,Par2==VALUE_ON);
        WiredOutputStatus[Par1-1]=Par2==VALUE_ON;
        PrintEvent(Content,VALUE_SOURCE_WIRED,VALUE_DIRECTION_OUTPUT);
        break;
                    
      case CMD_WIRED_SMITTTRIGGER:
        S.WiredInputSmittTrigger[Par1-1]=Par2; // Par1 is de poort[1..4], Par2 is de waarde [0..255]
        SaveSettings();
        break;
       
      case CMD_WAITFREERF: 
        S.WaitFreeRF_Delay=Par1;
        S.WaitFreeRF_Window=Par2;
        SaveSettings();
        break;
  
      case CMD_COPYSIGNAL:
        if(Par1==VALUE_RF_2_IR)CopySignalRF2IR(Par2);      
        if(Par1==VALUE_IR_2_RF)CopySignalIR2RF(Par2);
        break;        
  
      case CMD_SENDBUSY:
        if(Par1==VALUE_ALL)
          S.SendBusy=true;
        else
          if(S.SendBusy)
            S.SendBusy=false;
          else
            TransmitCode(command2event(CMD_BUSY,Par1,0),SIGNAL_TYPE_NODO);
        break;
        
      case CMD_WAITBUSY:
        if(Par1==VALUE_ALL)
          S.WaitBusy=true;
        else
          if(S.WaitBusy)
            S.WaitBusy=false;
          else
            {
            Hold=CMD_BUSY;
            loop(); // deze recursieve aanroep wordt beëindigd als BusyNodo==0
            }
        break;
  
      case CMD_TRANSMIT_SETTINGS:
        S.TransmitPort=Par1;
        if(Par2)S.TransmitRepeat=Par2;
        SaveSettings();
        break;
    
       case CMD_STATUS:
         Status(Src==VALUE_SOURCE_SERIAL, Par1, Par2);
         break;

      case CMD_ANALYSE_SETTINGS:
        S.AnalyseTimeOut=Par1;
        S.AnalyseSharpness=Par2*1000;
        SaveSettings();
        break;
       }
    }
  return error?false:true;
  }



  
  
  
  
  

