/*
  {
      while(TimeoutTimer>millis())
        {
        PulseTime = WaitForChangeState(PIN_RF_RX_DATA,HIGH, SIGNAL_TIMEOUT_RF);
        if(PulseTime>MIN_PULSE_LENGTH)
          {        
          if(PulseTime>NODO_PULSE_MID)
            ReceivedByte |= (1<<x);

          if(++x>7)
            {
            ReturnString[ReceivedByteCounter++]=ReceivedByte;
            ReceivedByte=0;
            x=0;
            if(ReceivedByteCounter==Length)
              {
              ReturnString[ReceivedByteCounter]=0; // sluit string af.
              TimeoutTimer=0L;// stop de while() loop.
                          
              // Decrypt het signaal met de MD5-Hash
              sprintf(TmpStr,"%d:%s",Cookie,S.Password);
              md5(TmpStr); 
              y=0;
              for(x=0; x<Length; x++)
                {
                ReturnString[x]=ReturnString[x] ^ MD5HashCode[y++];
                if(y>15)y=0;
                }
          
              // bereken de checksum van de ontvangen string.
              y=0;
              for(x=0;x<strlen(ReturnString);x++)
                y=(y + ReturnString[x])%256; // bereken checksum (crc-8)

              if(y==Checksum)
                Ok=true;
              }
            }
          }
        }
      }
sendto 1;sound;sound;sound;sound;sound;sound;sound;sound;sound;
#define NODO_MAC 0x54, 0xa5, 0x8d, 0x17, 0xaf, 0x41 // Productie MAC Paul ???

Serial.print("*** debug: =");Serial.println(); //??? Debug
Serial.println("*** debug: ");//???

Verzenden blok data:

Methode=1: Par1 en Par2 delen in een regulier event gebruiken voor overdracht
=============================================================================
* in delen van 16-bit Par1 en Pr2 bit van een 32-bit reguliere eventcode.
* opvangen in de queue.
* CMD_BLOCK niet weergeven voor gebruiker.
+ eenvoudig implementeerbaar. Code al aanwezig
- veel overhead in transport
? Handshaking

0: CMD_BLOCK,Par1=toepassing,0
1: data-1
2: data-2
3: data-3
n: data-n 
x: CMD_BLOCK, sluiting, checksum.

Methode=2: Oude Sendto methode (afwijkens signaal)
=============================================================================














*/



//###################################################################################################


