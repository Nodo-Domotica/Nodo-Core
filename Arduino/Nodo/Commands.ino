
/*********************************************************************************************\
 * Eenvoudige check of event een Nodo commando is die voor deze Nodo bestemd is.
 * Test NIET op geldigheid van de parameters
 * geeft de waarde 0, NODO_TYPE_EVENT of NODO_TYPE_COMMAND terug.
 \*********************************************************************************************/
byte NodoType(struct NodoEventStruct *InEvent)
  {
  byte x;

  if(InEvent->Flags & TRANSMISSION_EVENT)
     return NODO_TYPE_EVENT;  

  if(InEvent->Flags & TRANSMISSION_COMMAND)
     return NODO_TYPE_COMMAND;  

  if(InEvent->Command>=FIRST_EVENT && InEvent->Command<=LAST_EVENT)
    return NODO_TYPE_EVENT;

  if(InEvent->Command>=FIRST_COMMAND && InEvent->Command<=LAST_COMMAND)
    return NODO_TYPE_COMMAND;

  return false;
  }  
  
 
/*********************************************************************************************\
 * Deze functie checked of de code die ontvangen is een uitvoerbare opdracht is/
 * Als het een correct commando is wordt deze uitgevoerd en 
 * true teruggegeven. Zo niet dan wordt er een 'false' retour gegeven.
 \*********************************************************************************************/

boolean ExecuteCommand(NodoEventStruct *EventToExecute)
  {
  unsigned long a;
  int x,y;
  byte error=0;
  
  struct NodoEventStruct TempEvent=*EventToExecute;
  
  #ifdef NODO_MEGA
  char *TempString=(char*)malloc(50);
  #endif
  
  switch(EventToExecute->Command)
    {   
    #ifdef PROTOCOL_1
    case CMD_PROTOCOL_1_SEND:
      TempEvent.Command=CMD_PROTOCOL_1;
      Protocol_1_EventToRawsignal(&TempEvent);  // Bouw het RawSignal op
      SendEvent(&TempEvent,true,true);               // Verzenden signaal
      break;
    #endif
      
    #ifdef PROTOCOL_2
    case CMD_PROTOCOL_2_SEND:
      TempEvent.Command=CMD_PROTOCOL_2;
      Protocol_2_EventToRawsignal(&TempEvent);  // Bouw het RawSignal op
      SendEvent(&TempEvent,true,true);               // Verzenden signaal
      break;
    #endif
      
    #ifdef PROTOCOL_3
    case CMD_PROTOCOL_3_SEND:
      TempEvent.Command=CMD_PROTOCOL_3;
      Protocol_3_EventToRawsignal(&TempEvent);  // Bouw het RawSignal op
      SendEvent(&TempEvent,true,true);               // Verzenden signaal
      break;
    #endif
      
    #ifdef PROTOCOL_4
    case CMD_PROTOCOL_4_SEND:
      TempEvent.Command=CMD_PROTOCOL_4;
      Protocol_4_EventToRawsignal(&TempEvent);  // Bouw het RawSignal op
      SendEvent(&TempEvent,true,true);               // Verzenden signaal
      break;
    #endif      
      
    case CMD_VARIABLE_INC:
      if(EventToExecute->Par1>0 || EventToExecute->Par1<=USER_VARIABLES_MAX) // in de MMI al afvevangen, maar deze beschermt tegen vastlopers i.g.v. een foutief ontvangen event
        {
        UserVar[EventToExecute->Par1-1]+=ul2float(EventToExecute->Par2);
        TempEvent.Command      = CMD_VARIABLE_EVENT;
        TempEvent.Par2         = float2ul(UserVar[EventToExecute->Par1-1]);
        TempEvent.Direction    = VALUE_DIRECTION_INTERNAL;
        TempEvent.Port         = VALUE_SOURCE_VARIABLE;
        ProcessEvent2(&TempEvent);
        }
      break;        

    case CMD_VARIABLE_DEC:
      if(EventToExecute->Par1>0 || EventToExecute->Par1<=USER_VARIABLES_MAX) // in de MMI al afvevangen, maar deze beschermt tegen vastlopers i.g.v. een foutief ontvangen event
        {
        UserVar[EventToExecute->Par1-1]-=ul2float(EventToExecute->Par2);
        TempEvent.Command      = CMD_VARIABLE_EVENT;
        TempEvent.Par2         = float2ul(UserVar[EventToExecute->Par1-1]);
        TempEvent.Direction    = VALUE_DIRECTION_INTERNAL;
        TempEvent.Port         = VALUE_SOURCE_VARIABLE;
        ProcessEvent2(&TempEvent);
        }
      break;        

    case CMD_VARIABLE_SET:
      if(EventToExecute->Par1>0 || EventToExecute->Par1<=USER_VARIABLES_MAX) // in de MMI al afvevangen, maar deze beschermt tegen vastlopers i.g.v. een foutief ontvangen event
        {
        UserVar[EventToExecute->Par1-1]=ul2float(EventToExecute->Par2);
        TempEvent.Command=CMD_VARIABLE_EVENT;
        TempEvent.Port=VALUE_SOURCE_VARIABLE;
        TempEvent.Direction=VALUE_DIRECTION_INTERNAL;
        ProcessEvent2(&TempEvent);      // verwerk binnengekomen event.
        }
      break;         
  
    case CMD_VARIABLE_VARIABLE:
      if(EventToExecute->Par2>0 || EventToExecute->Par2<=USER_VARIABLES_MAX) // in de MMI al afvevangen, maar deze beschermt tegen vastlopers i.g.v. een foutief ontvangen event
        {
        UserVar[EventToExecute->Par1-1]=UserVar[EventToExecute->Par2-1];
        TempEvent.Command=CMD_VARIABLE_EVENT;
        TempEvent.Port=VALUE_SOURCE_VARIABLE;
        TempEvent.Direction=VALUE_DIRECTION_INTERNAL;
        ProcessEvent2(&TempEvent);      // verwerk binnengekomen event.
        }
      break;        

    case CMD_BREAK_ON_VAR_EQU:
      {
      if(int(UserVar[EventToExecute->Par1-1])==int(ul2float(EventToExecute->Par2)))
        error=true;
      break;
      }
      
    case CMD_BREAK_ON_VAR_NEQU:
      if(int(UserVar[EventToExecute->Par1-1])!=int(ul2float(EventToExecute->Par2)))
        error=true;
      break;

    case CMD_BREAK_ON_VAR_MORE:
      if(UserVar[EventToExecute->Par1-1] > ul2float(EventToExecute->Par2))
        error=true;
      break;

    case CMD_BREAK_ON_VAR_LESS:
      if(UserVar[EventToExecute->Par1-1] < ul2float(EventToExecute->Par2))
        error=true;
      break;

    case CMD_BREAK_ON_VAR_LESS_VAR:
      if(UserVar[EventToExecute->Par1-1] < UserVar[EventToExecute->Par2-1])
        error=true;
      break;

    case CMD_BREAK_ON_VAR_MORE_VAR:
      if(UserVar[EventToExecute->Par1-1] > UserVar[EventToExecute->Par2-1])
        error=true;
      break;


    case CMD_BREAK_ON_DAYLIGHT:
      if(EventToExecute->Par1==VALUE_ON && (Time.Daylight==2 || Time.Daylight==3))
        error=true;

      if(EventToExecute->Par1==VALUE_OFF && (Time.Daylight==0 || Time.Daylight==1 || Time.Daylight==4))
        error=true;
      break;

    case CMD_BREAK_ON_TIME_LATER:
      if((EventToExecute->Par1*60+EventToExecute->Par2)<(Time.Hour*60+Time.Minutes))
        error=true;
      break;

    case CMD_BREAK_ON_TIME_EARLIER:
      if((EventToExecute->Par1*60+EventToExecute->Par2)>(Time.Hour*60+Time.Minutes))
        error=true;
      break;

    case CMD_SEND_USEREVENT:
      ClearEvent(&TempEvent);    
      TempEvent.Port                  = VALUE_ALL;
      TempEvent.Command               = CMD_USEREVENT;
      TempEvent.Par1                  = EventToExecute->Par1;
      TempEvent.Par2                  = EventToExecute->Par2;
      SendEvent(&TempEvent, false, true);
      break;

//    case CMD_LOCK:
//      // In de bits van EventToExecute->Par1 en EventToExecute->Par2 zit een sleutel die gegenereerd is uit het wachtwoord van de Nodo die het commando verstuurd heeft.
//      a=InEvent&0x7fff;// Zet de lock met de bits 0 t/m 15
//      if(InEvent&0x8000) // On/Off bevindt zich in bit nr. 15
//        {// Als verzoek om inschakelen dan Lock waarde vullen
//        if(Settings.Lock==0)// mits niet al gelocked.
//          {
//          Settings.Lock=a; 
//          }
//        else
//          {
//          error=MESSAGE_10;
//          }
//        }
//      else
//        {// Verzoek om uitschakelen
//        if(Settings.Lock==a || Settings.Lock==0)// als lock code overeen komt of nog niet gevuld
//          {
//          Settings.Lock=0;
//          }
//        else
//          {
//          error=MESSAGE_10;
//          }
//        }              
//      break;
//??? Lock herstellen

    #ifdef NODO_MEGA
    case CMD_UNIT_LIST:
      PrintTerminal(ProgmemString(Text_22));
      for(x=1;x<=UNIT_MAX;x++)
        {
        y=NodoOnline(x,0);
        if(y!=0)
          {
          strcpy(TempString,cmd2str(CMD_UNIT));
          strcat(TempString," ");
          strcat(TempString,int2str(x));
          strcat(TempString," on port ");
          strcat(TempString,cmd2str(y));
          PrintTerminal(TempString);
          } 
        }
      PrintTerminal(ProgmemString(Text_22));
      break;

    case CMD_CLOCK_SYNC:
      // ClockSync stuurt de juise klokinstellingen naar een andere Nodo. Als het opgegeven unitnummer het eigen unitnummer is
      // dan wordt het verzoek naar HTTP verzonden.
      
      if(EventToExecute->Par1==Settings.Unit || EventToExecute->Par1==0)
        {        
        SendHTTPEvent(EventToExecute);
        }
      else
        {
        // oplossen met nieuwe adressering ???
//        Queue.Event[Queue.Position]=command2event(Par1, CMD_CLOCK_YEAR,Time.Year/100,Time.Year%100);Queue.Port[Queue.Position++]=VALUE_SOURCE_SYSTEM;
//        Queue.Event[Queue.Position]=command2event(Par1, CMD_CLOCK_DATE,Time.Date,Time.Month);Queue.Port[Queue.Position++]=VALUE_SOURCE_SYSTEM;
//        Queue.Event[Queue.Position]=command2event(Par1, CMD_CLOCK_TIME,Time.Hour,Time.Minutes);Queue.Port[Queue.Position++]=VALUE_SOURCE_SYSTEM;
//        Queue.Event[Queue.Position]=command2event(Par1, CMD_CLOCK_DOW ,Time.Day,0);Queue.Port[Queue.Position++]=VALUE_SOURCE_SYSTEM;
        
        if(EventToExecute->Par2==0)
          x=VALUE_SOURCE_RF;
        else
          x=EventToExecute->Par2;
          
        error=QueueSend(EventToExecute->Par1);
        }
      break;
    #endif

    case CMD_ALARM_SET:
      if(EventToExecute->Par1<ALARM_MAX) // niet buiten bereik array!
        {
        for(x=0;x<8;x++)// loop de acht nibbles van de 32-bit Par2 langs
          {          
          y=(EventToExecute->Par2>>(x*4))&0xF; // selecter nibble
          if(y!=0xE) // als de waarde geset moet worden
            {
            a=0xffffffff  ^ (0xfUL <<(x*4)); // Mask maken om de nibble positie y te wissen.
            Settings.Alarm[EventToExecute->Par1-1]&=a; // Maak nibble leeg
            Settings.Alarm[EventToExecute->Par1-1]|=((unsigned long)y)<<(x*4); // vul met door user opgegeven token
            }
          }
        }
      break;
      
    case CMD_CLOCK_YEAR:
      x=EventToExecute->Par1*100+EventToExecute->Par2;
      Time.Year=x;
      ClockSet();
      ClockRead();
      break;
    
    case CMD_CLOCK_TIME:
      Time.Hour=EventToExecute->Par1;
      Time.Minutes=EventToExecute->Par2;
      Time.Seconds=0;
      ClockSet();
      break;

    case CMD_CLOCK_DATE: // data1=datum, data2=maand
      Time.Date=EventToExecute->Par1;
      Time.Month=EventToExecute->Par2;
      ClockSet();
      ClockRead();
      break;

    case CMD_CLOCK_DOW:
      Time.Day=EventToExecute->Par1;
      ClockSet();
      break;

    case CMD_TIMER_SET_MIN:
      // EventToExecute->Par1=timer, EventToExecute->Par2=minuten. Timers werken op een resolutie van seconden maar worden door de gebruiker ingegeven in minuten        
      if(EventToExecute->Par1==0)
        {
        x=1;
        y=TIMER_MAX;
        }
      else
        {
        x=EventToExecute->Par1;
        y=EventToExecute->Par1;
        }
      for(x;x<=y;x++)
        TimerSet(x,int(EventToExecute->Par2)*60);
        
      break;
      
    case CMD_TIMER_SET_SEC:
      // EventToExecute->Par1=timer, EventToExecute->Par2=seconden. Timers werken op een resolutie van seconden.            
     if(EventToExecute->Par1==0)
        {
        x=1;
        y=TIMER_MAX;
        }
      else
        {
        x=EventToExecute->Par1;
        y=EventToExecute->Par1;
        }
      for(x;x<=y;x++)
       TimerSet(x,EventToExecute->Par2);
      break;

    case CMD_TIMER_RANDOM:
      UserTimer[EventToExecute->Par1-1]=millis()+random(EventToExecute->Par2)*60000;// EventToExecute->Par1=timer, EventToExecute->Par2=maximaal aantal minuten???
      break;

//    case CMD_WAIT_EVENT:// WaitEvent <unit>, <command>
//      Wait(true,60,0x0FFF0000,((unsigned long)EventToExecute->Par1)<<24 | ((unsigned long)EventToExecute->Par1)<<16);//?? Testen
//      break;
//??? herstellen

    case CMD_DELAY:
      Wait(EventToExecute->Par1, false, 0, false);
      break;        

    case CMD_SEND_EVENT://???@@
      ClearEvent(&TempEvent);
      TempEvent=LastReceived;
      TempEvent.Port                  = VALUE_ALL;
      SendEvent(&TempEvent, TempEvent.Command==CMD_RAWSIGNAL,true);
      break;        

    case CMD_SOUND: 
      Alarm(EventToExecute->Par1,EventToExecute->Par2);
      break;     
  
    case CMD_WIRED_PULLUP:
      Settings.WiredInputPullUp[EventToExecute->Par1-1]=EventToExecute->Par2; // EventToExecute->Par1 is de poort[1..]
      if(Settings.WiredInputPullUp[x]==VALUE_ON)
        pinMode(A0+PIN_WIRED_IN_1+EventToExecute->Par1-1,INPUT_PULLUP);
      else
        pinMode(A0+PIN_WIRED_IN_1+EventToExecute->Par1-1,INPUT);
      break;
                 
    case CMD_WIRED_OUT:
      if(EventToExecute->Par1==0)
        {
        x=1;
        y=WIRED_PORTS;
        }
      else
        {
        x=EventToExecute->Par1;
        y=EventToExecute->Par1;
        }
      for(x;x<=y;x++)
        {
        digitalWrite(PIN_WIRED_OUT_1+x-1,(EventToExecute->Par2==VALUE_ON));
        WiredOutputStatus[x-1]=(EventToExecute->Par2==VALUE_ON);
        TempEvent.Par1=x;
        TempEvent.Port=VALUE_SOURCE_WIRED;
        TempEvent.Direction=VALUE_DIRECTION_OUTPUT;
        PrintEvent(&TempEvent);
        }
      break;
                         
    case CMD_SETTINGS_SAVE:
      UndoNewNodo();// Status NewNodo verwijderen indien van toepassing
      #ifdef NODO_MEGA
      for(x=0;x<USER_VARIABLES_MAX;x++)
        Settings.UserVar[x]=UserVar[x];
      #endif  
      Save_Settings();
      break;

    case CMD_WAITFREERF: 
      Settings.WaitFreeRF=EventToExecute->Par1;
      break;

    case CMD_OUTPUT:
      switch(EventToExecute->Par1)
        {
        case VALUE_SOURCE_IR:
          Settings.TransmitIR=EventToExecute->Par2;
          break;       
        case VALUE_SOURCE_RF:
          Settings.TransmitRF=EventToExecute->Par2;
          break;       

        #ifdef NODO_MEGA
        case VALUE_SOURCE_HTTP:
          Settings.TransmitIP=EventToExecute->Par2;        
          break;       
        #endif
        }
      
      break;
      
    case CMD_USERPLUGIN:
      #ifdef USER_PLUGIN
        UserPlugin_Command(EventToExecute->Par1,EventToExecute->Par2);
      #endif
      break;        

    case CMD_WIRED_SMITTTRIGGER://??? voor veilighed bereik nog afvangen?
      Settings.WiredInputSmittTrigger[EventToExecute->Par1]=EventToExecute->Par2;
      break;                  

    case CMD_WIRED_THRESHOLD:
      Settings.WiredInputThreshold[EventToExecute->Par1]=EventToExecute->Par2;
      break;                  

    case CMD_STATUS:
      // Het commando [Status] verzendt de status naar de bron waar het verzoek van is ontvangen. Serial en TelNet worden beschouwd als dezelfde bron.
      // Als het door de gebruiker is verzocht om logging naar een file te doen, dan wordt de output NIET als events verzonden.

      #ifdef NODO_MEGA
      if(TempLogFile[0]!=0 || EventToExecute->Port==VALUE_SOURCE_SERIAL || EventToExecute->Port==VALUE_SOURCE_TELNET)
      #else
      if(EventToExecute->Port==VALUE_SOURCE_SERIAL)
      #endif
        Status(EventToExecute, false);
      else
        Status(EventToExecute, true);
      break;
      
    case CMD_UNIT:
      Settings.Unit=EventToExecute->Par1;
      if(EventToExecute->Par2 !=0)
        Settings.Home=(byte)EventToExecute->Par2;
      Save_Settings();
      Reset();

    case CMD_REBOOT:
      delay(1000);
      Reset();
      break;        

    case CMD_PULSE_COUNT:
      PulseCount=EventToExecute->Par2;
      break;
      
    case CMD_RESET:
      ResetFactory();

    case CMD_EVENTLIST_ERASE:
      UndoNewNodo();// Status NewNodo verwijderen indien van toepassing
      Led(BLUE);
      ClearEvent(&TempEvent);
      
      if(EventToExecute->Par1==0)
        {
        x=1;
        while(Eventlist_Write(x++,&TempEvent,&TempEvent));
        }
      else
        {
        Eventlist_Write(EventToExecute->Par1,&TempEvent,&TempEvent);
        }
      break;        

    case CMD_DEVICE:
      switch(EventToExecute->Par2)
        {
        #ifdef DEVICE_1
        case 1:
          Device_1(EventToExecute);
        #endif
      
        #ifdef DEVICE_2
        case 2:
          Device_2(EventToExecute);
        #endif
      
        #ifdef DEVICE_3
        case 3:
          Device_3(EventToExecute);
        #endif
      
        #ifdef DEVICE_4
        case 4:
          Device_4(EventToExecute);
        #endif
        }
        
      if(EventToExecute->Command)
        ProcessEvent2(EventToExecute);      
      break;

#ifdef NODO_MEGA

    case CMD_PORT_SERVER:
      Settings.OutputPort=EventToExecute->Par2;
      break;

    case CMD_PORT_CLIENT:
      Settings.PortClient=EventToExecute->Par2;
      break;

    case CMD_ECHO:
      if(EventToExecute->Port==VALUE_SOURCE_TELNET) 
        Settings.EchoTelnet=EventToExecute->Par1;
      if(EventToExecute->Port==VALUE_SOURCE_SERIAL) 
        Settings.EchoSerial=EventToExecute->Par1;        
      break;

    case CMD_DEBUG: 
      Settings.Debug=EventToExecute->Par1;
      break;

    case CMD_LOG: 
      Settings.Log=EventToExecute->Par1;
      break;

    case CMD_SIMULATE_DAY:
      SimulateDay(); 
      break;     
      
    case CMD_RAWSIGNAL_SAVE:
      Led(BLUE);
      PrintTerminal(ProgmemString(Text_07));
      RawSignal.Key=EventToExecute->Par1;
      break;              

//    case CMD_RAWSIGNAL_SEND:
//      if(EventToExecute->Par1!=0)
//        {
//        if(RawSignalGet(EventToExecute->Par1))
//          {
//          x=VALUE_ALL;
//          if(EventToExecute->Par2==VALUE_SOURCE_RF || EventToExecute->Par2==VALUE_SOURCE_IR)
//            x=EventToExecute->Par2;
//          SendEvent_OLD(AnalyzeRawSignal(),x);
//          }
//        else
//          error=MESSAGE_03;
//        }
//      else
//        SendEvent_OLD(AnalyzeRawSignal(),VALUE_ALL);
//      break;        
//??? 
    case CMD_FILE_EXECUTE:
      strcpy(TempString,cmd2str(CMD_FILE_EXECUTE));
      strcat(TempString," ");
      strcat(TempString,int2str(EventToExecute->Par1));
      ExecuteLine(TempString,EventToExecute->Port);
      break;        
    #endif
    }

  #ifdef NODO_MEGA
  free(TempString);
  #endif

  return error;
  }


