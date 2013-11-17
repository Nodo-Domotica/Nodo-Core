//#######################################################################################################
//##################################### Misc: Debugging    ##############################################
//#######################################################################################################
void PrintNodoEvent(char* str, struct NodoEventStruct *Event)
  {    
  Serial.print(str);
  Serial.print(F(" => Port="));Serial.print(Event->Port);
  Serial.print(F(", Direction="));Serial.print(Event->Direction);
  Serial.print(F(", Flags="));Serial.print(Event->Flags);
  Serial.print(F(", DestinationUnit= "));Serial.print(Event->DestinationUnit);
  Serial.print(F(", SourceUnit="));Serial.print(Event->SourceUnit);
  Serial.print(F(", Type="));Serial.print(Event->Type);
  Serial.print(F(", Command="));Serial.print(cmd2str(Event->Command));
  Serial.print(F(", Par1="));Serial.print(Event->Par1);
  Serial.print(F(", Par2=0x"));Serial.print(Event->Par2,HEX);
  Serial.print(F(", Version=0x"));Serial.println(Event->Version);
  }

void PrintRawSignal(void)
  {    
  int x;
  
  Serial.print(F("*** Debug: RawSignal: Repeats="));Serial.print(RawSignal.Repeats);
  Serial.print(F(", Delay="));Serial.print(RawSignal.Delay);
  Serial.print(F(", Source="));Serial.print(cmd2str(RawSignal.Source));
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
 