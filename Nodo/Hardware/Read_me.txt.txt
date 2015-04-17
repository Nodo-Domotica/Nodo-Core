De Nodo software ondersteunt verschillende hardware configuraties. De penbezetting van
deze configuratues kunnen van elkaar afwijken. Het niet mogelijk om alle pinnen vrij
te definieren. Dit omdat aan de Atmel pinnen vaar ook nog onderliggende technische
funkties hebben (IRQ, RX,TX,SPI, I2C, etc).Daarom is gebruik gemaakt van een aantal
standaard configuraties.

Afhankelijk van de hardware en penbezetting, kan de Nodo beschikken over de verschillende
voorzieningen. Kies altijd de juiste hardware configuratie omdat anders risico is voor 
beschadiging van hardware of niet/slecht funktioneren van de Nodo wat tot een zoektocht
kan leiden.


Toelichting software varianten:
|=======================================================================================================================
| Nodo-Mega:  Dit is de uitgebreide Nodo variant die draait op een ATMega2560. Deze Nodo wordt ingezet als centrale    |
|             Nodo die b.v. gegevens van andere Nodo's verzameld en doorstuurd naar de WebApp. Deze Nodo heeft ook     |
|             qua hardware- en software meeste mogelijkheden.                                                          |
|                                                                                                                      |
|----------------------------------------------------------------------------------------------------------------------|
| Nodo-Small: Dit is een kleinere Nodo variant, die vooral wordt ingezet als slave voor het verzamelen van meetwaarden |
|             of aansturen van apparaten/motoren/relais/verlichting/etc. Deze software variant draait op de arduino's: | 
|             Duemillanove / Uno / ProMini / Nano-V3 en andere boards gebaseerd op een ATMega328 chip.                 |
|             Let Op: de Small variant is installeerbaar op een Arduino Mega2560, maar zal niet naar behoren           |
|             functioneren.                                                                                            |
|======================================================================================================================|



Toelichting Hardware configuraties:


|================================== 1e generatie Nodo penbezetting=====================================================|
| 1000: Bestemd voor alle Arduino Mega's met standaard Nodo Ethernet Shields, een Wiznet Ethernetkaart of een eigen    |
|       ontwerp Nodo die op deze penbezetting is gebaseerd. Tevens is voorzien in aansluiting van een NRF24L01 voor    |
|       Nodo-2-Nodo communicatie opr 2.4Ghz. Deze wordt aangesloten op de analoge pennen A0..A3                        |
|                                                                                                                      |
|----------------------------------------------------------------------------------------------------------------------|
| 1001: Bestemd voor alle Arduino Mega's met standaard Nodo Ethernet Shields, een Wiznet Ethernetkaart of een eigen    |
|       ontwerp Nodo die op deze penbezetting is gebaseerd. Tevens is voorzien in aansluiting van een NRF24L01 voor    |
|       Nodo-2-Nodo communicatie opr 2.4Ghz. Deze wordt aangesloten op de analoge pennen A12..A15 op de 26P connector. |
|                                                                                                                      |
|----------------------------------------------------------------------------------------------------------------------|
| 1500: Voor alle Nodo Uno/Due gebaseerd op een ATMega328 zoals de Arduino Duemillanove en Nano met een Nodo shield of |
|       alle (zelf gemaakte) Nodo's die zijn gebaseerd op dit schema. Ook kunnen de Nano-V3 en de Pro-Mini worden      |
|       gebruikt.                                                                                                      |
|                                                                                                                      |
|----------------------------------------------------------------------------------------------------------------------|
| 1501: Komt overeen met de 1500, met uitzondering dat de realtimeklok achterwege is gelaten en dat er een 'sleep'-    |
|       mode is toegevoegd waarmee de Nodo in een energiespaarstand kan worden gezet bij gebruik van batterijvoeding.  |
|                                                                                                                      |
|----------------------------------------------------------------------------------------------------------------------|
| 1502: Identiek aan 1500, echter met 2.4Ghz voor Nodo-2-Nodo communicatie op de A0..A3 lijnen. De WiredIn lijnen      |
|       zijn hiervoor ingezet en dus niet meer beschikbaar                                                                                                                     |
|----------------------------------------------------------------------------------------------------------------------|
| 1510  'bare minimum variant' van de 1502, met 2.4Ghz communicatie voor Nodo-2-Nodo communicatie. Deze kan worden     |
|       toegepast daar waar een kleine Nodo als component deel uit gaat maken van een schakeling en zo veel mogelijk   |
|       ruimte (geheugen en pinnen) voor additionele plugins.                                                          | 
|                                                                                                                      |
|======================================================================================================================|



|================================== 2e generatie Nodo penbezetting=====================================================|
| 2000: 2e generatie Nodo-penbezetting voor Arduino ATMega's 2560. De nieuwe penbezetting maakt meer mogelijkheden     |
|       beschikbaar. Maak je een eigen Nodo-Mega, gebruik dan bij voorkeur deze hardware configuratie.                 |
|       Deze hardware-configuratie wordt tevens gebruikt voor de (nog te ontwikkelen) All-in-On Nodo waar zowel de     |
|       ATMega2560 als de Ethernet W5100 chip op één bord zitten.                                                      | 
|                                                                                                                      |
|----------------------------------------------------------------------------------------------------------------------|
| 2010: Komt overeen met de 2000 met als verschil dat dit een 'bare-minimum' variant is met alleen ondersteuning voor  |
|       2.4Ghz communicatie met een NRF24L01 en de WiredIn/WiredOut lijnen.                                            | 
|                                                                                                                      |
|----------------------------------------------------------------------------------------------------------------------|
| 2500: 2e generatie penbezetting voor alle Arduino's gebaseerd op een ATMega328 zoals de Arduino Duemillanove         |
|       en Nano-V2, ProMini of zelf gemaakte Nodo's die zijn gebaseerd op dit schema Extra zijn de RX/TX lijnen en     |
|       drie PWM uitgangen in de wired lijnen voor aanturen van RGB-Led (strips).                                      |
|                                                                                                                      |
|----------------------------------------------------------------------------------------------------------------------|
| 2501  2e generatie penbezetting. Zonder klok en zonder luidspreker, maar met een extra RX/TX lijn die in             |
|       plugins kan worden gebruikt voor seriele communicatie.                                                         |
|                                                                                                                      |
|----------------------------------------------------------------------------------------------------------------------|
| 2502  Identiek aan 2500, echter in plaats van 433Mhz ondersteuning wordt gebruik gemaakt van de NRF24L01 module voor |
|       Nodo-2-Nodo communicatie via 2.4Ghz.                                                                           |
|                                                                                                                      |
|----------------------------------------------------------------------------------------------------------------------|
| 2503  Identiek aan 2501, echter in plaats van 433Mhz ondersteuning wordt gebruik gemaakt van de NRF24L01 module voor |
|       Nodo-2-Nodo communicatie via 2.4Ghz.                                                                           |
|                                                                                                                      |
|----------------------------------------------------------------------------------------------------------------------|
| 2510  'bare minimum variant' van de 2500, met 2.4Ghz communicatie voor Nodo-2-Nodo communicatie. Deze kan worden     |
|       toegepast daar waar een kleine Nodo als component deel uit gaat maken van een schakeling en zo veel mogelijk   |
|       ruimte (geheugen en pinnen) voor additionele plugins.                                                          | 
|                                                                                                                      |
|======================================================================================================================|



|=========================================|
| V  = Ondersteund                        |
|(V) = Ondersteund, maar niet aanbevolen  |
| *  = Opmerking                          |   
|                                         |
|=====================================================================================================================================
|Hardware ondersteuning:            File: | 1000 | 1001 | 1500 | 1501 | 1502 | 1510 | 2000 | 2010 | 2500 | 2501 | 2502 | 2503 | 2510 |
|=====================================================================================================================================
|Arduino Uno/Duemillanove/NanoV3/ATMega328|      |      |  V   |  V   |  V   |  V   |      |      |  V   |  V   |  V   |  V   |  V   |
|-------------------------------------------------------------------------------------------------------------------------------------
|Arduino Mega 2560                        |  V   |  V   |      |      |      |      |  V   |  V   |      |      |      |      |      |
|-------------------------------------------------------------------------------------------------------------------------------------
|Arduino ProMini (16Mhz)                  |      |      |  V   |  V   |  V   |  V   |      |      |  V   |  V   |  V   |  V   |  V   |
|=====================================================================================================================================
|HWMESH_NES_Vxx (Nodo Ethernet Shield)    |  V   |  V   |      |      |      |      |      |      |      |      |      |      |      |
|-------------------------------------------------------------------------------------------------------------------------------------
|HWMESH_NCS_Vxx (Nodo koppelprint)        |  V   |  V   |      |      |      |      |      |      |      |      |      |      |      |
|-------------------------------------------------------------------------------------------------------------------------------------
|HWSMSH_NSH_Vxx (Nodo Shield)             |      |      |  V   | (V)  |  V   |      |      |      |      |      |      |      |      |
|-------------------------------------------------------------------------------------------------------------------------------------
|HWSMPL_NA_Vxx  (Nodo Arduino Small)      |      |      |  V   | (V)  |  V   |      |      |      |      |      |      |      |      |
|-------------------------------------------------------------------------------------------------------------------------------------
|HWMIPL_NMI_Vxx (Nodo Mini)               |      |      | (V)  |  V   | (V)  |      |      |      |      |      |      |      |      |
|-------------------------------------------------------------------------------------------------------------------------------------
|HWMEPL_NME_Vxx (Nodo Mega)               |      |      |      |      |      |      |  V   |      |      |      |      |      |      |
|-------------------------------------------------------------------------------------------------------------------------------------
|HWMIPL_PRO_Vxx (ProMini)                 |      |      |  V   |  V   |  V   |  V   |      |      |  V   |  V   |  V   |  V   |  V   |
|=====================================================================================================================================
|Nodo pinout 1e generatie                 |  V   |  V   |  V   |  V   |  V   |  V   |      |      |      |      |      |      |      |
|-------------------------------------------------------------------------------------------------------------------------------------
|Nodo pinout 2e generatie                 |      |      |      |      |      |      |  V   |  V   |  V   |  V   |  V   |  V   |  V   |
|=====================================================================================================================================
|Nodo Mega software variant               |  V   |  V   |      |      |      |      |  V   |  V   |      |      |      |      |      |
|-------------------------------------------------------------------------------------------------------------------------------------
|Nodo Small software variant              |      |      |  V   |  V   |  V   |  V   |      |      |  V   |  V   |  V   |  V   |  V   |
|=====================================================================================================================================
|Wired-In poorten                         |  8   |  8   |  4   |  4   |      |      |  8   |  8   |  4   |  4   |  4   |  4   |      |
|-------------------------------------------------------------------------------------------------------------------------------------
|Wired-Out poorten                        |  8   |  8   |  4   |  4   |  4   |  4   |  8   |  8   |  4   |  4   |  4   |  4   |      |
|-------------------------------------------------------------------------------------------------------------------------------------
|PWM op WiredOut                          |  0   |  0   | 2(*7)| 2(*7)| 2(*7)| 2(*7)|  0   |  0   |  3   |  3   |  3   |  3   |      |
|-------------------------------------------------------------------------------------------------------------------------------------
|Statusled RGB                            | V(*8)|V(*8) |      |      |      |      |  V   |      |      |      |      |      |      |
|-------------------------------------------------------------------------------------------------------------------------------------
|Statusled                                |      |      |  V   |      |  V   |      |      |      |  V   |  V   |  V   |  V   |      |
|-------------------------------------------------------------------------------------------------------------------------------------
|Luidspreker                              |  V   |  V   |  V   |      |  V   |      |  V   |      |  V   |      |  V   |      |      |
|-------------------------------------------------------------------------------------------------------------------------------------
|Pulsen teller                            |  *1  |  *1  |  *1  |  V   |  *1  |   V  |  V   |      |  V   |  V   |  V   |  V   |      |
|-------------------------------------------------------------------------------------------------------------------------------------
|Sleep mode voor batterij (*9)            |      |      |      |  V   |      |      |      |      |      |      |      |      |      |
|-------------------------------------------------------------------------------------------------------------------------------------
|Rawsignal                                |  *3  |  *3  |  *3  |  *3  |  *3  |      |  V   |      | *3   |  *3  |  *3  |  *3  |      |
|-------------------------------------------------------------------------------------------------------------------------------------
|Infrarood                                |  V   |  V   |  V   |      |  V   |      | *10  |      |      |      |      |      |      |
|-------------------------------------------------------------------------------------------------------------------------------------
|RF 433Mhz                                |  V   |  V   |  V   |  V   |  V   |      |  V   |      |  V   |  V   |      |      |      |
|-------------------------------------------------------------------------------------------------------------------------------------
|RF 2.4 Ghz via SPI-poort en NRF24L01     |  V   |  V   |      |      |  V   |  V   |  V   |      |      |      |  V   |  V   |  V   |
|-------------------------------------------------------------------------------------------------------------------------------------
|Seriale poort USB/FTDI (*4)              |  V   |  V   |  V   |  V   |  V   |  V   |  V   |      |  V   |  V   |  V   |  V   |  V   |
|-------------------------------------------------------------------------------------------------------------------------------------
|Additionele seriele poort (SoftSerial)   |      |      |      |      |      |      |  V   |      |      |  V   |      |      |      |
|-------------------------------------------------------------------------------------------------------------------------------------
|Additionele seriele poort (Hardware)     |      |      |      |      |      |      |  V   |      |      |      |      |  V   |      |
|-------------------------------------------------------------------------------------------------------------------------------------
|Realtime clock (DS1307)                  |  V   |  V   |  V   |      |  V   |      |  V   |      |  V   |      |      |      |      |
|-------------------------------------------------------------------------------------------------------------------------------------
|I2C                                      |  V   |  V   |  V   |  V   |  V   |  V   |  V   |  V   |  V   |  V   |  V   |  V   |  V   |
|-------------------------------------------------------------------------------------------------------------------------------------
|SDCard                                   |  V   |  V   |      |      |      |      |  V   |      |      |      |      |      |      |
|-------------------------------------------------------------------------------------------------------------------------------------
|Ethernet (Wiznet W5100) (*5)             |  V   |  V   |      |      |      |      |  V   |      |      |      |      |      |      |
|-------------------------------------------------------------------------------------------------------------------------------------
|SPI (Software)                           |  *2  |  *6  |      |      |  V      V    |      |      |      |      |      |      |      |
|-------------------------------------------------------------------------------------------------------------------------------------
|SPI (Hardware)                           |      |      |      |      |      |      | *11  | *11  |      |      |      |      |      |
|-------------------------------------------------------------------------------------------------------------------------------------
|Buttons voor events                      |      |      |      |      |      |      |  V   |      |      |      |      |      |      |
|-------------------------------------------------------------------------------------------------------------------------------------
|Reset factory button                     |      |      |      |      |      |      |  V   |      |      |      |      |      |      |
|=====================================================================================================================================
| 1* keuze: Infrarood TSOP of pulsen tellen. Dit i.v.m. gedeelde lijn. Op NES een jumper.                       |
| 2* Op pennen A0..A3 zodat geen WiredIn lijnen ingeleverd worden. Modificatie op NES is eenvoudig.             |                                                                                                |
| 3* Toegewezen aan 433Mhz / Infrarood. Niet als separate poort naar buiten gevoerd.                            |
| 4* Dit is de standaard primaire seriele poort voor code upload en als Nodo terminal.                          |
| 5* Standaard Arduino Ethernet controller of WizNet W5100 chip volgens zelfde schema.                          |
| 6* SPI op WiredIn 5..8 (A12..15). Deze ook beschikbaar op de 26P connector van de NES.                        |
| 7* De IR-Led kan worden verwijderd en worden gebruikt als 3e PWM uitgang. Dit is dan WiredOut-5.              |
| 8* Alleen Rood, Groen, Blauw. Geen PWM dus geen tussenliggende kleuren.                                       |
| 9* Voor standaard Arduino's moet spanningsregelaar worden verwijderd.                                         |
|10* Hier kan de RawSignal poort voor worden gebruikt.                                                          |
|11* Drie devices. CSN-0 default voor communictiemodule NRF24L01. CSN-1 en SCN-2 zijn vrij.                     |
|                                                                                                               |
|================================================================================================================






                                                                                                
                                                                                                  
                                                                                                  
                                                                                                  