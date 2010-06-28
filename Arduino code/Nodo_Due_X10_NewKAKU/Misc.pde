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
 * kopiëer de string can een commando naar een string[]
 \*********************************************************************************************/
char* cmd2str(int i)
  {
  static char string[20];
  if(i<COMMAND_MAX)
    strcpy_P(string,(char*)pgm_read_word(&(CommandText_tabel[i])));
  else
    string[0]=0;// voor als er geen gevonden wordt, dan is de string ook leeg
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
    unsigned long Code;
    return ((unsigned long)S.Home)<<28 | ((unsigned long)S.Unit)<<24 | ((unsigned long)Command)<<16 | ((unsigned long)Par1)<<8 | (unsigned long)Par2;
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
    case CMD_UNIT: 
      *Par1=S.Unit;
      break;
      
    case CMD_HOME: 
      *Par1=S.Home;
      break;
      
    case CMD_BUILD: 
      *Par1=BUILD;
      break;
      
    case CMD_SIMULATE:
      *Par1=Simulate;
      break;
      
    case CMD_CLOCK_EVENT_DAYLIGHT:
      *Par1=Time.Daylight;
      break;

    case CMD_CLOCK_DLS:
      *Par1=S.DaylightSaving;
      break;

    case CMD_RECEIVE_REPEATS_RF:
      *Par1=S.RFRepeatChecksum;
      *Par2=S.RFRepeatSuppress;
      break;

    case CMD_RECEIVE_REPEATS_IR:
      *Par1=S.IRRepeatChecksum;
      *Par2=S.IRRepeatSuppress;
      break;

    case CMD_VARIABLE_SET:
      *Par1=xPar1;
      *Par2=S.UserVar[xPar1-1];
      break;

    case CMD_TRACE:
      *Par1=Trace;
      break;

    case CMD_CLOCK_DATE:
      *Par1=Time.Date;
      *Par2=Time.Month;
      break;

    case CMD_CLOCK_TIME:
      *Par1=Time.Hour;
      *Par2=Time.Minutes;
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

    
 /**********************************************************************************************\
 * 
 \*********************************************************************************************/
byte EventType(unsigned long Code)
  {
  byte x;

  if(((Code>>28)&0xf)!=S.Home)return CMD_TYPE_UNKNOWN; // ander home adres, dus het is niet voor deze Nodo bestemd.
  if(((Code>>24)&0xf)!=S.Unit)return CMD_TYPE_OTHERUNIT; // andere unit, dus niet voor deze nodo bestemd.
  
  x=(Code>>16)&0xff; // 8-bits commando
  
  if(x>=RANGE_EVENT)
    {
    return CMD_TYPE_EVENT;
    }
  else
    {
    switch(x)
      {
      case 0:
         return CMD_TYPE_UNKNOWN;
      case CMD_KAKU:
         return CMD_KAKU;
      case CMD_KAKU_NEW:
         return CMD_KAKU_NEW;
      case CMD_X10:
         return CMD_X10;
      default:
        return CMD_TYPE_COMMAND;
      }
    }
  }
    
    
 /*********************************************************************************************\
 * Toetst of een Code een uitvoerbaar commando of een nodo event is. 
 * Geen controle of het een geldig commando is.
 \*********************************************************************************************/
int IsCommand(unsigned long Code)
  {
  if(((Code>>24)&0xff)==(S.Home<<4 | S.Unit))
    return((Code>>16)&0xff);
  else
    return false;
  }
  
 /*********************************************************************************************\
 * Alle settings van de Nodo weer op default.
 \*********************************************************************************************/
void ResetFactory(void)
  {
  Beep(2000,2000);
  
  S.DaylightSaving     =false;
  S.Build              =BUILD;
  S.RFRepeatChecksum   =true;
  S.RFRepeatSuppress   =true;
  S.IRRepeatChecksum   =true;
  S.IRRepeatSuppress   =true;
  S.Unit               =UNIT;
  S.Home               =HOME;
  
  for(byte x=0;x<4;x++)
    {
    S.WiredInputThreshold[x]=0x80; 
    S.WiredInputSmittTrigger[x]=5;
    S.WiredInputPullUp[x]=true;
    }

  FactoryEventlist();
  VariableClear(0); // alle variabelen op nul zetten
  SaveSettings();  
  Reset();
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
byte HA2address(char* HA)
  {
  byte x=0,y=false; // teller die wijst naar het het te behandelen teken
  byte c;   // teken uit de string die behandeld wordt
  byte Home=0,Address=0;
 
  while((c=HA[x++])!=0)
    {
    if(c>='0' && c<='9'){Address=Address*10;Address=Address+c-'0';}// KAKU adres 1 is intern 0
    if(c>='a' && c<='p'){Home=c-'a';y=true;} // KAKU home A is intern 0
    }

  if(y)// notatie [A1..P16] 
    return (Home<<4) | (Address-1);
  else // absoluut adres [0..255]
    return Address-1;      
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
  return ((par-1) << 4 | CMD_DIMLEVEL);      
}

