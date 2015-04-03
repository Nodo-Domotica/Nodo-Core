// ****************************************************************************************************************************************
// Definities voor Nodo units : CONFIG.C
// ****************************************************************************************************************************************
// Deel 1: geef hier op welke plugins er gebruikt worden.
// Voor een volledige en bijgewerkte lijst van alle beschikbare plugins verwijzen we naar de Wiki:
// http://www.nodo-domotica.nl/index.php
// ****************************************************************************************************************************************


#define UNIT_NODO                         10                                    // Na een reset wordt dit het unitnummer van de Nodo
#define NODO_MEGA                       true                                    // true=Nodo software voor op een ATMega2560
#define CFG_CLOCK                       true                                    // false=geen code voor Real Time Clock mee compileren. (Op Mega is meecompileren van Clock verplicht)
#define CFG_SOUND                       true                                    // false=geen luidspreker in gebruik.
#define CFG_WIRED                       true                                    // false=wired voorzieningen uitgeschakeld
#define CFG_I2C                         true                                    // false=I2C communicatie niet mee compileren (I2C plugins en klok blijvel wel gebruik maken van I2C)
#define CFG_SLEEP                      false                                    // false=Sleep mode mee compileren.
#define CFG_SERIAL                      true                                    // false=Seriele communicatie niet mee compileren. LET OP: hierdoor geen enkele weergave of input via seriele poort meer mogelijk!!! Alleen voor de Nodo-Small 
#define CFG_RAWSIGNAL                   true                                    // false=Rawsignal niet meecompileren. LET OP: Zowel RF als IR communicatie alsmede diverse plugins gebruiken RawSignal voorzieningen. Alleen voor de Nodo-Small
#define NODO_PORT_NRF24L01              true                                    // true=Ondersteuning voor tranciever module NRF24L01


#define PLUGIN_001                                                              // Kaku : Klik-Aan-Klik-Uit
#define PLUGIN_001_CORE

#define PLUGIN_002                                                              // NewKAKU : Klik-Aan-Klik-Uit ontvangst van signalen met automatische codering. Tevens bekend als Intertechno.
#define PLUGIN_002_CORE

#define PLUGIN_006                                                              // Vochtigheidssensor / Temperatuursensor DHT-22

#define PLUGIN_005                                                              // Temperatuursensor Dallas DS18B20

#define PLUGIN_020                                                              // Luchtdruksensor BPM085

#define PLUGIN_023                                                              // RGB-Led aansturing

