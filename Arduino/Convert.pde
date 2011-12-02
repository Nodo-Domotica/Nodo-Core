 /*********************************************************************************************\
 * Een string formaat van een Event weer in de vorm "(<commando> <parameter-1>,<parameter-2>)"
 \*********************************************************************************************/
#define P_NOT   0
#define P_TEXT  1
#define P_KAKU  2
#define P_VALUE 3
#define P_DIM   4

char* Event2str(unsigned long Code)
  {
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
      case CMD_TERMINAL:
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
      case CMD_DLS_EVENT:
      case CMD_ERROR:
      case CMD_BUSY:
      case CMD_SENDBUSY:
      case CMD_WAITBUSY:
        P1=P_TEXT;
        P2=P_NOT;
        break;
  
      // Par1 als waarde en par2 niet
      case CMD_UNIT:
      case CMD_DIVERT:
      case CMD_SIMULATE_DAY:
      case CMD_CLOCK_DOW:
        P1=P_VALUE;
        P2=P_NOT;
        break;
  
      // Geen parameters
      case CMD_SEND_SIGNAL:
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
    strcat(EventString,int2str(Code));
    
  strcat(EventString,")");
  return EventString;
  }

 /*********************************************************************************************\
 * kopiëer de string van een commando naar een string[]
 \*********************************************************************************************/
char* cmd2str(int i)
  {
  static char string[30];
  if(i<COMMAND_MAX)
    {
    strcpy_P(string,(char*)pgm_read_word(&(CommandText_tabel[i])));
    }
  else
    string[0]=0;// als er geen gevonden wordt, dan is de string leeg
  return string;
  }
  
 /*********************************************************************************************\
 * Haal uit een string de commando code. False indien geen geldige commando code.
 \*********************************************************************************************/
int str2cmd(char *command)
  {
  for(int x=0;x<=COMMAND_MAX;x++)
    if(strcasecmp(command,cmd2str(x))==0)
      return x;      

  return false;
  }

 /*********************************************************************************************\
 * Bouw een Code op uit commando, data1 en data2
 \*********************************************************************************************/
unsigned long command2event(int Command, byte Par1, byte Par2)
    {
    return ((unsigned long)SIGNAL_TYPE_NODO)<<28   | 
           ((unsigned long)S.Unit)<<24            | 
           ((unsigned long)Command)<<16           | 
           ((unsigned long)Par1)<<8               | 
            (unsigned long)Par2;
    }


/*********************************************************************************************\
 * String mag HEX, DEC of een NODO commando zijn.
 * Deze routine converteert uit een string een unsigned long waarde.
 * De string moet beginnen met het eerste teken(dus geen voorloop spaties).
 * bij ongeldige tekens in de string wordt een false terug gegeven.
 \*********************************************************************************************/
unsigned long str2val(char *string)
  {
  byte v,w,x,y,c;
  unsigned long Value;
  
  // Check of het een DEC waarde is.
  Value=0;
  x=0;
  do
    {
    if(string[x]>='0' && string[x]<='9')
      {
      Value=Value*10;
      Value+=string[x]-48;
      }
    else
      {
      x=false; // geen geldig DEC getal
      break;
      }
    x++;
    }while(string[x]!=0);
  if(x)return Value;

  // check of het een HEX code is.
  Value=0;
  x=0;
  do
    {
    if(string[x]>='0' && string[x]<='9')
      {
      Value=Value<<4;
      Value+=string[x]-48;
      }
    else
      {
      if(string[x]>='a' && string[x]<='f')
        {
        Value=Value<<4;
        Value+=string[x]-87;
        }
      else
        {
        if(x<1 || string[x-1]!='0' || string[x]!='x')
          {
          x=false; // geen geldig HEX getal
          break;
          }
        }
      }
    x++;
    }while(string[x]!=0);
  if(x)return Value;
  return str2cmd(string);
  }

/**********************************************************************************************\
 * Converteert een string volgens formaat "<Home><address>" naar een absoluut adres [0..255]
 \*********************************************************************************************/
int HA2address(char* HA, byte *group)
  {
  byte x=0,y=false; // teller die wijst naar het het te behandelen teken
  byte c;   // teken uit de string die behandeld wordt
  byte Home=0,Address=0;// KAKU-Home en KAKU-adres
 
  while((c=HA[x++])!=0)
    {
    if(c>='0' && c<='9'){Address=Address*10;Address=Address+c-'0';}
    if(c>='a' && c<='p'){Home=c-'a';y=true;} // KAKU home A is intern 0
    }

  if(y)// notatie [A1..P16]
    {
    if(Address==0)
      {// groep commando is opgegeven: 0=alle adressen
      *group=true;
      return Home<<4;
      }
    else
      return (Home<<4) | (Address-1);        
    }
  else // absoluut adres [0..255]
    return Address; // KAKU adres 1 is intern 0     
  }

  
 /**********************************************************************************************\
 * geeft *char pointer terug die naar een PROGMEM string wijst.
 \*********************************************************************************************/
char* PROGMEM2str(prog_char* text)
  {
  byte x=0;
  static char buffer[60];

  do
    {
    buffer[x]=pgm_read_byte_near(text+x);
    }while(buffer[x++]!=0);
  return buffer;  
  }

