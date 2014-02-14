/****************************************************************************************************************************\
 * Arduino project "NRF Extender" © Copyright 2014 Paul Tonkes / Martinus van den Broek 
 * 
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License 
 * as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 * You received a copy of the GNU General Public License along with this program in file 'COPYING.TXT'.
 *
 * Additional information about licensing can be found at : http://www.gnu.org/licenses
 * Documentation can be found at                          : http://www.nodo-domotica.nl
 * Bugs can be reported at                                : https://code.google.com/p/arduino-nodo/
 * Compiler download                                      : http://arduino.cc
 * Compiler used for testing/building                     : Arduino Compiler version 1.0.1
 * Libraries                                              : WireNodo library replaces standard Arduino Wire library!
 \*************************************************************************************************************************/

// NRF Extender
// Prototype R008
// 14-02-2014

// This version is "Nodo independent" but needs a small plugin!

// release notes
// R001 06-02-2014  first release
// R001 07-02-2014  added MMI for unit,address and channel configuration, stored in EEPROM
//                  added channel scanner
// R003 08-02-2014  added additional generic receive pipe on NRF
//                  changed to Nodo independent version
// R004 08-02-2014  filter on ID=0 in NRF receive
// R005 08-02-2014  malloc/free issue fixed
//                  added freemem command
// R006 09-02-2014  added ping command
//                  code optimizations
// R007 10-02-2014  leave auto-ack receive pipe at address 0 after send packet
// R008 14-02-2014  added more debugging output (dump payload data)
//                  added "wireless serial" mode, now default mode after boot!
//                    (type @command@ to get into command/debug mode)
//                  added "single peer" transmit mode
//                  added "reset" command to reset connected device on pin D3
//                    (future I2C watchdog or other purposes)
//                  bugfix for I2C_Received changed during NRFsend

#define NRF_RECEIVE_ADDRESS      16 // Default Radio address, range 1-31
#define THIS_EXTENDER_UNIT       31 // Default Unit, range 1-31

#define EXTENDER_VERSION          1

#define COMMAND_MODE_STRING       "@command@"

// NRF Settings
#define NRF_UNIT_MAX             32
#define NRF_CSN_PIN               7
#define NRF_CE_PIN                8
#define NRF_CHANNEL              72 // Default Radio channel, range 1-125
#define NRF_PAYLOAD_SIZE	 32

// I2C Settings
#define I2C_UNIT_MAX             31
#define I2C_START_ADDRESS         1
#define I2C_BUFFERSIZE           32

#define NRF_PAYLOAD_NODO           0
#define NRF_PAYLOAD_SENSOR         1
#define NRF_PAYLOAD_EVENT          2
#define NRF_PAYLOAD_TEXT           3
#define NRF_PAYLOAD_PINGREQ        4
#define NRF_PAYLOAD_PINGREP        5
#define NRF_PAYLOAD_ONLINE       255

#include <SPI.h>
#include <Arduino.h>
#include <WireNodo.h>
#include <EEPROM.h>

void(*Reboot)(void)=0;

struct SettingsStruct
{
  int  Version;        
  byte Unit;
  byte Address;
  byte Channel;
  byte Peer; 
}
Settings;

struct NRFPayloadStruct
{
  byte Source;
  byte Destination;
  byte ID;
  byte Size;
  byte Data[28];
}
NRFPayload;

byte NRF_status=0;
byte NRF_address[5] = { 1,2,3,4,5 };

boolean NRFOnline[NRF_UNIT_MAX+1];
boolean I2COnline[I2C_UNIT_MAX+1];

byte I2C_Received=0;                            // nr of bytes received through I2C bus
byte I2C_ReceiveBuffer[I2C_BUFFERSIZE];         // I2C receive buffer

boolean command_mode=false;

void setup() 
{
  LoadSettings();
  if(Settings.Version!=EXTENDER_VERSION)ResetFactory();
  
  Serial.begin(19200);
  WireNodo.begin(Settings.Unit + I2C_START_ADDRESS - 1);
  WireNodo.onReceive(ReceiveI2C);

  CheckI2COnline();                       // Detect I2C peers
  
  NRF_init();                             // Initialize NRF radio address, channel, etc
  NRF_CheckOnline();                      // Send data on air to find out who's online
}

void loop() 
{
  if(Serial.available())
    serial();

  if(NRF_receive())
    {
      if(command_mode)
        Serial.println("NRF -> I2C");
      SendI2C();
    }

  if(I2C_Received > 0)
  {
      if(command_mode)
        Serial.println("I2C -> NRF");

      if (I2C_Received <=NRF_PAYLOAD_SIZE-4)
      {
        NRF_send();
        I2C_Received=0;
      }
      else
        if(command_mode)
          Serial.println("I2C data too large!");
  }
}

