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

#define UNIT_NODO 1
#define NODO_MEGA                    true // true = Nodo Mega, false=Nodo-Small
#define ETHERNET                     true // true = Deze Nodo beschikt over een ethernet kaart

// KAKU devices
#define DEVICE_001
#define DEVICE_002
#define DEVICE_003
#define DEVICE_004

#define DEVICE_001_CORE
#define DEVICE_002_CORE
#define DEVICE_003_CORE
#define DEVICE_004_CORE

#define DEVICE_005 
#define DEVICE_005_CORE

