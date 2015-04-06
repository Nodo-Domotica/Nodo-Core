
// ****************************************************************************************************************************************
// Definities voor Nodo units : CONFIG.C
// ****************************************************************************************************************************************
// Deel 1: geef hier op welke devices er gebruikt worden.
// Voor een volledige, en bijgewerkte lijst van alle beschikbare devices verwijzen we naar de Wiki:
// http://www.nodo-domotica.nl/index.php/Device
// ****************************************************************************************************************************************



#define UNIT_NODO                          4                                    // Na een reset wordt dit het unitnummer van de Nodo

#define CFG_CLOCK                      false                                    // false=geen code voor Real Time Clock mee compileren. (Op Mega is meecompileren van Clock verplicht)
#define CFG_SOUND                       true                                    // false=geen luidspreker in gebruik.
#define CFG_WIRED                       true                                    // false=wired voorzieningen uitgeschakeld
#define CFG_I2C                        false                                    // false=I2C communicatie niet mee compileren (I2C plugins en klok blijvel wel gebruik maken van I2C)
#define CFG_SLEEP                      false                                    // false=Sleep mode mee compileren.
#define CFG_SERIAL                      true                                    // false=Seriele communicatie niet mee compileren. LET OP: hierdoor geen enkele weergave of input via seriele poort meer mogelijk!!! Alleen voor de Nodo-Small 
#define CFG_RAWSIGNAL                   true                                    // false=Rawsignal niet meecompileren. LET OP: Zowel RF als IR communicatie alsmede diverse plugins gebruiken RawSignal voorzieningen. Alleen voor de Nodo-Small


// Kaku: Klik-Aan-Klik-Uit
// #define PLUGIN_001
// #define PLUGIN_001_CORE

// NewKAKU: Klik-Aan-Klik-Uit met automatische codering. Tevens bekend als Intertechno.
// #define PLUGIN_002
// #define PLUGIN_002_CORE

// NRF24L01 Plugin
// #define PLUGIN_033                                                              
// #define PLUGIN_033_CORE

// Temperatuursensor Dallas DS18B20
#define PLUGIN_005
#define PLUGIN_005_CORE

// DTH-22 Vocht/Temperatuursensor
// #define PLUGIN_006
// #define PLUGIN_006_CORE 22

// BMP085 Luchtdruk sensor
// #define PLUGIN_020
// #define PLUGIN_020_CORE 

// RGB-Led aansturing
// #define PLUGIN_023
// #define PLUGIN_023_CORE
// #define PLUGIN_023_CORE_RGBLED
