//??? *************** Checklist voor release uitbrengen ***********
//??? Zoeken naar comments met drie vraagtekens.
//??? DEBUG: Regels remarken
//??? Build nummer ophogen
//??? VERSION_MINOR aanpassen indien nodig.
//??? Config_01.c schonen en ethernet default op true
//??? Config_15.c aanpassen
//??? Alleen plugins 1..5 meeleveren
//??? Includes zonder remarks
//??? Tabblad debug.ino verwijderen
//??? Nieuwe libraries mee distribueren
//??? Final compilatie voor een Mega
//??? Final compilatie voor een Small
//??? 
//??? Deze checklist verwijderen
//??? 
//??? 



/****************************************************************************************************************************\
* Arduino project "Nodo" © Copyright 2014 Paul Tonkes 
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
* Compiler                                          : Arduino Compiler met minimaal versie 1.0.5
* Libraties                                         : EthernetNodo library vervangt de standaard Ethernet library!
\*************************************************************************************************************************/



// ============================================================================================================================
// Geef in onderstaande regel op waar de .ino bestanden zich bevinden die je nu geopend hebt. 
// Dit is nodig om device- en configuratiebestanden te kunnen compileren.
// Geef het volledige pad op. Let op: Bij Linux/Unix zijn de namen case-sensitive.
// ============================================================================================================================

#define SKETCH_PATH C:\Users\tonkes\Google Drive\Paul\Nodo\SVN\Arduino\Nodo
//???#define SKETCH_PATH C:\..\..\..\Nodo



// ============================================================================================================================
// We kennen twee type Nodo's:
//
// Nodo-Mega:   Een Nodo op basis van een Arduino Mega met een ATMega1280 of 2560 processor. Deze Nodo heeft o.a. de mogelijkheid
//              tot ethernet communicatie.
//
// Nodo-Small:  Dit is een kleine Nodo die wordt vooral wordt gebruikt als satelliet in combinatie met een centrale Nodo-Mega.
//              Een Nodo-Small maakt gebruik van een Arduino met een ATMega328 processor. (Nano, Pro,Duemillanove, Uno, etc)                                                                                                                
//
// Bij gebruik van meerdere Nodo's kan voor iedere Nodo een eigen configutatie file worden aangemaakt. In deze configuratie files
// kan worden aangegeven welke plugins worden gebruikt en kunnen eventueel speciale instellingen worden opgegeven die mee
// gaan met het compileren van de code.
// Configuratie bestanden bevinden zich in de directory ../Config. In deze configuratiefiles kunnen settings worden opgegeven 
// die worden meegecompileerd of kunnen devices worden opgegeven waar de Nodo mee moet kunnen communiceren.
// Default zijn de volgende configuratie files gemaakt:
//
// Config_01.c => Deze is default bestemd voor een Nodo Mega met unitnummer 1.
// Config_15.c => Deze is default bestemd voor een Nodo Small met unitnummer 15.
//
// Alle regels gemarkeerd met een '//' worden niet meegecompilileerd.
// 
// LET OP:
//  
// -  Het unitnummer van de Nodo zal pas veranderen nadat de Nodo software voor het eerst wordt geinstalleerd of de Nodo het 
//    commando [Reset] uitvoert. Dus niet altijd na een compilatie!
// -  Indien gewenst kunnen de config files ook voor andere unitnummers worden aangemaakt (1..31)
//
// ============================================================================================================================


// ================================= Unit-1: Configuratie voor de Nodo-Mega ===================================================
// #define CONFIG_FILE Config_10.c
// #include <SD.h>                                 // Deze include noodzakelijk voor een Nodo-Mega. Niet gebruiken voor een Small!
// #include <EthernetNodo.h>                       // Deze include optioneel als Ethernet wordt gebruikt. Niet gebruiken voor een Small!


// ================================= Unit-15: Configuratie voor de Nodo-Small =================================================
#define CONFIG_FILE Config_02.c

