//#######################################################################################################
//#################################### Plugin-033: NRF24L01 plugin ######################################
//#######################################################################################################

/*********************************************************************************************\
 * Dit protocol zorgt voor communicatie met de NRF24L01 2.4 GHz Transceiver
 * 
 * Auteur             : Martinus van den Broek
 * Support            : Beta !!
 * Datum              : 21 Dec 2014 (bugfix in single master mode)
 * Versie             : 0.10
 * Nodo productnummer : 
 * Compatibiliteit    : Vanaf Nodo build nummer 765 voor Sendto (!!!!)
 *
 * 			en deze regel moet zijn opgenomen in de unit config file:
 *				#define NODO_BETA_PLUGIN_SENDTO
 *
 * Syntax             : De plugin kan worden gebruikt zonder commando's!
 *                      
 *                      De volgende commando's zijn nodig voor batterij gebruik
 *                      waarbij de NRF wordt uitgeschakeld tijdens Nodo sleep mode
 *                      Na het inschakelen:
 *                        nrf reset		init device
 *                      Voor het uitschakelen:
 *                        nrf off		turn off device
 *
 *                      Alleen op de Mega:
 *                        nrf rf,<channel>	set RF channel (1-62)
 *                        nrf status,<unit>	check connectivity/retries
 \*********************************************************************************************/
#ifndef NRF_DEBUG
  #define NRF_DEBUG false
#endif

#ifndef NRF_CSN_PIN
  #if NODO_MEGA
    #define NRF_CSN_PIN  A12
    #define NRF_MOSI_PIN A13
    #define NRF_MISO_PIN A14
    #define NRF_SCK_PIN  A15
  #else
    #define NRF_CSN_PIN  A0
    #define NRF_MOSI_PIN A1
    #define NRF_MISO_PIN A2
    #define NRF_SCK_PIN  A3
  #endif
#endif

#ifndef NRF_ADDRESS
  #define NRF_ADDRESS               2,3,4,5 // last 4 bytes of address, 1st byte is controlled by plugin
#endif
#ifndef NRF_CHANNEL
  #define NRF_CHANNEL               36 // Mega defaults to channel 36
#endif

#ifdef NRF_HWSPI		// test purposes for std hardware, not officially supported !!!
  #if NODO_MEGA
    #define NRF_CSN_PIN  40
    #define NRF_CE_PIN   41
  #else
    #define NRF_CSN_PIN   7
    #define NRF_CE_PIN    8
  #endif
#endif

#define PLUGIN_ID 33
#define PLUGIN_NAME_033 "NRF"

#define NRF_PAYLOAD_SIZE	  32
#define NRF_UNIT_MAX              32
#define NRF_SEND_RETRIES           3
#define NRF_RETRY_DELAYMS         50
#define NRF_SEND_TIMEOUTMS        150

#define NRF_PAYLOAD_NODO           0
#define NRF_PAYLOAD_PINGREQ        4
#define NRF_PAYLOAD_PINGREP        5
#define NRF_PAYLOAD_CHANNEL      254
#define NRF_PAYLOAD_ONLINE       255

#define NRF_EN_AA       0x01		// Register for Enable Auto Acknowledge configuration register
#define NRF_RF_SETUP    0x06		// Register for Set Data Rate
#define NRF_SETUP_RETR  0x04		// Register for Retry delay and count
#define NRF_EN_RXADDR   0x02		// Register for RX enable receive pipes
#define NRF_EN_RXADDR   0x02

#ifdef PLUGIN_033_CORE

#define NRF_ADDR_LEN	5
#define NRF_CONFIG_DATA ((1<<EN_CRC) | (0<<CRCO) )

void SPI_begin();
unsigned char SPI_transfer(unsigned char Byte);

// NRF function prototypes
void Nrf24_setChannel(byte channel);
void Nrf24_init();
void Nrf24_config();
byte Nrf24_send(uint8_t *value);
void Nrf24_setRADDR(uint8_t * adr);
void Nrf24_setTADDR(uint8_t * adr);
bool Nrf24_dataReady();
bool Nrf24_rxFifoEmpty();
bool Nrf24_txFifoEmpty();
void Nrf24_getData(uint8_t * data);
uint8_t Nrf24_getStatus();
void Nrf24_transmitSync(uint8_t *dataout,uint8_t len);
void Nrf24_transferSync(uint8_t *dataout,uint8_t *datain,uint8_t len);
void Nrf24_configRegister(uint8_t reg, uint8_t value);
void Nrf24_readRegister(uint8_t reg, uint8_t * value, uint8_t len);
void Nrf24_writeRegister(uint8_t reg, uint8_t * value, uint8_t len);
void Nrf24_powerUpRx();
void Nrf24_powerUpTx();
void Nrf24_powerDown();
void Nrf24_csnHi();
void Nrf24_csnLow();
void Nrf24_flushRx();
void Nrf24_flushTx();

// NRF Variables
uint8_t Nrf24_channel=NRF_CHANNEL;

boolean NRF_init(void);
byte NRF_findMaster();
void NRF_changeChannel(byte channel);
void NRF_CheckOnline();
byte NRF_sendpacket(byte Source, byte Destination, byte ID, byte Size, byte retries);

boolean NRFOnline[NRF_UNIT_MAX+1];
byte NRF_single_target=0;

#if NRF_DEBUG
  boolean NRF_debug = true;
#endif

#endif // plugin core

byte NRF_address[5] = { 1,NRF_ADDRESS };
byte NRF_status=0;
boolean NRF_live=false;

struct NRFPayloadStruct
{
  byte Source;
  byte Destination;
  byte ID;
  byte Size;
  byte Sequence;
  byte Data[27];
}
NRFPayload;

boolean Plugin_033(byte function, struct NodoEventStruct *event, char *string)
{
  boolean success=false;

  switch(function)
  {
#ifdef PLUGIN_033_CORE
    case PLUGIN_INIT:
      {
        NRF_live=NRF_init();
        #ifdef NRF_FINDMASTER
          if (Nrf24_channel==0) NRF_findMaster();
        #endif
        #ifndef NRF_SINGLE_MASTER_NODE
          NRF_CheckOnline();
        #endif
        break;
      }

#ifdef NODO_BETA_PLUGIN_SENDTO
  case PLUGIN_SCAN_EVENT:
    {
      if(NRF_live && Nrf24_dataReady())
        {
          Nrf24_getData((byte *)&NRFPayload);

          #if NRF_DEBUG
            if (NRF_debug)
              {
                Serial.println("NRF");
              }
          #endif

          switch(NRFPayload.ID)
            {
            case NRF_PAYLOAD_NODO:
            {
              memcpy((byte*)event, (byte*)&NRFPayload+5,sizeof(struct NodoEventStruct));
              event->Direction = VALUE_DIRECTION_INPUT;
              event->Port      = VALUE_SOURCE_RF;
              NRFOnline[event->SourceUnit]=true;

              #if NRF_DEBUG
                if (NRF_debug)
                  {
                    Serial.print("<T ");
                    Serial.print((int)event->Type);
                    Serial.print(" S ");
                    Serial.print((int)event->SourceUnit);
                    Serial.print(" D ");
                    Serial.print((int)event->DestinationUnit);
                    Serial.print(" C ");
                    Serial.print((int)event->Command);
                    Serial.print(" F ");
                    Serial.print((int)event->Flags);
                    Serial.print(" ST ");
                    Serial.println((int)NRF_single_target);
                  }
              #endif

              // experimental
              // this code is to speed up sendto eventlistshow
              // remember peer node during sendto 
              if ((event->Type == NODO_TYPE_SYSTEM) && (event->Command == SYSTEM_COMMAND_QUEUE_SENDTO) && (event->DestinationUnit == Settings.Unit))
                {
                  NRF_single_target = event->SourceUnit;
                  #if NRF_DEBUG
                    if (NRF_debug)
                      {
                        Serial.print("SQ to ");
                        Serial.println((int)NRF_single_target);
                      }
                  #endif
                }

              success=true;
              break;
            }

            case NRF_PAYLOAD_CHANNEL:
            {
              Nrf24_channel = NRFPayload.Data[0];
              Nrf24_setChannel(Nrf24_channel);
              #if NRF_DEBUG
                if (NRF_debug)
                  {
                    Serial.println(Nrf24_channel);
                  }
              #endif
              for (byte x=1; x < Settings.Unit+2; x++)
                 {
                   #if NRF_DEBUG
                     if (NRF_debug)
                       {
                         Serial.print("Wait:");
                         Serial.print(x);
                         Serial.print('/');
                         Serial.println(Settings.Unit+2);
                        }
                   #endif
                   delay(3000);
                   Nrf24_flushRx();
                 }
              #ifndef NRF_SINGLE_MASTER_NODE
                NRF_CheckOnline();
              #endif
              break;
            }

          }
        }
      break;
    } // case PLUGIN_SCAN_EVENT
#else
  case PLUGIN_ONCE_A_SECOND:
    {
      while(NRF_live && Nrf24_dataReady())
        {
          Nrf24_getData((byte *)&NRFPayload);

          switch(NRFPayload.ID)
            {

            case NRF_PAYLOAD_NODO:
            {
              struct NodoEventStruct TempEvent;
              memcpy((byte*)&TempEvent, (byte*)&NRFPayload+5,sizeof(struct NodoEventStruct));
              TempEvent.Direction = VALUE_DIRECTION_INPUT;
              TempEvent.Port      = VALUE_SOURCE_RF;
              NRFOnline[event->SourceUnit]=true;
              ProcessEvent(&TempEvent);
              success=true;
              break;
            }

            case NRF_PAYLOAD_CHANNEL:
            {
              Nrf24_channel = NRFPayload.Data[0];
              Nrf24_setChannel(Nrf24_channel);
              #if NRF_DEBUG
                if (NRF_debug)
                  {
                    Serial.println(Nrf24_channel);
                  }
              #endif
              for (byte x=1; x < Settings.Unit+2; x++)
                 {
                   #if NRF_DEBUG
                     if (NRF_debug)
                       {
                         Serial.print("Wait:");
                         Serial.print(x);
                         Serial.print('/');
                         Serial.println(Settings.Unit+2);
                       }
                   #endif
                   delay(3000);
                   Nrf24_flushRx();
                 }
              #ifndef NRF_SINGLE_MASTER_NODE
                NRF_CheckOnline();
              #endif
              break;
            }

          }
        }
      break;
    } // case PLUGIN_ONCE_A_SECOND
#endif

  case PLUGIN_COMMAND:
    {

      #if NRF_DEBUG
      if (event->Par1 == CMD_DEBUG)      
        {
          if (event->Par2 == VALUE_OFF) NRF_debug = false;
          if (event->Par2 == VALUE_ON) NRF_debug = true;
        }
      #endif

      if (event->Par1 == CMD_RESET)
        {
          NRF_live=NRF_init();
          #ifdef NRF_FINDMASTER
            if (Nrf24_channel==0) NRF_findMaster();
          #endif
          #ifndef NRF_SINGLE_MASTER_NODE
            NRF_CheckOnline();
          #endif
        }

      if (event->Par1 == VALUE_OFF)
        {
          NRF_live=false;
        }

      #if NODO_MEGA
      if (event->Par1 == VALUE_SOURCE_RF)
        {
          #ifdef NRF_FINDMASTER
            if (event->Par2 == 0) NRF_findMaster();
          #endif
          if ((event->Par2 >= 1) && (event->Par2 <= 62))  // Select channel between 1 <> 62
            {
              Nrf24_channel = event->Par2;
              NRF_changeChannel(Nrf24_channel);                             // broadcast change to all channels!
              Nrf24_setChannel(Nrf24_channel);
              NRF_CheckOnline();                      // Send data on air to find out who's online
            }
        }

      if (event->Par1 == CMD_STATUS)
        {
           unsigned long NRF_roundtrip=millis();
           NRF_status= NRF_sendpacket(Settings.Unit, event->Par2, NRF_PAYLOAD_PINGREQ, 0,0);
           Serial.print("status ");
           Serial.println((int)NRF_status);
           Serial.print("roundtrip ");
           Serial.println(millis()-NRF_roundtrip);
           byte NRF_retries=0;
           Nrf24_readRegister(8, &NRF_retries, sizeof(NRF_retries) );
           Serial.print("retries ");
           Serial.println((int)(NRF_retries & 0x0f));
        }
      #else
      if (event->Par1 == VALUE_SOURCE_RF)
        {
          if ((event->Par2 >= 1) && (event->Par2 <= 62))  // Select channel between 1 <> 62
            {
              Nrf24_channel = event->Par2;
              Nrf24_setChannel(Nrf24_channel);
              #ifndef NRF_SINGLE_MASTER_NODE
                NRF_CheckOnline();                      // Send data on air to find out who's online
              #endif
            }
        }
      #endif 
 
      success=true;
      break;
    } // case


  case PLUGIN_EVENT_OUT:
    {
    // Only send messages for port RF or ALL and only send Nodo messages (events)
    // This will process commands like "UserEventSend" and "VariableSend" 

#ifdef NODO_BETA_PLUGIN_SENDTO
    if(NRF_live && ((event->Port==VALUE_SOURCE_RF) || (event->Port==VALUE_ALL)))
#else
    if(NRF_live && ((event->Port==VALUE_SOURCE_RF) || (event->Port==VALUE_ALL)) && ((event->Type==NODO_TYPE_EVENT) || (event->Type==NODO_TYPE_PLUGIN_EVENT)))
#endif
      {
        // on the receiving end, this event may be passed through by NRFExtender on the I2C bus, so it needs a checksum
        Checksum(event);// bereken checksum: crc-8 uit alle bytes in de struct.
        memcpy((byte*)&NRFPayload+5, (byte*)event,sizeof(struct NodoEventStruct));

        byte first=1;
        byte last=NRF_UNIT_MAX;

        #if NODO_MEGA  // Sendto optimalisatie
          if ((event->Type == NODO_TYPE_COMMAND) && (event->Command == CMD_EVENTLIST_SHOW))
              NRF_single_target = event->DestinationUnit;
          if ((event->Flags != 0) && (event->DestinationUnit != 0))
              NRF_single_target = event->DestinationUnit;
        #endif

        // reset single target if flags are 0
        if (event->Flags ==0)
          {
            NRF_single_target = 0;
            #if NRF_DEBUG
              if (NRF_debug)
                {
                  Serial.print("So ");
                  Serial.println((int)NRF_single_target);
                }
            #endif
          }

        if (NRF_single_target > 0)
          {
            first=NRF_single_target;
            last=NRF_single_target;
          }

              #if NRF_DEBUG
                if (NRF_debug)
                  {
                    Serial.print(">T ");
                    Serial.print((int)event->Type);
                    Serial.print(" S ");
                    Serial.print((int)event->SourceUnit);
                    Serial.print(" D ");
                    Serial.print((int)event->DestinationUnit);
                    Serial.print(" C ");
                    Serial.print((int)event->Command);
                    Serial.print(" F ");
                    Serial.print((int)event->Flags);
                    Serial.print(" ST ");
                    Serial.println((int)NRF_single_target);
                  }
              #endif

        #ifndef NRF_SINGLE_MASTER_NODE
        for(int y=first;y<=last;y++)
          {
            if(NRFOnline[y]==true)
              {
                NRF_sendpacket(Settings.Unit, y, NRF_PAYLOAD_NODO, sizeof(struct NodoEventStruct),NRF_SEND_RETRIES);
              }
          }
        #else
          NRF_sendpacket(Settings.Unit, NRF_SINGLE_MASTER_NODE, NRF_PAYLOAD_NODO, sizeof(struct NodoEventStruct),NRF_SEND_RETRIES);
        #endif
      }
      success=true;
      break;
    }

#endif // PLUGIN_033_CORE
    #if NODO_MEGA
    case PLUGIN_MMI_IN:
      {
      char *TempStr=(char*)malloc(26);
      string[25]=0;

      if(GetArgv(string,TempStr,1))
        {
        if(strcasecmp(TempStr,PLUGIN_NAME_033)==0)
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
      strcpy(string,PLUGIN_NAME_033);
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

#ifdef PLUGIN_033_CORE


#ifndef NRF_SINGLE_MASTER_NODE
void NRF_CheckOnline()
{
  Nrf24_flushRx();
  #if NRF_DEBUG
    if (NRF_debug)
      {
        Serial.println("Online:");
      }
  #endif
  for(int y=1;y<=NRF_UNIT_MAX;y++)
    {
      if ((NRF_sendpacket(Settings.Unit, y, NRF_PAYLOAD_ONLINE, 0,0) & 0x20) == 0x20)
        {
          #if NRF_DEBUG
            if (NRF_debug)
              {
                Serial.print((int)y);
                Serial.println(" is Online");
              }
          #endif
          NRFOnline[y]=true;
          NodoOnline(y,VALUE_SOURCE_RF);
        }
    }
}
#endif

boolean NRF_init(void)
{
  SPI_begin();
  Nrf24_init();

  byte rf_setup=0x27;								// 250 kbps (bit 0 set for SI24R1 clone)
  Nrf24_writeRegister(NRF_RF_SETUP, &rf_setup, sizeof(rf_setup) );

  byte check=0;
  Nrf24_readRegister(NRF_RF_SETUP,&check,1);
  #if NRF_DEBUG
    if (NRF_debug)
      {
        Serial.println((int)check);
      }
  #endif
  if (check != 0x27)
    {
      #if NRF_DEBUG
        if (NRF_debug)
          {
            for(byte x=0; x < 10; x++)
              {
                digitalWrite(PIN_LED_RGB_R,HIGH);
                delay(50);
                digitalWrite(PIN_LED_RGB_R,LOW);
                delay(50);
              } 
          }
      #endif
      return false;
    }

  NRF_address[0]=Settings.Unit;
  Nrf24_setRADDR((byte *)NRF_address);
  Nrf24_config();

  byte en_aa=0x3F;								// Auto-Ack Enabled
  Nrf24_writeRegister(NRF_EN_AA, &en_aa, sizeof(en_aa) );
  byte setup_retr=0xff;								// delay 4000uS, 15 retries
  Nrf24_writeRegister(NRF_SETUP_RETR, &setup_retr, sizeof(setup_retr) );
  byte en_rxaddr=0x7;								// enable RX pipes 0,1,2
  Nrf24_writeRegister(NRF_EN_RXADDR, &en_rxaddr, sizeof(en_rxaddr) );
  Nrf24_flushRx();

  return true;
}

#ifdef NRF_FINDMASTER
byte NRF_findMaster()
{
  byte channel=2;
  byte found=0;
  Nrf24_flushRx();
  while(found < 5 && channel <63)
    {
      #if NRF_DEBUG
        if (NRF_debug)
          {
            Serial.print((int)channel);
          }
      #endif
      Nrf24_setChannel(channel);
      if ((NRF_sendpacket(Settings.Unit, 255, NRF_PAYLOAD_ONLINE, 0,0) & 0x20) == 0x20)
        found++;
      else
        {
          found=0;
          channel++;
        }
      #if NRF_DEBUG
        if (NRF_debug)
          {
            Serial.print("-");
            Serial.println((int)NRF_status);
          }
      #endif
   }
  return channel;
}
#endif

void NRF_changeChannel(byte channel)
{
  NRFPayload.Data[0]=channel;
  for (byte x=1; x <63; x++)
    {
      #if NRF_DEBUG
        if (NRF_debug)
          {
            Serial.println((int)x);
          }
      #endif
      Nrf24_setChannel(x);
      NRF_sendpacket(Settings.Unit, 255, NRF_PAYLOAD_CHANNEL, 1,0);
   }
}

byte NRF_sendpacket(byte Source, byte Destination, byte ID, byte Size, byte retries)
{
  if (!NRF_live) return 0;

  NRFPayload.Source=Source;
  NRFPayload.Destination=Destination;
  NRFPayload.ID=ID;
  NRFPayload.Sequence++;
  NRFPayload.Size=Size;
  
  NRF_address[0]=Destination;
  Nrf24_setTADDR((byte *)NRF_address);
  for (byte retry=0; retry <= retries; retry++)
    {
      #if NRF_DEBUG
        if (retry > 0)
          {
            Serial.print("R");
            Serial.println(retry);
          }
      #endif
      NRF_status = Nrf24_send((byte *)&NRFPayload);
      if ((NRF_status & 0x20) == 0x20)
        break;
      else
        delay(NRF_RETRY_DELAYMS + Settings.Unit);
    }

  #if NRF_DEBUG
    if (NRF_debug)
      {
        Serial.print("NRF Send:");
        Serial.print((int)Destination);
        Serial.print(" - ");
        Serial.println((int)NRF_status);
      }
  #endif

  // set auto-ack receive pipe to null
  NRF_address[0]=0;
  Nrf24_setTADDR((byte *)NRF_address);

  return NRF_status;
}


// ***********************************************************************************************************
// NRF24 specific code
// ***********************************************************************************************************

/*
    Copyright (c) 2007 Stefan Engelke <mbox@stefanengelke.de>
    Nathan Isburgh <nathan@mrroot.net>
    An Ardunio port of http://www.tinkerer.eu/AVRLib/nRF24L01
    Significant changes to remove depencence on interupts and auto ack support: Aaron Shrimpton <aaronds@gmail.com>

    Permission is hereby granted, free of charge, to any person 
    obtaining a copy of this software and associated documentation 
    files (the "Software"), to deal in the Software without 
    restriction, including without limitation the rights to use, copy, 
    modify, merge, publish, distribute, sublicense, and/or sell copies 
    of the Software, and to permit persons to whom the Software is 
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be 
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
    DEALINGS IN THE SOFTWARE.
*/

/* Memory Map */
#define CONFIG      0x00
#define EN_RXADDR   0x02
#define SETUP_AW    0x03
#define RF_CH       0x05
#define STATUS      0x07
#define OBSERVE_TX  0x08
#define CD          0x09
#define RX_ADDR_P0  0x0A
#define RX_ADDR_P1  0x0B
#define RX_ADDR_P2  0x0C
#define TX_ADDR     0x10
#define RX_PW_P0    0x11
#define RX_PW_P1    0x12
#define RX_PW_P2    0x13
#define FIFO_STATUS 0x17

/* Bit Mnemonics */
#define MASK_RX_DR  6
#define MASK_TX_DS  5
#define MASK_MAX_RT 4
#define EN_CRC      3
#define CRCO        2
#define PWR_UP      1
#define PRIM_RX     0
#define AW          0
#define ARD         4
#define ARC         0
#define PLL_LOCK    4
#define RF_DR       3
#define RF_PWR      1
#define LNA_HCURR   0        
#define RX_DR       6
#define TX_DS       5
#define MAX_RT      4
#define RX_P_NO     1
#define TX_FULL     0
#define PLOS_CNT    4
#define ARC_CNT     0
#define TX_REUSE    6
#define FIFO_FULL   5
#define TX_EMPTY    4
#define RX_FULL     1
#define RX_EMPTY    0

/* Instruction Mnemonics */
#define R_REGISTER    0x00
#define W_REGISTER    0x20
#define REGISTER_MASK 0x1F
#define R_RX_PAYLOAD  0x61
#define W_TX_PAYLOAD  0xA0
#define FLUSH_TX      0xE1
#define FLUSH_RX      0xE2
#define REUSE_TX_PL   0xE3
#define NOP           0xFF

void Nrf24_setChannel(byte channel)
{
  Nrf24_configRegister(RF_CH,channel*2);
  delayMicroseconds(256);
}

void Nrf24_transferSync(uint8_t *dataout,uint8_t *datain,uint8_t len)
{
	uint8_t i;
	for(i = 0;i < len;i++){
		datain[i] = SPI_transfer(dataout[i]);
	}
}

void Nrf24_transmitSync(uint8_t *dataout,uint8_t len)
{
	uint8_t i;
	for(i = 0;i < len;i++){
		SPI_transfer(dataout[i]);
	}
}


void Nrf24_init() 
{   
    pinMode(NRF_CSN_PIN,OUTPUT);
    Nrf24_csnHi();
}

void Nrf24_config() 
{
    Nrf24_setChannel(Nrf24_channel);
    Nrf24_configRegister(RX_PW_P0, NRF_PAYLOAD_SIZE);
    Nrf24_configRegister(RX_PW_P1, NRF_PAYLOAD_SIZE);
    Nrf24_configRegister(RX_PW_P2, NRF_PAYLOAD_SIZE);

    // Start receiver 
    Nrf24_powerUpRx();
    Nrf24_flushRx();
}

void Nrf24_setRADDR(uint8_t * adr) 
{
	Nrf24_writeRegister(RX_ADDR_P1,adr,NRF_ADDR_LEN);
        byte broadcast=255;
	Nrf24_writeRegister(RX_ADDR_P2,&broadcast,1);
}

void Nrf24_setTADDR(uint8_t * adr)
{
	Nrf24_writeRegister(RX_ADDR_P0,adr,NRF_ADDR_LEN);
	Nrf24_writeRegister(TX_ADDR,adr,NRF_ADDR_LEN);
}

bool Nrf24_dataReady() 
{
    return !Nrf24_rxFifoEmpty();
}

bool Nrf24_rxFifoEmpty(){
	uint8_t fifoStatus;

	Nrf24_readRegister(FIFO_STATUS,&fifoStatus,sizeof(fifoStatus));
	return (fifoStatus & (1 << RX_EMPTY));
}



void Nrf24_getData(uint8_t * data) 
{
    Nrf24_csnLow();                               // Pull down chip select
    SPI_transfer( R_RX_PAYLOAD );            // Send cmd to read rx payload
    Nrf24_transferSync(data,data,NRF_PAYLOAD_SIZE); // Read payload
    Nrf24_csnHi();                               // Pull up chip select
    Nrf24_configRegister(STATUS,(1<<RX_DR));   // Reset status register
}

void Nrf24_configRegister(uint8_t reg, uint8_t value)
{
    Nrf24_csnLow();
    SPI_transfer(W_REGISTER | (REGISTER_MASK & reg));
    SPI_transfer(value);
    Nrf24_csnHi();
}

void Nrf24_readRegister(uint8_t reg, uint8_t * value, uint8_t len)
{
    Nrf24_csnLow();
    SPI_transfer(R_REGISTER | (REGISTER_MASK & reg));
    Nrf24_transferSync(value,value,len);
    Nrf24_csnHi();
}

void Nrf24_writeRegister(uint8_t reg, uint8_t * value, uint8_t len) 
{
    Nrf24_csnLow();
    SPI_transfer(W_REGISTER | (REGISTER_MASK & reg));
    Nrf24_transmitSync(value,len);
    Nrf24_csnHi();
}

byte Nrf24_send(uint8_t * value) 
{
    Nrf24_configRegister(CONFIG, NRF_CONFIG_DATA); // power down trick to workaround CE pin tied high!
    Nrf24_powerUpTx();       // Set to transmitter mode , Power up
       
    Nrf24_csnLow();                    // Pull down chip select
    SPI_transfer( W_TX_PAYLOAD ); // Write cmd to write payload
    Nrf24_transmitSync(value,NRF_PAYLOAD_SIZE);   // Write payload
    Nrf24_csnHi();                    // Pull up chip select

    unsigned long timer=millis()+NRF_SEND_TIMEOUTMS;
    uint8_t status = Nrf24_getStatus();
    while(millis()<timer)
      {
        status = Nrf24_getStatus();
        if((status & ((1 << TX_DS)  | (1 << MAX_RT))))
          {
            break;
          }
      }

  Nrf24_flushTx();
  Nrf24_powerUpRx();
  return status;
}

uint8_t Nrf24_getStatus()
{
	uint8_t rv;
	Nrf24_readRegister(STATUS,&rv,1);
	return rv;
}

void Nrf24_powerUpRx()
{
	Nrf24_configRegister(CONFIG, NRF_CONFIG_DATA | ( (1<<PWR_UP) | (1<<PRIM_RX) ) );
	Nrf24_configRegister(STATUS,(1 << TX_DS) | (1 << MAX_RT)); 
}

void Nrf24_flushRx()
{
    Nrf24_csnLow();
    SPI_transfer( FLUSH_RX );
    Nrf24_csnHi();
}

void Nrf24_flushTx()
{
    Nrf24_csnLow();
    SPI_transfer( FLUSH_TX );
    Nrf24_csnHi();
}

void Nrf24_powerUpTx()
{
	Nrf24_configRegister(CONFIG, NRF_CONFIG_DATA| ( (1<<PWR_UP) | (0<<PRIM_RX) ) );
}

void Nrf24_csnHi()
{
	digitalWrite(NRF_CSN_PIN,HIGH);
	#ifdef NRF_HWSPI
	  #if NODO_MEGA
	    digitalWrite(EthernetShield_CS_W5100, 0);
	    digitalWrite(EthernetShield_CS_SDCard, 1);
	  #endif
	#endif
}

void Nrf24_csnLow()
{
	#ifdef NRF_HWSPI
	  #if NODO_MEGA
	    digitalWrite(EthernetShield_CS_W5100, 1);
	    digitalWrite(EthernetShield_CS_SDCard, 1);
	  #endif
	#endif
	digitalWrite(NRF_CSN_PIN,LOW);
}

void Nrf24_powerDown()
{
	Nrf24_configRegister(CONFIG, NRF_CONFIG_DATA);
}

#ifdef NRF_HWSPI
void SPI_begin()
{
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  pinMode(NRF_CE_PIN,OUTPUT);
  digitalWrite(NRF_CE_PIN,HIGH);
  Serial.println("HW SPI");
}

unsigned char SPI_transfer(unsigned char Byte)
{
  return SPI.transfer(Byte);
}

#else
// Software SPI routines

void SPI_begin()
{
  pinMode(NRF_SCK_PIN,OUTPUT);
  pinMode(NRF_MOSI_PIN,OUTPUT);
  pinMode(NRF_MISO_PIN,INPUT);
  digitalWrite(NRF_SCK_PIN,LOW);
  digitalWrite(NRF_MOSI_PIN,HIGH);
}

unsigned char SPI_transfer(unsigned char Byte)
{
  uint8_t mosibit = digitalPinToBitMask(NRF_MOSI_PIN);
  uint8_t mosiport = digitalPinToPort(NRF_MOSI_PIN);
  uint8_t sckbit = digitalPinToBitMask(NRF_SCK_PIN);
  uint8_t sckport = digitalPinToPort(NRF_SCK_PIN);
  uint8_t misobit = digitalPinToBitMask(NRF_MISO_PIN);
  uint8_t misoport = digitalPinToPort(NRF_MISO_PIN);
  volatile uint8_t *mosiout = portOutputRegister(mosiport);
  volatile uint8_t *sckout = portOutputRegister(sckport);
  volatile uint8_t *misoin = portInputRegister(misoport);
  uint8_t oldSREG = SREG;

  cli();

  for(unsigned char i=0;i<8;i++)
  {
    if(Byte & 0x80)
	*mosiout |= mosibit;
    else
	*mosiout &= ~mosibit;

    *sckout |= sckbit;

    Byte <<= 1;
    if (*misoin & misobit)
      Byte |= 1;

    *sckout &= ~sckbit;
  }
  SREG = oldSREG;
  return(Byte);
}
#endif // hw/sw spi

#endif
