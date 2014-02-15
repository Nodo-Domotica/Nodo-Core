//#######################################################################################################
//#################################### Plugin-254: NRF Extender #########################################
//#######################################################################################################

#define PLUGIN_ID   254

#define PLUGIN_254_NRF_EXTENDER_I2C_ADDRESS 127

boolean PLUGIN_254_NRF_Extender_Present=false;

boolean Plugin_254(byte function, struct NodoEventStruct *event, char *string)
  {
  switch(function)
    {    
    #ifdef PLUGIN_254_CORE
    case PLUGIN_INIT:
      {
         // Check if the NRF Extender is present on the local I2C bus...
         WireNodo.beginTransmission(PLUGIN_254_NRF_EXTENDER_I2C_ADDRESS);
         if (WireNodo.endTransmission() == 0)
           PLUGIN_254_NRF_Extender_Present=true;
      }

    case PLUGIN_EVENT_OUT:
      {
        if(PLUGIN_254_NRF_Extender_Present)
          {
            // Send data packet to NRF Extender
            byte x;
            byte *B=(byte*)event;
            Checksum(event);
            WireNodo.beginTransmission(PLUGIN_254_NRF_EXTENDER_I2C_ADDRESS);
            for(x=0;x<sizeof(struct NodoEventStruct);x++)
              WireNodo.write(*(B+x));
            WireNodo.endTransmission(true);

            // Wait for acknowlegde, NRF Extender will send an "Ack" packet when RF send to all radio's has finished
            Serial.println("NRF wait for ack");
            unsigned long acktimer=millis();
            while (I2C_Received == 0 && millis() - acktimer < 1000) {}
            Serial.println("NRF wait ready");
            I2C_Received=0;
          }
      }
    #endif // CORE
    }
    return false;
  }
