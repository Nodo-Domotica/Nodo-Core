
/*********************************************************************************************\
 * Eenvoudige check of event een Nodo commando is die voor deze Nodo bestemd is.
 * Test NIET op geldigheid van de parameters
 * geeft de waarde 0, NODO_TYPE_EVENT of NODO_TYPE_COMMAND terug.
 \*********************************************************************************************/
byte NodoType(unsigned long Content)
  {
  byte x;

  // als het geen Nodo event of commando was dan zowieso geen commando
  if(((Content>>28)&0xf)!=SIGNAL_TYPE_NODO)
    return false;
  
  // als het een UserEvent was, dan is die altijd voor deze Nodo
  if(((Content>>16)&0xff)==CMD_USEREVENT)
    return NODO_TYPE_EVENT;

  // als Unit deel ongelijk is aan eigen adres en ongelijk aan wildcard unit=0, dan was het een event voor een andere Nodo
  x=(Content>>24)&0xf;
  if(x!=S.Unit && x!=0)
    return false;

  x=(Content>>16)&0xff;

  if(x>=FIRST_EVENT && x<=LAST_EVENT)
    return NODO_TYPE_EVENT;

  if(x>=FIRST_COMMAND && x<=LAST_COMMAND)
    return NODO_TYPE_COMMAND;

  return false;
  }  
  

/*********************************************************************************************\
 * Deze functie checked of de code die ontvangen is een geldige uitvoerbare opdracht is
 * Als het een correct commando is wordt een false teruggegeven 
 * in andere gevallen een foutcode
 \*********************************************************************************************/
byte CommandError(unsigned long Content)
  {
  int x,y;
  
  x=NodoType(Content);
  if(x!=NODO_TYPE_COMMAND && x!=NODO_TYPE_EVENT)
    return MESSAGE_01;
  
  byte Command      = (Content>>16)&0xff;
  byte Par1         = (Content>>8)&0xff;
  byte Par2         = Content&0xff;
 
  switch(Command)
    {
    //test; geen, altijd goed
    case CMD_EVENTLIST_ERASE:
    case CMD_BOOT_EVENT:
    case CMD_RESET:
    case CMD_RAWSIGNAL_SEND:
    case CMD_RAWSIGNAL:
    case CMD_MESSAGE :
    case CMD_REBOOT:
    case CMD_WAITFREERF: 
    case CMD_USERPLUGIN: 
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
    case CMD_STATUS_SEND:
    case CMD_DELAY:
    case CMD_SOUND: 
    case CMD_USEREVENT:
    case CMD_SEND_USEREVENT:
    case CMD_KAKU:
    case CMD_SEND_KAKU:
    case CMD_RECEIVE_LINE:
    case CMD_RAWSIGNAL_SAVE:
      return false;
 
    case CMD_SEND_KAKU_NEW:
    case CMD_KAKU_NEW:    
      if(Par2==VALUE_ON || Par2==VALUE_OFF || Par2<=16)return false;
      return MESSAGE_02;

    case CMD_EVENTLIST_WRITE:
      if(Par1>EVENTLIST_MAX)return MESSAGE_02;    
      return false; 

    case CMD_TIMER_SET_SEC:
    case CMD_TIMER_SET_MIN:
      if(Par1>TIMER_MAX)return MESSAGE_02;
      return false;

    // test:Par1 binnen bereik maximaal beschikbare variabelen
    case CMD_VARIABLE_INC: 
    case CMD_VARIABLE_DEC: 
    case CMD_VARIABLE_EVENT:    
    case CMD_BREAK_ON_VAR_NEQU:
    case CMD_BREAK_ON_VAR_MORE:
    case CMD_BREAK_ON_VAR_LESS:
    case CMD_BREAK_ON_VAR_EQU:
      Par2PortAnalog(Par1, Par2, &x, &y);
      if(x<1 || x>USER_VARIABLES_MAX)return MESSAGE_02;
      return false;

    case CMD_WIRED_ANALOG_CALIBRATE:
    case CMD_WIRED_ANALOG:
    case CMD_WIRED_THRESHOLD:
    case CMD_WIRED_SMITTTRIGGER:
      Par2PortAnalog(Par1, Par2, &x, &y);// x=wired , y=waarde
      if(x<1 || x>WIRED_PORTS)return MESSAGE_02;
      return false;

    case CMD_ANALYSE_SETTINGS:
      if(x<1 || x>50)return MESSAGE_02;
      return false;

    case CMD_VARIABLE_SET:
    case CMD_VARIABLE_SAVE:
      if(Par1>USER_VARIABLES_MAX)return MESSAGE_02;
      return false;
      
    // test:Par1 en Par2 binnen bereik maximaal beschikbare variabelen
    case CMD_VARIABLE_VARIABLE:
      if(Par1<1 || Par1>USER_VARIABLES_MAX)return MESSAGE_02;
      if(Par2<1 || Par2>USER_VARIABLES_MAX)return MESSAGE_02;
      return false;
        
    // test:Par1 binnen bereik maximaal beschikbare timers
    case CMD_TIMER_EVENT:
    case CMD_TIMER_RANDOM:
      if(Par1<1 || Par1>TIMER_MAX)return MESSAGE_02;
      return false;
      
    // geldig jaartal
    case CMD_CLOCK_YEAR:
      if(Par1!=20 || Par2>99)return MESSAGE_02;
      return false;
    
    // geldige tijd    
    case CMD_CLOCK_TIME:
      if(Par1>23)return MESSAGE_02;
      if(Par2>59)return MESSAGE_02;
      return false;

    // geldige datum
    case CMD_CLOCK_DATE: // data1=datum, data2=maand
      if(Par1>31 || Par1<1)return MESSAGE_02;
      if(Par2>12 || Par2<1)return MESSAGE_02;
      return false;

    case CMD_CLOCK_DOW:
      if(Par1<1 || Par1>7)return MESSAGE_02;
      return false;

    case CMD_WIREDANALOG_VARIABLE:
      if(Par1<1 || Par1>USER_VARIABLES_MAX)return MESSAGE_02;
      if(Par2<1 || Par2>WIRED_PORTS)return MESSAGE_02;
      return false;
      
    // test:Par1 binnen bereik maximaal beschikbare wired poorten.
    case CMD_WIRED_IN_EVENT:
      if(Par1<1 || Par1>WIRED_PORTS)return MESSAGE_02;
      if(Par2!=VALUE_ON && Par2!=VALUE_OFF)return MESSAGE_02;
      return false;

    case CMD_WIRED_OUT:
      if(Par1>WIRED_PORTS)return MESSAGE_02;
      if(Par2!=VALUE_ON && Par2!=VALUE_OFF)return MESSAGE_02;
      return false;

    case CMD_WIRED_PULLUP:
      if(Par1<1 || Par1>WIRED_PORTS)return MESSAGE_02;
      if(Par2!=VALUE_ON && Par2!=VALUE_OFF)return MESSAGE_02;
      return false;

    case CMD_WAITBUSY:
    case CMD_SENDBUSY:
    case CMD_QUEUE:
      if(Par1!=0 && Par1!=VALUE_OFF && Par1!=VALUE_ON && Par1!=VALUE_ALL)return MESSAGE_02;
      return false;

    case CMD_PULSE_VARIABLE:
      if(Par1<1 || Par1>USER_VARIABLES_MAX || Par2>8)return MESSAGE_02;
      return false;

    case CMD_SEND_EVENT:
      switch(Par1)
        {
        case VALUE_ALL:
        case VALUE_SOURCE_IR:
        case VALUE_SOURCE_RF:
#if NODO_MEGA
        case VALUE_SOURCE_SERIAL:
        case VALUE_SOURCE_EVENTGHOST:
        case VALUE_SOURCE_HTTP:
#endif
          break;
        default:
          return MESSAGE_02;
        }
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
#if NODO_MEGA
        case VALUE_SOURCE_EVENTGHOST:
        case VALUE_SOURCE_HTTP:
#endif
          break;
        default:
          return MESSAGE_02;
        }

      switch(Par2)
        {
        case VALUE_ALL:
        case CMD_MESSAGE :
        case CMD_KAKU:
        case CMD_KAKU_NEW:
        case CMD_USEREVENT:
          break;
        default:
          return MESSAGE_02;
        } 
      return false;

     // par1 alleen On of Off.
     // par2 mag alles zijn
    case CMD_LOCK:
    case CMD_BREAK_ON_DAYLIGHT:
      if(Par1!=VALUE_OFF && Par1!=VALUE_ON)return MESSAGE_02;
      return false;

    case CMD_TRANSMIT_RF:
    case CMD_TRANSMIT_IR:
      if(Par1!=VALUE_OFF && Par1!=VALUE_ON)return MESSAGE_02;
      if(Par2>25)return MESSAGE_02;
      return false;

    case CMD_UNIT:
      if(Par1<1 || Par1>UNIT_MAX)return MESSAGE_02;
      return false;

#if NODO_MEGA
    case CMD_DEBUG:
    case CMD_SIMULATE_DAY:
      return false;
      
    case CMD_FILE_EXECUTE:
      return false;

    case CMD_TRANSMIT_IP:
      if(Par1!=VALUE_OFF && Par1!=VALUE_SOURCE_HTTP && Par1!=VALUE_SOURCE_EVENTGHOST)return MESSAGE_02;
      if(Par2!=VALUE_OFF && Par2!=VALUE_ON && Par2!=0)return MESSAGE_02;
      return false;
#endif

    default:
      return MESSAGE_01;
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
  unsigned long Event, a;  
  int x,y,z;

  byte error        = false;
  byte Command      = (Content>>16)&0xff;
  byte Par1         = (Content>>8)&0xff;
  byte Par2         = Content&0xff;
  byte Type         = (Content>>28)&0x0f;
  byte PreviousType = (PreviousContent>>28)&0x0f;

  switch(Command)
    {   
    case CMD_SEND_KAKU:
      TransmitCode(command2event(CMD_KAKU,Par1,Par2),VALUE_ALL);
      break;
      
    case CMD_SEND_KAKU_NEW:
      TransmitCode(command2event(CMD_KAKU_NEW,Par1,Par2),VALUE_ALL);
      break;
      
    case CMD_VARIABLE_INC:
      Par2PortAnalog(Par1, Par2, &y, &x);// y=variabele, x=waarde
      z=UserVar[y-1]+x;
      if(abs(z)<=10000)
        UserVar[y-1]+=x;
      ProcessEvent(AnalogInt2event(UserVar[y-1], y, CMD_VARIABLE_EVENT), VALUE_DIRECTION_INTERNAL, VALUE_SOURCE_VARIABLE, 0, 0);      // verwerk binnengekomen event.
      break;        

    case CMD_VARIABLE_DEC: 
      Par2PortAnalog(Par1, Par2, &y, &x);// y=variabele, x=waarde
      z=UserVar[y-1]-x;
      if(abs(z)<=10000)
        UserVar[y-1]-=x;
      ProcessEvent(AnalogInt2event(UserVar[y-1], y, CMD_VARIABLE_EVENT), VALUE_DIRECTION_INTERNAL, VALUE_SOURCE_VARIABLE, 0, 0);      // verwerk binnengekomen event.
      break;        

    case CMD_VARIABLE_SAVE:   
      if(Par1==0)
        for(x=0;x<USER_VARIABLES_MAX;x++)
          S.UserVar[x]=UserVar[x];
      else
        S.UserVar[Par1-1]=UserVar[Par1-1];            
      SaveSettings();
      break;        
  
    case CMD_VARIABLE_SET:
      Par2PortAnalog(Par1, Par2, &y, &x);// y=variabele, x=waarde
      if(y==0)
        {
        y=1;
        z=USER_VARIABLES_MAX;
        }
      else
        {
        z=y;
        }
      for(y;y<=z;y++)
        {
        UserVar[y-1]=x;
        ProcessEvent(AnalogInt2event(UserVar[y-1], y, CMD_VARIABLE_EVENT), VALUE_DIRECTION_INTERNAL, VALUE_SOURCE_VARIABLE, 0, 0);      // verwerk binnengekomen event.
        }
      break;        
  
  
    case CMD_WIREDANALOG_VARIABLE:
      UserVar[Par1-1]=map(analogRead(PIN_WIRED_IN_1+Par2-1),S.WiredInput_Calibration_IL[Par2-1],S.WiredInput_Calibration_IH[Par2-1],S.WiredInput_Calibration_OL[Par2-1],S.WiredInput_Calibration_OH[Par2-1]);
      ProcessEvent(AnalogInt2event(UserVar[Par1-1], Par1, CMD_VARIABLE_EVENT), VALUE_DIRECTION_INTERNAL, VALUE_SOURCE_VARIABLE, 0, 0);      // verwerk binnengekomen event.
      break;

    case CMD_PULSE_VARIABLE:
      a=0;

      // eerst een keer dit commando uitvoeren voordat de teller gaat lopen.
      attachInterrupt(PULSE_IRQ,PulseCounterISR,FALLING); // IRQ behorende bij PIN_IR_RX_DATA

#if NODO_MEGA
      if(S.Debug==VALUE_ON)
        {        
        Serial.print("# PulseTimeMillis=");Serial.print(PulseTime,DEC);//??? er uit om ruimte te beparen?
        Serial.print(", PulseCount=");Serial.println(PulseCount,DEC);
        }
#endif
      switch(Par2)
        {
        case 0:
          a=0;
          break;
        case 1:
          FORMULA_1;
          break;
        case 2:
          FORMULA_2;
          break;
        case 3:
          FORMULA_3;
          break;
        case 4:
          FORMULA_4;
          break;
        case 5:
          FORMULA_5;
          break;
        case 6:
          FORMULA_6;
          break;
        case 7:
          FORMULA_7;
          break;
        case 8:
          FORMULA_8;
          break;
        default:
          a=0;        
        }        
      if(abs(a)<=10000)
        UserVar[Par1-1]=(int)a;
      ProcessEvent(AnalogInt2event(UserVar[Par1-1], Par1, CMD_VARIABLE_EVENT), VALUE_DIRECTION_INTERNAL, VALUE_SOURCE_VARIABLE, 0, 0);      // verwerk binnengekomen event.
      PulseCount=0;
      break;

    case CMD_VARIABLE_VARIABLE:
      UserVar[Par1-1]=UserVar[Par2-1];
      ProcessEvent(AnalogInt2event(UserVar[Par1-1], Par1, CMD_VARIABLE_EVENT), VALUE_DIRECTION_INTERNAL, VALUE_SOURCE_VARIABLE, 0, 0);      // verwerk binnengekomen event.
      break;        

    case CMD_BREAK_ON_VAR_EQU:
      Par2PortAnalog(Par1, Par2, &y, &x);// y=variabele, x=waarde
      if(UserVar[y-1]==x)
        error=true;
      break;
      
    case CMD_BREAK_ON_VAR_NEQU:
      Par2PortAnalog(Par1, Par2, &y, &x);// y=variabele, x=waarde
      if(UserVar[y-1]!=x)
        error=true;
      break;

    case CMD_BREAK_ON_VAR_MORE:
      Par2PortAnalog(Par1, Par2, &y, &x);// y=variabele, x=waarde
      if(UserVar[y-1]>x)
        error=true;
      break;

    case CMD_BREAK_ON_VAR_LESS:
      Par2PortAnalog(Par1, Par2, &y, &x);// y=variabele, x=waarde
      if(UserVar[y-1]<x)
        error=true;
      break;

    case CMD_BREAK_ON_DAYLIGHT:
      if(Par1==VALUE_ON && (Time.Daylight==2 || Time.Daylight==3))
        error=true;

      if(Par1==VALUE_OFF && (Time.Daylight==0 || Time.Daylight==1 || Time.Daylight==4))
        error=true;
      break;

    case CMD_BREAK_ON_TIME_LATER:
      if((Par1*60+Par2)<(Time.Hour*60+Time.Minutes))
        error=true;
      break;

    case CMD_BREAK_ON_TIME_EARLIER:
      if((Par1*60+Par2)>(Time.Hour*60+Time.Minutes))
        error=true;
      break;

    case CMD_SEND_USEREVENT:
      // Voeg Unit=0 want een UserEvent is ALTIJD voor ALLE Nodo's. Verzend deze vervolgens.
      TransmitCode(command2event(CMD_USEREVENT,Par1,Par2),VALUE_ALL);
      break;

    case CMD_LOCK:
      S.Lock=Par1;
      SaveSettings();
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
      if(Par1==0)
        {
        x=1;
        y=TIMER_MAX;
        }
      else
        {
        x=Par1;
        y=Par1;
        }
      for(x;x<=y;x++)
        TimerSet(x,int(Par2)*60);
        
      break;
      
    case CMD_TIMER_SET_SEC:
      // Par1=timer, Par2=seconden. Timers werken op een resolutie van seconden.            
     if(Par1==0)
        {
        x=1;
        y=TIMER_MAX;
        }
      else
        {
        x=Par1;
        y=Par1;
        }
      for(x;x<=y;x++)
       TimerSet(x,Par2);
      break;

    case CMD_TIMER_RANDOM:
      UserTimer[Par1-1]=millis()+random(Par2)*60000;// Par1=timer, Par2=maximaal aantal minuten
      break;

    case CMD_DELAY:
      Led(BLUE);
      delay(Par1*1000);      
      break;        
      
    case CMD_QUEUE:
      Led(BLUE);     
      if(Par1==VALUE_ON)
        {
        a=(Par2==0)?60:Par2;
        WaitAndQueue(a,false);
        }        
      break;        
      
    case CMD_SEND_EVENT:
      TransmitCode(PreviousContent,Par1);
      break;        

    case CMD_SOUND: 
      Alarm(Par1,Par2);
      break;     
  
    case CMD_WIRED_PULLUP:
      S.WiredInputPullUp[Par1-1]=Par2; // Par1 is de poort[1..], Par2 is de waarde [0..1]
      digitalWrite(A0+PIN_WIRED_IN_1+Par1-1,S.WiredInputPullUp[Par1-1]==VALUE_ON?HIGH:LOW);// Zet de pull-up weerstand van 20K voor analoge ingangen. Analog-0 is gekoppeld aan Digital-14
      SaveSettings();
      break;
                 
    case CMD_WIRED_OUT:
      if(Par1==0)
        {
        x=1;
        y=WIRED_PORTS;
        }
      else
        {
        x=Par1;
        y=Par1;
        }
      for(x;x<=y;x++)
        {
        digitalWrite(PIN_WIRED_OUT_1+x-1,(Par2==VALUE_ON));
        WiredOutputStatus[x-1]=(Par2==VALUE_ON);
        Content=(Content&0xffff00ff)|x<<8;

        PrintEvent(Content,VALUE_SOURCE_WIRED,VALUE_DIRECTION_OUTPUT);
        }
      break;
                         
    case CMD_WAITFREERF: 
      S.WaitFreeRF_Delay=Par1;
      S.WaitFreeRF_Window=Par2;
      SaveSettings();
      break;

//    case CMD_RAWSIGNAL_COPY:
//      if(Par1==VALUE_RF_2_IR)CopySignalRF2IR(Par2);      
//      if(Par1==VALUE_IR_2_RF)CopySignalIR2RF(Par2);
//      break;        

    case CMD_SENDBUSY:
      if(Par1==VALUE_ALL)
        {// All
        S.SendBusy=Par1;
        SaveSettings();
        }
      else
        {// on / off 
        TransmitCode(command2event(CMD_BUSY,Par1,0),VALUE_ALL);
        if(S.SendBusy==VALUE_ALL && Par1==VALUE_OFF)
          {// De SendBusy A;; mode moet worden uitgeschakeld
          S.SendBusy=VALUE_OFF;
          SaveSettings();
          }
        BusyOnSent=(Par1==VALUE_ON);
        }
      break;
            
    case CMD_WAITBUSY:
      if(Par1==VALUE_ALL)
        {
        S.WaitBusy=Par1;
        SaveSettings();
        }
      else
        {
        if(S.WaitBusy==VALUE_ALL)
          S.WaitBusy=VALUE_OFF;
        else
          {
          if(Par1==0)
            Par1=60;
          WaitAndQueue(Par1,true);
          }
        }

      break;

    case CMD_TRANSMIT_IR:
      S.TransmitIR=Par1;
      if(Par2!=0)S.TransmitRepeatIR=Par2;
      SaveSettings();
      break;

    case CMD_TRANSMIT_RF:
      S.TransmitRF=Par1;
      if(Par2!=0)S.TransmitRepeatRF=Par2;
      SaveSettings();
      break;
      
    case CMD_USERPLUGIN:
      #ifdef USER_PLUGIN
        UserPlugin_Command(Par1,Par2);
      #endif
      break;        

    case CMD_WIRED_THRESHOLD:
      Par2PortAnalog(Par1, Par2, &z, &x);// y=port, x=waarde
      S.WiredInputThreshold[z-1]=x;
      SaveSettings();
      break;                  

    case CMD_WIRED_ANALOG_CALIBRATE:
      {
      Par2PortAnalog(Par1, Par2, &z, &x);// y=port, x=waarde
      z--;
            
      y=analogRead(PIN_WIRED_IN_1+z);      
      
      // als exact dezelfde ijkwaarde vorige keer ook al gebruikt, dan een reset van de ijkwaarden
      if(x==S.WiredInput_Calibration_OH[z] || x==S.WiredInput_Calibration_OL[z])
        {
        S.WiredInput_Calibration_IH[z]=y;
        S.WiredInput_Calibration_IL[z]=y;
        S.WiredInput_Calibration_OH[z]=x;
        S.WiredInput_Calibration_OL[z]=x;
        }
      else if(x>=(((S.WiredInput_Calibration_OH[z] - S.WiredInput_Calibration_OL[z]) / 2) + S.WiredInput_Calibration_OL[z]))
        {
        S.WiredInput_Calibration_IH[z]=y;
        S.WiredInput_Calibration_OH[z]=x;
        }
      else
        {
        S.WiredInput_Calibration_IL[z]=y;
        S.WiredInput_Calibration_OL[z]=x;
        }
      SaveSettings();
      break;
      }
      
    case CMD_WIRED_SMITTTRIGGER:
      Par2PortAnalog(Par1, Par2, &z, &x);// y=variabele, x=waarde
      S.WiredInputSmittTrigger[z-1]=x;
      SaveSettings();
      break;                  

    case CMD_STATUS:
      Status(Par1, Par2, false);
      break;

    case CMD_STATUS_SEND:
      if(Par1==0)
        TransmitCode(command2event(CMD_MESSAGE ,S.Unit,MESSAGE_00),VALUE_ALL);        
      else
        Status(Par1, Par2, true);
      break;

    case CMD_UNIT:
      S.Unit=Par1;
      if(Par1>1)
         {
         S.WaitFreeRF_Delay=3 + S.Unit;
         S.WaitFreeRF_Window=3; // 1 eenheid = 100 ms.
         }
      else
         {
         S.WaitFreeRF_Delay=0;
         S.WaitFreeRF_Window=0;
         }
      SaveSettings();
      FactoryEventlist();
      Reset();

    case CMD_REBOOT:
      delay(1000);
      Reset();
      break;        

    case CMD_ANALYSE_SETTINGS:
      S.AnalyseTimeOut=Par1*1000;
      S.AnalyseSharpness=Par2;
      SaveSettings();
      break;

    case CMD_RESET:
      ResetFactory();

    case CMD_EVENTLIST_ERASE:
      if(Par1>EVENTLIST_MAX)
        error=MESSAGE_06;
      else
        {            
        if(Par1==0)
          {
          Led(BLUE);
          for(x=1;x<=EVENTLIST_MAX;x++)
            Eventlist_Write(x,0L,0L);
          Led(RED);
          }
        else
          Eventlist_Write(Par1,0L,0L);
        }
      break;        

    case CMD_WIRED_ANALOG://het kan zijn dat de gebruiker dit intypt. Geen geldig commando of event
      error=MESSAGE_01;
      break;
       
#if NODO_MEGA
    case CMD_DEBUG: 
      S.Debug=Par1;
      SaveSettings();
      break;

    case CMD_SIMULATE_DAY:
      SimulateDay(); 
      break;     

    case CMD_RAWSIGNAL_SAVE:
      PrintTerminal(ProgmemString(Text_07));
      RawSignal.Key=Par1;
      break;                  

    case CMD_RAWSIGNAL_SEND:
      if(Par1!=0)
        {
        if(RawSignalGet(Par1))
          TransmitCode(AnalyzeRawSignal(),VALUE_ALL);
        else
          RaiseMessage(MESSAGE_03);
        }
      else
        TransmitCode(AnalyzeRawSignal(),VALUE_ALL);
      break;        

    case CMD_TRANSMIT_IP:
      S.TransmitIP=Par1;        
      S.HTTP_Pin=Par2;
      SaveSettings();
      break;

    case CMD_FILE_EXECUTE:
      strcpy(TempString,cmd2str(CMD_FILE_EXECUTE));
      strcat(TempString," ");
      strcat(TempString,int2str(Par1));
      ExecuteLine(TempString,Src);
      break;        
#endif

    }
  return error?false:true;
  }

#if NODO_MEGA
 /*******************************************************************************************************\
 * Deze funktie parsed een string en voert de commando uit die daarin voorkomen. Commandos
 * worden gescheiden met een puntkomma.
 \*******************************************************************************************************/
void ExecuteLine(char *Line, byte Port)
  {
  const int MaxCommandLength=40; // maximaal aantal tekens van een commando
  char Command[MaxCommandLength+1]; 
  char TmpStr[INPUT_BUFFER_SIZE+2]; // Twee extra posities i.v.m. afsluitende nul en het prompt teken. regelen met malloc() ???
  int PosCommand;
  int PosLine;
  int L=strlen(Line);
  int w,x,y,z;
  int EventlistWriteLine=0;
  int Error=0,Par1,Par2,Cmd;
  byte State_EventlistWrite=0;
  unsigned long v,event,a; 
  byte SendTo=0; // Als deze waarde ongelijk aan nul, dan wordt het commando niet uitgevoerd maar doorgestuurd naar de andere Nodo

  Led(RED);

  // verwerking van commando's is door gebruiker tijdelijk geblokkeerd door FileWrite commando
  if(FileWriteMode>0)
    {
    if(StringFind(Line,cmd2str(CMD_FILE_WRITE))!=-1)// string gevonden!
      {
      // beëindig de FileWrite modus
      FileWriteMode=0;
      TempLogFile[0]=0;
      return;
      }
    else if(TempLogFile[0]!=0)
      {
      AddFileSDCard(TempLogFile,Line); // Extra logfile op verzoek van gebruiker
      return;
      }
    }

  // geef invoer regel weer 
  strcpy(TmpStr,">");
  strcat(TmpStr,Line);
  PrintTerminal(TmpStr);

  PosCommand=0;
  for(PosLine=0;PosLine<=L && Error==0 ;PosLine++)
    {
    x=Line[PosLine];

    // Comment teken. hierna verder niets meer doen.
    if(x=='!') 
      PosLine=L+1; // ga direct naar einde van de regel.
       
    // als puntkomma (scheidt opdachten) of einde string(0)
    if((x=='!' || x==';' || x==0) && PosCommand>1)
      {
      Command[PosCommand]=0;
      PosCommand=0;
      Cmd=0;
      Par1=0;
      Par2=0;
      v=0;
      
      // string met commando compleet
      
      if(GetArgv(Command,TmpStr,1));
        v=str2int(TmpStr); // als hex event dan is v gevuld met waarde

      if(v==0) // Als geen een hex-event.
        {
        Cmd=str2cmd(TmpStr); // als bestaand commando of event, dan Cmd gevuld met waarde
        
        if(GetArgv(Command,TmpStr,2))
          {
          Par1=str2cmd(TmpStr);
          if(!Par1)
            Par1=str2int(TmpStr);
          }
        
        if(GetArgv(Command,TmpStr,3))
          {
          Par2=str2cmd(TmpStr);
          if(!Par2)
            Par2=str2int(TmpStr);
          }        

        // Hier worden de commando's verwerkt die een afwijkende MMI hebben.
        switch(Cmd)
          {
          // onderstaande commando's worden verwerkt in ExecuteCommand(); Hier vindt alleen omzetting plaats van string naar 32-bit event.
          case CMD_BREAK_ON_VAR_EQU:
          case CMD_BREAK_ON_VAR_LESS:
          case CMD_BREAK_ON_VAR_MORE:
          case CMD_BREAK_ON_VAR_NEQU:
          case CMD_VARIABLE_DEC:
          case CMD_VARIABLE_SET:
          case CMD_VARIABLE_INC:
          case CMD_VARIABLE_EVENT:
          case CMD_WIRED_THRESHOLD:
          case CMD_WIRED_SMITTTRIGGER:
          case CMD_WIRED_ANALOG_CALIBRATE:
            if(GetArgv(Command,TmpStr,3))        
              v=AnalogInt2event(str2AnalogInt(TmpStr), Par1, Cmd);
            break;

          case CMD_EVENTLIST_WRITE:
            if(SendTo!=0)
              {
              v=command2event(CMD_EVENTLIST_WRITE,Par1,0);      // verwerk binnengekomen event.
              }
            else
              {                          
              if(Par1>EVENTLIST_MAX)
                Error=MESSAGE_06;
              else
                {
                EventlistWriteLine=Par1;
                State_EventlistWrite=1;
                }
              }
            break;
            
          case CMD_SEND_KAKU_NEW:
          case CMD_KAKU_NEW:
            {
            if(GetArgv(Command,TmpStr,2))
              v=str2int(TmpStr);
              
            if(v>255)
              {
              v=(v&0x0fffffff) | (((unsigned long)SIGNAL_TYPE_NEWKAKU)<<28); //  // Niet Par1 want NewKAKU kan als enige op de Par1 plaats een 28-bit waarde hebben. Hoogste nible wissen en weer vullen met type NewKAKU        
              v|=(Par2==VALUE_ON)<<4; // Stop on/off commando op juiste plek in NewKAKU code
              }
            else
              {
              v=command2event(Cmd,v,Par2);      // verwerk binnengekomen event.
              Error=CommandError(v);
              }
            break;
            }
                  
          case CMD_SEND_KAKU:
          case CMD_KAKU:
            {
            if(GetArgv(Command,TmpStr,2))
              {
              byte grp=0,c;
              x=0;     // teller die wijst naar het het te behandelen teken
              z=0;     // Home
              w=0;     // Address
              y=false; // Notatiewijze
              
              while((c=tolower(TmpStr[x++]))!=0)
                {
                if(c>='0' && c<='9'){w=w*10;w=w+c-'0';}
                if(c>='a' && c<='p'){z=c-'a';y=true;} // KAKU home A is intern 0
                }
            
              if(y)// notatie [A1..P16]
                {
                if(w==0)
                  {// groep commando is opgegeven: 0=alle adressen
                  grp=true;
                  Par1=z<<4;
                  }
                else
                  Par1= (z<<4) | (w-1);        
                }
              else // absoluut adres [0..255]
                Par1=w; // KAKU adres 1 is intern 0     

              // Parameter-1 bevat [A1..P16]. Nu omgezet naar absolute waarde. grp=groep commando

              if(Par2==VALUE_ON || Par2==VALUE_OFF)
                {
                Par2=(Par2==VALUE_ON) | (grp<<1); // Parameter-2 bevat [On,Off]. Omzetten naar 1,0. tevens op bit-2 het groepcommando zetten.
                v=command2event(Cmd,Par1,Par2);
                }
              else
                Error=MESSAGE_02;
              }
            else
              Error=MESSAGE_02;
              
            break;
            }
            
          case CMD_SEND:
            // als de SendTo is gevuld, dan event versturen naar een andere Nodo en niet zelf uitvoeren
            // Protocol bestaat uit de volgende stappen:
            // 1. Master nodo verzendt een "Delay 60,On" naar de Slave nodo. Slave gaat in de Hold&Queue mode staan
            // 2. Master nodo verzendt commandos uit de regel, slave stopt deze in de queue
            // 3. Als alle commandos verzonden, dan stuurt de master weer een "Delay 0" om de slave uit de Hold&Queue mode te halen.

            SendTo=Par1;
            a=((command2event(CMD_QUEUE,VALUE_ON,0))&0xf0ffffff) | ((unsigned long)SendTo<<24);
            TransmitCode(a,VALUE_SOURCE_RF); // SendTo: Stap-1
            delay(500);
            break;

          case CMD_PASSWORD:
            {
            TmpStr[0]=0;
            GetArgv(Command,TmpStr,2);
            TmpStr[24]=0; // voor geval de string te lang is.
            strcpy(S.Password,TmpStr);
            SaveSettings();
            break;
            }  

          case CMD_ID:
            {
            TmpStr[0]=0;
            GetArgv(Command,TmpStr,2);
            TmpStr[9]=0; // voor geval de string te lang is.
            strcpy(S.ID,TmpStr);
            SaveSettings();
            break;
            }  

          case CMD_FILE_LOG:
            if(GetArgv(Command,TmpStr,2))
              {
              strcat(TmpStr,".dat");
              strcpy(TempLogFile,TmpStr);
              }
            else
              TempLogFile[0]=0;
            break;
          
          case CMD_FILE_ERASE:      
            if(GetArgv(Command,TmpStr,2))
              {
              strcat(TmpStr,".dat");

              SelectSD(true);
              SD.remove(TmpStr);
              SelectSD(false);
              }
            break;
    
          case CMD_FILE_GET_HTTP:
            {
            if(GetArgv(Command,TempString,2))
              {
              Led(BLUE);
              GetHTTPFile(TempString);
              }
            else
              Error=MESSAGE_02;            
            break; 
            }
            
          case CMD_FILE_SHOW:
            {
            if(GetArgv(Command,TmpStr,2))
              {
              Led(BLUE);
              strcat(TmpStr,".dat");
              SelectSD(true);
              File dataFile=SD.open(TmpStr);
              if(dataFile) 
                {
                PrintTerminal(ProgmemString(Text_22));
                y=0;       
                while(dataFile.available())
                  {
                  x=dataFile.read();
                  if(isprint(x) && y<INPUT_BUFFER_SIZE)
                    {
                    TmpStr[y++]=x;
                    }
                  else
                    {
                    TmpStr[y]=0;
                    y=0;
                    PrintTerminal(TmpStr);
                    }
                  }
                dataFile.close();
                PrintTerminal(ProgmemString(Text_22));
                }  
              else 
                TransmitCode(command2event(CMD_MESSAGE ,MESSAGE_03,0),VALUE_ALL);
      
              SelectSD(false);
              }
            break;
            }
    
          case CMD_FILE_EXECUTE:
            {
            if(GetArgv(Command,TmpStr,2))
              {
              if(State_EventlistWrite!=0)
                {
                // Als er een EventlistWrite actief is, dan hoeft het commando niet uitgevoerd te worden. Alleen het event v moet
                // worden gevuld zodat deze kan worden weggeschreven in de Eventlist.
                v=command2event(Cmd,Par1,Par2);
                Error=CommandError(v);
                }

              else // Commando uitvoeren heeft alleen zin er geen eventlistwrite commando actief is
                {                
                strcat(TmpStr,".dat");

                // zet (eventueel) de extra logging aan
                GetArgv(Command,TempString,3);
                strcat(TempString,".dat");
                TempString[14]=0; // voor het geval de gebruiker een te lange naam heeft ingegeven
                strcpy(TempLogFile,TempString);
                
                SelectSD(true);
                File dataFile=SD.open(TmpStr);
                if(dataFile) 
                  {
                  y=0;       
                  while(dataFile.available())
                    {
                    x=dataFile.read();
                    if(isprint(x) && y<INPUT_BUFFER_SIZE)
                      TmpStr[y++]=x;
                    else
                      {
                      TmpStr[y]=0;
                      y=0;
                      SelectSD(false);
                      ExecuteLine(TmpStr,VALUE_SOURCE_FILE);
                      SelectSD(true);
                      }
                    }
                  dataFile.close();
                  }  
                else
                  {
                  TransmitCode(command2event(CMD_MESSAGE ,MESSAGE_03,0),VALUE_ALL);
                  }
                SelectSD(false);
                }
              }
            TempLogFile[0]=0;
            break;
            }
    
          case CMD_EVENTLIST_SHOW:
            if(Par1>EVENTLIST_MAX)
              Error=MESSAGE_06;
            else
              {            
              PrintTerminal(ProgmemString(Text_22));
              if(Par1==0)
                {
                for(x=1;x<=EVENTLIST_MAX;x++)
                  {
                  if(EventlistEntry2str(x,0,TempString, false))
                    PrintTerminal(TempString);
                  }
                }
              else
                {
                EventlistEntry2str(Par1,0,TempString, false);
                PrintTerminal(TempString);
                }
              PrintTerminal(ProgmemString(Text_22));
              }
            break;

          case CMD_EVENTGHOST_SERVER:
            if(Par1==VALUE_AUTO)
              { 
              if(Par2==VALUE_ON)
                S.AutoSaveEventGhostIP=VALUE_AUTO;  // Automatisch IP adres opslaan na ontvangst van een EG event of niet.
              else
                S.AutoSaveEventGhostIP=0;
              }
            else
              {
              if(GetArgv(Command,TmpStr,2))
                if(!str2ip(TmpStr,S.EventGhostServer_IP))
                  Error=MESSAGE_02;
              }
            SaveSettings();
            break;
            
          case CMD_NODO_IP:
            if(GetArgv(Command,TmpStr,2))
              if(!str2ip(TmpStr,S.Nodo_IP))
                Error=MESSAGE_02;
            SaveSettings();
            break;
            
          case CMD_CLIENT_IP:
            if(GetArgv(Command,TmpStr,2))
              if(!str2ip(TmpStr,S.Client_IP))
                Error=MESSAGE_02;
            SaveSettings();
            break;
            
          case CMD_SUBNET:
            if(GetArgv(Command,TmpStr,2))
              if(!str2ip(TmpStr,S.Subnet))
                Error=MESSAGE_02;
            SaveSettings();
            break;
            
          case CMD_DNS_SERVER:
            if(GetArgv(Command,TmpStr,2))
              if(!str2ip(TmpStr,S.DnsServer))
                Error=MESSAGE_02;
            SaveSettings();
            break;
            
          case CMD_GATEWAY:
            if(GetArgv(Command,TmpStr,2))
              if(!str2ip(TmpStr,S.Gateway))
                Error=MESSAGE_02;
            SaveSettings();
            break;
            
          case CMD_EVENTLIST_FILE:
            Led(BLUE);
            if(GetArgv(Command,TmpStr,2))
              {
              strcat(TmpStr,".dat");
              if(!SaveEventlistSDCard(TmpStr))
                TransmitCode(command2event(CMD_MESSAGE ,MESSAGE_03,0),VALUE_ALL);// geen RaiseMessage() anders weer poging om te loggen naar SDCard ==> oneindige loop
              }
            break;

          case CMD_FILE_LIST:
            if(!FileList())
              Error=MESSAGE_03;
            break;

          case CMD_FILE_WRITE:
            if(GetArgv(Command,TmpStr,2))
              {
              strcat(TmpStr,".dat");
              strcpy(TempLogFile,TmpStr);
              FileWriteMode=60;
              }
            else
              Error=MESSAGE_02;
            break;

          case CMD_HTTP_REQUEST:
            // zoek in de regel waar de string met het http request begint.
            x=StringFind(Line,cmd2str(CMD_HTTP_REQUEST))+strlen(cmd2str(CMD_HTTP_REQUEST));
            while(Line[x]==32)x++;
            strcpy(S.HTTPRequest,&Line[0]+x);
            SaveSettings(); 
            break;

          case CMD_PORT_SERVER:
            {
            if(GetArgv(Command,TmpStr,2))
              {
              S.PortServer=str2int(TmpStr);
              SaveSettings();
              }
            break;
            }  

          case CMD_PORT_CLIENT:
            {
            if(GetArgv(Command,TmpStr,2))
              {
              S.PortClient=str2int(TmpStr);
              SaveSettings();
              }
            break;
            }  

          default:
            {              
            // standaard commando volgens gewone syntax
            v=command2event(Cmd,Par1,Par2);            
            Error=CommandError(v);
            }
          }
        }
          
      if(v && Error==0)
        {
        if(State_EventlistWrite==0)// Gewoon uitvoeren
          {
          if(SendTo==0)
            ProcessEvent(v,VALUE_DIRECTION_INPUT,Port,0,0);      // verwerk binnengekomen event.
          else
            {
            if(NodoType(v))
              v=(v&0xf0ffffff)|(unsigned long)SendTo<<24;// Commando/Event unit nummer van de bestemming toewijzen, behalve als het een HEX-event is.
            TransmitCode(v,VALUE_SOURCE_RF);// SendTo: Stap-2. Voer commando niet zelf uit, maar stuur deze naar de Nodo zoals opgegeven in de SendTo. 
            delay(100);//??? kleine pauze om de ontvangende Nodo de tijd te geven event in de queue te kunnen stoppen
            }
          continue;
          }
  
        if(State_EventlistWrite==2)
          {
          a=v;
          if(!Eventlist_Write(EventlistWriteLine,event,a))
            {
            RaiseMessage(MESSAGE_06);    
            return;
            }
          State_EventlistWrite=0;
          continue;
          }
  
        if(State_EventlistWrite==1)
          {
          event=v;
          State_EventlistWrite=2;
          }
        }
        
      if(Error) // er heeft zich een fout voorgedaan
        {
        strcpy(TmpStr,Command);
        strcat(TmpStr, " ?");
        PrintTerminal(TmpStr);
        RaiseMessage(Error);
        Line[0]=0;
        }
      }
      
    // printbare tekens toevoegen aan commando zolang er nog ruimte is in de string
    if(isprint(x) && x!=';' && PosCommand<MaxCommandLength)
      Command[PosCommand++]=x;      
    }    

  if(SendTo!=0)// Geef de slave-nodo de opdracht te stoppen met de WaitAndQueue routine en de queue te verwerken.
    {
    TransmitCode(((command2event(CMD_QUEUE,VALUE_OFF,0))&0xf0ffffff) | ((unsigned long)SendTo<<24),VALUE_SOURCE_RF);
    WaitAndQueue(60,true);    // wacht totdat zolang er nog Nodo's bezig zijn met verwerking
    }
  }
#endif
  
  
