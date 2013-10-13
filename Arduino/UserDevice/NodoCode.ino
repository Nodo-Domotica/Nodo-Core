
//                !!! UNDER CONSTRUCTION !!!


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
* |    | Nodo_DataSend()|                                     |  MyPlugin    |       |
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
* Ontwikkel in het tabblad MyDevice je eigen oplossing. Met de funkties in tabblad NodoCode 
* is het mogelijk om gegevens via I2C uit te wisselen met een Nodo. Ontwikkel aan de Nodo-zijde een
* bijbehorende plugin die zorg draagt voor verwerking van de uitgewisselde gegevens.
*
* Naast het uitwisselen van gegevens tussen MyDevice en de Nodo plugin, is het mogelijk
* om een UserEvent en een Variable te versturen die direct door een Nodo kunnen worden
* verwerkt.
*
* Kijk in het voorbeeldcode MyDevice en Nodo-Plugin 'Plugin_255' voor verdere toelichting.
*
\*********************************************************************************************/

#define I2C_START_ADDRESS               1 // Alle Nodo's op de I2C bus hebben een uniek adres dat start vanaf dit nummer. Er zijn max. 32 Nodo's. Let op overlap met andere devices. RTC zit op adres 104.

// Definites vanuit de Nodo code
#define NODO_VERSION_MAJOR              3 
#define NODO_TYPE_EVENT                 1
#define NODO_TYPE_PLUGIN_DATA           6
#define NODO_TYPE_COMMAND               2
#define UNIT_MAX                       32

// Commando's en events
#define EVENT_BOOT                      1
#define EVENT_USEREVENT                 3
#define EVENT_VARIABLE                  4

#include <Wire.h>


// Deze struct definieert een Nodo even
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
  }NodoEventIncomming;

boolean NodoEventReceived=false;       // Als deze vlag staat, is er een I2C event binnengekomen.
byte NodoPluginID=0;
byte UserDeviceUnit=0;
boolean NodoOnline[UNIT_MAX+1];
boolean FirstTime=true;
unsigned long DelayBetweenSend=0;

/*********************************************************************************************\
*
*
\*********************************************************************************************/
void NodoInit(byte Unit, byte Plugin)
  {
  UserDeviceUnit    = Unit;
  NodoPluginID      = PLUGIN_ID;
  
  Wire.begin(I2C_START_ADDRESS + Unit-1);
  Wire.onReceive(ReceiveI2C);   // verwijs naar ontvangstroutine

  // moeten we nog een event verzenden om dit device bekend te maken???
  }


/*********************************************************************************************\
* Verzend de Par1 en Par2 data naar de plugin met ID <Plugin> die draait in Nodo <Unit>
* Als <Unit> nul is, dan verzenden naar alle Nodo's.
\*********************************************************************************************/
void Nodo_DataSend(byte Unit, byte Par1, unsigned long Par2)
  {
  struct NodoEventStruct NodoEvent;
  NodoEvent.Type            = NODO_TYPE_PLUGIN_DATA;
  NodoEvent.Command         = NodoPluginID;
  NodoEvent.Par1            = Par1; 
  NodoEvent.Par2            = Par2;
  NodoEvent.DestinationUnit = Unit;
  
  NodoEventSend(&NodoEvent);
  }

/*********************************************************************************************\
* Verzend een UserEvent.
\*********************************************************************************************/
void Nodo_UserEventSend(byte Par1, unsigned long Par2)
  {
  struct NodoEventStruct NodoEvent;
  NodoEvent.Type            = NODO_TYPE_EVENT;
  NodoEvent.Command         = EVENT_USEREVENT;
  NodoEvent.Par1            = Par1; 
  NodoEvent.Par2            = Par2;
  NodoEvent.DestinationUnit = 0;
  
  NodoEventSend(&NodoEvent);
  }


/*********************************************************************************************\
* Verzend een Variabele. 
\*********************************************************************************************/
void Nodo_VariableSend(byte Par1, float f)
  {
  unsigned long ul;
  memcpy(&ul, &f,4);
  
  struct NodoEventStruct NodoEvent;
  NodoEvent.Type            = NODO_TYPE_EVENT;
  NodoEvent.Command         = EVENT_VARIABLE;
  NodoEvent.Par1            = Par1; 
  NodoEvent.Par2            = ul;
  NodoEvent.DestinationUnit = 0;
  
  NodoEventSend(&NodoEvent);
  }

/*********************************************************************************************\
*
*
\*********************************************************************************************/
#define DELAY_SEND                    100 // Minimale tijd tussen twee zend acties.

void NodoEventSend(struct NodoEventStruct *NodoEventOutgoing)
  {
  byte *B=(byte*)NodoEventOutgoing;

  NodoEventOutgoing->SourceUnit      = UserDeviceUnit;
  NodoEventOutgoing->Flags           = 0;
  NodoEventOutgoing->Port            = 0;
  NodoEventOutgoing->Direction       = 0;
  NodoEventOutgoing->Version         = 0;

  Checksum(NodoEventOutgoing);// bereken checksum: crc-8 uit alle bytes in de struct.

  while(DelayBetweenSend > millis() );
  DelayBetweenSend=millis()+DELAY_SEND;

  for(int y=1;y<=UNIT_MAX;y++)
    {            
    // We sturen de events naar bekende Nodo's die zich op de I2C bus bevinden. Tenzij een specifiek unitnummer is opgegeven.
    if(y==NodoEventOutgoing->DestinationUnit || ( NodoEventOutgoing->DestinationUnit==0 && NodoOnline[y]) || FirstTime )
      {
      // verzend Event 
      Wire.beginTransmission(I2C_START_ADDRESS+y-1);
      for(int x=0;x<sizeof(struct NodoEventStruct);x++)
        Wire.write(*(B+x));

      Wire.endTransmission(false); // verzend de data, sluit af en geef de bus NOG NIET vrij.
      }
    }
  Wire.endTransmission(true); // verzend de data, sluit af en geef de bus vrij.
  }


void I2C_Send(char* string)
  {
  while(DelayBetweenSend > millis() );
  DelayBetweenSend=millis()+DELAY_SEND;

  Wire.beginTransmission(I2C_START_ADDRESS+1-1);
  for(int x=0;x<strlen(string);x++)
    Wire.write(*(string+x));
  Wire.endTransmission(true); // verzend de data, sluit af en geef de bus vrij.
  }



/*********************************************************************************************\
* Als er een event binnen is gekomen dan worden de waarden voor Par1 en Par2 gevuld.
* De routine geeft een nul terug als er geen data gereed staat. Is er een event ontvangen
* dan wordt het nummer geretourneerd van de Nodo die het event verzonden heeft.
*
* LET OP: Par1 en Par2 => call by reference !!!
\*********************************************************************************************/  
byte NodoReceive(byte *Par1, unsigned long *Par2)
  {
  if(NodoEventReceived)
    {
    NodoEventReceived     = false;
    *Par1    = NodoEventIncomming.Par1;
    *Par2    = NodoEventIncomming.Par2;
    return NodoEventIncomming.SourceUnit;
    }
  return 0;
  }


/*********************************************************************************************\
*
*
\*********************************************************************************************/  
void ReceiveI2C(int n)
  {
  byte *B=(byte*)&NodoEventIncomming;
  int x=0;

  while(Wire.available()) // Haal de bytes op
    {
    if(x<sizeof(struct NodoEventStruct))
      *(B+x)=Wire.read(); 
    x++;
    }


  // laatste ontvangen byte bevat de checksum. Als deze gelijk is aan de berekende checksum, dan event uitvoeren
  if(Checksum(&NodoEventIncomming))    
    {
    // Maak eerste keer de tabel leeg.
    if(FirstTime)
      {
      FirstTime=false;
      for(x=0;x<=UNIT_MAX;x++)
        NodoOnline[x]=false;
      }

    NodoOnline[NodoEventIncomming.SourceUnit]=true;

    if(   NodoEventIncomming.Type     == NODO_TYPE_PLUGIN_DATA                                         // Juiste type event
       && NodoEventIncomming.Command  == NodoPluginID                                                  // Plugin ID 
       && (NodoEventIncomming.DestinationUnit==NodoPluginID || NodoEventIncomming.DestinationUnit==0)) // Unit nummer komt overeen of wildcard
      NodoEventReceived=true;    
    }
  else
    NodoEventReceived=false;
  }

/*********************************************************************************************\
 * Deze funktie berekend de CRC-8 checksum uit van een NodoEventStruct. 
 * Als de Checksum al correct gevuld was wordt er een true teruggegeven. Was dit niet het geval
 * dan wordt NodoEventStruct.Checksum gevuld met de juiste waarde en een false teruggegeven.
 \*********************************************************************************************/
boolean Checksum(NodoEventStruct *event)
  {
  byte OldChecksum=event->Checksum;
  byte NewChecksum=NODO_VERSION_MAJOR;  // Verwerk versie in checksum om communicatie tussen verschillende versies te voorkomen

  event->Checksum=0; // anders levert de beginsituatie een andere checksum op

  for(int x=0;x<sizeof(struct NodoEventStruct);x++)
    NewChecksum^(*((byte*)event+x)); 

  event->Checksum=NewChecksum;
  return(OldChecksum==NewChecksum);
  }
