// Array to hold Channel data
#define CHANNELS  125
byte channel[CHANNELS];

// get the value of a NRF24 register
byte getRegister(byte r)
{
  byte c;
  Nrf24_csnLow();
  c = SPI.transfer(r&0x1F);
  c = SPI.transfer(0);  
  Nrf24_csnHi();
  return(c);
}

// set the value of a NRF24L register
void setRegister(byte r, byte v)
{
  Nrf24_csnLow();
  SPI.transfer((r&0x1F)|0x20);
  SPI.transfer(v);
  Nrf24_csnHi();
}

// scanning all channels in the 2.4GHz band
void scanChannels(void)
{
  Nrf24_ceLow();
  for( int j=0 ; j<200  ; j++)
  {
    for( int i=1 ; i <= CHANNELS ; i++)
    {
      // select a new channel
      setRegister(NRF_RF_CH,i);
      
      // switch on RX
      setRegister(NRF_CONFIG,getRegister(NRF_CONFIG)|0x01);
      Nrf24_ceHi();
      // wait enough for RX-things to settle
      delayMicroseconds(128);      
      
      // this is actually the point where the RPD-flag is set, when CE goes low
      Nrf24_ceLow();
      
      // read out RPD flag; set to 1 if received power > -64dBm
      if( getRegister(NRF_CD)>0 )
        channel[i]++;
    }
  }
}

void scanner()
{
  Serial.println("Scanning...");
  for( int i=1 ; i <= CHANNELS ; i++) channel[i]=0;
  
  Nrf24_ceLow();
  
  // now start receiver
  setRegister(NRF_CONFIG,getRegister(NRF_CONFIG)|0x02);
  delayMicroseconds(130);
  
  // switch off Shockburst
  setRegister(NRF_EN_AA,0x0);
  
  // make sure RF-section is set properly 
  setRegister(NRF_RF_SETUP,0x0F); 
  
  scanChannels();
  
  for( int i=1 ; i <= CHANNELS ; i++)
    {
      if (channel[i] >0)
        {
          Serial.print((int)i);
          Serial.print('-');
          Serial.println((int)channel[i]);
        }
    }
}

