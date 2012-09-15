
/*********************************************************************************************\
 * Een string formaat van een Event weer in de vorm "(<commando> <parameter-1>,<parameter-2>)"
 \*********************************************************************************************/
#define P_NOT     0
#define P_TEXT    1 
#define P_KAKU    2
#define P_VALUE   3
#define P_DIM     4
#define P_ANALOG  5

#if NODO_MEGA
/*********************************************************************************************\
 * Print een event volgens formaat:  'EVENT/ACTION: <port>, <type>, <content>
 \*********************************************************************************************/
void PrintEvent(unsigned long Content, byte Direction, byte Port )
  {

  byte x;

  // Commando(s) die NIET aan de gebruiker getoond hoeven te worden.
  switch((Content>>16)&0xff)// Cmd
    {
    case CMD_TRANSMIT_QUEUE:
      return;
    }

  char* StringToPrint=(char*)malloc(100);
  char* TmpStr=(char*)malloc(INPUT_BUFFER_SIZE+1);

  StringToPrint[0]=0; // als start een lege string

  // Geef de richting van de communicatie weer
  if(Direction)
    {
    strcat(StringToPrint,cmd2str(Direction));      
    strcat(StringToPrint,"=");
    }

  // Poort
  x=true;

  strcat(StringToPrint, cmd2str(Port));

  if(Port==VALUE_SOURCE_EVENTGHOST || Port==VALUE_SOURCE_HTTP || Port==VALUE_SOURCE_TELNET)
    {
    strcat(StringToPrint, "(");
    strcat(StringToPrint, ip2str(ClientIPAddress));
    strcat(StringToPrint, ")");
    x=false;
    }

  // Unit
  if(((Content>>28)&0xf)==((unsigned long)(SIGNAL_TYPE_UNKNOWN)))  // Als het een onbekend type signaal is, dan unit niet weergeven.
    x=false;

  if(x)
    {
    strcat(StringToPrint, ", "); 
    strcat(StringToPrint, cmd2str(CMD_UNIT));
    strcat(StringToPrint, "=");
    strcat(StringToPrint, int2str((Content>>24)&0xf)); 
    }

  // Event
  strcat(StringToPrint, ", ");
  strcat(StringToPrint, ProgmemString(Text_14));
  Event2str(Content,TmpStr);
  strcat(StringToPrint, TmpStr);

  if(((Content>>16)&0xff)==CMD_MESSAGE)
    {
    strcat(StringToPrint, " (");
    strcat(StringToPrint, cmd2str(Content&0xff));
    strcat(StringToPrint, ")");
    }

  PrintTerminal(StringToPrint);   // stuur de regel naar Serial en/of naar Ethernet

  if(bitRead(HW_Config,HW_SDCARD)) 
    {
    TmpStr[0]=0;
    // datum en tijd weergeven
    if(bitRead(HW_Config,HW_CLOCK)) // bitRead(HW_Config,HW_CLOCK)=true want dan is er een RTC aanwezig.
      {   
      strcat(TmpStr,DateTimeString());
      strcat(TmpStr,", ");
      }
    strcat(TmpStr,StringToPrint);
    AddFileSDCard(ProgmemString(Text_23),TmpStr); // Extra logfile op verzoek van gebruiker
    }
  free(TmpStr);
  free(StringToPrint);
  } 
#else

/*********************************************************************************************\
 * Print een event: debug mode Nodo-Small
 \*********************************************************************************************/
void PrintEvent(unsigned long Content, byte Direction, byte Port)
  {
  Serial.print(Direction);
  Serial.print(",");
  Serial.print(Port);
  Serial.print(",0x");
  Serial.println(Content,HEX);
  } 

#endif


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

#if NODO_MEGA

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
  sprintf(TempString,"Version=3.0(Mega), Build=%04d, ThisUnit=%d", NODO_BUILD, Settings.Unit);
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

    #if TRACE
    AddFileSDCard("TRACE.DAT", TempString);
    #endif
    }

  // print IP adres van de Nodo
  if(bitRead(HW_Config,HW_ETHERNET))
    {
    sprintf(TempString,"NodoIP=%u.%u.%u.%u, MAC=%02X-%02X-%02X-%02X-%02X-%02X",Ethernet.localIP()[0],Ethernet.localIP()[1],Ethernet.localIP()[2],Ethernet.localIP()[3], NODO_MAC);
    PrintTerminal(TempString);
    }
  
  #if TRACE
  PrintTerminal("");
  PrintTerminal(ProgmemString(Text_08));
  PrintTerminal("");
  #endif

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
    }

  // FileLog wordt hier uitgevoerd.
  if(TempLogFile[0]!=0)
    if(bitRead(HW_Config,HW_SDCARD))
      AddFileSDCard(TempLogFile,LineToPrint); // Extra logfile op verzoek van gebruiker: CMD_FILE_LOG

}

/**********************************************************************************************\
 * Converteert een 32-bit eventcode naar een voor de gebruiker leesbare string
 \*********************************************************************************************/
void Event2str(unsigned long Code, char* EventString)
  {
  int x,y;
  byte P1,P2,Par2_b; 
  boolean P2Z=true;     // vlag: true=Par2 als nul waarde afdrukken false=nulwaarde weglaten

  byte Type     = (Code>>28)&0xf;
  byte Command  = (Code>>16)&0xff;
  byte Par1     = (Code>>8)&0xff;
  byte Par2     = (Code)&0xff;

  EventString[0]=0;

  if(Type==SIGNAL_TYPE_NEWKAKU)
    {
    // Aan/Uit zit in bit 5 
    strcpy(EventString,cmd2str(CMD_KAKU_NEW));
    strcat(EventString," ");
    strcat(EventString,int2str(Code&0x0FFFFFEF));
    strcat(EventString,",");   
    strcat(EventString,cmd2str(((Code>>4)&0x1)?VALUE_ON:VALUE_OFF)); 
  }

  else if(Type==SIGNAL_TYPE_NODO || Type==SIGNAL_TYPE_KAKU)
  {
    strcat(EventString,cmd2str(Command));
    switch(Command)
    {
    case CMD_LOCK:
      Par1=(Code&0x8000)?VALUE_ON:VALUE_OFF;
      P1=P_TEXT;
      P2=P_NOT;
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
    case CMD_WIRED_SMITTTRIGGER:
    case CMD_WIRED_THRESHOLD:
    case CMD_WIRED_ANALOG:
    case CMD_WIRED_ANALOG_CALIBRATE_HIGH:
    case CMD_WIRED_ANALOG_CALIBRATE_LOW:
      P1=P_ANALOG;
      P2=P_NOT;
      break;

      // Par1 als KAKU adres [A0..P16] en Par2 als [On,Off]
    case CMD_KAKU:
    case CMD_SEND_KAKU:
      P1=P_KAKU;
      P2=P_TEXT;
      Par2_b=Par2;
      break;

    case CMD_KAKU_NEW:
    case CMD_SEND_KAKU_NEW:
      P1=P_VALUE;
      P2=P_DIM;
      break;

      // Par1 als waarde en par2 als tekst
    case CMD_RAWSIGNAL_SEND:
    case CMD_DELAY:
    case CMD_WIRED_PULLUP:
    case CMD_WIRED_OUT:
    case CMD_WIRED_IN_EVENT:
      P1=P_VALUE;
      P2=P_TEXT;
      break;

      // Par1 als tekst en par2 als tekst
    case CMD_TRANSMIT_IP:
    case CMD_COMMAND_WILDCARD:
      P1=P_TEXT;
      P2=P_TEXT;
      break;

      // Par1 als tekst en par2 als getal
    case CMD_RAWSIGNAL_COPY:
    case CMD_TRANSMIT_IR:
    case CMD_TRANSMIT_RF:
    case CMD_STATUS:
      P1=P_TEXT;
      P2=P_VALUE;
      break;

      // Par1 als tekst en par2 niet
    case CMD_SEND_EVENT:
    case CMD_DEBUG:
    case CMD_ECHO:
    case CMD_BUSY:
    case CMD_WAITFREERF:
    case CMD_SENDBUSY:
    case CMD_BREAK_ON_DAYLIGHT:
      P1=P_TEXT;
      P2=P_NOT;
      break;

      // Par1 als waarde en par2 niet
    case CMD_WAITBUSY:
    case CMD_EVENTLIST_SHOW:
    case CMD_EVENTLIST_ERASE:
    case CMD_FILE_EXECUTE:
    case CMD_UNIT:
    case CMD_RAWSIGNAL:
    case CMD_SIMULATE_DAY:
    case CMD_CLOCK_DOW:
    case CMD_BOOT_EVENT:
      P1=P_VALUE;
      P2=P_NOT;
      break;

      // Geen parameters
    case CMD_REBOOT:
    case CMD_RESET:
      P1=P_NOT;
      P2=P_NOT;
      break;

      // Par1 als waarde en par2 als waarde
    default:
      P1=P_VALUE;
      P2=P_VALUE;    
      }

    // Print Par1      
    if(P1!=P_NOT)
      {
      strcat(EventString," ");
      switch(P1)
      {
      case P_ANALOG:
        strcat(EventString,int2str(((Code>>12)&0x0f)));
        strcat(EventString,",");
        strcat(EventString,AnalogInt2str(event2AnalogInt(Code))); // waarde analoog
        break;
      case P_TEXT:
        strcat(EventString,cmd2str(Par1));
        break;
      case P_VALUE:
        strcat(EventString,int2str(Par1));
        break;
      case P_KAKU:
        char t[3];
        t[0]= 'A' + ((Par1&0xf0)>>4);      // A..P
        t[1]= 0;                         // en de mini-string afsluiten.
        strcat(EventString,t);

        // als 2e bit in Par2 staat, dan is het een groep commando en moet adres '0' zijn.
        if(Par2_b&0x02)
          strcat(EventString,int2str(0));              
        else
          strcat(EventString,int2str((Par1&0xF)+1));              

        Par2=(Par2&0x1)?VALUE_ON:VALUE_OFF;
        break;
      }
    }// P1

    // Print Par2    
    if(P2!=P_NOT)
      {
      switch(P2)
        {
        case P_TEXT:
          if(Par2)
            strcat(EventString,",");
          strcat(EventString,cmd2str(Par2));
          break;
        case P_VALUE:
          strcat(EventString,",");
          strcat(EventString,int2str(Par2));
          break;
        case P_DIM:
          {
            strcat(EventString,",");
            if(Par2==VALUE_OFF || Par2==VALUE_ON)
              strcat(EventString, cmd2str(Par2)); // Print 'On' of 'Off'
            else
              strcat(EventString,int2str(Par2));
            break;
          }
        }
      }// P2
    }//   if(Type==SIGNAL_TYPE_NODO || Type==SIGNAL_TYPE_OTHERUNIT)

  else // wat over blijft is het type UNKNOWN.
    {
    strcat(EventString,int2str(Code));
    }
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
  sprintf(str,"Version=3.0(Small), Revision=%04d, ThisUnit=%d", NODO_BUILD, Settings.Unit);
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
#endif

