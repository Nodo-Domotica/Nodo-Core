//#######################################################################################################
//#####################################      Debug routines     #########################################
//#######################################################################################################
void PrintNodoEvent(char* str, struct NodoEventStruct *Event)
  {    
  Serial.print(str);
  Serial.print(F(", Type="));             Serial.print(Event->Type);
  Serial.print(F(", Command="));          Serial.print(Event->Command);
  Serial.print(F(", Par1="));             Serial.print(Event->Par1);
  Serial.print(F(", Par2=0x"));           Serial.print(Event->Par2,HEX);
  Serial.print(F(", Payload="));          Serial.print(Event->Payload);
  Serial.print(F(", Port="));             Serial.print(Event->Port);
  Serial.print(F(", Direction="));        Serial.print(Event->Direction);
  Serial.print(F(", Flags="));            Serial.print(Event->Flags,BIN);
  Serial.print(F(", DestinationUnit= ")); Serial.print(Event->DestinationUnit);
  Serial.print(F(", SourceUnit="));       Serial.println(Event->SourceUnit);
  }

#if HARDWARE_RAWSIGNAL
void PrintRawSignal(void)
  {    
  int x;
  
  Serial.print(F("*** Debug: RawSignal: Repeats="));Serial.print(RawSignal.Repeats);
  Serial.print(F(", Delay="));Serial.print(RawSignal.Delay);
  #if NODO_MEGA
  Serial.print(F(", Source="));Serial.print(cmd2str(RawSignal.Source));
  #else if
  Serial.print(F(", Source="));Serial.print(RawSignal.Source);
  #endif
  Serial.print(F(", Multiply="));Serial.print(RawSignal.Multiply);
  Serial.print(F(", Number="));Serial.print(RawSignal.Number);
  Serial.print(F(", Pulses (uSec): "));
  
  for(x=1;x<=RawSignal.Number;x++)
    {
    Serial.print(RawSignal.Pulses[x]*RawSignal.Multiply); 
    Serial.write(',');       
    }
  Serial.println();
  }
#endif 

void PrintTransmissionStruct(struct TransmissionStruct *T)//???
  {
  byte *b=(byte*)T;
  
  Serial.print(F("TransmissionStruct="));
  for(int x=0;x<sizeof(struct TransmissionStruct); x++)
    {
    Serial.print( *b,DEC  );
    Serial.print(',');
    b++;
    }
  Serial.println();
  }
  
  
void Trace(char* label, unsigned long value)
  {
  static unsigned long previous=0;
  unsigned long now=millis();
  
  Serial.print(F("Trace: "));
  Serial.print(label);
  if(value<=0xffff)
    {
    Serial.print(value);
    }
  else
    {
    Serial.print(F("0x"));
    Serial.print(value,HEX);
    }
  Serial.print(F(", (dTime="));
  Serial.print(now-previous);
  Serial.println(F(")"));
  previous=millis();
  }

