// ****************************************************************************************************************************************
// Definities voor Nodo units : CONFIG.C
// ****************************************************************************************************************************************
// Deel 1: geef hier op welke plugins er gebruikt worden.
// Voor een volledige en bijgewerkte lijst van alle beschikbare plugins verwijzen we naar de Wiki:
// http://www.nodo-domotica.nl/index.php
// ****************************************************************************************************************************************


#define NODO_MEGA              true                                             // true = Nodo Mega, false=Nodo-Small
#define UNIT_NODO                 1                                             // LET OP: Alleen na een [Reset] commando wordt dit het unitnummer van de Nodo, dus niet direct bij upload van deze code!

#define PLUGIN_001                                                              // Kaku : Klik-Aan-Klik-Uit
#define PLUGIN_001_CORE

#define PLUGIN_002                                                              // NewKAKU : Klik-Aan-Klik-Uit ontvangst van signalen met automatische codering. Tevens bekend als Intertechno.
#define PLUGIN_002_CORE

#define PLUGIN_006                                                              // Vochtigheidssensor / Temperatuursensor DHT-22

#define PLUGIN_005                                                              // Temperatuursensor Dallas DS18B20

#define PLUGIN_020                                                              // Luchtdruksensor BPM085

#define PLUGIN_023                                                              // RGB-Led aansturing

