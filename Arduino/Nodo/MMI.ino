
#ifdef NODO_MEGA
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

  if(Port==VALUE_SOURCE_HTTP || Port==VALUE_SOURCE_TELNET)
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

  if(bitRead(HW_Config,HW_SDCARD && Settings.Log==VALUE_ON)) 
    {
    TmpStr[0]=0;
    // datum en tijd weergeven
    if(bitRead(HW_Config,HW_CLOCK)) // bitRead(HW_Config,HW_CLOCK)=true want dan is er een RTC aanwezig.
      {   
      strcat(TmpStr,DateTimeString());
      strcat(TmpStr,", ");
      }
    strcat(TmpStr,StringToPrint);
    AddFileSDCard(ProgmemString(Text_23),TmpStr);
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
  // Trace(0,0,0);
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

#ifdef NODO_MEGA
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
    }

  // FileLog wordt hier uitgevoerd.
  if(TempLogFile[0]!=0)
    if(bitRead(HW_Config,HW_SDCARD))
      AddFileSDCard(TempLogFile,LineToPrint); // Extra logfile op verzoek van gebruiker: CMD_FILE_LOG
  }

/**********************************************************************************************\
 * Converteert een 32-bit eventcode naar een voor de gebruiker leesbare string
 \*********************************************************************************************/
#define P_NOT     0
#define P_TEXT    1 
#define P_KAKU    2
#define P_VALUE   3
#define P_DIM     4
#define P_PORT    5
#define P_INT16   6
#define P_INT10   7
#define P_FLOAT   8
#define P_ALARM   9

void Event2str(unsigned long Code, char* EventString)
  {
  int x,y,z;
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
    strcat(EventString,int2strhex(Code&0x0FFFFFEF));
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

    case CMD_ALARM_SET:
      P1=P_ALARM;
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
      P1=P_PORT;
      P2=P_FLOAT;
      break;

    case CMD_WIRED_SMITTTRIGGER:
    case CMD_WIRED_THRESHOLD:
    case CMD_WIRED_ANALOG:
      P1=P_PORT;
      P2=P_INT10;
      break;

    // Par1 als KAKU adres [A0..P16] en Par2 als [On,Off]
    case CMD_KAKU:
    case CMD_SEND_KAKU:
      P1=P_KAKU;
      P2=P_TEXT;
      Par2_b=Par2;
      break;
    
    // Toon Par1 en Par2 tezamen als één unsigned 16-bit integer.
    case VALUE_BUILD:
    case VALUE_HWCONFIG:
    case CMD_PULSE_TIME:
    case CMD_PULSE_COUNT:
      P1=P_INT16;
      P2=P_NOT;
      break;
        
    case CMD_KAKU_NEW:
    case CMD_SEND_KAKU_NEW:
      P1=P_VALUE;
      P2=P_DIM;
      break;

    case CMD_WAIT_EVENT:
      if(Par1==VALUE_ALL)
        P1=P_TEXT;
      else
        P1=P_VALUE;

      P2=P_TEXT;
      break;

      // Par1 als waarde en par2 als tekst
    case CMD_WAITBUSY:
    case CMD_RAWSIGNAL_SEND:
    case CMD_DELAY:
    case CMD_WIRED_PULLUP:
    case CMD_WIRED_OUT:
    case CMD_WIRED_IN_EVENT:
      P1=P_VALUE;
      P2=P_TEXT;
      break;

      // Par1 als tekst en par2 als tekst
    case CMD_COMMAND_WILDCARD:
    case CMD_OUTPUT:
      P1=P_TEXT;
      P2=P_TEXT;
      break;

      // Par1 als tekst en par2 als getal
    case CMD_STATUS:
      P1=P_TEXT;
      P2=P_VALUE;
      break;

      // Par1 als tekst en par2 niet
    case CMD_SEND_EVENT:
    case CMD_DEBUG:
    case CMD_LOG:
    case CMD_ECHO:
    case CMD_BUSY:
    case CMD_WAITFREERF:
    case CMD_SENDBUSY:
    case CMD_BREAK_ON_DAYLIGHT:
      P1=P_TEXT;
      P2=P_NOT;
      break;

      // Par1 als waarde en par2 niet
    case CMD_EVENTLIST_SHOW:
    case CMD_EVENTLIST_ERASE:
    case CMD_FILE_EXECUTE:
    case CMD_CLOCK_SYNC:
    case CMD_TIMER_EVENT:
    case CMD_UNIT:
    case CMD_ALARM:
    case CMD_RAWSIGNAL:
    case CMD_SIMULATE_DAY:
    case CMD_CLOCK_DOW:
    case CMD_BOOT_EVENT:
    case CMD_NEWNODO:
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
      case P_INT16:
        strcat(EventString,int2str(EventPart16Bit(Code)));
        break;

      case P_PORT:
        strcat(EventString,int2str(((Code>>12)&0x0f) +1)); // +1 omdat intern vananf nul wordt geteld en gebruiker vanaf 1.
        break;

      case P_TEXT:
        strcat(EventString,cmd2str(Par1));
        break;

      case P_VALUE:
        strcat(EventString,int2str(Par1));
        break;

      case P_ALARM:
        strcat(EventString,int2str(((Code>>14)&0x03)+1)); // Alarm nummer              

        strcat(EventString,",");
        strcat(EventString,cmd2str((Code>>13)&1?VALUE_ON:VALUE_OFF)); // Enabled

        strcat(EventString,",");

        x=(Code&0xFFF)/288;       // Dag
        y=(Code&0xFFF)%288;       // Alarmtijd filteren op minuten na 0:00 van dit etmaal met 5 min. resolutie.
        z=y/12;                   // Uren
        y=(y%12)*5;               // Minuten
        
        strcat(EventString,int2str(z)); // uren

        if(y<10)
          strcat(EventString,"0");
        strcat(EventString,int2str(y)); // minuten

        if(x>0)
          {
          strcat(EventString,",");
          strcat(EventString,int2str(x)); // Dag
          }
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
      strcat(EventString,",");
      switch(P2)
        {
        case P_TEXT:
          strcat(EventString,cmd2str(Par2));
          break;

        case P_VALUE:
          strcat(EventString,int2str(Par2));
          break;

        case P_DIM:
          {
          if(Par2==VALUE_OFF || Par2==VALUE_ON)
            strcat(EventString, cmd2str(Par2)); // Print 'On' of 'Off'
          else
            strcat(EventString,int2str(Par2));
          break;
          }

        case P_INT10:
          strcat(EventString,int2str(EventPart10Bit(Code)));
          break;
  
        case P_FLOAT:
          strcat(EventString,Float2str(EventPartFloat(Code))); // waarde analoog
          break;

        }
      }// P2
    }//   if(Type==SIGNAL_TYPE_NODO || Type==SIGNAL_TYPE_OTHERUNIT)

  else // wat over blijft is het type UNKNOWN.
    {
    strcat(EventString,int2strhex(Code));
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
#endif

