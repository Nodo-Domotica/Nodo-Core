// *************************************************************************************************
// Config bestand voor:      Nodo Small op basis van Arduino Uno / Duemillanove / Nano-V3 / ProMini
//                           en klonen op basis van een Atmel ATMega328 chip. 
//                           Er kan gekozen kan worden uit communicatie via 433Mhz modules of 2.4Ghz 
//                           via een NRF24L01 module. Zeer geschikt als simpele sensor, RGB-led
//                           aansturing of als waarbij de Nodo een compenent is in een grotere 
//                           schakeling.
//
//                           LET OP: 2e generatie Nodo penbezetting! Niet gebruiken voor een NES of oudere Nodo 
// 
// Voor een volledige en bijgewerkte lijst van alle beschikbare plugins verwijzen we 
// naar de Wiki. Een plugin wordt als volgt opgegeven:
//
// #define PLUGIN_nnn:       is nodig om de plugin bekend te maken aan de Nodo zodat events
//                           kunnen worden weergegeven en eventlists geprogrammeerd kunnen
//                           worden. Neem voor alle plugins die je gebruikt in ieder geval
//                           in je centrale Nodo-Mega (die b.v. met de WebApp communiceert)
//                           deze regel op.
//
// #define PLUGIN_nnn_CORE:  Deze regel is alleen nodig voor de Nodo waar daadwerkelijk de
//                           plugin zijn werk moet doen, zoals bijvoorbeeld het meten van
//                           waarden van sensoren. 
//
//
// **************************************************************************************************


#define HARDWARE_CONFIG                 2502                                    // Nodo-Small met 2e generatie penbezetting met NRF24L01 

#define UNIT_NODO                         20                                    // Na een reset-commando wordt dit het unitnummer van de Nodo


// Temperatuursensor Dallas DS18B20
// #define PLUGIN_005
// #define PLUGIN_005_CORE

// DTH-22 Vocht/Temperatuursensor
// #define PLUGIN_006
// #define PLUGIN_006_CORE 22 // 22=DHT-22 sensor, 11=DHT-11 sensor

// BMP085 Luchtdruk sensor
// #define PLUGIN_020
// #define PLUGIN_020_CORE 

// RGB-Led aansturing
// #define PLUGIN_023
// #define PLUGIN_023_CORE
// #define PLUGIN_023_CORE_RGBLED

