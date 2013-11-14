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
  Serial.print(F(", Command="));Serial.print(Event->Command);
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


#define LONGH  1060
#define SHORTH  310
#define LONGL   970
#define SHORTL  180

void Test(void)     //???                                                                
  {
  int x;

  Serial.println("Test: Verzend iedere twee seconden een rawsignal");
  
  noInterrupts();//???

  while(true)
    {
    Led(RED);
    digitalWrite(PIN_RF_RX_VCC,LOW);   // Spanning naar de RF ontvanger uit om interferentie met de zender te voorkomen.
    digitalWrite(PIN_RF_TX_VCC,HIGH); // zet de 433Mhz zender aan
    delay(5);// kleine pause om de zender de tijd te geven om stabiel te worden 
    
    for(int frames=0; frames<10;frames++)
      {
      for(int pulses=0; pulses<20; pulses++) // herhaal verzenden RF code
        {
        digitalWrite(PIN_RF_TX_DATA,HIGH);
        delayMicroseconds(LONGH);  
        digitalWrite(PIN_RF_TX_DATA,LOW);
        delayMicroseconds(SHORTL);  
    
        digitalWrite(PIN_RF_TX_DATA,HIGH);
        delayMicroseconds(LONGH);  
        digitalWrite(PIN_RF_TX_DATA,LOW);
        delayMicroseconds(SHORTL);  
    
        digitalWrite(PIN_RF_TX_DATA,HIGH);
        delayMicroseconds(SHORTH);  
        digitalWrite(PIN_RF_TX_DATA,LOW);
        delayMicroseconds(SHORTL);  
    
        digitalWrite(PIN_RF_TX_DATA,HIGH);
        delayMicroseconds(SHORTH);  
        digitalWrite(PIN_RF_TX_DATA,LOW);
        delayMicroseconds(SHORTL);  
    
        digitalWrite(PIN_RF_TX_DATA,HIGH);
        delayMicroseconds(SHORTH);  
        digitalWrite(PIN_RF_TX_DATA,LOW);
        delayMicroseconds(LONGL);  
        }
      delayMicroseconds(10000UL);  
      }
    digitalWrite(PIN_RF_TX_VCC,LOW); // zet de 433Mhz zender weer uit
    digitalWrite(PIN_RF_RX_VCC,HIGH); // Spanning naar de RF ontvanger weer aan.
    Led(GREEN);
    delayMicroseconds(2000000UL);  

    }      
  }
  
  
  
  
  
  
  