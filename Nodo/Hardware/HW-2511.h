// ############## Nodo Hardware configuratie file: HW-2511.h ######################
// #                                                                              #
// #                                                                              #
// # Gebruik deze file alleen voor deze hardware omdat anders mogelijk            #
// # beschadiging van hardware kan optreden als gevolg van andere pen-definities. #
// #                                                                              #
// #                                                                              #
// ####### Arduino project "Nodo" ? Copyright 2010..2015 Paul Tonkes ############## 


// Nodo algemeen: =======================================================================================================
#define NODO_MEGA                false

// ATMega specifiek: ====================================================================================================
#define EEPROM_SIZE               1024                                          // Groote van het EEPROM geheugen.

// WiredOut: ============================================================================================================
#define HARDWARE_WIRED_OUT_PORTS     2                                          // aantal WiredOut poorten
#define PIN_WIRED_OUT_1              9                                          // WiredOut 1 (PWM)
#define PIN_WIRED_OUT_2             10                                          // WiredOut 2 (PWM)

// Status Led: ==============================================================================================================
// #define HARDWARE_STATUS_LED       true    
// #define PIN_LED                     13

// Serial: ============================================================================================================
#define HARDWARE_SERIAL_1         true
#define PIN_SERIAL_1_RX              0                                          // RX-0 lijn o.a. verbonden met de UART-USB of FTDI
#define PIN_SERIAL_1_TX              1                                          // TX-0 lijn o.a. verbonden met de UART-USB of FTDI
#define BAUD                     19200                                          // Baudrate voor seriele communicatie.

#define HARDWARE_SERIAL_2         true                                          // Additionele seriele poort beschikbaar. Baudrate voor deze lijn wordt in plugin bepaald,
#define HARDWARE_SERIAL_2_SW      true                                          // ...en de variant is SoftSerial.
#define PIN_SERIAL_2_TX              7                                          // TX lijn voor additionele seriele communicatie vanuit plugin (SoftSerial) Werkt niet samen met luidspreker.
#define PIN_SERIAL_2_RX              8                                          // RX lijn voor additionele seriele communicatie vanuit plugin (SoftSerial)

// I2C: =================================================================================================================
#define HARDWARE_I2C              true
#define PIN_I2C_SDA                 A4                                          // I2C communicatie lijn voor de o.a. de realtime clock.
#define PIN_I2C_SLC                 A5                                          // I2C communicatie lijn voor de o.a. de realtime clock.

// SPI (Software): ======================================================================================================
#define HARDWARE_SPI_SOFTWARE     true                                          // Simuleer op niet standaard SPI lijnen de SPI communicatie 
#define PIN_SPI_MISO                 3                                          // MISO lijn van de SPI-Poort   (NRF werk niet samen met 433)
#define PIN_SPI_CSN_0                4                                          // SPI Chipselect voor NRF24L01 (NRF werk niet samen met 433)  
#define PIN_SPI_SCK                  5                                          // SCK-lijn van de de SPI-Poort (NRF werk niet samen met 433)
#define PIN_SPI_MOSI                 6                                          // MISO lijn van de SPI-Poort   (NRF werk niet samen met 433)

// Battery: ============================================================================================================
#define HARDWARE_BATTERY          true                                          // Batterij-modus is mogelijk

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