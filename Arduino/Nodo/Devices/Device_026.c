//#######################################################################################################
//#################################### Device-026: ExtWiredIn ###########################################
//#######################################################################################################

/*********************************************************************************************\
 * Dit protocol zorgt voor uitlezen van de PCF8574 I2C IO-Expander (fabrikant NXP/Philips)
 * 
 * Auteur             : Nodo-team (Martinus van den Broek) www.nodo-domotica.nl
 * Support            : www.nodo-domotica.nl
 * Datum              : 10 Sep 2013
 * Versie             : 1.0
 * Nodo productnummer : 
 * Compatibiliteit    : Vanaf Nodo build nummer 555
 * Syntax             : "ExtWiredIn <Par1:Poort>, <Par2:On/Off>"
 *********************************************************************************************
 * Technische beschrijving:
 *
 * Compiled size      : <grootte> bytes voor een Mega en <grootte> voor een Small.
 * Externe funkties   : <geef hier aan welke funkties worden gebruikt. 
 *
 * De PCF8574 is een IO Expander chip die via de I2C bus moet worden aangesloten
 * Het basis I2C adres = 0x20
 * Elke chip heeft 8 digitale pinnen die we hier als input gebruiken.
 *   i.v.m. technische beperkingen (performance) lezen we op dit moment maar 1 chip uit op het basis adres
 *   daarom worden nu maximaal 8 inputs ondersteund
 \*********************************************************************************************/

#define DEVICE_NAME_026 "ExtWiredIn"
#define DEVICE_ID 26

boolean Device_026(byte function, struct NodoEventStruct *event, char *string)
  {
  boolean success=false;

  #ifdef DEVICE_026_CORE
  static byte portValue=0;

  switch(function)
    {
    case DEVICE_INIT:
    {
      // get the current pin status
      uint8_t address = 0x20;
      Wire.requestFrom(address, (uint8_t)0x1);
      if(Wire.available()) portValue = Wire.read();
      break;
    }
    
    case DEVICE_ONCE_A_SECOND:
      {
      byte tmpPortValue=0;
      uint8_t address = 0x20;

      // get the current pin status
      Wire.requestFrom(address, (uint8_t)0x1);
      if(Wire.available())
      {
        tmpPortValue = Wire.read();
        if (tmpPortValue != portValue)
          {
            for (byte x=0; x<8; x++)
              {
                if ((portValue & (1 << x)) != (tmpPortValue & (1 << x)))
                  {
                    struct NodoEventStruct tmpEvent;
                    ClearEvent(&tmpEvent);
                    tmpEvent.Type         = NODO_TYPE_DEVICE_EVENT;
                    tmpEvent.Command      = DEVICE_ID;
                    tmpEvent.Par1         = x+1;
                    tmpEvent.Par2         = VALUE_ON;
                    if (tmpPortValue & (1 << x)) tmpEvent.Par2 = VALUE_OFF;
                    ProcessEvent2(&tmpEvent);
                    //QueueAdd(&tmpEvent);
                  }
              }
            portValue = tmpPortValue;
          }
        success=true;
      }
      break;
      }
    #endif // CORE
    
    #if NODO_MEGA // alleen relevant voor een Nodo Mega want de Small heeft geen MMI!
    case DEVICE_MMI_IN:
      {
      char *TempStr=(char*)malloc(26);
      string[25]=0;

      if(GetArgv(string,TempStr,1))
        {
        if(strcasecmp(TempStr,DEVICE_NAME_026)==0)
          {
          if(GetArgv(string,TempStr,2)) 
            {
            if(GetArgv(string,TempStr,3))
              {
              if(event->Par1>0 && event->Par1<65 && (event->Par2==VALUE_ON || event->Par2==VALUE_OFF))            
                {
                  event->Type = NODO_TYPE_DEVICE_EVENT;
                  event->Command = 26; // Device nummer  
                  success=true;
                }
              }
            }
          }
        }
      free(TempStr);
      break;
      }

    case DEVICE_MMI_OUT:
      {
      strcpy(string,DEVICE_NAME_026);
      strcat(string," ");
      strcat(string,int2str(event->Par1));
      strcat(string,",");
      if(event->Par2==VALUE_ON)
        strcat(string,"On");  
      else strcat(string,"Off");
      break;
      }
    #endif //MMI
    }
    
  return success;
  }
