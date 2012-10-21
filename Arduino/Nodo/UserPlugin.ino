#ifdef USER_PLUGIN

 /*********************************************************************************************\
 * Hier kan de gebruiker zelf een stuk code plaatsen.
 *
 *
 * WAARSCHUWING: 
 * - Digitale poorten mogen NOOIT DIRECT WORDEN BESCHREVEN. KANS OP HARDWARE BESCHADIGING!
 * - Het wordt ten zeerste af te raden om direct globale variabelen uit de Nodo code te veranderen! Kans op vastlopen Nodo software.
 * - zorg er voor dat er geen wachtloops in de code zitten. Lang durende verwerking / pauzes leiden er toe
 *   dat de Nodo niet meer betrouwbaar signalen zal ontvangen, dat timers niet correct verwerkt worden en dat
 *   communicatie niet correct verloopt.
 * - Wees zuinig op RAM-geheugen (geen float variabelen, string arrays, etc.)
 * - Pas geen code aan buiten dit Plugin tabblad, anders zul je bij volgende releases steeds je
 *   aanpassingen opnieuw moeten aanbrengen.
 \*********************************************************************************************/


 /*********************************************************************************************\
 * Auteur       :
 * Plugin naam  :
 * Datum        :
 * Versie       :
 * Nodo versie  :
 * Doel         :
 * Beschrijving :
 *
 *
 *
 \*********************************************************************************************/

 /*********************************************************************************************\ 
 * Dit deel van de plugin wordt eenmalig aangeroepen direct na boot en tonen welkomsttekst
 \*********************************************************************************************/
void UserPlugin_Init(void)
  {

  }

 /*********************************************************************************************\ 
 * Dit deel van de plugin wordt aangeroepen met het Nodo commando "UserPlugin Par1,Par2"
 * 
 * TIPS:
 * - Par1 en Par2 zijn twee parameters die je kunt meegeven. Deze kunnen beide een waarde hebben van 0..255
 * - De userplugin kan worden aangeroepen als commando of als actie vanuit de eventlist.
 * - Gebruikersvariabelen kunnen worden gelezen, waarbij UserVar[0] gelijk is aan de eerste gebruikersvariabele.
 \*********************************************************************************************/

void UserPlugin_Command(int Par1, int Par2)
  {
  // Schrijf hier je eigen code
  // onderstaand een paar voorbeelden.


  // VOORBEELD-1: Verzend een userevent [UserEvent 111,222]
  // De constanten CMD_xxx die de opdracht representeren kunnen worden gevonden in het tabblad [Nodo] 
  // TransmitCode(command2event(Settings.Unit, CMD_USEREVENT, 111, 222),VALUE_ALL);

  // VOORBEELD-2: uitvoeren van een event: [UserEvent 123,100]
  // ProcessEvent2(command2event(Settings.Unit, CMD_USEREVENT, 123, 100),VALUE_DIRECTION_INPUT,0,0,0);

  // VOORBEELD-3: uitvoeren van een commando: [Sound 0]
  // ExecuteCommand(command2event(Settings.Unit, CMD_SOUND, 0, 0) ,0 , 0, 0);
  
  // VOORBEELD-4: uitvoeren van een Nodo voor de Mega variant
  // ExecuteLine("Sound 0;",VALUE_DIRECTION_INTERNAL);    

  // VOORBEELD-5: Loggen naar de SDCard
  // AddFileSDCard("log.dat","Hello, UserPlugin was here.");
    
  }


 /*********************************************************************************************\ 
 * Dit deel van de plugin wordt altijd aangeroepen direct na ontvangst van een event
 * 
 * TIPS:
 * - Rawsignal is de tabel met de ontvangen impulsen van IR of RF. Deze kan worden gebruikt 
 *   voor verwerken van signalen.
 * - Gebruik deze functie alleen voor onderscheppen van events
 * - Deze functie wordt bij ieder binnenkomend event doorlopen, zorg er voor dat hier
 *   geen onnodige en/of tijdrovende verwerking plaatsvindt.
 \*********************************************************************************************/
boolean UserPlugin_Receive(unsigned long Event)
  {

  // Event = 32-bit Nodo event.
    
    
  // funktie kan op twee manieren worden verlaten:
  // true = Nodo gaat verder met verwerking van het event
  // false = Event wordt niet verder behandeld.
  return true; 
  }
  
 /*********************************************************************************************\ 
 * UserPlugin_Periodically() wordt ongeveer eenmaal per seconde aangeroepen.
 * Het het bedoeld voor taken die korte doorlooptijd vragen zoals checken van zaken.
 *
 * TIPS:
 * - door millis() op te slaan in variabelen, kunnen exacte tijden worden berekend.
 * - De variabele PulseCounter houdt bij hoeveel pulsen hoog-laag-hoog pulsen er zijn geweest.
 * - In deze functie geen tijdrovende taken verrichten anders worden en RF of IR signalen gemist
 \*********************************************************************************************/

void UserPlugin_Periodically()
  {
  // Voorbeeld: Lees iedere 5 sec. de WiredIn poorten uit en geef deze weer op Serial/USB.

//  static int Secondenteller=0; // static, dan blijft deze bewaard na verlaten van deze routine
//  char str[80]; // reserveer een kleine (!) string voor weergave van de analoge waarde
//  int y,z;
//
//  Secondenteller = Secondenteller +1;
//  if(Secondenteller==5)
//   {
//   Secondenteller=0; 
//   for(int x=0;x<WIRED_PORTS;x++)
//    {
//    y=analogRead(PIN_WIRED_IN_1+x);
//    z=map(y,Settings.WiredInput_Calibration_IL[x],Settings.WiredInput_Calibration_IH[x],Settings.WiredInput_Calibration_OL[x],Settings.WiredInput_Calibration_OH[x]);
//    sprintf(str,"Arduino port A%02d=%d, WiredIn-%d=%d.%d",x+PIN_WIRED_IN_1,y,x+1,z/100,abs(z)%100); // %100 en %100 omdat zo een integer kan worden weergegeven als een float
//    Serial.println(str);
//    }
//   Serial.println();
//   }
//  
//  return;
  }


 /*********************************************************************************************\ 
 * Dit deel van de plugin wordt altijd aangeroepen direct na ontvangst van een RF of IR signaal.
 * Deze routine wordt uitgevoerd alvorens het signaal is gedecodeerd en weergegeven op het 
 * eventueel openstaande Serial/Telnet venster.
 *
 * Port bevat een waarde waaruit kan worden afgeleid of het signaal afkomstig was van IR of RF
 * Bij verlaten kan een EventCode worden teruggegeven die vervolgens wordt verwerkt door de Nodo. 
 * Is deze EventCode nul, dan gaat de Nodo regulier verder met verwerking.
 * Is deze EventCode één, dan vindt geen verdere verwerking plaats.
 * Is deze EventCode een andere waarde, dan wordt deze waarde verwerkt als een eventcode.
 * 
 *
 * TIPS:
 * - Rawsignal is de tabel met de ontvangen impulsen van IR of RF. Deze kan worden gebruikt 
 *   voor verwerken van signalen.
 * - Gebruik deze functie alleen voor onderscheppen van signalen en hier een eigen analyse
 *   op uit te voeren.
 * - Deze functie wordt bij ieder binnenkomend event doorlopen, zorg er voor dat hier
 *   geen onnodige en/of tijdrovende verwerking plaatsvindt.
 \*********************************************************************************************/
unsigned long RawSignal_2_UserPlugin(int Port)
  {
    
  // Voorbeeld: Onderstaand een stukje code dat een binnengekomen signaal weergeeft. Eén 'M' of '_'-teken staat voor 250uSec. 
    
//  int x,y;
//  
//  Serial.println(F("==================================== RawSignal ==================================="));
//
//  Serial.print(RawSignal.Number/2);
//  Serial.print(F(" bits received. Pulses (uSec): "));
//  for(x=1;x<=RawSignal.Number;x++)
//    {
//    Serial.print(RawSignal.Pulses[x]); 
//    Serial.write(',');       
//    }
//  Serial.println();
//
//  const int dev=250;  
//  for(x=1;x<=RawSignal.Number;x+=2)
//    {
//    for(y=1+RawSignal.Pulses[x]/dev; y;y--)
//      Serial.write('M');  // Mark  
//    for(y=1+RawSignal.Pulses[x+1]/dev; y;y--)
//      Serial.write('_');    // Space  
//    }
//
//  Serial.println(F("=================================================================================="));
//  Serial.println();

  return 0L; // ga verder met verwerking.
  }

#endif  
