//#######################################################################################################
//#################################### Plugin-011: OpenTherm GateWay plugin #############################
//#######################################################################################################

/*********************************************************************************************\
 * Dit protocol zorgt voor communicatie met een OTGW Opentherm gateway
 * 
 * Auteur             : Martinus van den Broek
 * Support            : www.nodo-domotica.nl
 * Datum              : 10 mrt 2015
 * Versie             : Proof of Concept 0.6! (added some variables)
 * Nodo productnummer : 
 * Compatibiliteit    : Vanaf Nodo build nummer 744
 *
 * Instructie         : Zet de volgende regels in het config bestand
 *
 *				#define PLUGIN_090
 *				#define PLUGIN_011_CORE
 *				#define PLUGIN_011_DEBUG          true
 * 				#define PLUGIN_011_SERIAL_RX_PIN  A0
 * 				#define PLUGIN_011_SERIAL_TX_PIN  A1
 *
 *			Var 1 wordt gebruikt om temperatuur setpoint in te stellen
 *			Var 2 wordt gebruikt voor monitoring van Room Temperature
 *			Var 3 wordt gebruikt voor monitoring van Boiler Water Temperature
 *			Var 4 wordt gebruikt voor monitoring van Modulation
 *			Var 5 wordt gebruikt voor monitoring van Boiler Water Pressure
 *			Var 6 wordt gebruikt voor monitoring van Thermostat Setpoint
 *			Var 7 wordt gebruikt voor monitoring van Flame status
 *			Var 8 wordt gebruikt voor monitoring van Boiler Return Water Temperature
 *			Var 9 wordt gebruikt voor monitoring van Domestic Hot Water Mode
 *
 *			De plugin gebruikt software serial voor communicatie met de otgw
 *                      De plugin geeft conflicten met andere Pin Change Int plugins (019,028,030,???)
 *
\*********************************************************************************************/

#define PLUGIN_ID   11
#define PLUGIN_NAME "OTGW"

#define PLUGIN_011_BUFFERSIZE   40

#ifndef PLUGIN_011_DEBUG
  #define PLUGIN_011_DEBUG false
#endif

char PLUGIN_011_buffer[PLUGIN_011_BUFFERSIZE+1];

// function prototypes for plugin
boolean PLUGIN_011_softSerialSend(char *cmd, int intdelay);
byte PLUGIN_011_SerialRead();
byte PLUGIN_011_hextobyte(char *string, byte pos);
char* PLUGIN_011_int2str(unsigned long x);

// function prototypes software serial
void SoftwareSerial_print(char* string);
void SoftwareSerial_println(char* string);
void SoftwareSerial_init(byte receivePin, byte transmitPin);
inline void SoftwareSerial_tunedDelay(uint16_t delay);
void SoftwareSerial_recv();
void SoftwareSerial_tx_pin_write(uint8_t pin_state);
uint8_t SoftwareSerial_rx_pin_read();
void SoftwareSerial_setTX(uint8_t tx);
void SoftwareSerial_setRX(uint8_t rx);
void SoftwareSerial_end();
int SoftwareSerial_read();
int SoftwareSerial_available();
size_t SoftwareSerial_write(uint8_t b);
int SoftwareSerial_peek();
void SoftwareSerial_flush();

boolean Plugin_011(byte function, struct NodoEventStruct *event, char *string)
  {
  boolean success=false;

  switch(function)
    {    
    #ifdef PLUGIN_011_CORE

    case PLUGIN_INIT:
      {
        SoftwareSerial_init(PLUGIN_011_SERIAL_RX_PIN,PLUGIN_011_SERIAL_TX_PIN);         // RX, TX
        break;
      }

    case PLUGIN_COMMAND:
      {

        if (event->Par1 == CMD_VARIABLE_SET)
          {
            char *cmd=(char*)malloc(10);
            cmd[0]=0;
            byte value=(byte)UserVar[event->Par2-1];
            strcpy(cmd,"TT=");
            strcat(cmd,PLUGIN_011_int2str(value));
            if ((UserVar[event->Par2-1] - value) > 0)
              strcat(cmd,".5");
            PLUGIN_011_softSerialSend(cmd,100);
            free(cmd);
          }

        success = true;
        break;
      }

  case PLUGIN_ONCE_A_SECOND:
    {
      byte count = PLUGIN_011_SerialRead();
      if (count > 0)
        for (byte x=0; x <= count ; x++)
          {
            if (PLUGIN_011_buffer[x]==0x0D) // scan for EOL
              {
                char source = PLUGIN_011_buffer[x-9];
                byte b1 = PLUGIN_011_hextobyte(PLUGIN_011_buffer,x-8);
                byte b2 = PLUGIN_011_hextobyte(PLUGIN_011_buffer,x-6);
                byte b3 = PLUGIN_011_hextobyte(PLUGIN_011_buffer,x-4);
                byte b4 = PLUGIN_011_hextobyte(PLUGIN_011_buffer,x-2);

                if (source=='T')
                  {
                    if (b2 == 0x18)					// Room Temperature
                      {
                         float roomtemp=b3+b4*((float)1/256);
                         Serial.print("RT ");
                         Serial.println(roomtemp);
                         UserVar[1]=roomtemp;				// store to nodo var 2
                      }

                    if (b2 == 0x10)					// Thermostat Set Point
                      {
                         float setpoint=b3+b4*((float)1/256);
                         Serial.print("TC ");
                         Serial.println(setpoint);
                         UserVar[5]=setpoint;				// store to nodo var 6
                      }
                  }


                if (source=='B')
                  {
                    if (b2 == 0x19)					// Boiler Water Temperature
                      {
                         float boilertemp=b3+b4*((float)1/256);
                         Serial.print("BWT ");
                         Serial.println(boilertemp);
                         UserVar[2]=boilertemp;				// store to nodo var 3
                      }
                    if (b2 == 0x11)					// Relative Modulation
                      {
                         float modulation=b3+b4*((float)1/256);
                         Serial.print("MOD ");
                         Serial.println(modulation);
                         UserVar[3]=modulation;				// store to nodo var 4
                      }
                    if (b2 == 0x12)					// Boiler Water Pressure
                      {
                         float pressure=b3+b4*((float)1/256);
                         Serial.print("PR ");
                         Serial.println(pressure);
                         UserVar[4]=pressure;				// store to nodo var 5
                      }
                    if (b2 == 0x00)					// Boiler Status
                      {
                         byte status=(b4 & 0x8) >> 3;			// Flame Status in bit 3
                         Serial.print("FS ");
                         Serial.println(status);
                         UserVar[6]=status;				// store to nodo var 7
                         status=(b4 & 0x4) >> 2;			// DHW Mode in bit 2
                         Serial.print("DHWM ");
                         Serial.println(status);
                         UserVar[8]=status;				// store to nodo var 9
                      }
                    if (b2 == 0x1C)					// Boiler Water Return Temperature
                      {
                         float boilerRtemp=b3+b4*((float)1/256);
                         Serial.print("BWRT ");
                         Serial.println(boilerRtemp);
                         UserVar[7]=boilerRtemp;			// store to nodo var 8
                      }
                  }

              } // eol
          } // for

      break;
    } // case PLUGIN_ONCE_A_SECOND

    #endif // CORE

    #if NODO_MEGA
    case PLUGIN_MMI_IN:
      {
      char *TempStr=(char*)malloc(INPUT_COMMAND_SIZE);

      if(GetArgv(string,TempStr,1))
        {
        if(strcasecmp(TempStr,PLUGIN_NAME)==0)
          {
          if(GetArgv(string,TempStr,2)) 
            {
              event->Type = NODO_TYPE_PLUGIN_COMMAND;
              event->Command = PLUGIN_ID; // Plugin nummer  
              success=true;
            }
          }
        }
      free(TempStr);
      break;
      }

    case PLUGIN_MMI_OUT:
      {
      strcpy(string,PLUGIN_NAME);
      strcat(string," ");
      strcat(string,cmd2str(event->Par1));
      strcat(string,",");
      strcat(string,int2str(event->Par2));
      break;
      }
    #endif //MMI

    }
    return success;
  }

#ifdef PLUGIN_011_CORE
boolean PLUGIN_011_softSerialSend(char *cmd, int intdelay)
{
  SoftwareSerial_flush();

  #if PLUGIN_011_DEBUG
    Serial.println(cmd);
  #endif

  SoftwareSerial_println(cmd);
  delay(intdelay);
}

byte PLUGIN_011_SerialRead()
{
 char received;
 byte count=0;
 PLUGIN_011_buffer[0]=0;
 if (SoftwareSerial_available())
        {
          while (SoftwareSerial_available())
          {
            received=SoftwareSerial_read();
            if(count<PLUGIN_011_BUFFERSIZE)
              PLUGIN_011_buffer[count++]=received;
            Serial.print(received);
          }
          Serial.println();
        }
  PLUGIN_011_buffer[count]=0;
  return count; 
}

byte PLUGIN_011_hextobyte(char *string, byte pos)
{
  byte b1 = string[pos] - '0';
  byte b2 = string[pos+1] -'0';
  if (b1 > 16) b1=b1-7;
  if (b2 > 16) b2=b2-7;
  return b1*16 + b2;
}

char* PLUGIN_011_int2str(unsigned long x)
{
  static char OutputLine[12];
  char* OutputLinePosPtr=&OutputLine[10];
  int y;

  *OutputLinePosPtr=0;

  if(x==0)
    {
    *--OutputLinePosPtr='0';
    }
  else
    {  
    while(x>0)
      {
      *--OutputLinePosPtr='0'+(x%10);
      x/=10;
      }
    }    
  return OutputLinePosPtr;
  }

/*
  Software serial gebaseerd op originele softwareserial lib
  Waarom deze code?
   - Include lib vanuit plugin file werkt niet
   - Code vereenvoudig en gereduceerd ivm Nodo plugin gebruik op Atmel 328 platform
*/

#define _SS_MAX_RX_BUFF         64 // RX buffer size

#if F_CPU == 16000000
  // settings for 16 MHz CPU, 9600 BAUD
  // Intrabit timing slightly adjusted, because program code is reduced
  #define RX_DELAY_CENTERING      114
  #define RX_DELAY_INTRABIT       236 // original 236
  #define RX_DELAY_STOPBIT        236
  #define TX_DELAY                233
  #define XMIT_START_ADJUSTMENT     5
#else
  // settings for 8 MHz CPU, 9600 BAUD
  // Intrabit timing slightly adjusted, because program code is reduced
  #define RX_DELAY_CENTERING      50
  #define RX_DELAY_INTRABIT       114 // original 236
  #define RX_DELAY_STOPBIT        114
  #define TX_DELAY                112
  #define XMIT_START_ADJUSTMENT     4
#endif

uint8_t _receivePin;
uint8_t _receiveBitMask;
volatile uint8_t *_receivePortRegister;
char _receive_buffer[_SS_MAX_RX_BUFF]; 
volatile uint8_t _receive_buffer_tail = 0;
volatile uint8_t _receive_buffer_head = 0;

uint8_t _transmitBitMask;
volatile uint8_t *_transmitPortRegister;

void SoftwareSerial_print(char* string)
{
  byte x=0;
  while (string[x] != 0)
  {
    SoftwareSerial_write(string[x]);
    x++;
  }
}

void SoftwareSerial_println(char* string)
{
  SoftwareSerial_print(string);
  SoftwareSerial_write(13);
  SoftwareSerial_write(10);
}

void SoftwareSerial_init(byte receivePin, byte transmitPin)
{
  SoftwareSerial_setTX(transmitPin);
  SoftwareSerial_setRX(receivePin);
  if (digitalPinToPCICR(_receivePin))
  {
    *digitalPinToPCICR(_receivePin) |= _BV(digitalPinToPCICRbit(_receivePin));
    *digitalPinToPCMSK(_receivePin) |= _BV(digitalPinToPCMSKbit(_receivePin));
  }
  SoftwareSerial_tunedDelay(TX_DELAY); // if we were low this establishes the end
}

inline void SoftwareSerial_tunedDelay(uint16_t delay) { 
  uint8_t tmp=0;

  asm volatile("sbiw    %0, 0x01 \n\t"
    "ldi %1, 0xFF \n\t"
    "cpi %A0, 0xFF \n\t"
    "cpc %B0, %1 \n\t"
    "brne .-10 \n\t"
    : "+w" (delay), "+a" (tmp)
    : "0" (delay)
    );
}

void SoftwareSerial_recv()
{
  uint8_t d = 0;

  // If RX line is high, then we don't see any start bit
  // so interrupt is probably not for us
  if (!SoftwareSerial_rx_pin_read())
  {
    // Wait approximately 1/2 of a bit width to "center" the sample
    SoftwareSerial_tunedDelay(RX_DELAY_CENTERING);

    // Read each of the 8 bits
    for (uint8_t i=0x1; i; i <<= 1)
    {
      SoftwareSerial_tunedDelay(RX_DELAY_INTRABIT);
      uint8_t noti = ~i;
      if (SoftwareSerial_rx_pin_read())
        d |= i;
      else // else clause added to ensure function timing is ~balanced
        d &= noti;
    }

    // skip the stop bit
    SoftwareSerial_tunedDelay(RX_DELAY_STOPBIT);

    if ((_receive_buffer_tail + 1) % _SS_MAX_RX_BUFF != _receive_buffer_head) 
    {
      // save new data in buffer: tail points to where byte goes
      _receive_buffer[_receive_buffer_tail] = d; // save new byte
      _receive_buffer_tail = (_receive_buffer_tail + 1) % _SS_MAX_RX_BUFF;
    } 
  }
}

void SoftwareSerial_tx_pin_write(uint8_t pin_state)
{
  if (pin_state == LOW)
    *_transmitPortRegister &= ~_transmitBitMask;
  else
    *_transmitPortRegister |= _transmitBitMask;
}

uint8_t SoftwareSerial_rx_pin_read()
{
  return *_receivePortRegister & _receiveBitMask;
}

ISR(PCINT0_vect)
{
  SoftwareSerial_recv();
}

ISR(PCINT1_vect)
{
  SoftwareSerial_recv();
}

ISR(PCINT2_vect)
{
  SoftwareSerial_recv();
}

void SoftwareSerial_setTX(uint8_t tx)
{
  pinMode(tx, OUTPUT);
  digitalWrite(tx, HIGH);
  _transmitBitMask = digitalPinToBitMask(tx);
  uint8_t port = digitalPinToPort(tx);
  _transmitPortRegister = portOutputRegister(port);
}

void SoftwareSerial_setRX(uint8_t rx)
{
  pinMode(rx, INPUT);
  digitalWrite(rx, HIGH);  // pullup for normal logic!
  _receivePin = rx;
  _receiveBitMask = digitalPinToBitMask(rx);
  uint8_t port = digitalPinToPort(rx);
  _receivePortRegister = portInputRegister(port);
}

void SoftwareSerial_end()
{
  if (digitalPinToPCMSK(_receivePin))
    *digitalPinToPCMSK(_receivePin) &= ~_BV(digitalPinToPCMSKbit(_receivePin));
}

int SoftwareSerial_read()
{
  // Empty buffer?
  if (_receive_buffer_head == _receive_buffer_tail)
    return -1;

  // Read from "head"
  uint8_t d = _receive_buffer[_receive_buffer_head]; // grab next byte
  _receive_buffer_head = (_receive_buffer_head + 1) % _SS_MAX_RX_BUFF;
  return d;
}

int SoftwareSerial_available()
{
  return (_receive_buffer_tail + _SS_MAX_RX_BUFF - _receive_buffer_head) % _SS_MAX_RX_BUFF;
}

size_t SoftwareSerial_write(uint8_t b)
{
  uint8_t oldSREG = SREG;
  cli();  // turn off interrupts for a clean txmit

  // Write the start bit
  SoftwareSerial_tx_pin_write(LOW);
  SoftwareSerial_tunedDelay(TX_DELAY + XMIT_START_ADJUSTMENT);

  // Write each of the 8 bits
  for (byte mask = 0x01; mask; mask <<= 1)
  {
    if (b & mask) // choose bit
      SoftwareSerial_tx_pin_write(HIGH); // send 1
    else
      SoftwareSerial_tx_pin_write(LOW); // send 0
  
    SoftwareSerial_tunedDelay(TX_DELAY);
  }
  SoftwareSerial_tx_pin_write(HIGH); // restore pin to natural state

  SREG = oldSREG; // turn interrupts back on
  SoftwareSerial_tunedDelay(TX_DELAY);
  
  return 1;
}

int SoftwareSerial_peek()
{
  // Empty buffer?
  if (_receive_buffer_head == _receive_buffer_tail)
    return -1;

  // Read from "head"
  return _receive_buffer[_receive_buffer_head];
}

void SoftwareSerial_flush()
{
  uint8_t oldSREG = SREG;
  cli();
  _receive_buffer_head = _receive_buffer_tail = 0;
  SREG = oldSREG;
}

#endif