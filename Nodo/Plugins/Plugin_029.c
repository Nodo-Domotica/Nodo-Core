
//#######################################################################################################
//#################################### Plugin-029: ExtWiredAnalog #######################################
//#######################################################################################################

/*********************************************************************************************\
 * Dit protocol zorgt voor uitlezen van de PCF8591 I2C ADC/DAC met 4 analoge ingangen (fabrikant NXP/Philips)
 * 
 * Auteur             : Nodo-team (Martinus van den Broek) www.nodo-domotica.nl
 * Support            : www.nodo-domotica.nl
 * Versie             : 1.2, 19-03-2015 Aanpassing t.b.v. Nodo release 3.9 (Paul Tonkes)) 
 *                      1.1, 25-01-2014 (Martinus van den Broek))
 * Nodo productnummer : 
 * Compatibiliteit    : Vanaf Nodo versie 3.8
 * Syntax             : "ExtWiredAnalog <Par1:Poort>, <Par2:Variable>"
 *********************************************************************************************
 * Technische beschrijving:
 *
 * Compiled size      : <grootte> bytes voor een Mega en <grootte> voor een Small.
 * Externe funkties   : <geef hier aan welke funkties worden gebruikt. 
 *
 * De PCF8591 is een IO Expander chip die via de I2C bus moet worden aangesloten
 * Het basis I2C adres = 0x48
 * Elke chip heeft 4 analoge ingangen en er kunnen 8 chips op de I2C bus worden gezet. Maximaal dus 32 analoge ingangen
 * Het commando leest de analoge <Poort> ingang uit en slaat de waarde op in <Variable>
 * De nummering van poorten loopt van 1 t/m 32
 *   Poort 1 = chip 1, ingang 1
 *   Poort 2 = chip 1, ingang 2
 *   Poort 5 = chip 2, ingang 1
 *   etc
 \*********************************************************************************************/

#define PLUGIN_NAME_029 "ExtWiredAnalog"
#define PLUGIN_ID 29

boolean Plugin_029(byte function, struct NodoEventStruct *event, char *string)
  {
  boolean success=false;

  #ifdef PLUGIN_029_CORE
  static byte portValue=0;

  switch(function)
    {
    
    case PLUGIN_COMMAND:
      {
      byte unit = (event->Par1-1) / 4;
      byte port = event->Par1 - (unit * 4);
      uint8_t address = 0x48 + unit;

      // get the current pin value
      Wire.beginTransmission(address);
      Wire.write(port-1);
      Wire.endTransmission();

      Wire.requestFrom(address, (uint8_t)0x2);
      if(Wire.available())
      {
        Wire.read(); // Read older value first (stored in chip)
        TempFloat=(float)Wire.read();// now read actual value and store into Nodo var
        UserVariableSet(event->Par2,TempFloat,true);
      }
      break;
      }
    #endif // CORE
    
    #if NODO_MEGA // alleen relevant voor een Nodo Mega want de Small heeft geen MMI!
    case PLUGIN_MMI_IN:
      {
      char *TempStr=(char*)malloc(INPUT_COMMAND_SIZE);

      if(GetArgv(string,TempStr,1))
        {
        if(strcasecmp(TempStr,PLUGIN_NAME_029)==0)
          {
          if(GetArgv(string,TempStr,2)) 
            {
            if(GetArgv(string,TempStr,3))
              {
              if(event->Par1>0 && event->Par1<33 && event->Par2>=0 && event->Par2<=USER_VARIABLES_MAX)            
                {
                  event->Type = NODO_TYPE_PLUGIN_COMMAND;
                  event->Command = 29; // Plugin nummer  
                  success=true;
                }
              }
            }
          }
        }
      free(TempStr);
      break;
      }

    case PLUGIN_MMI_OUT:
      {
      strcpy(string,PLUGIN_NAME_029);
      strcat(string," ");
      strcat(string,int2str(event->Par1));
      strcat(string,",");
      strcat(string,int2str(event->Par2));
      break;
      }
    #endif //MMI
    }
    
  return success;
  }
