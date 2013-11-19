// ****************************************************************************************************************************************
// Definities voor Nodo units : CONFIG.C
// ****************************************************************************************************************************************
// Deel 1: geef hier op welke devices er gebruikt worden.
// Voor een volledige, en bijgewerkte lijst van alle beschikbare devices verwijzen we naar de Wiki:
// http://www.nodo-domotica.nl/index.php/Device
// ****************************************************************************************************************************************

#define UNIT_NODO        1 // Na een reset wordt dit het unitnummer van de Nodo
#define CLOCK         true // true=code voor Real Time Clock mee compileren.
#define NODO_MEGA     true // true = Nodo Mega, false=Nodo-Small
#define ETHERNET      true // true = Deze Nodo beschikt over een ethernet kaart

// Kaku              : Klik-Aan-Klik-Uit
#define PLUGIN_001
#define PLUGIN_001_CORE

// KakuSend          : Klik-Aan-Klik-Uit
#define PLUGIN_002
#define PLUGIN_002_CORE

// NewKAKU           : Klik-Aan-Klik-Uit ontvangst van signalen met automatische codering. Tevens bekend als Intertechno.
#define PLUGIN_003
#define PLUGIN_003_CORE

// NewKAKUSend       : Klik-Aan-Klik-Uit ontvangst van signalen met automatische codering. Tevens bekend als Intertechno. 
#define PLUGIN_004
#define PLUGIN_004_CORE

// TempSensor        : Temperatuursensor Dallas DS18B20. (Let op; -B- variant, dus niet DS1820)
#define PLUGIN_005 
#define PLUGIN_005_CORE

// UserEvent receiver voor oude Nodo codering
#define PLUGIN_007 
#define PLUGIN_007_CORE

// DTH-22 Vocht/Temperatuursensor
#define PLUGIN_006
#define PLUGIN_006_CORE 22


