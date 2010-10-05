
 /*********************************************************************************************\
 * Print een event volgens formaat:  'EVENT/ACTION: <port>, <type>, <content>
 \*********************************************************************************************/
void PrintEvent(unsigned long Content, byte Port, byte Type, boolean Direction)
  {
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
    
  if(S.Trace)
    {
    // Type event
    if(Type)
      {
      if(Port)Serial.print(", ");    
      Serial.print(cmd2str(Type));
      }
    }

  if(Content)
    {
    if(Port | (Type && S.Trace))Serial.print(", ");
    PrintEventCode(Content,Type);
    }
  PrintTerm();
  }  


 /*********************************************************************************************\
 * print een lijst met de inhoud van de RawSignal buffer.
 \*********************************************************************************************/
void PrintRawSignal(void)
  {
  byte x;
  PrintEventCode(AnalyzeRawSignal(&x),0);
  Serial.print(Text(Text_30));
  for(int x=1;x<=RawSignal[0];x++)
     {
     if(x>1)Serial.print(",");
     Serial.print(RawSignal[x],DEC);
     }
  Serial.print(")");
  }


 /*********************************************************************************************\
 * Print een EventCode. 
 \*********************************************************************************************/
void PrintEventCode(unsigned long Code,byte Type)
  {
  byte NodoUnit=(Code>>24)&0xf;
  byte NodoHome=(Code>>28)&0xf;
  byte cmd=(Code>>16)&0xff;
  byte par1=(Code>>8)&0xff;
  byte par2=(Code)&0xff;

  if(Type==CMD_TYPE_UNKNOWN)
    {
    Serial.print("0x"); 
    Serial.print(Code,HEX);
    return;
    }

  if(NodoUnit!=S.Unit && NodoUnit!=0)
    {
    Serial.print("Unit-"); 
    Serial.print(NodoUnit,DEC); 
    Serial.print(", "); 
    }
    
  Serial.print("("); 
  Serial.print(cmd2str(cmd));
  Serial.print(" ");

  switch(cmd)
    {
    case CMD_KAKU:
    case CMD_SEND_KAKU:
      Serial.print('A'+((par1&0xf0)>>4),BYTE); //  A1..P1 printen...
      Serial.print(par2&0x2?0:(par1&0xF)+1,DEC);// als 2e bit in par2 staat, dan is het een groep commando en moet adres '0' zijn.
      Serial.print(",");
      Serial.print(cmd2str(par2 & 0x1)); // Print 'On' of 'Off'. laat cmd2str dit oplossen
      break;
    
    case CMD_KAKU_NEW:
    case CMD_SEND_KAKU_NEW:
      Serial.print(par1,DEC);
      Serial.print(",");
      if ((par2 & 0xF) == KAKU_DIMLEVEL)
        {// als dimlevel commando dan 'Dim' met daarna het hoogste nibble als getal printen
        Serial.print(Text(Text_05));
        Serial.print((par2 >> 4)+1,DEC);
        } 
      else
        Serial.print(cmd2str(par2 & 0x1)); // Print 'On' of 'Off'. laat cmd2str dit oplossen
      break;

    case CMD_WILDCARD_EVENT:
      if(par1!=0)
        Serial.print(cmd2str(par1));
      else
        Serial.print("0");
      Serial.print(","); 
      if(par2)
        Serial.print(cmd2str(par2));
      else
        Serial.print("0");
      break;

    case CMD_STATUS_EVENT:
      if(par1)
        Serial.print(cmd2str(par1));
      else
        Serial.print("0");
      Serial.print(","); 
      Serial.print(par2,DEC);
      break;

    default:
      Serial.print(par1,DEC);
      Serial.print(","); 
      Serial.print(par2,DEC);
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
  PrintEventCode(Event,EventType(Event));
  Serial.print("; ");
  PrintEventCode(Action,EventType(Action));
  PrintTerm();
  }
