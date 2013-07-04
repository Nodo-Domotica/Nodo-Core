//#######################################################################################################
//##################################### Device-14 Flamingo FA20RF Rookmelder ############################
//#######################################################################################################

/*********************************************************************************************\
 * Dit protocol zorgt voor aansturen van Flamingo FA20RF rookmelder
 * 
 * Auteur             : Nodo-team (Martinus van den Broek) www.nodo-domotica.nl
 * Support            : www.nodo-domotica.nl
 * Datum              : Mrt.2013
 * Versie             : 1.0
 * Nodo productnummer : n.v.t. meegeleverd met Nodo code.
 * Compatibiliteit    : Vanaf Nodo build nummer 508
 * Syntax             : "SmokeAlertSend 0, <Par2: rookmelder ID>"
 *********************************************************************************************
 * Technische informatie:
 * De Flamingo FA20RF rookmelder bevat een RF zender en ontvanger. Standaard heeft elke unit een uniek ID
 * De rookmelder heeft een learn knop waardoor hij het ID van een andere unit kan overnemen
 * Daardoor kunnen ze onderling worden gekoppeld.
 * Na het koppelen hebben ze dus allemaal hetzelfde ID!
 * Je gebruikt 1 unit als master, waarvan de je code aanleert aan de andere units (slaves)
 *
 * Let op: De rookmelder geeft alarm zolang dit bericht wordt verzonden en stopt daarna automatisch
 \*********************************************************************************************/

#ifdef DEVICE_014
#define DEVICE_ID 14
#define DEVICE_NAME "SmokeAlertSend"

boolean Device_014(byte function, struct NodoEventStruct *event, char *string)
  {
  boolean success=false;

  switch(function)
  {
#ifdef DEVICE_CORE_014
  case DEVICE_RAWSIGNAL_IN:
    {
      break;
    }

  case DEVICE_COMMAND:
    {
    unsigned long bitstream=event->Par2;
    RawSignal.Multiply=50;
    RawSignal.Repeats=1;
    RawSignal.Delay=0;
    RawSignal.Pulses[1]=FA20RFSTART/RawSignal.Multiply;
    RawSignal.Pulses[2]=FA20RFSPACE/RawSignal.Multiply;
    RawSignal.Pulses[3]=FA20RFSPACE/RawSignal.Multiply;
    for(byte x=49;x>=3;x=x-2)
      {
      RawSignal.Pulses[x]=FA20RFSPACE/RawSignal.Multiply;
      if ((bitstream & 1) == 1) RawSignal.Pulses[x+1] = FA20RFHIGH/RawSignal.Multiply; 
      else RawSignal.Pulses[x+1] = FA20RFLOW/RawSignal.Multiply;
      bitstream = bitstream >> 1;
      }

    RawSignal.Pulses[51]=FA20RFSPACE/RawSignal.Multiply;
    RawSignal.Pulses[52]=0;
    RawSignal.Number=52;

    for (byte x =0; x<50; x++) RawSendRF();
    break;
    } 
#endif // DEVICE_CORE_014

#if NODO_MEGA
  case DEVICE_MMI_IN:
    {
    char *TempStr=(char*)malloc(26);
    string[25]=0;

    if(GetArgv(string,TempStr,1))
      {
      if(strcasecmp(TempStr,DEVICE_NAME)==0)
        success=true;
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
#endif //NODO_MEGA
  }      
  return success;
}
#endif //DEVICE_14


