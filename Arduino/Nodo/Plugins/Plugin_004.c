//#######################################################################################################
//#################################### Plugin-04: NewKAKUSend ###########################################
//#######################################################################################################

/*********************************************************************************************\
 * Dit protocol zorgt voor aansturing van Klik-Aan-Klik-Uit ontvangers
 * die werken volgens de automatische codering (Ontvangers met leer-knop) Dit protocol is 
 * eveneens bekend onder de naam HomeEasy. Het protocol ondersteunt eveneens dim funktionaliteit.
 * 
 * Auteur             : Nodo-team (P.K.Tonkes) www.nodo-domotca.nl
 * Support            : www.nodo-domotica.nl
 * Datum              : Jan.2013
 * Versie             : 1.1
 * Nodo productnummer : n.v.t. meegeleverd met Nodo code.
 * Compatibiliteit    : Vanaf Nodo build nummer 508
 * Compiled size      : ??? bytes voor een Mega, ??? voor een Small
 * Syntax             : "SendNewKAKU <Adres>,<On|Off|dimlevel 1..15>
 ***********************************************************************************************
 * Vereiste library   : - geen -
 * Externe funkties   : strcat(),  strcpy(),  cmd2str(),  tolower(),  GetArgv(),  str2cmd()
 *
 * Pulse (T) is 275us PDM
 * 0 = T,T,T,4T, 1 = T,4T,T,T, dim = T,T,T,T op bit 27
 *
 * NewKAKU ondersteund:
 *   on/off       ---- 000x Off/On
 *   all on/off   ---- 001x AllOff/AllOn
 *   dim absolute xxxx 0110 Dim16        // dim op bit 27 + 4 extra bits voor dim level
 *
 *  NewKAKU bitstream= (First sent) AAAAAAAAAAAAAAAAAAAAAAAAAACCUUUU(LLLL) -> A=KAKU_adres, C=commando, U=KAKU-Unit, L=extra dimlevel bits (optioneel)
 *
 * Interne gebruik van de parameters in het Nodo event:
 * 
 * Cmd  : Hier zit het commando SendNewKAKU of het event NewKAKU in. Deze gebruiken we verder niet.
 * Par1 : Commando VALUE_ON, VALUE_OFF of dim niveau [1..15]
 * Par2 : Adres
 *
 \*********************************************************************************************/

#define PLUGIN_ID 04
#define PLUGIN_NAME "NewKAKUSend"
#define NewKAKU_RawSignalLength      132
#define NewKAKUdim_RawSignalLength   148
#define NewKAKU_1T                   275        // us
#define NewKAKU_mT                   500        // us, midden tussen 1T en 4T 
#define NewKAKU_4T                  1100        // us
#define NewKAKU_8T                  2200        // us, Tijd van de space na de startbit

boolean Plugin_004(byte function, struct NodoEventStruct *event, char *string)
  {
  boolean success=false;
  
  switch(function)
    {
    #ifdef PLUGIN_004_CORE
    case PLUGIN_RAWSIGNAL_IN:
      break;
      
    case PLUGIN_COMMAND:
      {
      unsigned long bitstream=0L;
      byte i=1;
      byte x; /// aantal posities voor pulsen/spaces in RawSignal
        
      // bouw het KAKU adres op. Er zijn twee mogelijkheden: Een adres door de gebruiker opgegeven binnen het bereik van 0..255 of een lange hex-waarde
      if(event->Par2<=255)
        bitstream=1|(event->Par2<<6);  // Door gebruiker gekozen adres uit de Nodo_code toevoegen aan adres deel van de KAKU code. 
      else
        bitstream=event->Par2 & 0xFFFFFFCF; // adres geheel over nemen behalve de twee bits 5 en 6 die het schakel commando bevatten.
    
      event->Port=VALUE_ALL; // Signaal mag naar alle door de gebruiker met [Output] ingestelde poorten worden verzonden.
      RawSignal.Repeats=7;   // Aantal herhalingen van het signaal.
      RawSignal.Delay=20; // Tussen iedere pulsenreeks enige tijd rust.
      RawSignal.Multiply=25;

      if(event->Par1==VALUE_ON || event->Par1==VALUE_OFF)
        {
        bitstream|=(event->Par1==VALUE_ON)<<4; // bit-5 is het on/off commando in KAKU signaal
        x=130;// verzend startbit + 32-bits = 130
        }
      else
        x=146;// verzend startbit + 32-bits = 130 + 4dimbits = 146
     
      // bitstream bevat nu de KAKU-bits die verzonden moeten worden.
    
      for(i=3;i<=x;i++)RawSignal.Pulses[i]=NewKAKU_1T/RawSignal.Multiply;  // De meeste tijden in signaal zijn T. Vul alle pulstijden met deze waarde. Later worden de 4T waarden op hun plek gezet
      
      i=1;
      RawSignal.Pulses[i++]=NewKAKU_1T/RawSignal.Multiply; //pulse van de startbit
      RawSignal.Pulses[i++]=NewKAKU_8T/RawSignal.Multiply; //space na de startbit
      
      byte y=31; // bit uit de bitstream
      while(i<x)
        {
        if((bitstream>>(y--))&1)
          RawSignal.Pulses[i+1]=NewKAKU_4T/RawSignal.Multiply;     // Bit=1; // T,4T,T,T
        else
          RawSignal.Pulses[i+3]=NewKAKU_4T/RawSignal.Multiply;     // Bit=0; // T,T,T,4T
    
        if(x==146)  // als het een dim opdracht betreft
          {
          if(i==111) // Plaats van de Commando-bit uit KAKU 
            RawSignal.Pulses[i+3]=NewKAKU_1T/RawSignal.Multiply;  // moet een T,T,T,T zijn bij een dim commando.
          if(i==127)  // als alle pulsen van de 32-bits weggeschreven zijn
            {
            bitstream=(unsigned long)event->Par1-1; //  nog vier extra dim-bits om te verzenden. -1 omdat dim niveau voor gebruiker begint bij 1
            y=3;
            }
          }
        i+=4;
        }
      RawSignal.Pulses[i++]=NewKAKU_1T/RawSignal.Multiply; //pulse van de stopbit
      RawSignal.Pulses[i]=0; //space van de stopbit
      RawSignal.Number=i; // aantal bits*2 die zich in het opgebouwde RawSignal bevinden
      SendEvent(event,true,true,Settings.WaitFree==VALUE_ON);
      success=true;
      break;
      }
    #endif // CORE
      
    #if NODO_MEGA
    case PLUGIN_MMI_IN:
      {
      char* str=(char*)malloc(40);
      string[25]=0; // kap voor de zekerheid de string af.
    
      if(GetArgv(string,str,1))
        {
        if(strcasecmp(str,PLUGIN_NAME)==0)
          {
          if(GetArgv(string,str,2))
            {
            event->Par2=str2int(str);    
            if(GetArgv(string,str,3))
              {
              // Vul Par1 met het KAKU commando. Dit kan zijn: VALUE_ON, VALUE_OFF, 1..16. Andere waarden zijn ongeldig.
              
              // haal uit de tweede parameter een 'On' of een 'Off'.
              if(event->Par1=str2cmd(str))
                success=true;
                
              // als dit niet is gelukt, dan uit de tweede parameter de dimwaarde halen.
              else
                {
                event->Par1=str2int(str);             // zet string om in integer waarde
                if(event->Par1<=16) // geldig dim bereik 1..16 ?
                  {
                  success=true;
                  if(event->Par1==0)
                    event->Par1=VALUE_OFF;
                  }
                }
              event->Type = NODO_TYPE_PLUGIN_COMMAND; 
              event->Command = 4; // Plugin nummer  
              }
            }
          }
        }
      free(str);
      break;
      }

    case PLUGIN_MMI_OUT:
      {
      strcpy(string,PLUGIN_NAME);            // Eerste argument=het commando deel
      strcat(string," ");
    
      // In Par3 twee mogelijkheden: Het bevat een door gebruiker ingegeven adres 0..255 of een volledig NewKAKU adres.
      if(event->Par2>=0x0ff)
        strcat(string,int2strhex(event->Par2)); 
      else
        strcat(string,int2str(event->Par2)); 
      
      strcat(string,",");
      
      if(event->Par1==VALUE_ON)
        strcat(string,"On");  
      else if(event->Par1==VALUE_OFF)
        strcat(string,"Off");
      else
        strcat(string,int2str(event->Par1));

      break;
      }
    #endif //MMO
    }      
  return success;
  }

