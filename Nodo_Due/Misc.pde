/**************************************************************************\

    This file is part of Nodo Due, © Copyright Paul Tonkes

    Nodo Due is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Nodo Due is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Nodo Due.  If not, see <http://www.gnu.org/licenses/>.

\**************************************************************************/

 /*********************************************************************************************\
 * kopiëer de string van een commando naar een string[]
 \*********************************************************************************************/
char* cmd2str(int i)
  {
  static char string[20];
  if(i<COMMAND_MAX)
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
 * Bouw een Code op uit commando, data1 en data2
 \*********************************************************************************************/
unsigned long command2event(int Command, byte Par1, byte Par2)
    {
    return ((unsigned long)S.Home)<<28  | 
           ((unsigned long)S.Unit)<<24  | 
           ((unsigned long)Command)<<16 | 
           ((unsigned long)Par1)<<8     | 
            (unsigned long)Par2;
    }

/*********************************************************************************************\
 * String mag HEX, DEC of een NODO commando zijn.
 * Deze routine coverteert uit een string een unsigned long waarde.
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
      *Par1=S.WaitFreeRFAction;
      *Par2=S.WaitFreeRFWindow/100;
      break;
  
    case CMD_UNIT: 
      *Par1=S.Unit;
      *Par2=S.Home;
      break;        

    case CMD_CONFIRM: 
      *Par1=S.Confirm;
      break;        

    case CMD_DLS_EVENT:
      *Par1=S.DaylightSaving;
      break;
      
    case CMD_SIMULATE:
      *Par1=Simulate;
      break;
            
    case CMD_ANALYSE_SETTINGS:
      *Par1=S.AnalyseTimeOut/1000;
      *Par2=S.AnalyseSharpness;
      break;

    case CMD_TRANSMIT_SETTINGS:
      *Par1=S.TransmitPort;
      break;

    case CMD_CLOCK_EVENT_DAYLIGHT:
      *Par1=Time.Daylight;
      break;

    case CMD_VARIABLE_SET:
      *Par1=xPar1;
      *Par2=S.UserVar[xPar1-1];
      break;

    case CMD_TRACE:
      *Par1=S.Trace&1;
      *Par2=S.Trace&2>0;      
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

    case CMD_TIMER_SET:
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
      *Par2=S.WiredInputPullUp[xPar1-1];
      break;

    case CMD_WIRED_SMITTTRIGGER:
      *Par1=xPar1;
      *Par2=S.WiredInputSmittTrigger[xPar1-1];
      break;

    case CMD_WIRED_IN_EVENT:
      *Par1=xPar1;
      *Par2=WiredInputStatus[xPar1-1];
      break;

    case CMD_WIRED_ANALOG:
      *Par1=xPar1;
      *Par2=WiredAnalog(xPar1-1);
      break;
      
    case CMD_WIRED_OUT:
      *Par1=xPar1;
      *Par2=WiredOutputStatus[xPar1-1];
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
 char* Text(prog_char* text)
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
  char ByteToSave,*pointerToByteToSave=pointerToByteToSave=(char*)&S;    //pointer verwijst nu naar startadres van de struct. Ge-cast naar char omdat EEPROMWrite per byte wegschrijft
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
  char ByteToSave,*pointerToByteToRead=(char*)&S;    //pointer verwijst nu naar startadres van de struct.
  for(int x=0; x<sizeof(struct Settings);x++)
    {
    *pointerToByteToRead=EEPROM.read(x);
    pointerToByteToRead++;// volgende byte uit de struct
    }
  }
 
  
 /*********************************************************************************************\
 * Alle settings van de Nodo weer op default.
 \*********************************************************************************************/
void ResetFactory(void)
  {
  Beep(2000,2000);
  ClockRead();
  
  S.Version            = VERSION;
  S.Unit               = UNIT;
  S.Home               = HOME;
  S.Trace              = 0;
  S.AnalyseSharpness   = 50;
  S.AnalyseTimeOut     = SIGNAL_TIMEOUT_IR;
  S.TransmitPort       = VALUE_SOURCE_IR_RF;
  S.WaitFreeRFAction   = VALUE_OFF;
  S.WaitFreeRFWindow   = S.Unit*500;
  S.DaylightSaving     = Time.DaylightSaving;
  S.Confirm            = false;
  
  for(byte x=0;x<4;x++)
    {
    S.WiredInputThreshold[x]=0x80; 
    S.WiredInputSmittTrigger[x]=5;
    S.WiredInputRange[x]=0;
    S.WiredInputPullUp[x]=true;
    }

  VariableClear(0); // alle variabelen op nul zetten
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
  Eventlist_Write(0,command2event(CMD_COMMAND_WILDCARD,VALUE_SOURCE_IR,CMD_KAKU),command2event(CMD_SEND_SIGNAL,0,0)); // Kort geluidssignaal bij ieder binnenkomend event
  }

 /*********************************************************************************************\
 * Wist de inhoud van een variabele zonder een event te genereren.
 * als opgegeven variabele gelijk is aan 0, dan worden alle gewist
 * geeft true terug bij succes.
 * LET OP: Voor de user is variabele 1 de eeste. Intern wordt getelt vanaf 0 (S.UserVar[0])
 \*********************************************************************************************/
boolean VariableClear(byte Variable)
  {
  if(Variable==0)
    {
    for(byte x=0;x<USER_VARIABLES_MAX;x++)
      {
      S.UserVar[x]=0;
      UserVarPrevious[x]=0;
      }
    SaveSettings();
    return true;
    }

  if(Variable>0 && Variable<=USER_VARIABLES_MAX)
    {
    S.UserVar[Variable-1]=0;
    UserVarPrevious[Variable-1]=0;
    SaveSettings();
    return true;
    }
  return false;
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
      {
      return (Home<<4) | (Address-1);        
      }
    }
  else // absoluut adres [0..255]
    return Address; // KAKU adres 1 is intern 0     
  }

/**********************************************************************************************\
 * Converteert een string volgens formaat "Dim<level>" naar een parameter code
 \*********************************************************************************************/
byte DL2par(char* DL)
{
  byte c;   // teken uit de string die behandeld wordt
  byte x=3, par=0;
  
  if (DL[0] != 'd' || DL[1] != 'i' || DL[2] != 'm' || DL[3] == 0) { return 0; }
  while((c=DL[x++])!=0) {
    if(c>='0' && c<='9') { par = par*10; par = par + c - '0'; }
  }
  return ((par-1) << 4 | KAKU_DIMLEVEL);      
}


 /**********************************************************************************************\
 * Bepaal wat voor een type Event het is.
 \*********************************************************************************************/
byte EventType(unsigned long Code)
  {
  byte Unit=(Code>>24)&0xf;
  byte Command=(Code>>16)&0xff;
  
  if(((Code>>28)&0xf)!=S.Home)                return VALUE_TYPE_UNKNOWN; 
  if(Unit!=S.Unit && Unit!=0)                 return VALUE_TYPE_OTHERUNIT; // andere unit, dus niet voor deze nodo bestemd. Behalve unit=0, die is voor alle units    }
  if(Command<=RANGE_VALUE)                    return VALUE_TYPE_UNKNOWN;
  if(Command<RANGE_EVENT)                     return VALUE_TYPE_COMMAND;
  if(Command<=COMMAND_MAX)                    return VALUE_TYPE_EVENT;
  /* in andere gevallen */                    return VALUE_TYPE_UNKNOWN;  
  }
    
 /**********************************************************************************************\
 * Set de timer op nul zonder dat er een event wordt gegenereerd.
 \*********************************************************************************************/
void TimerClear(byte TimerToReset)
  {
  if(TimerToReset==0)
    for(int x=0;x<TIMER_MAX;x++)
      UserTimer[x]=0;
  else
    UserTimer[TimerToReset-1]=0;
  }
  
// this function will return the number of bytes currently free in RAM
//int MemoryTest() 
//  {
//  int byteCounter = 0; // initialize a counter
//  byte *byteArray; // create a pointer to a byte array
//  while ( (byteArray = (byte*) malloc (byteCounter * sizeof(byte))) != NULL ) {
//    byteCounter++; // if allocation was successful, then up the count for the next try
//    free(byteArray); // free memory after allocating it
//  }  
//  free(byteArray); // also free memory after the function finishes
//  return byteCounter; // send back the highest number of bytes successfully allocated
// }

 /**********************************************************************************************\
 * Geeft de analoge waarde [0..255] van de WIRED-IN poort <WiredPort> [0..3]
 \*********************************************************************************************/
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
  
