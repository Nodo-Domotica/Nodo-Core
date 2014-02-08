/**********************************************************************************************\
 * I2C bus receive handler (IRQ) 
 \*********************************************************************************************/

void ReceiveI2C(int n)
{
  I2C_Received=0;
  byte b;

  while(WireNodo.available()) // Store received bytes into I2C buffer
  {
    b=WireNodo.read();
    I2C_ReceiveBuffer[I2C_Received++]=b; 
  }
}

/**********************************************************************************************\
 * Send event on I2C bus. 
 \*********************************************************************************************/
void SendI2C()
{
  for(int y=1;y<=I2C_UNIT_MAX;y++)
  {            
    if(I2COnline[y]==true)
    {
      Serial.print("Send to I2C address:");
      Serial.println((int)y);
      WireNodo.beginTransmission(I2C_START_ADDRESS+y-1);
      for(byte x=0;x<NRFPayload.Size;x++)
        WireNodo.write(NRFPayload.Data[x]);
      WireNodo.endTransmission(false); // transmit data, do not release I2C bus
    }
  }
  WireNodo.endTransmission(true); // Release I2C bus
}

/*******************************************************************************************************\
 * Discover I2C peers
 \*******************************************************************************************************/
byte CheckI2COnline()
{
  byte I2Cstatus, I2Caddress;
  
  for(I2Caddress = 1; I2Caddress < I2C_UNIT_MAX; I2Caddress++ ) 
  {
    WireNodo.beginTransmission(I2Caddress);
    I2Cstatus = WireNodo.endTransmission();

    if (I2Cstatus == 0)
    {
      I2COnline[I2Caddress]=true;
    }
  }
}

void PrintI2COnline(void)
{
  Serial.println("I2C Peers Online:");
  for(int y=1;y<=I2C_UNIT_MAX;y++)
  {            
    if(I2COnline[y]==true)
    {
      Serial.print((int)y);
      Serial.println(" is Online");
    }
  }
}

