// *************************************************************************************************
// Config bestand voor:      OpenTherm Gateway met een Nodo Small op basis van Arduino Uno, 
//                           Duemillanove, Nano-V3 of een ProMini, danwel klonen op basis van de 
//                           Atmel ATMega328 chip.
//       
//                           RF communicatie op 2.4GHz via een NRF24L01 module. 
//
//                           LET OP: 2e generatie Nodo penbezetting! Niet gebruiken voor een NES of 
//                                   oudere Nodo. 
//
// **************************************************************************************************




//                   U N D E R  C O N S T R U C T I O N !                       



#define HARDWARE_CONFIG                 2511                                    // Nodo-Small met 2e generatie penbezetting met NRF24L01 en extra RX/TX
#define UNIT_NODO                         16                                    // Na een reset-commando wordt dit het unitnummer van de Nodo

#define PLUGIN_011_DEBUG                true                                    // Debug optie voor veekijken via RX0/TX0 (USB / FTDI) 
#define PLUGIN_011                                                              // OpenTherm gateway otgw.                                                              
#define PLUGIN_011_CORE                    1                                    // Basis variabele start bij 1
                                         			                                  // <basis variabele> + 0 wordt gebruikt om temperatuur setpoint in te stellen
                                        			                                  // <basis variabele> + 1 wordt gebruikt voor monitoring van Room Temperature
                                        			                                  // <basis variabele> + 2 wordt gebruikt voor monitoring van Boiler Water Temperature
                                        			                                  // <basis variabele> + 3 wordt gebruikt voor monitoring van Modulation
                                        			                                  // <basis variabele> + 4 wordt gebruikt voor monitoring van Boiler Water Pressure
                                        			                                  // <basis variabele> + 5 wordt gebruikt voor monitoring van Thermostat Setpoint
                                        			                                  // <basis variabele> + 6 wordt gebruikt voor monitoring van Flame status
                                        			                                  // <basis variabele> + 7 wordt gebruikt voor monitoring van Boiler Return Water Temperature
                                        			                                  // <basis variabele> + 8 wordt gebruikt voor monitoring van Domestic Hot Water Mode
 


