
 /*********************************************************************************************\
 * Deze functie checkt of de code die ontvangen is een uitvoerbare opdracht is.
 * Als het een correct commando is wordt deze uitgevoerd en 
 * true teruggegeven. Zo niet dan wordt er een 'false' retour gegeven.
 \*********************************************************************************************/

byte ExecuteCommand(struct NodoEventStruct *EventToExecute)
  {
  unsigned long a;
  int w,x,y,z;
  byte error=0;
  
  struct NodoEventStruct TempEvent,TempEvent2;
  ClearEvent(&TempEvent);
  ClearEvent(&TempEvent2);

  #if NODO_MEGA
  char *TempString=(char*)malloc(80);
  char *TempString2=(char*)malloc(15);
  #endif
  
  switch(EventToExecute->Command)
    {   
    #if HARDWARE_WIRED_IN_PORTS
    case CMD_WIRED_SMITTTRIGGER:
      if(EventToExecute->Par1>0 && EventToExecute->Par1<=HARDWARE_WIRED_IN_PORTS)
        Settings.WiredInputSmittTrigger[EventToExecute->Par1-1]=EventToExecute->Par2;
      break;                  

    case CMD_WIRED_THRESHOLD:
      if(EventToExecute->Par1>0 && EventToExecute->Par1<=HARDWARE_WIRED_IN_PORTS)
        Settings.WiredInputThreshold[EventToExecute->Par1-1]=EventToExecute->Par2;
      break;                  

    case CMD_VARIABLE_SET_WIRED_ANALOG:
      if(UserVariableSet(EventToExecute->Par1,analogRead(PIN_WIRED_IN_1+EventToExecute->Par2-1),true)==-1)
        error=MESSAGE_VARIABLE_ERROR;
      break;

    case CMD_WIRED_PULLUP:
      Settings.WiredInputPullUp[EventToExecute->Par1-1]=EventToExecute->Par2; // EventToExecute->Par1 is de poort[1..]
      
      if(EventToExecute->Par2==VALUE_ON)
        pinMode(A0+PIN_WIRED_IN_1+EventToExecute->Par1-1,INPUT_PULLUP);
      else
        pinMode(A0+PIN_WIRED_IN_1+EventToExecute->Par1-1,INPUT);
      break;
    #endif
                 

    #if HARDWARE_WIRED_OUT_PORTS
    case CMD_WIRED_OUT_VARIABLE:
      if(UserVariable(EventToExecute->Par2,&TempFloat)!=-1)
        {
        analogWrite(PIN_WIRED_OUT_1+EventToExecute->Par1-1,(byte)TempFloat);
        WiredOutputStatus[EventToExecute->Par1-1]=(byte)TempFloat;
        }
      else
        error=MESSAGE_VARIABLE_ERROR;
      break;
    
    case CMD_WIRED_OUT:
      analogWrite(PIN_WIRED_OUT_1+EventToExecute->Par1-1,EventToExecute->Par2);
      WiredOutputStatus[EventToExecute->Par1-1]=EventToExecute->Par2;
      break;
    #endif

    case CMD_VARIABLE_TOGGLE:
      UserVariable(EventToExecute->Par1,&TempFloat);
      TempFloat=TempFloat>0.5?0.0:1.0;
      if(UserVariableSet(EventToExecute->Par1,TempFloat,true)==-1)
        error=MESSAGE_VARIABLE_ERROR;
      break;         

    case CMD_VARIABLE_INC:
      UserVariable(EventToExecute->Par1,&TempFloat);
      TempFloat+=ul2float(EventToExecute->Par2);
      if(UserVariableSet(EventToExecute->Par1,TempFloat,true)==-1)
        error=MESSAGE_VARIABLE_ERROR;
      break;         

    case CMD_VARIABLE_DEC:
      UserVariable(EventToExecute->Par1,&TempFloat);
      TempFloat-=ul2float(EventToExecute->Par2);
      if(UserVariableSet(EventToExecute->Par1,TempFloat,true)==-1)
        error=MESSAGE_VARIABLE_ERROR;
      break;         

    case CMD_VARIABLE_SET:
      if(UserVariableSet(EventToExecute->Par1,ul2float(EventToExecute->Par2),true)==-1)
        error=MESSAGE_VARIABLE_ERROR;
      break;         

    case CMD_VARIABLE_PAYLOAD:
      if(!UserVariablePayload(EventToExecute->Par1,EventToExecute->Par2))
        error=MESSAGE_VARIABLE_ERROR;
      break;         

    case CMD_VARIABLE_VARIABLE:
      if(UserVariable(EventToExecute->Par2,&TempFloat)!=-1)
        if(UserVariableSet(EventToExecute->Par1,TempFloat,true)==-1)
          error=MESSAGE_VARIABLE_ERROR;
      break;        

    case CMD_VARIABLE_GLOBAL:
      if(!UserVariableGlobal(EventToExecute->Par1,EventToExecute->Par2==VALUE_ON))
        error=MESSAGE_VARIABLE_ERROR;
      break;        
                                                                                       
    #if HARDWARE_PULSE
    case CMD_VARIABLE_PULSE_COUNT:
      // Tellen van pulsen actief: enable IRQ behorende bij PIN_IR_RX_DATA. Zodra dit commando wordt gebruikt, wordt ontvangst van IR uitgeschakeld.
      HW_SetStatus(HW_PULSE,true,true);
      attachInterrupt(PULSE_IRQ,PulseCounterISR,PULSE_TRANSITION); 
      if(UserVariableSet(EventToExecute->Par1,(float)PulseCount,true)==-1)
        error=MESSAGE_VARIABLE_ERROR;
      else
        PulseCount=0L;
      break;         

    case CMD_VARIABLE_PULSE_TIME:
      // Tellen van pulsen actief: enable IRQ behorende bij PIN_IR_RX_DATA. Zodra dit commando wordt gebruikt, wordt ontvangst van IR uitgeschakeld.
      if(UserVariableSet(EventToExecute->Par1,(float)PulseTime,true)!=-1)
        {
        HW_SetStatus(HW_PULSE,true,true);
        attachInterrupt(PULSE_IRQ,PulseCounterISR,PULSE_TRANSITION); 
        UserVariablePayload(EventToExecute->Par1,0x0021); // milliseconds
        }
      else
        error=MESSAGE_VARIABLE_ERROR;
      break;         
    #endif // HARDWARE_PULSE

    case CMD_STOP:
      error=MESSAGE_EXECUTION_STOPPED;
      break;
      
    case CMD_BREAK_ON_VAR_EQU:
      UserVariable(EventToExecute->Par1,&TempFloat);
      if((int)TempFloat==(int)ul2float(EventToExecute->Par2))
        error=MESSAGE_BREAK;
      break;
      
    case CMD_BREAK_ON_VAR_NEQU:
      UserVariable(EventToExecute->Par1,&TempFloat);
      if((int)TempFloat!=(int)ul2float(EventToExecute->Par2))
        error=MESSAGE_BREAK;
      break;

    case CMD_BREAK_ON_VAR_MORE:
      UserVariable(EventToExecute->Par1,&TempFloat);
      if((int)TempFloat>(int)ul2float(EventToExecute->Par2))
        error=MESSAGE_BREAK;
      break;

    case CMD_BREAK_ON_VAR_LESS:
      UserVariable(EventToExecute->Par1,&TempFloat);
      if((int)TempFloat<(int)ul2float(EventToExecute->Par2))
        error=MESSAGE_BREAK;

    case CMD_BREAK_ON_VAR_LESS_VAR:
      UserVariable(EventToExecute->Par1,&TempFloat);
      UserVariable(EventToExecute->Par2,&TempFloat2);
      if(TempFloat<TempFloat2)
          error=MESSAGE_BREAK;
      break;

    case CMD_BREAK_ON_VAR_MORE_VAR:
      UserVariable(EventToExecute->Par1,&TempFloat);
      UserVariable(EventToExecute->Par2,&TempFloat2);
      if(TempFloat>TempFloat2)
        error=MESSAGE_BREAK;
      break;

    case CMD_SEND_USEREVENT:
      TempEvent.Port                  = VALUE_ALL;
      TempEvent.Type                  = NODO_TYPE_EVENT;
      TempEvent.Command               = EVENT_USEREVENT;
      TempEvent.Par1                  = EventToExecute->Par1;
      TempEvent.Par2                  = EventToExecute->Par2;
      SendEvent(&TempEvent, false, true);
      break;

    case CMD_VARIABLE_SEND:
      a=UserVariable(EventToExecute->Par1,&TempFloat);
      if(a!=-1)
        {
        TempEvent.Type          = NODO_TYPE_EVENT;
        TempEvent.Command       = EVENT_VARIABLE;
        TempEvent.Port          = EventToExecute->Par2;
        TempEvent.Direction     = VALUE_DIRECTION_OUTPUT;
        TempEvent.Par1          = EventToExecute->Par1;
        TempEvent.Par2          = float2ul(TempFloat);
        TempEvent.Payload       = UserVarPayload[a];
        SendEvent(&TempEvent, false, true);
        }
      else
        error=MESSAGE_VARIABLE_ERROR;
      break;         

    case CMD_TIMER_SET:
      if(EventToExecute->Par2==0)
        UserTimer[EventToExecute->Par1-1]=0L;
      else
        UserTimer[EventToExecute->Par1-1]=millis()+EventToExecute->Par2*1000L;
      break;

    case CMD_TIMER_SET_VARIABLE:
      if(UserVariable(EventToExecute->Par2,&TempFloat)!=-1)
        UserTimer[EventToExecute->Par1-1]=millis()+(unsigned long)(TempFloat)*1000L;
      else
        error=MESSAGE_VARIABLE_ERROR;
      break;

    case CMD_TIMER_RANDOM:
      UserTimer[EventToExecute->Par1-1]=millis()+random(EventToExecute->Par2)*1000;
      break;

    case CMD_DELAY:
      Wait(EventToExecute->Par2, false, 0, false);
      break;        

    case CMD_SEND_EVENT:
      TempEvent=LastReceived;
      TempEvent.Port=EventToExecute->Par1==0?VALUE_ALL:EventToExecute->Par1;
      SendEvent(&TempEvent, TempEvent.Command==EVENT_RAWSIGNAL,true);
      break;        

    case CMD_VARIABLE_SAVE: 
      for(z=0;z<=USER_VARIABLES_MAX;z++)
        {
        if(UserVarKey[z]==EventToExecute->Par1 || EventToExecute->Par1==0)
          {
          x=0;
          w=0;                                                                  // Plaats waar variabele al bestaat in eventlist
          y=0;                                                                  // hoogste beschikbare plaats
          while(Eventlist_Read(++x,&TempEvent,&TempEvent2))                     // Zoek of variabele al bestaat in eventlist
            {
            if(TempEvent.Type==NODO_TYPE_EVENT && TempEvent.Command==EVENT_BOOT && TempEvent.Par1==Settings.Unit)
              if(TempEvent2.Type==NODO_TYPE_COMMAND && TempEvent2.Command==CMD_VARIABLE_SET && TempEvent2.Par1==UserVarKey[z])
                w=x;
              
            if(TempEvent.Command==0)
              y=x;
            }            

          x=w>0?w:y;                                                            // Bestaande regel of laatste vrije plaats.
    
          if(UserVariable(UserVarKey[z],&TempFloat)!=-1)
            {
            TempEvent.Type      = NODO_TYPE_EVENT;
            TempEvent.Command   = EVENT_BOOT;
            TempEvent.Par1      = Settings.Unit;
            TempEvent.Par2      = 0;
            TempEvent2.Type     = NODO_TYPE_COMMAND;
            TempEvent2.Command  = CMD_VARIABLE_SET;
            TempEvent2.Par1     = UserVarKey[z];
            TempEvent2.Par2     = float2ul(TempFloat);
           
            Eventlist_Write(x, &TempEvent, &TempEvent2);                        // Schrijf weg in eventlist
            }
          }
        }
      break;

    case CMD_EVENTLIST_ERASE:
      #if HARDWARE_STATUS_LED || HARDWARE_STATUS_LED_RGB
      Led(BLUE);
      #endif
      
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
        
    case CMD_EVENTLIST_SHOW:
      // Er kunnen zich hier twee situaties voordoen: het verzoek is afkomstig van een Terminal (Serial/Telnet) of 
      // via IR/RF/I2C. Beide kennen een andere afhandeling immers de Terminal variant kan direct naar de MMI.
      // Bij de anderen moet er nog transport plaats vinden via IR, RF, I2C. De Terminal variant is NIET relevant
      // voor een Small omdat deze geen MMI heeft.

      #if NODO_MEGA
      if(EventToExecute->Port==VALUE_SOURCE_SERIAL || EventToExecute->Port==VALUE_SOURCE_TELNET)
        {      
        if(EventToExecute->Par1<=EventlistMax)
          {
          PrintString(ProgmemString(Text_22),EventToExecute->Port);
          if(EventToExecute->Par1==0)
            {
            x=1;
            while(EventlistEntry2str(x++,0,TempString,false))
              if(TempString[0]!=0)
                PrintString(TempString,EventToExecute->Port);
            }
          else
            {
            EventlistEntry2str(EventToExecute->Par1,0,TempString,false);
              if(TempString[0]!=0)
                PrintString(TempString,EventToExecute->Port);
            }
          PrintString(ProgmemString(Text_22),EventToExecute->Port);
          }
        else
          error=MESSAGE_INVALID_PARAMETER;
        }
      else // Transmissie via I2C/RF/IR: dan de inhoud van de Eventlist versturen.
        {
      #endif
      
        if(EventToExecute->Par1==0)
          {
          x=1;
          y=EventlistMax;
          }
        else
          {
          x=EventToExecute->Par1;
          y=EventToExecute->Par1;
          }
                
        // Haal de event en action op uit eeprom en verzend deze met extra transmissie vlaggen zodat de data:
        // 1. alleen wordt verstuurd naar de nodo die de data heeft opgevraagd.
        // 2. alleen wordt verzonden naar de poort waar het verzoek vandaan kwam
        // 3. aan de ontvangende zijde in de queue wordt geplaatst
        // 4. de vlag VIEW_SPECIAL mee krijgt zodat de events/commando's niet worden uitgevoerd aan de ontvangende zijde.
        // 5. Met LOCK alle andere Nodo's tijdelijk in de hold worden gezet.
        // In geval van verzending naar queue zal deze tijd niet van toepassing zijn omdat er dan geen verwerkingstijd nodig is.
        // Tussen de events die de queue in gaan een kortere delay tussen verzendingen.

        z=EventToExecute->Port;
        w=EventToExecute->SourceUnit;
        
        while(x<=y && Eventlist_Read(x,&TempEvent,&TempEvent2))
          {
          if(TempEvent.Command!=0)
            {
            ClearEvent(EventToExecute);
            EventToExecute->Par1=x;
            EventToExecute->Command=SYSTEM_COMMAND_QUEUE_EVENTLIST_SHOW;
            EventToExecute->Flags=TRANSMISSION_QUEUE | TRANSMISSION_QUEUE_NEXT | TRANSMISSION_BUSY;
            EventToExecute->Type=NODO_TYPE_SYSTEM;
            EventToExecute->Port=z;
            EventToExecute->SourceUnit=Settings.Unit;
            EventToExecute->DestinationUnit=w;
            SendEvent(EventToExecute,false,false);

            TempEvent.Flags=TRANSMISSION_VIEW_EVENTLIST | TRANSMISSION_QUEUE | TRANSMISSION_BUSY;
            TempEvent.Port=z;
            TempEvent.DestinationUnit=w;
            SendEvent(&TempEvent,false,false);
    
            TempEvent2.Flags=TRANSMISSION_VIEW_EVENTLIST | TRANSMISSION_QUEUE | TRANSMISSION_BUSY | (x!=y?TRANSMISSION_QUEUE_NEXT:0); 
            TempEvent2.Port=z;
            TempEvent2.DestinationUnit=w;
            SendEvent(&TempEvent2,false,false);
            }
          x++;
          }
        #if NODO_MEGA
        }        
        #endif
      break;

    #if HARDWARE_CLOCK && HARDWARE_I2C
    case CMD_BREAK_ON_DAYLIGHT:
      if(EventToExecute->Par1==VALUE_ON && (Time.Daylight==2 || Time.Daylight==3))
        error=MESSAGE_BREAK;

      if(EventToExecute->Par1==VALUE_OFF && (Time.Daylight==0 || Time.Daylight==1 || Time.Daylight==4))
        error=MESSAGE_BREAK;
      break;

    case CMD_BREAK_ON_TIME_LATER:
      if(EventToExecute->Par2<(Time.Minutes%10 | (unsigned long)(Time.Minutes/10)<<4 | (unsigned long)(Time.Hour%10)<<8 | (unsigned long)(Time.Hour/10)<<12))
        error=MESSAGE_BREAK;
      break;

    case CMD_BREAK_ON_TIME_EARLIER:
      if(EventToExecute->Par2>(Time.Minutes%10 | (unsigned long)(Time.Minutes/10)<<4 | (unsigned long)(Time.Hour%10)<<8 | (unsigned long)(Time.Hour/10)<<12))
        error=MESSAGE_BREAK;
      break;

    case CMD_CLOCK_TIME:
      Time.Hour    =((EventToExecute->Par2>>12)&0xf)*10 + ((EventToExecute->Par2>>8)&0xf);
      Time.Minutes =((EventToExecute->Par2>>4 )&0xf)*10 + ((EventToExecute->Par2   )&0xf);
      Time.Seconds=0;
      ClockSet();
      break;

    case CMD_CLOCK_DATE:
      Time.Date    =((EventToExecute->Par2>>28 )&0xf)*10   + ((EventToExecute->Par2>>24 )&0xf);
      Time.Month   =((EventToExecute->Par2>>20 )&0xf)*10   + ((EventToExecute->Par2>>16 )&0xf);
      Time.Year    =((EventToExecute->Par2>>12 )&0xf)*1000 + ((EventToExecute->Par2>>8 )&0xf)*100 + ((EventToExecute->Par2>>4)&0xf)*10 + ((EventToExecute->Par2)&0xf);
      ClockSet();
      break;

    #endif // HARDWARE_CLOCK 


    #if NODO_MEGA
    #if HARDWARE_CLOCK  && HARDWARE_I2C
    case CMD_ALARM_SET:
      if(EventToExecute->Par1>=1 && EventToExecute->Par1<=ALARM_MAX)              // niet buiten bereik array!
        {
        for(x=0;x<8;x++)                                                          // loop de acht nibbles van de 32-bit Par2 langs
          {          
          y=(EventToExecute->Par2>>(x*4))&0xF;                                    // selecter nibble
          if(y!=0xE)                                                              // als de waarde geset moet worden
            {
            a=0xffffffff  ^ (0xfUL <<(x*4));                                      // Mask maken om de nibble positie y te wissen.
            Settings.Alarm[EventToExecute->Par1-1]  &=  a;                        // Maak nibble leeg
            Settings.Alarm[EventToExecute->Par1-1]  |= ((unsigned long)y)<<(x*4); // vul met door user opgegeven token
            }
          }
        }
      break;

    case CMD_CLOCK_SYNC:
      if(HW_Status(HW_CLOCK))
        {   
        // haal de tijd op van de Webserver. Dit vind plaats in de funktie: boolean SendHTTPRequest(char* Request)
        if(WebApp)
          {
          ClockSyncHTTP=true;
          EventToExecute->Port=VALUE_SOURCE_HTTP;
          SendEvent(EventToExecute, false, true);
          ClockSyncHTTP=false;
          }
  
        // Geef de juiste tijd nu door aan alle andere Nodo's

        // Verzend datum
        TempEvent.Port                  = VALUE_ALL;
        TempEvent.Type                  = NODO_TYPE_COMMAND;
        TempEvent.DestinationUnit       = 0;    
        TempEvent.Flags                 = TRANSMISSION_QUEUE | TRANSMISSION_QUEUE_NEXT; 
        TempEvent.Command=CMD_CLOCK_DATE;
        TempEvent.Par2= ((unsigned long)Time.Year  %10)      | ((unsigned long)Time.Year  /10)%10<<4  | ((unsigned long)Time.Year/100)%10<<8 | ((unsigned long)Time.Year/1000)%10<<12 | 
                        ((unsigned long)Time.Month %10) <<16 | ((unsigned long)Time.Month /10)%10<<20 | 
                        ((unsigned long)Time.Date  %10) <<24 | ((unsigned long)Time.Date  /10)%10<<28 ;

        SendEvent(&TempEvent, false, true);
              
        // Verzend tijd
        ClearEvent(&TempEvent);    
        TempEvent.Port                  = VALUE_ALL;
        TempEvent.Type                  = NODO_TYPE_COMMAND;
        TempEvent.DestinationUnit       = 0;    
        TempEvent.Flags                 = TRANSMISSION_QUEUE; 
        TempEvent.Command=CMD_CLOCK_TIME;
        TempEvent.Par2=Time.Minutes%10 | Time.Minutes/10<<4 | Time.Hour%10<<8 | Time.Hour/10<<12;

        SendEvent(&TempEvent, false, true);        
        }
      break;
    #endif //HARDWARE_CLOCK 
    #endif NODO_MEGA
          

    case CMD_SLEEP:
      #if !NODO_MEGA
      #if HARDWARE_BATTERY
      GoodNightSleepTight();
      #endif
      #endif      
      break;


    #if HARDWARE_SPEAKER
    case CMD_SOUND: 
      Alarm(EventToExecute->Par1,EventToExecute->Par2);
      break;
    #endif    
  
    case CMD_SETTINGS_SAVE:
      Save_Settings();
      break;

    #if HARDWARE_ETHERNET                                                       // Zal verdwijnen zodra de WebSocket is geimplementeerd.
    case CMD_OUTPUT:
      switch(EventToExecute->Par1)
        {
        case VALUE_SOURCE_HTTP:
          Settings.TransmitHTTP=EventToExecute->Par2;        
          break;       
        }
      break;
    #endif
      
      
    case CMD_STATUS:
      Status(EventToExecute);
      break;
            
    case CMD_UNIT_SET:
      if(EventToExecute->Par1>0 && EventToExecute->Par1<=UNIT_MAX)
        {
        Settings.Unit=EventToExecute->Par1;  
        Save_Settings();
        Reboot();
        break;
        }

    case CMD_REBOOT:
      Reboot();
      break;        

    case CMD_BUSY:
      TempEvent.Port                  = VALUE_ALL;
      TempEvent.Type                  = NODO_TYPE_SYSTEM;
      TempEvent.Command               = SYSTEM_COMMAND_BUSY;
      TempEvent.Flags                 = (EventToExecute->Par1==VALUE_ON)?TRANSMISSION_BUSY:0;
      SendEvent(&TempEvent, false,false);
      break;        
      
    case CMD_RESET:
      ResetFactory();
      break;

    case CMD_WAIT_FREE_NODO:
      Settings.WaitBusyNodo=EventToExecute->Par1;
      break;                                                                      


#if NODO_MEGA // vanaf hier commando's die alleen de Mega kent.

    #if HARDWARE_SDCARD
    case CMD_LOG: 
      Settings.Log=EventToExecute->Par1;
      break;
      
    case CMD_FILE_EXECUTE:
      error=FileExecute("",int2str(EventToExecute->Par2),"DAT", EventToExecute->Par1==VALUE_ON, VALUE_ALL);
      break;      

    case CMD_ALIAS_SHOW:
      Settings.Alias=EventToExecute->Par1; 
      break;

    case CMD_VARIABLE_LOG:
      {
      if(EventToExecute->Par1==0)
        {
        x=1;
        y=USER_VARIABLES_MAX;
        }
      else
        {
        x=EventToExecute->Par1;
        y=EventToExecute->Par1;
        }
      
      for(w=x;w<=y;w++)
        {
        #if HARDWARE_CLOCK && HARDWARE_I2C
        sprintf(TempString,ProgmemString(Text_16),Time.Date,Time.Month,Time.Year,Time.Hour,Time.Minutes,w);
        #else
        sprintf(TempString,ProgmemString(Text_13),w);
        #endif
                
        if(UserVariable(w,&TempFloat)!=-1)
          {
          dtostrf(TempFloat, 0, 2,TempString+strlen(TempString));
          strcpy(TempString2,"VAR_");
          strcat(TempString2,int2str(w));
          FileWriteLine("",TempString2,"DAT",TempString, false);
          }
        }        
      break;
      }
    #endif // HARDWARE_SDCARD
    
    case CMD_PORT_INPUT:
      Settings.PortInput=EventToExecute->Par2;
      break;

    case CMD_PORT_OUTPUT:
      Settings.PortOutput=EventToExecute->Par2;
      break;

    case CMD_ECHO:
      #if HARDWARE_ETHERNET
      if(EventToExecute->Port==VALUE_SOURCE_TELNET) 
        Settings.EchoTelnet=EventToExecute->Par1;
      #endif
      if(EventToExecute->Port==VALUE_SOURCE_SERIAL) 
        Settings.EchoSerial=EventToExecute->Par1;        
      break;


    case CMD_DEBUG: 
      Settings.Debug=EventToExecute->Par1;
      break;

    #if (HARDWARE_RAWSIGNAL || HARDWARE_RF433 || HARDWARE_INFRARED) && HARDWARE_SDCARD
    case CMD_RAWSIGNAL_RECEIVE:
      Settings.RawSignalReceive=EventToExecute->Par1;
      if(EventToExecute->Par2==VALUE_ON || EventToExecute->Par2==VALUE_OFF)
        Settings.RawSignalChecksum=EventToExecute->Par2;
      break;

    case CMD_RAWSIGNAL_SAVE:
      if(EventToExecute->Par2==0)                                               // Als er geen HEX-code is opgegeven door de gebruiker...
        RawSignal32bit(&TempEvent);                                             // dan eentje berekenen aan de hand van de inhoud van de RawSignal buffer
      else
        TempEvent.Par2=EventToExecute->Par2;
        
      error=RawSignalWrite(TempEvent.Par2);
      Settings.RawSignalReceive=VALUE_ON;
      break;

    case CMD_RAWSIGNAL_SHOW: 
      error=FileShow(ProgmemString(Text_08),int2strhex(EventToExecute->Par2)+2,"DAT", VALUE_ALL);
      break;

    case CMD_RAWSIGNAL_SEND:
      error=CommandRawSignalSend(EventToExecute->Par1,EventToExecute->Par2);
      break;
    #endif //HARDWARE_RAWSIGNAL && HARDWARE_SDCARD 
              
    #endif // NODO_MEGA    
    }

  #if NODO_MEGA
  free(TempString2);
  free(TempString);
  #endif

  return error;
  }


                             