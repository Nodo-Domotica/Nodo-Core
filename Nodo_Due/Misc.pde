
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
boolean GetStatus(int *Command, int *Par1, int *Par2)
 { 
  byte x;
  int xPar1=*Par1,xPar2=*Par2;
  *Par1=0;
  *Par2=0;
  switch (*Command)
    {
    //??? divertsettings opnemen
  
    case CMD_UNIT: 
      *Par1=S.Unit;
      break;
        
    case CMD_HOME: 
      *Par1=S.Home;
      break;
      
    case CMD_SIMULATE:
      *Par1=Simulate;
      break;
            
    case CMD_ANALYSE_SETTINGS:
      *Par1=S.AnalyseTimeOut/1000;
      *Par2=S.AnalyseSharpness;
      break;

    case CMD_DIVERT_SETTINGS:
      *Par1=S.DivertType;
      *Par2=S.DivertPort;
      break;

    case CMD_CLOCK_EVENT_DAYLIGHT:
      *Par1=Time.Daylight;
      break;

    case CMD_CLOCK_DLS:
      *Par1=S.DaylightSaving;
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
      *Par2=analogRead(WiredAnalogInputPin_1+xPar1-1)>>2;
      break;

    case CMD_WAITFREERF:
      *Par1=S.WaitForFreeRF;
      *Par2=S.WaitForFreeRF_Time;
      break;
      
    case CMD_WIRED_OUT:
      *Par1=xPar1;
      *Par2=WiredOutputStatus[xPar1];
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
  
  S.DaylightSaving     = false;
  S.Version            = VERSION;
  S.Unit               = UNIT;
  S.Home               = HOME;
  S.Trace              = 0;
  S.AnalyseSharpness   = 50;
  S.AnalyseTimeOut     = 10000;
  S.WaitForFreeRF      = 0;
  S.WaitForFreeRF_Time = 0;
  S.DivertPort         = DIVERT_PORT_IR_RF;
  S.DivertType         = DIVERT_TYPE_USEREVENT;
    
  for(byte x=0;x<4;x++)
    {
    S.WiredInputThreshold[x]=0x80; 
    S.WiredInputSmittTrigger[x]=5;
    S.WiredInputPullUp[x]=true;
    }

  VariableClear(0); // alle variabelen op nul zetten
  SaveSettings();  
  FactoryEventlist();
  Reset();
  }
  
 /**********************************************************************************************\
 * Maak de Eventlist leeg en herstel de default inhoud
 \*********************************************************************************************/
void FactoryEventlist(void)
  {
  Eventlist_Write(1,0L,0L); // maak de eventlist leeg.
  Eventlist_Write(0,command2event(CMD_BOOT_EVENT,0,0),command2event(CMD_SOUND,7,0)); // geluidssignaal na opstarten Nodo
  Eventlist_Write(0,command2event(CMD_WILDCARD_EVENT,0,0),command2event(CMD_SOUND,0,0)); // Kort geluidssignaal bij ieder binnenkomend event
  }

 /*********************************************************************************************\
 * Wist de inhoud van een variabele zonder een event te genereren.
 * als opgegeven variabele gelijk is aan 0, dan worden alle gewist
 * geeft true terug bij succes.
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
    S.UserVar[Variable]=0;
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

 /*********************************************************************************************\
 * geeft een deel uit de code terug.
 * er wordt geen rekening gehouden met juiste home of unit adressering.
 \*********************************************************************************************/
byte EventPart(unsigned long Code, byte Part)
  {
  int x;

  switch(Part)
    {
    case EVENT_PART_HOME:
      return (Code>>28)&0xf;

    case EVENT_PART_UNIT:
      return (Code>>24)&0xf;

    case EVENT_PART_COMMAND:
      return (Code>>16)&0xff;
      
    case EVENT_PART_PAR1:
      return (Code>>8)&0xff;

    case EVENT_PART_PAR2:
      return Code&0xff;

    default:
      return 0;
    }
  }

 /**********************************************************************************************\
 * Bepaal wat voor een type Event het is.
 \*********************************************************************************************/
byte EventType(unsigned long Code)
  {
  byte Command,Unit;
  
  Unit=   EventPart(Code,EVENT_PART_UNIT);
  Command=EventPart(Code,EVENT_PART_COMMAND);
  
  if(EventPart(Code,EVENT_PART_HOME)!=S.Home) return CMD_TYPE_UNKNOWN; 
  if(Unit!=S.Unit && Unit!=0)                 return CMD_TYPE_OTHERUNIT; // andere unit, dus niet voor deze nodo bestemd. Behalve unit=0, die is voor alle units    }
  if(Command<=RANGE_VALUE)                    return CMD_TYPE_UNKNOWN;
  if(Command<RANGE_EVENT)                     return CMD_TYPE_COMMAND;
  if(Command<=COMMAND_MAX)                    return CMD_TYPE_EVENT;
  /* in andere gevallen */                    return CMD_TYPE_UNKNOWN;  
  }
    

  
