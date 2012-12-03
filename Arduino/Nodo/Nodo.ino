void setup();
int dummy;
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
 Libraties                                         : EthernetNodo library vervang de standaard Ethernet library!
  
\****************************** Door gebruiker in te stellen: ***************************************************************/

// De code kan worden gecompileerd als een Nodo-Small voor de Arduino met een ATMega328 processor of een Nodo-Mega met een ATMega1280/2560.
// stel allereerst het juiste board-type en COM-poort in op de Arduino compiler.
// en pas eventueel de drie include statements aan (zie onder)


#define USER_PLUGIN       "UserPlugin"                             // Commando naam waarmee de plugin kan worden aangeroepen. Remarken met // als UserPluging niet nodig is
#define PULSE_TIME_DIVIDE            1                             // PulseTime geeft waarden van 0..65535 milliseconden. Door de deelfactor te vergroten kan de tijdseenheid worden vergroot. 1000-seconden
#define PULSE_DEBOUNCE_TIME         10                             // pulsen kleiner dan deze waarde worden niet geteld. Bedoeld on verstoringen a.g.v. ruis of dender te voorkomen
#define PULSE_TRANSITION       FALLING                             // FALLING of RISING: Geeft aan op welke flank de PulseCounter start start met tellen. Default FALLING


#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)     // NIET veranderen
  #define NODO_MEGA          1                                     // NIET veranderen !
  #define UNIT_NODO          1                                     // default unit nummer na een [Reset] commando
  #define ETHERNET           1                                     // op 0 zetten als ethernet kaart niet aanwezig
  

  // Keuze tussen SMALL of MEGA: **********************************************************************************************
  // Kies voor compilatie het juiste Arduino board. Dit kan een Arduino zijn gebaseerd op een zijn een ATMega1280, ATMega2560 of een ATMega328.
  // vanwege een bug in de Arduino compiler moeten daarnaast nog enkele regels handmatig worden aangepast!
  // Voor een arduino anders dan een Mega1280 of Mega2560 de onderstaande vier regels vooraf laten gaan door twee slashes //
  // Anders worden de niet gebruikte libraries meegecompileerd en zal deze niet in een ATMega328 passen.

  #include <SD.h>
  #include <EthernetNodo.h>
  #include <SPI.h>
  #include <Arduino.h>
  
#else
  // definities Small
  #define UNIT_NODO    15                                    // default unit nummer na een [Reset] commando
#endif


//****************************** Einde gedeelte door gebruiker in te stellen ************************************************


