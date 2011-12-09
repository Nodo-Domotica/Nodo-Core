

 /*********************************************************************************************\
 * Zet het hoogste nibbel van een event zodat het event het Type kenmerk krijgt
 \*********************************************************************************************/
unsigned long SetEventType(unsigned long Event, byte Type)
  {
  return (Event&0x0fffffff) | (unsigned long)(Type)<<28;
  }



 /*********************************************************************************************\
 * (re)set een timer. Eenheden in seconden. 
 * Timer 1..15. Timer=0 is een wildcard voor alle timers
 * Als de timer op 0 wordt gezet, wordt er geen event gegenereerd.
 \*********************************************************************************************/
void TimerSet(byte Timer, int Time)
  {
  if(Timer==0)// 0=wildcard voor alle timers
    {
    for(int x=0;x<TIMER_MAX;x++)
      {
      if(Time==0)
        UserTimer[x]=0L;
      else
        UserTimer[x]=millis()+(unsigned long)(Time)*1000;
      }
    }
  else
    {
    if(Time==0)
      UserTimer[Timer-1]=0L;
    else
      UserTimer[Timer-1]=millis()+(unsigned long)Time*1000;
    }
  }

 /*********************************************************************************************\
 * Haal voor het opgegeven Command de status op.
 * Let op: call by reference!
 \*********************************************************************************************/
boolean GetStatus(byte *Command, byte *Par1, byte *Par2)
 { 
  byte x;
  byte xPar1=*Par1,xPar2=*Par2;
  
  *Par1=0;
  *Par2=0;
  switch (*Command)
    {
    case CMD_WAITFREERF: 
      *Par1=S.WaitFreeRF_Delay;
      *Par2=S.WaitFreeRF_Window;
      break;
  
    case CMD_UNIT: 
      *Par1=S.Unit;
      break;        

    case CMD_DLS_EVENT:
      *Par1=S.DaylightSaving?VALUE_ON:VALUE_OFF;
      break;

    case CMD_SENDBUSY:
      *Par1=S.SendBusy;
      break;
      
    case CMD_TERMINAL:
      *Par1=S.Terminal_Enabled;
      *Par2=S.Terminal_Prompt;
      break;
      
    case CMD_WAITBUSY:
      *Par1=S.WaitBusy?VALUE_ALL:VALUE_OFF;
      break;
                  
    case CMD_ANALYSE_SETTINGS:
      *Par1=S.AnalyseTimeOut/1000;
      *Par2=S.AnalyseSharpness;
      break;

    case CMD_TRANSMIT_RF:
      *Par1=S.TransmitRF;
      *Par2=S.TransmitRepeatIR;
      break;
      
    case CMD_TRANSMIT_IR:
      *Par1=S.TransmitIR;
      *Par2=S.TransmitRepeatIR;
      break;

    case CMD_CLOCK_EVENT_DAYLIGHT:
      *Par1=Time.Daylight;
      break;

    case CMD_VARIABLE_SET:
      *Par1=xPar1;
      *Par2=UserVar[xPar1-1];
      break;

    case CMD_CLOCK_DATE:
      *Par1=Time.Date;
      *Par2=Time.Month;
      break;

    case CMD_CLOCK_TIME:
      *Par1=Time.Hour;
      *Par2=Time.Minutes;
      break;

    case CMD_CLOCK_DOW:
      *Par1=Time.Day;
      break;

    case CMD_CLOCK_YEAR:
      *Par1=Time.Year/100;
      *Par2=Time.Year-2000;
      break;

    case CMD_TIMER_SET_MIN:
      *Par1=xPar1;
      if(UserTimer[xPar1-1])
        *Par2=(byte)((UserTimer[xPar1-1]-millis()+59999UL)/60000UL);
      else
        *Par2=0;
      break;

    case CMD_WIRED_RANGE:
      *Par1=xPar1;
      *Par2=S.WiredInputRange[xPar1-1];
      break;

    case CMD_WIRED_THRESHOLD:
      *Par1=xPar1;
      *Par2=S.WiredInputThreshold[xPar1-1];
      break;

    case CMD_WIRED_PULLUP:
      *Par1=xPar1;
      *Par2=(S.WiredInputPullUp[xPar1-1])?VALUE_ON:VALUE_OFF;
      break;

    case CMD_WIRED_SMITTTRIGGER:
      *Par1=xPar1;
      *Par2=S.WiredInputSmittTrigger[xPar1-1];
      break;

    case CMD_WIRED_IN_EVENT:
      *Par1=xPar1;
      *Par2=(WiredInputStatus[xPar1-1])?VALUE_ON:VALUE_OFF;
      break;

    case CMD_WIRED_ANALOG:
      *Par1=xPar1;
      *Par2=WiredAnalog(xPar1-1);
      break;
      
    case CMD_WIRED_OUT:
      *Par1=xPar1;
      *Par2=(WiredOutputStatus[xPar1-1])?VALUE_ON:VALUE_OFF;
      break;
      
    default:
      return false;
    }
  return true;
  }


 /**********************************************************************************************\
 * Deze functie haalt een tekst op uit PROGMEM en geeft als string terug
 * BUILD 01, 09-01-2010, P.K.Tonkes@gmail.com
 \*********************************************************************************************/
 char* ProgmemString(prog_char* text)
  {
  byte x=0;
  static char buffer[40];

  do
    {
    buffer[x]=pgm_read_byte_near(text+x);
    }while(buffer[x++]!=0);
  return buffer;
  }


 /*********************************************************************************************\
 * Sla alle settings op in het EEPROM geheugen.
 \*********************************************************************************************/
void SaveSettings(void)  
  {
  char ByteToSave,*pointerToByteToSave=pointerToByteToSave=(char*)&S;    //pointer verwijst nu naar startadres van de struct. 
  for(int x=0; x<sizeof(struct Settings) ;x++)
    {
    EEPROM.write(x,*pointerToByteToSave); 
    pointerToByteToSave++;
    }  
  }

 /*********************************************************************************************\
 * Laad de settings uit het EEPROM geheugen.
 \*********************************************************************************************/
boolean LoadSettings()
  {
  byte x;
    
  char ByteToSave,*pointerToByteToRead=(char*)&S;    //pointer verwijst nu naar startadres van de struct.
  for(int x=0; x<sizeof(struct Settings);x++)
    {
    *pointerToByteToRead=EEPROM.read(x);
    pointerToByteToRead++;// volgende byte uit de struct
    }
    
  for(x=0;x<USER_VARIABLES_MAX;x++)
    UserVar[x]=S.UserVar[x];
      
  }
 
  
 /*********************************************************************************************\
 * Alle settings van de Nodo weer op default.
 \*********************************************************************************************/
void ResetFactory(void)
  {
  Beep(2000,2000);
  int x,y;
  ClockRead();

  S.Version                = VERSION;
  S.Unit                   = UNIT;
  S.Trace                  = false;
  S.AnalyseSharpness       = 50;
  S.AnalyseTimeOut         = SIGNAL_TIMEOUT_IR;
  S.TransmitIR             = VALUE_ON;
  S.TransmitRF             = VALUE_ON;
  S.TransmitRepeatIR       = TX_REPEATS;
  S.TransmitRepeatRF       = TX_REPEATS;
  S.SendBusy               = VALUE_OFF;
  S.WaitBusy               = VALUE_OFF;
  S.WaitFreeRF_Window      = 0;
  S.WaitFreeRF_Delay       = 0;
  S.DaylightSaving         = Time.DaylightSaving;
  S.Event_Port             = 1024;
  S.Terminal_Port          = 23;
  S.Terminal_Enabled       = VALUE_OFF;
  S.Terminal_Prompt        = VALUE_ON;
  S.Password[0]            = 'N';
  S.Password[1]            = 'o';
  S.Password[2]            = 'd';
  S.Password[3]            = 'o';
  S.Password[4]            = 0;
//  S.Nodo_IP[0]             = 0;
//  S.Nodo_IP[1]             = 0;
//  S.Nodo_IP[2]             = 0;
//  S.Nodo_IP[3]             = 0;
  
  // zet alle IP adressen van de eventghost servers op 0
  for(x=0;x<SERVER_IP_MAX;x++)
    for(y=0;y<=3;y++)    
      S.Server_IP[x][y]=0;

  for(x=0;x<WIRED_PORTS;x++)
    {
    S.WiredInputThreshold[x]=0x80; 
    S.WiredInputSmittTrigger[x]=5;
    S.WiredInputRange[x]=0;
    S.WiredInputPullUp[x]=true;
    }

  // maak alle variabelen leeg
  for(byte x=0;x<USER_VARIABLES_MAX;x++)
     S.UserVar[x]=0;

  SaveSettings();  
  FactoryEventlist();
  delay(500);// kleine pauze, anders kans fout bij seriële communicatie
  Reset();
  }
  
 /**********************************************************************************************\
 * Maak de Eventlist leeg en herstel de default inhoud
 \*********************************************************************************************/
void FactoryEventlist(void)
  {
  Eventlist_Write(1,0L,0L); // maak de eventlist leeg.
  Eventlist_Write(0,command2event(CMD_BOOT_EVENT,0,0),command2event(CMD_SOUND,7,0)); // geluidssignaal na opstarten Nodo
  Eventlist_Write(0,command2event(CMD_COMMAND_WILDCARD,VALUE_SOURCE_IR,CMD_KAKU),command2event(CMD_RAWSIGNAL_SEND,0,0)); // Kort geluidssignaal bij ieder binnenkomend event
  }

  

/* This function places the current value of the heap and stack pointers in the
 * variables. You can call it from any place in your code and save the data for
 * outputting or displaying later. This allows you to check at different parts of
 * your program flow.
 * The stack pointer starts at the top of RAM and grows downwards. The heap pointer
 * starts just above the static variables etc. and grows upwards. SP should always
 * be larger than HP or you'll be in big trouble! The smaller the gap, the more
 * careful you need to be. Julian Gall 6-Feb-2009.
 */
uint8_t *heapptr, *stackptr;
void FreeMemory(int x) 
  {
  stackptr = (uint8_t *)malloc(4);          // use stackptr temporarily
  heapptr = stackptr;                     // save value of heap pointer
  free(stackptr);      // free up the memory again (sets stackptr to 0)
  stackptr =  (uint8_t *)(SP);           // save value of stack pointer
  Serial.print("*** Free RAM (");Serial.print(x,DEC);Serial.print(")=");Serial.println(stackptr-heapptr,DEC);
  }

 /**********************************************************************************************\
 * Geeft de analoge waarde [0..255] van de WIRED-IN poort <WiredPort> [0..3]
 \**********************************************************************************************/
byte WiredAnalog(byte WiredPort)
  {
  int x=analogRead(WiredAnalogInputPin_1+WiredPort);
    
  if(S.WiredInputRange[WiredPort]==0)
    // Hele bereik 0..5 volt 
    // S.WiredInputRange=0, bits 0..10, Spanning 0.00-5.00, Resolutie 19.53 mllivolt. ADC bereik 0..1023 delen door 4 om op 0.255 uit te komen
    x=x>>2;
  else
    {
    // Deel van het bereik gebruiken
    // S.WiredInputRange=1, bits 0..7,  Spanning 0.00-1.24, Resolutie 4.88 mllivolt. ADC bereik 0..255
    // S.WiredInputRange=2, bits 1..8,  Spanning 1.25-2.49, Resolutie 4.88 mllivolt. ADC bereik 256..511
    // S.WiredInputRange=3, bits 2..9,  Spanning 2.50-3.74, Resolutie 4.88 mllivolt. ADC bereik 512..983
    // S.WiredInputRange=4, bits 3..10, Spanning 3.75-5.00, Resolutie 4.88 mllivolt. ADC bereik 768..1023
    x=x-256*(int(S.WiredInputRange[WiredPort])-1);
    }
  if(x>255)x=255;
  if(x<0)x=0;
  return x;
  }

 /**********************************************************************************************\
  * Geeft de status weer of verzendt deze.
  *
  *
 \**********************************************************************************************/
void Status(boolean ToSerial, byte Par1, byte Par2)
  {
  byte CMD_Start,CMD_End;
  byte Par1_Start,Par1_End;
  byte x,P1,P2; // in deze variabele wordt de waarde geplaats (call by reference)
  
  if(Par1==0)
    return;
    
  if(Par2==VALUE_ALL)
    Par2==0;

  if(Par1==CMD_BOOT_EVENT)
    {
    PrintWelcome();
    return;
    }
    
  if(Par1==VALUE_ALL)
    {
    Par2=0;
    if(ToSerial)
      PrintWelcome();
    CMD_Start=FIRST_COMMAND;
    CMD_End=LAST_COMMAND;
    }
  else
    {
    if(!GetStatus(&Par1,&P1,&P2)) // P1 en P2 even gebruikt als dummy
      return;
    CMD_Start=Par1;
    CMD_End=Par1;
    }

  // tijdelijk logging naar de SDCard uitzetten omdat anders alle status informatie ook wordt opgeslagen op de SDCard door de PrintEvent routine.
  boolean SDCardPresentBackup=SDCardPresent;
  SDCardPresent=false;

  for(x=CMD_Start; x<=CMD_End; x++)
    {
    if(GetStatus(&x,&Par1_Start,&Par1_Start)) // Als het een geldige uitvraag is. let op: call by reference ! Par1_Start is gebruikt als dummy.
      {
      if(Par2==0) // Als in het commando 'Status Par1, Par2' Par2 niet is gevuld met een waarde
        {
        switch(x)
          {
          case CMD_WIRED_ANALOG:
          case CMD_WIRED_OUT:
          case CMD_WIRED_PULLUP:
          case CMD_WIRED_SMITTTRIGGER:
          case CMD_WIRED_THRESHOLD:
          case CMD_WIRED_RANGE:
          case CMD_WIRED_IN_EVENT:
            Par1_Start=1;
            Par1_End=WIRED_PORTS;
            break;      
          case CMD_VARIABLE_SET:
            Par1_Start=1;
            Par1_End=USER_VARIABLES_MAX;
            break;
          case CMD_TIMER_SET_MIN:
            Par1_Start=1;
            Par1_End=TIMER_MAX;
            break;
          default:
            Par1_Start=0;
            Par1_End=0;
          }
        }
      else
        {
        Par1_Start=Par2;
        Par1_End=Par2;
        }

      for(byte y=Par1_Start;y<=Par1_End;y++)
          {
          P1=y;
          P2=0;
          GetStatus(&x,&P1,&P2); // haal status op. Call by Reference!
          if(ToSerial)
            PrintEvent(command2event(x,P1,P2),CMD_STATUS,VALUE_DIRECTION_OUTPUT);  // geef event weer op Serial
          else
            TransmitCode(command2event(x,P1,P2),SIGNAL_TYPE_NODO);
          }
      }// if Getstatus
    }// for x

  if(ToSerial && CMD_Start!=CMD_End) 
    PrintLine(ProgmemString(Text_22));

  // logging naar SDCard weer inschakelen indien deze actief was.
  SDCardPresent=SDCardPresentBackup;
  }


/*********************************************************************************************\
* Deze routine parsed string en geeft het opgegeven argument Argc terug in Argv
* argumenten worden van elkaar gescheiden door een puntkomma, komma of een spatie.
\*********************************************************************************************/
boolean GetArgv(char *string, char *argv, byte argc)
  {
  int string_pos=0,argv_pos=0,argc_pos=0; 
  argc--;
  int c;

  while(string[string_pos]==32)string_pos++;   // sla voorloopspaties over
  
  while(string[string_pos]!=0 && argc_pos<=argc)
    {
    if(string[string_pos]=='.' || string[string_pos]==','|| string[string_pos]==';' || string[string_pos]==32)
      {
      while(string[string_pos+1]==32)string_pos++;   // sla voorloopspaties over
      argc_pos++;
      }
    else if(string[string_pos]>=33 && string[string_pos]<=126 && argc_pos==argc)
      argv[argv_pos++]=string[string_pos];
      
    string_pos++;
    }    
  argv[argv_pos]=0;
  
  if(argv_pos>0)
    return true;
  return false;
  }


/**********************************************************************************************\
 * Geef een geluidssignaal met toonhoogte van 'frequentie' in Herz en tijdsduur 'duration' in milliseconden.
 * LET OP: toonhoogte is slechts een grove indicatie. Deze routine is bedoeld als signaalfunctie
 * en is niet bruikbaar voor toepassingen waar de toonhoogte zuiver/exact moet zijn. Geen PWM.
 * Definieer de constante:
 * #define BuzzerPin <LuidsprekerAansluiting>
 * Neem in setup() de volgende regel op:
 * pinMode(BuzzerPin, OUTPUT);
 * Routine wordt verlaten na beeindiging van de pieptoon.
 * Revision 01, 13-02-2009, P.K.Tonkes@gmail.com
 \*********************************************************************************************/

void Beep(int frequency, int duration)//Herz,millisec 
  {
  long halfperiod=500000L/frequency;
  long loops=(long)duration*frequency/(long)1000;
  for(loops;loops>0;loops--) 
    {
    digitalWrite(BuzzerPin, HIGH);
    delayMicroseconds(halfperiod);
    digitalWrite(BuzzerPin, LOW);
    delayMicroseconds(halfperiod);
    }
  }
 
 /**********************************************************************************************\
 * Geeft een belsignaal.
 * Revision 01, 09-03-2009, P.K.Tonkes@gmail.com
 \*********************************************************************************************/
void Alarm(int Variant,int Duration)
  {
   byte x,y;
   if(Duration==0)Duration=1;

   switch (Variant)
    { 
    case 1:// four beeps
      for(y=1;y<=(Duration>1?Duration:1);y++)
        {
        Beep(3000,30);
        delay(100);
        Beep(3000,30);
        delay(100);
        Beep(3000,30);
        delay(100);
        Beep(3000,30);
        delay(1000);
        }    
      break;

    case 2: // whoop up
      for(y=1;y<=(Duration>1?Duration:1);y++)
        {
        for(x=1;x<=50;x++)
            Beep(250*x/4,20);
        }          
      break;

    case 3: // whoop down
      for(y=1;y<=(Duration>1?Duration:1);y++)
        {
        for(x=50;x>0;x--)
            Beep(250*x/4,20);
        }          
      break;

    case 4:// S.O.S.
      for(y=1;y<=(Duration>1?Duration:1);y++)
        {
          Beep(1200,50);
          delay(100);
          Beep(1200,50);
          delay(100);
          Beep(1200,50);
          delay(200);
          Beep(1200,300);
          delay(100);
          Beep(1200,300);
          delay(100);
          Beep(1200,300);
          delay(200);
          Beep(1200,50);
          delay(100);
          Beep(1200,50);
          delay(100);
          Beep(1200,50);
          if(Duration>1)delay(500);
        }
      break;
          
     case 5:// ding-dong
       for(x=0;x<Duration;x++)
         {
         if(x>0)delay(2000);
         Beep(1500,500);
         Beep(1200,500);
         }    
       break;

    case 6: // phone ring
      for(x=0;x<(15*Duration);x++)
        {
          Beep(1000,40);
          Beep(750,40);
        }
        break;

    case 7: // boot
        Beep(1500,100);
        Beep(1000,100);
        break;
                       
    default:// beep
       Beep(2000,20*(Duration>1?Duration:1));
       break;
    }
  }


/*********************************************************************************************\
* Deze routine parsed string en zoekt naar keyword. Geeft de startpositie terug waar het keyword
* gevonden is. -1 indien niet gevonden. Niet casesensitive.
\*********************************************************************************************/
int StringFind(char *string, char *keyword)
  {
  int x,y;
  int keyword_len=strlen(keyword);
  int string_len=strlen(string);
  
  for(x=0; x<=(string_len-keyword_len); x++)
    {
    y=0;

    while(y<keyword_len && (tolower(string[x+y])==tolower(keyword[y])))
      {
      y++;
      }

    if(y==keyword_len)
      return x;
    }
  return -1;
  }

 /**********************************************************************************************\
 * Schrijft een event in de Eventlist. Deze Eventlist bevindt zich in het EEPROM geheugen.
 \*********************************************************************************************/
boolean Eventlist_Write(int address, unsigned long Event, unsigned long Action)// LET OP: eerste adres=1
  {
  unsigned long TempEvent,TempAction;

  // als adres=0, zoek dan de eerste vrije plaats.
  if(address==0)
    {
    address++;
    while(Eventlist_Read(address,&TempEvent,&TempAction))address++;
    }
  if(address>EVENTLIST_MAX)return false;// geen geldig adres meer c.q. alles is vol.

  address--;// echte adressering begint vanaf nul. voor de user vanaf 1.
  address=address*8+sizeof(struct Settings);     // Eerste deel van het EEPROM geheugen is voor de settings. Reserveer deze bytes. Deze niet te gebruiken voor de Eventlist!

  EEPROM.write(address++,(Event>>24  & 0xFF));
  EEPROM.write(address++,(Event>>16  & 0xFF));
  EEPROM.write(address++,(Event>> 8  & 0xFF));
  EEPROM.write(address++,(Event      & 0xFF));
  EEPROM.write(address++,(Action>>24 & 0xFF));
  EEPROM.write(address++,(Action>>16 & 0xFF));
  EEPROM.write(address++,(Action>> 8 & 0xFF));
  EEPROM.write(address++,(Action     & 0xFF));

  // Eerste volgende Event vullen met een 0. Dit markeert het einde van de Eventlist.
  EEPROM.write(address++,0);
  EEPROM.write(address++,0);
  EEPROM.write(address++,0);
  EEPROM.write(address  ,0);
  
  return true;
  }
  
 /**********************************************************************************************\
 * 
 * Revision 01, 09-12-2009, P.K.Tonkes@gmail.com
 \*********************************************************************************************/
boolean Eventlist_Read(int address, unsigned long *Event, unsigned long *Action)// LET OP: eerste adres=1
  {
  if(address>EVENTLIST_MAX)return(false);
  address--;// echte adressering begint vanaf nul. voor de user vanaf 1.
  address=address*8+sizeof(struct Settings);     // Eerste deel van het EEPROM geheugen is voor de settings. Reserveer deze bytes. Deze niet te gebruiken voor de Eventlist!

  *Event  =((unsigned long)(EEPROM.read(address++))) << 24;
  *Event|= ((unsigned long)(EEPROM.read(address++))) << 16;
  *Event|= ((unsigned long)(EEPROM.read(address++))) <<  8;
  *Event|= ((unsigned long)(EEPROM.read(address++)))      ;
  *Action =((unsigned long)(EEPROM.read(address++))) << 24;
  *Action|=((unsigned long)(EEPROM.read(address++))) << 16;
  *Action|=((unsigned long)(EEPROM.read(address++))) <<  8;
  *Action|=((unsigned long)(EEPROM.read(address  )))      ;
  if(*Event==0L)
    return(false);
  else
    return(true);
  }

/*********************************************************************************************\
* Funties op dit tabblad worden gebruikt voor het genereren van een MD5-hascode
* voor het veilig kunnen checken van een wachtwoord.
\*********************************************************************************************/
uint32_t md5_T[] PROGMEM = {
	0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf,
	0x4787c62a, 0xa8304613, 0xfd469501, 0x698098d8, 0x8b44f7af,
	0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e,
	0x49b40821, 0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
	0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8, 0x21e1cde6,
	0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8,
	0x676f02d9, 0x8d2a4c8a, 0xfffa3942, 0x8771f681, 0x6d9d6122,
	0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
	0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039,
	0xe6db99e5, 0x1fa27cf8, 0xc4ac5665, 0xf4292244, 0x432aff97,
	0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d,
	0x85845dd1, 0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
	0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 };

  
  #define MD5_HASH_BITS  128
  #define MD5_HASH_BYTES (MD5_HASH_BITS/8)
  #define MD5_BLOCK_BITS 512
  #define MD5_BLOCK_BYTES (MD5_BLOCK_BITS/8)
  #define MD5_HASH_BITS  128

  // typedef uint8_t md5_hash_t[MD5_HASH_BYTES];

  struct md5_ctx_t 
    {
    uint32_t a[4];
    int32_t counter;
    };
  
  // typedef uint8_t md5_hash_t[MD5_HASH_BYTES];
  
  void md5_init(struct md5_ctx_t *s){
  	s->counter = 0;
  	s->a[0] = 0x67452301;
  	s->a[1] = 0xefcdab89;
  	s->a[2] = 0x98badcfe;
  	s->a[3] = 0x10325476;
  }
  
  static
  uint32_t md5_F(uint32_t x, uint32_t y, uint32_t z){
  	return ((x&y)|((~x)&z));
  }
  
  static
  uint32_t md5_G(uint32_t x, uint32_t y, uint32_t z){
  	return ((x&z)|((~z)&y));
  }
  
  static
  uint32_t md5_H(uint32_t x, uint32_t y, uint32_t z){
  	return (x^y^z);
  }
  
  static
  uint32_t md5_I(uint32_t x, uint32_t y, uint32_t z){
  	return (y ^ (x | (~z)));
  }
  
  typedef uint32_t md5_func_t(uint32_t, uint32_t, uint32_t);
  
  #define ROTL32(x,n) (((x)<<(n)) | ((x)>>(32-(n))))  
  
  static
  void md5_core(uint32_t* a, void* block, uint8_t as, uint8_t s, uint8_t i, uint8_t fi){
  	uint32_t t;
  	md5_func_t* funcs[]={md5_F, md5_G, md5_H, md5_I};
  	as &= 0x3;
  	/* a = b + ((a + F(b,c,d) + X[k] + T[i]) <<< s). */
  	t = a[as] + funcs[fi](a[(as+1)&3], a[(as+2)&3], a[(as+3)&3])
  	    + *((uint32_t*)block) + pgm_read_dword(md5_T+i) ;
  	a[as]=a[(as+1)&3] + ROTL32(t, s);
  }
  
  void md5_nextBlock(struct md5_ctx_t *state, const void* block){
  	uint32_t	a[4];
  	uint8_t		m,n,i=0;
  	/* this requires other mixed sboxes */
  	a[0]=state->a[0];
  	a[1]=state->a[1];
  	a[2]=state->a[2];
  	a[3]=state->a[3];
  
  	/* round 1 */
  	uint8_t s1t[]={7,12,17,22}; // 1,-1   1,4   2,-1   3,-2
  	for(m=0;m<4;++m){
  		for(n=0;n<4;++n){
  			md5_core(a, &(((uint32_t*)block)[m*4+n]), 4-n, s1t[n],i++,0);
  		}
  	}
  	/* round 2 */
  	uint8_t s2t[]={5,9,14,20}; // 1,-3   1,1   2,-2   2,4
  	for(m=0;m<4;++m){
  		for(n=0;n<4;++n){
  			md5_core(a, &(((uint32_t*)block)[(1+m*4+n*5)&0xf]), 4-n, s2t[n],i++,1);
  		}
  	}
  	/* round 3 */
  	uint8_t s3t[]={4,11,16,23}; // 0,4   1,3   2,0   3,-1
  	for(m=0;m<4;++m){
  		for(n=0;n<4;++n){
  			md5_core(a, &(((uint32_t*)block)[(5-m*4+n*3)&0xf]), 4-n, s3t[n],i++,2);
  		}
  	}
  	/* round 4 */
  	uint8_t s4t[]={6,10,15,21}; // 1,-2   1,2   2,-1   3,-3
  	for(m=0;m<4;++m){
  		for(n=0;n<4;++n){
  			md5_core(a, &(((uint32_t*)block)[(0-m*4+n*7)&0xf]), 4-n, s4t[n],i++,3);
  		}
  	}
  	state->a[0] += a[0];
  	state->a[1] += a[1];
  	state->a[2] += a[2];
  	state->a[3] += a[3];
  	state->counter++;
  }
  
  void md5_lastBlock(struct md5_ctx_t *state, const void* block, uint16_t length_b){
  	uint16_t l;
  	uint8_t b[64];
  	while (length_b >= 512){
  		md5_nextBlock(state, block);
  		length_b -= 512;
  		block = ((uint8_t*)block) + 512/8;
  	}
  	memset(b, 0, 64);
  	memcpy(b, block, length_b/8);
  	/* insert padding one */
  	l=length_b/8;
  	if(length_b%8){
  		uint8_t t;
  		t = ((uint8_t*)block)[l];
  		t |= (0x80>>(length_b%8));
  		b[l]=t;
  	}else{
  		b[l]=0x80;
  	}
  	/* insert length value */
  	if(l+sizeof(uint64_t) >= 512/8){
  		md5_nextBlock(state, b);
  		state->counter--;
  		memset(b, 0, 64-8);
  	}
  	*((uint64_t*)&b[64-sizeof(uint64_t)]) = (state->counter * 512) + length_b;
  	md5_nextBlock(state, b);
  }
  
  void md5_ctx2hash(struct md5_hash_t* dest, const struct md5_ctx_t* state){
  	memcpy(dest, state->a, MD5_HASH_BYTES);
  }
  
  void md5(struct md5_hash_t* dest, const void* msg)
    {
    uint32_t length_b = strlen((char*)msg) * 8;
    struct md5_ctx_t ctx;
  
    md5_init(&ctx);
    while(length_b>=MD5_BLOCK_BITS){
      md5_nextBlock(&ctx, msg);
      msg = (uint8_t*)msg + MD5_BLOCK_BYTES;
      length_b -= MD5_BLOCK_BITS;
    }
  md5_lastBlock(&ctx, msg, length_b);
  md5_ctx2hash(dest, &ctx);
  }

char* ip2str(byte* IP)
  {
  static char str[20];
  sprintf(str,"%u.%u.%u.%u",IP[0],IP[1],IP[2],IP[3]);
  return str;
  }

char* int2str(unsigned long x)
  {
  static char OutputLine[10];
  char* OutputLinePosPtr=&OutputLine[10];
  int y;
  
  *OutputLinePosPtr=0;

  if(x==0)
    {
    *--OutputLinePosPtr='0';
    }
    
  else if(x<=10000)  // getallen weergeven als een integer in decimaal formaat
    {
    while(x>0)
      {
      *--OutputLinePosPtr='0'+(x%10);
      x/=10;
      }
    }
    
  else // getallen weergeven als een integer in hexadecimaal formaat
    {
    while(x>0)
      {
      y=x&0xf;

      if(y<10)
        *--OutputLinePosPtr='0'+y;
      else
        *--OutputLinePosPtr='A'+(y-10);

      x=x>>4;;
      }
    *--OutputLinePosPtr='x';
    *--OutputLinePosPtr='0';
    }
    
  return OutputLinePosPtr;
  }

void RaiseError(byte ErrorCode)
  {
  unsigned long eventcode;

  eventcode=command2event(CMD_ERROR,ErrorCode,0);
  TransmitCode(eventcode,SIGNAL_TYPE_NODO);    
  PrintEvent(eventcode,VALUE_DIRECTION_INTERNAL,VALUE_SOURCE_SYSTEM);  // geef event weer op Serial
  }
    
 /**********************************************************************************************\
 * Voeg een regel toe aan de logfile.
 \*********************************************************************************************/
boolean LogSDCard(char *Line)
  {
  if(SDCardPresent)
    {
    // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer SDCard chip
    digitalWrite(Ethernetshield_CS_W5100, HIGH);
    digitalWrite(EthernetShield_CS_SDCard,LOW);

    File LogFile = SD.open(ProgmemString(Text_23), FILE_WRITE);
    if(LogFile) 
      {
      LogFile.write((uint8_t*)Line,strlen(Line));      
      LogFile.write('\n'); // nieuwe regel
      LogFile.close();
      }
    else 
      {
      SDCardPresent=false; // niet meer weer proberen weg te schrijven.
      TransmitCode(command2event(CMD_ERROR,ERROR_03,0),SIGNAL_TYPE_NODO);
      }

    // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer W510 chip
    digitalWrite(Ethernetshield_CS_W5100, LOW);
    digitalWrite(EthernetShield_CS_SDCard,HIGH);
    }
  return SDCardPresent;
  }


