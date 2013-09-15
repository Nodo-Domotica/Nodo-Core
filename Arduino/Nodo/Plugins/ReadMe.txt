=====================================================================================================================================
Arduino project "Nodo" © Copyright 2013 Paul Tonkes 

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License 
as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You received a copy of the GNU General Public License along with this program in file 'COPYING.TXT'.

Voor toelichting op de licentievoorwaarden zie    : http://www.gnu.org/licenses
Uitgebreide documentatie is te vinden op          : http://www.nodo-domotica.nl
bugs kunnen worden gelogd op                      : https://code.google.com/p/arduino-nodo/
Compiler voor deze programmacode te downloaden op : http://arduino.cc
Voor vragen of suggesties, mail naar              : p.k.tonkes@gmail.com
=====================================================================================================================================
                                                                                      
In deze directory horen de Nodo devices die als plugin kunnen worden meegecompileerd. Alle devices hebben een
stuk programmacode en een uniek nummer. Zie www.nodo-domotica.nl/wiki voor meer informatie.

De programmacode bevindt zich in file "Device_nnn", waarbij nnn staat voor
het nummer van het device. Om een device mee te compileren moeten de volgende stappen worden uitgevoerd:


1. ??? nog kort beschrijven.
2.
3.
4.
  
=====================================================================================================================================
Beschikbare devices:

001 Kaku              : Klik-Aan-Klik-Uit / HomeEasy protocol ontvangst
002 SendKaku          : Klik-Aan-Klik-Uit / HomeEasy protocol verzenden 
003 NewKAKU           : Klik-Aan-Klik-Uit ontvangst van signalen met automatische codering. Tevens bekend als Intertechno.
004 SendNewKAKU       : Klik-Aan-Klik-Uit ontvangst van signalen met automatische codering. Tevens bekend als Intertechno. 
005 TempSensor        : Temperatuursensor Dallas DS18B20. (Let op; -B- variant, dus niet DS1820)
006 DHT11Read         : Uitlezen temperatuur & vochtigheidsgraad sensor DHT-11
007 Reserved!         : UserEvents van de oude Nodo. t.b.v. compatibiliteit reeds geprogrammeerde universele afstandsbedieningen.
008 AlectoV1          : Dit protocol zorgt voor ontvangst van Alecto weerstation buitensensoren met protocol V1
009 AlectoV2          : Dit protocol zorgt voor ontvangst van Alecto weerstation buitensensoren met protocol V2
010 AlectoV3          : Dit protocol zorgt voor ontvangst van Alecto weerstation buitensensoren met protocol V3
011 Reserved!         : OpenTherm (SWACDE-11-V10)
012 OregonV2          : Dit protocol zorgt voor ontvangst van Oregon buitensensoren met protocol versie V2
013 FA20RF            : Dit protocol zorgt voor ontvangst van Flamingo FA20RF rookmelder signalen
014 FA20RFSend        : Dit protocol zorgt voor aansturen van Flamingo FA20RF rookmelder
015 HomeEasy          : Dit protocol zorgt voor ontvangst HomeEasy EU zenders die werken volgens de automatische codering (Ontvangers met leer-knop)
016 HomeEasySend      : Dit protocol stuurt HomeEasy EU ontvangers aan die werken volgens de automatische codering (Ontvangers met leer-knop)
017 Reserved!         : Laat continue analoge metingen zien van alle Wired-In poorten. 
018 RawSignalAnalyze  : Geeft bij een binnenkomend signaal informatie over de pulsen weer. (SWACDE-18-V10)
019 RFID              : Innovations ID-12 RFID Tag reader (SWACDE-19-V10) 
020 Reserved!         : BMP085 Barometric pressure sensor (SWACDE-20-V10)
021 LCDI2CWrite       : DFRobot LCD I2C/TWI 1602 display
022 HCSR04_Read       : Ultrasoon afstandsmeter HC-SR04
023 PWM               : LED-Dimmer PWM driver (SWACDE-18-V10)
024 
099 UserDevice        : Device voor eigen toepassing door gebruiker te bouwen.

=====================================================================================================================================


=====================================================================================================================================

