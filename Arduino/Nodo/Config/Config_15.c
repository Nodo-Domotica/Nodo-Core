
// ****************************************************************************************************************************************
// Definities voor Nodo units : CONFIG.C
// ****************************************************************************************************************************************
// Deel 1: geef hier op welke devices er gebruikt worden.
// Voor een volledige, en bijgewerkte lijst van alle beschikbare devices verwijzen we naar de Wiki:
// http://www.nodo-domotica.nl/index.php/Device
// ****************************************************************************************************************************************

#define UNIT_NODO       15 // Na een reset wordt dit het unitnummer van de Nodo
#define CLOCK        false // true=code om Real Time Clock mee compileren.
#define NODO_MEGA    false // true = Nodo Mega, false=Nodo-Small

// Kaku              : Klik-Aan-Klik-Uit
// #define PLUGIN_001
// #define PLUGIN_001_CORE

// NewKAKU           : Klik-Aan-Klik-Uit met automatische codering. Tevens bekend als Intertechno.
// #define PLUGIN_003
// #define PLUGIN_003_CORE

#define PLUGIN_023
#define PLUGIN_023_CORE
#define PLUGIN_023_CORE_RGBLED
