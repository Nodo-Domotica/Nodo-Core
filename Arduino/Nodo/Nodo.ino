
/****************************************************************************************************************************\
* Arduino project "Nodo" © Copyright 2013 Paul Tonkes 
* 
* This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License 
* as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
* This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
* You received a copy of the GNU General Public License along with this program in file 'COPYING.TXT'.
*
* Voor toelichting op de licentievoorwaarden zie    : http://www.gnu.org/licenses
* Uitgebreide documentatie is te vinden op          : http://www.nodo-domotica.nl
* bugs kunnen worden gelogd op                      : https://code.google.com/p/arduino-nodo/
* Compiler voor deze programmacode te downloaden op : http://arduino.cc
* Voor vragen of suggesties, mail naar              : p.k.tonkes@gmail.com
* Compiler                                          : Arduino Compiler met minimaal versie 1.0.1
* Libraties                                         : EthernetNodo library vervangt de standaard Ethernet library!
\*************************************************************************************************************************/
                                                                  
                                                                                                                                                                                  
//****************************** Door gebruiker in te stellen - voor elke keer dat u compileert ***************************************************************

// Stap 1.
// De code kan worden gecompileerd als een Nodo-Small voor de Arduino met een ATMega328 processor of een Nodo-Mega met een ATMega1280/2560.
// stel allereerst het juiste board-type en COM-poort in op de Arduino compiler.

// Stap 2.
// Dit volgende blokje moet u handmatig instellen afhankelijk van het Arduino type!
// Vanwege een bug in de Arduino linker moeten enkele '#include' regels handmatig worden aangepast.
// Voor een arduino anders dan een Mega1280 of Mega2560 de onderstaande vier regels vooraf laten gaan door twee slashes //
// Anders worden de niet gebruikte libraries toch meegecompileerd en zal deze niet in een ATMega328 passen.
// en pas eventueel de vier include statements aan (zie onder):
#include <EthernetNodo.h>
#include <SPI.h>
#include <Arduino.h>
#include <SD.h>

// Stap 3.tset
// Stel hieronder in welk Nodo unit nummer u gaat compileren. Uitgangspunt is dat elke fysieke Nodo een uniek unit nummer krijgt toegekend.
// Voor elk unit nummer kunt u verderop deze pagina een blokje kenmerken instellen.
// Als dat eenmalig is gedaan, kunt u voortaan die set instellingen selecteren met het unit nummer:
#define UNIT_NODO          1



//****************************** Door gebruiker in te stellen - eenmalig, en daarna alleen bij wijzigingen ***************************************************************
// Voor een beschrijving van de beschikbare devices: zie de Wiki pagina's. 
// Onder aan deze pagina vindt u een complete lijst met de benodigde statements om een device te kunnen gebruiken.
// In dit blokje geeft u aan welke devices gebruikt worden in het landschap met Nodo's. 

  #define DEVICE_01       // Kaku            : Klik-Aan-Klik-Uit ontvangen
  #define DEVICE_02       // SendKaku        : Klik-Aan-Klik-Uit verzenden
  #define DEVICE_03       // NewKAKU         : Klik-Aan-Klik-Uit ontvangen, automatische codering. 
  #define DEVICE_04       // SendNewKAKU     : Klik-Aan-Klik-Uit zenden, automatische codering. 
  #define DEVICE_05       // TempRead        : Temperatuursensor Dallas DS18B20. (Let op; -B- variant, dus niet DS1820) 

// Geef in onderstaande tabel aan welke devices die in ALLE Nodo's feitelijk worden gebruikt: 
  #define DEVICE_CORE_01  // Kaku            : Klik-Aan-Klik-Uit ontvangen  
  #define DEVICE_CORE_02  // SendKaku        : Klik-Aan-Klik-Uit verzenden 
  #define DEVICE_CORE_03  // NewKAKU         : Klik-Aan-Klik-Uit ontvangen, automatische codering.
  #define DEVICE_CORE_04  // SendNewKAKU     : Klik-Aan-Klik-Uit zenden, automatische codering.


// hieronder kunt u de instellingen per unit nummer vastleggen. Bij compileren wordt a.d.h.v. het UNIT_NODO nummer de juiste set gekozen.
// het is handig om per gebruikte Nodo eenmalig een blokje met definities te maken. 
// dit blokje hoeft daarna alleen gewijzigd te worden als er een wijziging in die Nodo nodig is.
// Als startpunt hebben we een aantal Nodo's als voorbeeld gedefinieerd.


// ----------------------------------------------------------------------------------------------------------------------------------------
// Definities voor Nodo unit nummer 1 
// Hardware    : Arduino Mega 2560
// Toepassing  : 
#if (UNIT_NODO == 1)
  // Specificatie van deze Nodo:
  #define NODO_MEGA                    true
  #define ETHERNET                     true 
  // Devices in gebruik op deze Nodo:
#endif 
// ----------------------------------------------------------------------------------------------------------------------------------------




// ----------------------------------------------------------------------------------------------------------------------------------------
// Definities voor Nodo unit nummer 10 
// Hardware    : Arduino Mega 2560, Geen Ethernetkaart, Dallas sensoren op poort 1 en 2
// Toepassing  :
#if (UNIT_NODO == 10)
  // Specificatie van deze Nodo:
  #define NODO_MEGA                    true
  #define ETHERNET                     false 
  // Devices in gebruik op deze Nodo:
  #define DEVICE_CORE_18  // SignalAnalyzer  : Geeft bij een binnenkomend signaal informatie over de pulsen weer.
#endif 
// ----------------------------------------------------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------------------------------------------------
// Definities voor Nodo unit nummer 3 
// Hardware    : Arduino Mega 2560, Ethernetkaart
// Toepassing  :
#if (UNIT_NODO == 3)
  // Specificatie van deze Nodo:
  #define NODO_MEGA                    true
  #define ETHERNET                     false 
  // Devices in gebruik op deze Nodo:

#endif 
// ----------------------------------------------------------------------------------------------------------------------------------------




// ----------------------------------------------------------------------------------------------------------------------------------------
// Definities voor Nodo unit nummer 2
// Hardware    : Arduino Duemilanove, Dallas sensor op poort 1
// Toepassing  :
#if (UNIT_NODO == 2)
  // Specificatie van deze Nodo:
  // geen
  // Devices in gebruik op deze Nodo:
  #define DEVICE_CORE_05 // TempRead        : Temperatuursensor Dallas DS18B20. (Let op; -B- variant, dus niet DS1820)
#endif 
// ----------------------------------------------------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------------------------------------------------
// Definities voor Nodo unit nummer 15
// Hardware    : Arduino Duemilanove, alleen wired in/out
// Toepassing  :
#if (UNIT_NODO == 15)
  // Specificatie van deze Nodo:
  // geen
  // Devices in gebruik op deze Nodo:
  // geen
#endif 
// ----------------------------------------------------------------------------------------------------------------------------------------




// ----------------------------------------------------------------------------------------------------------------------------------------
// Hieronder vindt u een complete lijst van alle device selectie statements.
// Kopieer de gewenste statements naar:
// * ofwel het blokje waarin u aangeeft dat een device in alle Nodo's moet worden gebruikt;
// * ofwel naar het definitie blokje van de specifieke Nodo die het device nodig heeft.
//
// toelichting:
// De regel die de toevoeging "CORE" in de naam heeft, zorgt ervoor dat de kern van de code wordt meegecompileerd. 
// Beschouw dit als een driver voor het device. Compileer hem alleen mee in de Nodo die het device fysiek aan boord heeft. 
//
// de regel die "MMI" in de naam heeft, is het front end. Dit is nodig in alle Nodo's die op een of andere manier met het device gaan werken.
// Het heeft geen zin om dit mee te compileren in een small, want die kent geen MMI.
// Wel is het nodig in een Mega, als die het device ofwel zelf aan boord heeft, ofwel het device in een andere Nodo gebruikt (of moet begrijpen).
// Met dat laatste bedoelen we het tonen van events als die binnenkomen op een Mega vanuit een andere Nodo.
//
// Tip: laat de lijst hieronder compleet voor toekomstig gebruik.
// Dus met copy/paste overnemen, niet met cut/paste!
//
// Repository:
//
/*
  #define DEVICE_01       // Kaku            : Klik-Aan-Klik-Uit ontvangen
  #define DEVICE_CORE_01  // Kaku            : Klik-Aan-Klik-Uit ontvangen  
  #define DEVICE_02       // SendKaku        : Klik-Aan-Klik-Uit verzenden
  #define DEVICE_CORE_02  // SendKaku        : Klik-Aan-Klik-Uit verzenden 
  #define DEVICE_03       // NewKAKU         : Klik-Aan-Klik-Uit ontvangen, automatische codering. 
  #define DEVICE_CORE_03  // NewKAKU         : Klik-Aan-Klik-Uit ontvangen, automatische codering.
  #define DEVICE_04       // SendNewKAKU     : Klik-Aan-Klik-Uit zenden, automatische codering. 
  #define DEVICE_CORE_04  // SendNewKAKU     : Klik-Aan-Klik-Uit zenden, automatische codering. 
  #define DEVICE_05       // TempRead        : Temperatuursensor Dallas DS18B20. (Let op; -B- variant, dus niet DS1820)
  #define DEVICE_CORE_05  // TempRead        : Temperatuursensor Dallas DS18B20. (Let op; -B- variant, dus niet DS1820) 


*/
// ----------------------------------------------------------------------------------------------------------------------------------------

// hieronder volgen nog enkele instellingen voor alle Nodo unit nummers
#define HOME_NODO                    1     // Home adres van Nodo's die tot één groep behoren (1..7)
#define PULSE_DEBOUNCE_TIME         10     // pulsen kleiner dan deze waarde worden niet geteld. Bedoeld on verstoringen a.g.v. ruis of dender te voorkomen
#define PULSE_TRANSITION       FALLING     // FALLING of RISING: Geeft aan op welke flank de PulseCounter start start met tellen. Default FALLING
#define I2C_START_ADDRESS            1     // Alle Nodo's op de I2C bus hebben een uniek adres dat start vanaf dit nummer. Er zijn max. 32 Nodo's. Let op verlap met andere devices. RTC zit op adres 104.
#define NODO_30_COMPATIBLE        true     // t.b.v. compatibiliteit met vorige Nodo versie: NewKAKU HEX waarden en UserEvents
#define BAUD                     19200     // Baudrate voor seriële communicatie.
#define PASSWORD_MAX_RETRY           5     // aantal keren dat een gebruiker een foutief wachtwoord mag ingeven alvorens tijdslot in werking treedt
#define PASSWORD_TIMEOUT           300     // aantal seconden dat het terminal venster is geblokkeerd na foutive wachtwoord
#define TERMINAL_TIMEOUT           600     // Aantal seconden dat, na de laatst ontvangen regel, de terminalverbinding open mag staan.
#define SMALL_DEBUG_INFO         false     // Laat op serial van een Small debug info zien als er events worden verzonden of ontvangen.

//****************************** Einde gedeelte door gebruiker in te stellen ************************************************



