void Save_Settings(void)  
{
  char ByteToSave,*pointerToByteToSave=pointerToByteToSave=(char*)&Settings;    //pointer verwijst nu naar startadres van de struct. 

  for(int x=0; x<sizeof(struct SettingsStruct) ;x++)
  {
    EEPROM.write(x,*pointerToByteToSave); 
    pointerToByteToSave++;
  }  
}

/*********************************************************************************************\
 * Laad de settings uit het EEPROM geheugen.
 \*********************************************************************************************/
boolean LoadSettings()
 {
  byte x;

  char ByteToSave,*pointerToByteToRead=(char*)&Settings;    //pointer verwijst nu naar startadres van de struct.

  for(int x=0; x<sizeof(struct SettingsStruct);x++)
    {
    *pointerToByteToRead=EEPROM.read(x);
    pointerToByteToRead++;// volgende byte uit de struct
    }
  }
  
/*********************************************************************************************\
 * Alle settings van de Nodo weer op default.
\*********************************************************************************************/

void ResetFactory(void)
  {
    Serial.println("Reset!");
    // Herstel alle settings naar defaults
    Settings.Version    = NODO_VERSION_MINOR;
    Settings.Unit       = THIS_EXTENDER_UNIT;
    Settings.Address    = NRF_RECEIVE_ADDRESS;
    Settings.Channel    = NRF_CHANNEL;
    
    Save_Settings();
    Reboot();
  }

