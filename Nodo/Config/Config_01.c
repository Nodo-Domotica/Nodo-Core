// *********************************************************************************************************************
// Config bestand voor: Arduino Mega2560 board met een NES of ethernetkaart & koppelprint
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
// Een plugin bestaat in de meeste gevallen uit twee delen:
// A: Een deel dat verantwoordelijk is voor de feitelijke aansturing/verwerking (MMI)
// B: Een deel dat alleen nodig is om Events/commando's te kunnen invoeren, doorsturen en weergeven alsmede
//    eventlist van een andere Nodo programmeren.
// 
// Als een plugin zijn feitelijke werk doet op een andere (slave) Nodo dan de centrale (master) Nodo, dan zal deel A.
// alleen nodig zijn op de slave en deel B. op de master. Er zijn echter ook plugins  
// *********************************************************************************************************************


#define UNIT_NODO                          1                                    // Na een reset-commando wordt dit het unitnummer van de Nodo
#define HARDWARE_CONFIG                 1000                                    // Arduino Mega2560 board met een NES of ethernetkaart & koppelprint
                                                                                // NRF24L01 module voor Nodo-2-Nodo communicatie op A0..A3. 
                                                                                // Kies 1001 als je een NRF24L01 op de WiredIn lijnen hebt aangeloten 
                                                                                // zoals die zich op de 26-polige stekker van de NES bevindt. 
                                                                                //
                                                                                // Voor extra info over haqrdware en penbezetting: zie ../Hardware/Read_me.txt



// A: Feitelijke aansturing/verwerking
#define PLUGIN_001_CORE                                                         // Kaku : Klik-Aan-Klik-Uit
#define PLUGIN_002_CORE                                                         // NewKAKU : Klik-Aan-Klik-Uit automatische codering.


// B: Events/commando's invoeren, doorgeven en weergeven (MMI)
#define PLUGIN_001                                                              // Kaku : Klik-Aan-Klik-Uit
#define PLUGIN_002                                                              // NewKAKU : Klik-Aan-Klik-Uit automatische codering.
#define PLUGIN_006                                                              // Vochtigheid-/temperatuursensor DHT-22
#define PLUGIN_005                                                              // Temperatuursensor Dallas DS18B20
#define PLUGIN_020                                                              // Luchtdruksensor BPM085
#define PLUGIN_023                                                              // RGB-Led aansturing


//=======================================================================================================================
// Eventueel mogen hardware opties worden uitgeschakeld door remarks '//' voor onderstaande regels weg te halen
// bijvoorbeeld om niet gebruike opties uit te schakelen en geheugenruimte vrij te maken voor plugins.
// Let echter wel op dan sommige voorzieningen van elkaar afhankelijk zijn of dat plugins hier gebruik van maken.
//=======================================================================================================================
// #define HARDWARE_WIRED_IN_PORTS    false
// #define HARDWARE_WIRED_OUT_PORTS   false
// #define HARDWARE_STATUS_LED_RGB    false
// #define HARDWARE_SPEAKER           false
// #define HARDWARE_PULSE             false
// #define HARDWARE_INFRARED          false
// #define HARDWARE_RF433             false
// #define HARDWARE_RAWSIGNAL         false
// #define HARDWARE_CLOCK             false
// #define HARDWARE_I2C               false
// #define HARDWARE_SDCARD            false
#define HARDWARE_ETHERNET          false
// #define HARDWARE_NRF24L01          false        
