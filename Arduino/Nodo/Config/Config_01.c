// ****************************************************************************************************************************************
// Definities voor Nodo units : CONFIG.C
// ****************************************************************************************************************************************
// Deel 1: geef hier op welke plugins er gebruikt worden.
// Voor een volledige en bijgewerkte lijst van alle beschikbare plugins verwijzen we naar de Wiki:
// http://www.nodo-domotica.nl/index.php
// ****************************************************************************************************************************************


#define UNIT_NODO        1 // Na een [Reset] commando wordt dit het unitnummer van de Nodo
#define CLOCK         true // true=code voor Real Time Clock mee compileren.
#define NODO_MEGA     true // true = Nodo Mega, false=Nodo-Small

// Kaku : Klik-Aan-Klik-Uit
#define PLUGIN_001
#define PLUGIN_001_CORE

// NewKAKU : Klik-Aan-Klik-Uit ontvangst van signalen met automatische codering. Tevens bekend als Intertechno.
#define PLUGIN_002
#define PLUGIN_002_CORE

// Vochtigheidssensor / Temperatuursensor DHT-22
#define PLUGIN_006

// Temperatuursensor Dallas DS18B20
#define PLUGIN_005

// Oude UserEvents Nodo Due compatibiliteit
#define PLUGIN_007
#define PLUGIN_007_CORE

//Luchtdruksensor BPM085
#define PLUGIN_020

// RGB-Led aansturing
#define PLUGIN_023
#define PLUGIN_023_CORE

