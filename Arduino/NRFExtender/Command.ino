void ExecuteCommand(char *Command, byte Par1, byte Par2)
{
  if(strcasecmp(Command,"Unit")==0)
      {
        Settings.Unit=Par1;
      }

  if(strcasecmp(Command,"Address")==0)
      {
        Settings.Address=Par1;
      }

  if(strcasecmp(Command,"Channel")==0)
      {
        Settings.Channel=Par1;
      }

  if(strcasecmp(Command,"Reboot")==0)
      {
        Reboot();
      }
      
  if(strcasecmp(Command,"Online")==0)
      {
        NRF_CheckOnline();
        Serial.println();
        CheckI2COnline();
        PrintI2COnline();
      }
      
  if(strcasecmp(Command,"Scan")==0)
      {
        scanner();
        NRF_init();
        NRF_CheckOnline();
      }
      
  if(strcasecmp(Command,"Save")==0)
      {
        Save_Settings();
      }
      
  if(strcasecmp(Command,"Settings")==0)
      {
        Serial.print("Unit:");
        Serial.println((int)Settings.Unit);
        Serial.print("Address:");
        Serial.println((int)Settings.Address);
        Serial.print("Channel:");
        Serial.println((int)Settings.Channel);
      }

   if(strcasecmp(Command,"Ping")==0)
      {
        Serial.print("Ping ");
        Serial.println((int)Par1);
        unsigned long NRF_roundtrip=millis();
        NRF_sendpacket(Settings.Address, Par1, NRF_PAYLOAD_PINGREQ, 0);
        Serial.print("status ");
        Serial.println((int)NRF_status);
        Serial.print("roundtrip ");
        Serial.println(millis()-NRF_roundtrip);
        byte NRF_retries=0;
        Nrf24_readRegister(8, &NRF_retries, sizeof(NRF_retries) );
        Serial.print("retries ");
        Serial.println((int)(NRF_retries & 0x0f));
      }

   if(strcasecmp(Command,"Test")==0)
      {
        NRF_sendpacket(Settings.Address, Par1, Par2, 0);
      }
      
   if(strcasecmp(Command,"Freemem")==0)
      {
        Serial.println(FreeMem());
      }
}

