// Als een Nodo tijdens verzending één specifieke Nodo adresseert, dan is de zender de master en de ontvanger de slave
// Er is gelijktijdig slechs één master actief en één slave kan geselecteerd zijn.
// Niet geselecteerde slaves kunnen niet zenden, wel events opvangen die voor de slave bestemd zijn (queue)
// In de queue geplaatste events in een slave queue kunnen (mogelijk) pas leiden tot een zendactie na vrijgave van de band.
// 

void PrintNodoEventStruct(char* str, struct NodoEventStruct *Event)
  {    
  Serial.println(str);
  Serial.print(F("*** debug: Event->Port=             = "));Serial.println(Event->Port); //??? Debug
  Serial.print(F("*** debug: Event->Direction         = "));Serial.println(Event->Direction); //??? Debug
  Serial.print(F("*** debug: Event->Flags             = "));Serial.println(Event->Flags); //??? Debug
  Serial.print(F("*** debug: Event->DestinationUnit   = "));Serial.println(Event->DestinationUnit); //??? Debug
  Serial.print(F("*** debug: Event->SourceUnit        = "));Serial.println(Event->SourceUnit); //??? Debug
  Serial.print(F("*** debug: Event->Checksum=         = "));Serial.println(Event->Checksum); //??? Debug
  Serial.print(F("*** debug: Event->Command           = "));Serial.println(Event->Command); //??? Debug
  Serial.print(F("*** debug: Event->Par1              = "));Serial.println(Event->Par1); //??? Debug
  Serial.print(F("*** debug: Event->Par2              = "));Serial.print(Event->Par2);Serial.print(", 0x");Serial.println(Event->Par2,HEX); //??? Debug
  }

/*


Serial.print(F("*** debug: "));Serial.println(); //??? Debug
PrintNodoEventStruct(Event,"");//???

Nodo adressering en event uitwisseling:

- Iedere Nodo heeft een uniek adres dat gelijk is aan het unitnummer. Dit adres is transparant geldig voor RF, IR en I2C communicatie.
- Er kan maar één Nodo tegelijk zenden. Andere situaties zijn niet onmogelijk maar wel ongeldig.
- Een event bevat het zendende unitnummer en een ontvangend unitnummer
- Default staan alle Nodo's in de luister-modus.
- Default wordt een verzonden event verstuurd naar en verwerkt door alle Nodo's.
- Met [Select <unitnummer>] kan exclusief een bestemmingsnodo worden geselecteerd. Selectie wordt actief na de eerste verzending van een event.
- De selectie moet ook weer worden opgeheven door dezelfde Nodo (of verloopt na 30sec. na laatste event)
- Alle niet-geselecteerde Nodo's zullen het event niet ontvangen.
- Een niet geselecteerde Nodo kan geen gegevens verzenden totdat de selectie is opgeheven of de betreffende Nodo is geselecteerd.

Idee:
- resultaat van html event naar bodytext plakken.
- websocket implementeren op arduino?

Onder de motorkap:

- Een 64-bit event bestaat uit een 32-bit transport deel en een 32-bit data deel. 
- Bestemming en bron unitnummers bevinden zich in het transportdeel.
- Nodo events worden eenmaal verzonden (zonder herhalingen) en hebben een CRC-8 checksum die meegezonden wordt.



































  **:**,*
E,HH:MM,D



*/

