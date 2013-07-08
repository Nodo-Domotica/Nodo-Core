
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
// Een Nodo krijg je werkend in de volgende hoofd stappen:
//
// Stap-1: Maak de instellingen voor een Arduino of Nodo op basis van een ATMega2560 of een ATMega328 (Uno, Nano, Duemillanove).
// Stap-2: Bepaal welke devices je wilt gebruiken.
// Stap-3: Upload deze code naar je Nodo / Arduino.
// 
// Tip: Maak je gebruik van meerdere Nodo's, dan kun je voor iedere Nodo een kopie maken van dit tabblad Nodo.ino of, voor de
//      gevorderde gebruiker, een Nodo.ino tabblad maken die a.d.h.v. een opgegeven unitnummer de juiste settings compileert.raadpleeg
//      voor verdere informatie over deze tip http://www.nodo-domotica/wiki
// ============================================================================================================================




// ======================================== Stap-1 ============================================================================
// Gebruik je een Arduino/Nodo Small op basis van een ATMega328 chip, plaats dan een "//" markeringen voorafgaand aan
// onderstaande 7 regels. Gebruik je een Arduino / Uno op basis van een ATMega2560 chip, dan hoef je in deze stap niets te doen.
// ============================================================================================================================

//#include <SPI.h>
//#include <Arduino.h>
//#include <SD.h>
//#include <NodoEthernet.h>
#define NODO_MEGA                    false  // true = Nodo Mega, false=Nodo-Small
#define ETHERNET                     false  // true = Deze Nodo beschikt over een ethernet kaart



// ======================================== Stap-2 ============================================================================
// De Nodo kan diverse devices (apparaten) aansturen of uitlezen. Hiervoor moet voor ieder device een eigen stuk programmacode worden 
// toegevoegd aan de Nodo code. Deze is meegeleverd met de Nodo code en bevindt zich in de map "Devices". Druk nu op ctrl-k om deze map
// weer te geven. Niet alle devices worden aangebden namens het Nodo team. kijk voor meer informatie op http://www.nodo-domotica.nl/wiki
// Neem voor ieder device dat je gebruikt de volgende drie regels op, waarbij xxx verwijst naar het device nummer:
// a) #define DEVICE_xxx_MMI   => Opnemen als je invoer/uitvoer van het device op je Mega wilt weergeven. LET OP: ALLEEN VOOR EEN MEGA!
// b) #define DEVICE_xxx_CORE  => Alleen opnemen als je Nodo gegevens van het device moet uitlezen, verwerken, versturen.
// c) #define DEVICE_xxx_FILE  => Verwijzing naar de bestandsnaam met de code van het device (Let op volledige pad en filenaam!).
//
// Onderstaand een overzicht van de beschikbare devices.
// 
// Device_000 => Clock             : Realtime clock RTC1307
// Device_001 => Kaku              : Klik-Aan-Klik-Uit / HomeEasy protocol ontvangst
// Device_002 => SendKaku          : Klik-Aan-Klik-Uit / HomeEasy protocol verzenden 
// Device_003 => NewKAKU           : Klik-Aan-Klik-Uit ontvangst van signalen met automatische codering. Tevens bekend als Intertechno.
// Device_004 => SendNewKAKU       : Klik-Aan-Klik-Uit ontvangst van signalen met automatische codering. Tevens bekend als Intertechno. 
// Device_005 => TempRead          : Uitlezen Dallas DS18B20 temperatuur sensor volgens OneWire verbinding.
// Device_006 => DHT11Read         : Uitlezen temperatuur & vochtigheidsgraad sensor DHT-11
// Device_007 => Reserved!         : UserEvents van de oude Nodo. t.b.v. compatibiliteit reeds geprogrammeerde universele afstandsbedieningen.
// Device_008 => AlectoV1          : Dit protocol zorgt voor ontvangst van Alecto weerstation buitensensoren met protocol V1
// Device_009 => AlectoV2          : Dit protocol zorgt voor ontvangst van Alecto weerstation buitensensoren met protocol V2
// Device_010 => AlectoV3          : Dit protocol zorgt voor ontvangst van Alecto weerstation buitensensoren met protocol V3
// Device_011 => Reserved!         : OpenTherm (SWACDE-11-V10)
// Device_012 => OregonV2          : Dit protocol zorgt voor ontvangst van Oregon buitensensoren met protocol versie V2
// Device_013 => FA20RF            : Dit protocol zorgt voor ontvangst van Flamingo FA20RF rookmelder signalen
// Device_014 => FA20RFSend        : Dit protocol zorgt voor aansturen van Flamingo FA20RF rookmelder
// Device_015 => HomeEasy          : Dit protocol zorgt voor ontvangst HomeEasy EU zenders die werken volgens de automatische codering (Ontvangers met leer-knop)
// Device_016 => HomeEasySend      : Dit protocol stuurt HomeEasy EU ontvangers aan die werken volgens de automatische codering (Ontvangers met leer-knop)
// Device_017 => WiredMonitor      : Laat continue analoge metingen zien van alle Wired-In poorten. 
// Device_018 => RawSignalAnalyze  : Geeft bij een binnenkomend signaal informatie over de pulsen weer. (SWACDE-18-V10)
// Device_019 => RFID              : Innovations ID-12 RFID Tag reader (SWACDE-19-V10) 
// Device_020 => Reserved!         : BMP085 Barometric pressure sensor (SWACDE-20-V10)
// Device_021 => LCDI2CWrite       : DFRobot LCD I2C/TWI 1602 display
// Device_022 => HCSR04_Read       : Ultrasoon afstandsmeter HC-SR04
// Device_023 => PWM               : LED-Dimmer PWM driver (SWACDE-18-V10)
// Device_255 => UserDevice        : Template Device voor eigen toepassing door gebruiker te bouwen.
// ============================================================================================================================


// Device_001 => Kaku              : Klik-Aan-Klik-Uit / HomeEasy protocol ontvangst
// #define DEVICE_001_MMI 
#define DEVICE_001_CORE
#define DEVICE_001_FILE "C:\Users\tonkes\Google Drive\Paul\Nodo\SVN\Arduino\Nodo\Devices\Device_001.c"

// Device_002 => SendKaku          : Klik-Aan-Klik-Uit / HomeEasy protocol verzenden
//#define DEVICE_002_MMI 
#define DEVICE_002_CORE
#define DEVICE_002_FILE "C:\Users\tonkes\Google Drive\Paul\Nodo\SVN\Arduino\Nodo\Devices\Device_002.c"

// Device_003 => NewKAKU           : Klik-Aan-Klik-Uit ontvangst van signalen met automatische codering. Tevens bekend als Intertechno.//
//#define DEVICE_003_MMI 
#define DEVICE_003_CORE
#define DEVICE_003_FILE "C:\Users\tonkes\Google Drive\Paul\Nodo\SVN\Arduino\Nodo\Devices\Device_003.c"

// Device_004 => SendNewKAKU       : Klik-Aan-Klik-Uit ontvangst van signalen met automatische codering. Tevens bekend als Intertechno. 
//#define DEVICE_004_MMI 
#define DEVICE_004_CORE
#define DEVICE_004_FILE "C:\Users\tonkes\Google Drive\Paul\Nodo\SVN\Arduino\Nodo\Devices\Device_004.c"

// Device_005 => TempRead          : Uitlezen Dallas DS18B20 temperatuur sensor volgens OneWire verbinding.
//#define DEVICE_005_MMI 
//#define DEVICE_005_CORE
//#define DEVICE_005_FILE "C:\Users\tonkes\Google Drive\Paul\Nodo\SVN\Arduino\Nodo\Devices\Device_005.c"

// Device_022 => reboo
//#define DEVICE_022_MMI 
#define DEVICE_022_CORE
#define DEVICE_022_FILE "C:\Users\tonkes\Google Drive\Paul\Nodo\SVN\Arduino\Nodo\Devices\Device_022.c"

// Device_023 => PWM Led dimmer
//#define DEVICE_023_MMI 
#define DEVICE_023_CORE
#define DEVICE_023_FILE "C:\Users\tonkes\Google Drive\Paul\Nodo\SVN\Arduino\Nodo\Devices\Device_023.c"




// ======================================== Stap-3 ============================================================================
// a) Sluit je Nodo/Arduino aan op je usb/com-poort. Het kan zijn dat uw computer niet beschikt over de juiste drivers, kijk hiervoor
//    op http://www.arduino.cc voor meer informatie. Kies de juiste seriele poort uit het menu [Extra/Serial port]
// b) Plaats de meegeleverde directory [NodoEthernet] in de [libraries] directory van de Arduino compiler.
// c) (her-)start de Arduino compiler en selecteer de juiste Nodo/Arduino via het menu [Extra/Board].
// d) Upload de code naar je Arduino [File/Upload]. Dit duurt enige minuten.
// e) De Nodo/Arduino is geprogrammeerd. Dit is te controleren met de seriele monitor [Extra/Serial monitor]. Kies onder in het 
//    venster: Baud 19200, anders zal er een reeks vreemde tekens verschijnen.
// ============================================================================================================================




// ======================================== Extra aanpassingen ================================================================
// De gevorderde gebruiker kan eventueel onderstaande settings naar wens aanpassen of een eigen versie worden gemaakt 
// van dit tabblad
// "Nodo.ino" 
// ============================================================================================================================

#if NODO_MEGA
#define UNIT_NODO                      1  // Nadat de Nodo wordt gereset, is dit het default unitnummer
#else
#define UNIT_NODO                     15  // Nadat de Nodo wordt gereset, is dit het default unitnummer
#endif

#define HOME_NODO                      1 // Home adres van Nodo's die tot één groep behoren (1..7)
#define PULSE_DEBOUNCE_TIME           10 // pulsen kleiner dan deze waarde in milliseconden worden niet geteld. Bedoeld om verstoringen a.g.v. ruis of dender te voorkomen
#define PULSE_TRANSITION         FALLING // FALLING of RISING: Geeft aan op welke flank de PulseCounter start start met tellen. Default FALLING
#define I2C_START_ADDRESS              1 // Alle Nodo's op de I2C bus hebben een uniek adres dat start vanaf dit nummer. Er zijn max. 32 Nodo's. Let op overlap met andere devices. RTC zit op adres 104.
#define NODO_30_COMPATIBLE          true // t.b.v. compatibiliteit met vorige Nodo versie: NewKAKU HEX waarden en UserEvents
#define BAUD                       19200 // Baudrate voor seriële communicatie.
#define WAIT_FREE_RX               false // true: wacht default op verzenden van een event tot de IR/RF lijn onbezet is. Wordt overruled door commando [WaitFreeRX]
#define WAIT_FREE_RX_WINDOW          500 // minimale wachttijd wanneer wordt gewacht op een vrije RF of IR band. Is deze waarde te klein, dan kunnen er restanten van signalen binnenkomen als RawSignal. Te groot maakt de Nodo sloom.
#define WAITFREE_TIMEOUT           30000 // tijd in ms. waarna het wachten wordt afgebroken als er geen ruimte in de vrije ether komt
#define PASSWORD_MAX_RETRY             5 // aantal keren dat een gebruiker een foutief wachtwoord mag ingeven alvorens tijdslot in werking treedt
#define PASSWORD_TIMEOUT             300 // aantal seconden dat het terminal venster is geblokkeerd na foutive wachtwoord
#define TERMINAL_TIMEOUT             600 // Aantal seconden dat, na de laatst ontvangen regel, de terminalverbinding open mag staan.
#define DELAY_BETWEEN_TRANSMISSIONS  500 // Minimale tijd tussen verzenden van twee events. Geeft ontvangende apparaten (en Nodo's) verwerkingstijd.
#define NODO_TX_TO_RX_SWITCH_TIME    500 // Tijd die andere Nodo's nodig hebben om na zenden weer gereed voor ontvangst te staan. (Opstarttijd 433RX modules)
byte dummy=1;

