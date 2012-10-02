

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
  if(x!=Settings.Unit && x!=0)
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
byte Commanderror(unsigned long Content)
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
    case CMD_LOCK:
    case CMD_WAITBUSY:
    case CMD_TRANSMIT_QUEUE:
    case CMD_EVENTLIST_ERASE:
    case CMD_RESET:
    case CMD_RAWSIGNAL_SAVE:
    case CMD_RAWSIGNAL:
    case CMD_MESSAGE :
    case CMD_REBOOT:
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
    case CMD_DELAY:
    case CMD_SOUND: 
    case CMD_USEREVENT:
    case CMD_SEND_USEREVENT:
    case CMD_KAKU:
    case CMD_SEND_KAKU:
      return false;
 
    case CMD_SEND_KAKU_NEW:
    case CMD_KAKU_NEW:    
      if(Par2==VALUE_ON || Par2==VALUE_OFF || Par2<=16)return false;
      return MESSAGE_02;

    case CMD_RAWSIGNAL_SEND:    
      if(Par2==VALUE_SOURCE_RF || Par2==VALUE_SOURCE_IR || Par2==0)return false;
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

    case CMD_WIRED_ANALOG_CALIBRATE_HIGH:
    case CMD_WIRED_ANALOG_CALIBRATE_LOW:
    case CMD_WIRED_ANALOG:
    case CMD_WIRED_THRESHOLD:
    case CMD_WIRED_SMITTTRIGGER:
      Par2PortAnalog(Par1, Par2, &x, &y);// x=wired , y=waarde
      if(x<1 || x>WIRED_PORTS)return MESSAGE_02;
      return false;

    case CMD_UNIT:
    case CMD_BOOT_EVENT:
      if(Par1<1 || Par1>UNIT_MAX)return MESSAGE_02;
      return false;

    case CMD_ANALYSE_SETTINGS:
      if(Par1<1 || Par1>50)return MESSAGE_02;
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
    case CMD_BREAK_ON_TIME_LATER:
    case CMD_BREAK_ON_TIME_EARLIER:
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

    case CMD_VARIABLE_WIREDANALOG:
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

    case CMD_SENDBUSY:
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
        case VALUE_SOURCE_TELNET:
        case VALUE_SOURCE_SERIAL:
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
    case CMD_BUSY:
    case CMD_BREAK_ON_DAYLIGHT:
    case CMD_WAITFREERF:
      if(Par1!=VALUE_OFF && Par1!=VALUE_ON)return MESSAGE_02;
      return false;

    case CMD_OUTPUT:
      if(Par1!=VALUE_SOURCE_IR && Par1!=VALUE_SOURCE_RF && Par1!=VALUE_SOURCE_HTTP)return MESSAGE_02;
      if(Par2!=VALUE_OFF && Par2!=VALUE_ON)return MESSAGE_02;
      return false;

#if NODO_MEGA
    case CMD_LOG:
    case CMD_DEBUG:
    case CMD_ECHO:
      if(Par1!=VALUE_OFF && Par1!=VALUE_ON)return MESSAGE_02;
      return false;
    
    case CMD_SIMULATE_DAY:
      return false;
      
    case CMD_FILE_EXECUTE:
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
  char *TempString=(char*)malloc(25);

  byte error        = false;
  byte Command      = (Content>>16)&0xff;
  byte Par1         = (Content>>8)&0xff;
  byte Par2         = Content&0xff;
  byte Type         = (Content>>28)&0x0f;
  byte PreviousType = (PreviousContent>>28)&0x0f;

  switch(Command)
    {   
    case CMD_SEND_KAKU:
      TransmitCode(command2event(Settings.Unit, CMD_KAKU,Par1,Par2),VALUE_ALL);
      break;
      
    case CMD_SEND_KAKU_NEW:
      TransmitCode(command2event(Settings.Unit, CMD_KAKU_NEW,Par1,Par2),VALUE_ALL);
      break;
      
    case CMD_VARIABLE_INC:
      Par2PortAnalog(Par1, Par2, &y, &x);// y=variabele, x=waarde
      z=UserVar[y-1]+x;
      if(abs(z)<=10000)
        UserVar[y-1]+=x;
      ProcessEvent2(AnalogInt2event(UserVar[y-1], y, CMD_VARIABLE_EVENT), VALUE_DIRECTION_INTERNAL, VALUE_SOURCE_VARIABLE, 0, 0);      // verwerk binnengekomen event.
      break;        

    case CMD_VARIABLE_DEC: 
      Par2PortAnalog(Par1, Par2, &y, &x);// y=variabele, x=waarde
      z=UserVar[y-1]-x;
      if(abs(z)<=10000)
        UserVar[y-1]-=x;
      ProcessEvent2(AnalogInt2event(UserVar[y-1], y, CMD_VARIABLE_EVENT), VALUE_DIRECTION_INTERNAL, VALUE_SOURCE_VARIABLE, 0, 0);      // verwerk binnengekomen event.
      break;        

    case CMD_VARIABLE_SAVE:   
      if(Par1==0)
        for(x=0;x<USER_VARIABLES_MAX;x++)
          Settings.UserVar[x]=UserVar[x];
      else
        Settings.UserVar[Par1-1]=UserVar[Par1-1];            
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
        ProcessEvent2(AnalogInt2event(UserVar[y-1], y, CMD_VARIABLE_EVENT), VALUE_DIRECTION_INTERNAL, VALUE_SOURCE_VARIABLE, 0, 0);      // verwerk binnengekomen event.
        }
      break;         
  
    case CMD_VARIABLE_WIREDANALOG:
      UserVar[Par1-1]=map(analogRead(PIN_WIRED_IN_1+Par2-1),Settings.WiredInput_Calibration_IL[Par2-1],Settings.WiredInput_Calibration_IH[Par2-1],Settings.WiredInput_Calibration_OL[Par2-1],Settings.WiredInput_Calibration_OH[Par2-1]);
      ProcessEvent2(AnalogInt2event(UserVar[Par1-1], Par1, CMD_VARIABLE_EVENT), VALUE_DIRECTION_INTERNAL, VALUE_SOURCE_VARIABLE, 0, 0);      // verwerk binnengekomen event.
      break;

    case CMD_PULSE_VARIABLE:
      a=0;
      // eerst een keer dit commando uitvoeren voordat de teller gaat lopen.
      bitWrite(HW_Config,HW_IR_PULSE,true);

      attachInterrupt(PULSE_IRQ,PulseCounterISR,FALLING); // IRQ behorende bij PIN_IR_RX_DATA

#if NODO_MEGA
      if(Settings.Debug==VALUE_ON)
        {        
        Serial.print("# PulseTimeMillis=");Serial.print(PulseTime,DEC);
        Serial.print(", PulseCount=");Serial.println(PulseCount,DEC);
        }
#endif
      switch(Par2)
        {
        case 0:
          a=0;
          break;
        case 1:
          FORMULA_1
          break;
        case 2:
          FORMULA_2
          break;
        case 3:
          FORMULA_3
          break;
        case 4:
          FORMULA_4
          break;
        case 5:
          FORMULA_5
          break;
        case 6:
          FORMULA_6
          break;
        case 7:
          FORMULA_7
          break;
        case 8:
          FORMULA_8
          break;
        default:
          a=0;        
        }        
      if(abs(a)<=10000)
        UserVar[Par1-1]=(int)a;
      ProcessEvent2(AnalogInt2event(UserVar[Par1-1], Par1, CMD_VARIABLE_EVENT), VALUE_DIRECTION_INTERNAL, VALUE_SOURCE_VARIABLE, 0, 0);      // verwerk binnengekomen event.
      break;

    case CMD_VARIABLE_VARIABLE:
      UserVar[Par1-1]=UserVar[Par2-1];
      ProcessEvent2(AnalogInt2event(UserVar[Par1-1], Par1, CMD_VARIABLE_EVENT), VALUE_DIRECTION_INTERNAL, VALUE_SOURCE_VARIABLE, 0, 0);      // verwerk binnengekomen event.
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
      TransmitCode(command2event(Settings.Unit, CMD_USEREVENT,Par1,Par2),VALUE_ALL);
      break;

    case CMD_LOCK:
      // In de bits van Par1 en Par2 zit een sleutel die gegenereerd is uit het wachtwoord van de Nodo die het commando verstuurd heeft.
      a=Content&0x7fff;// Zet de lock met de bits 0 t/m 15
      if(Content&0x8000) // On/Off bevindt zich in bit nr. 15
        {// Als verzoek om inschakelen dan Lock waarde vullen
        if(Settings.Lock==0)// mits niet al gelocked.
          {
          Settings.Lock=a; 
          }
        else
          {
          error=MESSAGE_10;
          }
        }
      else
        {// Verzoek om uitschakelen
        if(Settings.Lock==a || Settings.Lock==0)// als lock code overeen komt of nog niet gevuld
          {
          Settings.Lock=0;
          }
        else
          {
          error=MESSAGE_10;
          }
        }        
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
      
      if(Par2==VALUE_OFF)    
        delay(Par1*1000);      
      else      
        WaitAndQueue(Par1,false,0);
      break;        
      
    case CMD_SEND_EVENT:
      TransmitCode(PreviousContent,Par1);
      break;        

    case CMD_SOUND: 
      Alarm(Par1,Par2);
      break;     
  
    case CMD_WIRED_PULLUP:
      Settings.WiredInputPullUp[Par1-1]=Par2; // Par1 is de poort[1..]
      if(Settings.WiredInputPullUp[x]==VALUE_ON)
        pinMode(A0+PIN_WIRED_IN_1+Par1-1,INPUT_PULLUP);
      else
        pinMode(A0+PIN_WIRED_IN_1+Par1-1,INPUT);

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

        PrintEvent(Content,VALUE_DIRECTION_OUTPUT, VALUE_SOURCE_WIRED);
        }
      break;
                         
    case CMD_WAITFREERF: 
      Settings.WaitFreeRF=Par1;
      SaveSettings();
      break;

    case CMD_SENDBUSY:
      if(Par1==VALUE_ALL)
        {// All
        Settings.SendBusy=Par1;
        SaveSettings();
        }
      else
        {// on / off 
        TransmitCode(command2event(Settings.Unit, CMD_BUSY,Par1,0),VALUE_ALL);
        if(Settings.SendBusy==VALUE_ALL && Par1==VALUE_OFF)
          {// De SendBusy mode moet worden uitgeschakeld
          Settings.SendBusy=VALUE_OFF;
          SaveSettings();
          }
        }
      break;
            
    case CMD_WAITBUSY:
      if(Par2==VALUE_ALL)
        {
        Settings.WaitBusyAll=Par1;
        SaveSettings();
        }
      else
        NodoBusy(0L, Par1);
      break;

    case CMD_OUTPUT:
      switch(Par1)
        {
        case VALUE_SOURCE_IR:
          Settings.TransmitIR=Par2;
          break;       
        case VALUE_SOURCE_RF:
          Settings.TransmitRF=Par2;
          break;       

        #if NODO_MEGA
        case VALUE_SOURCE_HTTP:
          Settings.TransmitIP=Par2;        
          break;       
        #endif
        }
      SaveSettings();
      break;
      
    case CMD_USERPLUGIN:
      #if USER_PLUGIN
        UserPlugin_Command(Par1,Par2);
      #endif
      break;        

    case CMD_WIRED_THRESHOLD:
      Par2PortAnalog(Par1, Par2, &z, &x);// y=port, x=waarde
      Settings.WiredInputThreshold[z-1]=x;
      SaveSettings();
      break;                  

    case CMD_WIRED_ANALOG_CALIBRATE_HIGH:
      {
      Par2PortAnalog(Par1, Par2, &z, &x);// y=port, x=waarde
      z--;
      y=analogRead(PIN_WIRED_IN_1+z);      
      Settings.WiredInput_Calibration_IH[z]=y;
      Settings.WiredInput_Calibration_OH[z]=x;
      SaveSettings();
      break;
      }

    case CMD_WIRED_ANALOG_CALIBRATE_LOW:
      {
      Par2PortAnalog(Par1, Par2, &z, &x);// y=port, x=waarde
      z--;
      y=analogRead(PIN_WIRED_IN_1+z);      
      Settings.WiredInput_Calibration_IL[z]=y;
      Settings.WiredInput_Calibration_OL[z]=x;
      SaveSettings();
      break;
      }
      
    case CMD_WIRED_SMITTTRIGGER:
      Par2PortAnalog(Par1, Par2, &z, &x);// y=variabele, x=waarde
      Settings.WiredInputSmittTrigger[z-1]=x;
      SaveSettings();
      break;                  

    case CMD_STATUS:
      // Het commando [Status] verzendt de status naar de bron waar het verzoek van is ontvangen. Serial en TelNet worden beschouwd als dezelfde bron.
      // Als het door de gebruiker is verzocht om logging naar een file te doen, dan wordt de output NIET als events verzonden.

      #if NODO_MEGA
      if(TempLogFile[0]!=0 || Src==VALUE_SOURCE_SERIAL || Src==VALUE_SOURCE_TELNET)
      #else
      if(Src==VALUE_SOURCE_SERIAL || Src==VALUE_SOURCE_TELNET)
      #endif

        Status(Par1, Par2, false);
      else
        {
        if(Par1==0)
          TransmitCode(command2event(Settings.Unit, CMD_MESSAGE ,Settings.Unit,MESSAGE_00),Src);        
        else
          Status(Par1, Par2, true);
        }
      break;

    case CMD_UNIT:
      if(Busy.Sent)
        TransmitCode(command2event(Settings.Unit,CMD_BUSY,VALUE_OFF,0),VALUE_ALL);
      Settings.Unit=Par1;
      SaveSettings();
      FactoryEventlist();      
      Reset();

    case CMD_REBOOT:
      delay(1000);
      if(Busy.Sent)
        TransmitCode(command2event(Settings.Unit,CMD_BUSY,VALUE_OFF,0),VALUE_ALL);
      Reset();
      break;        

    case CMD_ANALYSE_SETTINGS:
      Settings.AnalyseTimeOut=Par1*1000;
      Settings.AnalyseSharpness=Par2;
      SaveSettings();
      break;

    case CMD_RESET:
      if(Busy.Sent)
        TransmitCode(command2event(Settings.Unit,CMD_BUSY,VALUE_OFF,0),VALUE_ALL);
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
          }
        else
          Eventlist_Write(Par1,0L,0L);
        }
      break;        

    case CMD_WIRED_ANALOG://het kan zijn dat de gebruiker dit intypt. Geen geldig commando of event
      error=MESSAGE_01;
      break;
       
#if NODO_MEGA
    case CMD_ECHO:
      if(Src==VALUE_SOURCE_TELNET) 
        Settings.EchoTelnet=Par1;
      if(Src==VALUE_SOURCE_SERIAL) 
        Settings.EchoSerial=Par1;        
      SaveSettings();
      break;

    case CMD_DEBUG: 
      Settings.Debug=Par1;
      SaveSettings();
      break;

    case CMD_LOG: 
      Settings.Log=Par1;
      SaveSettings();
      break;

    case CMD_SIMULATE_DAY:
      SimulateDay(); 
      break;     

    case CMD_RAWSIGNAL_SAVE:
      Led(BLUE);
      PrintTerminal(ProgmemString(Text_07));
      RawSignal.Key=Par1;
      break;              

    case CMD_RAWSIGNAL_SEND:
      if(Par1!=0)
        {
        if(RawSignalGet(Par1))
          {
          x=VALUE_ALL;
          if(Par2==VALUE_SOURCE_RF || Par2==VALUE_SOURCE_IR)
            x=Par2;
          TransmitCode(AnalyzeRawSignal(),x);
          }
        else
          error=MESSAGE_03;
        }
      else
        TransmitCode(AnalyzeRawSignal(),VALUE_ALL);
      break;        


    case CMD_FILE_EXECUTE:
      strcpy(TempString,cmd2str(CMD_FILE_EXECUTE));
      strcat(TempString," ");
      strcat(TempString,int2str(Par1));
      ExecuteLine(TempString,Src);
      break;        
#endif

    }
  free(TempString);
  return error?false:true;
  }

#if NODO_MEGA
 /*******************************************************************************************************\
 * Deze funktie parsed een string en voert de commando uit die daarin voorkomen. Commandos
 * worden gescheiden met een puntkomma.
 \*******************************************************************************************************/
int ExecuteLine(char *Line, byte Port)
  {
  const int MaxCommandLength=40; // maximaal aantal tekens van een commando
  char Command[MaxCommandLength+1]; 
  char *TmpStr1=(char*)malloc(MaxCommandLength+1);
  char *TmpStr2=(char*)malloc(INPUT_BUFFER_SIZE+2);
  int PosCommand;
  int PosLine;
  int L=strlen(Line);
  int w,x,y,z;
  int EventlistWriteLine=0;
  int error=0,Par1,Par2,Cmd;
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
      }

    if(TempLogFile[0]!=0)
      {
      AddFileSDCard(TempLogFile,Line); // Extra logfile op verzoek van gebruiker
      }
    }
  else
    {
    PosCommand=0;
    for(PosLine=0;PosLine<=L && error==0 ;PosLine++)
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
        
        if(GetArgv(Command,TmpStr1,1));
          v=str2int(TmpStr1); // als hex event dan is v gevuld met waarde
  
        if(v==0) // Als geen een hex-event.
          {
          Cmd=str2cmd(TmpStr1); // als bestaand commando of event, dan Cmd gevuld met waarde
          
          if(GetArgv(Command,TmpStr1,2))
            {
            Par1=str2cmd(TmpStr1);
            if(!Par1)
              Par1=str2int(TmpStr1);
            }
          
          if(GetArgv(Command,TmpStr1,3))
            {
            Par2=str2cmd(TmpStr1);
            if(!Par2)
              Par2=str2int(TmpStr1);
            }        
  
          // Geef aan de WebApp en andere Nodo's te kennen dat deze Nodo bezig is met verwerking
          // Maar niet met het SendTo commando, ander is de Slave niet bereikbaar!
          if(Cmd!=CMD_SEND && Settings.SendBusy==VALUE_ALL && !Busy.Sent)
            {
            TransmitCode(command2event(Settings.Unit,CMD_BUSY,VALUE_ON,0),VALUE_ALL);
            Busy.Sent=true;
            }

          // Hier worden de commando's verwerkt die een afwijkende MMI hebben.
          switch(Cmd)
            {
            case CMD_LOCK: // Hier wordt de lock code o.b.v. het wachtwoord ingesteld. Alleen van toepassing voor de Mega.
              a=0L;
              for(x=0;x<strlen(Settings.Password);x++)
                {
                a=a<<5;
                a^=Settings.Password[x];
                }
              a&=0x7fff;// 15-bits pincode uit wachtwoord samengesteld. 16e bit is lock aan/uit.
              if(Par1==VALUE_ON)
                a |= (1<<15);
               v=command2event(Settings.Unit,CMD_LOCK,(a>>8)&0xff,a&0xff);
              break;
  
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
            case CMD_WIRED_ANALOG_CALIBRATE_HIGH:
            case CMD_WIRED_ANALOG_CALIBRATE_LOW:
              if(GetArgv(Command,TmpStr1,3))
                v=AnalogInt2event(str2AnalogInt(TmpStr1), Par1, Cmd);
              break;
  
            case CMD_EVENTLIST_WRITE:
              if(SendTo!=0)
                {
                v=command2event(Settings.Unit, CMD_EVENTLIST_WRITE,Par1,0);      // verwerk binnengekomen event.
                }
              else
                {                          
                if(Par1>EVENTLIST_MAX)
                  error=MESSAGE_06;
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
              if(GetArgv(Command,TmpStr1,2))
                v=str2int(TmpStr1);
                
              if(v>255)
                {
                v=(v&0x0fffffff) | (((unsigned long)SIGNAL_TYPE_NEWKAKU)<<28); //  // Niet Par1 want NewKAKU kan als enige op de Par1 plaats een 28-bit waarde hebben. Hoogste nible wissen en weer vullen met type NewKAKU        
                v|=(Par2==VALUE_ON)<<4; // Stop on/off commando op juiste plek in NewKAKU code
                }
              else
                {
                v=command2event(Settings.Unit, Cmd,v,Par2);      // verwerk binnengekomen event.
                error=Commanderror(v);
                }
              break;
              }
                    
            case CMD_SEND_KAKU:
            case CMD_KAKU:
              {
              if(GetArgv(Command,TmpStr1,2))
                {
                byte grp=0,c;
                x=0;     // teller die wijst naar het het te behandelen teken
                z=0;     // Home
                w=0;     // Address
                y=false; // Notatiewijze
                
                while((c=tolower(TmpStr1[x++]))!=0)
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
                  v=command2event(Settings.Unit, Cmd,Par1,Par2);
                  }
                else
                  error=MESSAGE_02;
                }
              else
                error=MESSAGE_02;
                
              break;
              }
              
            case CMD_SEND:
              // als de SendTo is gevuld, dan event versturen naar een andere Nodo en niet zelf uitvoeren
              SendTo=Par1;             
              break;
  
            case CMD_PASSWORD:
              {
              TmpStr1[0]=0;
              GetArgv(Command,TmpStr1,2);
              TmpStr1[24]=0; // voor geval de string te lang is.
              strcpy(Settings.Password,TmpStr1);
  
              // Als een lock actief, dan lock op basis van nieuwe password instellen
              if(Settings.Lock)
                {
                a=0L;
                for(x=0;x<strlen(Settings.Password);x++)
                  {
                  a=a<<5;
                  a^=Settings.Password[x];
                  }
                Settings.Lock=a&0x7fff;
                }
              SaveSettings();
  
              break;
              }  
  
            case CMD_ID:
              {
              TmpStr1[0]=0;
              GetArgv(Command,TmpStr1,2);
              TmpStr1[9]=0; // voor geval de string te lang is.
              strcpy(Settings.ID,TmpStr1);
              SaveSettings();
              break;
              }  
  
            case CMD_FILE_LOG:
              if(GetArgv(Command,TmpStr1,2))
                {
                strcat(TmpStr1,".dat");
                strcpy(TempLogFile,TmpStr1);
                }
              else
                TempLogFile[0]=0;
              break;
            
            case CMD_FILE_ERASE:      
              if(GetArgv(Command,TmpStr1,2))
                {
                strcat(TmpStr1,".dat");
  
                SelectSD(true);
                SD.remove(TmpStr1);
                SelectSD(false);
                }
              break;
      
            case CMD_FILE_GET_HTTP:
              {
              if(GetArgv(Command,TmpStr1,2))
                {
                Led(BLUE);
                GetHTTPFile(TmpStr1);
                }
              else
                error=MESSAGE_02;            
              break; 
              }
              
            case CMD_FILE_SHOW:
              {
              char FileName[13];
              TmpStr1[8]=0;// voor de zekerheid te lange filename afkappen
              if(GetArgv(Command,FileName,2))
                {
                Led(BLUE);
                strcat(FileName,".dat");
                SelectSD(true);
                File dataFile=SD.open(FileName);
                if(dataFile) 
                  {
                  SelectSD(false);
                  PrintTerminal(ProgmemString(Text_22));
                  SelectSD(true);

                  // Als de file groter is dan 10K, dan alleen laatste stuk laten zien
                  if(Par2 != VALUE_ALL)
                    {
                    a=dataFile.size();
                    if(a>10000)
                      {
                      w=dataFile.seek(a-10000UL);                    
                      while(dataFile.available() && isprint(dataFile.read())); // ga naar eerstvolgende nieuwe regel
                      SelectSD(false);
                      PrintTerminal(ProgmemString(Text_09));
                      SelectSD(true);
                      }
                    }

                  TmpStr2[0]=0;
                  y=0;       
                  while(dataFile.available())
                    {
                    x=dataFile.read();
                    if(isprint(x) && y<INPUT_BUFFER_SIZE)
                      {
                      TmpStr2[y++]=x;
                      }
                    else
                      {
                      TmpStr2[y]=0;
                      y=0;
                      SelectSD(false);
                      PrintTerminal(TmpStr2);
                      SelectSD(true);
                      }
                    }
                  dataFile.close();
                  SelectSD(false);
                  PrintTerminal(ProgmemString(Text_22));
                  }  
                else
                  {
                  SelectSD(false);
                  TransmitCode(command2event(Settings.Unit, CMD_MESSAGE, Settings.Unit, MESSAGE_03),VALUE_ALL);      
                  }
                }
              break;
              }
      
            case CMD_FILE_EXECUTE:
              {
              char FileName[13];
              TmpStr1[8]=0;// voor de zekerheid te lange filename afkappen
              if(GetArgv(Command,FileName,2))
                {
                if(State_EventlistWrite!=0)
                  {
                  // Als er een EventlistWrite actief is, dan hoeft het commando niet uitgevoerd te worden. Alleen het event v moet
                  // worden gevuld zodat deze kan worden weggeschreven in de Eventlist.
                  v=command2event(Settings.Unit, Cmd,Par1,Par2);
                  error=Commanderror(v);
                  }
  
                else // Commando uitvoeren heeft alleen zin er geen eventlistwrite commando actief is
                  {
                  FileExecute(FileName);//??? error afvangen als niet uitvoerbaar?                
                  // TempLogFile[0]=0;//??? waarom zou deze leeg gemaakt moeten worden? Kan weg?
                  }
                }
              break;
              }
      
            case CMD_EVENTLIST_SHOW:
              if(Par1>EVENTLIST_MAX)
                error=MESSAGE_06;
              else
                {            
                PrintTerminal(ProgmemString(Text_22));
                if(Par1==0)
                  {
                  for(x=1;x<=EVENTLIST_MAX;x++)
                    {
                    if(EventlistEntry2str(x,0,TmpStr2, false))
                      PrintTerminal(TmpStr2);
                    }
                  }
                else
                  {
                  EventlistEntry2str(Par1,0,TmpStr2, false);
                  PrintTerminal(TmpStr2);
                  }
                PrintTerminal(ProgmemString(Text_22));
                }
              break;
                
            case CMD_NODO_IP:
              if(GetArgv(Command,TmpStr1,2))
                if(!str2ip(TmpStr1,Settings.Nodo_IP))
                  error=MESSAGE_02;
              SaveSettings();
              break;
              
            case CMD_CLIENT_IP:
              if(GetArgv(Command,TmpStr1,2))
                if(!str2ip(TmpStr1,Settings.Client_IP))
                  error=MESSAGE_02;
              SaveSettings();
              break;
              
            case CMD_SUBNET:
              if(GetArgv(Command,TmpStr1,2))
                if(!str2ip(TmpStr1,Settings.Subnet))
                  error=MESSAGE_02;
              SaveSettings();
              break;
              
            case CMD_DNS_SERVER:
              if(GetArgv(Command,TmpStr1,2))
                if(!str2ip(TmpStr1,Settings.DnsServer))
                  error=MESSAGE_02;
              SaveSettings();
              break;
              
            case CMD_GATEWAY:
              if(GetArgv(Command,TmpStr1,2))
                if(!str2ip(TmpStr1,Settings.Gateway))
                  error=MESSAGE_02;
              SaveSettings();
              break;
              
            case CMD_EVENTLIST_FILE:
              Led(BLUE);
              if(GetArgv(Command,TmpStr1,2))
                {
                strcat(TmpStr1,".dat");
                if(!SaveEventlistSDCard(TmpStr1))
                  TransmitCode(command2event(Settings.Unit, CMD_MESSAGE, Settings.Unit, MESSAGE_03),VALUE_ALL);// geen RaiseMessage() anders weer poging om te loggen naar SDCard ==> oneindige loop
                }
              break;
  
            case CMD_FILE_LIST:
              if(!FileList())
                error=MESSAGE_03;
              break;
  
            case CMD_FILE_WRITE:
              if(GetArgv(Command,TmpStr1,2))
                {
                strcat(TmpStr1,".dat");
                strcpy(TempLogFile,TmpStr1);
                FileWriteMode=60;
                }
              else
                error=MESSAGE_02;
              break;
  
            case CMD_HTTP_REQUEST:
              // zoek in de regel waar de string met het http request begint.
              x=StringFind(Line,cmd2str(CMD_HTTP_REQUEST))+strlen(cmd2str(CMD_HTTP_REQUEST));
              while(Line[x]==32)x++;
              strcpy(Settings.HTTPRequest,&Line[0]+x);
              SaveSettings(); 
              break;
  
            case CMD_PORT_SERVER:
              {
              if(GetArgv(Command,TmpStr1,2))
                {
                Settings.PortServer=str2int(TmpStr1);
                SaveSettings();
                }
              break;
              }  
  
            case CMD_PORT_CLIENT:
              {
              if(GetArgv(Command,TmpStr1,2))
                {
                Settings.PortClient=str2int(TmpStr1);
                SaveSettings();
                }
              break;
              }  
  
            default:
              {              
              // standaard commando volgens gewone syntax
              v=command2event(Settings.Unit,Cmd,Par1,Par2);            
              error=Commanderror(v);
  
              // Als het geen regulier commando was EN geen commando met afwijkende MMI, dan kijken of file op SDCard staat)
              if(error)
                {
                Command[8]=0;// Gebruik commando als een filename. Voor de zekerheid te lange filename afkappen
                if(!FileExecute(Command))
                  {
                  error=0;
                  v=0;
                  }
                }
              }
            }
          }
            
        if(v && error==0)
          {
          if(State_EventlistWrite==0)// Gewoon uitvoeren
            {
            if(SendTo==0)
              {
              ProcessEvent2(v,VALUE_DIRECTION_INPUT,Port,0,0);      // verwerk binnengekomen event.
              }              
            else
              {
              if(NodoType(v))
                v=(v&0xf0ffffff)|(unsigned long)SendTo<<24;// Commando/Event unit nummer van de bestemming toewijzen, behalve als het een HEX-event is.
  
              if(Queue.Position<EVENT_QUEUE_MAX)
                {
                Queue.Event[Queue.Position]=v;
                Queue.Port[Queue.Position]=Port;
                Queue.Position++;           
                }       
              else
                break;
              }
            continue;
            }
    
          if(State_EventlistWrite==2)
            {
            a=v;            
            if(!Eventlist_Write(EventlistWriteLine,event,a))
              {
              RaiseMessage(MESSAGE_06);
              break;
              // return;
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
          
        if(error) // er heeft zich een fout voorgedaan
          {
          strcpy(TmpStr2,Command);
          strcat(TmpStr2, " ?");
          PrintTerminal(TmpStr2);
          RaiseMessage(error);
          Line[0]=0;
          }
        }
        
      // printbare tekens toevoegen aan commando zolang er nog ruimte is in de string
      if(isprint(x) && x!=';' && PosCommand<MaxCommandLength)
        Command[PosCommand++]=x;      
      }    
  
    if(SendTo!=0)// Verzend de inhoud van de queue naar de slave Nodo
      {
      if(!QueueSend(SendTo))
        RaiseMessage(MESSAGE_12);
      
      // Verwerk eventuele events die in de queue zijn geplaatst.
      ProcessQueue();
      }
    }

  if(Busy.Sent)
    {
    TransmitCode(command2event(Settings.Unit,CMD_BUSY,VALUE_OFF,0),VALUE_ALL);
    Busy.Sent=false;
    }

  free(TmpStr2);
  free(TmpStr1);
  return error;
  }
#endif
  

