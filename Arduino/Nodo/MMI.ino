#if NODO_MEGA


/*********************************************************************************************\
 * Print een event naar de opgegeven poort. Dit kan zijn:
 * 
 * VALUE_ALL, VALUE_SOURCE_SERIAL, VALUE_SOURCE_TELNET, VALUE_SOURCE_FILE
 * 
 * 
 * 
 * 
 * 
 \*********************************************************************************************/
void PrintEvent(struct NodoEventStruct *Event, byte Port)
  {
  int x;

  // Systeem events niet weergeven.
  if(Event->Type==NODO_TYPE_SYSTEM || Event->Type==0)
    return;
  
  char* StringToPrint=(char*)malloc(100);
  char* TmpStr=(char*)malloc(INPUT_BUFFER_SIZE+1);

  StringToPrint[0]=0; // als start een lege string
  
  // Direction
  if(Event->Direction)
    {
    strcat(StringToPrint,cmd2str(Event->Direction));      
    strcat(StringToPrint,"=");
  
    // Poort
    strcat(StringToPrint, cmd2str(Event->Port));
    if(Event->Port==VALUE_SOURCE_HTTP || Event->Port==VALUE_SOURCE_TELNET)
      {
      strcat(StringToPrint, "(");
      strcat(StringToPrint, ip2str(ClientIPAddress));
      strcat(StringToPrint, ")");
      }
      
    if(Event->Port==VALUE_SOURCE_EVENTLIST)
      {
      // print de nessting diepte van de eventlist en de regel uit de eventlist.
      strcat(StringToPrint, "(");
      strcat(StringToPrint, int2str(ExecutionDepth-1));
      strcat(StringToPrint, ".");
      strcat(StringToPrint, int2str(ExecutionLine));
      strcat(StringToPrint, ")");
      }
    strcat(StringToPrint, "; "); 
    }

  // Unit 
  strcat(StringToPrint, cmd2str(VALUE_UNIT));
  strcat(StringToPrint, "=");  
  if(Event->Direction==VALUE_DIRECTION_OUTPUT && Event->Port!=VALUE_SOURCE_HTTP)
    strcat(StringToPrint, int2str(Event->DestinationUnit));
  else
    strcat(StringToPrint, int2str(Event->SourceUnit)); 

  // Event
  strcat(StringToPrint, "; ");
  strcat(StringToPrint, ProgmemString(Text_14));
  Event2str(Event,TmpStr);
  strcat(StringToPrint, TmpStr);

  // WEERGEVEN OP TERMINAL
  PrintString(StringToPrint,Port);   // stuur de regel naar Serial en/of naar Ethernet

  // LOG OP SDCARD
  if(bitRead(HW_Config,HW_SDCARD) && Settings.Log==VALUE_ON) 
    {
    TmpStr[0]=0;
    // datum en tijd weergeven
    if(bitRead(HW_Config,HW_CLOCK)) // alleen als bitRead(HW_Config,HW_CLOCK)=true want dan is er een RTC aanwezig.
      {   
      strcat(TmpStr,DateTimeString());
      strcat(TmpStr,"; ");
      }
    strcat(TmpStr,StringToPrint);
    AddFileSDCard(ProgmemString(Text_23),TmpStr);
    }

  free(TmpStr);
  free(StringToPrint);
  } 


/**********************************************************************************************\
 * Print actuele dag, datum, tijd.
 \*********************************************************************************************/
char* DateTimeString(void)
  {
  int x;
  static char dt[40];
  char s[5];

  // Print de dag. 1=zondag, 0=geen RTC aanwezig
  for(x=0;x<=2;x++)
    s[x]=(*(ProgmemString(Text_04)+(Time.Day-1)*3+x));
  s[x]=0;

  sprintf(dt,"Date=%02d-%02d-%d (%s), Time=%02d:%02d",Time.Date,Time.Month,Time.Year, s, Time.Hour, Time.Minutes);

  return dt;
  }

/**********************************************************************************************\
 * Print de welkomsttekst van de Nodo.
 \*********************************************************************************************/
prog_char PROGMEM Text_welcome[] = "Nodo-Mega V%d.%d (beta), Product=SWACNC-MEGA-R%03d, Home=%d, ThisUnit=%d";
void PrintWelcome(void)
  {
  char *TempString=(char*)malloc(80);

  // Print Welkomsttekst
  PrintString("",VALUE_ALL);
  PrintString(ProgmemString(Text_22),VALUE_ALL);
  PrintString(ProgmemString(Text_01),VALUE_ALL);
  PrintString(ProgmemString(Text_02),VALUE_ALL);

  // print versienummer, unit en indien gevuld het ID
  sprintf(TempString,ProgmemString(Text_welcome),NODO_VERSION/10,NODO_VERSION%10, NODO_BUILD, Settings.Home, Settings.Unit);
  if(Settings.ID[0])
    {
    strcat(TempString,", ID=");
    strcat(TempString,Settings.ID);
    }

  PrintString(TempString,VALUE_ALL);

  // Geef datum en tijd weer.
  if(bitRead(HW_Config,HW_CLOCK))
    {
    sprintf(TempString,"%s %s",DateTimeString(), cmd2str(Time.DaylightSaving?VALUE_DLS:0));
    PrintString(TempString,VALUE_ALL);
    }

  // print IP adres van de Nodo
  if(bitRead(HW_Config,HW_ETHERNET))
    {
    sprintf(TempString,"NodoIP=%u.%u.%u.%u, ", Ethernet.localIP()[0],Ethernet.localIP()[1],Ethernet.localIP()[2],Ethernet.localIP()[3]);

    strcat(TempString,cmd2str(CMD_PORT_CLIENT));
    strcat(TempString,"=");        
    strcat(TempString,int2str(Settings.OutputPort));

    strcat(TempString,", ");        

    strcat(TempString,cmd2str(CMD_PORT_SERVER));
    strcat(TempString,"=");        
    strcat(TempString,int2str(Settings.PortClient));

    PrintString(TempString,VALUE_ALL);
    
    if(Settings.HTTPRequest[0])
      {
      strcpy(TempString,"Host=");        
      strcat(TempString,Settings.HTTPRequest);
      int x=StringFind(TempString,"/");
      TempString[x]=0;    
      strcat(TempString,", HostIP=");        
  
      if((HTTPClientIP[0] + HTTPClientIP[1] + HTTPClientIP[2] + HTTPClientIP[3]) > 0)
        strcat(TempString,ip2str(HTTPClientIP));
      else
        strcat(TempString,"?");        
  
      PrintString(TempString,VALUE_ALL);
      }
    }
    
  PrintString(ProgmemString(Text_22),VALUE_ALL);
  free(TempString);
  }


 /**********************************************************************************************\
 * Verzend teken(s) naar de Terminal
 \*********************************************************************************************/
void PrintString(char* LineToPrint, byte Port)
  {  
  if((Port==VALUE_SOURCE_SERIAL || Port==VALUE_ALL) && bitRead(HW_Config,HW_SERIAL))
    Serial.println(LineToPrint);

  if(bitRead(HW_Config,HW_ETHERNET))
    {
    if((Port==VALUE_SOURCE_TELNET || Port==VALUE_ALL) && TerminalClient.connected() && TerminalConnected>0 && TerminalLocked==0)
      TerminalClient.println(LineToPrint);
      
    if((Port==VALUE_SOURCE_HTTP || Port==VALUE_ALL) && HTTPClient.connected())
      {
      HTTPClient.print(LineToPrint);
      HTTPClient.println("<br>");
      }
    }

  // FileLog wordt hier uitgevoerd.
  if(TempLogFile[0]!=0)
    if(bitRead(HW_Config,HW_SDCARD))
      AddFileSDCard(TempLogFile,LineToPrint); // Extra logfile op verzoek van gebruiker: CMD_FILE_LOG
  }
  

#define PAR1_INT           1
#define PAR1_TEXT          2
#define PAR1_MESSAGE       3
#define PAR2_INT           4
#define PAR2_TEXT          5
#define PAR2_FLOAT         6
#define PAR2_INT_HEX       7
#define PAR2_DIM           8
#define PAR2_WDAY          9
#define PAR2_TIME         10
#define PAR2_DATE         11
#define PAR2_ALARMENABLED 12
#define PAR2_INT8         13
#define PAR3_INT          14
#define PAR4_INT          15
#define PAR5_INT          16


void Event2str(struct NodoEventStruct *Event, char* EventString)
  {
  int x;
  EventString[0]=0;

  // Er kunnen een aantal parameters worden weergegeven. In een kleine tabel wordt aangegeven op welke wijze de parameters aan de gebruiker
  // moeten worden getoond. Het is niet per defiitie zo dat de interne Par1, Par2 en Par3 ook dezelfe parameters zijn die aan de gebruiker
  // worden getoond.   
  byte ParameterToView[8]={0,0,0,0,0,0,0,0};

  // Devices hebben een eigen MMI, Roep het device aan voor verwerking van de parameter DEVICE_MMI_OUT.
  // zoek het device op in de devices tabel en laat de string vullen. Als het niet gelukt is om de string te
  // vullen dan behandelen als een regulier event/commando
  if(Event->Type == NODO_TYPE_DEVICE_COMMAND || Event->Type == NODO_TYPE_DEVICE_EVENT)
    for(x=0;Device_ptr[x]!=0 && x<DEVICE_MAX; x++)
      if(Device_id[x]==Event->Command)
        Device_ptr[x](DEVICE_MMI_OUT,Event,EventString);

  if(EventString[0]==0)
    {
    strcat(EventString,cmd2str(Event->Command));
    strcat(EventString," ");      
    switch(Event->Command)
      {
      case EVENT_TIME:
        ParameterToView[0]=PAR2_TIME;         // tijd
        ParameterToView[1]=PAR2_WDAY;         // dag van de week of een wildcard
        break;

      case CMD_ALARM_SET:
        ParameterToView[0]=PAR1_INT;          // Alarmnummer
        ParameterToView[1]=PAR2_ALARMENABLED; // On / Off en de tijd
        ParameterToView[2]=PAR2_TIME;         // tijd
        ParameterToView[3]=PAR2_WDAY;         // dag van de week of een wildcard
        break;

      case CMD_BREAK_ON_TIME_LATER:
      case CMD_BREAK_ON_TIME_EARLIER:
      case CMD_CLOCK_TIME:
        ParameterToView[0]=PAR2_TIME;         // tijd
        break;

      case CMD_CLOCK_DATE:
        ParameterToView[0]=PAR2_DATE;         // datum
        break;

      // Par1 en Par2 samengesteld voor weergave van COMMAND <nummer> , <analoge waarde>
      case CMD_BREAK_ON_VAR_EQU:
      case CMD_BREAK_ON_VAR_LESS:
      case CMD_BREAK_ON_VAR_MORE:
      case CMD_BREAK_ON_VAR_NEQU:
      case CMD_VARIABLE_SET:
      case CMD_VARIABLE_INC:
      case CMD_VARIABLE_DEC:
      case EVENT_VARIABLE:
        ParameterToView[0]=PAR1_INT;
        ParameterToView[1]=PAR2_FLOAT;
        break;

      // Par2 als hex waarde
      case CMD_RAWSIGNAL_SEND:
      case CMD_RAWSIGNAL_ERASE:
      case EVENT_RAWSIGNAL:
        ParameterToView[0]=PAR2_INT_HEX;
        break;

      case CMD_WIRED_SMITTTRIGGER:
      case CMD_WIRED_THRESHOLD:
      case CMD_WIRED_ANALOG:
      case CMD_VARIABLE_RECEIVE:
        ParameterToView[0]=PAR1_INT;
        ParameterToView[1]=PAR2_INT;
        break;

      case VALUE_BUILD:
      case VALUE_HWCONFIG:
      case VALUE_FREEMEM:
      case CMD_PORT_SERVER:
      case CMD_PORT_CLIENT:
        ParameterToView[0]=PAR2_INT;
        break;
        
      case CMD_WAIT_EVENT:
        if(Event->Par1==VALUE_ALL)
          ParameterToView[0]=PAR1_TEXT;
        else
          ParameterToView[0]=PAR1_INT;
  
        ParameterToView[1]=PAR2_TEXT;
        break;

      // Par1 als waarde en par2 als tekst
      case CMD_DELAY:
      case CMD_VARIABLE_SEND:
      case CMD_WIRED_PULLUP:
      case CMD_WIRED_OUT:
      case EVENT_WIRED_IN:
        ParameterToView[0]=PAR1_INT;
        ParameterToView[1]=PAR2_TEXT;
        break;

        // Par1 als tekst en par2 als tekst
      case CMD_OUTPUT:
        ParameterToView[0]=PAR1_TEXT;
        ParameterToView[1]=PAR2_TEXT;
        break;

      case EVENT_WILDCARD:
        ParameterToView[0]=PAR1_TEXT;
        ParameterToView[1]=PAR2_TEXT;
        ParameterToView[2]=PAR3_INT;
        break;

      // Par1 als tekst en par2 als getal
      case CMD_STATUS:
        ParameterToView[0]=PAR1_TEXT;
        ParameterToView[1]=PAR2_INT;
        break;

      // Par1 als tekst en par2 niet
      case CMD_RAWSIGNAL_RECEIVE:
      case CMD_SEND_EVENT:
      case CMD_LOCK:
      case CMD_RAWSIGNAL_SAVE:
      case CMD_DEBUG:
      case CMD_LOG:
      case CMD_ECHO:
      case CMD_WAITFREERF:
      case CMD_BREAK_ON_DAYLIGHT:
        ParameterToView[0]=PAR1_TEXT;
        break;

      // Par1 als waarde en par2 niet
      case CMD_EVENTLIST_SHOW:
      case CMD_EVENTLIST_ERASE:
      case EVENT_TIMER:
      case EVENT_ALARM:
      case EVENT_BOOT:
      case EVENT_NEWNODO:
      case CMD_UNIT_SET:
      case CMD_HOME_SET:
      case CMD_VARIABLE_PULSE_TIME:
      case CMD_VARIABLE_PULSE_COUNT:
        ParameterToView[0]=PAR1_INT;
        break;

      // Par2 int en par1 als tekst
      case CMD_SENDTO:
      case CMD_FILE_EXECUTE:
        ParameterToView[0]=PAR2_INT;
        ParameterToView[1]=PAR1_TEXT;
        break;
        
      // geen parameters.
      case CMD_REBOOT:
      case CMD_CLOCK_SYNC:
      case CMD_RESET:
      case CMD_STOP:
      case CMD_SETTINGS_SAVE:
        break;
        
      // Twee getallen en een aanvullende tekst
      case EVENT_MESSAGE:
        ParameterToView[0]=PAR1_MESSAGE;
        break;

      // Par1 als waarde en par2 als waarde
      default:
        ParameterToView[0]=PAR1_INT;
        ParameterToView[1]=PAR2_INT;
      }
    
    for(byte p=0;p<=3;p++)
      {
      switch (ParameterToView[p])
        {
        case PAR1_INT:
          strcat(EventString,int2str(Event->Par1));
          break;
          
        case PAR1_TEXT:
          strcat(EventString,cmd2str(Event->Par1));
          break;
        
        case PAR1_MESSAGE:
          strcat(EventString,int2str(Event->Par1-MESSAGE_01+1));
          strcat(EventString, ": ");
          strcat(EventString,cmd2str(Event->Par1));
          break;

        case PAR2_INT:
          strcat(EventString,int2str(Event->Par2));
          break;

        case PAR2_INT8:
          strcat(EventString,int2str(Event->Par2&0xff));
          break;

        case PAR3_INT:
          strcat(EventString,int2str((Event->Par2>>8)&0xff));
          break;

        case PAR4_INT:
          strcat(EventString,int2str((Event->Par2>>16)&0xff));
          break;

        case PAR5_INT:
          strcat(EventString,int2str((Event->Par2>>24)&0xff));
          break;

        case PAR2_TEXT:
          strcat(EventString,cmd2str(Event->Par2 & 0xff));
          break;

        case PAR2_DIM:
          if(Event->Par2==VALUE_OFF || Event->Par2==VALUE_ON)
            strcat(EventString, cmd2str(Event->Par2)); // Print 'On' of 'Off'
          else
            strcat(EventString,int2str(Event->Par2));
          break;

        case PAR2_ALARMENABLED:
          // Alarm on/of zit in de 20e bit van Par2.
          if((Event->Par2>>20)&1)
            strcat(EventString, cmd2str(VALUE_ON)); // Print 'On' of 'Off'
          else
            strcat(EventString, cmd2str(VALUE_OFF)); // Print 'On' of 'Off'
          break;
        
        case PAR2_TIME:
          x=(Event->Par2>>12)&0xf;
          if(x==0xf)
            strcat(EventString, "*");
          else if(x==0xe)
            strcat(EventString, "-");
          else
            strcat(EventString, int2str(x));
          
          x=(Event->Par2>>8)&0xf;
          if(x==0xf)
            strcat(EventString, "*");
          else if(x==0xe)
            strcat(EventString, "-");
          else
            strcat(EventString, int2str(x));
          
          strcat(EventString, ":");

          x=(Event->Par2>>4)&0xf;
          if(x==0xf)
            strcat(EventString, "*");
          else if(x==0xe)
            strcat(EventString, "-");
          else
            strcat(EventString, int2str(x));

          x=(Event->Par2)&0xf;
          if(x==0xf)
            strcat(EventString, "*");
          else if(x==0xe)
            strcat(EventString, "-");
          else
            strcat(EventString, int2str(x));
          break;
          
        case PAR2_DATE:
          strcat(EventString, int2str((Event->Par2>>28)&0xf));
          strcat(EventString, int2str((Event->Par2>>24)&0xf));
            strcat(EventString, "-");
          strcat(EventString, int2str((Event->Par2>>20)&0xf));
          strcat(EventString, int2str((Event->Par2>>16)&0xf));
            strcat(EventString, "-");
          strcat(EventString, int2str((Event->Par2>>12)&0xf));
          strcat(EventString, int2str((Event->Par2>>8)&0xf));
          strcat(EventString, int2str((Event->Par2>>4 )&0xf));
          strcat(EventString, int2str((Event->Par2    )&0xf));
          break;
          
        case PAR2_WDAY:
          // dag van de week zit in bits 17,18, 19 en 20. Waarde 0xf is wildcard, andere waarden vertegenwoordigen de dag van de week.
          x=(Event->Par2>>16)&0xf;
          if(x==0xf || x==0)
            strcat(EventString, "*");
          else if(x==0xe)
            strcat(EventString, "---");
          else            
            {
            char *StrPtr=ProgmemString(Text_04)+((x-1)*3);
            *(StrPtr+3)=0;
            strcat(EventString, StrPtr);
            }
          break;

        case PAR2_FLOAT:
          // Een float en een unsigned long zijn beide 4bytes groot. We gebruiken ruimte van Par2 om een float in op te slaan
          float f;
          memcpy(&f, &Event->Par2, 4);
          dtostrf(f, 0, 2,EventString+strlen(EventString)); // Kaboem... 2100 bytes programmacode extra ! Gelukkig alleen voor de mega.
          break;

        case PAR2_INT_HEX:
          strcat(EventString,int2strhex(Event->Par2));
          break;

        }

      if(ParameterToView[p+1]!=0)// komt er nog een vervolgparameter, dan een komma als scheidingsteken.
        strcat(EventString,",");      
      }
    }          
  }

   
 /*******************************************************************************************************\
 * Deze funktie parsed een string zoals die door de gebruiker wordt opgegeven. De commando's
 * worden gechecked op geldigheid en ter uitvoering doorgegeven.
 * Commando's en events worden gescheiden met een puntkomma.
 \*******************************************************************************************************/
int ExecuteLine(char *Line, byte Port)
  {
  const int MaxCommandLength=40; // maximaal aantal tekens van een commando
  char Command[MaxCommandLength+1]; 
  char *TmpStr1=(char*)malloc(MaxCommandLength+1);
  char *TmpStr2=(char*)malloc(INPUT_BUFFER_SIZE+2);
  int CommandPos;
  int LinePos;
  int w,x,y;
  int EventlistWriteLine=0;
  byte error=0, State_EventlistWrite=0;
  unsigned long a;
  struct NodoEventStruct EventToExecute,TempEvent;

  Led(RED);

  // Als de SendTo niet permanent is ingeschakeld, dan deze weer uitzetten
  if(Transmission_SendToAll!=VALUE_ALL)Transmission_SendToUnit=0;

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
      AddFileSDCard(TempLogFile,Line); // Extra logfile op verzoek van gebruiker
    }
  else
    {
    if(Substitute(Line)!=0)
      error=MESSAGE_02;

    CommandPos=0;
    LinePos=0;    
    int LineLength=strlen(Line);
    while(LinePos<=LineLength && error==0)
      {
      char LineChar=Line[LinePos];
  
      // Comment teken. hierna verder niets meer doen.
      if(LineChar=='!') 
        LinePos=LineLength+1; // ga direct naar einde van de regel.

      // als puntkomma (scheidt opdrachten) of einde string
      if((LineChar=='$' || LineChar=='!' || LineChar==';' || LineChar==0) && CommandPos>0)
        {
        Command[CommandPos]=0;
        CommandPos=0;
        ClearEvent(&EventToExecute);
        EventToExecute.Port=Port;
        
        GetArgv(Command,TmpStr1,1);
        EventToExecute.Command=str2cmd(TmpStr1); 

        // Haal Par1 uit het commando.
        if(GetArgv(Command,TmpStr1,2))
          {
          EventToExecute.Par1=str2cmd(TmpStr1);
          if(!EventToExecute.Par1)
            EventToExecute.Par1=str2int(TmpStr1);
          }
        
        // Haal Par2 uit het commando.
        if(GetArgv(Command,TmpStr1,3))
          {
          EventToExecute.Par2=str2cmd(TmpStr1);
          if(!EventToExecute.Par2)
            EventToExecute.Par2=str2int(TmpStr1);
          }        

        switch(EventToExecute.Command)
          {
          //test; geen, altijd goed
          case EVENT_CLOCK_DAYLIGHT:
          case EVENT_USEREVENT:
            EventToExecute.Type=NODO_TYPE_EVENT;
            break; 

          //test; geen, altijd goed
          case CMD_EVENTLIST_ERASE:
          case CMD_STOP:
          case CMD_RESET:
          case CMD_REBOOT:
          case CMD_SETTINGS_SAVE:
          case CMD_STATUS:
          case CMD_DELAY:
          case CMD_SOUND: 
          case CMD_SEND_USEREVENT:
          case CMD_CLOCK_SYNC:
            EventToExecute.Type=NODO_TYPE_COMMAND;
            break; 
      
          case CMD_LOG:
          case CMD_RAWSIGNAL_RECEIVE:
          case CMD_RAWSIGNAL_SAVE:
          case CMD_DEBUG:
          case CMD_ECHO:
            EventToExecute.Type=NODO_TYPE_COMMAND;
            if(EventToExecute.Par1!=VALUE_OFF && EventToExecute.Par1!=VALUE_ON)
              error=MESSAGE_02;
           break;
                        
          case EVENT_MESSAGE:
            EventToExecute.Type=NODO_TYPE_EVENT; 
            if(EventToExecute.Par1 <1 || (EventToExecute.Par1+MESSAGE_01) > COMMAND_MAX)
              error=MESSAGE_02;
            else
              EventToExecute.Par1 += (MESSAGE_01-1);
            break;
              
          case CMD_TIMER_SET:
            EventToExecute.Type=NODO_TYPE_COMMAND;
            if(EventToExecute.Par1>TIMER_MAX)
              error=MESSAGE_02;
            break;
                  
          case CMD_TIMER_SET_VARIABLE:
            EventToExecute.Type=NODO_TYPE_COMMAND;
            if(EventToExecute.Par1>TIMER_MAX || EventToExecute.Par2<1 || EventToExecute.Par2>USER_VARIABLES_MAX)
              error=MESSAGE_02;
            break;
                  
          case CMD_WAIT_EVENT:
            EventToExecute.Type=NODO_TYPE_COMMAND;
            if((EventToExecute.Par1<1 || EventToExecute.Par1>UNIT_MAX) &&  EventToExecute.Par1!=VALUE_ALL)
              error=MESSAGE_02;
            break;
      
          case EVENT_ALARM:
            EventToExecute.Type=NODO_TYPE_EVENT;
            if(EventToExecute.Par1<1 || EventToExecute.Par1>ALARM_MAX)
              error=MESSAGE_02;

          case EVENT_NEWNODO:
          case EVENT_BOOT:
            EventToExecute.Type=NODO_TYPE_EVENT;
            if(EventToExecute.Par1<1 || EventToExecute.Par1>UNIT_MAX)
              error=MESSAGE_02;
            break;

          case CMD_UNIT_SET:
            EventToExecute.Type=NODO_TYPE_COMMAND;
            if(EventToExecute.Par1<1 || EventToExecute.Par1>UNIT_MAX)
              error=MESSAGE_02;
            break;
      
          case CMD_HOME_SET:
            EventToExecute.Type=NODO_TYPE_COMMAND;
            if(EventToExecute.Par1<1 || EventToExecute.Par1>HOME_MAX)
              error=MESSAGE_02;
            break;
      
          case CMD_ANALYSE_SETTINGS:
            EventToExecute.Type=NODO_TYPE_COMMAND;
            if(EventToExecute.Par1<1 || EventToExecute.Par1>50)
              error=MESSAGE_02;
            break;
      
          case CMD_VARIABLE_PULSE_TIME:
          case CMD_VARIABLE_PULSE_COUNT:
            EventToExecute.Type=NODO_TYPE_COMMAND;
            if(EventToExecute.Par1<1 || EventToExecute.Par1>USER_VARIABLES_MAX)
              error=MESSAGE_02;
            break;

          case CMD_BREAK_ON_VAR_LESS_VAR:
          case CMD_BREAK_ON_VAR_MORE_VAR:
          case CMD_VARIABLE_VARIABLE:
          case CMD_VARIABLE_RECEIVE:
            EventToExecute.Type=NODO_TYPE_COMMAND;
            if(EventToExecute.Par1<1 || EventToExecute.Par1>USER_VARIABLES_MAX)
              error=MESSAGE_02;
            if(EventToExecute.Par2<1 || EventToExecute.Par2>USER_VARIABLES_MAX)
              error=MESSAGE_02;
            break;
              
          case CMD_VARIABLE_SET_WIRED_ANALOG:
            EventToExecute.Type=NODO_TYPE_COMMAND;
            if(EventToExecute.Par1<1 || EventToExecute.Par1>USER_VARIABLES_MAX)
              error=MESSAGE_02;
            if(EventToExecute.Par2<1 || EventToExecute.Par2>WIRED_PORTS)
              error=MESSAGE_02;
            break;
              
          // test:EventToExecute.Par1 binnen bereik maximaal beschikbare timers
          case CMD_TIMER_RANDOM:
            EventToExecute.Type=NODO_TYPE_COMMAND;
            if(EventToExecute.Par1<1 || EventToExecute.Par1>TIMER_MAX)
              error=MESSAGE_02;
            break;

          // test:EventToExecute.Par1 binnen bereik maximaal beschikbare timers
          case EVENT_TIMER:
            EventToExecute.Type=NODO_TYPE_EVENT;
            if(EventToExecute.Par1<1 || EventToExecute.Par1>TIMER_MAX)
              error=MESSAGE_02;
            break;
                      
          // geldige tijd    
          case CMD_BREAK_ON_TIME_LATER:
          case CMD_BREAK_ON_TIME_EARLIER:
          case CMD_CLOCK_TIME:
            EventToExecute.Type=NODO_TYPE_COMMAND;
            error=MESSAGE_02;
            if(GetArgv(Command,TmpStr1,2))
              {
              EventToExecute.Par1=0;
              if((EventToExecute.Par2=str2ultime(TmpStr1))!=0xffffffff)
                error=0;
              }
              
            break;
    
          // geldige datum
          case CMD_CLOCK_DATE:
            EventToExecute.Type=NODO_TYPE_COMMAND;
            // datum in Par2 volgens format DDMMYYYY. 
            EventToExecute.Par1=0;
            if((EventToExecute.Par2=str2uldate(Command))==0xffffffff)
              error=MESSAGE_02;
            break;
            
          // test:EventToExecute.Par1 binnen bereik maximaal beschikbare wired poorten.
          case EVENT_WIRED_IN:
            EventToExecute.Type=NODO_TYPE_EVENT;
            if(EventToExecute.Par1<1 || EventToExecute.Par1>WIRED_PORTS)
              error=MESSAGE_02;
            if(EventToExecute.Par2!=VALUE_ON && EventToExecute.Par2!=VALUE_OFF)
              error=MESSAGE_02;
            break;
      
          case CMD_WIRED_OUT:
            EventToExecute.Type=NODO_TYPE_COMMAND;
            if(EventToExecute.Par1>WIRED_PORTS)
              error=MESSAGE_02;
            if(EventToExecute.Par2!=VALUE_ON && EventToExecute.Par2!=VALUE_OFF)
              error=MESSAGE_02;
            break;
      
          case CMD_WIRED_PULLUP:
            EventToExecute.Type=NODO_TYPE_COMMAND;
            if(EventToExecute.Par1<1 || EventToExecute.Par1>WIRED_PORTS)
              error=MESSAGE_02;
            if(EventToExecute.Par2!=VALUE_ON && EventToExecute.Par2!=VALUE_OFF)
              error=MESSAGE_02;
            break;

          
          case CMD_SEND_EVENT:
            EventToExecute.Type=NODO_TYPE_COMMAND;

            if(EventToExecute.Par2==0);
              EventToExecute.Par2=VALUE_ALL;

            switch(EventToExecute.Par1)
              {
              case VALUE_ALL:
              case VALUE_SOURCE_I2C:
              case VALUE_SOURCE_IR:
              case VALUE_SOURCE_RF:
              case VALUE_SOURCE_HTTP:
                break;
              default:
                error=MESSAGE_02;
              }
            break;
      
          case CMD_VARIABLE_SEND:
            EventToExecute.Type=NODO_TYPE_COMMAND;
            if(EventToExecute.Par1>USER_VARIABLES_MAX)
              error=MESSAGE_02;

            if(EventToExecute.Par2==0);
              EventToExecute.Par2=VALUE_ALL;
              
            switch(EventToExecute.Par2)
              {
              case VALUE_ALL:
              case VALUE_SOURCE_I2C:
              case VALUE_SOURCE_IR:
              case VALUE_SOURCE_RF:
              case VALUE_SOURCE_HTTP:
                break;
              default:
                error=MESSAGE_02;
              }
            break;
      
          case EVENT_WILDCARD:
            EventToExecute.Type=NODO_TYPE_EVENT;
            switch(EventToExecute.Par1)
              {
              case VALUE_ALL:
              case VALUE_SOURCE_THISUNIT:
              case VALUE_SOURCE_EVENTLIST:
              case VALUE_SOURCE_WIRED:
              case VALUE_SOURCE_I2C:
              case VALUE_SOURCE_IR:
              case VALUE_SOURCE_RF:
              case VALUE_SOURCE_SERIAL:
              case VALUE_SOURCE_HTTP:
              case VALUE_SOURCE_TELNET:
                break;
              default:
                error=MESSAGE_02;
              }
              
            switch(EventToExecute.Par2)
              {
              case VALUE_ALL:
              case EVENT_USEREVENT:
              case EVENT_CLOCK_DAYLIGHT:
              case EVENT_TIME:
              case EVENT_RAWSIGNAL:
              case EVENT_TIMER:
              case EVENT_WIRED_IN:
              case EVENT_VARIABLE:
              case EVENT_NEWNODO:
              case EVENT_MESSAGE:
              case EVENT_BOOT:
              case EVENT_ALARM:
                break;
              default:
                error=MESSAGE_02;
              }
              
            if(GetArgv(Command,TmpStr1,4))
              EventToExecute.Par2|=(str2int(TmpStr1)<<8);

            break;
            
           // par1 alleen On of Off.
           // par2 mag alles zijn
          case CMD_BREAK_ON_DAYLIGHT:
          case CMD_WAITFREERF:
            EventToExecute.Type=NODO_TYPE_COMMAND;
            if(EventToExecute.Par1!=VALUE_OFF && EventToExecute.Par1!=VALUE_ON)
              error=MESSAGE_02;
            break;
      
          case CMD_OUTPUT:
            EventToExecute.Type=NODO_TYPE_COMMAND;
            if(EventToExecute.Par1!=VALUE_SOURCE_I2C && EventToExecute.Par1!=VALUE_SOURCE_IR && EventToExecute.Par1!=VALUE_SOURCE_RF && EventToExecute.Par1!=VALUE_SOURCE_HTTP)
              error=MESSAGE_02;
            if(EventToExecute.Par2!=VALUE_OFF && EventToExecute.Par2!=VALUE_ON)
              error=MESSAGE_02;
            break;

          case CMD_SENDTO:
            EventToExecute.Type=NODO_TYPE_COMMAND;
            Transmission_SendToAll=EventToExecute.Par2;
            if(EventToExecute.Par1<=UNIT_MAX)
              {
              Transmission_SendToUnit=EventToExecute.Par1;
              }
            else
              Transmission_SendToUnit=0;
            ExecuteCommand(&EventToExecute);            
            EventToExecute.Command=0;
            break;    

          case CMD_LOCK: // Hier wordt de lock code o.b.v. het wachtwoord ingesteld. Alleen van toepassing voor de Mega
            EventToExecute.Type=NODO_TYPE_COMMAND;
            EventToExecute.Par2=0L;
            for(x=0;x<strlen(Settings.Password);x++)
              {
              EventToExecute.Par2=EventToExecute.Par2<<5;
              EventToExecute.Par2^=Settings.Password[x];
              }
            break;

          case CMD_VARIABLE_SET:
            EventToExecute.Type=NODO_TYPE_COMMAND;
            if(EventToExecute.Par1>USER_VARIABLES_MAX)
              error=MESSAGE_02;
            else if(GetArgv(Command,TmpStr1,3))// waarde van de variabele
              EventToExecute.Par2=float2ul(atof(TmpStr1));
            else
              error=MESSAGE_02;
            break;

          case CMD_BREAK_ON_VAR_EQU:
          case CMD_BREAK_ON_VAR_LESS:
          case CMD_BREAK_ON_VAR_MORE:
          case CMD_BREAK_ON_VAR_NEQU:
          case CMD_VARIABLE_DEC:
          case CMD_VARIABLE_INC:
            EventToExecute.Type=NODO_TYPE_COMMAND;
            if(EventToExecute.Par1<1 || EventToExecute.Par1>USER_VARIABLES_MAX)
              error=MESSAGE_02;
            else if(GetArgv(Command,TmpStr1,3))// waarde van de variabele
              EventToExecute.Par2=float2ul(atof(TmpStr1));
            else
              error=MESSAGE_02;
            break;

          case EVENT_VARIABLE:
            EventToExecute.Type=NODO_TYPE_EVENT;
            if(EventToExecute.Par1<1 || EventToExecute.Par1>USER_VARIABLES_MAX)
              error=MESSAGE_02;
            else if(GetArgv(Command,TmpStr1,3))// waarde van de variabele
              EventToExecute.Par2=float2ul(atof(TmpStr1));
            else
              error=MESSAGE_02;
            break;

          case CMD_WIRED_ANALOG:
          case CMD_WIRED_THRESHOLD:
          case CMD_WIRED_SMITTTRIGGER:
            EventToExecute.Type=NODO_TYPE_COMMAND;
            if(EventToExecute.Par1<1 || EventToExecute.Par1>WIRED_PORTS)
              error=MESSAGE_02;
            else if(GetArgv(Command,TmpStr1,3))
              EventToExecute.Par2=str2int(TmpStr1);
            break;
              
          case CMD_EVENTLIST_WRITE:
            EventToExecute.Type=NODO_TYPE_COMMAND;
            if(Transmission_SendToUnit==Settings.Unit || Transmission_SendToUnit==0)
              {                          
              if(EventToExecute.Par1<=EventlistMax)
                {
                EventlistWriteLine=EventToExecute.Par1;
                State_EventlistWrite=1;
                EventToExecute.Command=0;// geen verdere verwerking
                }
              else
                error=MESSAGE_02;
              }
            break;

          case CMD_PASSWORD:
            {
            EventToExecute.Type=NODO_TYPE_COMMAND;
            TmpStr1[0]=0;
            GetArgv(Command,TmpStr1,2);
            TmpStr1[25]=0; // voor geval de string te lang is.
            strcpy(Settings.Password,TmpStr1);

//            // Als een lock actief, dan lock op basis van nieuwe password instellen
//            if(Settings.Lock)
//              {
//              a=0L;
//              for(x=0;x<strlen(Settings.Password);x++)
//                {
//                a=a<<5;
//                a^=Settings.Password[x];
//                }
//              Settings.Lock=a&0x7fff;
//              }//??? lock nog aanpassen.

            EventToExecute.Command=0; // Geen verdere verwerking meer nodig.
            break;
            }  

          case CMD_ID:
            {
            EventToExecute.Type=NODO_TYPE_COMMAND;
            TmpStr1[0]=0;
            GetArgv(Command,TmpStr1,2);
            TmpStr1[9]=0; // voor geval de string te lang is.
            strcpy(Settings.ID,TmpStr1);
            EventToExecute.Command=0; // Geen verdere verwerking meer nodig.
            break;
            }  

          case CMD_TEMP:
            {
            EventToExecute.Type=NODO_TYPE_COMMAND;
            x=StringFind(Command,cmd2str(CMD_TEMP))+strlen(cmd2str(CMD_TEMP));
            while(Command[x]==' ')x++;             // eventuele spaties verwijderen
            strcpy(TmpStr1,Command+x);             // Alles na de "temp" hoort bij de variabele
            TmpStr1[25]=0;                         // voor geval de string te lang is.
            strcpy(Settings.Temp,TmpStr1);
            EventToExecute.Command=0; // Geen verdere verwerking meer nodig.
            break;
            }  
          
          case EVENT_RAWSIGNAL:
            EventToExecute.Type=NODO_TYPE_EVENT;
            if(GetArgv(Command,TmpStr1,2))
              {
              EventToExecute.Par1=0;
              EventToExecute.Par2=str2int(TmpStr1);
              }
            break;

          case CMD_ALARM_SET:
            EventToExecute.Type=NODO_TYPE_COMMAND;
            // Commando format: [AlarmSet <AlarmNumber 1..4>, <Enabled On|Off>, <Time HHMM>, <Day 1..7>]
            //                  [Time <Time HHMM>, <Day 1..7>]
            // We moeten wat truucs uithalen om al deze info in een 32-bit variabele te krijgen.
            // Alarmtijd wordt in Par2 opgeslagen volgens volgende format: MSB-EEEEWWWWAAAABBBBCCCCDDDD-LSB
            // E=Enabled, WWWW=weekdag, AAAA=Uren tiental, BBBB=uren, CCCC=minuten tiental DDDD=minuten
            // Als een deel gevuld met 0xE, dan waarde niet setten.
            // Als gevuld met 0xF, dan wildcard.             
            if(GetArgv(Command,TmpStr1,2)) // Alarm number
              {
              error=MESSAGE_02;
              EventToExecute.Par1=str2int(TmpStr1);
              EventToExecute.Par2=0xEEEEEEEE; 
              
              if(EventToExecute.Par1>0 && EventToExecute.Par1<=ALARM_MAX)
                {
                if(GetArgv(Command,TmpStr1,3)) // Enabled
                  {
                  x=str2cmd(TmpStr1);
                  if(x==VALUE_ON || x==VALUE_OFF)
                    {
                    EventToExecute.Par2&=0xff0fffff;
                    EventToExecute.Par2|=(unsigned long)(x==VALUE_ON)<<20; // Enabled bit setten.
                    error=0;
                    if(GetArgv(Command,TmpStr1,4)) // Minutes
                      {
                      EventToExecute.Par2&=0xffff0000;
                      y=0;
                      for(x=strlen(TmpStr1)-1;x>=0;x--)
                        {
                        w=TmpStr1[x];
                        if(w>='0' && w<='9' || w=='*')
                          {
                          if(w=='*')
                            EventToExecute.Par2|=(0xFUL<<y); // vul nibble met wildcard
                          else
                            EventToExecute.Par2|=(w-'0')<<y; // vul nibble met token
                          y+=4;
                          }
                        else if(w==':');
                        else
                          {
                          error=MESSAGE_02;
                          break;
                          }
                        }
                      if(GetArgv(Command,TmpStr1,5)) // Day is optioneel. Maar als deze parameter ingevuld, dan meenemen in de berekening.
                        y=str2weekday (TmpStr1);
                      else // Dag niet opgegeven
                        y=0xF;

                      EventToExecute.Par2&=0xfff0ffff;
                      if(y!=-1)
                        EventToExecute.Par2|=(unsigned long)y<<16;
                      else
                        error=MESSAGE_02;
                      }
                    }
                  }
                }
              }
            else 
              error=MESSAGE_02;

            break;
          
          case EVENT_TIME:
            // Event format: [Time <Time HHMM>, <Day 1..7>]
            // We moeten wat truucs uithalen om al deze info in een 32-bit variabele te krijgen.
            // Tijd wordt in Par2 opgeslagen volgens volgende format: MSB-0000WWWWAAAABBBBCCCCDDDD-LSB
            // WWWW=weekdag, AAAA=Uren tiental, BBBB=uren, CCCC=minuten tiental DDDD=minuten
            {              
            EventToExecute.Type=NODO_TYPE_EVENT;
            if(GetArgv(Command,TmpStr1,2)) // Minutes
              {
              EventToExecute.Par2=0L; 
              y=0;
              for(x=strlen(TmpStr1)-1;x>=0;x--)
                {
                w=TmpStr1[x];
                if(w>='0' && w<='9' || w=='*')
                  {
                  a=0xffffffff  ^ (0xfUL <<y); // Mask maken om de nibble positie y te wissen.
                  EventToExecute.Par2&=a; // maak nibble leeg
                  if(w=='*')
                    EventToExecute.Par2|=(0xFUL<<y); // vul nibble met wildcard
                  else
                    EventToExecute.Par2|=(w-'0')<<y; // vul nibble met token
                  y+=4;
                  }
                else if(w==':');
                else
                  {
                  error=MESSAGE_02;
                  break;
                  }
                }
              if(GetArgv(Command,TmpStr1,3)) // Day is optioneel. Maar als deze parameter ingevuld, dan meenemen in de berekening.
                y=str2weekday(TmpStr1);
              else // Dag niet opgegeven
                y=0xF;

              EventToExecute.Par2&=0xfff0ffff;
              if(y!=-1)
                EventToExecute.Par2|=(unsigned long)y<<16;
              else
                error=MESSAGE_02;
              }
            else 
              error=MESSAGE_02;
            break;
            }

          case CMD_FILE_LOG:
            EventToExecute.Type=NODO_TYPE_COMMAND;
            if(GetArgv(Command,TmpStr1,2))
              {
              strcat(TmpStr1,".dat");
              strcpy(TempLogFile,TmpStr1);
              }
            else
              TempLogFile[0]=0;
            EventToExecute.Command=0; // Geen verdere verwerking meer nodig.
            break;
          
          case CMD_FILE_ERASE:      
            EventToExecute.Type=NODO_TYPE_COMMAND;
            if(GetArgv(Command,TmpStr1,2))
              {
              strcat(TmpStr1,".dat");
              FileErase(TmpStr1);
              }
            else
              FileList("",true,Port);

            EventToExecute.Command=0; // Geen verdere verwerking meer nodig.
            break;
    
          case CMD_RAWSIGNAL_ERASE:      
          case CMD_RAWSIGNAL_SEND:      
            EventToExecute.Type=NODO_TYPE_COMMAND;
            // Haal Par1 uit het commando. let op Par1 gebruiker is een 32-bit hex-getal die wordt opgeslagen in struct Par2.
            if(GetArgv(Command,TmpStr1,2))
              {
              EventToExecute.Par2=str2int(TmpStr1);
              if(GetArgv(Command,TmpStr1,3))
                EventToExecute.Par1=str2cmd(TmpStr1);
              }
            break;
    
          case CMD_FILE_GET_HTTP:
            {
            EventToExecute.Type=NODO_TYPE_COMMAND;
            if(GetArgv(Command,TmpStr1,2))
              {
              Led(BLUE);
              GetHTTPFile(TmpStr1);
              }
            else
              error=MESSAGE_02;            
            EventToExecute.Command=0; // Geen verdere verwerking meer nodig.
            break; 
            }
            
          case CMD_FILE_SHOW:
            {
            EventToExecute.Type=NODO_TYPE_COMMAND;
            char FileName[13];
            TmpStr1[8]=0;// voor de zekerheid te lange filename afkappen
            if(GetArgv(Command,FileName,2))
              {
              Led(BLUE);
              strcat(FileName,".dat");
              error=FileShow(FileName,Port);
              }
            EventToExecute.Command=0; // Geen verdere verwerking meer nodig.
            break;
            }
    
          case CMD_FILE_EXECUTE:
            {
            EventToExecute.Type=NODO_TYPE_COMMAND;
            if(EventToExecute.Par2==VALUE_ON)
              EventToExecute.Par1=VALUE_ON;
            else
              EventToExecute.Par1=VALUE_OFF;
            
            if(GetArgv(Command,TmpStr1,2))
              {
              EventToExecute.Par2=str2int(TmpStr1);
              if(State_EventlistWrite==0)
                {
                // Commando uitvoeren heeft alleen zin er geen eventlistwrite commando actief is
                error=FileExecute(TmpStr1, EventToExecute.Par1==VALUE_ON);
                EventToExecute.Command=0; // Geen verdere verwerking meer nodig.
                }
              }
            break;
            }
    
          case CMD_EVENTLIST_SHOW:
            EventToExecute.Type=NODO_TYPE_COMMAND;
            if(EventToExecute.Par1<=EventlistMax)
              {
              PrintString(ProgmemString(Text_22),Port);
              if(EventToExecute.Par1==0)
                {
                x=1;
                while(EventlistEntry2str(x++,0,TmpStr2,false))
                  if(TmpStr2[0]!=0)
                    PrintString(TmpStr2,Port);
                }
              else
                {
                EventlistEntry2str(EventToExecute.Par1,0,TmpStr2,false);
                  if(TmpStr2[0]!=0)
                    PrintString(TmpStr2,Port);
                }
              PrintString(ProgmemString(Text_22),Port);
              EventToExecute.Command=0; // Geen verdere verwerking meer nodig.
              }
            else
              error=MESSAGE_02;
            break;
              
          case CMD_NODO_IP:
            EventToExecute.Type=NODO_TYPE_COMMAND;
            if(GetArgv(Command,TmpStr1,2))
              if(!str2ip(TmpStr1,Settings.Nodo_IP))
                error=MESSAGE_02;
            EventToExecute.Command=0; // Geen verdere verwerking meer nodig.
            break;
            
          case CMD_CLIENT_IP:
            EventToExecute.Type=NODO_TYPE_COMMAND;
            if(GetArgv(Command,TmpStr1,2))
              if(!str2ip(TmpStr1,Settings.Client_IP))
                error=MESSAGE_02;
            EventToExecute.Command=0; // Geen verdere verwerking meer nodig.
            break;
            
          case CMD_SUBNET:
            EventToExecute.Type=NODO_TYPE_COMMAND;
            if(GetArgv(Command,TmpStr1,2))
              if(!str2ip(TmpStr1,Settings.Subnet))
                error=MESSAGE_02;
            break;
            
          case CMD_DNS_SERVER:
            EventToExecute.Type=NODO_TYPE_COMMAND;
            if(GetArgv(Command,TmpStr1,2))
              if(!str2ip(TmpStr1,Settings.DnsServer))
                error=MESSAGE_02;
            EventToExecute.Command=0; // Geen verdere verwerking meer nodig.
            break;
            
          case CMD_GATEWAY:
            EventToExecute.Type=NODO_TYPE_COMMAND;
            if(GetArgv(Command,TmpStr1,2))
              if(!str2ip(TmpStr1,Settings.Gateway))
                error=MESSAGE_02;
            EventToExecute.Command=0; // Geen verdere verwerking meer nodig.
            break;
            
          case CMD_EVENTLIST_FILE:
            EventToExecute.Type=NODO_TYPE_COMMAND;
            Led(BLUE);
            if(GetArgv(Command,TmpStr1,2))
              {
              strcat(TmpStr1,".dat");
              if(!SaveEventlistSDCard(TmpStr1))
                {
                error=MESSAGE_14;
                break;
                }
              }
            EventToExecute.Command=0; // Geen verdere verwerking meer nodig.
            break;

          case CMD_IF:
            {
            EventToExecute.Type=NODO_TYPE_COMMAND;
            x=StringFind(Command," ") ;           // laat x wijzen direct NA het if commando.
            strcpy(TmpStr1,Command+x);            // Alles na de "if" hoort bij de voorwaarde

            // eventuele spaties er uithalen
            y=0;
            for(x=0;x<strlen(TmpStr1);x++)
              {
              if(TmpStr1[x]!=' ')
                TmpStr1[y++]=TmpStr1[x];
              }
            TmpStr1[y]=0;
            
            // Zoek '=' teken op en splitst naar linker en rechter operand.
            x=StringFind(TmpStr1,"<>");
            if(x!=-1)
              {
              strcpy(TmpStr2,TmpStr1+x+2);
              TmpStr1[x]=0;
              if(strcasecmp(TmpStr1,TmpStr2)==0)
                LinePos=LineLength+1; // ga direct naar einde van de regel.
              }
              
            x=StringFind(TmpStr1,"=");
            if(x!=-1)
              {
              strcpy(TmpStr2,TmpStr1+x+1);
              TmpStr1[x]=0;
              if(strcasecmp(TmpStr1,TmpStr2)!=0)
                LinePos=LineLength+1; // ga direct naar einde van de regel.
              }
              
            else
              error=MESSAGE_02;
            EventToExecute.Command=0; // Geen verdere verwerking meer nodig.
            break;
            }            

          case CMD_RAWSIGNAL_LIST:
            FileList("/RAW",false,Port);
            EventToExecute.Command=0; // Geen verdere verwerking meer nodig.
            break;

          case CMD_FILE_LIST:
            FileList("/",false,Port);
            EventToExecute.Command=0; // Geen verdere verwerking meer nodig.
            break;

          case CMD_FILE_WRITE:
            if(GetArgv(Command,TmpStr1,2) && strlen(TmpStr1)<=8)
              {
              strcat(TmpStr1,".dat");
              strcpy(TempLogFile,TmpStr1);
              FileWriteMode=60;
              EventToExecute.Command=0; // Geen verdere verwerking meer nodig.
              }
            else
              error=MESSAGE_02;
            break;

          case CMD_HTTP_REQUEST:
            // zoek in de regel waar de string met het http request begint.
            x=StringFind(Line,cmd2str(CMD_HTTP_REQUEST))+strlen(cmd2str(CMD_HTTP_REQUEST));
            while(Line[x]==32)x++;
            strcpy(Settings.HTTPRequest,&Line[0]+x);
            EventToExecute.Command=0; // Geen verdere verwerking meer nodig.
            break;

          case CMD_PORT_SERVER:
          case CMD_PORT_CLIENT:
            {
            EventToExecute.Type=NODO_TYPE_COMMAND;
            if(GetArgv(Command,TmpStr1,2))
              EventToExecute.Par2=str2int(TmpStr1);
            if(EventToExecute.Par2 > 0xffff)
              error=MESSAGE_02;
            break;
            }  

          default:
            {              
            // Ingevoerde commando is niet gevonden. 
            // Loop de devices langs om te checken if er een hit is. Zo ja, dan heeft de Device_x funktie de struct
            // met de juiste waarden gevuld. 
            y=false;
            for(x=0; !y && x<DEVICE_MAX; x++)
              if(Device_ptr[x]!=0)
                {
                EventToExecute.Command=Device_id[x];
                if(Device_ptr[x](DEVICE_MMI_IN,&EventToExecute,Command))
                   y=true;
                }

            if(!y)
              {
              // Als het geen regulier commando was EN geen commando met afwijkende MMI en geen Device, dan kijken of file op SDCard staat)
              error=FileExecute(Command, EventToExecute.Par2==VALUE_ON);
              EventToExecute.Command=0;
      
              // als script niet te openen, dan is het ingevoerde commando ongeldig.
              if(error==MESSAGE_03)
                error=MESSAGE_01;
              }
            }
          }// switch(command...@2
            
        if(EventToExecute.Command && error==0)
          {            
          // Er kunnen zich twee situaties voordoen:
          //
          // A: Event is voor deze Nodo en moet gewoon worden uitgevoerd;
          // B: SendTo is actief. Event moet worden verzonden naar een andere Nodo. Hier wordt de Queue voor gebruikt.
          
          if(State_EventlistWrite==0)// Gewoon uitvoeren
            {
            if(Transmission_SendToUnit==Settings.Unit || Transmission_SendToUnit==0)
              {
              EventToExecute.Direction=VALUE_DIRECTION_INPUT;
              error=ProcessEvent1(&EventToExecute);
              }
            else
              {
              if(EventToExecute.Command)          // geen lege events in de queue plaatsen
                QueueAdd(&EventToExecute);        // Plaats in queue voor latere verzending.
              }
            continue;
            }
    
          if(State_EventlistWrite==2)
            {            
            UndoNewNodo();// Status NewNodo verwijderen indien van toepassing
            if(!Eventlist_Write(EventlistWriteLine,&TempEvent,&EventToExecute))
              {
              RaiseMessage(MESSAGE_06);
              break;
              }
            State_EventlistWrite=0;
            continue;
            }  
            
          if(State_EventlistWrite==1)
            {
            TempEvent=EventToExecute; // TempEvent = >Event< dat moet worden weggeschreven in de eventlist;
            State_EventlistWrite=2;
            }
          }
        }// if(LineChar.

      // Tekens toevoegen aan commando zolang er nog ruimte is in de string
      if(LineChar!=';' && CommandPos<MaxCommandLength)
        Command[CommandPos++]=LineChar;      

      LinePos++;
      }// while(LinePos...
  
    // Verzend de inhoud van de queue naar de slave Nodo
    if(Transmission_SendToUnit!=Settings.Unit && Transmission_SendToUnit!=0 && error==0 && QueuePosition>0)
      {
      error=QueueSend();
      if(error)
        {
        CommandPos=0;
        LinePos=0;
        }
      }
    }// einde regel behandeling

  free(TmpStr2);
  free(TmpStr1);

  // Verwerk eventuele events die in de queue zijn geplaatst.
  if(error==0)
    QueueProcess();
    
  return error;
  }

#else

/**********************************************************************************************\
 * Print de welkomsttekst van de Nodo. ATMega328 variant
 \*********************************************************************************************/

void PrintWelcome(void)
  {
  // Print Welkomsttekst
  Serial.println(F("!******************************************************************************!"));
  Serial.println(F("Nodo Domotica controller (c) Copyright 2013 P.K.Tonkes."));
  Serial.println(F("Licensed under GNU General Public License."));
  Serial.print(F("Nodo-Small V"));
  Serial.print(NODO_VERSION/10);
  Serial.print(".");
  Serial.print(NODO_VERSION%10);
  Serial.print(F(" (beta), Product=SWACNC-SMALL-R"));
  Serial.print(NODO_BUILD);
  Serial.print(F(", Home="));
  Serial.print(Settings.Home);
  Serial.print(F(", ThisUnit="));
  Serial.println(Settings.Unit);
  Serial.println(F("!******************************************************************************!"));
  }

#endif
  
