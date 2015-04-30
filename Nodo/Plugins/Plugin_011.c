                                              
//#######################################################################################################
//#################################### Plugin-011: OpenTherm GateWay plugin #############################
//#######################################################################################################

/*********************************************************************************************\
 * Dit protocol zorgt voor communicatie met een OTGW Opentherm gateway
 * 
 * Support            : www.nodo-domotica.nl
 *                      http://otgw.tclcode.com (OpenTherm Gateway OTGW informatie)
  
 *  
 * Versie             : Versie 0.8, 26-04-2015, Paul Tonkes: Testversie voor Nodo team
 *                      Versie 0.7, 16-04-2015, Paul Tonkes: Testversie voor Integratie in Nodo versie 3.8
 *                      Versie 0.6, 10-03-2015, Martinus van den Broek: Versie 0.6 Proof of Concept 0.6
 *
 * Nodo productnummer : U N D E R  C O N S T R U C T I O N !
 *   
 * Compatibiliteit    : Vanaf Nodo VERSIE 3.8 build nummer 812
 *			                De plugin gebruikt software serial voor communicatie met de OpenTherm Gateway.
 *                      en geeft daardoor mogelijk conflicten met andere Pin Change Int plugins (019,028,030,???)!
 *                      Ook zal deze plugin niet samenwerken met Plugin_023 (RGB-Led) 
 * 
 * Hardware           : De plugin maakt gebruik van de 2e generatie Nodo-penbezetting met extra lijnen voor
 *                      seriele communicatie. 
 *                      
 *                      LET OP: HW_25XX.H NIET GEBRUIKEN OP EEN NODO-UNO, NODO-DUE, NODO-MINI OF EEN NES.  
 *
 *                      Voor deze plugin wordt aangeraden een dedicated Arduino te gebruiken. Neem hierbij 
 *                      de penbezetting zoals gedefinieerd in HW-2501.h.
 *                      
 *                      Maak je gebruik van de OTGW hardware zoals aangeboden op http://otgw.tclcode.com 
 *                      sluit dan de TX lijn van de Arduino aan op de RX-lijn van de PIC16 op de OTGW alsmede
 *                      de RX lijn van de Arduino op de TX lijn van de RX van de PIC16. De Max232 chip uit het 
 *                      voetje halen.  
 *                      
 *                      Optioneel gebruiken:
 *                       
 *                      a) WiredOut-1 is hoog zolang er data wordt ontvangen van de OTGW.
 *                      b) WiredOut-2 kan worden gebruikt voor een LED: Licht op zodra Nodo variabele ontvangen of verzonden.      
 *
 * Configuratie       : Zet de volgende regels in het config bestand:
 *
 *                      #define HARDWARE_CONFIG 2501
 *                      #define PLUGIN_011
 *                      #define PLUGIN_011_CORE  <basisvariabele>
 *
 * Gebruik            : De belangrijkste OpenTherm waarden worden in variabelen geplaatst te beginnen
 *                      vanaf basisvariabele zoals opgegeven bij de configuratie:     
 *
 *			                <basisvariabele> wordt gebruikt om temperatuur setpoint in te stellen
 *			                <basisvariabele> + 1 wordt gebruikt voor monitoring van Room Temperature
 *			                <basisvariabele> + 2 wordt gebruikt voor monitoring van Boiler Water Temperature
 *			                <basisvariabele> + 3 wordt gebruikt voor monitoring van Modulation
 *			                <basisvariabele> + 4 wordt gebruikt voor monitoring van Boiler Water Pressure
 *			                <basisvariabele> + 5 wordt gebruikt voor monitoring van Thermostat Setpoint
 *			                <basisvariabele> + 6 wordt gebruikt voor monitoring van Flame status
 *			                <basisvariabele> + 7 wordt gebruikt voor monitoring van Boiler Return Water Temperature
 *			                <basisvariabele> + 8 wordt gebruikt voor monitoring van Domestic Hot Water Mode
 * 
 *                      Zodra de OpenTherm data wordt ontvangen van de OTGW, zal de data worden verstuurd 
 *                      als event. Er is geen Eventlist nodig om de data te verzenden en deze plugin kent geen 
 *                      commando's.  
 * 
 \*********************************************************************************************/

#define PLUGIN_ID                        11
#define PLUGIN_NAME                  "OTGW"

#define PLUGIN_011_BUFFERSIZE            40
#define PLUGIN_011_MAXVAR                 9
#define PLUGIN_011_WATCHDOG_TIMEOUT   60000                                     // Als de plugin x milliseconden geen data meer ontvangt van de PIC16, dan wordt de WiredOut lijn LAAG.
#define PLUGIN_011_LED_FLASH            500


// function prototypes for plugin
boolean PLUGIN_011_softSerialSend(char *cmd, int intdelay);
byte PLUGIN_011_hextobyte(char *string, byte pos);
char* PLUGIN_011_int2str(unsigned long x);
boolean PLUGIN_011_VarChange(byte Variable, float Value, unsigned long Data);

boolean PLUGIN_011_Debug=false;
unsigned long OTGW_Data, OTGW_PreviousData[PLUGIN_011_MAXVAR];
char PLUGIN_011_buffer[PLUGIN_011_BUFFERSIZE+1];
unsigned long PLUGIN_011_WatchDogTimer;
unsigned long PLUGIN_011_LedTimer;

void Plugin_011_FLC(void)                                                       // wordt vanuit de hoofdloop van de Nodo-core zeer frequent aangeroepen.
  {  
  static byte count=0;
  char received;

  if(PLUGIN_011_LedTimer>millis())
    digitalWrite(PIN_WIRED_OUT_2, HIGH);
  else
    digitalWrite(PIN_WIRED_OUT_2, LOW);

  while(Serial_available())
    {
    received=Serial_read();

    if(count<PLUGIN_011_BUFFERSIZE && isprint(received) && (isalpha(received) || isxdigit(received)))
      PLUGIN_011_buffer[count++]=received;

    if(received==0x0A)
      {
      PLUGIN_011_buffer[count]=0;

      PLUGIN_011_WatchDogTimer=millis() + PLUGIN_011_WATCHDOG_TIMEOUT;

      #if HARDWARE_SERIAL_1
      Serial.print(PLUGIN_011_buffer);
      #endif // HARDWARE_SERIAL_1
              
      byte source = PLUGIN_011_buffer[0];
      byte b1     = PLUGIN_011_hextobyte(PLUGIN_011_buffer,1);
      byte b2     = PLUGIN_011_hextobyte(PLUGIN_011_buffer,3);
      byte b3     = PLUGIN_011_hextobyte(PLUGIN_011_buffer,5);
      byte b4     = PLUGIN_011_hextobyte(PLUGIN_011_buffer,7);
      
      OTGW_Data=(((unsigned long)b1)<<24) + (((unsigned long)b2)<<16) + (((unsigned long)b3)<<8) + ((unsigned long)b4);
      
      if(source==0x54)                                                          // Source = Thermostat
        {
        if (b2 == 0x18)					                                                // +1 Room Temperature
          {
          TempFloat=b3+b4*((float)1/256);
          PLUGIN_011_VarChange(PLUGIN_011_CORE + 1, TempFloat, OTGW_Data);
          }

        if (b2 == 0x10)					                                                // +5 Thermostat Set Point
          {
          TempFloat=b3+b4*((float)1/256);
          PLUGIN_011_VarChange(PLUGIN_011_CORE + 5, TempFloat, OTGW_Data);
          }
        } // if(source==0x54)


      if(source==0x42)                                                          // Source = Boiler
        {
      	if (b2 == 0x19)					                                                // +2 Boiler Water Temperature
          {
          TempFloat=b3+b4*((float)1/256);
          PLUGIN_011_VarChange(PLUGIN_011_CORE + 2, TempFloat, OTGW_Data);
          }

        if (b2 == 0x11)					                                                // +3 Relative Modulation
          {
          TempFloat=b3+b4*((float)1/256);
          PLUGIN_011_VarChange(PLUGIN_011_CORE + 3, TempFloat, OTGW_Data);
          }

        if (b2 == 0x12)					                                                // +4 Boiler Water Pressure
          {
          TempFloat=b3+b4*((float)1/256);
          PLUGIN_011_VarChange(PLUGIN_011_CORE + 4, TempFloat, OTGW_Data);
          }

        if (b2 == 0x00)					                                                // Boiler Status
          {
          TempFloat=(float)((b4 & 0x8) >> 3);                                   // +6 Flame Status in bit 3
          PLUGIN_011_VarChange(PLUGIN_011_CORE + 6, TempFloat, OTGW_Data);

          TempFloat=(float)((b4 & 0x4) >> 2);	  		                            // +8 DHW Mode in bit 2
          PLUGIN_011_VarChange(PLUGIN_011_CORE + 8, TempFloat, OTGW_Data);
          }

        if (b2 == 0x1C)					                                                // +7 Boiler Water Return Temperature
          {
          TempFloat=b3+b4*((float)1/256);
          PLUGIN_011_VarChange(PLUGIN_011_CORE + 7, TempFloat, OTGW_Data);
          }

        }// if (source==0x42)
        
      #if HARDWARE_SERIAL_1
      Serial.println();
      #endif

      PLUGIN_011_buffer[0]=0;
      count=0;      
      } // if(received[x]==..)
    }// while
  }  


boolean Plugin_011(byte function, struct NodoEventStruct *event, char *string)
  {
  boolean success=false;

  switch(function)
    {    
    case PLUGIN_INIT:
      {
      Serial_begin(9600,0);                                                     // Communicatie tussen PIC en ATMega

      PLUGIN_011_WatchDogTimer=millis() + PLUGIN_011_WATCHDOG_TIMEOUT;

      #if HARDWARE_SERIAL_1
      Serial.begin(9600);                                                       // Communcatie ATMega via FTDI
      #endif

      FastLoopCall_ptr=&Plugin_011_FLC;                                         // Hiermee plaatsen we een in de hoofdloop van de Nodo een snelle scan-loop

      UserVariablePayload(PLUGIN_011_CORE  , 0x8010);                           // Gateway Setpoint            
      UserVariablePayload(PLUGIN_011_CORE+1, 0x8011);                           // Room Temperature            
      UserVariablePayload(PLUGIN_011_CORE+2, 0x8012);                           // Boiler Water Temperature    
      UserVariablePayload(PLUGIN_011_CORE+3, 0x8013);                           // Relative Modulation         
      UserVariablePayload(PLUGIN_011_CORE+4, 0x8014);                           // Boiler Water Pressure 
      UserVariablePayload(PLUGIN_011_CORE+5, 0x8015);                           // Thermostat Set Point        
      UserVariablePayload(PLUGIN_011_CORE+6, 0x8016);                           // Flame Status                
      UserVariablePayload(PLUGIN_011_CORE+7, 0x8017);                           // Boiler Water Return Temp.   
      UserVariablePayload(PLUGIN_011_CORE+8, 0x8018);                           // DHW Mode                    

  	  for(byte x=0;x<=PLUGIN_011_MAXVAR;x++)
        {
        OTGW_PreviousData[x]=0;
    		UserVariableGlobal(PLUGIN_011_CORE+x,true);                             // Variabelen globaal, zodat de Nodo deze direct verzend.
        }
	  
      break;
      }

    case PLUGIN_EVENT_IN:
      {
      if(event->Command==EVENT_VARIABLE && event->Type==NODO_TYPE_EVENT)        // Er is en variabele gewijzigd. 
        {
        if(event->Par1==PLUGIN_011_CORE)                                        // 1e variabele is voor de in te stellen temperatuur.
          {
          char *cmd=(char*)malloc(10);
          cmd[0]=0;
              
          UserVariable(PLUGIN_011_CORE, &TempFloat);
          byte value=(byte)TempFloat;
              
          strcpy(cmd,"TT=");
          strcat(cmd,PLUGIN_011_int2str(value));
              
          if ((TempFloat - value) > 0)
            strcat(cmd,".5");

          Serial_flush();
          Serial_println(cmd);
          delay(100);
          
          #if HARDWARE_SERIAL_1
          if(PLUGIN_011_Debug)
            {
            Serial.print(F("("));
            Serial.print(cmd);
            Serial.println(F(") *** Command ***"));
            }
          #endif
          
          free(cmd);

          UserVariableSet(PLUGIN_011_CORE+5, TempFloat, true);                  // We vullen direct de 'Thermostat Setpoint' omdat de PIC op de OTGW deze waarde pas
                                                                                // na een minuut verwerkt waardoor de spinbox van de WebApp gedurende lange tijd niet
                                                                                // op de juiste waarde staat. 
          PLUGIN_011_LedTimer=millis()+ PLUGIN_011_LED_FLASH;                   // Laat LED op WiredOut-2 oplichten.
          }
        }
      break;
      }


    case PLUGIN_ONCE_A_SECOND:
      {
      if(Serial.available())
        PLUGIN_011_Debug=true;
        
      if(PLUGIN_011_WatchDogTimer<millis())
        {
        digitalWrite(PIN_WIRED_OUT_1, LOW);

        #if HARDWARE_SERIAL_1
        Serial.println(F("*** No data received from OTGW! ***"));//???
        #endif
        }
      else
        {
        digitalWrite(PIN_WIRED_OUT_1, HIGH);
        }
        
  
      } // case PLUGIN_ONCE_A_SECOND
    }
  return success;
  }


byte PLUGIN_011_hextobyte(char *string, byte pos)
  {
  byte b1 = string[pos] - '0';
  byte b2 = string[pos+1] -'0';
  if (b1 > 16) b1=b1-7;
  if (b2 > 16) b2=b2-7;
  return b1*16 + b2;
  }

char* PLUGIN_011_int2str(unsigned long x)
  {
  static char OutputLine[12];
  char* OutputLinePosPtr=&OutputLine[10];
  int y;

  *OutputLinePosPtr=0;

  if(x==0)
    {
    *--OutputLinePosPtr='0';
    }
  else
    {  
    while(x>0)
      {
      *--OutputLinePosPtr='0'+(x%10);
      x/=10;
      }
    }    
  return OutputLinePosPtr;
  }


boolean PLUGIN_011_VarChange(byte Variable, float Value, unsigned long Data)
  {
  boolean Change=OTGW_PreviousData[Variable]!=Data;
  
  UserVariableSet(PLUGIN_011_CORE+Variable, Value, Change);
  OTGW_PreviousData[Variable]=Data;

  if(Change)
    PLUGIN_011_LedTimer=millis()+ PLUGIN_011_LED_FLASH;                         // Laat LED op WiredOut-2 oplichten.

  
  #if HARDWARE_SERIAL_1
  if(PLUGIN_011_Debug)
    {
    switch(Variable-PLUGIN_011_CORE)
      {
      case 1:
        Serial.print(F(" (Room Temperature="));Serial.print(TempFloat);Serial.print(")");
        break;
      
      case 2:
        Serial.print(F(" (Boiler Water Temperature="));Serial.print(TempFloat);Serial.print(")");
        break;
      
      case 3:
        Serial.print(F(" (Relative modulation:"));Serial.print(TempFloat);Serial.print(")");
        break;
      
      case 4:
        Serial.print(F(" (Boiler water pressure="));Serial.print(TempFloat);Serial.print(")");
        break;
      
      case 5:
        Serial.print(F(" (Thermostat Setpoint="));Serial.print(TempFloat);Serial.print(")");
        break;
      
      case 6:
        Serial.print(F(" (Flame status="));Serial.print(TempFloat);Serial.print(")");
        break;
      
      case 7:
        Serial.print(F(" (Boiler water return temp="));Serial.print(TempFloat);Serial.print(")");
        break;
      
      case 8:
        Serial.print(F(" (Domestic hot water mode="));Serial.print(TempFloat);Serial.print(")");
        break;
      }  
  
    if(Change)
      Serial.print(F(" *** Changed ***"));
    }
  #endif // HARDWARE_SERIAL_1
  
  } 

