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
 * - Wees zuinig op RAM-geheugen (variabelen zoals, integers, strings, etc.)
 * - Pas geen code aan buiten dit Plugin tabblad, anders zul je bij volgende releases steeds je
 *   aanpassingen opnieuw moeten aanbrengen.
 \*********************************************************************************************/


 /*********************************************************************************************\
 * Auteur       :
 * Plugin naam  :
 * Datum        :
 * Versie       :
 * Nodo versie  :
 * Beschrijving :
 *
 *
 *
 \*********************************************************************************************/


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
  
  
  // VOORBEELD-1: een gebruikersvariabelen gebruiken:
  UserVar[0]=123;           // Gebruikersvariabele 1 is nu gelijk aan 123. variabelen starten met [0] en mogen max. waarde hebben van 255 
  int MijnVar=UserVar[5];   // MijnVar is gelijk aan de gebruikersvariabele 6 (immers 0 is de eerste)


  // VOORBEELD-2: Lezen analoge waarde. De analoge poorten kunnen worden uitgelezen met: WiredAnalog(Poort). De teruggegeven waarde is 8-bits. Poort 0..7
  int AnalogeWaarde=WiredAnalog(3); // Lees de waarde van de vierde WiredIn poort. Immers 0 is de eerste poort.

  // VOORBEELD-3: uitvoeren van een Nodo commando
  ExecuteLine("Sound 0;",VALUE_DIRECTION_INTERNAL);    

  // VOORBEELD-4: Loggen naar de SDCard
  LogSDCard("Hello, UserPlugin was here.");

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
  
