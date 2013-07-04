//#######################################################################################################
//##################################### Device-13 Flamingo FA20RF Rookmelder ############################
//#######################################################################################################

/*********************************************************************************************\
 * Dit protocol zorgt voor ontvangst van Flamingo FA20RF rookmelder
 * 
 * Auteur             : Nodo-team (Martinus van den Broek) www.nodo-domotica.nl
 * Support            : www.nodo-domotica.nl
 * Datum              : Mrt.2013
 * Versie             : 1.0
 * Nodo productnummer : n.v.t. meegeleverd met Nodo code.
 * Compatibiliteit    : Vanaf Nodo build nummer 508
 * Syntax             : "SmokeAlert 0, <Par2: rookmelder ID>"
 *********************************************************************************************
 * Technische informatie:
 * De Flamingo FA20RF rookmelder bevat een RF zender en ontvanger. Standaard heeft elke unit een uniek ID
 * De rookmelder heeft een learn knop waardoor hij het ID van een andere unit kan overnemen
 * Daardoor kunnen ze onderling worden gekoppeld.
 * Na het koppelen hebben ze dus allemaal hetzelfde ID!
 * Je gebruikt 1 unit als master, waarvan de je code aanleert aan de andere units (slaves)
 \*********************************************************************************************/
#define FA20RFSTART                 3000
#define FA20RFSPACE                  800
#define FA20RFLOW                   1300
#define FA20RFHIGH                  2600

#ifdef DEVICE_013
#define DEVICE_ID 13
#define DEVICE_NAME "SmokeAlert"

boolean Device_013(byte function, struct NodoEventStruct *event, char *string)
{
  boolean success=false;

  switch(function)
  {
#ifdef DEVICE_CORE_013
  case DEVICE_RAWSIGNAL_IN:
    {
      RawSignal.Multiply=50;
      if (RawSignal.Number != 52) return false;

      unsigned long bitstream=0L;
      for(byte x=4;x<=50;x=x+2)
      {
        if (RawSignal.Pulses[x]*RawSignal.Multiply > 1800) bitstream = (bitstream << 1) | 0x1; 
        else bitstream = bitstream << 1;
      }

      event->Par1=0;
      event->Par2=bitstream;
      event->SourceUnit    = 0;                     // Komt niet van een Nodo unit af, dus unit op nul zetten
      event->Port          = VALUE_SOURCE_RF;

      return true;
      break;
    }
  case DEVICE_COMMAND:
    {
      break;
    }
#endif // DEVICE_CORE_013

#if NODO_MEGA
  case DEVICE_MMI_IN:
    {
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
#endif //DEVICE_13


