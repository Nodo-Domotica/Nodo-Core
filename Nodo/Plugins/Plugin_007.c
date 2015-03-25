//#######################################################################################################
//#################################### Plugin-007: UserEvent   ############################################
//#######################################################################################################


/*********************************************************************************************\
* Funktionele beschrijving: Dit device makt dat de Nodo compatibel is voor ontvangst 
*                           van UserEvents volgens het oude signaalformat van de Nodo Due.
*
* Auteur             : p.k.tonkes@gmail.com (Paul Tonkes))
* Support            : www.nodo-domotica.nl
* Datum              : 10-09-2013
* Versie             : 1.0
* Nodo productnummer : 
* Compatibiliteit    : 573
* Syntax             : n.v.t.
*
***********************************************************************************************
* Technische beschrijving:
*
* Compiled size      :  848 bytes voor een Mega.
*
* Oude Nodo versies maakten gebruik van een 32-bit signaal. UserEvents werden eveneens in dit format
* verzonden. Vanaf de 3.6 versie is gekozen voor een technisch geavanceerdere opbouw van RF en IR codes 
* Consequentie hiervan is dat het oude formaat niet meer werd herkend door de Nodo. Deze plugin zorgt er
* voor dat de oude UserEvents nog kunnen worden ingelezen en verwerkt.
* 
* LET OP: In het oude format wordt geen gebruik gemaakt van het Home adres van
*         een Nodo. Events komen ongeacht de home definitie altijd binnen.
* 
\*********************************************************************************************/
 
#define PLUGIN_NAME "UserEvent"
#define PLUGIN_ID   7

boolean Plugin_007(byte function, struct NodoEventStruct *event, char *string)
  {
  boolean success=false;
  unsigned long bitstream=0L;
  int x,z;

  switch(function)
    {    
    #ifdef PLUGIN_007_CORE
    case PLUGIN_RAWSIGNAL_IN:
      {
      // NODO signaal bestaat uit start bit + 32 bits. Als ongelijk aan 66, dan geen Nodo signaal
      if(RawSignal.Number==66)
        {
        // 0=aantal, 1=startpuls, 2=space na startpuls, 3=1e pulslengte. Dus start loop met drie.
        z=0;
        for(x=3;x<=RawSignal.Number;x+=2)
          {
          if((RawSignal.Pulses[x]*RawSignal.Multiply)>1000) //??? NODO_PULSE_MID      
            bitstream|=(long)(1L<<z); //LSB in signaal wordt  als eerste verzonden
          z++;
          }
  
        // We hoeven alleen maar compatible te zijn met de userevents van de oude Nodo.
        // in code 100 heeft in de vorige versies altijd het userevent gezeten.
        if(((bitstream>>16)&0xff)==100)
          {
          RawSignal.Repeats    = true; // het is een herhalend signaal. Bij ontvangst herhalingen onderdrukken.
          ClearEvent(event);
          event->SourceUnit=(bitstream>>24)&0xf;
          event->DestinationUnit=0;
          event->Type=NODO_TYPE_EVENT;
          event->Command=EVENT_USEREVENT;
          event->Par1=(bitstream>>8)&0xff;
          event->Par2=bitstream&0xff;
          success=true;
          }
        }
      }
    #endif // CORE
    
    #if NODO_MEGA // alleen relevant voor een Nodo Mega want de Small heeft geen MMI!
    case PLUGIN_MMI_IN:
      {
      char *TempStr=(char*)malloc(INPUT_COMMAND_SIZE);

      if(GetArgv(string,TempStr,1))
        {
        if(strcasecmp(TempStr,PLUGIN_NAME)==0)
          {
          event->Type    = NODO_TYPE_EVENT;
          event->Command = PLUGIN_ID;
          success=true;
          }
        }
      free(TempStr);
      break;
      }

    case PLUGIN_MMI_OUT:
      {
      strcpy(string,PLUGIN_NAME);               // Commando 
      strcat(string," ");
      strcat(string,int2str(event->Par1));      // Parameter-1 (8-bit)
      strcat(string,",");
      strcat(string,int2str(event->Par2));      // Parameter-2 (32-bit)
      break;
      }
    #endif //MMI
    }    
  return success;
  }

