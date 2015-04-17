// ############## Nodo Hardware configuratie file: HW-2510.h ######################
// #                                                                              #
// #                                                                              #
// # Gebruik deze file alleen voor deze hardware omdat anders mogelijk            #
// # beschadiging van hardware kan optreden als gevolg van andere pen-definities. #
// #                                                                              #
// #                                                                              #
// ####### Arduino project "Nodo" © Copyright 2010..2015 Paul Tonkes ############## 


// ATMega specifiek: ====================================================================================================
#define EEPROM_SIZE               1024                                          // Groote van het EEPROM geheugen.

// Serial: ============================================================================================================
#define HARDWARE_SERIAL_1         true
#define PIN_SERIAL_1_RX              0                                          // RX-0 lijn o.a. verbonden met de UART-USB of FTDI
#define PIN_SERIAL_1_TX              1                                          // TX-0 lijn o.a. verbonden met de UART-USB of FTDI
#define BAUD                     19200                                          // Baudrate voor seriele communicatie.

// I2C: =================================================================================================================
#define HARDWARE_I2C              true
#define PIN_I2C_SDA                 A4                                          // I2C communicatie lijn voor de o.a. de realtime clock.
#define PIN_I2C_SLC                 A5                                          // I2C communicatie lijn voor de o.a. de realtime clock.

// SPI (Software): ======================================================================================================
#define HARDWARE_SPI_SOFTWARE     true                                           // Simuleer op niet standaard SPI lijnen de SPI communicatie 
#define PIN_SPI_MISO                 3                                          // MISO lijn van de SPI-Poort   (NRF werk niet samen met 433)
#define PIN_SPI_CSN_0                4                                          // SPI Chipselect voor NRF24L01 (NRF werk niet samen met 433)  
#define PIN_SPI_SCK                  5                                          // SCK-lijn van de de SPI-Poort (NRF werk niet samen met 433)
#define PIN_SPI_MOSI                 6                                          // MOSO lijn van de SPI-Poort   (NRF werk niet samen met 433)

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
