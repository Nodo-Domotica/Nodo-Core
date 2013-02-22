#ifdef PROTOCOL_2

//#######################################################################################################
//##################################### Protocol_2: NewKAKU      ###########################################
//#######################################################################################################

/*********************************************************************************************\
 * Dit protocol zorgt voor ontvangst en aansturing van Klik-Aan-Klik-Uit zenders/ontvangers
 * die werken volgens de automatische codering (Ontvangers met leer-knop) Dit protocol is 
 * eveneens bekend onder de naam HomeEasy. Het protocol ondersteunt eveneens dim funktionaliteit.
 * 
 * Auteur             : Nodo-team (P.K.Tonkes) www.nodo-domotca.nl
 * Support            : www.nodo-domotica.nl
 * Datum              : Jan.2013
 * Versie             : 1.0
 * Nodo productnummer : n.v.t. meegeleverd met Nodo code.
 * Compatibiliteit    : Vanaf Nodo build nummer 500
 * Compiled size      : 1100 bytes voor een Mega, 725 voor een Small
 * Vereiste library   : - geen -
 * Externe funkties   : strcat(),  strcpy(),  cmd2str(),  tolower(),  GetArgv(),  str2cmd()
 *
 ***********************************************************************************************
 * Toelichting protocol:
 *
 * Pulse (T) is 275us PDM
 * 0 = T,T,T,4T, 1 = T,4T,T,T, dim = T,T,T,T op bit 27
 *
 * NewKAKU ondersteund:
 *   on/off       ---- 000x Off/On
 *   all on/off   ---- 001x AllOff/AllOn
 *   dim absolute xxxx 0110 Dim16        // dim op bit 27 + 4 extra bits voor dim level
 *
 *  NewKAKU (org.)        = (First sent) AAAAAAAAAAAAAAAAAAAAAAAAAACCUUUU(LLLL) -> A=KAKU_adres, C=commando, U=KAKU-Unit, L=extra dimlevel bits (optioneel)
 *
 * Interne gebruik van de parameters in het Nodo Event:
 * 
 * Cmd  : Hier zit het commando SendNewKAKU of het event NewKAKU in. Deze gebruiken we verder niet.
 * Par1 : Commando VALUE_ON, VALUE_OFF of dim niveau [1..15]
 * Par2 : <niet gebruikt>
 * Par3 : Adres
 *
 \*********************************************************************************************/
 

#define NewKAKU_RawSignalLength      132
#define NewKAKUdim_RawSignalLength   148
#define NewKAKU_1T                   275        // us
#define NewKAKU_mT                   500        // us, midden tussen 1T en 4T 
#define NewKAKU_4T                  1100        // us
#define NewKAKU_8T                  2200        // us, Tijd van de space na de startbit

 /*******************************************************************************************************\
 * Vult aan de hand van een Nodo event de RawSignal buffer zodat deze kan worden verzonden via RF of IR.
 \*******************************************************************************************************/
void Protocol_2_EventToRawsignal(struct NodoEventStruct *Event)
  {
  unsigned long bitstream=0L;
  byte Bit, i=1;
  byte x; /// aantal posities voor pulsen/spaces in RawSignal

  // bouw het KAKU adres op. Er zijn twee mogelijkheden: Een adres door de gebruiker opgegeven binnen het bereik van 0..255 of een lange hex-waarde
  if(Event->Par2<=255)
    bitstream=1|(Event->Par2<<6);  // Door gebruiker gekozen adres uit de Nodo_code toevoegen aan adres deel van de KAKU code. 
  else
    bitstream=Event->Par2 & 0xFFFFFFCF; // adres geheel over nemen behalve de twee bits 5 en 6 die het schakel commando bevatten.

  Event->Port=VALUE_ALL; // Signaal mag naar alle door de gebruiker met [Output] ingestelde poorten worden verzonden.
  RawSignal.Repeats=5;   // vijf herhalingen.

  if(Event->Par1==VALUE_ON || Event->Par1==VALUE_OFF)
    {
    bitstream|=(Event->Par1==VALUE_ON)<<4; // bit-5 is het on/off commando in KAKU signaal
    x=130;// verzend startbit + 32-bits = 130
    }
  else
    x=146;// verzend startbit + 32-bits = 130 + 4dimbits = 146
 
  // bitstream bevat nu de KAKU-bits die verzonden moeten worden.

  for(i=3;i<=x;i++)RawSignal.Pulses[i]=NewKAKU_1T;  // De meeste tijden in signaal zijn T. Vul alle pulstijden met deze waarde. Later worden de 4T waarden op hun plek gezet
  
  i=1;
  RawSignal.Pulses[i++]=NewKAKU_1T; //pulse van de startbit
  RawSignal.Pulses[i++]=NewKAKU_8T; //space na de startbit
  
  byte y=31; // bit uit de bitstream
  while(i<x)
    {
    if((bitstream>>(y--))&1)
      RawSignal.Pulses[i+1]=NewKAKU_4T;     // Bit=1; // T,4T,T,T
    else
      RawSignal.Pulses[i+3]=NewKAKU_4T;     // Bit=0; // T,T,T,4T

    if(x==146)  // als het een dim opdracht betreft
      {
      if(i==111) // Plaats van de Commando-bit uit KAKU 
        RawSignal.Pulses[i+3]=NewKAKU_1T;  // moet een T,T,T,T zijn bij een dim commando.
      if(i==127)  // als alle pulsen van de 32-bits weggeschreven zijn
        {
        bitstream=(unsigned long)Event->Par1-1; //  nog vier extra dim-bits om te verzenden. -1 omdat dim niveau voor gebruiker begint bij 1
        y=3;
        }
      }
    i+=4;
    }
  RawSignal.Pulses[i++]=NewKAKU_1T; //pulse van de stopbit
  RawSignal.Pulses[i]=NewKAKU_1T*32; //space van de stopbit tevens pause tussen signalen
  RawSignal.Number=i; // aantal bits*2 die zich in het opgebouwde RawSignal bevinden
  }

/*********************************************************************************************\
* Deze routine berekent de uit een RawSignal een Nodo event.
* Geeft een false retour als geen geldig KAKU signaal uit het signaal te destilleren
\*********************************************************************************************/
boolean Protocol_2_RawsignalToEvent(struct NodoEventStruct *Event)
  {
  unsigned long bitstream=0L;
  boolean Bit;
  int i;
  
  Event->Par1=0;
  
  // nieuwe KAKU bestaat altijd uit start bit + 32 bits + evt 4 dim bits. Ongelijk, dan geen NewKAKU
  if (RawSignal.Number!=NewKAKU_RawSignalLength && (RawSignal.Number!=NewKAKUdim_RawSignalLength))return false;

  // RawSignal.Number bevat aantal pulsen * 2  => negeren
  // RawSignal.Pulses[1] bevat startbit met tijdsduur van 1T => negeren
  // RawSignal.Pulses[2] bevat lange space na startbit met tijdsduur van 8T => negeren
  i=3; // RawSignal.Pulses[3] is de eerste van een T,xT,T,xT combinatie
  
  do 
    {
    if     (RawSignal.Pulses[i]<NewKAKU_mT && RawSignal.Pulses[i+1]<NewKAKU_mT && RawSignal.Pulses[i+2]<NewKAKU_mT && RawSignal.Pulses[i+3]>NewKAKU_mT)Bit=0; // T,T,T,4T
    else if(RawSignal.Pulses[i]<NewKAKU_mT && RawSignal.Pulses[i+1]>NewKAKU_mT && RawSignal.Pulses[i+2]<NewKAKU_mT && RawSignal.Pulses[i+3]<NewKAKU_mT)Bit=1; // T,4T,T,T
    else if(RawSignal.Pulses[i]<NewKAKU_mT && RawSignal.Pulses[i+1]<NewKAKU_mT && RawSignal.Pulses[i+2]<NewKAKU_mT && RawSignal.Pulses[i+3]<NewKAKU_mT)       // T,T,T,T Deze hoort te zitten op i=111 want: 27e NewKAKU bit maal 4 plus 2 posities voor startbit
      {
      if(RawSignal.Number!=NewKAKUdim_RawSignalLength) // als de dim-bits er niet zijn
        return false;
      }
    else
      return false; // andere mogelijkheden zijn niet geldig in NewKAKU signaal.  
      
    if(i<130) // alle bits die tot de 32-bit pulstrein behoren 32bits * 4posities per bit + pulse/space voor startbit
      bitstream=(bitstream<<1) | Bit;
    else // de resterende vier bits die tot het dimlevel behoren 
      Event->Par1=(Event->Par1<<1) | Bit;
 
    i+=4;// volgende pulsenquartet
    }while(i<RawSignal.Number-2); //-2 omdat de space/pulse van de stopbit geen deel meer van signaal uit maakt.


  // Adres deel:
  if(bitstream>0xffff)                         // Is het signaal van een originele KAKU zender afkomstig, of van een Nodo ingegeven door de gebruiker ?
  #if NODO_30_COMPATIBLE
    Event->Par2=bitstream &0x0FFFFFCF;         // Op hoogste nibble zat vroeger het signaaltype. 
  #else
    Event->Par2=bitstream &0xFFFFFFCF;         // dan hele adres incl. unitnummer overnemen. Alleen de twee commando-bits worden er uit gefilterd
  #endif
  
  else                                         // Het is van een andere Nodo afkomstig. 
    Event->Par2=(bitstream>>6)&0xff;           // Neem dan alleen 8bit v/h adresdeel van KAKU signaal over
    
  // Commando en Dim deel
  if(i>140)
    Event->Par1++;                             // Dim level. +1 omdat gebruiker dim level begint bij één.
  else
    Event->Par1=((bitstream>>4)&0x01)?VALUE_ON:VALUE_OFF; // On/Off bit omzetten naar een Nodo waarde. 

  Event->Command       = CMD_PROTOCOL_2;        // Command deel t.b.v. weergave van de string "NewKAKU"
  Event->SourceUnit    = 0;                     // Komt niet van een Nodo unit af, dus unit op nul zetten
  Event->Flags         = TRANSMISSION_REPEATING;// het is een herhalend signaal. Bij ontvangst herhalingen onderdrukken.

  return true;
  }

#ifdef NODO_MEGA

 /*******************************************************************************************************\
 * Converteert een Nodo event naar een string volgens de NewKAKU adressering 'NewKaku 0x123456A1,On'
 * In plaats van On/Off kan ook het dim niveau 1..16 worden weergegeven.
 \*******************************************************************************************************/
void Protocol_2_EventToString(struct NodoEventStruct *Event, char *OutputString)
  {
  strcpy(OutputString,cmd2str(Event->Command));
  strcat(OutputString," ");

  // In Par3 twee mogelijkheden: Het bevat een door gebruiker ingegeven adres 0..255 of een volledig NewKAKU adres.
  if(Event->Par2>=0x0ff)
    strcat(OutputString,int2strhex(Event->Par2)); 
  else
    strcat(OutputString,int2str(Event->Par2)); 
  
  strcat(OutputString,",");
  
  if(Event->Par1==VALUE_ON)
    strcat(OutputString,"On");  
  else if(Event->Par1==VALUE_OFF)
    strcat(OutputString,"Off");
  else
    strcat(OutputString,int2str(Event->Par1));
  }


 /*******************************************************************************************************\
 * Converteert een string volgens de KAKU adressering 'NewKaku 0x123456,On' naar een Nodo Event.
 * Als er geen geldige parameters worden opgegeven dan keert de funktie terug met een false.
 \*******************************************************************************************************/
boolean Protocol_2_StringToEvent(char *InputString, struct NodoEventStruct *Event)
  {
  char* str=(char*)malloc(40);
  boolean ok=false;
  
  *(InputString+40)=0; // kap voor de zekerheid de string af.

  if(GetArgv(InputString,str,2))
    {
    Event->Par2=str2int(str);    
    if(GetArgv(InputString,str,3))
      {
      // Vul Par1 met het KAKU commando. Dit kan zijn: VALUE_ON, VALUE_OFF, 1..16. Andere waarden zijn ongeldig.
      
      // haal uit de tweede parameter een 'On' of een 'Off'.
      if(Event->Par1=str2cmd(str))
        ok=true;
        
      // als dit niet is gelukt, dan uit de tweede parameter de dimwaarde halen.
      else
        {
        Event->Par1=str2int(str);             // zet string om in integer waarde
        if(Event->Par1>=1 && Event->Par1<=16) // geldig dim bereik 1..16 ?
           ok=true;
        }
      }
    }

  free(str);
  return ok;
  }
  
 
#endif // mega

#endif // protocol
