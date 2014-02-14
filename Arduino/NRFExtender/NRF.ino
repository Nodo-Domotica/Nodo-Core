#define NRF_CONFIG      0x00  // Register for config
#define NRF_EN_AA       0x01  // Register for Enable Auto Acknowledge configuration register
#define NRF_SETUP_RETR  0x04  // Register for Retry delay and count
#define NRF_RF_CH       0x05  // Register to set channel
#define NRF_RF_SETUP    0x06  // Register for Set Data Rate
#define NRF_STATUS      0x07  // Register for status
#define NRF_CD          0x09  // Register for Carried Detect
#define NRF_RX_ADDR_P1  0x0B
#define NRF_RX_ADDR_P2  0x0C
#define NRF_EN_RXADDR   0x02

#define mirf_ADDR_LEN	5
#define mirf_CONFIG ((1<<EN_CRC) | (0<<CRCO) )

// NRF Variables
uint8_t Nrf24_PTX;
uint8_t Nrf24_channel=1;

void NRF_init(void)
{
  pinMode(NRF_CSN_PIN,OUTPUT);
  pinMode(NRF_CE_PIN,OUTPUT);
  pinMode(10,OUTPUT); // Needed for SPI !!
  digitalWrite(NRF_CSN_PIN,HIGH);
  digitalWrite(NRF_CE_PIN,HIGH);
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_2XCLOCK_MASK);
  Nrf24_init();
  NRF_address[0]=Settings.Address;
  Nrf24_setRADDR((byte *)NRF_address);
  Nrf24_channel = Settings.Channel;
  Nrf24_config();

  byte rf_setup=0x26; // 250 kbps
  Nrf24_writeRegister(NRF_RF_SETUP, &rf_setup, sizeof(rf_setup) );
  byte en_aa=0x3f; // Auto-Ack Enabled
  Nrf24_writeRegister(NRF_EN_AA, &en_aa, sizeof(en_aa) );
  byte setup_retr=0xff;  // delay 4000uS, 15 retries
  Nrf24_writeRegister(NRF_SETUP_RETR, &setup_retr, sizeof(setup_retr) );
  byte en_rxaddr=0x7;  // enable RX pipes 0,1,2
  Nrf24_writeRegister(NRF_EN_RXADDR, &en_rxaddr, sizeof(en_rxaddr) );
  Nrf24_flushRx();
}

boolean NRF_receive(void)
{
  if(!Nrf24_isSending() && Nrf24_dataReady())
    {
      Nrf24_getData((byte *)&NRFPayload);

      if(command_mode)
        {
          Serial.print("NRF RX ");
          Serial.print("S:");
          Serial.print((int)NRFPayload.Source);
          Serial.print(", D:");
          Serial.print((int)NRFPayload.Destination);
          Serial.print(", ID:");
          Serial.print((int)NRFPayload.ID);
          Serial.print(", SZ:");
          Serial.println((int)NRFPayload.Size);
        
          for (byte x=0; x < NRFPayload.Size; x++)
            {
              Serial.print(NRFPayload.Data[x]);
              Serial.print(' ');
            }
          Serial.println("");
        }
      
      switch(NRFPayload.ID)
        {
          case NRF_PAYLOAD_NODO:
            {
              return true;
              break;
            }  
          case NRF_PAYLOAD_ONLINE:
            {
              if(command_mode)
                {
                  Serial.print("NRF RX anouncement from:");
                  Serial.println((int)NRFPayload.Source);
                }
              NRFOnline[NRFPayload.Source]=true;
              break;
            }          
          case NRF_PAYLOAD_PINGREP:
            {
              if(command_mode)
                {
                  Serial.print("Ping Reply:");
                  Serial.println((int)NRFPayload.Source);
                }
              break;
            }
          case NRF_PAYLOAD_PINGREQ:
            {
              NRF_sendpacket(Settings.Address, NRFPayload.Source, NRF_PAYLOAD_PINGREP, 0);
              break;
            }
          case NRF_PAYLOAD_TEXT:
            {
              for (byte x=0; x < NRFPayload.Size; x++)
                Serial.write(NRFPayload.Data[x]);
              break;
            }          
        }
    }
  return false;
}

void NRF_send()
{
  byte len=I2C_Received;
  memcpy((byte*)&NRFPayload+4, (byte*)&I2C_ReceiveBuffer,len);
  byte first=1;
  byte last=NRF_UNIT_MAX;
  if (Settings.Peer != 0)
    {
      first=Settings.Peer;
      last=Settings.Peer;
    }

  for(int y=first;y<=last;y++)
    {
      if(NRFOnline[y]==true)
        {
          if(command_mode)
            {
              Serial.print("Send to NRF address: ");
              Serial.println((int)y);
            }
          NRF_sendpacket(Settings.Address, y, NRF_PAYLOAD_NODO, len);
        }
    }
}

void NRF_send_text(char * data, byte len)
{
  data[len]=0x0d;
  data[len+1]=0x0a;
  len+=2;

  byte first=1;
  byte last=NRF_UNIT_MAX;
  if (Settings.Peer != 0)
    {
      first=Settings.Peer;
      last=Settings.Peer;
    }

  for(int y=first;y<=last;y++)
    {
      if(NRFOnline[y]==true)
        {
          byte pos=0;
          int sendlen=0;
          while(pos < len)
          {
            sendlen=len-pos;
            if (sendlen > NRF_PAYLOAD_SIZE-4) sendlen=NRF_PAYLOAD_SIZE-4;
            memcpy((byte*)&NRFPayload+4, (byte*)data+pos,sendlen);
            NRF_sendpacket(Settings.Address, y, NRF_PAYLOAD_TEXT, sendlen);
            pos+=NRF_PAYLOAD_SIZE-4;
          }
        }
    }
}

void NRF_CheckOnline()
{
  if(command_mode)
    Serial.println("NRF Peers Online:");

  byte first=1;
  byte last=NRF_UNIT_MAX;
  if (Settings.Peer != 0)
    {
      first=Settings.Peer;
      last=Settings.Peer;
    }
    
  for(int y=first;y<=last;y++)
    {
      if (NRF_sendpacket(Settings.Address, y, NRF_PAYLOAD_ONLINE, 0) == 46)
        {
          if(command_mode)
            {
              Serial.print((int)y);
              Serial.println(" is Online");
            }
          NRFOnline[y]=true;
        }
    }
}

byte NRF_sendpacket(byte Source, byte Destination, byte ID, byte Size)
{
  NRFPayload.Source=Source;
  NRFPayload.Destination=Destination;
  NRFPayload.ID=ID;
  NRFPayload.Size=Size;
  
  NRF_address[0]=Destination;
  NRF_status=0;
  Nrf24_setTADDR((byte *)NRF_address);
  Nrf24_send((byte *)&NRFPayload);
  while(Nrf24_isSending()) {}
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

void Nrf24_transferSync(uint8_t *dataout,uint8_t *datain,uint8_t len)
{
	uint8_t i;
	for(i = 0;i < len;i++){
		datain[i] = SPI.transfer(dataout[i]);
	}
}

void Nrf24_transmitSync(uint8_t *dataout,uint8_t len)
{
	uint8_t i;
	for(i = 0;i < len;i++){
		SPI.transfer(dataout[i]);
	}
}


void Nrf24_init() 
// Initializes pins to communicate with the MiRF module, should be called in the early initializing phase at startup.
{   
    pinMode(NRF_CE_PIN,OUTPUT);
    pinMode(NRF_CSN_PIN,OUTPUT);

    Nrf24_ceLow();
    Nrf24_csnHi();

}

void Nrf24_config() 
// Sets the important registers in the MiRF module and powers the module in receiving mode. NB: channel and payload must be set now.
{
    // Set RF channel
	Nrf24_configRegister(RF_CH,Nrf24_channel);

    // Set length of incoming payload 
	Nrf24_configRegister(RX_PW_P0, NRF_PAYLOAD_SIZE);
	Nrf24_configRegister(RX_PW_P1, NRF_PAYLOAD_SIZE);
	Nrf24_configRegister(RX_PW_P2, NRF_PAYLOAD_SIZE);

    // Start receiver 
    Nrf24_powerUpRx();
    Nrf24_flushRx();
}

void Nrf24_setRADDR(uint8_t * adr) 
// Sets the receiving address
{
	Nrf24_ceLow();
	Nrf24_writeRegister(RX_ADDR_P1,adr,mirf_ADDR_LEN);
        byte broadcast=255;
	Nrf24_writeRegister(RX_ADDR_P2,&broadcast,1);
	Nrf24_ceHi();
}

void Nrf24_setTADDR(uint8_t * adr)
// Sets the transmitting address
{
	Nrf24_writeRegister(RX_ADDR_P0,adr,mirf_ADDR_LEN);
	Nrf24_writeRegister(TX_ADDR,adr,mirf_ADDR_LEN);
}

bool Nrf24_dataReady() 
// Checks if data is available for reading
{
	uint8_t status = Nrf24_getStatus();

    if ( status & (1 << RX_DR) ) return 1;
    return !Nrf24_rxFifoEmpty();
}

bool Nrf24_rxFifoEmpty(){
	uint8_t fifoStatus;

	Nrf24_readRegister(FIFO_STATUS,&fifoStatus,sizeof(fifoStatus));
	return (fifoStatus & (1 << RX_EMPTY));
}



void Nrf24_getData(uint8_t * data) 
// Reads payload bytes into data array
{
    Nrf24_csnLow();                               // Pull down chip select
    SPI.transfer( R_RX_PAYLOAD );            // Send cmd to read rx payload
    Nrf24_transferSync(data,data,NRF_PAYLOAD_SIZE); // Read payload
    Nrf24_csnHi();                               // Pull up chip select
    Nrf24_configRegister(STATUS,(1<<RX_DR));   // Reset status register
}

void Nrf24_configRegister(uint8_t reg, uint8_t value)
// Clocks only one byte into the given MiRF register
{
    Nrf24_csnLow();
    SPI.transfer(W_REGISTER | (REGISTER_MASK & reg));
    SPI.transfer(value);
    Nrf24_csnHi();
}

void Nrf24_readRegister(uint8_t reg, uint8_t * value, uint8_t len)
// Reads an array of bytes from the given start position in the MiRF registers.
{
    Nrf24_csnLow();
    SPI.transfer(R_REGISTER | (REGISTER_MASK & reg));
    Nrf24_transferSync(value,value,len);
    Nrf24_csnHi();
}

void Nrf24_writeRegister(uint8_t reg, uint8_t * value, uint8_t len) 
// Writes an array of bytes into inte the MiRF registers.
{
    Nrf24_csnLow();
    SPI.transfer(W_REGISTER | (REGISTER_MASK & reg));
    Nrf24_transmitSync(value,len);
    Nrf24_csnHi();
}


void Nrf24_send(uint8_t * value) 
// Sends a data package to the default address. Be sure to send the correct amount of bytes as configured as payload on the receiver.
{
    uint8_t status;
    status = Nrf24_getStatus();

    while (Nrf24_PTX) {
	    status = Nrf24_getStatus();

	    if((status & ((1 << TX_DS)  | (1 << MAX_RT)))){
		    Nrf24_PTX = 0;
		    break;
	    }
    }                  // Wait until last paket is send

    Nrf24_ceLow();
    Nrf24_powerUpTx();       // Set to transmitter mode , Power up
    
    Nrf24_csnLow();                    // Pull down chip select
    SPI.transfer( FLUSH_TX );     // Write cmd to flush tx fifo
    Nrf24_csnHi();                    // Pull up chip select
    
    Nrf24_csnLow();                    // Pull down chip select
    SPI.transfer( W_TX_PAYLOAD ); // Write cmd to write payload
    Nrf24_transmitSync(value,NRF_PAYLOAD_SIZE);   // Write payload
    Nrf24_csnHi();                    // Pull up chip select

    Nrf24_ceHi();                     // Start transmission
}


bool Nrf24_isSending()
{
	uint8_t status;
	if(Nrf24_PTX){
		status = Nrf24_getStatus();

		// if sending successful (TX_DS) or max retries exceded (MAX_RT).

		if((status & ((1 << TX_DS)  | (1 << MAX_RT)))){
			Nrf24_powerUpRx();
                        NRF_status = status;
			return false; 
		}

		return true;
	}
	return false;
}

uint8_t Nrf24_getStatus()
{
	uint8_t rv;
	Nrf24_readRegister(STATUS,&rv,1);
	return rv;
}

void Nrf24_powerUpRx()
{
	Nrf24_PTX = 0;
	Nrf24_ceLow();
	Nrf24_configRegister(CONFIG, mirf_CONFIG | ( (1<<PWR_UP) | (1<<PRIM_RX) ) );
	Nrf24_ceHi();
	Nrf24_configRegister(STATUS,(1 << TX_DS) | (1 << MAX_RT)); 
}

void Nrf24_flushRx()
{
    Nrf24_csnLow();
    SPI.transfer( FLUSH_RX );
    Nrf24_csnHi();
}

void Nrf24_powerUpTx()
{
	Nrf24_PTX = 1;
	Nrf24_configRegister(CONFIG, mirf_CONFIG | ( (1<<PWR_UP) | (0<<PRIM_RX) ) );
}

void Nrf24_ceHi()
{
	digitalWrite(NRF_CE_PIN,HIGH);
}

void Nrf24_ceLow()
{
	digitalWrite(NRF_CE_PIN,LOW);
}

void Nrf24_csnHi()
{
	digitalWrite(NRF_CSN_PIN,HIGH);
}

void Nrf24_csnLow()
{
	digitalWrite(NRF_CSN_PIN,LOW);
}

void Nrf24_powerDown()
{
	Nrf24_ceLow();
	Nrf24_configRegister(CONFIG, mirf_CONFIG );
}

