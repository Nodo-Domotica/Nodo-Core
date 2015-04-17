
// ################### Nodo Hardware configuratie file: HW-1502.h #################
// #                                                                              #
// #                                                                              #
// # Gebruik deze file alleen voor de geschikte hardware omdat anders mogelijk    #
// # beschadiging van hardware kan optreden als gevolg van andere pen-definities. #
// #                                                                              #
// #                                                                              #
// ####### Arduino project "Nodo" � Copyright 2010..2015 Paul Tonkes ############## 


// Nodo algemeen: =======================================================================================================
#define NODO_MEGA                false

// ATMega specifiek: ====================================================================================================
#define EEPROM_SIZE               1024                                          // Groote van het EEPROM geheugen.

// WiredOut: ============================================================================================================
#define HARDWARE_WIRED_OUT_PORTS     5                                          // aAntal WiredIn/WiredOut poorten
#define PIN_WIRED_OUT_1              7                                          // digitale outputs D07 t/m D10 worden gebruikt voor WiredOut 1 tot en met 4
#define PIN_WIRED_OUT_2              8                                          // digitale outputs D07 t/m D10 worden gebruikt voor WiredOut 1 tot en met 4
#define PIN_WIRED_OUT_3              9                                          // (pwm) 4 digitale outputs D07 t/m D10 worden gebruikt voor WiredOut 1 tot en met 4
#define PIN_WIRED_OUT_4             10                                          // (pwm) 4 digitale outputs D07 t/m D10 worden gebruikt voor WiredOut 1 tot en met 4
#define PIN_WIRED_OUT_5             11                                          // (pwm) Extra digitale output, op standaard Nodo is dit de IR-Led !!!

// Status Led: ==============================================================================================================
#define HARDWARE_STATUS_LED       true    
#define PIN_LED                     13

// Speaker: ==============================================================================================================
#define HARDWARE_SPEAKER          true
#define PIN_SPEAKER                  6                                          // Luidspreker aansluiting

// Pulse: ===================================================================================================================
#define HARDWARE_PULSE            true
#define PULSE_IRQ                    1                                          // IRQ-1 verbonden aan de IR_RX_DATA pen 3 van de ATMega328 (Uno/Nano/Duemillanove)
#define PIN_PULSE                    3

// RawSignal: ===========================================================================================================
#define HARDWARE_RAWSIGNAL        true
#define RAW_BUFFER_SIZE            256                                          // Maximaal aantal te ontvangen bits = waarde/2

// Infrarood: ===========================================================================================================
#define HARDWARE_INFRARED         true
#define PIN_IR_TX_DATA              11                                          // Zender IR-Led. (gebufferd via transistor i.v.m. hogere stroom die nodig is voor IR-led)
#define PIN_IR_RX_DATA               3                                          // Op deze input komt het IR signaal binnen van de TSOP. Bij HIGH bij geen signaal.

// RF 433Mhz: ===========================================================================================================
#define HARDWARE_RF433            true
#define PIN_RF_TX_VCC                4                                          // +5 volt / Vcc spanning naar de zender.
#define PIN_RF_TX_DATA               5                                          // data naar de zender
#define PIN_RF_RX_DATA               2                                          // Op deze input komt het 433Mhz-RF signaal binnen. LOW bij geen signaal.
#define PIN_RF_RX_VCC               12                                          // Spanning naar de ontvanger via deze pin.

// Serial: ============================================================================================================
#define HARDWARE_SERIAL_1         true
#define PIN_SERIAL_1_RX              0                                          // RX-0 lijn o.a. verbonden met de UART-USB of FTDI
#define PIN_SERIAL_1_TX              1                                          // TX-0 lijn o.a. verbonden met de UART-USB of FTDI
#define BAUD                     19200                                          // Baudrate voor seriele communicatie.

// Realtime clock DS1307: ===============================================================================================
#define HARDWARE_CLOCK            true
#define HARDWARE_I2C              true

// I2C: =================================================================================================================
#define HARDWARE_I2C              true
#define PIN_I2C_SDA                 A4                                          // I2C communicatie lijn voor de o.a. de realtime clock.
#define PIN_I2C_SLC                 A5                                          // I2C communicatie lijn voor de o.a. de realtime clock.

// SPI: ================================================================================================================
#define HARDWARE_SPI_SOFTWARE     true
#define PIN_SPI_CSN_0               A0                                          // SPI Chipselect lijn 0 (Default ingezet voor de NRF24L01)  
#define PIN_SPI_MOSI                A1                                          // MISO lijn van de SPI-Poort
#define PIN_SPI_MISO                A2                                          // MISO lijn van de SPI-Poort
#define PIN_SPI_SCK                 A3                                          // SCK-lijn van de de SPI-Poort

// NRF24L01: ============================================================================================================
#define HARDWARE_NRF24L01         true
#define NRF_CHANNEL                 36                                          // Kanaal op de 2.4Ghz band waarmee de Nodo's met elkaar communiceren.
#define NRF_ADDRESS         B1,0xB2,0xB3,0xB4,0xB5                              // 5-byte NRF adres (1e byte wordt ingevuld met unitnummer)

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
