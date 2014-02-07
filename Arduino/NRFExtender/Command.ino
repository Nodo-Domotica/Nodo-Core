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
        PrintNodoOnline();
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
}

