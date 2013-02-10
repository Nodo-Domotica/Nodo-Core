/*****************************************************************************************************************\
*
*  === ALGEMENE BESCHRIJVING VOOR DEVECES ===
*
* De Nodo beschikt over de mogelijkheid om diverse sensoren uit te lezen of devices te sturen.
* Er kunnen vier devices worden opgenomen in de Nodo code.
* 
* Syntax: [Device_n <Par1:Parameterr>, <Par2:Parameter>]
*         De funktie die hoort bij het betreffende device wordt aangeroepen waarbij <Parameter> kan worden
*         gebruikt als optionele stuurinformatie. Mocht het noodzakelijk zijn om nog andere gegevens mee te sturen
*         naar het device, dan kunnen hiervoor vooraf gevulde Nodo variabelen worden gebruikt.
*
* Voorbeelden van userdevices:
* - Digitale temperatuur sensoren (Zoals Dallas DS18B20)
* - Digitale vochtigheidssensoren (Zoals DTH-11)
* - Vergroten van aantal digitale wired met een multiplexer. Tot 8-ingangen per Wired poort met bv. een 74151.
* - Acht verschillende analoge ingangen meten met eén WiredIn met een LTC1380.
* - WiredOut uitgangen uitbreiden tot 8, 16, 32, 64 verschillende digitale uitgangen met behulp van 74HCT595
* - I2C devices aansturen via de SLC en SDA lijnen van de arduino.
* - etcetera.
* De Wired poorten en de additionele IO poorten op de Mega in uw eigen code gebruiken aan de hand van de naam zoals deze zijn gedefinieerd
* in de code:
*
* PIN_WIRED_OUT_n staat voor WiredOut poort, waarbij n overeen komt met het nummer van de WiredOut poort te beginnen met 1.
* PIN_WIRED_IN_n staat voor WiredOut poort, waarbij n overeen komt met het nummer van de WiredOut poort te beginnen met 1.
* PIN_IO_n staat voor additionele IO-poort, waarbij n overeen komt met het nummer van deze poort, te beginnen met 1 (Alleen beschikbaar op de Mega).
* 
* Voor de verwijzing naar de Arduino pinnummers: zie schema of declaraties in tabblad "Base"
* ANDERE POORTEN NIET GEBRUIKEN OMDAT DEZE (IN DE TOEKOMST) EEN SPECIFIEKE FUNKTIE HEBBEN.
*
* Om een device mee te compileren moet in het tabblad van Nodo.INO de volgende regel worden opgenonen:
*
* #define DEVICE_n "MijnDeviceNaam"
*
* Naam van de device wordt NIET gebruikt in de gebruikersinterface. De 'n' staat in dit geval voor het devicenummer. 
*
* Let op dat de geheugenruimte in de Arduino beperkt is. Schrijf compact, maak bij voorkeur geen gebruik van globale variabelen en zorg er 
* voor dat in de eigen code een lange verwerkings/wachttijden zitten die de betrouwbaarheid van de werking van de Nodo kunnen verstoren.
* leiden tot verstoring van de werking van de Nodo.
* sensor routines geven moeten een int of float teruggeven die binnen het bereik van een nodo variabele ligt. Valt een waarde buiten dit bereik,
* dan zal de inhoud van de variabele niet correct worden weergegeven.
* Let bij het ontwerp van de Hardware en de software op dat geen gebruik wordt gemaakt van de Arduino pinnen die al een voorgedefinieerde
* Nodo funktie hebben, De WiredIn en WiredOut kunnen wel vrij worden gebruikt met die kanttekening dat Nodo commando's de lijnen eveneens 
* Besturen wat kan leiden tot beschadiging van hardware. Let hier in het bijzonder op als een Arduino pin wordt gedefinieerd als een output.
* Op de Nodo Mega zijn nog vier extra communicatielijnen die gebruikt kunnen worden voor User input/output: PIN_IO_1 t/m PIN_IO_4 (Arduino pin 38 t/m 41)
* Besef dat niet alle pennen van de Arduino gebruikt kunnen worden daar vele Arduino pinnen al een voorgedefinieerde
* Nodo funktie hebben, De WiredIn en WiredOut kunnen wel vrij worden gebruikt met die kanttekening dat Nodo commando's de lijnen eveneens 
* Besturen wat kan leiden tot beschadiging van hardware. Let hier in het bijzonder op als een Arduino pin wordt gedefinieerd als een output.
* Op de Nodo Mega zijn nog vier extra communicatielijnen die gebruikt kunnen worden voor User input/output: PIN_IO_1 t/m PIN_IO_4 (Arduino pin 38 t/m 41)
\*****************************************************************************************************************/



//#######################################################################################################
//######################## DEVICE_1: Dallas Temp. sensor DS18B20      ###################################
//#######################################################################################################

#ifdef DEVICE_1

/*********************************************************************************************\
 * Gebruik van de parameters in het Nodo commando "Device_1 <Par1:Poortnummer>, <Par2:Variabele>"
 **********************************************************************************************
 * Deze funktie leest een Dallas temperatuursensor uit. De sensor moet volgens de paracitaire
 * mode worden aangesloten. De signaallijn tevens verbinden met een 4K7 naar de Vcc/+5
 * Deze fucntie kan worden gebruikt voor alle digitale poorten van de Arduino.
 * Er wordt gebruik gemaakt van de ROM-skip techniek, dus er worden geen adressen uitgelezen.
 * Dit betekent max. één sensor per poort. Dit om geheugen te besparen. De uitgelezen waarde
 * wordt in de opgegeven variabele opgeslagen.
 * 
 * Deze funktie kan met een kleine aanpassing worden gebruikt voor zowel de DS1820 als de DS18B20
 * variant. Zie comments in de code.
 *
 * Auteur           : Nodo-team (P.K.Tonkes) www.nodo-domotca.nl
 * Datum            : Jan.2013
 * Compatibiliteit  : Vanaf Nodo build nummer 500
 * Compiled size    : 645 bytes
 * Vereiste library : - geen -
 * Externe funkties : float2ul()
 \*********************************************************************************************/

uint8_t DallasPin;
boolean Device_1(struct NodoEventStruct *Event)
  {        
  int DSTemp;                 // Temperature in 16-bit Dallas format.
  byte ScratchPad[12];        // Scratchpad buffer Dallas sensor.   
  DallasPin=Event->Par2;      // Hier heeft de user het poortnummer in opgegeven
      
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
    }

  // Indien gewenst kan de struct Event worden gevuld met een nieuw event of commando. De Nodo verwerkt deze dan als een regulier
  // event. Zorg er dan wel voor dat de struct Event correct is gevuld met Command, Par1 en Par2.
  // We verwerken de uitgelezen waarde van de sensor door deze in een variabele te stoppen. Omdat het binnengekomen event nu geen nut 
  // meer heeft, mag zonder bezwaar de struct Event worden gebruikt om een nieuw event te genereren, dan hoeven we geen nieuwe te declareren. 
  // Bij terugkomst zal de Nodo dit event verwerken. Als er niets verwerkt moet worden, dan Event->Command gelijk maken aan nul.
  // Par3 wordt gebruikt voor opslag van de waarde. Dit is een 32-bit variabele. Om een float te converteren naar een Par3 hebben we hulp nodig
  // van de funktie float2ul()

  byte VarNr = Event->Par2;                               // De originele Par1 tijdelijk opslaan want hier zit de variabelenummer in waar de gebruiker de uitgelezen waarde in wil hebben
  ClearEvent(Event);                                      // Ga uit van een default schone event. Oude eventgegevens wissen.
  Event->Command      = CMD_VARIABLE_SET;                 // Commando "VariableSet"
  Event->Par1         = VarNr;                            // Par1 is de variabele die we willen vullen.
  Event->Par2         = float2ul(float(DSTemp)*0.0625); // DS18B20 variant;
  // Event->Par2         = float2ul(float(DSTemp)*0.0625); // Deze regel gebruiken voor de oudere en minder nauwkeurige DS1820 variant  

Serial.print(F("*** debug: Device_1: "));Serial.println(float(DSTemp)*0.0625); //??? Debug

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

#endif
// *************************** Einde: Sensor_Temp_Dallas1820(); **************************************************


