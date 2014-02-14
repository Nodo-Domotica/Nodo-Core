void Save_Settings(void)  
{
  char ByteToSave,*pointerToByteToSave=pointerToByteToSave=(char*)&Settings;    //pointer to settings struct 

  for(int x=0; x<sizeof(struct SettingsStruct) ;x++)
  {
    EEPROM.write(x,*pointerToByteToSave); 
    pointerToByteToSave++;
  }  
}

/*********************************************************************************************\
 * Load settings from EEPROM
 \*********************************************************************************************/
boolean LoadSettings()
 {
  byte x;

  char ByteToSave,*pointerToByteToRead=(char*)&Settings;    //pointer to settings struct

  for(int x=0; x<sizeof(struct SettingsStruct);x++)
    {
    *pointerToByteToRead=EEPROM.read(x);
    pointerToByteToRead++;// next byte
    }
  }
  
/*********************************************************************************************\
 * Reset Extender settings to default values.
\*********************************************************************************************/

void ResetFactory(void)
  {
    Serial.println("Reset!");

    Settings.Version    = EXTENDER_VERSION;
    Settings.Unit       = THIS_EXTENDER_UNIT;
    Settings.Address    = NRF_RECEIVE_ADDRESS;
    Settings.Channel    = NRF_CHANNEL;
    Settings.Peer       = 0;

    Save_Settings();
    Reboot();
  }

uint8_t *heapptr, *stackptr;
unsigned long FreeMem(void)
  {
  stackptr = (uint8_t *)malloc(4);        // use stackptr temporarily
  heapptr = stackptr;                     // save value of heap pointer
  free(stackptr);                         // free up the memory again (sets stackptr to 0)
  stackptr =  (uint8_t *)(SP);            // save value of stack pointer
  return (stackptr-heapptr);
  }
  
