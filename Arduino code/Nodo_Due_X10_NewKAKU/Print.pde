
 /*********************************************************************************************\
 * Print een event volgens formaat:  'EVENT/ACTION: <port>, <type>, <content>
 \*********************************************************************************************/
void PrintEvent(boolean Direction, byte Port, byte Type, unsigned long Content)
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
    
  if(Trace)
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
    if(Port | (Type && Trace))Serial.print(", ");
    PrintCode(Content);
    }
  }  


 /*********************************************************************************************\
 * print een lijst met de inhoud van de RawSignal buffer.
 \*********************************************************************************************/
void PrintRawSignal(void)
  {
  PrintCode(AnalyzeSignal());
  Serial.print(Text(Text_30));
  for(int x=1;x<=RawSignal[0];x++)
     {
     if(x>1)Serial.print(",");
     PrintCode(RawSignal[x]);
     }
  Serial.print(")");
  }


 /*********************************************************************************************\
 * Print een Event. 
 \*********************************************************************************************/
void PrintCode(unsigned long Code)
  {
  byte NodoUnit=(Code>>24)&0xf;
  byte NodoHome=(Code>>28)&0xf;
  byte cmd=(Code>>16)&0xff;
  byte par1=(Code>>8)&0xff;
  byte par2=(Code)&0xff;
  
  if(Code<=0xffff)
    Serial.print(Code,DEC);
  else
    {
    if(NodoHome==S.Home)
      {
      if(NodoUnit!=S.Unit)
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
        case CMD_X10:
        case CMD_KAKU_NEW:
          Serial.print('A'+((par1&0xf0)>>4),BYTE); // NewKAKU ook als A1..P1 printen...
//          if (cmd == CMD_KAKU_NEW) {
//            Serial.print(par1+1,DEC);
//          } else {
          Serial.print((par1&0xF)+1,DEC);
//          }
          Serial.print(",");
          Serial.print(cmd2str(par2 & 0xF)); // laat cmd2str het oplossen ipv hardcoded
          if ((par2 & 0xF) == CMD_DIMLEVEL) { Serial.print((par2 >> 4)+1,DEC); } // als dimlevel commando dan hoogste nibble als getal printen
//          if        ((par2 & 0xE) == CMD_DIMLEVEL) {
//                          Serial.print("Dim"); Serial.print(par2 >> 4);
//          } else if ((par2 & 0xE) == CMD_DIM) {
//            if (par2 & 1) Serial.print("Bright");
//            else          Serial.print("Dim");
//          } else if ((par2 & 0xE) == CMD_ALLOFF) {
//            if (par2 & 1) Serial.print("AllOn");
//            else          Serial.print("AllOff");
//          } else {
//            if (par2 & 1) Serial.print("On");
//            else          Serial.print("Off");
//          }
          break;

        case CMD_WILDCARD_EVENT:
          if(par1!=0)
            Serial.print(cmd2str(par1));
          else
            Serial.print("0");
          Serial.print(","); 
          if(par2!=0)
            Serial.print(cmd2str(par2));
          else
            Serial.print("0");
          break;                      

        case CMD_STATUS_EVENT:
          if(par1!=0)
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
    else
      {
      Serial.print("0x"); 
      Serial.print(Code,HEX);
      }
    }
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
    PrintCode(d);
    Serial.print(") ");
    }
  PrintCode(entry);
  Serial.print(": ");
  PrintCode(Event);
  Serial.print("; ");
  PrintCode(Action);
  PrintTerm();
  }
