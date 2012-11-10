
 /**********************************************************************************************\
 * De Nodo gebruikt voor metingen analoge waarden. Deze analoge waarden worden zodanig in een 
 * int vastgelegd dat zij naar transporteerbaar zijn in een kleiner bit-formaat om zo 
 * het aaltal te verzenden/ontvangen bits te beperken. Dit variabeletype wordt een NodoFloat genoemd.
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
 * NodoFloat2event();  converteert een AnalogInt + poort naar een eventcode
 * Par2PortAnalog();  converteert een set Par1 en Par2 met integer waarden naar Poort en Analoge waarde
 * PortAnalog2Par();  converteert een poort en analoge waarde naar Par1 en Par2
 \*********************************************************************************************/

unsigned long float2event(float f, byte port, byte cmd)
  {
  boolean high=false;
  boolean sign=false;
  long wi;

  if(f>=USER_VARIABLES_RANGE_MIN && f<= USER_VARIABLES_RANGE_MAX)
    {
    wi=int(f * 100);
    
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
           ((unsigned long)Settings.Unit)<<24      | 
           ((unsigned long)cmd)<<16                |
           ((unsigned long)port)<<12               |
           ((unsigned long)high)<<11               |
           ((unsigned long)sign)<<10               |
           ((unsigned long)(wi & 0x3ff));
    }
  else
    return 0L;  
  }
  
float EventPartFloat(unsigned long event)
  {
  float f;
  
  f=float(event&0x3ff);    // 10-bits waarde

  if(event & (1<<11)) // als high bit staat
    f*=10;

  f=f/100;
  
  if(event & (1<<10)) // als sign bit staat
    f=-f;

  
  return f;
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

 /*********************************************************************************************\
 * Bouw een Code op uit commando, data1 en data2
 \*********************************************************************************************/
unsigned long command2event(byte Unit, byte Command, byte Par1, byte Par2)
    {
    return ((unsigned long)SIGNAL_TYPE_NODO)<<28  | 
           ((unsigned long)Unit)<<24            | 
           ((unsigned long)Command)<<16           | 
           ((unsigned long)Par1)<<8               | 
            (unsigned long)Par2;
    }

#ifdef NODO_MEGA
 /*********************************************************************************************\
 * kopiëer de string van een commando naar een string[]
 \*********************************************************************************************/
char* cmd2str(int i)
  {
  static char string[80];

  if(i<=COMMAND_MAX)
    strcpy_P(string,(char*)pgm_read_word(&(CommandText_tabel[i])));

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
 * String mag HEX, DEC
 * Deze routine converteert uit een string een unsigned long waarde.
 * De string moet beginnen met het eerste teken(dus geen voorloop spaties).
 * bij ongeldige tekens in de string wordt een false terug gegeven.
 \*********************************************************************************************/
unsigned long str2int(char *string)
  {
  return(strtol(string,NULL,0));  
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
 * Converteert een unsigned long naar een string met decimale integer.
 \*********************************************************************************************/
char* int2str(unsigned long x)
  {
  static char OutputLine[12];
  char* OutputLinePosPtr=&OutputLine[10];
  int y;
  
  *OutputLinePosPtr=0;

  if(x==0)
    {
    *--OutputLinePosPtr='0';
    }
  else
    {  
    while(x>0)
      {
      *--OutputLinePosPtr='0'+(x%10);
      x/=10;
      }
    }    
  return OutputLinePosPtr;
  }

 /**********************************************************************************************\
 * Converteert een unsigned long naar een hexadecimale string.
 \*********************************************************************************************/
char* int2strhex(unsigned long x)
  {
  static char OutputLine[12];
  char* OutputLinePosPtr=&OutputLine[10];
  int y;
  
  *OutputLinePosPtr=0;

  if(x==0)
    {
    *--OutputLinePosPtr='0';
    }
  else
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
  

char* Float2str(float f)
  {
  static char rString[25];
  dtostrf(f, 0, 2, rString); // Kaboem... 2100 bytes programmacode extra !
  return rString;
  }


 /**********************************************************************************************\
 * vult een string met een regel uit de Eventlist.
 * geeft false terug als de regel leeg is
 * Let op dat er voldoende ruimte is in [Line]
 \*********************************************************************************************/
boolean EventlistEntry2str(int entry, byte d, char* Line, boolean Script)
  {
  unsigned long Event, Action;
  char *TempString=(char*)malloc(80);
  boolean Ok;

  
  Eventlist_Read(entry,&Event,&Action); // leesregel uit de Eventlist.    

  if(Event==0)
    {
    Ok=false;
    }
  else
    {
    // Geef de entry van de eventlist weer
    strcpy(Line,cmd2str(Script?CMD_EVENTLIST_WRITE:VALUE_SOURCE_EVENTLIST));
    
    if(!Script)
      {
      strcat(Line," ");
      strcat(Line,int2str(entry));
      }
  
    // geef het event weer
    strcat(Line,"; ");
    Event2str(Event, TempString);
    strcat(Line, TempString);
  
    // geef het action weer
    strcat(Line,"; ");
    Event2str(Action, TempString);  
    strcat(Line,TempString);
    Ok=true;
    }

  free(TempString);
  return Ok;
  }

/*******************************************************************************************
 *  floatToString.h
 *
 *  Usage: floatToString(buffer string, float value, precision, minimum text width)
 *
 *  Example:
 *  char test[20];    // string buffer
 *  float M;          // float variable to be converted
 *                 // precision -> number of decimal places
 *                 // min text width -> character output width, 0 = no right justify
 * 
 *  Serial.print(floatToString(test, M, 3, 7)); // call for conversion function
 *  
 * Thanks to capt.tagon / Orygun
 **********************************************************************************************/
 
 char * floatToString(char * outstr, double val, byte precision, byte widthp){
  char temp[16]; //increase this if you need more digits than 15
  byte i;

  temp[0]='\0';
  outstr[0]='\0';

  if(val < 0.0){
    strcpy(outstr,"-\0");  //print "-" sign
    val *= -1;
  }

  if( precision == 0) {
    strcat(outstr, ltoa(round(val),temp,10));  //prints the int part
  }
  else {
    unsigned long frac, mult = 1;
    byte padding = precision-1;
    
    while (precision--)
      mult *= 10;

    val += 0.5/(float)mult;      // compute rounding factor
    
    strcat(outstr, ltoa(floor(val),temp,10));  //prints the integer part without rounding
    strcat(outstr, ".\0"); // print the decimal point

    frac = (val - floor(val)) * mult;

    unsigned long frac1 = frac;

    while(frac1 /= 10) 
      padding--;

    while(padding--) 
      strcat(outstr,"0\0");    // print padding zeros

    strcat(outstr,ltoa(frac,temp,10));  // print fraction part
  }

  // generate width space padding 
  if ((widthp != 0)&&(widthp >= strlen(outstr))){
    byte J=0;
    J = widthp - strlen(outstr);

    for (i=0; i< J; i++) {
      temp[i] = ' ';
    }

    temp[i++] = '\0';
    strcat(temp,outstr);
    strcpy(outstr,temp);
  }

  return outstr;
}

#endif
