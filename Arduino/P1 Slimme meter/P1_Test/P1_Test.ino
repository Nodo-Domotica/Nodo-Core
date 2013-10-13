// 
// Eenvoudig testprogramma om de verbinding met een P1 slimme meter te bekijken/testen. 
//
// ZORG ER VOOR DAT =EERST= DEZE CODE WORDT GEUPLOAD NAAR DE ARDUINO EN PAS DAARNA DE AANSLUITINGEN MET DE SLIMME METER WORDEN GEMAAKT !!!
// aansluitingen weer verwijderen voordat andere code wordt geupload.
//
// We gaan uit van de volgende configuratie:
// 
// 1. Een Arduino Mega ZONDER aanvullende hardware aangesloten
// 2. De Trigger / RTS lijn van de slimme meter is verbonden met D13 (Komt overeen met deLed op de Arduino)
// 3. De data lijn van de slimmer meter wordt geinverteerd aangeboden aan RX2 / D17 van de Arduino Mega.
// 4. Slimme meter stuurt data met: 9600 Baud, 7 bits, Even pariteit en 1 stopbit.
// 5. Arduino stuurt data direct door naar USB met 115200 baud.
// 

const int TriggerPin=13;

void setup()
  {
  Serial.begin(115200);
  Serial2.begin(9600,SERIAL_7E1);
  Serial.println("P1 Slimme Meter test. Voer teken in om trigger te versturen.");

  delay(1000);  
  pinMode(TriggerPin, OUTPUT);
  delay(1000);  
  }


void loop() 
  {
  char inputString[100];
  int inCharCounter=0;
  int LineCounter=0;
  char inChar,x;
  unsigned long TriggerLowTimer=0L;
  unsigned long SerialInputTimer=0L;
  boolean TriggerState=false;

  while(true)
    {
    // Stuur iedere 5 seconden een trigger puls=HOOG die twee seconden duurt.
    if(Serial.available())
      {
      if(Serial.read()=='\n')
        {      
        Serial.println(F("Sending trigger."));
        Serial.println(F("************************************************************"));
        TriggerLowTimer=millis()+2000;
        TriggerState=true;
        LineCounter=0;
        digitalWrite(TriggerPin, TriggerState);
        }
      }

    // Als de trigger lijn twee seconden hoog is geweest, dan zal de
    // slimme meter zijn data zeker hebben verzonden.
    if(TriggerLowTimer<millis() && TriggerState)
      {
      TriggerState=false;
      digitalWrite(TriggerPin, TriggerState);
      }
  
    // Als er een seconde lang geen data meer binnengekomen is, kan worden aangenomen
    // dat het blok met data gereed is.
    if(SerialInputTimer<millis() && LineCounter)
      {
      Serial.println(F("************************************************************"));
      LineCounter=0;
      }


    while(Serial2.available())
      {
      inChar=Serial2.read();
      if(inCharCounter<200)
        inputString[inCharCounter++]=inChar;
        
      if(inChar=='\n')
        {
        inputString[inCharCounter]=0;
        inCharCounter=0;
        SerialInputTimer=millis()+1000;
        
        Serial.print(++LineCounter);
        Serial.print(": ");
        Serial.print(inputString);
        }                    
      }
    }
  }



