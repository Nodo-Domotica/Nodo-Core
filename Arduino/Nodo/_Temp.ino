// Als een Nodo tijdens verzending één specifieke Nodo adresseert, dan is de zender de master en de ontvanger de slave
// Er is gelijktijdig slechs één master actief en één slave kan geselecteerd zijn.
// Niet geselecteerde slaves kunnen niet zenden, wel events opvangen die voor de slave bestemd zijn (queue)
// In de queue geplaatste events in een slave queue kunnen (mogelijk) pas leiden tot een zendactie na vrijgave van de band.
// 


/*


Serial.print(F("*** debug: "));Serial.println(); //??? Debug
PrintNodoEvent(Event,"");//???

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

      digitalWrite(PIN_IO_2,HIGH);//??? debugging
      delay(1);//???
      digitalWrite(PIN_IO_2,LOW);//???
























*/


void PrintNodoEvent(char* str, struct NodoEventStruct *Event)
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

void RawSignalShow(void)
  {    
  int x,y;
  byte PTMF=RawSignal.Pulses[0];
  
  Serial.println(F("==================================== RawSignal ==================================="));

  Serial.print(F("Resolution="));
  Serial.println(RawSignal.Pulses[0]);

  Serial.print(RawSignal.Number/2);
  Serial.print(F(" bits received. Pulses (uSec): "));
  for(x=1;x<=RawSignal.Number;x++)
    {
    Serial.print(RawSignal.Pulses[x]*PTMF); 
    Serial.write(',');       
    }
  Serial.println();

  const int dev=250;  
  for(x=1;x<=RawSignal.Number;x+=2)
    {
    for(y=1+(RawSignal.Pulses[x]*PTMF)/dev; y;y--)
      Serial.write('M');  // Mark  
    for(y=1+(RawSignal.Pulses[x+1]*PTMF)/dev; y;y--)
      Serial.write('_');    // Space  
    }
  Serial.println();
  Serial.println(F("=================================================================================="));
  }

