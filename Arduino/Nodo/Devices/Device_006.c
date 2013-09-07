//#######################################################################################################
//######################## DEVICE-06 Temperatuur en Luchtvochtigheid sensor DHT 11 ######################
//#######################################################################################################

/*********************************************************************************************\
 * Dit protocol zorgt voor ontvangst van Alecto weerstation buitensensoren met protocol V1
 * 
 * Auteur             : Nodo-team (Martinus van den Broek) www.nodo-domotica.nl
 * Support            : www.nodo-domotica.nl
 * Datum              : 12 Aug 2013
 * Versie             : 1.1
 * Nodo productnummer : n.v.t. meegeleverd met Nodo code.
 * Compatibiliteit    : Vanaf Nodo build nummer 555
 * Syntax             : "DHT11 <Par1:Poortnummer>, <Par2:Basis Variabele>"
 *********************************************************************************************
 * Technische informatie:
 * De DHT11 sensor is een 3 pin sensor met een bidirectionele datapin
 * Het principe is "onewire" maar dit protocol is NIET compatible met het bekende Dallas onewire protocol
 * Dit protocol gebruikt twee variabelen, 1 voor temperatuur en 1 voor luchtvochtigheid
 \*********************************************************************************************/

#define DEVICE_ID 06
#define DEVICE_NAME "DHT11Read"
uint8_t DHT11_Pin;

#ifdef DEVICE_006_CORE
/*********************************************************************************************\
 * DHT11 sub to get an 8 bit value from the receiving bitstream
 \*********************************************************************************************/
byte read_dht11_dat(void)
  {
  byte i = 0;
  byte result=0;
  noInterrupts();
  for(i=0; i< 8; i++)
    {
    while(!digitalRead(DHT11_Pin));  // wait for 50us
    delayMicroseconds(30);
    if(digitalRead(DHT11_Pin)) 
      result |=(1<<(7-i));
    while(digitalRead(DHT11_Pin));  // wait '1' finish
    }
  interrupts();
  return result;
  }
#endif // CORE


boolean Device_006(byte function, struct NodoEventStruct *event, char *string)
  {
  boolean success=false;

  switch(function)
    {
    #ifdef DEVICE_006_CORE
  
    case DEVICE_COMMAND:
      {
      DHT11_Pin=PIN_WIRED_OUT_1+event->Par1-1;
      byte dht11_dat[5];
      byte dht11_in;
      byte i;
      byte temperature=0;
      byte humidity=0;
  
      pinMode(DHT11_Pin,OUTPUT);
  
      // DHT11 start condition, pull-down i/o pin for 18ms
      digitalWrite(DHT11_Pin,LOW);               // Pull low
      delay(18);
      digitalWrite(DHT11_Pin,HIGH);              // Pull high
      delayMicroseconds(40);
      pinMode(DHT11_Pin,INPUT);                  // change pin to input
      delayMicroseconds(40);
  
      dht11_in = digitalRead(DHT11_Pin);
      if(dht11_in) return false;
  
      delayMicroseconds(80);
      dht11_in = digitalRead(DHT11_Pin);
      if(!dht11_in) return false;
  
      delayMicroseconds(40);                     // now ready for data reception
      for (i=0; i<5; i++)
        dht11_dat[i] = read_dht11_dat();
        
      // Checksum calculation is a Rollover Checksum by design!
      byte dht11_check_sum = dht11_dat[0]+dht11_dat[1]+dht11_dat[2]+dht11_dat[3];// check check_sum
      if(dht11_dat[4]!= dht11_check_sum)
        return false;
  
      temperature = dht11_dat[2];
      humidity = dht11_dat[0];
  
      byte VarNr = event->Par2; // De originele Par1 tijdelijk opslaan want hier zit de variabelenummer in waar de gebruiker de uitgelezen waarde in wil hebben

      ClearEvent(event);                                      // Ga uit van een default schone event. Oude eventgegevens wissen.
      
      event->Type         = NODO_TYPE_COMMAND;
      event->Command      = CMD_VARIABLE_SET;                 // Commando "VariableSet"
      event->Par1         = VarNr;                            // Par1 is de variabele die we willen vullen.
      event->Par2         = float2ul(float(temperature));
      QueueAdd(event);                                        // Event opslaan in de event queue, hierna komt de volgende meetwaarde
      event->Par1         = VarNr+1;                          // Par1+1 is de variabele die we willen vullen voor luchtvochtigheid
      event->Par2         = float2ul(float(humidity));
      QueueAdd(event);
      success=true;
  
      break;
      }
    #endif // CORE
    
    #if NODO_MEGA
    case DEVICE_MMI_IN:
      {
      char *TempStr=(char*)malloc(26);
      string[25]=0;
  
      if(GetArgv(string,TempStr,1))
        {
        if(strcasecmp(TempStr,DEVICE_NAME)==0)
          {
          if(event->Par1>0 && event->Par1<WIRED_PORTS && event->Par2>0 && event->Par2<=USER_VARIABLES_MAX-1)
            {
            event->Type = NODO_TYPE_DEVICE_COMMAND;
            event->Command = 6; // Device nummer  
            success=true;
            }
          }
        }
      free(TempStr);
      break;
      }
  
    case DEVICE_MMI_OUT:
      {
      strcpy(string,DEVICE_NAME);            // Eerste argument=het commando deel
      strcat(string," ");
      strcat(string,int2str(event->Par1));
      strcat(string,",");
      strcat(string,int2str(event->Par2));
      break;
      }
    #endif // MMI
    }      
  return success;
  }
