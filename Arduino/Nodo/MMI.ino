#ifdef NODO_MEGA


/*********************************************************************************************\
 * Print een event volgens formaat:  'EVENT/ACTION: <port>, <type>, <content>
 \*********************************************************************************************/
void PrintEvent(struct NodoEventStruct *Event)
  {
  int x;

  // Systeem events niet weergeven.
  if(Event->Flags & TRANSMISSION_SYSTEM)
    return;
    
  char* StringToPrint=(char*)malloc(100);
  char* TmpStr=(char*)malloc(INPUT_BUFFER_SIZE+1);

  StringToPrint[0]=0; // als start een lege string

  // Bij input uit de eventlist wordt tevens de executiondepth en regel uit de eventlist weergegeven. We hebben hiervoor uit de struct
  // Checksum en Direction 'misbruikt'. Bewaar regelnummer in x en herstel direction weer. Deze oplossing is niet netjes maar bespaart veel code/ram,
  if(Event->Port==VALUE_SOURCE_EVENTLIST)
    {
    x=Event->Checksum*256+Event->Direction;
    Event->Direction=VALUE_DIRECTION_INPUT;
    }
  
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
      strcat(StringToPrint, int2str(ExecutionDepth));
      strcat(StringToPrint, ".");
      strcat(StringToPrint, int2str(x));
      strcat(StringToPrint, ")");
      }
    strcat(StringToPrint, "; "); 
    }

  // Unit 
  strcat(StringToPrint, cmd2str(CMD_UNIT));
  strcat(StringToPrint, "=");  
  if(Event->Direction==VALUE_DIRECTION_OUTPUT)
    strcat(StringToPrint, int2str(Event->DestinationUnit));
  else
    strcat(StringToPrint, int2str(Event->SourceUnit)); 

  // Event
  strcat(StringToPrint, "; ");
  strcat(StringToPrint, ProgmemString(Text_14));
  Event2str(Event,TmpStr);
  strcat(StringToPrint, TmpStr);

  // WEERGEVEN OP TERMINAL
  PrintTerminal(StringToPrint);   // stuur de regel naar Serial en/of naar Ethernet

  // LOG OP SDCARD
  if(bitRead(HW_Config,HW_SDCARD) && Settings.Log==VALUE_ON) 
    {
    TmpStr[0]=0;
    // datum en tijd weergeven
    if(bitRead(HW_Config,HW_CLOCK)) // bitRead(HW_Config,HW_CLOCK)=true want dan is er een RTC aanwezig.
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

  sprintf(dt,"Date=%d-%02d-%02d (%s), Time=%02d:%02d", Time.Year, Time.Month, Time.Date, s, Time.Hour, Time.Minutes);

  return dt;
  }

/**********************************************************************************************\
 * Print de welkomsttekst van de Nodo.
 \*********************************************************************************************/
void PrintWelcome(void)
  {
  char *TempString=(char*)malloc(80);

  // Print Welkomsttekst
  PrintTerminal("");
  PrintTerminal(ProgmemString(Text_22));
  PrintTerminal(ProgmemString(Text_01));
  PrintTerminal(ProgmemString(Text_02));

  // print versienummer, unit en indien gevuld het ID
  sprintf(TempString,ProgmemString(Text_15), NODO_BUILD, Settings.Unit);
  if(Settings.ID[0])
    {
    strcat(TempString,", ID=");
    strcat(TempString,Settings.ID);
    }

  PrintTerminal(TempString);

  // Geef datum en tijd weer.
  if(bitRead(HW_Config,HW_CLOCK))
    {
    sprintf(TempString,"%s %s",DateTimeString(), cmd2str(Time.DaylightSaving?VALUE_DLS:0));
    PrintTerminal(TempString);
    }

  // print IP adres van de Nodo
  if(bitRead(HW_Config,HW_ETHERNET))
    {
    sprintf(TempString,"NodoIP=%u.%u.%u.%u, %s=%d", Ethernet.localIP()[0],Ethernet.localIP()[1],Ethernet.localIP()[2],Ethernet.localIP()[3],cmd2str(CMD_PORT_SERVER),Settings.OutputPort );
    PrintTerminal(TempString);
    
    if((HTTPClientIP[0] + HTTPClientIP[1] + HTTPClientIP[2] + HTTPClientIP[3]) > 0)
      {
      strcpy(TempString,"Host=");        
      strcat(TempString,Settings.HTTPRequest);
      int x=StringFind(TempString,"/");
      TempString[x]=0;    
      strcat(TempString,", HostIP=");        
      strcat(TempString,ip2str(HTTPClientIP));
      strcat(TempString,", ");        
      strcat(TempString,cmd2str(CMD_PORT_CLIENT));        
      strcat(TempString,"=");        
      strcat(TempString,int2str(Settings.PortClient));
      PrintTerminal(TempString);
      }
    }
    
  PrintTerminal(ProgmemString(Text_22));
  free(TempString);
  }


 /**********************************************************************************************\
 * Verzend teken(s) naar de Terminal
 \*********************************************************************************************/
void PrintTerminal(char* LineToPrint)
  {  
  if(bitRead(HW_Config,HW_SERIAL))
    Serial.println(LineToPrint);

  if(bitRead(HW_Config,HW_ETHERNET))
    {
    if(TerminalClient.connected() && TerminalConnected>0 && TerminalLocked==0)
      TerminalClient.println(LineToPrint);
      
    if(HTTPResultFile)
      {
      SelectSDCard(true);
      HTTPResultFile.write((uint8_t*)LineToPrint,strlen(LineToPrint));
      HTTPResultFile.write('\n'); // nieuwe regel
      SelectSDCard(false);
      }
    }

  // FileLog wordt hier uitgevoerd. ??? kan dit wel nav de nieuwe HTTPRequestFile voorziening?
  if(TempLogFile[0]!=0)
    if(bitRead(HW_Config,HW_SDCARD))
      AddFileSDCard(TempLogFile,LineToPrint); // Extra logfile op verzoek van gebruiker: CMD_FILE_LOG
  }


#define PAR1_INT           1
#define PAR1_TEXT          2
#define PAR2_INT           3
#define PAR2_TEXT          4
#define PAR2_FLOAT         5
#define PAR2_INT_HEX       7
#define PAR2_DIM           8
#define PAR2_WDAYTIME      9
#define PAR2_ALARMENABLED 10
#define PAR2_INT8         11
#define PAR3_INT          12
#define PAR4_INT          13
#define PAR5_INT          14

void Event2str(struct NodoEventStruct *Event, char* EventString)
  {
  int x;
  EventString[0]=0;

  // Er kunnen een aantal parameters worden weergegeven. In een kleine tabel wordt aangegeven op welke wijze de parameters aan de gebruiker
  // moeten worden getoond. Het is niet per defiitie zo dat de interne Par1, Par2 en Par3 ook dezelfe parameters zijn die aan de gebruiker
  // worden getoond.   
  byte ParameterToView[8]={0,0,0,0,0,0,0,0};

  // Devices hebben een eigen MMI. Loop de devices langs. Als er een device is die hoort bij het opgegeven command
  // dan zal de string worden teruggegeven met de correct ingevulde MMI
  for(x=0;x<DEVICE_MAX && EventString[0]==0; x++)
    if(Device_ptr[x]!=0)
      Device_ptr[x](DEVICE_MMI_OUT,Event,EventString);

  if(EventString[0]==0)
    {
    strcat(EventString,cmd2str(Event->Command));
    strcat(EventString," ");      
    switch(Event->Command)
      {
//      case CMD_LOCK:
//        Par1=(Code&0x8000)?VALUE_ON:VALUE_OFF;
//        P1=P_TEXT;
//        P2=P_NOT;
//        break;??? lock later uitwerken

      case CMD_TIME:
        ParameterToView[2]=PAR2_WDAYTIME;
        break;

      case CMD_ALARM_SET:
        ParameterToView[0]=PAR1_INT;
        ParameterToView[1]=PAR2_ALARMENABLED;
        ParameterToView[2]=PAR2_WDAYTIME;
        break;

    // Par1 en Par2 samengesteld voor weergave van COMMAND <nummer> , <analoge waarde>
      case CMD_BREAK_ON_VAR_EQU:
      case CMD_BREAK_ON_VAR_LESS:
      case CMD_BREAK_ON_VAR_MORE:
      case CMD_BREAK_ON_VAR_NEQU:
      case CMD_VARIABLE_SET:
      case CMD_VARIABLE_INC:
      case CMD_VARIABLE_DEC:
      case CMD_VARIABLE_EVENT:
        ParameterToView[0]=PAR1_INT;
        ParameterToView[1]=PAR2_FLOAT;
        break;

      case CMD_RAWSIGNAL:
        ParameterToView[0]=PAR2_INT_HEX;
        break;

      case CMD_WIRED_SMITTTRIGGER:
      case CMD_WIRED_THRESHOLD:
      case CMD_WIRED_ANALOG:
        ParameterToView[0]=PAR1_INT;
        ParameterToView[1]=PAR2_INT;
        break;

      case VALUE_BUILD:
      case VALUE_HWCONFIG:
      case VALUE_FREEMEM:
      case CMD_PULSE_TIME:
      case CMD_PULSE_COUNT:
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
      case CMD_RAWSIGNAL_SEND:
      case CMD_DELAY:
      case CMD_WIRED_PULLUP:
      case CMD_WIRED_OUT:
      case CMD_WIRED_IN_EVENT:
        ParameterToView[0]=PAR1_INT;
        ParameterToView[1]=PAR2_TEXT;
        break;

        // Par1 als tekst en par2 als tekst
      case CMD_OUTPUT:
        ParameterToView[0]=PAR1_TEXT;
        ParameterToView[1]=PAR2_TEXT;
        break;

      case CMD_COMMAND_WILDCARD:
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
      case CMD_SEND_EVENT:
      case CMD_DEBUG:
      case CMD_LOG:
      case CMD_RAWSIGNAL_SAVE:
      case CMD_ECHO:
      case CMD_WAITFREERF:
      case CMD_BREAK_ON_DAYLIGHT:
        ParameterToView[0]=PAR1_TEXT;
        break;

      // Par1 als waarde en par2 niet
      case CMD_SENDTO:
      case CMD_EVENTLIST_SHOW:
      case CMD_EVENTLIST_ERASE:
      case CMD_TIMER_EVENT:
      case CMD_UNIT:
      case CMD_ALARM:
      case CMD_SIMULATE_DAY:
      case CMD_CLOCK_DOW:
      case CMD_BOOT_EVENT:
      case CMD_NEWNODO:
        ParameterToView[0]=PAR1_INT;
        break;

      // Par2 int en par1 als tekst
      case CMD_FILE_EXECUTE:
        ParameterToView[0]=PAR2_INT;
        ParameterToView[1]=PAR1_TEXT;
        break;
        
      // geen parameters.
      case CMD_REBOOT:
      case CMD_CLOCK_SYNC:
      case CMD_RESET:
      case CMD_SETTINGS_SAVE:
      case CMD_UNIT_LIST:
        break;
        
      // Twee getallen en een aanvullende tekst
      case CMD_MESSAGE:
        ParameterToView[0]=PAR1_INT;
        ParameterToView[1]=PAR2_INT;
        ParameterToView[2]=PAR2_TEXT;
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
          strcat(EventString,cmd2str(Event->Par2 &0xff));
          break;

        case PAR2_DIM:
          if(Event->Par2==VALUE_OFF || Event->Par2==VALUE_ON)
            strcat(EventString, cmd2str(Event->Par2)); // Print 'On' of 'Off'
          else
            strcat(EventString,int2str(Event->Par2));
          break;

        case PAR2_ALARMENABLED:
          if((Event->Par2>>20)&1)
            strcat(EventString, cmd2str(VALUE_ON)); // Print 'On' of 'Off'
          else
            strcat(EventString, cmd2str(VALUE_OFF)); // Print 'On' of 'Off'
          break;
        
        case PAR2_WDAYTIME:
          // tijd
          x=(Event->Par2>>12)&0xf;
          if(x==0xf)
            strcat(EventString, "*");
          else
            strcat(EventString, int2str(x));
          
          x=(Event->Par2>>8)&0xf;
          if(x==0xf)
            strcat(EventString, "*");
          else
            strcat(EventString, int2str(x));
          
          strcat(EventString, ":");

          x=(Event->Par2>>4)&0xf;
          if(x==0xf)
            strcat(EventString, "*");
          else
            strcat(EventString, int2str(x));

          x=(Event->Par2)&0xf;
          if(x==0xf)
            strcat(EventString, "*");
          else
            strcat(EventString, int2str(x));

          strcat(EventString, ",");
          // Dag
          x=(Event->Par2>>16)&0xf;
          if(x==0xf)
            strcat(EventString, "*");
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

      if(ParameterToView[p] && ParameterToView[p+1])// komt er nog een vervolgparameter, dan een komma als scheidingsteken.
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
  byte error=0, State_EventlistWrite=0,SendToUnit=0;
  unsigned long a;
  struct NodoEventStruct EventToExecute,TempEvent;

//  Serial.print(F("*** debug: ExecuteLine="));Serial.println(Line); //??? Debug

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
      AddFileSDCard(TempLogFile,Line); // Extra logfile op verzoek van gebruiker
    }
  else
    {
    if(Substitute(Line)!=0)
      {
      error=MESSAGE_02;
      Serial.print(F("Substitute error="));Serial.println(Line);
      }

    CommandPos=0;
    LinePos=0;    
    int LineLength=strlen(Line);
    while(LinePos<=LineLength && error==0)
      {
      char LineChar=Line[LinePos];
  
      // Chat teken. 
      if(LineChar=='$')
        {
        y=bitRead(HW_Config,HW_SERIAL);
        bitWrite(HW_Config,HW_SERIAL,1);
        PrintTerminal(Line+LinePos);
        bitWrite(HW_Config,HW_SERIAL,y);
        LinePos=LineLength+1; // ga direct naar einde van de regel.
        }

      // Comment teken. hierna verder niets meer doen.
      if(LineChar=='!') 
        LinePos=LineLength+1; // ga direct naar einde van de regel.

      // als puntkomma (scheidt opdrachten) of einde string
      if((LineChar=='!' || LineChar==';' || LineChar==0) && CommandPos>1)
        {
        Command[CommandPos]=0;
        CommandPos=0;
        ClearEvent(&EventToExecute);
        EventToExecute.Port=Port;
        
        GetArgv(Command,TmpStr1,1);
        EventToExecute.Command=str2cmd(TmpStr1); 
        
        
        if(GetArgv(Command,TmpStr1,2))
          {
          EventToExecute.Par1=str2cmd(TmpStr1);
          if(!EventToExecute.Par1)
            EventToExecute.Par1=str2int(TmpStr1);
          }
        
        if(GetArgv(Command,TmpStr1,3))
          {
          EventToExecute.Par2=str2cmd(TmpStr1);
          if(!EventToExecute.Par2)
            EventToExecute.Par2=str2int(TmpStr1);
          }        
        
        switch(EventToExecute.Command)
          {
          //test; geen, altijd goed
          case CMD_SIMULATE_DAY:
          case CMD_EVENTLIST_ERASE:
          case CMD_RESET:
          case CMD_ALARM:
          case CMD_MESSAGE:
          case CMD_REBOOT:
          case CMD_SETTINGS_SAVE:
          case CMD_USERPLUGIN: 
          case CMD_CLOCK_EVENT_DAYLIGHT:
          case CMD_STATUS:
          case CMD_TEST:
          case CMD_DELAY:
          case CMD_SOUND: 
          case CMD_USEREVENT:
          case CMD_SEND_USEREVENT:
          case CMD_UNIT_LIST:
          case CMD_CLOCK_SYNC:
            break; 
      
          case CMD_LOG:
          case CMD_DEBUG:
          case CMD_ECHO:
            if(EventToExecute.Par1!=VALUE_OFF && EventToExecute.Par1!=VALUE_ON)
              error=MESSAGE_02;
           break;
            
          case CMD_RAWSIGNAL_SEND:    
            if(EventToExecute.Par2!=VALUE_SOURCE_RF && EventToExecute.Par2!=VALUE_SOURCE_IR && EventToExecute.Par2!=0)
              error=MESSAGE_02;
            break;
            
          case CMD_TIMER_SET:
            if(EventToExecute.Par1>TIMER_MAX)
              error=MESSAGE_02;
            break;
                  
          case CMD_WAIT_EVENT:
            if((EventToExecute.Par1<1 || EventToExecute.Par1>UNIT_MAX) &&  EventToExecute.Par1!=VALUE_ALL)
              error=MESSAGE_02;
            break;
      
          case CMD_UNIT:
          case CMD_NEWNODO:
          case CMD_BOOT_EVENT:
            if(EventToExecute.Par1<1 || EventToExecute.Par1>UNIT_MAX)
              error=MESSAGE_02;
            break;
      
          case CMD_ANALYSE_SETTINGS:
            if(EventToExecute.Par1<1 || EventToExecute.Par1>50)
              error=MESSAGE_02;
            break;
      
          // test:EventToExecute.Par1 en EventToExecute.Par2 binnen bereik maximaal beschikbare variabelen
          case CMD_BREAK_ON_VAR_LESS_VAR:
          case CMD_BREAK_ON_VAR_MORE_VAR:
          case CMD_VARIABLE_VARIABLE:
            if(EventToExecute.Par1<1 || EventToExecute.Par1>USER_VARIABLES_MAX)
              error=MESSAGE_02;
            if(EventToExecute.Par2<1 || EventToExecute.Par2>USER_VARIABLES_MAX)
              error=MESSAGE_02;
            break;
              
          case CMD_VARIABLE_SET_WIRED_ANALOG:
            if(EventToExecute.Par1<1 || EventToExecute.Par1>USER_VARIABLES_MAX)
              error=MESSAGE_02;
            if(EventToExecute.Par2<1 || EventToExecute.Par2>WIRED_PORTS)
              error=MESSAGE_02;
            break;
              
          // test:EventToExecute.Par1 binnen bereik maximaal beschikbare timers
          case CMD_TIMER_EVENT:
          case CMD_TIMER_RANDOM:
            if(EventToExecute.Par1<1 || EventToExecute.Par1>TIMER_MAX)
              error=MESSAGE_02;
            break;
            
          // geldig jaartal
          case CMD_CLOCK_YEAR:
            if(EventToExecute.Par1!=20 || EventToExecute.Par2>99)
              error=MESSAGE_02;
            break;
          
          // geldige tijd    
          case CMD_BREAK_ON_TIME_LATER:
          case CMD_BREAK_ON_TIME_EARLIER:
          case CMD_CLOCK_TIME:
            if(EventToExecute.Par1>23 || EventToExecute.Par2>59)
              error=MESSAGE_02;
            break;
      
          // geldige datum
          case CMD_CLOCK_DATE: // data1=datum, data2=maand
            if(EventToExecute.Par1>31 || EventToExecute.Par1<1 || EventToExecute.Par2>12 || EventToExecute.Par2<1)
              error=MESSAGE_02;
            break;
      
          case CMD_CLOCK_DOW:
            if(EventToExecute.Par1<1 || EventToExecute.Par1>7)
              error=MESSAGE_02;
            break;
      
          // test:EventToExecute.Par1 binnen bereik maximaal beschikbare wired poorten.
          case CMD_WIRED_IN_EVENT:
            if(EventToExecute.Par1<1 || EventToExecute.Par1>WIRED_PORTS)
              error=MESSAGE_02;
            if(EventToExecute.Par2!=VALUE_ON && EventToExecute.Par2!=VALUE_OFF)
              error=MESSAGE_02;
            break;
      
          case CMD_WIRED_OUT:
            if(EventToExecute.Par1>WIRED_PORTS)
              error=MESSAGE_02;
            if(EventToExecute.Par2!=VALUE_ON && EventToExecute.Par2!=VALUE_OFF)
              error=MESSAGE_02;
            break;
      
          case CMD_WIRED_PULLUP:
            if(EventToExecute.Par1<1 || EventToExecute.Par1>WIRED_PORTS)
              error=MESSAGE_02;
            if(EventToExecute.Par2!=VALUE_ON && EventToExecute.Par2!=VALUE_OFF)
              error=MESSAGE_02;
            break;

          
          case CMD_SEND_EVENT:
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
      
          case CMD_COMMAND_WILDCARD:
            switch(EventToExecute.Par1)
              {
              case VALUE_ALL:
              case VALUE_SOURCE_I2C:
              case VALUE_SOURCE_IR:
              case VALUE_SOURCE_RF:
              case VALUE_SOURCE_SERIAL:
              case VALUE_SOURCE_WIRED:
              case VALUE_SOURCE_EVENTLIST:
              case VALUE_SOURCE_SYSTEM:
              case VALUE_SOURCE_TIMER:
              case VALUE_SOURCE_VARIABLE:
              case VALUE_SOURCE_CLOCK:
              case VALUE_SOURCE_HTTP:
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
            if(EventToExecute.Par1!=VALUE_OFF && EventToExecute.Par1!=VALUE_ON)
              error=MESSAGE_02;
            break;
      
          case CMD_OUTPUT:
            if(EventToExecute.Par1!=VALUE_SOURCE_I2C && EventToExecute.Par1!=VALUE_SOURCE_IR && EventToExecute.Par1!=VALUE_SOURCE_RF && EventToExecute.Par1!=VALUE_SOURCE_HTTP)
              error=MESSAGE_02;
            if(EventToExecute.Par2!=VALUE_OFF && EventToExecute.Par2!=VALUE_ON)
              error=MESSAGE_02;
            break;

          case CMD_SENDTO:
            if(EventToExecute.Par1==VALUE_OFF)
              SendToUnit=0;
            else
              SendToUnit=EventToExecute.Par1;
            EventToExecute.Command=0;
            break;    

//          case CMD_LOCK: // Hier wordt de lock code o.b.v. het wachtwoord ingesteld. Alleen van toepassing voor de Mega.??? nog uitwerken
//            a=0L;
//            for(x=0;x<strlen(Settings.Password);x++)
//              {
//              a=a<<5;
//              a^=Settings.Password[x];
//              }
//            a&=0x7fff;// 15-bits pincode uit wachtwoord samengesteld. 16e bit is lock aan/uit.
//            if(EventToExecute.Par1==VALUE_ON)
//              a |= (1<<15);
//             EventToExecute.Command=CMD_LOCK;
//             EventToExecute.Par1=(a>>8)&0xff;
//             EventToExecute.Par2=a&0xff;
//            break;

          case CMD_BREAK_ON_VAR_EQU:
          case CMD_BREAK_ON_VAR_LESS:
          case CMD_BREAK_ON_VAR_MORE:
          case CMD_BREAK_ON_VAR_NEQU:
          case CMD_VARIABLE_DEC:
          case CMD_VARIABLE_SET:
          case CMD_VARIABLE_INC:
          case CMD_VARIABLE_EVENT:
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
            if(EventToExecute.Par1<1 || EventToExecute.Par1>WIRED_PORTS)
              error=MESSAGE_02;
            else if(GetArgv(Command,TmpStr1,3))
              EventToExecute.Par2=str2int(TmpStr1);
            break;
              
          case CMD_EVENTLIST_WRITE:
            if(SendToUnit==Settings.Unit || SendToUnit==0)
              {                          
              if(EventToExecute.Par1<=EVENTLIST_MAX)
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
            TmpStr1[0]=0;
            GetArgv(Command,TmpStr1,2);
            TmpStr1[9]=0; // voor geval de string te lang is.
            strcpy(Settings.ID,TmpStr1);
            EventToExecute.Command=0; // Geen verdere verwerking meer nodig.
            break;
            }  

          case CMD_TEMP:
            {
            x=StringFind(Command,cmd2str(CMD_TEMP))+strlen(cmd2str(CMD_TEMP));
            while(Command[x]==' ')x++;             // eventuele spaties verwijderen
            strcpy(TmpStr1,Command+x);             // Alles na de "temp" hoort bij de variabele
            TmpStr1[25]=0;                         // voor geval de string te lang is.
            strcpy(Settings.Temp,TmpStr1);
            EventToExecute.Command=0; // Geen verdere verwerking meer nodig.
            break;
            }  
          
          case CMD_RAWSIGNAL:
            if(GetArgv(Command,TmpStr1,2))
              {
              EventToExecute.Par1=0;
              EventToExecute.Par2=str2int(TmpStr1);
              }
            break;

          case CMD_ALARM_SET:
            // Commando format: [AlarmSet <AlarmNumber 1..4>, <Enabled On|Off>, <Time HHMM>, <Day 1..7>]
            //                  [Time <Time HHMM>, <Day 1..7>]
            // We moeten wat truucs uithalen om al deze info in een 32-bit variabele te krijgen.
            // Alarmtijd wordt in Par2 opgeslagen volgens volgende format: MSB-EEEEWWWWAAAABBBBCCCCDDDD-LSB
            // E=Enabled, WWWW=weekdag, AAAA=Uren tiental, BBBB=uren, CCCC=minuten tiental DDDD=minuten
            // Als een deel gevuld met 0xE, dan waarde niet setten.
            // Als gevuld met 0xF, dan wildcard.             {
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
                      if(GetArgv(Command,TmpStr1,5)) // Day is optioneel. Maar als deze parameter ingevuld, dan meenemen in de berekening.
                        y=str2weekday(TmpStr1);
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
          
          case CMD_TIME:
            // Event format: [Time <Time HHMM>, <Day 1..7>]
            // We moeten wat truucs uithalen om al deze info in een 32-bit variabele te krijgen.
            // Tijd wordt in Par2 opgeslagen volgens volgende format: MSB-0000WWWWAAAABBBBCCCCDDDD-LSB
            // WWWW=weekdag, AAAA=Uren tiental, BBBB=uren, CCCC=minuten tiental DDDD=minuten
            {              
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
            if(GetArgv(Command,TmpStr1,2))
              {
              strcat(TmpStr1,".dat");
              FileErase(TmpStr1);
              }
            else
              FileList("",true);

            EventToExecute.Command=0; // Geen verdere verwerking meer nodig.
            break;
    
          case CMD_RAWSIGNAL_ERASE:      
            if(GetArgv(Command,TmpStr1,2))
              {
              sprintf(TmpStr2,"%s/%s.raw",ProgmemString(Text_28),TmpStr1);
              FileErase(TmpStr2);
              }
            else
              FileList("/RAW",true);
              
            EventToExecute.Command=0; // Geen verdere verwerking meer nodig.
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
            EventToExecute.Command=0; // Geen verdere verwerking meer nodig.
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
              error=FileShow(FileName);
              }
            EventToExecute.Command=0; // Geen verdere verwerking meer nodig.
            break;
            }
    
          case CMD_FILE_EXECUTE:
            {
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
            if(EventToExecute.Par1<=EVENTLIST_MAX)
              {
              PrintTerminal(ProgmemString(Text_22));
              if(EventToExecute.Par1==0)
                {
                x=1;
                while(EventlistEntry2str(x++,0,TmpStr2, false))
                  if(TmpStr2[0]!=0)
                    PrintTerminal(TmpStr2);
                }
              else
                {
                EventlistEntry2str(EventToExecute.Par1,0,TmpStr2, false);//??? buiten bereik afvangen
                  if(TmpStr2[0]!=0)
                    PrintTerminal(TmpStr2);
                }
              PrintTerminal(ProgmemString(Text_22));
              EventToExecute.Command=0; // Geen verdere verwerking meer nodig.
              }
            else
              error=MESSAGE_02;
            break;
              
          case CMD_NODO_IP:
            if(GetArgv(Command,TmpStr1,2))
              if(!str2ip(TmpStr1,Settings.Nodo_IP))
                error=MESSAGE_02;
            EventToExecute.Command=0; // Geen verdere verwerking meer nodig.
            break;
            
          case CMD_CLIENT_IP:
            if(GetArgv(Command,TmpStr1,2))
              if(!str2ip(TmpStr1,Settings.Client_IP))
                error=MESSAGE_02;
            EventToExecute.Command=0; // Geen verdere verwerking meer nodig.
            break;
            
          case CMD_SUBNET:
            if(GetArgv(Command,TmpStr1,2))
              if(!str2ip(TmpStr1,Settings.Subnet))
                error=MESSAGE_02;
            break;
            
          case CMD_DNS_SERVER:
            if(GetArgv(Command,TmpStr1,2))
              if(!str2ip(TmpStr1,Settings.DnsServer))
                error=MESSAGE_02;
            EventToExecute.Command=0; // Geen verdere verwerking meer nodig.
            break;
            
          case CMD_GATEWAY:
            if(GetArgv(Command,TmpStr1,2))
              if(!str2ip(TmpStr1,Settings.Gateway))
                error=MESSAGE_02;
            EventToExecute.Command=0; // Geen verdere verwerking meer nodig.
            break;
            
          case CMD_EVENTLIST_FILE:
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
            FileList("/RAW",false);
            EventToExecute.Command=0; // Geen verdere verwerking meer nodig.
            break;

          case CMD_FILE_LIST:
            FileList("/",false);
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

          case CMD_PULSE_COUNT:
            {
            if(GetArgv(Command,TmpStr1,2))
              EventToExecute.Par2=str2int(TmpStr1);                  
            break;
            }  

          case CMD_PORT_SERVER:
            {
            if(GetArgv(Command,TmpStr1,2))
              EventToExecute.Par2=str2int(TmpStr1);
            break;
            }  

          case CMD_PORT_CLIENT:
            {
            if(GetArgv(Command,TmpStr1,2))
              EventToExecute.Par2=str2int(TmpStr1);
            break;
            }  

          default:
            {              
            // Als er geen vast commando is ingevoerd, dan kan het zijn dat er een Device commando is opgegeven.
            // Loop de devices langs om te checken if er een hit is. Zo ja, dan heeft de Device_xx funktie de struct
            // met de juiste waarden gevuld.
            y=false;
            for(x=0;x<DEVICE_MAX && EventToExecute.Command==0; x++)
              if(Device_ptr[x]!=0)
                y=Device_ptr[x](DEVICE_MMI_IN,&EventToExecute,Command);

            if(!y)
              {
              // Als het geen regulier commando was EN geen commando met afwijkende MMI en geen Device, dan kijken of file op SDCard staat)
              strcpy(TmpStr1,Command);
              TmpStr1[8]=0;// Gebruik commando als een filename. Voor de zekerheid te lange filename afkappen ???? dit zorgt ook voor afkappen van commando bij foutcode
              error=FileExecute(TmpStr1, EventToExecute.Par2==VALUE_ON);
              EventToExecute.Command=0;
  
              // als script niet te openen, dan is het ingevoerde commando ongeldig.
              if(error==MESSAGE_03)
                error=MESSAGE_01;
              }
            }
          }
            
        if(EventToExecute.Command && error==0)
          {
          // Er kunnen zich twee situaties voordoen:
          //
          // A: Event is voor deze Nodo en moet gewoon worden uitgevoerd;
          // B: SendTo is actief. Event moet worden verzonden naar een andere Nodo. Hier wordt de Queue voor gebruikt.
          
          if(State_EventlistWrite==0)// Gewoon uitvoeren
            {
            if(SendToUnit==Settings.Unit || SendToUnit==0)
              {
              EventToExecute.Direction=VALUE_DIRECTION_INPUT;
              error=ProcessEvent2(&EventToExecute);      // verwerk binnengekomen event.
              }
            else
              {
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
        }
        
      // Tekens toevoegen aan commando zolang er nog ruimte is in de string
      if(LineChar!=';' && CommandPos<MaxCommandLength)
        Command[CommandPos++]=LineChar;      

      LinePos++;
      }// einde commando behandeling    
  
    // Verzend de inhoud van de queue naar de slave Nodo
    if(SendToUnit!=Settings.Unit && SendToUnit!=0 && error==0)
      {
      error=QueueSend(SendToUnit);
      if(error==MESSAGE_13)
        {
        CommandPos=0;
        LinePos=0;
        error=0;    
        Serial.print(F("*** debug: SendTo retry."));Serial.println(); //??? Debug
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
  byte x;
  char* str=(char*)malloc(80);

  // Print Welkomsttekst
  Serial.println();
  Serial.println(ProgmemString(Text_22));
  Serial.println(ProgmemString(Text_01));
  Serial.println(ProgmemString(Text_02));

  // print versienummer, unit en indien gevuld het ID
  sprintf(str,ProgmemString(Text_15), NODO_BUILD, Settings.Unit);
  Serial.println(str);

  // Geef datum en tijd weer.
  if(bitRead(HW_Config,HW_CLOCK))
    {
    // Print de dag. 1=zondag, 0=geen RTC aanwezig
    char s[5];
    for(x=0;x<=2;x++)
      s[x]=(*(ProgmemString(Text_04)+(Time.Day-1)*3+x));
    s[x]=0;

    sprintf(str,"Date=%d-%02d-%02d (%s), Time=%02d:%02d, DaylightSaving=%d", Time.Year, Time.Month, Time.Date, s, Time.Hour, Time.Minutes,Time.DaylightSaving);
    Serial.println(str);
    }
  Serial.println(ProgmemString(Text_22));
  free(str);
  }

/*********************************************************************************************\
 * Print een event: debug mode Nodo-Small
 \*********************************************************************************************/
void PrintEvent(struct NodoEventStruct *Event)
  {
  Serial.print(Event->Direction);
  Serial.print(",");
  Serial.print(Event->Port);
  Serial.print(",");
  Serial.print(Event->Command);
  Serial.print(",");
  Serial.print(Event->Par1);
  Serial.print(",");
  Serial.println(Event->Par2,HEX);
  } 

#endif
  

