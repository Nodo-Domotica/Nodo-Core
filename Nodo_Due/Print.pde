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
 * Print een event volgens formaat:  'EVENT/ACTION: <port>, <type>, <content>
 \*********************************************************************************************/
void PrintEvent(unsigned long Content, byte Port, boolean Direction)
  {
  byte Type=EventType(Content);

  if(S.Trace&2 && Time.Day) // Time.Day=true want dan is er een RTC aanwezig.
    {   
    PrintDateTime();
    PrintComma();
    }

  if(Direction)
    {
    Serial.print(cmd2str(Direction));
    Serial.print(": ");
    }

  if(Port)
    Serial.print(cmd2str(Port));
    
  if(S.Trace&1)
    {
    // Type event
    if(Type)
      {
      if(Port)PrintComma();   
      Serial.print(cmd2str(Type));
      }
    }

  if(Content)
    {
    if(Port | (Type && S.Trace&1))PrintComma();
    PrintEventCode(Content);
    }
  PrintTerm();
  }  


 /*********************************************************************************************\
 * print een lijst met de inhoud van de RawSignal buffer.
 \*********************************************************************************************/
void PrintRawSignal(void)
  {
  PrintText(Text_07,false);
  for(byte x=1;x<=RawSignal[0];x++)
     {
     if(x>1)PrintComma();
     Serial.print(RawSignal[x],DEC);
     }
  PrintChar(')');
  PrintTerm();
  }

 /*********************************************************************************************\
 * Serial.Print neemt veel progmem in beslag. 
 * Print een lijn met het teken '*' 
 \*********************************************************************************************/
void PrintLine(void)
  {
  for(byte x=1;x<=50;x++)Serial.print("*");
  PrintTerm();
  }

 /*********************************************************************************************\
 * Serial.Print neemt veel progmem in beslag. 
 * Print het teken ',' 
 \*********************************************************************************************/
void PrintComma(void)
  {
  Serial.print(", ");
  }


 /*********************************************************************************************\
 * Serial.Print neemt veel progmem in beslag. 
 * Print het teken '0' 
 \*********************************************************************************************/
void PrintChar(byte S)
  {
  Serial.print(S, BYTE);
  }


 /*********************************************************************************************\
 * Print een EventCode. de MMI van de Nodo
 * Een Evencode wordt weergegeven als "(<commando> <parameter-1>,<parameter-2>)"
 \*********************************************************************************************/
#define P_NOT   0
#define P_TEXT  1
#define P_KAKU  2
#define P_VALUE 3
#define P_DIM   4

void PrintEventCode(unsigned long Code)
  {
  byte P1,P2,Par2_b; 
  boolean P2Z=true;     // vlag: true=Par2 als nul waarde afdrukken false=nulwaarde weglaten
  
  byte Unit     = EventPart(Code,EVENT_PART_UNIT);
  byte Home     = EventPart(Code,EVENT_PART_HOME);
  byte Command  = EventPart(Code,EVENT_PART_COMMAND);
  byte Par1     = EventPart(Code,EVENT_PART_PAR1);
  byte Par2     = EventPart(Code,EVENT_PART_PAR2);
  byte Type     = EventType(Code);

  if(Type==VALUE_TYPE_UNKNOWN)
    {
    Serial.print("0x"); 
    Serial.print(Code,HEX);
    return;
    }

  if(Unit!=S.Unit && Unit!=0)
    {
    PrintText(Text_08,false); 
    Serial.print(Unit,DEC); 
    PrintComma();
    }
    
  Serial.print("("); 
  Serial.print(cmd2str(Command));

  switch(Command)
    {
    case CMD_KAKU:
    case CMD_SEND_KAKU:
      P1=P_KAKU;
      P2=P_TEXT;
      Par2_b=Par2;
      Par2&=1;
      break;

    case CMD_KAKU_NEW:
    case CMD_SEND_KAKU_NEW:
      P1=P_VALUE;
      P2=P_DIM;
      break;

    // Par1 als waarde en par2 als tekst
    case CMD_WIRED_PULLUP:
    case CMD_WIRED_OUT:
    case CMD_WIRED_IN_EVENT:
      P1=P_VALUE;
      P2=P_TEXT;
      break;

    // Par1 als tekst en par2 als tekst
    case CMD_OK:
    case CMD_TRACE:
    case CMD_COMMAND_WILDCARD:
      P1=P_TEXT;
      P2=P_TEXT;
      break;

    // Par1 als tekst en par2 als getal
    case CMD_ERROR:
    case CMD_COPYSIGNAL:
    case CMD_WAITFREERF:
    case CMD_SEND_STATUS:
    case CMD_STATUS:
      P1=P_TEXT;
      P2=P_VALUE;
      break;

    // Par1 als tekst en par2 niet
    case CMD_DLS_EVENT:
    case CMD_TRANSMIT_SETTINGS:
    case CMD_SIMULATE:
      P1=P_TEXT;
      P2=P_NOT;
      break;

    // Par1 als waarde en par2 niet
    case CMD_DIVERT:
    case CMD_VARIABLE_CLEAR:
    case CMD_SIMULATE_DAY:
    case CMD_TIMER_RESET:
    case CMD_CLOCK_DOW:
    case CMD_DELAY:
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
  if(P1!=P_NOT)PrintChar(' ');
  
  switch(P1)
    {
    case P_TEXT:
      Serial.print(cmd2str(Par1));
      break;
    case P_VALUE:
      Serial.print(Par1,DEC);
      break;
    case P_KAKU:
      Serial.print('A'+((Par1&0xf0)>>4),BYTE); //  A..P printen.
      Serial.print(Par2_b&0x2?0:(Par1&0xF)+1,DEC);// als 2e bit in Par2 staat, dan is het een groep commando en moet adres '0' zijn.
      break;
    default:
      PrintChar(' ');
    }

  // Print Par2    
  if(P2!=P_NOT)PrintComma();
  switch(P2)
    {
    case P_TEXT:
      Serial.print(cmd2str(Par2));
      break;
    case P_VALUE:
      Serial.print(Par2,DEC);
      break;
    case P_DIM:
      {
      if ((Par2 & 0xF) == KAKU_DIMLEVEL)
        {// als dimlevel commando dan 'Dim' met daarna het hoogste nibble als getal printen
        PrintText(Text_05,false);
        Serial.print((Par2 >> 4)+1,DEC);
        } 
      else
        Serial.print(cmd2str(Par2 & 0x1)); // Print 'On' of 'Off'. laat cmd2str dit oplossen
      break;
      }
    }

  PrintChar(')'); 
  }

     
 /**********************************************************************************************\
 * Verzend teken(s) naar de seriële poort die een regel afsluiten.
 \*********************************************************************************************/
void PrintTerm()
  {
  if(SERIAL_TERMINATOR_1)Serial.print(SERIAL_TERMINATOR_1,BYTE);
  if(SERIAL_TERMINATOR_2)Serial.print(SERIAL_TERMINATOR_2,BYTE);
  }


  
 /**********************************************************************************************\
 * Print een regel uit de Eventlist.
 \*********************************************************************************************/
void PrintEventlistEntry(int entry, byte d)
  {
  unsigned long Event, Action;

  Eventlist_Read(entry,&Event,&Action); // leesregel uit de Eventlist.    
  Serial.print(cmd2str(VALUE_SOURCE_EVENTLIST));
  Serial.print(" ");

  if(d>1)
    {
    Serial.print(d,DEC);
    PrintChar('.');
    }
  Serial.print(entry,DEC);
  Serial.print(": ");
  PrintEventCode(Event);
  Serial.print("; ");
  PrintEventCode(Action);
  PrintTerm();
  }
  
 /**********************************************************************************************\
 * Print actuele dag, datum, tijd.
 \*********************************************************************************************/
void PrintDateTime(void)
    {
    // Print de dag. 1=zondag, 0=geen RTC aanwezig
    for(byte x=0;x<=2;x++)Serial.print(*(Text(Text_02)+(Time.Day-1)*3+x),BYTE);
    Serial.print(" ");

    // print datum.    
    if(Time.Date<10)PrintChar('0');
    Serial.print(Time.Date,DEC);

    PrintChar('-');
    
    // print maand.    
    if(Time.Month<10)PrintChar('0');
    Serial.print(Time.Month,DEC);

    PrintChar('-');

    // print year.    
    Serial.print(Time.Year,DEC);
    PrintComma();
    
    // print uren.    
    if(Time.Hour<10)PrintChar('0');
    Serial.print(Time.Hour,DEC);

    PrintChar(':');

    // print minuten.
    if(Time.Minutes<10)PrintChar('0');
    Serial.print(Time.Minutes,DEC);
    }

 /**********************************************************************************************\
 * Print de welkomsttekst van de Nodo.
 \*********************************************************************************************/
void PrintWelcome(void)
  {
  // Print Welkomsttekst
  PrintTerm();
  PrintLine();
  PrintText(Text_01,false);

  Serial.print(S.Version/100,DEC);
  PrintChar('.');
  Serial.print(S.Version%100,DEC);  
  PrintText(Text_03,false);
  Serial.print(S.Home,DEC);
  PrintText(Text_14,false);
  Serial.print(S.Unit,DEC);PrintTerm();
  if(Time.Day)
    {
    PrintDateTime();
    if(Time.DaylightSaving)
      {
      PrintComma();
      Serial.print(cmd2str(CMD_DLS_EVENT));
      }
    PrintTerm();
    }
  PrintLine();
  }

 /**********************************************************************************************\
 * Print een string uit PROGMEM
 \*********************************************************************************************/
void PrintText(prog_char* text,boolean Term)
  {
  byte x=0;
  char buffer[40];

  do
    {
    buffer[x]=pgm_read_byte_near(text+x);
    }while(buffer[x++]!=0);

  Serial.print(buffer);
  if(Term)
    PrintTerm();
  }
