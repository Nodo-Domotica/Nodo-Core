/****************************************************************************************************************************\
 * Arduino project "Nodo NRF Extender" © Copyright 2014 Paul Tonkes / Martinus van den Broek 
 * 
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License 
 * as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 * You received a copy of the GNU General Public License along with this program in file 'COPYING.TXT'.
 *
 * Voor toelichting op de licentievoorwaarden zie    : http://www.gnu.org/licenses
 * Uitgebreide documentatie is te vinden op          : http://www.nodo-domotica.nl
 * bugs kunnen worden gelogd op                      : https://code.google.com/p/arduino-nodo/
 * Compiler voor deze programmacode te downloaden op : http://arduino.cc
 * Compiler                                          : Arduino Compiler met minimaal versie 1.0.1
 * Libraries                                         : WireNodo library vervangt de standaard Wire library!
 \*************************************************************************************************************************/

// Nodo NRF Extender
// Prototype 001
// 06-02-2014
// This version is Nodo version dependent!

#define NRF_RECEIVE_ADDRESS      60 // Range 1-64

#define THIS_EXTENDER_UNIT       31

#define NODO_VERSION_MAJOR        3
#define NODO_VERSION_MINOR        6

// NRF Settings
#define NRF_CSN_PIN               7
#define NRF_CE_PIN                8
#define NRF_CHANNEL              72
#define NRF_PAYLOAD_SIZE	 32
#define NRF_UNIT_MAX             64

// Nodo specific #defines
#define UNIT_MAX                 31
#define EVENT_BOOT                1
#define VALUE_SOURCE_I2C         64
#define EVENT_NEWNODO            73
#define TRANSMISSION_CONFIRM     16
#define NODO_TYPE_EVENT           1
#define NODO_TYPE_SYSTEM          3

#define I2C_START_ADDRESS         1
#define I2C_BUFFERSIZE           32

#include <SPI.h>
#include <Arduino.h>
#include <WireNodo.h>

struct SettingsStruct
{
  int     Version;        
  byte    Unit;
}
Settings;

struct NodoEventStruct
{
  // Event deel
  byte Type;
  byte Command;
  byte Par1;
  unsigned long Par2;

  // Transmissie deel
  byte SourceUnit;
  byte DestinationUnit;
  byte Flags;
  byte Port;
  byte Direction;
  byte Version;
  byte Checksum;
}
NodoEvent;

boolean NRFOnline[NRF_UNIT_MAX+1];
byte I2C_Received=0;                            // nr of bytes received through I2C bus
byte I2C_ReceiveBuffer[I2C_BUFFERSIZE];         // I2C receive buffer

void setup() 
{
  Settings.Unit=THIS_EXTENDER_UNIT;
  Serial.begin(19200);
  WireNodo.begin(Settings.Unit + I2C_START_ADDRESS - 1);
  WireNodo.onReceive(ReceiveI2C);

  // prepare and send Nodo Boot event to all possible Nodo units
  NodoEvent.Type            = NODO_TYPE_EVENT;
  NodoEvent.Command         = EVENT_BOOT;  
  NodoEvent.Par1            = Settings.Unit;
  NodoEvent.Par2            = 0;
  NodoEvent.SourceUnit      = Settings.Unit;
  NodoEvent.DestinationUnit = 0;
  NodoEvent.Flags           = TRANSMISSION_CONFIRM;
  SendI2C(&NodoEvent);                    // anounce ourselves on the I2C bus to Nodo devices

  NRF_init();                             // Initialize NRF radio address, channel, etc
  NRF_CheckOnline();                      // Send data on air to find out who's online
}

void loop() 
{
  if(Serial.available())
  {
    byte command=Serial.read();
    if (command == '?')
    {
      NRF_CheckOnline();
      PrintNodoOnline();
    }
  }

  if(NRF_receive(&NodoEvent))
  {
    Serial.println("Direction NRF -> I2C");
    SendI2C(&NodoEvent);
  }

  if(I2C_Received > 0)
  {
    if(I2C_Received==sizeof(struct NodoEventStruct))
    {
      memcpy(&NodoEvent, &I2C_ReceiveBuffer, sizeof(struct NodoEventStruct));
      Serial.println("Direction I2C -> NRF");
      NodoEvent.Port = VALUE_SOURCE_I2C;
      NodoOnline(NodoEvent.SourceUnit,NodoEvent.Port);
      NRF_send(&NodoEvent);

      if((NodoEvent.Command == EVENT_BOOT || NodoEvent.Command == EVENT_NEWNODO) && NodoEvent.Type == NODO_TYPE_EVENT)
        {
          Serial.print("Confirm I2C: ");
          Serial.println((int)NodoEvent.SourceUnit);
          NodoEvent.DestinationUnit       = NodoEvent.SourceUnit;
          NodoEvent.SourceUnit            = Settings.Unit;
          NodoEvent.Type                  = NODO_TYPE_EVENT;
          NodoEvent.Command               = EVENT_NEWNODO;
          NodoEvent.Par1                  = Settings.Unit;
          NodoEvent.Par2                  = 0L;
          NodoEvent.Flags                 = 0;
          NodoEvent.Port                  = VALUE_SOURCE_I2C;
          NodoEvent.Direction             = 0;
          NodoEvent.Version               = NODO_VERSION_MINOR;
          NodoEvent.Checksum              = 0;
          SendI2C(&NodoEvent);
        }
      I2C_Received=0;
    }
  }
}


