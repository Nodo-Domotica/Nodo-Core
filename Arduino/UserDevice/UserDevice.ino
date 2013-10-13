//
//                            ***** MY - U S E R D E V I C E ***** 
//
//                 ==>>> Zie tabblad NodoCode voor toelichting  <<<==
//

#define DEVICE_UNIT     7                        // Unitnummer waarmee MyDevice bekend is in het Nodo landschap.
#define PLUGIN_ID     255                        // Plugin nummer waar MyDevice mee communiceert, wordt gebruikt voor verzenden data naar een specifieke Nodo plugin

void setup() 
  {    
  NodoInit(DEVICE_UNIT, PLUGIN_ID);              // start I2C communicatie.  
  Serial.begin(19200);                           // Start seriele communicatie.
  }

  
void loop() 
  {
  byte Par1,Unit;
  unsigned long Par2, DelayTimer;
  int x=0,y=128;
  
  while(true)
    {
    // Controleer of er data ontvangen is.
    if(Unit=NodoReceive(&Par1, &Par2))
      {
      Serial.print(F("Received: Unit="));  Serial.print(Unit);  Serial.print(F(", Par1=")); Serial.print(Par1); Serial.print(F(", Par2="));  Serial.println(Par2);
      }
  
    // Verzend om de vijf seconden een DataEvent naar de plugin 255
    if((DelayTimer + 5000)< millis())
      {
      DelayTimer=millis();                         // Teller voor de pauze weer resetten.
  
      // In it voorbeeld worden drie soorten events naar de Nodo verstuurd:
      I2C_Send("Hello world!");
      Serial.println("Sent.");
//      Nodo_DataSend(10,x,y);                       // Verzend Par1=x en Par2=y naar de plugin die draait in Unit=10
//      Nodo_UserEventSend(x, y);                    // Verzend een userevent
//      Nodo_VariableSend(1,y);                      // Verzend een variabele-1
//      Serial.print(F("Sent: Par1=")); Serial.print(x); Serial.print(F(", Par2=")); Serial.println(y);
  
      if(x<255)x++;else x=0;
      if(y<255)y++;else y=0;    
      
      }//timer
    }//while
  }//loop


