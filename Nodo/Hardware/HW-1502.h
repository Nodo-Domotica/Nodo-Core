
// ################### Nodo Hardware configuratie file: HW-1502.h #################
// #                                                                              #
// #                                                                              #
// # Gebruik deze file alleen voor de geschikte hardware omdat anders mogelijk    #
// # beschadiging van hardware kan optreden als gevolg van andere pen-definities. #
// #                                                                              #
// #                                                                              #
// ####### Arduino project "Nodo" © Copyright 2010..2015 Paul Tonkes ############## 


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

// Serial: ============================================================================================================
#define HARDWARE_SERIAL_1         true
#define PIN_SERIAL_1_RX              0                                          // RX-0 lijn o.a. verbonden met de UART-USB of FTDI
#define PIN_SERIAL_1_TX              1                                          // TX-0 lijn o.a. verbonden met de UART-USB of FTDI
#define BAUD                     19200                                          // Baudrate voor seriele communicatie.

// I2C: =================================================================================================================
#define HARDWARE_I2C              true
#define PIN_I2C_SDA                 A4                                          // I2C communicatie lijn voor de o.a. de realtime clock.
#define PIN_I2C_SLC                 A5                                          // I2C communicatie lijn voor de o.a. de realtime clock.

// Battery: ============================================================================================================
#define HARDWARE_BATTERY          true                                          // Batterij-modus is mogelijk

// SPI: ================================================================================================================
#define HARDWARE_SPI_SOFTWARE     true
#define PIN_SPI_CSN_0               A0                                          // SPI Chipselect lijn 0 (Default ingezet voor de NRF24L01)  
#define PIN_SPI_MOSI                A1                                          // MISO lijn van de SPI-Poort
#define PIN_SPI_MISO                A2                                          // MISO lijn van de SPI-Poort
#define PIN_SPI_SCK                 A3                                          // SCK-lijn van de de SPI-Poort

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
