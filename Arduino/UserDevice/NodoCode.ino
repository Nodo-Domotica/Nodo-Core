/*************************** N O D O - D E V I C E ***********************************************************************\
* 
* Arduino project "Nodo" © Copyright 2013 Paul Tonkes 
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
* Voor vragen of suggesties, mail naar              : p.k.tonkes@gmail.com
* Compiler                                          : Arduino Compiler met minimaal versie 1.0.1
**************************************************************************************************************************
* 
* In sommige gevallen is het noodzakelijk of handig om taken buiten de Nodo uit te voeren. Dit geldt in het bijzonder voor
* taken die zeer tijdkritisch zijn of die veer programma en/of verwerkingsgeheugen vragen. In deze gevallen is het aan te 
* bevelen om hiervoor een eigen ATMega microcontroller in te zetten die communiceert met een Nodo.
* 
* Met dit programma is het mogelijk om een eigen device te ontwikkelen die op een Arduino draait, maar waarmee het mogelijk is
* om via I2C gegevens uit te wisselen met een Nodo.
* 
* +---------------------+                                     +----------------------+
* |     MyDevice        |                                     |         Nodo         |
* |     software        |                                     |       software       |
* |                     |                                     |                      |
* |                     |                                     |                      |
* |    +----------------+                                     +--------------+       |
* |    | NodoSend();    |                                     |  MyDevice    |       |
* |    | NodoReceive(); |                                     |  Nodo-code   |       |
* |    | NodoInit();    |                                     |              |       |
* +----+----------------+                                     +--------------+       |
* |  I2C-Library        |------------ SDA --------------------|                      |
* |                     |------------ SLC --------------------|                      |
* |                     |------------ GND --------------------|                      |
* +---------------------+                                     +----------------------+
*    MyDevice hardware                                              Nodo hardware
* 
* 
* Ontwikkel in het tabblad MyDevice je eigen oplossing. Met de funkties in tabblad NodoCommunication 
* is het mogelijk om gegevens via I2C uit te wisselen met een Nodo. Ontwikkel aan de Nodo-zijde een
* bijbehorend stukje Nodo-code (software device) die zorg draagt voor verwerking van de uitgewisselde
* gegevens.
*
* Gegevensuitwisseling vindt plaats m.b.v. de onderstaande struct:
*
* struct NodoDataStruct                            // De data die wordt uitgewisseld met de Nodo wordt opgeslagen in een struct. Deze struct wordt opgenomen in een Nodo event.
*   {
*   byte           Par1;                           // Vrij te gebruiken: byte ter grootte van 8-bits.
*   unsigned long  Par2;                           // Vrij te gebruikern: unsigned long van 32-bits.
*   byte           Unit;                           // Nodo unitnummer waar de data naar toegezonden mmoet worden of vandaan komt.
*   }MyData;
*
* Kijk in het voorbeeldcode MyDevice en Nodo-device 'Device_255' voor verdere toelichting.
*
\*********************************************************************************************/

#define I2C_START_ADDRESS               1 // Alle Nodo's op de I2C bus hebben een uniek adres dat start vanaf dit nummer. Er zijn max. 32 Nodo's. Let op overlap met andere devices. RTC zit op adres 104.

// Definites vanuit de Nodo code
#define NODO_COMPATIBILITY              2
#define NODO_TYPE_DEVICE_DATA           6
#define UNIT_MAX                       32
#define EVENT_BOOT                      1
#define CMD_REBOOT                      4
#define CMD_SOUND                       5
#define EVENT_USEREVENT                 6
#define EVENT_VARIABLE                  7
#define CMD_VARIABLE_SET                8

#include <Wire.h>

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
  }NodoEventIncomming;

boolean NodoEventReceived=false;       // Als deze vlag staat, is er een I2C event binnengekomen.
byte NodoDeviceUnit=0;
byte NodoDeviceID=0;
boolean NodoOnline[UNIT_MAX+1];
boolean FirstTime=true;

void NodoInit(byte Unit, byte Device)
  {
  NodoDeviceUnit    = Unit;
  NodoDeviceID      = Device;
  
  Wire.begin(I2C_START_ADDRESS + Unit-1);
  Wire.onReceive(ReceiveI2C);   // verwijs naar ontvangstroutine

  // moeten we nog een event verzenden om dit device bekend te maken???
  }


void NodoDataSend(byte Unit, byte Par1, unsigned long Par2)
  {
  struct NodoEventStruct NodoEvent;
  NodoEvent.Type            = NODO_TYPE_DEVICE_DATA;
  NodoEvent.Command         = NodoDeviceID;
  NodoEvent.Par1            = Par1; 
  NodoEvent.Par2            = Par2;
  NodoEvent.SourceUnit      = NodoDeviceUnit;
  NodoEvent.DestinationUnit = Unit;
  NodoEvent.Flags           = 0;
  NodoEvent.Port            = 0;
  NodoEvent.Direction       = 0;
  
  NodoEventSend(&NodoEvent);
  }

void NodoEventSend(struct NodoEventStruct *NodoEventOutgoing)
  {
  byte b,*B=(byte*)NodoEventOutgoing;
  byte Checksum;

  for(int y=1;y<=UNIT_MAX;y++)
    {            
    // We sturen de events naar bekende Nodo's die zich op de I2C bus bevinden. Tenzij een specifiek unitnummer is opgegeven.
    if(y==NodoEventOutgoing->DestinationUnit || ( NodoEventOutgoing->DestinationUnit==0 && NodoOnline[y]) || FirstTime )
      {
      Checksum=0;

      // verzend Event 
      Wire.beginTransmission(I2C_START_ADDRESS+y-1);
      for(int x=0;x<sizeof(struct NodoEventStruct);x++)
        {
        b=*(B+x); 
        Wire.write(b);
        Checksum^=b; 
        }
      Checksum^=(NODO_COMPATIBILITY & 0xff); // Verwerk build in checksum om communicatie tussen verschillende versies te voorkomen 
      Wire.write(Checksum); 
      Wire.endTransmission(false); // verzend de data, sluit af en geef de bus NOG NIET vrij.
      }
    }
  Wire.endTransmission(true); // verzend de data, sluit af en geef de bus vrij.
  }

  
boolean NodoReceive(byte *Unit, byte *Par1, unsigned long *Par2)
  {
  if(NodoEventReceived)
    {
    NodoEventReceived     = false;
    *Par1    = NodoEventIncomming.Par1;
    *Par2    = NodoEventIncomming.Par2;
    *Unit    = NodoEventIncomming.SourceUnit;
    return true;
    }
  return false;
  }
  
void ReceiveI2C(int n)
  {
  byte b,*B=(byte*)&NodoEventIncomming;
  byte Checksum=0;
  int x=0;

  while(Wire.available()) // Haal de bytes op
    {
    b=Wire.read(); 
    if(x<sizeof(struct NodoEventStruct))
      {
      *(B+x)=b; 
      Checksum^=b; 
      }
    Checksum^=(NODO_COMPATIBILITY & 0xff); // Verwerk build in checksum om communicatie tussen verschillende versies te voorkomen 
    x++;
    }

  // laatste ontvangen byte bevat de checksum. Als deze gelijk is aan de berekende checksum, dan event uitvoeren
  if(b==Checksum)    
    {
    // Maak eerste keer de tabel leeg.
    if(FirstTime)
      {
      FirstTime=false;
      for(x=0;x<=UNIT_MAX;x++)
        NodoOnline[x]=false;
      }

    NodoOnline[NodoEventIncomming.SourceUnit]=true;

    if(   NodoEventIncomming.Type     == NODO_TYPE_DEVICE_DATA                                         // Juiste type event
       && NodoEventIncomming.Command  == NodoDeviceID                                                  // Device ID 
       && (NodoEventIncomming.DestinationUnit==NodoDeviceID || NodoEventIncomming.DestinationUnit==0)) // Unit nummer komt overeen of wildcard
      NodoEventReceived=true;    
    }
  else
    NodoEventReceived=false;
  }

