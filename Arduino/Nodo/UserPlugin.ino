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
 * - Wees zuinig op RAM-geheugen (geen float variabelen, etc.)
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
 * - Gebruikersvariabelen kunnen worden gelezen, waarbij S.UserVar[0] gelijk is aan de eerste gebruikersvariabele.
 \*********************************************************************************************/

void UserPlugin_Command(int Par1, int Par2)
  {
  // Schrijf hier je eigen code
  // onderstaand een paar voorbeelden.
  


  // VOORBEELD-1: uitvoeren van een Nodo commando
  // ExecuteLine("Sound 0;",VALUE_DIRECTION_INTERNAL);    

  // VOORBEELD-2: Loggen naar de SDCard
  // AddFileSDCard("log.dat","Hello, UserPlugin was here.");
  }


 /*********************************************************************************************\ 
 * Dit deel van de plugin wordt altijd aangeroepen direct na ontvangst van een signaal
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
    
  return;
  }
  
