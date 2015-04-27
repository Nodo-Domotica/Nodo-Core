// *********************************************************************************************************************
// Config bestand voor: Arduino Uno / Duemillanove / Nano en klonen op basis van een Atmel ATMega328 
//                      chip compatibel met het Nodo-Uno shield.
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
// Een plugin bestaat in de meeste gevallen uit twee delen:
// A: Een deel dat verantwoordelijk is voor de feitelijke aansturing/verwerking (MMI)
// B: Een deel dat alleen nodig is om Events/commando's te kunnen invoeren, doorsturen en weergeven alsmede
//    eventlist van een andere Nodo programmeren.
// 
// Als een plugin zijn feitelijke werk doet op een andere (slave) Nodo dan de centrale (master) Nodo, dan zal deel A.
// alleen nodig zijn op de slave en deel B. op de master. Een Nodo Small heeft geen MMI, dus geen PLUGIN_xxx_CORE definities. 
// *********************************************************************************************************************


#define UNIT_NODO                         15                                    // Na een reset-commando wordt dit het unitnummer van de Nodo
#define HARDWARE_CONFIG                 1500                                    // Arduino Uno / Duemillanove / Nano en klonen op basis van een Atmel ATMega328 
                                                                                // chip compatibel met het Nodo-Uno shield.  

																				
// A: Feitelijke aansturing/verwerking.
// #define PLUGIN_001_CORE                                                        // Kaku: Klik-Aan-Klik-Uit
// #define PLUGIN_002_CORE                                                        // NewKAKU: Klik-Aan-Klik-Uit met automatische codering. Tevens bekend als Intertechno.
// #define PLUGIN_005_CORE                                                        // Temperatuursensor Dallas DS18B20
// #define PLUGIN_006_CORE 22                                                     // 22=DHT-22 sensor, 11=DHT-11 sensor
// #define PLUGIN_020_CORE                                                        // BMP085 Luchtdruk sensor 
// #define PLUGIN_023_CORE                                                        // RGB-Led aansturing



//=======================================================================================================================
// Eventueel mogen hardware opties worden uitgeschakeld door remarks '//' voor onderstaande regels weg te halen
// bijvoorbeeld om niet gebruike opties uit te schakelen en geheugenruimte vrij te maken voor plugins.
// Let echter wel op dan sommige voorzieningen van elkaar afhankelijk zijn of dat plugins hier gebruik van maken.
//=======================================================================================================================
// #define HARDWARE_WIRED_IN_PORTS      0
// #define HARDWARE_WIRED_OUT_PORTS     0
// #define HARDWARE_STATUS_LED      false    
// #define HARDWARE_SPEAKER         false
// #define HARDWARE_INFRARED        false
// #define HARDWARE_SERIAL_1        false
#define HARDWARE_CLOCK           false
// #define HARDWARE_I2C             false

