//#######################################################################################################
//################################# Plugin-30: Wiegand RFID Tag Reader ##################################
//#######################################################################################################

/*********************************************************************************************\
 * Dit protocol zorgt voor verwerking van RFID Tag Readers die werken met het Wiegand-26 protocol
 * 
 * Auteur             : Martinus van den Broek / Richard Ten Klooster
 * Support            : www.nodo-domotica.nl
 * Datum              : 25 Jan 2014
 * Versie             : 0.3
 * Nodo productnummer : 
 * Compatibiliteit    : Vanaf Nodo build nummer 596
 * Syntax             : "RFIDWG 0,<tag ID>
 ***********************************************************************************************
 * Technische beschrijving:
 *
 * Een Wiegand-26 Tag Reader is een device die 125KHz EM4001 compatible tags kan uitlezen. De data wordt via het Wiegand 26 protocol aangeboden
 * Dit betekent dat er twee datalijnen zijn welke korte pulsen genereren voor respectievelijk 0 en 1. Pulsen zijn zeer kort (50uSec) dus IRQ is nodig.
 * We gebruiken de PIN-CHANGE interrupt voorziening van de Atmel.
 * We gebuiken hiervoor twee van de WiredIn aansluitingen van de Nodo
 * We maken dus gebruik van een analoge input die we als digitale input met IRQ support gaan gebruiken
 * Op de ATMEGA328P platformen gebruiken we poort A0 en A1
 * Op de ATMEGA2560 platformen gebruiken we poort A8 en A9
 * Elke EM4001 tag levert een unieke code, bestaande uit een 26 bits serienummer
 * Par1 bevat het altijd waarde 0
 * Par2 bevat het 26 bits serienummer
 * Dit device genereert een event, zodat actie kan worden ondernomen via de Nodo eventlist indien een bekende tag wordt gebruikt
 \*********************************************************************************************/

#define PLUGIN_NAME "RFIDWG"
#define PLUGIN_ID   30

#include "Arduino.h"
#include <avr/interrupt.h>

void SetPinIRQ(byte _receivePin);
inline void pulse_handle_interrupt();

volatile uint8_t *_receivePortRegister;

volatile int bitCount = 0;	// Count the number of bits received.
unsigned long keyBuffer = 0;	// A 32-bit-long keyBuffer into which the number is stored.

int codeSize = 26; // Assuming it's the Wiegand 26 protocol, there are 26 bits per number.

// In de funktienaam zit het pluginnummer verwerkt zodat deze eenduidig kan worden geidentificeerd en aangeroepen. De Nodo
boolean Plugin_030(byte function, struct NodoEventStruct *event, char *string)
  {
  boolean success=false;

  switch(function)
    {    
    #ifdef PLUGIN_030_CORE

    case PLUGIN_INIT:
      {
        // Init IO pins
        #if NODO_MEGA
          SetPinIRQ(A8);
          SetPinIRQ(A9);
        #else
          SetPinIRQ(A0);
          SetPinIRQ(A1);
        #endif
        break;
      }

    case PLUGIN_ONCE_A_SECOND:
      {
        if (bitCount == codeSize)
        {
          bitCount = 0;          // Read in the current key and reset everything so that the interrupts can

          keyBuffer = keyBuffer >> 1;          // Strip leading and trailing parity bits from the keyBuffer
          keyBuffer &= 0xFFFFFF;

          struct NodoEventStruct TempEvent;
          ClearEvent(&TempEvent);
          TempEvent.SourceUnit = Settings.Unit;
          TempEvent.Direction  = VALUE_DIRECTION_INPUT;
          TempEvent.Command    = PLUGIN_ID;
          TempEvent.Port       = VALUE_SOURCE_WIRED;
          TempEvent.Par1       = 0;
          TempEvent.Par2       = keyBuffer;
          TempEvent.Type       = NODO_TYPE_PLUGIN_EVENT;
          ProcessEvent(&TempEvent);

          keyBuffer = 0;          // Clear the buffer for the next iteration.
        }
      break;
      }
    #endif // CORE
    
    #if NODO_MEGA // alleen relevant voor een Nodo Mega want de Small heeft geen MMI!
     case PLUGIN_MMI_IN:
    {
    char* str=(char*)malloc(40);
    string[25]=0; // kap voor de zekerheid de string af.
  
    if(GetArgv(string,str,1))
      {
      if(strcasecmp(str,PLUGIN_NAME)==0)
        {
        if(GetArgv(string,str,2))
          {
          event->Par1=str2int(str);    
          if(GetArgv(string,str,3))
            {
              event->Par2=str2int(str);
              event->Type = NODO_TYPE_PLUGIN_EVENT;
              event->Command = PLUGIN_ID;   
              success=true;
            }
          }
        }
      }
    free(str);
    break;
    }
     
    case PLUGIN_MMI_OUT:
      {
        strcpy(string,PLUGIN_NAME);               // Commando 
        strcat(string," ");
        strcat(string,int2str(event->Par1));      // Parameter-1 (8-bit)
        strcat(string,",");
        strcat(string,int2strhex(event->Par2));
        break;
      }
    #endif //MMI
    }
    
  return success;
  }

#ifdef PLUGIN_030_CORE
void SetPinIRQ(byte _receivePin)
{
  uint8_t _receiveBitMask;
  pinMode(_receivePin, INPUT);
  digitalWrite(_receivePin, HIGH);
  _receiveBitMask = digitalPinToBitMask(_receivePin);
  uint8_t port = digitalPinToPort(_receivePin);
  _receivePortRegister = portInputRegister(port);
  if (digitalPinToPCICR(_receivePin))
    {
      *digitalPinToPCICR(_receivePin) |= _BV(digitalPinToPCICRbit(_receivePin));
      *digitalPinToPCMSK(_receivePin) |= _BV(digitalPinToPCMSKbit(_receivePin));
    }
}


/*********************************************************************/
inline void pulse_handle_interrupt()
/*********************************************************************/
{
  byte portstate= *_receivePortRegister & 0x3;

  switch(portstate)
    {
       case 0:			         // Should never happen with Wiegand protocol...
       break;
       case 1: 			         // We've received a 1 bit. (bit 0 = high, bit 1 = low)
         keyBuffer = keyBuffer << 1;     // Left shift the number (effectively multiplying by 2)
         keyBuffer += 1;    		 // Add the 1 (not necessary for the zeroes)
         bitCount++;   			 // Increment the bit count
       break;
       case 2:				 // We've received a 0 bit. (bit 0 = low, bit 1 = high)
         keyBuffer = keyBuffer << 1;     // Left shift the number (effectively multiplying by 2)
         bitCount++;       		 // Increment the bit count
         break;
       case 3:				 // Back to "idle state"
       break;
     }
}

#if defined(PCINT1_vect)
ISR(PCINT1_vect) { pulse_handle_interrupt(); }
#endif
#if defined(PCINT2_vect)
ISR(PCINT2_vect) { pulse_handle_interrupt(); }
#endif

#endif // CORE
