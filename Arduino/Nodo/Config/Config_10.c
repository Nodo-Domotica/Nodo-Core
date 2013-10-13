// ****************************************************************************************************************************************
// Definities voor Nodo units : CONFIG.C
// ****************************************************************************************************************************************
// Deel 1: geef hier op welke devices er gebruikt worden.
// Voor een volledige, en bijgewerkte lijst van alle beschikbare devices verwijzen we naar de Wiki:
// http://www.nodo-domotica.nl/index.php/Device
// ****************************************************************************************************************************************

#define UNIT_NODO       10 // Na een reset wordt dit het unitnummer van de Nodo
#define CLOCK         true // true=code voor Real Time Clock mee compileren.
#define NODO_MEGA     true // true = Nodo Mega, false=Nodo-Small
#define ETHERNET      true // true = Deze Nodo beschikt over een ethernet kaart

// Kaku              : Klik-Aan-Klik-Uit / HomeEasy protocol ontvangst
#define PLUGIN_001
#define PLUGIN_001_CORE

// SendKaku          : Klik-Aan-Klik-Uit / HomeEasy protocol verzenden 
#define PLUGIN_002
#define PLUGIN_002_CORE

// NewKAKU           : Klik-Aan-Klik-Uit ontvangst van signalen met automatische codering. Tevens bekend als Intertechno.
#define PLUGIN_003
#define PLUGIN_003_CORE

// SendNewKAKU       : Klik-Aan-Klik-Uit ontvangst van signalen met automatische codering. Tevens bekend als Intertechno. 
#define PLUGIN_004
#define PLUGIN_004_CORE

// P1 slimme meter 
#define PLUGIN_024
#define PLUGIN_024_CORE
