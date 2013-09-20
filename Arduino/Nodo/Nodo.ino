
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



// ============================================================================================================================
// Geef in onderstaande regel op waar de .ino bestanden zich bevinden die je nu geopend hebt. 
// Dit is nodig om device- en configuratiebestanden te kunnen compileren.
// Geef het volledige pad op. Let op: Bij Linux/Unix zijn de namen case-sensitive.
// ============================================================================================================================
#define SKETCH_PATH C:/Users/tonkes/Google Drive/Paul/Nodo/SVN/Arduino/Nodo


// ============================================================================================================================
// Bij gebruik van meerdere Nodo's kan voor iedere Nodo een eigen configutatie file worden aangemaakt. Deze
// files bevinden zich in de directory ../Config. In deze configuratiefiles kunnen settings worden opgegeven die worden 
// meegecompileerd of kunnen devices worden opgegeven waar de Nodo mee moet kunnen communiceren.
// Default zijn de volgende configuratie files gemaakt:
//
// Config_01.c => Deze is default bestemd voor een Nodo Mega met unitnummer 1.
// Config_15.c => Deze is default bestemd voor een Nodo Small met unitnummer 15.
// ============================================================================================================================
#define CONFIG_FILE Config_01.c


// ============================================================================================================================
// Gebruik je een Arduino-Uno/Duemillanove met daarop de code voor een Nodo-Small, plaats dan een "//" markeringen voorafgaand aan
// onderstaande twee regels. Gebruik je een Arduino-Mega op basis van een ATMega2560 chip, dan hoef je in deze stap niets te doen.
// Dit omdat een 'onhebbelijkheid' in de Arduino compiler anders de library altijd meecompileerd waardoor deze niet meer in een ATMeg328 past.
// ============================================================================================================================
#include <SD.h>
#include <NodoEthernet.h>


