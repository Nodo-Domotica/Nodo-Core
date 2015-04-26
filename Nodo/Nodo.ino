#define NODO_DEBUG_QUEUE                        false
#define NODO_DEBUG_NRF24L01                     false

/****************************************************************************************************************************\
* Arduino project "Nodo" © Copyright 2010..2015 Paul Tonkes
*
* This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
* This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
* You received a copy of the GNU General Public License along with this program in file 'COPYING.TXT'.
*
* Voor toelichting op de licentievoorwaarden zie    : http://www.gnu.org/licenses
* Uitgebreide documentatie is te vinden op          : http://www.nodo-domotica.nl
* Forum                                             : http://www.nodo-domotica.nl/forum/
* bugs kunnen worden gelogd op                      : https://github.com/Nodo-Domotica
* Compiler voor deze programmacode te downloaden op : http://arduino.cc
* Voor vragen of suggesties, mail naar              : p.k.tonkes@gmail.com
* Compiler                                          : Arduino Compiler V1.6.1
\*************************************************************************************************************************/







// ================================================================================================
// Op de verschillende typen Arduino's kennen we kennen twee software varianten
// van de Nodo:
//
// Nodo-Mega:   Een Nodo op basis van een Arduino Mega met een ATMega1280 of
//              2560 processor. Deze Nodo heeft o.a. de mogelijkheid tot ethernet
//              communicatie.
//
// Nodo-Small:  Dit is een kleine Nodo die wordt vooral wordt gebruikt als
//              satelliet in combinatie met een centrale Nodo-Mega.
//              Een Nodo-Small maakt gebruik van een Arduino met een ATMega328
//              processor. (Nano, Duemillanove, Uno, etc.)
//
//
// Bij gebruik van meerdere Nodo's kan voor iedere Nodo een eigen configuratie
// file worden aangemaakt. In deze configuratie files kan worden aangegeven welke
// plugins worden gebruikt en kunnen eventueel speciale instellingen worden
// opgegeven die mee gaan met het compileren van de code.
//
// Plaatsen van de juiste programmacode bestaat uit de volgende stappen:
//
// Stap-1: Geef in deze file de directory aan waar deze code zich bevindt.
// Stap-2: Bepaal welke hardware je hebt. Dit bepaald welke config-file je nodig hebt
// Stap-3: Optioneel: Geef in de config-file aan welke plugins je wilt gebruiken.
// Stap-4: Compileer en upload de Nodo code met de Arduino compiler.
//
// ================================================================================================



// ################################### Stap-1 #####################################################
// Geef in onderstaande regel op waar de .ino bestanden zich bevinden die je nu
// geopend hebt.Dit is nodig om hardware- en configuratiebestanden te kunnen
// compileren.Geef het volledige pad op.
// Let op: Bij Linux/Unix zijn de namen case-sensitive.

#define SKETCH_PATH  D:\User\Paul\GitHub\Nodo-Core\Nodo


// ################################### Stap-2 #####################################################
// Bepaal wat voor Nodo je hebr door een config-file te kiezen. Er zijn enkele config-files
// meegeleverd als voorbeeld, echter voor al je verschillende Nodo's kun je een eigen config-file
// maken of aanpassen.  
//
// * Nodo-Mega: Heb je een Arduino Mega2560 board met een NES of koppelprint,
//   gebruik dan het config-bestand 'Config_01.c'.
//
// * Nodo-Mini: Heb je een Arduino Uno / Duemillanove of ander board met een
//   andere Nodo op basis van de ATMega328, gebruik dan het config-bestand
//   'Config_15.c'
//
// * Nodo-Mini: Heb je een Arduino ProMini die alleen beschikt over een 2.4Ghz
//   zend/ontvangst module van het type NRF24L01, gebruik dan het config-bestand
//   'Config_20.c'
//
// In de config-file wordt tevens opgegeven welke plugins in de Nodo geladen moeten worden.
// Kijk voor verdere info in de config-files.

#define CONFIG_FILE Config_01.c


// ################################### Stap-3 #####################################################
// Optioneel: Geef in de config-file aan welke plugins je wilt gebruiken en
// selecteer indien nodig afwijkende settings. Ook kunnen andere hardware
// varianten worden opgegeven in het Config-bestand.
// Open hiervoor het Config-bestand zoals boven opgegeven en pas aan naar wens.
// Je kunt naar eigen wens voor iedere Nodo die je hebt een specifiek
// config-bestand aanmaken waar alle gewenste settings kunnen worden
// opgegeven.
//
// Let op: als gevolg van een probleem in de Ardiuno IDE is het niet mogelijk
//         om een library conditioneel mee te compileren in de code.
//         Helaas gebruikt de SDCard library behoorlijk wat programmageheugen
//         zelfs als er geen SDCard funktionaliteit of hardware aanwezig is.
//
//         Heeft je Nodo geen SDCard, dan kan voor een Nodo-Small de onderstaande
//         regel worden voorafgegaan door de comment tekens '//' om zo geheugen
//         te besparen. Voor een Nodo-Mega is het gebruik van een SDCard verplicht.

#include <SD.h>




// ################################### Stap-4 #####################################################
// Uploaden van de code werkt als volgt: Selecteer in de Arduino compiler in het menu 'Extra/Board'
// het juiste Arduino board en de com-poort. Type vervolgens ctrl-u en de code wordt gecompileerd
// en geupload naar de Arduino. Open vanuit het menu 'Extra' de seriele monitor
// en er zal, als alles correct is verlopen, een Nodo welkomsttekst worden getoond.
//




