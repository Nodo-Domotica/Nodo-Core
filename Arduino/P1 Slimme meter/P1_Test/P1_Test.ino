// 
// Eenvoudig testprogramma om de verbinding met een P1 slimme meter te bekijken/testen. 
//
// ZORG ER VOOR DAT =EERST= DEZE CODE WORDT GEUPLOAD NAAR DE ARDUINO EN PAS DAARNA DE AANSLUITINGEN MET DE SLIMME METER WORDEN GEMAAKT !!!
// aansluitingen weer verwijderen voordat andere code wordt geupload.
//
// We gaan uit van de volgende configuratie:
// 
// 1. Een Arduino Mega ZONDER aanvullende hardware aangesloten
// 2. De Trigger / RTS lijn van de slimme meter is verbonden met D30
// 3. De data lijn van de slimmer meter wordt geinverteerd aangeboden aan RX3 / D15 van de Arduino Mega.
// 4. Slimme meter stuurt data met: 9600 Baud, 7 bits, Even pariteit en 1 stopbit.
// 5. Arduino stuurt data direct door naar USB met 115200 baud.
// 
// IR_RX_DATA  = D18 = PORTD bit 3 = Input
// IR_TX_DATA  = D17 = PORTH bit 0 = geïnverteerde output

#define PIN_IR_TX_DATA              17 // NIET VERANDEREN. Aan deze pin zit een zender IR-Led. (gebufferd via transistor i.v.m. hogere stroom die nodig is voor IR-led)
#define PIN_IR_RX_DATA              18 // Op deze input komt het IR signaal binnen van de TSOP. Bij HIGH bij geen signaal.
#define PULSE_IRQ                    5 // IRQ verbonden aan de IR_RX_DATA pen 18 van de Mega

#define PIN_LED                     13
#define PIN_TRIGGER                 30

void setup()
  {
  pinMode(PIN_TRIGGER, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_IR_RX_DATA, INPUT);
  pinMode(PIN_IR_TX_DATA, OUTPUT);
  digitalWrite(PIN_IR_RX_DATA,INPUT_PULLUP);

  attachInterrupt(PULSE_IRQ,ISR_Invert,CHANGE);

  Serial.begin(115200);
  Serial3.begin(9600,SERIAL_7E1);
  Serial.println("P1 Slimme Meter test. Geef ENTER om trigger te versturen.");

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
    delay(500);
    Serial.print(PORTA,BIN);Serial.print(" ");
    Serial.print(PORTB,BIN);Serial.print(" ");
    Serial.print(PORTC,BIN);Serial.print(" ");
    Serial.print(PORTD,BIN);Serial.print(" ");
    Serial.print(PORTE,BIN);Serial.print(" ");
    Serial.print(PORTF,BIN);Serial.print(" ");
    Serial.print(PORTG,BIN);Serial.print(" ");
    Serial.print(PORTH,BIN);Serial.println(" ");
    if(Serial.available())
      {
      if(Serial.read()=='\n')
        {      
        Serial.println(F("Sending trigger."));
        Serial.println(F("************************************************************"));
        TriggerLowTimer=millis()+2000;
        TriggerState=true;
        LineCounter=0;
        digitalWrite(PIN_TRIGGER, TriggerState);
        digitalWrite(PIN_LED, TriggerState);
        }
      }

    // Als de trigger lijn twee seconden hoog is geweest, dan zal de
    // slimme meter zijn data zeker hebben verzonden.
    if(TriggerLowTimer<millis() && TriggerState)
      {
      TriggerState=false;
      digitalWrite(PIN_TRIGGER, TriggerState);
      digitalWrite(PIN_LED, TriggerState);
      }

    // Als er een seconde lang geen data meer binnengekomen is, kan worden aangenomen
    // dat het blok met data gereed is.
    if(SerialInputTimer<millis() && LineCounter)
      {
      Serial.println(F("************************************************************"));
      LineCounter=0;
      }


    while(Serial3.available())
      {
      inChar=Serial3.read();
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


void ISR_Invert(void)
  {
  // Hier wordt de gebruiker een inverter aangeboden. Dat scheelt hardware.
  // Directe poortmanipulatie werkt vele malen sneler.
  // Op een ATMega2560 gelden de volgende port/pin mappings:
  // IR_RX_DATA  = D18 = PORTD bit 3 = Input
  // IR_TX_DATA  = D17 = PORTH bit 0 = geïnverteerde output

  // ??? Nog omzetten daar directe poort manipulatie:  bitWrite(PORTH,0, bitRead(PORTD,3));
  digitalWrite(PIN_IR_TX_DATA,!digitalRead(PIN_IR_RX_DATA));

  }


