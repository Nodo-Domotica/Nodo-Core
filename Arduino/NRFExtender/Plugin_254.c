//#######################################################################################################
//#################################### Plugin-254: to get NRF Extender working ##########################
//#######################################################################################################

#define PLUGIN_ID   254

boolean Plugin_254(byte function, struct NodoEventStruct *event, char *string)
  {
  switch(function)
    {    
    #ifdef PLUGIN_254_CORE
    case PLUGIN_INIT:
      {
        for(byte I2Caddress = 1; I2Caddress <= UNIT_MAX; I2Caddress++ ) 
         {
           WireNodo.beginTransmission(I2Caddress);
           if (WireNodo.endTransmission() == 0)
               NodoOnline(I2Caddress,VALUE_SOURCE_I2C);
         }
      }
    #endif // CORE
    }
    return true;
  }
