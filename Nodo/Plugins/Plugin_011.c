//#######################################################################################################
//#################################### Plugin-011: OpenTherm GateWay plugin #############################
//#######################################################################################################

/*********************************************************************************************\
 * Dit protocol zorgt voor communicatie met een OTGW Opentherm gateway
 * 
 * Support            : www.nodo-domotica.nl
 *  
 * Versie             : Versie 0.7  16-04-2015, Paul Tonkes: Testversie voor Integratie in Nodo versie 3.8
 *                      Versie 0.6, 10-03-2015, Martinus van den Broek: Versie 0.6 Proof of Concept 0.6
 *
 * Nodo productnummer : U N D E R  C O N S T R U C T I O N !
 *   
 * Compatibiliteit    : Vanaf Nodo VERSIE 3.9 build nummer 807
 *			                De plugin gebruikt software serial voor communicatie met de otgw
 *                      en geeft daardoor mogelijk conflicten met andere Pin Change Int plugins (019,028,030,???)!
 * 
 * Hardware           : De plugin maakt gebruik van de 2e generatie Nodo-penbezetting met extra lijnen voor
 *                      seriele communicatie. Gebruik bijvoorbeeld HW-2501.h als hardware-configuratiebestand.
 *                      
 *                      LET OP: HW_25XX.H NIET GEBRUIKEN OP EEN NODO-UNO, NODO-DUE, NODO-MINI OF EEN NES.  
 *
 *                      Voor deze plugin wordt aangeraden een dedicated Arduino te gebruiken. Neem hierbij 
 *                      de penbezetting zoals gedefinieerd in HW-2501.h.  
 *
 * Configuratie       : Zet de volgende regels in het config bestand:
 *
 *                      #define HARDWARE_CONFIG 2501
 *                      #define PLUGIN_011
 *                      #define PLUGIN_011_CORE  1
 *
 * Gebruik            : De belangrijkste OpenThem waarden worden in variabelen geplaatst te beginnen
 *                      vanaf basisvariabele zoals opgegeven bij de configuratie:     
 *
 *			                <basisvariabele>     wordt gebruikt om temperatuur setpoint in te stellen
 *			                <basisvariabele> + 1 wordt gebruikt voor monitoring van Room Temperature
 *			                <basisvariabele> + 2 wordt gebruikt voor monitoring van Boiler Water Temperature
 *			                <basisvariabele> + 3 wordt gebruikt voor monitoring van Modulation
 *			                <basisvariabele> + 4 wordt gebruikt voor monitoring van Boiler Water Pressure
 *			                <basisvariabele> + 5 wordt gebruikt voor monitoring van Thermostat Setpoint
 *			                <basisvariabele> + 6 wordt gebruikt voor monitoring van Flame status
 *			                <basisvariabele> + 7 wordt gebruikt voor monitoring van Boiler Return Water Temperature
 *			                <basisvariabele> + 8 wordt gebruikt voor monitoring van Domestic Hot Water Mode
 *
\*********************************************************************************************/

#define PLUGIN_ID   11
#define PLUGIN_NAME "OTGW"

#define PLUGIN_011_BUFFERSIZE   40

#ifndef PLUGIN_011_DEBUG
  #define PLUGIN_011_DEBUG false
#endif

char PLUGIN_011_buffer[PLUGIN_011_BUFFERSIZE+1];

// function prototypes for plugin
boolean PLUGIN_011_softSerialSend(char *cmd, int intdelay);
byte PLUGIN_011_SerialRead();
byte PLUGIN_011_hextobyte(char *string, byte pos);
char* PLUGIN_011_int2str(unsigned long x);

boolean Plugin_011(byte function, struct NodoEventStruct *event, char *string)
  {
  boolean success=false;

  switch(function)
    {    
    #ifdef PLUGIN_011_CORE

    case PLUGIN_INIT:
      {
      Serial_begin(9600,0);

      // Zie Nodo-payload tabel
      UserVariablePayload(PLUGIN_011_CORE  , 0x8010); // Gateway Setpoint            
      UserVariablePayload(PLUGIN_011_CORE+1, 0x8011); // Room Temperature            
      UserVariablePayload(PLUGIN_011_CORE+2, 0x8012); // Thermostat Set Point        
      UserVariablePayload(PLUGIN_011_CORE+3, 0x8013); // Boiler Water Temperature    
      UserVariablePayload(PLUGIN_011_CORE+4, 0x8014); // Relative Modulation         
      UserVariablePayload(PLUGIN_011_CORE+5, 0x8015); // Boiler Water Pressure       
      UserVariablePayload(PLUGIN_011_CORE+6, 0x8016); // Flame Status                
      UserVariablePayload(PLUGIN_011_CORE+7, 0x8017); // DHW Mode                    
      UserVariablePayload(PLUGIN_011_CORE+8, 0x8018); // Boiler Water Return Temp.   

      break;
      }

    case PLUGIN_COMMAND:
      {
      if (event->Par1 == CMD_VARIABLE_SET)
        {
        char *cmd=(char*)malloc(10);
        cmd[0]=0;
            
        UserVariable(event->Par2, &TempFloat);
        byte value=(byte)TempFloat;
            
        strcpy(cmd,"TT=");
        strcat(cmd,PLUGIN_011_int2str(value));
            
        if ((TempFloat - value) > 0)
          strcat(cmd,".5");
        PLUGIN_011_softSerialSend(cmd,100);
        free(cmd);
        }
      success = true;
      break;
      }

  case PLUGIN_ONCE_A_SECOND:
    {
    byte count = PLUGIN_011_SerialRead();
    if (count > 0)
      for (byte x=0; x <= count ; x++)
        {
        if (PLUGIN_011_buffer[x]==0x0D) // scan for EOL
          {
          char source = PLUGIN_011_buffer[x-9];
          byte b1 = PLUGIN_011_hextobyte(PLUGIN_011_buffer,x-8);
          byte b2 = PLUGIN_011_hextobyte(PLUGIN_011_buffer,x-6);
          byte b3 = PLUGIN_011_hextobyte(PLUGIN_011_buffer,x-4);
          byte b4 = PLUGIN_011_hextobyte(PLUGIN_011_buffer,x-2);

          if (source=='T')
            {
            if (b2 == 0x18)					// Room Temperature
              {
              float roomtemp=b3+b4*((float)1/256);
              Serial.print("RT ");
              Serial.println(roomtemp);
              UserVariableSet(PLUGIN_011_CORE+1,roomtemp,false);
              }

            if (b2 == 0x10)					// Thermostat Set Point
              {
              float setpoint=b3+b4*((float)1/256);
              Serial.print("TC ");
              Serial.println(setpoint);
              UserVariableSet(PLUGIN_011_CORE+5,setpoint,false);
              }
            }

           if (source=='B')
             {
             if (b2 == 0x19)					// Boiler Water Temperature
               {
               float boilertemp=b3+b4*((float)1/256);
               Serial.print("BWT ");
               Serial.println(boilertemp);
               UserVariableSet(PLUGIN_011_CORE+2,boilertemp,false);
               }
  
             if (b2 == 0x11)					// Relative Modulation
               {
               float modulation=b3+b4*((float)1/256);
               Serial.print("MOD ");
               Serial.println(modulation);
               UserVariableSet(PLUGIN_011_CORE+3,modulation,false);
               }
  
             if (b2 == 0x12)					// Boiler Water Pressure
               {
               float pressure=b3+b4*((float)1/256);
               Serial.print("PR ");
               Serial.println(pressure);
               UserVariableSet(PLUGIN_011_CORE+4,pressure,false);
               }
  
             if (b2 == 0x00)					// Boiler Status
               {
               byte status=(b4 & 0x8) >> 3;			// Flame Status in bit 3
               Serial.print("FS ");
               Serial.println(status);
               TempFloat=(float)status;
               UserVariableSet(PLUGIN_011_CORE+6,TempFloat,false);
               status=(b4 & 0x4) >> 2;			// DHW Mode in bit 2
               Serial.print("DHWM ");
               Serial.println(status);
               TempFloat=(float)status;
               UserVariableSet(PLUGIN_011_CORE+7,TempFloat,false);
               }
  
             if (b2 == 0x1C)					// Boiler Water Return Temperature
               {
               float boilerRtemp=b3+b4*((float)1/256);
               Serial.print("BWRT ");
               Serial.println(boilerRtemp);
               UserVariableSet(PLUGIN_011_CORE+8,boilerRtemp,false);
               }
             }
           } // eol
         } // for
        break;
      } // case PLUGIN_ONCE_A_SECOND

    #endif // CORE

    #if NODO_MEGA
    case PLUGIN_MMI_IN:
      {
      char *TempStr=(char*)malloc(INPUT_COMMAND_SIZE);

      if(GetArgv(string,TempStr,1))
        {
        if(strcasecmp(TempStr,PLUGIN_NAME)==0)
          {
          if(GetArgv(string,TempStr,2)) 
            {
              event->Type = NODO_TYPE_PLUGIN_COMMAND;
              event->Command = PLUGIN_ID; // Plugin nummer  
              success=true;
            }
          }
        }
      free(TempStr);
      break;
      }

    case PLUGIN_MMI_OUT:
      {
      strcpy(string,PLUGIN_NAME);
      strcat(string," ");
      strcat(string,cmd2str(event->Par1));
      strcat(string,",");
      strcat(string,int2str(event->Par2));
      break;
      }
    #endif //MMI

    }
    return success;
  }

#ifdef PLUGIN_011_CORE
boolean PLUGIN_011_softSerialSend(char *cmd, int intdelay)
{
  Serial_flush();

  #if PLUGIN_011_DEBUG
    Serial.println(cmd);
  #endif

  Serial_println(cmd);
  delay(intdelay);
}

byte PLUGIN_011_SerialRead()
{
 char received;
 byte count=0;
 PLUGIN_011_buffer[0]=0;
 if (Serial_available())
        {
          while (Serial_available())
          {
            received=Serial_read();
            if(count<PLUGIN_011_BUFFERSIZE)
              PLUGIN_011_buffer[count++]=received;
            Serial.print(received);
          }
          Serial.println();
        }
  PLUGIN_011_buffer[count]=0;
  return count; 
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

#endif