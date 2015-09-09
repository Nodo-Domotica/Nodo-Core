//#######################################################################################################
//################# PLUGIN-06 Temperatuur en Luchtvochtigheid sensor DHT 11 / 22 / 33 ###################
//#######################################################################################################

/*********************************************************************************************\
 * 
 * Auteur             : Nodo-team (Martinus van den Broek) www.nodo-domotica.nl
 * Support            : www.nodo-domotica.nl
 * Datum              : 05 Sept 2015
 * Versie             : 1.4
 *                      
 *                      Modificaties: 
 *                      2015-09, V1.4  Paul Tonkes: Compatibiliteit met DHT-33 (comments only)
 *                      2013-11, V1.3, Paul Tonkes: Geen events meer genereren bij vullen variabelen
 *                      2013-11, V1.2, Paul Tonkes: Retry bij fout tijdens uitlezen en DHT-22 compatibel
 *                         
 * Nodo productnummer : n.v.t. meegeleverd met Nodo code.
 * Compatibiliteit    : Vanaf Nodo build nummer 820
 * Syntax             : "DHTRead <Par1:Poortnummer>, <Par2:Basis Variabele>"
 *********************************************************************************************
 * Configuratie:
 * 
 * Deze plugin is zowel geschikt voor een DHT-11 , DHT-22 en de DHT-33 sensor.
 * Geef in je config_nn.c file op welke sensor 
 * je gebruikt met:
 * 
 * #define PLUGIN_006_CORE <DHT-nummer 11,22 of 33>
 * 
 * Verkeerde definitie zal leiden tot geen of foutieve uitlezing. Gebruik van de DHT-11 wordt afgeraden
 * omdat deze een vrij grove resolutie heeft en dus minder nauwkeurige meetwaarden levert.
 * 
 * DHT-22 = AM2302 = RHT02
 * DHT-33 = AM2303 = RHT04
 * 
 * Technische informatie:
 *  
 * De DHT sensor is een 3 pin sensor met een bidirectionele datapin. De datalijn verbind je met een WiredOut poort. 
 * Het principe is "onewire" maar dit protocol is NIET compatible met het bekende Dallas onewire protocol
 *
 * Dit protocol gebruikt twee variabelen, 1 voor temperatuur en 1 voor luchtvochtigheid.
 * Na uitlezen wordt de opgegeven variabele gevuld met de meetwaarden, maar er worden geen events gegenereerd.
 * Dit kan worden aangepast door de #define DHT_EVENT in true te veranderen.  
 *
 * Voorbeeld:
 * Als een DHT-33 wordt aangesloten op de WiredOut-1 (D30) van een Arduino Mega, dan zal onderstaand
 * commande de temperatuur plaatsen in variabele-1 en de vochtigheid in variabele-2.:
 * 
 * > DHTRead 1,1
 * Input=Serial; Unit=1; Event=DHTRead 1,1
 * Input=System; Unit=1; Event=Variable 1,22.800; Payload=0x11
 * Input=System; Unit=1; Event=Variable 2,48.300; Payload=0xD1
 *
 \*********************************************************************************************/

#define PLUGIN_ID 6
#define PLUGIN_NAME "DHTRead"
#define DHT_EVENT true                                                          // false=geen events genereren bij uitlezen, true genereert wel events.
uint8_t DHT_Pin;

#ifdef PLUGIN_006_CORE
/*********************************************************************************************\
 * DHT sub to get an 8 bit value from the receiving bitstream
 \*********************************************************************************************/
byte read_dht_dat(void)
  {
  byte i = 0;
  byte result=0;

  noInterrupts();
  for(i=0; i< 8; i++)
    {
    while(!digitalRead(DHT_Pin));                                               // wait for 50us
    delayMicroseconds(30);
    if(digitalRead(DHT_Pin)) 
      result |=(1<<(7-i));
    while(digitalRead(DHT_Pin));                                                // wait '1' finish
    }
  interrupts();

  return result;
  }
#endif // CORE


boolean Plugin_006(byte function, struct NodoEventStruct *event, char *string)
  {
  boolean success=false;
  
  switch(function)
    {
    #ifdef PLUGIN_006_CORE
  
    
    case PLUGIN_COMMAND:
      {
      DHT_Pin=PIN_WIRED_OUT_1+event->Par1-1;
      byte dht_dat[5];
      byte dht_in;
      byte i;
      byte Retry=0;

      UserVariablePayload(event->Par2,  0x0011); // Temperature
      UserVariablePayload(event->Par2+1,0x00d1); // Relative humidity %

      do
        {  
        pinMode(DHT_Pin,OUTPUT);                                                // DHT start condition, pull-down i/o pin for 18ms
        digitalWrite(DHT_Pin,LOW);                                              // Pull low
        delay(18);
        digitalWrite(DHT_Pin,HIGH);                                             // Pull high
        delayMicroseconds(40);
        pinMode(DHT_Pin,INPUT);                                                 // change pin to input
        delayMicroseconds(40);
        dht_in = digitalRead(DHT_Pin);

        if(!dht_in)
          {
          delayMicroseconds(80);
          dht_in = digitalRead(DHT_Pin);

          if(dht_in)
            {
            delayMicroseconds(40);                                              // now ready for data reception
            for (i=0; i<5; i++)
              dht_dat[i] = read_dht_dat();
              
            byte dht_check_sum = dht_dat[0]+dht_dat[1]+dht_dat[2]+dht_dat[3];   // check checksum. Checksum calculation is a Rollover Checksum by design.

            if(dht_dat[4]== dht_check_sum)
              {

              #if PLUGIN_006_CORE==11                                           // Code door de DHT-11 variant
              TempFloat=float(dht_dat[2]);                                      // Temperatuur
              UserVariableSet(event->Par2  ,TempFloat,DHT_EVENT);
              TempFloat=float(dht_dat[0]);                                      // Vochtigheid
              UserVariableSet(event->Par2+1,TempFloat,DHT_EVENT);
              #endif
              
              
              #if PLUGIN_006_CORE==22 || PLUGIN_006_CORE==33                    // Code door de DHT-22 of DHT-33 variant
              if (dht_dat[2] & 0x80)                                            // negative temperature
                {
                TempFloat=-0.1 * word(dht_dat[2] & 0x7F, dht_dat[3]);
                UserVariableSet(event->Par2,TempFloat,DHT_EVENT);
                }
              else
                {
                TempFloat=0.1 * word(dht_dat[2], dht_dat[3]);
                UserVariableSet(event->Par2,TempFloat,DHT_EVENT);
                }
              TempFloat=word(dht_dat[0], dht_dat[1]) * 0.1;                     // vochtigheid
              UserVariableSet(event->Par2+1,TempFloat,DHT_EVENT);
              #endif

              success=true;
              }
            }
          }
        if(!success)
          delay(1000);

        }while(!success && ++Retry<3);        
      break;
      }
    #endif // CORE
    
    #if NODO_MEGA
    case PLUGIN_MMI_IN:
      {
      char *TempStr=(char*)malloc(INPUT_COMMAND_SIZE);
  
      if(GetArgv(string,TempStr,1))
        {
        if(strcasecmp(TempStr,PLUGIN_NAME)==0)
          {
          if(event->Par1>0 && event->Par1<WIRED_PORTS && event->Par2>0 && event->Par2<=USER_VARIABLES_MAX_NR-1)
            {
            event->Type = NODO_TYPE_PLUGIN_COMMAND;
            event->Command = PLUGIN_ID;                                         // Plugin nummer  
            success=true;
            }
          }
        }
      free(TempStr);
      break;
      }
  
    case PLUGIN_MMI_OUT:
      {
      strcpy(string,PLUGIN_NAME);                                               // Eerste argument=het commando deel
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
