

/*

digitalWrite(PIN_WIRED_OUT_1,HIGH);delay(10);digitalWrite(PIN_WIRED_OUT_1,LOW);//??? Debugging: Wired-Out 1

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
- mini webserver implementeren op arduino?





















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

void PrintRawSignal(void)
  {    
  int x,y;
  
  Serial.println(F("==================================== RawSignal ==================================="));

  Serial.print(F("Repeats="));
  Serial.println(RawSignal.Repeats);

  Serial.print(F("Delay="));
  Serial.println(RawSignal.Delay);

  Serial.print(F("Source="));
  Serial.println(cmd2str(RawSignal.Source));

  Serial.print(F("Multiply="));
  Serial.println(RawSignal.Multiply);

  Serial.print(F("Number="));
  Serial.println(RawSignal.Number);

  Serial.print(F("Pulses (uSec): "));
  for(x=1;x<=RawSignal.Number;x++)
    {
    Serial.print(RawSignal.Pulses[x]*RawSignal.Multiply); 
    Serial.write(',');       
    }
    
  Serial.println();
  Serial.println(F("=================================================================================="));
  }

