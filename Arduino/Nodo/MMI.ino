 /*********************************************************************************************\
 * Een string formaat van een Event weer in de vorm "(<commando> <parameter-1>,<parameter-2>)"
 \*********************************************************************************************/
#define P_NOT     0
#define P_TEXT    1 
#define P_KAKU    2
#define P_VALUE   3
#define P_DIM     4
#define P_ANALOG  5

 /*********************************************************************************************\
 * Print een event volgens formaat:  'EVENT/ACTION: <port>, <type>, <content>
 \*********************************************************************************************/
void PrintEvent(unsigned long Content, byte Port, byte Direction)
  {
  byte x;

  TempString[0]=0; // als start een lege string

  // datum en tijd weergeven
  if(Time.Day) // Time.Day=true want dan is er een RTC aanwezig.
    {   
    strcat(TempString,DateTimeString());
    strcat(TempString,", ");
    }
    
  // Geef de richting van de communicatie weer
  switch(Direction)
    {
    case VALUE_SOURCE_QUEUE:// testen ???
      strcat(TempString,cmd2str(VALUE_SOURCE_QUEUE));      
      strcat(TempString,"-");
      strcat(TempString, int2str(QueuePos+1));
      strcat(TempString,"=");
      break;
    
    case VALUE_DIRECTION_INTERNAL:
      strcat(TempString,ProgmemString(Text_25));
      break;
      
    case VALUE_DIRECTION_INPUT:
      strcat(TempString,ProgmemString(Text_12));
      break;
      
    case VALUE_DIRECTION_OUTPUT:
      strcat(TempString,ProgmemString(Text_11));
      break;
    }
      
  if(Port==VALUE_SOURCE_EVENTGHOST)
    strcat(TempString, ip2str(IP));
  else
    strcat(TempString, cmd2str(Port));
  

  if(Port!=VALUE_SOURCE_EVENTGHOST)
    {
    if(((Content>>28)&0xf)==SIGNAL_TYPE_NODO && ((Content>>16)&0xff)!=CMD_KAKU_NEW && ((Content>>16)&0xff)!=CMD_KAKU)
      {
      strcat(TempString, ", "); 
      strcat(TempString, cmd2str(CMD_UNIT));
      strcat(TempString, "=");
      strcat(TempString, int2str((Content>>24)&0xf)); 
      }
    }
    
  // geef het event weer
  strcat(TempString, ", ");
  if(((Content>>16)&0xff)==CMD_ERROR)
    {
    strcat(TempString, ProgmemString(Text_06));
    strcat(TempString, cmd2str((Content>>8)&0xff));
    }
  else
    {
    strcat(TempString, ProgmemString(Text_14));
    strcat(TempString, Event2str(Content));
    }

  // stuur de regel naar Serial en/of naar Ethernet
  PrintLine(TempString);

  // Indien er een SDCard gevonden is, dan loggen naar file
  if(SDCardPresent)
    LogSDCard(TempString);
  } 
      

  
 /**********************************************************************************************\
 * Print een regel uit de Eventlist.
 \*********************************************************************************************/
void PrintEventlistEntry(int entry, byte d)
  {
  unsigned long Event, Action;
  String Line;
  
  Eventlist_Read(entry,&Event,&Action); // leesregel uit de Eventlist.    

  // Geef de entry van de eventlist weer
  Line=ProgmemString(Text_03);
    
  if(d>1)
    {
    Line+=d;
    Line+='.';
    }
  Line+=entry;

  // geef het event weer
  Line+=", ";
  Line+=ProgmemString(Text_14);
  Line+=Event2str(Event);

  // geef het action weer
  Line+=", ";
  Line+=ProgmemString(Text_16);
  Line+=Event2str(Action);
  PrintLine(Line);
  }
  
 /**********************************************************************************************\
 * Print actuele dag, datum, tijd.
 \*********************************************************************************************/
char* DateTimeString(void)
    {
    int x;
    static char dt[40];
    char s[5];
    
    for(x=0;x<=2;x++)
      s[x]=(*(ProgmemString(Text_04)+(Time.Day-1)*3+x));
    s[x]=0;

    sprintf(dt,"Date=%d-%02d-%02d, Time=%02d:%02d", Time.Year, Time.Month, Time.Date, Time.Hour, Time.Minutes);
    
    return dt;
    }

 /**********************************************************************************************\
 * Print de welkomsttekst van de Nodo.
 \*********************************************************************************************/
void PrintWelcome(void)
  {
  // Print Welkomsttekst
  PrintLine(ProgmemString(Text_22));
  PrintLine(ProgmemString(Text_01));
  PrintLine(ProgmemString(Text_02));

  // print versienummer
  sprintf(TempString,"Version= (Beta) %d.%d.%d, ThisUnit=%d",S.Version/100, (S.Version%100)/10, S.Version%10,S.Unit); //??? beta tekst verwijderen
  PrintLine(TempString);

  // print IP adres van de Nodo
  if(EthernetEnabled)
    {
    sprintf(TempString,"ThisUnit=%u.%u.%u.%u, TerminalPort=%d, EventPort=%d",Ethernet.localIP()[0],Ethernet.localIP()[1],Ethernet.localIP()[2],Ethernet.localIP()[3], S.Terminal_Port, S.Event_Port);
    PrintLine(TempString);
    }
    
  // geef melding als de SDCard goed geconnect is
  if(SDCardPresent)
    PrintLine(ProgmemString(Text_24));

  // Geef datum en tijd weer.
  if(Time.Day)
    {
    sprintf(TempString,"%s %s",DateTimeString(), cmd2str(Time.DaylightSaving?CMD_DLS_EVENT:0));
    PrintLine(TempString);
    }
    
  PrintLine(ProgmemString(Text_22));
  }


 /**********************************************************************************************\
 * Verzend teken(s) naar de Terminal
 \*********************************************************************************************/
void PrintLine(String LineToPrint)
  {
  if(SerialConnected)
    Serial.println(LineToPrint);
    
  if(EthernetEnabled)
    {
    if(S.Terminal_Enabled==VALUE_ON && TerminalConnected)
      {
      if(TerminalClient.connected())
        {
        TerminalClient.println(LineToPrint);
        }
      else
        {
        // als de client niet meer verbonden is, maar deze was dat wel,
        // dan de verbinding netjes afsluiten
        TerminalClient.flush();
        TerminalClient.stop();
        TerminalConnected=false;
        }
      }
    }
  }

char* Event2str(unsigned long Code)
  {
  int x,y;
  byte P1,P2,Par2_b; 
  boolean P2Z=true;     // vlag: true=Par2 als nul waarde afdrukken false=nulwaarde weglaten

  byte Type     = (Code>>28)&0xf;
  byte Command  = (Code>>16)&0xff;
  byte Par1     = (Code>>8)&0xff;
  byte Par2     = (Code)&0xff;
  static char EventString[50]; 

  //  Serial.print("*** Event   =");Serial.println(Code,HEX);//??? Debug
  //  Serial.print("*** Command =");Serial.println(Command,DEC);//??? Debug
  //  Serial.print("*** Par1    =");Serial.println(Par1,DEC);//??? Debug
  //  Serial.print("*** Par2    =");Serial.println(Par2,DEC);//??? Debug
  
  strcpy(EventString, "(");

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
      case CMD_WIRED_ANALOG:

        // sprintf(EventString,"( %d,%d.%d)",cmd2str(Command),int2str(y+1),int2str(x/10) ??? onderstaande simpeler maken met sprintf?

        strcat(EventString," ");                      
        strcat(EventString,int2str(analog2port(Par1,Par2)+1));// wired poort: voor gebruiker 1..8
        strcat(EventString,", ");        
        strcat(EventString,calibrated2str(Par1,Par2));        
        return EventString; // deze functie niet verder afwerken. Dit CMD_WIRED_ANALOG heeft een afwijkende MMI
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
      case CMD_DELAY:
      case CMD_WIRED_PULLUP:
      case CMD_WIRED_OUT:
      case CMD_WIRED_IN_EVENT:
        P1=P_VALUE;
        P2=P_TEXT;
        break;
  
      // Par1 als tekst en par2 als tekst
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
      case CMD_TERMINAL:
      case CMD_DLS_EVENT:
      case CMD_BUSY:
      case CMD_SENDBUSY:
      case CMD_WAITBUSY:
        P1=P_TEXT;
        P2=P_NOT;
        break;
  
      // Par1 als waarde en par2 niet
      case CMD_UNIT:
      case CMD_RAWSIGNAL:
      case CMD_RAWSIGNAL_SEND:
      case CMD_DIVERT:
      case CMD_SIMULATE_DAY:
      case CMD_CLOCK_DOW:
        P1=P_VALUE;
        P2=P_NOT;
        break;
  
      // Geen parameters
      case CMD_BOOT_EVENT:
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
        }
      }// P2
    }//   if(Type==SIGNAL_TYPE_NODO || Type==SIGNAL_TYPE_OTHERUNIT)
    
  else // wat over blijft is het type UNKNOWN.
    {
    strcat(EventString,int2str(Code));
    }
  strcat(EventString,")");
  return EventString;
  }



 /**********************************************************************************************\
 * 
 \*********************************************************************************************/
void PrintEGServers(void)
  {
  int x;
  
  // toon de clients die eerder met succes verbonden zijn en nu als server worden beschouwd;
  for(x=0;x<SERVER_IP_MAX;x++)
    {
    if((S.Server_IP[x][0]+S.Server_IP[x][1]+S.Server_IP[x][2]+S.Server_IP[x][3])>0)
      {
      sprintf(TempString,"%s=%u.%u.%u.%u",ProgmemString(Text_10),S.Server_IP[x][0],S.Server_IP[x][1],S.Server_IP[x][2],S.Server_IP[x][3]);
      PrintLine(TempString);
      }
    }
  }

