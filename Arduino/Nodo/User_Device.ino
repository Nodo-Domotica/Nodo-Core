// De Nodo beschikt over de mogelijkheid om diverse sensoren uit te lezen. Hier staat specifieke code voor uitlezen van deze sensoren.
// Eventueel kunnen sensoren worden aangepast, vervangen of worden toegevoegd. 
// Een Device kan zowel input als output zijn. Er zijn voor de gebruiker twee manieren om een device aan te spreken:
//
// A) Lezen van een waarde: [VariableDevice <Variabele>, <DeviceNummer>]
//          Na aanroep van dit commando wordet de funktie aangeroepen die hoort bij het betreffence device wn
//          wordt bij terugkeer aan de opgegeven variabele een waarde toegekend. Vervolgens verzendt de Nodo
//          automatisch een event zodat waarden kunnen worden verwerkt door deze of een andere Nodo.
//          Bij deze aanroep zal de parameter het variabelenummer bevatten waarvoor opdracht is gegeven om deze te vullen.
//          Eventueel kan deze variabele worden gebruikt om extra gegevens door te geven aan uw routine.
//
// B) Sturen van een device: [Device <DeviceNummer>,<Parameter>]
//          De funktie die hoort bij het betreffende device wordt aangeroepen waarbij <Parameter> kan worden
//          gebruikt als optionele stuurinformatie. Mocht het noodzakelijk zijn om nog andere gegevens mee te sturen
//          naar het device, dan kunnen hiervoor vooraf gevulde Nodo variabelen worden gebruikt.
//
// Voorbeelden van userdevices:
// - Digitale temperatuur sensoren (Zoals Dallas DS18B20)
// - Digitale vochtigheidssensoren (Zoals DTH-11)
// - Vergroten van aantal digitale wired met een multiplexer. Tot 8-ingangen per Wired poort met bv. een 74151.
// - Acht verschillende analoge ingangen meten met eén WiredIn met een LTC1380.
// - WiredOut uitgangen uitbreiden tot 8, 16, 32, 64 verschillende digitale uitgangen met behulp van 74HCT595
// - I2C devices aansturen via de SLC en SDA lijnen van de arduino.
// - etcetera.



//================================= Verwijzing naar code: ===========================================

// Onderstaand de aanroep van de code. Funkties die niet in onderstaande switch worden opgenomen, worden ook door de compiler niet
// mee gecompileerd. De variabele <Sensor> is de tweede parameter van het commando [VariableSensor].
// Het is geen bezwaar om één funktie vanuit meerdere sensornummers (cases) aan te roepen.

float Device(byte Device, int Parameter)
  {
  switch(Device)
    {
    case 1: 
      return Sensor_Temp_Dallas1820(7); // Lees de Dallas DS1820 sensor uit die zich op Arduino poort 7 bevindt. Dit is WiredOut-1

    case 2:
      return 0; // Verwijs hier naar een funktie

    case 3:
      return 0; // Verwijs hier naar een funktie

    case 4:
      return 0; // Verwijs hier naar een funktie

    /* enzovoort */
    
    default:
      return 0; // Als de gebruiker geen geldige sensor heeft ingegeven een 0 teruggeven.
    }
  }


//================================= Sensor specifieke code: ===========================================

// Let op dat de geheugenruimte in de Arduino beperkt is. Scrijf compact, maak bij voorkeur geen gebruik van globale variabelen en zorg er 
// voor dat in de eigen code een lange verwerkings/wachttijden zitten die de betrouwbaarheid van de werking van de Nodo kunnen verstoren.
// leiden tot verstoring van de werking van de Nodo.
// De sensor routines geven moeten een int of float teruggeven die binnen het bereik van een nodo variabele ligt. Valt een waarde buiten dit bereik,
// dan zal de inhoud van de variabele niet correct worden weergegeven.
//
// Let bij het ontwerp van de Hardware en de software op dat geen gebruik wordt gemaakt van de Arduino pinnen die al een voorgedefinieerde
// Nodo funktie hebben, De WiredIn en WiredOut kunnen wel vrij worden gebruikt met die kanttekening dat Nodo commando's de lijnen eveneens 
// Besturen wat kan leiden tot beschadiging van hardware. Let hier in het bijzonder op als een Arduino pin wordt gedefinieerd als een output.
// Op de Nodo Mega zijn nog vier extra communicatielijnen die gebruikt kunnen worden voor User input/output: PIN_IO_1 t/m PIN_IO_4 (Arduino pin 38 t/m 41)
//

/*********************************************************************************************\
 * Deze funktie leest een Dallas temperatuursensor uit. De sensor moet volgens de paracitaire
 * mode worden aangesloten. Des de signaallijn tevens verbinden met een 4K7 naar de Vcc/+5
 * Deze fountie kan worden gebruikt voor alle digitale poorten van de Arduino.
 * Er wordt gebruik gemaakt van de ROM-skip techniek, dus er worden geen adressen uitgelezen.
 * Dit betekent max. één sensor per poort. Dit om geheugen te besparen en geen code kwijt
 * 
 * Deze funktie kan met een kleine aanpassing worden gebruikt voor zowel de DS1820 als de DS18B20
 * variant. Zie comments in de code.
 *
 * Auteur           : Nodo-team (P.K.Tonkes) www.nodo-domotca.nl
 * Datum            : Nov.2012
 * Compatibiliteit  : Vanaf Nodo build nummer 480
 * Compiled size    : 810 bytes
 * Vereiste library : - geen -
 * Externe funkties : - geen -
 \*********************************************************************************************/

uint8_t DallasPin;
float Sensor_Temp_Dallas1820(uint8_t port) // Arduino port.
  {        
  int DSTemp;   // Temperature in 16-bit Dallas format.
  byte ScratchPad[12];  // Scratchpad buffer Dallas sensor.   
  DallasPin=port;
      
  noInterrupts();
  boolean present=DS_reset();DS_write(0xCC /* rom skip */); DS_write(0x44 /* start conversion */);
  interrupts();
  
  if(present)
    {
    delay(800);     // uitleestijd die de sensor nodig heeft
    
    noInterrupts();
    DS_reset(); DS_write(0xCC /* rom skip */); DS_write(0xBE /* Read Scratchpad */);

    // Maak de lijn floating zodat de sensor de data op de lijn kan zetten.
    digitalWrite(DallasPin,LOW);
    pinMode(DallasPin,INPUT);

    for (byte i = 0; i < 9; i++)            // copy 8 bytes
      ScratchPad[i] = DS_read();
    interrupts();
  
    DSTemp = (ScratchPad[1] << 8) + ScratchPad[0];
    if (ScratchPad[1] & 0x80) // negative
      DSTemp = (DSTemp ^ 0xffff) + 1; // 2's complement
  
    return float(DSTemp)/2; // DS1820 variant
    // return float(DSTemp)*0.0625; // DS18-B-20 variant
    }
  return 0;
  }
  
  uint8_t DS_read(void)
  {
  uint8_t bitMask;
  uint8_t r = 0;
  uint8_t BitRead;
  
  for (bitMask = 0x01; bitMask; bitMask <<= 1)
    {
    pinMode(DallasPin,OUTPUT);
    digitalWrite(DallasPin,LOW);
    delayMicroseconds(3);

    pinMode(DallasPin,INPUT);// let pin float, pull up will raise
    delayMicroseconds(10);
    BitRead = digitalRead(DallasPin);
    delayMicroseconds(53);

    if (BitRead)
      r |= bitMask;
    }
  return r;
  }

void DS_write(uint8_t ByteToWrite) 
  {
  uint8_t bitMask;

  pinMode(DallasPin,OUTPUT);
  for (bitMask = 0x01; bitMask; bitMask <<= 1)
    {// BitWrite
    digitalWrite(DallasPin,LOW);
    if(((bitMask & ByteToWrite)?1:0) & 1)
      {
      delayMicroseconds(5);// Dallas spec.= 5..15 uSec.
      digitalWrite(DallasPin,HIGH);
      delayMicroseconds(55);// Dallas spec.= 60uSec.
      }
    else
      {
      delayMicroseconds(55);// Dallas spec.= 60uSec.
      digitalWrite(DallasPin,HIGH);
      delayMicroseconds(5);// Dallas spec.= 5..15 uSec.
      }
    }
  }

uint8_t DS_reset()
  {
  uint8_t r;
  uint8_t retries = 125;
  
  pinMode(DallasPin,INPUT);
  do  {  // wait until the wire is high... just in case
      if (--retries == 0) return 0;
      delayMicroseconds(2);
      } while ( !digitalRead(DallasPin));

  pinMode(DallasPin,OUTPUT); digitalWrite(DallasPin,LOW);
  delayMicroseconds(492); // Dallas spec. = Min. 480uSec. Arduino 500uSec.
  pinMode(DallasPin,INPUT);//Float
  delayMicroseconds(40);
  r = !digitalRead(DallasPin);// Dallas sensor houdt voor 60uSec. de bus laag nadat de Arduino de lijn heeft vrijgegeven.
  delayMicroseconds(420);
  return r;
  }

// *************************** Einde: Sensor_Temp_Dallas1820(); **************************************************
