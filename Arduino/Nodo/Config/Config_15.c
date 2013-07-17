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
   int dummy=123;

#define UNIT_NODO 15
#define NODO_MEGA                    false // true = Nodo Mega, false=Nodo-Small



// KAKU devices
#define DEVICE_001 blabla
#define DEVICE_002
#define DEVICE_003
#define DEVICE_004

#define DEVICE_001_CORE
#define DEVICE_002_CORE
#define DEVICE_003_CORE
#define DEVICE_004_CORE

// ****************************************************************************************************************************************
// Deel 2: Nodo specifieke zaken. 
// Allereerst unitnummer en home adres
// daarnaast ook per device de CORE, als dat device tenminste fysiek aan deze Nodo is gekoppeld
// ****************************************************************************************************************************************

// ----------------------------------------------------------------------------------------------------------------------------------------
// Definities voor Nodo unit nummer 1 
#if (NODO_SELECTOR == 1)
  // Specificatie van deze Nodo:
  #define UNIT_NODO 1 
  // Devices in gebruik op deze Nodo:
  // Dallas sensor
  #define DEVICE_005 
#endif 
// ----------------------------------------------------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------------------------------------------------
// Definities voor Nodo unit nummer 15 
#if (NODO_SELECTOR == 15)
  // Specificatie van deze Nodo:
  #define UNIT_NODO 15 
  // Devices in gebruik op deze Nodo:
  // Dallas sensor
  #define DEVICE_005
  #define DEVICE_005_CORE
#endif 
// ----------------------------------------------------------------------------------------------------------------------------------------
