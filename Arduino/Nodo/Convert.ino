
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
unsigned long str2int(char *string)
  {
  int x;
  
  x=str2cmd(string);
  if(x>0)
    return x;

  return(strtol(string,NULL,0));
  
  }

/**********************************************************************************************\
 * Converteert een string volgens formaat "<Home><address>" naar een absoluut adres [0..255]
 \*********************************************************************************************/
int HA2address(char* HA, byte *group)
  {
  byte x=0,y=false; // teller die wijst naar het het te behandelen teken
  byte c;   // teken uit de string die behandeld wordt
  byte Home=0,Address=0;// KAKU-Home en KAKU-adres
 
  while((c=tolower(HA[x++]))!=0)
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

 /**********************************************************************************************\
 * Converteert een 4byte array IP adres naar een string.
 \*********************************************************************************************/
char* ip2str(byte* IP)
  {
  static char str[20];
  sprintf(str,"%u.%u.%u.%u",IP[0],IP[1],IP[2],IP[3]);
  return str;
  }

 /**********************************************************************************************\
 * Converteert een string naar een 4byte array IP adres
 * 
 \*********************************************************************************************/
boolean str2ip(char *string, byte* IP)
  {
  byte c;
  byte part=0;
  int value=0;
  
  for(int x=0;x<=strlen(string);x++)
    {
    c=string[x];
    if(isdigit(c))
      {
      value*=10;
      value+=c-'0';
      }
      
    else if(c=='.' || c==0) // volgende deel uit IP adres
      {
      if(value<=255)
        IP[part++]=value;
      else 
        return false;
      value=0;
      }
    else if(c==' ') // deze tekens negeren
      ;
    else // ongeldig teken
      return false;
    }
  if(part==4)// correct aantal delen van het IP adres
    return true;
  return false;
  }


 /**********************************************************************************************\
 * Converteert een unsigned long naar een integer.
 * als waarde groter dan 10.000, dan resultaat in hexadecimaal formaat.
 \*********************************************************************************************/
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


 /**********************************************************************************************\
 * De Nodo gebruikt voor metingen analoge waarden. Deze analoge waarden worden zodanig in een 
 * int vastgelegd dat zij naar de gebruiker kunnen worden getoond als een getal met cijfers achter
 * komma. Dit wordt do gedaan ondat gebruik van floats erg veel geheugen vraagt.
 * een int voor analoge waarden bevat de analoge waarde maal honderd, zodat deze later kunnen
 * worden getoond met twee cijfers achter de komma.
 * Voor uitwisseling van events met analoge waarden worden Par1 en Par2 anders gebruikt. Zij worden
 * samengevoegd tot een 16-bit waarde met de volgende indeling:
 *
 * WWWWhsnnnnnnnnnn, waarbij W de Wired poort is en n de analoge waarde en s de sign-bit voor negatieve getallen
 * h= high/low bit. als h=1 dan bereik 0.0-100.0, als h=0 dan bereik 0.00-10.00
 * 
 * De volgende conversiefunkties worden gebruikt:
 *
 * str2AnalogInt();    converteert een string "-nnn.nn" naar een int
 * AnalogInt2str();    converteert een int naar een string met format "-nnn.nnn"
 * event2AnalogInt();  converteert een eventcode naar een int
 * AnalogInt2event();  converteert een AnalogInt + poort naar een eventcode
 * Par2PortAnalog();  converteert een set Par1 en Par2 met integer waarden naar Poort en Analoge waarde
 * PortAnalog2Par();  converteert een poort en analoge waarde naar Par1 en Par2
 *
 * verder:
 *   (event>>12)&0x0f haalt de wired poort uit een eventcode
 \*********************************************************************************************/

void Par2PortAnalog(byte Par1, byte Par2, int *port, int *wi)
  {
  *port=(Par1>>4)&0xf;

  *wi=((Par1<<8)|Par2)&0x3ff;    // 10-bits waarde

  if(Par1 & B1000) // als high bit staat
    *wi=*wi*10;
  
  if(Par1 & B0100) // als sign bit staat
    *wi=-(*wi);
  }
  
void PortAnalog2Par(byte *Par1, byte *Par2, int port, int wi)
  {
  boolean high=false;
  boolean sign=false;
  
  if(wi<0)
    {
    sign=true;
    wi=-wi;
    }
    
  if(wi>=1000)
    {
    high=true;
    wi=wi/10;
    }

  *Par1=(port<<4) |  (high<<3) | (sign<<2) | ((wi>>8)&3);
  *Par2=wi & 0xff;    
  }
  

char* AnalogInt2str(int wi)
  {
  static char rString[10];
  if(abs(wi)>10230)
    strcpy(rString,"Overflow");
  else
    {
    if(abs(wi)>=1000)
      {
      wi/=10;
      sprintf(rString,"%d.%01d",wi/10,abs(wi)%10);
      }
    else
      {
      sprintf(rString,"%d.%02d",wi/100,abs(wi)%100);
      }
    }
  return rString;
  }

int str2AnalogInt(char* string)
  {
  byte x,c;
  int dot=100;
  int result=0;           // resultaatwaarde achter de decimale punt
  boolean negative=false;  // vlag staat als het sign teken is gevonden is.
  byte digit=0;
  
  for(byte pos=0; pos<=strlen(string); pos++)
    {
    c=string[pos];
    if(c=='-')
      negative=true;
    
    else if(c=='.')
      dot=10;

    else if(isdigit(c) && digit<4)
      {
      digit++;
      if(dot==100)
        {
        result=result*10;
        result=result+(c-'0')*dot;        
        }
      else
        {
        result=result+(c-'0')*dot;        
        dot=dot/10;
        }
      }
    }
        
  if(result>10230)
    {
    Serial.println("*** debug: str2analogint(); Overflow!");//???
    return 0;
    }      
  
  if(negative)
    result=-result;

  //  Serial.print("*** debug: str2AnalogInt()=");Serial.println(result,DEC);//??? Debug         
  return result;
  }


unsigned long AnalogInt2event(int wi, byte port, byte cmd)
  {
  boolean high=false;
  boolean sign=false;
  
  if(wi<0)
    {
    sign=true;
    wi=-wi;
    }
    
  if(wi>=1000)
    {
    high=true;
    wi=wi/10;
    }

  return ((unsigned long)SIGNAL_TYPE_NODO)<<28   |
         ((unsigned long)S.Unit)<<24             | 
         ((unsigned long)cmd)<<16                |
         ((unsigned long)port)<<12               |
         ((unsigned long)high)<<11               |
         ((unsigned long)sign)<<10               |
         ((unsigned long)(wi & 0x3ff));
  }
  
int event2AnalogInt(unsigned long event)
  {
  int wi; 
  
  wi=event&0x3ff;    // 10-bits waarde

  if(event & (1<<11)) // als high bit staat
    wi=wi*10;
  
  if(event & (1<<10)) // als sign bit staat
    wi=-wi;
  
  return wi;
  }
  

 /**********************************************************************************************\
 * vult een string met een regel uit de Eventlist.
 * geeft false terug als de regel leeg is
 \*********************************************************************************************/
boolean EventlistEntry2str(int entry, byte d, char* Line, boolean Script)
  {
  unsigned long Event, Action;
    
  Eventlist_Read(entry,&Event,&Action); // leesregel uit de Eventlist.    
  if(Event==0)
    return false;

  // Geef de entry van de eventlist weer
  strcpy(Line,cmd2str(Script?CMD_EVENTLIST_WRITE:VALUE_SOURCE_EVENTLIST));
  if(!Script)
    {
    strcat(Line," ");
    strcat(Line,int2str(entry));
    }

  // geef het event weer
  strcat(Line,"; ");
  strcat(Line,Event2str(Event));

  // geef het action weer
  strcat(Line,"; ");
  strcat(Line,Event2str(Action));
  return true;
  }
