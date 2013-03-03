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
* Default settings voor verbinding met de WebApp    : HTTPHost www.nodo-domotica.nl/webapp/nodo.php
*                                                     PortOutput 80
*                                                     InputPort 6636
*                                                     Password <gelijk aan in WebApp opgegeven>
*                                                     Output HTTP,On
*
\*************************************************************************************************************************/


                                                                     
                                                                     
                                                                     
                                             
//****************************** Door gebruiker in te stellen: ***************************************************************
// De code kan worden gecompileerd als een Nodo-Small voor de Arduino met een ATMega328 processor of een Nodo-Mega met een ATMega1280/2560.
// stel allereerst het juiste board-type en COM-poort in op de Arduino compiler.
// en pas eventueel de vier include statements aan (zie onder)
#include <EthernetNodo.h>
#include <SPI.h>
#include <Arduino.h>
#include <SD.h>

// Stel hieronder in welk Nodo unit nummer u gaat compileren. De set instellingen moet dan wel hieronder beschreven staan!
// als je eenmalig per unit nummer de definities vastlegt, dan hoef je daarna bij compileren slechts het juiste unit nummer te kiezen.

#define UNIT_NODO          1

// Let op, dit volgende blokje moet u handmatig instellen afhankelijk van het Arduino type!!!!!!!!!!!!
// Vanwege een bug in de Arduino compiler moeten enkele regels handmatig worden aangepast!
// Voor een arduino anders dan een Mega1280 of Mega2560 de onderstaande vier regels vooraf laten gaan door twee slashes //
// Anders worden de niet gebruikte libraries toch meegecompileerd en zal deze niet in een ATMega328 passen.


// Geef onderstaand aan welke devices worden gebruikt in deze EN andere Nodo's die ondeling met elkaar communiceren.
// Deze definities zijn noodzakelijk om de Nodo's onderling goed te kunnen laten communiceren.
// Onderstaande regels zijn dus niet de feitelijke programmacode die de I/O van het device verzorgen.
#define DEVICE_01 // Kaku            : Klik-Aan-Klik-Uit / HomeEasy protocol ontvangst
#define DEVICE_02 // SendKaku        : Klik-Aan-Klik-Uit / HomeEasy protocol verzenden 
#define DEVICE_03 // NewKAKU         : Klik-Aan-Klik-Uit ontvangst van signalen met automatische codering. Tevens bekend als Intertechno.
#define DEVICE_04 // SendNewKAKU     : Klik-Aan-Klik-Uit ontvangst van signalen met automatische codering. Tevens bekend als Intertechno.
#define DEVICE_05 // TempRead        : Temperatuursensor Dallas DS18B20. (Let op; -B- variant, dus niet DS1820)

// hieronder kunt u de instellingen per unit nummer vastleggen. Bij compileren wordt a.d.h.v. het UNIT_NODO nummer de juiste set gekozen.
// het is handig om per gebruikte Nodo eenmalig een blokje met definities te maken. 
// dit blokje hoeft daarna alleen gewijzigd te worden als er een wijziging in die Nodo nodig is.
// Als startpunt hebben we twee Nodo's als voorbeeld gedefinieerd: een Mega, die unit nummer 1 krijgt, en een Small, die nummer 15 ontvangt.
// ----------------------------------------------------------------------------------------------------------------------------------------
// Definities voor Nodo unit nummer 1 
// Hardware    : Arduino Mega 2560, Ethernetkaart
// Toepassing  :
#if (UNIT_NODO == 1)
  #define NODO_MEGA                    1
  #define ETHERNET                     0 
  
  // Devices in gebruik op deze Nodo:
  #define DEVICE_CORE_01 // Kaku            : Klik-Aan-Klik-Uit / HomeEasy protocol ontvangst  
  #define DEVICE_CORE_02 // SendKaku        : Klik-Aan-Klik-Uit / HomeEasy protocol verzenden 
  #define DEVICE_CORE_03 // NewKAKU         : Klik-Aan-Klik-Uit ontvangst van signalen met automatische codering. Tevens bekend als Intertechno.
  #define DEVICE_CORE_04 // SendNewKAKU     : Klik-Aan-Klik-Uit ontvangst van signalen met automatische codering. Tevens bekend als Intertechno.

#endif 
// ----------------------------------------------------------------------------------------------------------------------------------------


// ----------------------------------------------------------------------------------------------------------------------------------------
// Definities voor Nodo unit nummer 15
// Hardware    : Arduino Duemilanove
// Toepassing  :
#if (UNIT_NODO == 15)

  // Devices in gebruik op deze Nodo:
  #define DEVICE_CORE_05 // TempRead        : Temperatuursensor Dallas DS18B20. (Let op; -B- variant, dus niet DS1820)

#endif 
// ----------------------------------------------------------------------------------------------------------------------------------------


// hieronder volgen nog enkele instellingen die u wilt laten gelden voor alle Nodo unit nummers
#define NODO_HOME                    0     // Home adres van Nodo's die tot één groep behoren (??? Nog niet operationeel!)
#define PULSE_DEBOUNCE_TIME         10     // pulsen kleiner dan deze waarde worden niet geteld. Bedoeld on verstoringen a.g.v. ruis of dender te voorkomen
#define PULSE_TRANSITION       FALLING     // FALLING of RISING: Geeft aan op welke flank de PulseCounter start start met tellen. Default FALLING
#define I2C_START_ADDRESS            1     // Alle Nodo's op de I2C bus hebben een uniek adres dat start vanaf dit nummer. Er zijn max. 32 Nodo's. Let op verlap met andere devices. RTC zit op adres 104.
#define NODO_30_COMPATIBLE        true     // t.b.v. compatibiliteit met vorige Nodo versie: NewKAKU HEX waarden en UserEvents
#define BAUD                     19200     // Baudrate voor seriële communicatie.
#define PASSWORD_MAX_RETRY           5     // aantal keren dat een gebruiker een foutief wachtwoord mag ingeven alvorens tijdslot in werking treedt
#define PASSWORD_TIMEOUT           300     // aantal seconden dat het terminal venster is geblokkeerd na foutive wachtwoord
#define TERMINAL_TIMEOUT           600     // Aantal seconden dat, na de laatst ontvangen regel, de terminalverbinding open mag staan.


//****************************** Einde gedeelte door gebruiker in te stellen ************************************************


