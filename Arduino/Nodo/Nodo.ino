/****************************************************************************************************************************\
* Arduino project "Nodo" © Copyright 2012 Paul Tonkes 
* This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License 
* as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*
* You received a copy of the GNU General Public License along with this program in tab '_COPYING'.
******************************************************************************************************************************

 Voor toelichting op de licentievoorwaarden zie    : http://www.gnu.org/licenses
 Uitgebreide documentatie is te vinden op          : http://www.nodo-domotica.nl
 bugs kunnen worden gelogd op                      : https://code.google.com/p/arduino-nodo/
 Compiler voor deze programmacode te downloaden op : http://arduino.cc
 Voor vragen of suggesties, mail naar              : p.k.tonkes@gmail.com
 Compiler                                          : Arduino Compiler met minimaal versie 1.0.1



\****************************** Door gebruiker in te stellen: ***************************************************************/

// De code kan worden gecompileerd als een Nodo-Small voor de Arduino met een ATMega328 processor of een Nodo-Mega met een ATMega1280/2560.
// stel allereerst het juiste board-type en COM-poort in op de Arduino compiler.
// en pas eventueel de drie include statements aan (zie onder)


#define USER_PLUGIN         "UserPlugin"                           // Commando naam waarmee de plugin kan worden aangeroepen. Remarken met // als UserPluging niet nodig is

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)     // NIET veranderen
  #define NODO_MEGA 1                                              // NIET veranderen !
  #define UNIT_NODO          1                                     // default unit nummer na een [Reset] commando
  #define NODO_MAC           0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF    // MAC adres voor de ethernet kaart. Remarken met // als ethernet niet nodig is


  // Keuze tussen SMALL of MEGA: **********************************************************************************************
  // Kies voor compilatie het juiste Arduino board. Dit kan een Arduino zijn gebaseerd op een zijn een ATMega1280, ATMega2560 of een ATMega328.
  // vanwege een bug in de Arduino compiler moeten daarnaast nog enkele regels handmatig worden aangepast!
  // Voor een arduino anders dan een Mega1280 of Mega2560 de onderstaande vier regels vooraf laten gaan door twee slashes //
  // Anders worden de niet gebruikte librariesmeegecompileerd en zal deze niet in een ATMega328 passen.
  #include <SD.h>
  #include <EthernetNodo.h>
  #include <SPI.h>
  #include <Arduino.h>
  
#else
  // definities Small
  #define UNIT_NODO    15                                    // default unit nummer na een [Reset] commando
#endif


// Onderstaand de formules die gebruikt worden voor omrekening van pulsen naar analoge waarden.
// Het bereik van het resultaat uit de formule moet zich tussen van -100 tot 100 bevinden.
// Deling door nul is ongeldig, maar zal niet tot een error leiden.
//
// f          = variabele met resultaat van de berekening.
// PulseTime  = tijd tussen twee pulsen uitgedrukt in milliseconden.
// PulseCount = Aantal pulsen tussen twee metingen.

#define FORMULA_1            f = 3600/PulseTime;            /* 1000 pulsen in een uur = 1KWh */
#define FORMULA_2            f = 2160/PulseTime;            /* 600 pulsen per uur = 1KWh */
#define FORMULA_3            f = PulseCount; PulseCount=0;
#define FORMULA_4            f = 0;
#define FORMULA_5            f = 0;
#define FORMULA_6            f = 0;
#define FORMULA_7            f = 0;
#define FORMULA_8            f = 0;

//****************************** Einde gedeelte door gebruiker in te stellen ************************************************


