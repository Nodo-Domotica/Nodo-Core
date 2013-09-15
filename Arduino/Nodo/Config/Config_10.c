
// ****************************************************************************************************************************************
// Definities voor Nodo units : CONFIG.C
// ****************************************************************************************************************************************
// Deel 1: geef hier op welke devices er gebruikt worden. Het maakt niet uit in welke Nodo ht device zich bevindt.
// Let op: u dient voor elke device file het VOLLEDIGE path op te geven!
//
// Voor een volledige, en bijgewerkte lijst van alle beschikbare devices verwijzen we naar de Wiki:
// http://www.nodo-domotica.nl/index.php/Device
//
// Allereerst de KAKU devices. Die worden gespecificeerd voor ALLE Nodo's. Alle andere devices worden Nodo-specifiek opgegeven
// ****************************************************************************************************************************************

#define UNIT_NODO 10
#define NODO_MEGA                    true // true = Nodo Mega, false=Nodo-Small
#define ETHERNET                     false // true = Deze Nodo beschikt over een ethernet kaart

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

// UserEvent          : Ontvangst compatibiliteit met UserEvents van de oude Nodo
#define PLUGIN_007
#define PLUGIN_007_CORE

// MyDevice           : Voorbeeld device.
#define PLUGIN_006
#define PLUGIN_006_CORE

#define PLUGIN_012
#define PLUGIN_012_CORE

#define PLUGIN_013
#define PLUGIN_013_CORE

#define PLUGIN_014
#define PLUGIN_014_CORE

#define PLUGIN_015
#define PLUGIN_015_CORE

#define PLUGIN_016
#define PLUGIN_016_CORE

#define PLUGIN_018
#define PLUGIN_018_CORE


#define PLUGIN_020
#define PLUGIN_020_CORE

#define PLUGIN_021
#define PLUGIN_021_CORE

#define PLUGIN_022
#define PLUGIN_022_CORE

#define PLUGIN_023
#define PLUGIN_023_CORE

#define PLUGIN_025
#define PLUGIN_025_CORE

#define PLUGIN_026
#define PLUGIN_026_CORE

#define PLUGIN_027
#define PLUGIN_027_CORE

#define PLUGIN_028
#define PLUGIN_028_CORE

#define PLUGIN_029
#define PLUGIN_029_CORE







