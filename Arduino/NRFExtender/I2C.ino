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
void SendI2C(struct NodoEventStruct *EventBlock)
{  
  byte x;
  byte *B=(byte*)EventBlock;  

  Checksum(EventBlock);// calculate checksum: crc-8

  for(int y=1;y<=UNIT_MAX;y++)
  {            
    if((EventBlock->Command==EVENT_BOOT && EventBlock->Type == NODO_TYPE_EVENT) || (NodoOnline(y,0)==VALUE_SOURCE_I2C))
    {
      Serial.print("Send to I2C address:");
      Serial.println((int)y);
      WireNodo.beginTransmission(I2C_START_ADDRESS+y-1);
      for(x=0;x<sizeof(struct NodoEventStruct);x++)
        WireNodo.write(*(B+x));
      WireNodo.endTransmission(false); // verzend de data, sluit af maar geef de bus NIET vrij
    }
  }
  WireNodo.endTransmission(true); // Geef de bus vrij
}

/*********************************************************************************************\
 * I2C Checksum
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

/*******************************************************************************************************\
 * Keep track of all Online Nodo units
 \*******************************************************************************************************/
byte NodoOnline(byte Unit, byte Port)
{
  static byte NodoOnlinePort[UNIT_MAX+1];
  static boolean FirstTime=true;

  int x;

  // On first call, clear table
  if(FirstTime)
  {
    FirstTime=false;
    for(x=0;x<=UNIT_MAX;x++)
      NodoOnlinePort[x]=0;
  }

  if(Port && Port!=NodoOnlinePort[Unit])
  {
    // Update table.
    if(Port==VALUE_SOURCE_I2C)
      NodoOnlinePort[Unit]=VALUE_SOURCE_I2C;
  }    
  return NodoOnlinePort[Unit];
}

void PrintNodoOnline(void)
{
  Serial.println("Nodo Online:");
  for(int y=1;y<=UNIT_MAX;y++)
  {            
    if((NodoOnline(y,0)==VALUE_SOURCE_I2C))
    {
      Serial.print((int)y);
      Serial.println(" is Online");
    }
  }
}


