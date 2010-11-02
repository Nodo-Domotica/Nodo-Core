
 /*********************************************************************************************\
 * Print een event volgens formaat:  'EVENT/ACTION: <port>, <type>, <content>
 \*********************************************************************************************/
void PrintEvent(unsigned long Content, byte Port, boolean Direction)
  {
  byte Type=EventType(Content);

  if(S.Trace&2 && Time.Day) // Time.Day=true want dan is er een RTC aanwezig
    {   
    // Print de dag. 1=zondag, 0=geen RTC aanwezig
    for(byte x=0;x<=2;x++)Serial.print(*(Text(Text_02)+(Time.Day-1)*3+x),BYTE);
    Serial.print(" ");

    // print uren.    
    if(Time.Hour<10)Serial.print("0");
    Serial.print(Time.Hour,DEC);

    Serial.print(":");

    // print minuten.
    if(Time.Minutes<10)Serial.print("0");
    Serial.print(Time.Minutes,DEC);
    PrintComma();
    }

  switch(Direction)
    {
    case DIRECTION_IN:
       Serial.print(Text(Text_10));
       break;
    case DIRECTION_OUT:
       Serial.print(Text(Text_11));
       break;
    case DIRECTION_INTERNAL:
       Serial.print(Text(Text_12));
       break;
    case DIRECTION_EXECUTE:
       Serial.print(Text(Text_13));
       break;
    default:
       break;   
    }

  // Poort
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
  byte x;
  PrintEventCode(AnalyzeRawSignal());
  Serial.print(Text(Text_30));
  for(int x=1;x<=RawSignal[0];x++)
     {
     if(x>1)PrintComma();
     Serial.print(RawSignal[x],DEC);
     }
  Serial.print(")");
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
 * Print een EventCode. 
 \*********************************************************************************************/
void PrintEventCode(unsigned long Code)
  {
  byte Unit     = EventPart(Code,EVENT_PART_UNIT);
  byte Home     = EventPart(Code,EVENT_PART_HOME);
  byte Command  = EventPart(Code,EVENT_PART_COMMAND);
  byte Par1     = EventPart(Code,EVENT_PART_PAR1);
  byte Par2     = EventPart(Code,EVENT_PART_PAR2);
  byte Type     = EventType(Code);

  if(Type==CMD_TYPE_UNKNOWN)
    {
    Serial.print("0x"); 
    Serial.print(Code,HEX);
    return;
    }

  if(Unit!=S.Unit && Unit!=0)
    {
    Serial.print("Unit-"); 
    Serial.print(Unit,DEC); 
    PrintComma();
    }
    
  Serial.print("("); 
  Serial.print(cmd2str(Command));
  Serial.print(" ");

  switch(Command)
    {
    case CMD_KAKU:
    case CMD_SEND_KAKU:
      Serial.print('A'+((Par1&0xf0)>>4),BYTE); //  A1..P1 printen...
      Serial.print(Par2&0x2?0:(Par1&0xF)+1,DEC);// als 2e bit in Par2 staat, dan is het een groep commando en moet adres '0' zijn.
      PrintComma();
      Serial.print(cmd2str(Par2 & 0x1)); // Print 'On' of 'Off'. laat cmd2str dit oplossen
      break;
    
    case CMD_KAKU_NEW:
    case CMD_SEND_KAKU_NEW:
      Serial.print(Par1,DEC);
      PrintComma();
      if ((Par2 & 0xF) == KAKU_DIMLEVEL)
        {// als dimlevel commando dan 'Dim' met daarna het hoogste nibble als getal printen
        Serial.print(Text(Text_05));
        Serial.print((Par2 >> 4)+1,DEC);
        } 
      else
        Serial.print(cmd2str(Par2 & 0x1)); // Print 'On' of 'Off'. laat cmd2str dit oplossen
      break;

    case CMD_WILDCARD_EVENT:
      if(Par1!=0)
        Serial.print(cmd2str(Par1));
      else
        Serial.print("0");
      PrintComma();
      if(Par2)
        Serial.print(cmd2str(Par2));
      else
        Serial.print("0");
      break;

    case CMD_SEND_STATUS:
      if(Par1)
        Serial.print(cmd2str(Par1));
      else
        Serial.print("0");
      PrintComma(); 
      Serial.print(Par2,DEC);
      break;

    default:
      Serial.print(Par1,DEC);
      PrintComma();
      Serial.print(Par2,DEC);
    }
  Serial.print(")"); 
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
  Serial.print(Text(Text_15));

  if(d>1)
    {
    Serial.print("(");
    Serial.print(d,DEC);
    Serial.print(") ");
    }
  Serial.print(entry,DEC);
  Serial.print(": ");
  PrintEventCode(Event);
  Serial.print("; ");
  PrintEventCode(Action);
  PrintTerm();
  }
