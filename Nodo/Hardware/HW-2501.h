// ############## Nodo Hardware configuratie file: HW-2501.h ######################
// #                                                                              #
// #                                                                              #
// # Gebruik deze file alleen voor deze hardware omdat anders mogelijk            #
// # beschadiging van hardware kan optreden als gevolg van andere pen-definities. #
// #                                                                              #
// #                                                                              #
// ####### Arduino project "Nodo" © Copyright 2010..2015 Paul Tonkes ############## 


// ATMega specifiek: ====================================================================================================
#define EEPROM_SIZE               1024                                          // Groote van het EEPROM geheugen.

// WiredIn: =============================================================================================================
#define HARDWARE_WIRED_IN_PORTS      4                                          // Aantal WiredIn poorten
#define PIN_WIRED_IN_1              A0                                          // WiredIn 1
#define PIN_WIRED_IN_2              A1                                          // WiredIn 2
#define PIN_WIRED_IN_3              A2                                          // WiredIn 3
#define PIN_WIRED_IN_4              A3                                          // WiredIn 4

// WiredOut: ============================================================================================================
#define HARDWARE_WIRED_OUT_PORTS     4                                          // aantal WiredOut poorten
#define PIN_WIRED_OUT_1              9                                          // WiredOut 1 (PWM)
#define PIN_WIRED_OUT_2             10                                          // WiredOut 2 (PWM)
#define PIN_WIRED_OUT_3             11                                          // WiredOut 3 (PWM)
#define PIN_WIRED_OUT_4             12                                          // WiredOut 4

// Status Led: ==============================================================================================================
#define HARDWARE_STATUS_LED       true    
#define PIN_LED                     13

// Pulse: ===================================================================================================================
#define HARDWARE_PULSE            true
#define PIN_PULSE                    2                                          // Input voor pulsen tellen
#define PULSE_IRQ                    0                                          // Aan pin-2 zit de externe interrupt 0                                          

// RF 433Mhz: ===========================================================================================================
#define HARDWARE_RF433            true
#define PIN_RF_RX_DATA               3                                          // Op deze input komt het 433Mhz-RF signaal binnen. LOW bij geen signaal. (Kan niet samen met NRF24L01 worden gebruikt)
#define PIN_RF_RX_ENABLE             4                                          // +5 volt / Vcc spanning naar de zender / pulse bij een Aurel tranceiver (Kan niet samen met NRF24L01 worden gebruikt)
#define PIN_RF_TX_ENABLE             5                                          // +5 volt / Vcc spanning naar de zender / pulse bij een Aurel tranceiver (Kan niet samen met NRF24L01 worden gebruikt)
#define PIN_RF_TX_DATA               6                                          // Data naar de 433Mhz zender. (Kan niet samen met NRF24L01 worden gebruikt)
#define RAW_BUFFER_SIZE            256                                          // Maximaal aantal te ontvangen bits = waarde/2

// Serial: ============================================================================================================
#define HARDWARE_SERIAL_1         true
#define PIN_SERIAL_1_RX              0                                          // RX-0 lijn o.a. verbonden met de UART-USB of FTDI
#define PIN_SERIAL_1_TX              1                                          // TX-0 lijn o.a. verbonden met de UART-USB of FTDI
#define BAUD                     19200                                          // Baudrate voor seriele communicatie.
#define HARDWARE_SERIAL_2_SW      true                                          // Baudrate voor deze lijn wordt in plugin bepaald
#define PIN_SERIAL_2_TX              7                                          // TX lijn voor additionele seriele communicatie vanuit plugin (SoftSerial) Werkt niet samen met luidspreker.
#define PIN_SERIAL_2_RX              8                                          // RX lijn voor additionele seriele communicatie vanuit plugin (SoftSerial)

// Realtime clock DS1307: ===============================================================================================
#define HARDWARE_CLOCK            true
#define HARDWARE_I2C              true

// I2C: =================================================================================================================
#define HARDWARE_I2C              true
#define PIN_I2C_SDA                 A4                                          // I2C communicatie lijn voor de o.a. de realtime clock.
#define PIN_I2C_SLC                 A5                                          // I2C communicatie lijn voor de o.a. de realtime clock.

