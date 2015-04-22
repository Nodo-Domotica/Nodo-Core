
// ############## Nodo Hardware configuratie file: HW-1001.h  #####################
// #                                                                              #
// #                                                                              #
// # Gebruik deze file alleen voor de geschikte hardware omdat anders mogelijk    #
// # beschadiging van hardware kan optreden als gevolg van andere pen-definities. #
// #                                                                              #
// #                                                                              #
// ####### Arduino project "Nodo" © Copyright 2010..2015 Paul Tonkes ############## 


// Nodo algemeen: =======================================================================================================
#define NODO_MEGA                 true

// ATMega specifiek: ====================================================================================================
#define EEPROM_SIZE               4096                                          // Groote van het EEPROM geheugen.

// User: ================================================================================================================
#define PIN_USER_0                  D22                                         // Vrij voor gebruiker voor gebruik in plugins
#define PIN_USER_1                  D23                                         // Vrij voor gebruiker voor gebruik in plugins
#define PIN_USER_2                  D24                                         // Vrij voor gebruiker voor gebruik in plugins
#define PIN_USER_3                  D25                                         // Vrij voor gebruiker voor gebruik in plugins
#define PIN_USER_4                  D26                                         // Vrij voor gebruiker voor gebruik in plugins
#define PIN_USER_5                  D27                                         // Vrij voor gebruiker voor gebruik in plugins
#define PIN_USER_6                  D28                                         // Vrij voor gebruiker voor gebruik in plugins
#define PIN_USER_7                  D29                                         // Vrij voor gebruiker voor gebruik in plugins

// WiredIn: =============================================================================================================
#define HARDWARE_WIRED_IN_PORTS      4                                          // Aantal WiredIn/WiredOut poorten
#define PIN_WIRED_IN_1               8                                          // Analoge input A8 wordt gebruikt voor WiredIn 1
#define PIN_WIRED_IN_2               9                                          // Analoge input A9 wordt gebruikt voor WiredIn 2
#define PIN_WIRED_IN_3              10                                          // Analoge input A10 wordt gebruikt voor WiredIn 3
#define PIN_WIRED_IN_4              11                                          // Analoge input A11 wordt gebruikt voor WiredIn 4

// WiredOut: ============================================================================================================
#define HARDWARE_WIRED_OUT_PORTS     8                                          // aAntal WiredIn/WiredOut poorten
#define PIN_WIRED_OUT_1             30                                          // Digitale output D30 wordt gebruikt voor WiredOut 1
#define PIN_WIRED_OUT_2             31                                          // Digitale output D31 wordt gebruikt voor WiredOut 2
#define PIN_WIRED_OUT_3             32                                          // Digitale output D32 wordt gebruikt voor WiredOut 3
#define PIN_WIRED_OUT_4             33                                          // Digitale output D33 wordt gebruikt voor WiredOut 4
#define PIN_WIRED_OUT_5             34                                          // Digitale output D34 wordt gebruikt voor WiredOut 5
#define PIN_WIRED_OUT_6             35                                          // Digitale output D35 wordt gebruikt voor WiredOut 6
#define PIN_WIRED_OUT_7             36                                          // Digitale output D36 wordt gebruikt voor WiredOut 7
#define PIN_WIRED_OUT_8             37                                          // Digitale output D37 wordt gebruikt voor WiredOut 8

// Status Led: ==============================================================================================================
#define HARDWARE_STATUS_LED_RGB   true    
#define PIN_LED_R                   47                                          // RGB-Led, aansluiting rood
#define PIN_LED_G                   48                                          // RGB-Led, aansluiting groen
#define PIN_LED_B                   49                                          // RGB-Led, aansluiting blauw

// Speaker: ==============================================================================================================
#define HARDWARE_SPEAKER          true
#define PIN_SPEAKER                 42                                          // Luidspreker aansluiting

// Pulse: ===================================================================================================================
#define HARDWARE_PULSE            true
#define PIN_PULSE                   18
#define PULSE_IRQ                    5

// RawSignal: ===========================================================================================================
#define HARDWARE_RAWSIGNAL        true
#define RAW_BUFFER_SIZE            512                                          // Maximaal aantal te ontvangen bits = waarde/2

// Infrarood: ===========================================================================================================
#define HARDWARE_INFRARED         true
#define PIN_IR_TX_DATA              17                                          // Zender IR-Led. (gebufferd via transistor i.v.m. hogere stroom die nodig is voor IR-led)
#define PIN_IR_RX_DATA              18                                          // Op deze input komt het IR signaal binnen van de TSOP. Bij HIGH bij geen signaal.

// RF 433Mhz: ===========================================================================================================
#define HARDWARE_RF433            true
#define PIN_RF_TX_ENABLE            15                                          // +5 volt / Vcc spanning naar de zender.
#define PIN_RF_TX_DATA              14                                          // Data naar de 433Mhz zender
#define PIN_RF_RX_ENABLE            16                                          // Spanning naar de ontvanger via deze pin.
#define PIN_RF_RX_DATA              19                                          // Op deze input komt het 433Mhz-RF signaal binnen. LOW bij geen signaal.

// Serial: ============================================================================================================
#define HARDWARE_SERIAL_1         true
#define PIN_SERIAL_1_RX              0                                          // RX-0 lijn o.a. verbonden met de UART-USB
#define PIN_SERIAL_2_TX              1                                          // TX-0 lijn o.a. verbonden met de UART-USB
#define BAUD                     19200                                          // Baudrate voor seriele communicatie.

// Realtime clock DS1307: ===============================================================================================
#define HARDWARE_CLOCK            true
#define HARDWARE_I2C              true

// I2C: =================================================================================================================
#define HARDWARE_I2C              true
#define PIN_I2C_SDA                 20                                          // I2C communicatie lijn voor de o.a. de realtime clock.
#define PIN_I2C_SLC                 21                                          // I2C communicatie lijn voor de o.a. de realtime clock.

// SDCard: ==============================================================================================================
#define HARDWARE_SDCARD           true
#define EthernetShield_CS_SDCardH   53                                          // Ethernet shield: Gereserveerd voor correct funktioneren van de SDCard: Hardware CS/SPI ChipSelect
#define EthernetShield_CS_SDCard     4                                          // Ethernet shield: Chipselect van de SDCard. Niet gebruiken voor andere doeleinden

// Ethernet: ============================================================================================================
#define HARDWARE_ETHERNET         true
#define ETHERNET_MAC_0            0xCC                                          // Dit is byte 0 van het MAC adres. In de bytes 3,4 en 5 zijn het Home en Unitnummer van de Nodo verwerkt.
#define ETHERNET_MAC_1            0xBB                                          // Dit is byte 1 van het MAC adres. In de bytes 3,4 en 5 zijn het Home en Unitnummer van de Nodo verwerkt.
#define ETHERNET_MAC_2            0xAA                                          // Dit is byte 2 van het MAC adres. In de bytes 3,4 en 5 zijn het Home en Unitnummer van de Nodo verwerkt.
#define EthernetShield_CS_W5100     10                                          // Ethernet shield: D10..D13  // gereserveerd voor Ethernet & SDCard

// SPI: ================================================================================================================
#define HARDWARE_SPI_SOFTWARE     true
#define PIN_SPI_CSN_0              A12                                          // SPI Chipselect lijn 0 (Default ingezet voor de NRF24L01)  
#define PIN_SPI_MOSI               A13                                          // MISO lijn van de SPI-Poort
#define PIN_SPI_MISO               A14                                          // MISO lijn van de SPI-Poort
#define PIN_SPI_SCK                A15                                          // SCK-lijn van de de SPI-Poort

// NRF24L01: ============================================================================================================
#define HARDWARE_NRF24L01         true
                                                                                // NRF24L01:         Arduino:
                                                                                // ------------------------------------------------------------------------------
                                                                                // VCC        =>     +3V3 (LET OP:de voedspanning van de NRF is NIET 5V tolerant!)
                                                                                // CSN        =>     PIN_SPI_CSN_0
                                                                                // MOSI       =>     PIN_SPI_MOSI
                                                                                // MISO       =>     PIN_SPI_MISO
                                                                                // SCK        =>     PIN_SPI_SCK
                                                                                // CE         =>     +5V
                                                                                // GND        =>     GND
                                                                                // IRQ                -                 
