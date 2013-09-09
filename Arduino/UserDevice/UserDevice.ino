//
//
//                            ***** MY - D E V I C E ***** 
//
//                 ==>>> Zie tabblad NodoCommunication voor toelichting
//
//

unsigned long DelayTimer=0;
#define DEVICE_UNIT     7                        // Unitnummer waarmee dit device bekend is in het Nodo landschap.
#define DEVICE_ID     255                        // ID van dit device. Moet corresponderen met Nodo-device ID dat in de Nodo draait.


void setup() 
  {    
  NodoInit(DEVICE_UNIT, DEVICE_ID);               // start I2C communicatie.  
  Serial.begin(19200);                            // Start seriele communicatie.
  }

  
void loop() 
  {
  byte Unit;
  byte Par1;
  unsigned long Par2;
  
  if(NodoReceive(&Unit, &Par1, &Par2)){                      // Controleer of er data ontvangen is. Indien dit het geval is, dan wordt de struct MyData gevuld en kunnen de gegevens worden gebruikt.
    Serial.print(F("Ontvangen: Unit="));  Serial.print(Unit);
    Serial.print(F(", Par1="));    Serial.print(Par1);
    Serial.print(F(", Par2="));  Serial.println(Par2);
    }

  if((DelayTimer + 5000)< millis()) {            // Iedere vijf seconden. De hoofdloop blijft nu gewoon werken zodat ontvangen wordt.
    DelayTimer=millis();                         // Teller voor de pauze weer resetten.

    Par1 = 123;                                  // Vul als voorbeeld Par1 met een (willekeurige) waarde.
    Par2 = millis()/1000;                        // Vul als voorbeeld Par2 met een waarde. In dit geval aantal seconden na een reset.
    Unit = 10;                                    // Nodo unit waar de data naar toe moet. nul waarde = De data wordt verzonden naar alle bekende Nodo's.
    
    NodoDataSend(Unit, Par1, Par2);               // Verzend de data via I2C.
  
    Serial.print(F("Verzonden: Unit="));  Serial.print(Unit);
    Serial.print(F(", Par1="));    Serial.print(Par1);
    Serial.print(F(", Par2="));  Serial.println(Par2);
    }
  }



