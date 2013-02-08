/*****************************************************************************************************************\
Een RF/IR signaal kan worden ontvangen en verzonden. Er is voorzien in vier verschillende
protocollen, PROTOCOL_1...PROTOCOL_4. Neem in tabblad 'Nodo.ino' een regel op met: 

#define PROTOCOL_n "MijnProtocolNaam"

Voor verzending wordt het commando gebruikt "SendMijnProtocolNaam" en bij ontvangst leidt een 
binnenkomend signaal tot een event "MijnProtocolNaam".

Een protocol heeft de volgende vier funkties nodig:

Protocol_n_RawsignalToEvent() : Ontvangen RawSignal omzetten naar een Nodo-Event
Protocol_n_EventToRawsignal() : Nodo-Event omzetten in een RawSignal
Protocol_n_StrindToEvent()    : MMI, Door de gebruiker ingevoerde string om te zetten in een Nodo-Event (Alleen voor de Mega).
Protocol_n_EventToString()    : MMI, Nodo-event omzetten in een string voor gebruikersweergave (Alleen voor de Mega).

Tips en aandachtspunten voor programmeren van een protocol:

-  Geheugen is beperkt. Programmeer compact en benut iedere byte RAM en PROGMEM.
-  Bouw geen lange wachtlussen in de code. Dit geldt met name voor Protocol_1_RawsignalToEvent();
   Deze funktie bevindt zich ik een tijdkritische loop. Detecteer zo snel mogelijk of het ontvangen signaal
   ook bij dit protocol hoort. 
-  De array RawSignal.Pulses[] bevat alle Mark en Space tijden in microseconden. Te beginnen vanaf element [1].
   Deze array is reeds gevuld bij aankomst in Protocol_n_RawsignalToEvent().
-  RawSignal.Pulses bevat het aantal posities die een mark/space bevatten. Let op dat de waarde RAW_BUFFER_SIZE
   nooit overschreden wordt. Anders gegarandeerd vastlopers! Positie [1] bevat de 'mark' van de startbit. 
-  De struct NodoEventStruct bevat alle informatie die nodig is voor verwerking en weergave van het event
   dat is ontvangen of moet worden weergegeven. 
-  byte Par1, byte Par2 en unsigned int Par3 van de NodoEventStruct kunnen worden gebruikt voor de metagegevens 
   van het signaal.
-  Let in een multi nodo omgeving op dat de Nodo's dezelfde protocollen gebruiken behorende bij het protocolnummer.
-  Er is voorzien in vier verschillende protocollen, het is verantwoordelijkheid van de programmeur om te bewaken
   of er voldoende geheugen over blijft voor stabiele werking van de Nodo.
-  Om uitwisselbaar te blijven met andere Nodo versies en Nodo gebruikers, geen aanpassingen aan de Code maken
   anders dan voorgeschreven.
-  Maak geen gebruik van interrupt driven routines, dit verstoort (mogelijk) de werking van de I2C, Serial en ethernet
   communicatie.
-  Maak geen gebruik van Globals en pas geen waarden van globals uit de Nodo code aan.
-  Besteed uitgebreid aandacht aan de documentatie van het protocol. Indien mogelijk verwijzen naar originele
   specificatie.

\*****************************************************************************************************************/

#ifdef PROTOCOL_1  

//#######################################################################################################
//##################################### Protocol_1: KAKU      ###########################################
//#######################################################################################################

/*********************************************************************************************\
 * Dit protocol zorgt voor ontvangst en aansturing van Klik-Aan-Klik-Uit zenders/ontvangers
 * die werken volgens de handmatige codering (draaiwiel met adres- en huiscodering). Dit protocol
 * kan eveneens worden ontvangen door Klik-Aan-Klik-Uit apparaten die werken met automatische
 * code programmering. Dimmen wordt niet ondersteund. Coding/Encoding principe is in de markt bekend
 * onder de namen: Princeton PT2262 / MOSDESIGN M3EB / Domia Lite / Klik-Aan-Klik-Uit / Intertechno
 * 
 * Auteur             : Nodo-team (P.K.Tonkes) www.nodo-domotca.nl
 * Support            : www.nodo-domotica.nl
 * Datum              : Jan.2013
 * Versie             : 1.0
 * Nodo productnummer : n.v.t. meegeleverd met Nodo code.
 * Compatibiliteit    : Vanaf Nodo build nummer 500
 * Compiled size      : 1100 bytes voor een Mega, 540 bytes voor een Small
 * Vereiste library   : - geen -
 * Externe funkties   : strcat(),  strcpy(),  cmd2str(),  tolower(),  GetArgv(),  str2cmd()
 *
 ***********************************************************************************************
 * Het signaal bestaat drie soorten reeksen van vier pulsen, te weten: 
 * 0 = T,3T,T,3T, 1 = T,3T,3T,T, short 0 = T,3T,T,T Hierbij is iedere pulse (T) 350us PWDM
 *
 * KAKU ondersteund:
 *        on/off, waarbij de pulsreeks er als volgt uit ziet: 000x en x staat voor Off / On
 *    all on/off, waarbij de pulsreeks er als volgt uit ziet: 001x en x staat voor All Off / All On 
 *
 * Interne gebruik van de parameters in het Nodo Event:
 * 
 * Cmd  : Hier zit het commando SendKAKU of het event KAKU in. Deze gebruiken we verder niet.
 * Par1 : Groep commando (true of false)
 * Par2 : Hier zit het KAKU commando (On/Off) in. True of false
 * Par3 : Adres en Home code. Acht bits AAAAHHHH
 *
 \*********************************************************************************************/
 
#define KAKU_CodeLength    12  // aantal data bits
#define KAKU_T            350  // us


//??? hoe omgaan met waitfreerf;

 /*******************************************************************************************************\
 * Vult aan de hand van een Nodo event de RawSignal buffer zodat deze kan worden verzonden via RF of IR.
 \*******************************************************************************************************/
void Protocol_1_EventToRawsignal(struct NodoEventStruct *Event)
  {
  unsigned long Bitstream;

  RawSignal.Repeats=5;                   // KAKU heeft vijf herhalingen nodig om te schakelen.
  RawSignal.Number=KAKU_CodeLength*4+2;
  Event->Port=VALUE_ALL;                 // Signaal mag naar alle door de gebruiker met [Output] ingestelde poorten worden verzonden.
 
  Bitstream = Event->Par2 | (0x600 | ((Event->Par1&1/*Commando*/) << 11)); // Stel een bitstream samen
  
  // loop de 12-bits langs en vertaal naar pulse/space signalen.  
  for (byte i=0; i<KAKU_CodeLength; i++)
    {
    RawSignal.Pulses[4*i+1]=KAKU_T;
    RawSignal.Pulses[4*i+2]=KAKU_T*3;

    if (((Event->Par1>>1)&1) /* Groep */ && i>=4 && i<8) 
      {
      RawSignal.Pulses[4*i+3]=KAKU_T;
      RawSignal.Pulses[4*i+4]=KAKU_T;
      } // short 0
    else
      {
      if((Bitstream>>i)&1)// 1
        {
        RawSignal.Pulses[4*i+3]=KAKU_T*3;
        RawSignal.Pulses[4*i+4]=KAKU_T;
        }
      else //0
        {
        RawSignal.Pulses[4*i+3]=KAKU_T;
        RawSignal.Pulses[4*i+4]=KAKU_T*3;          
        }          
      }
    }
  RawSignal.Pulses[(KAKU_CodeLength*4)+1] = KAKU_T;
  RawSignal.Pulses[(KAKU_CodeLength*4)+2] = KAKU_T*32; // pauze tussen de pulsreeksen
  }

/*********************************************************************************************\
* Deze routine berekent de uit een RawSignal een Nodo event.
* Geeft een false retour als geen geldig KAKU signaal uit het signaal te destilleren
\*********************************************************************************************/
boolean Protocol_1_RawsignalToEvent(struct NodoEventStruct *Event)
  {
  int i,j;
  unsigned long bitstream=0;
  
  // conventionele KAKU bestaat altijd uit 12 data bits plus stop. Ongelijk, dan geen KAKU!
  if (RawSignal.Number!=(KAKU_CodeLength*4)+2)return false;
  RawSignal.Repeats=5;

  for (i=0; i<KAKU_CodeLength; i++)
    {
    j=KAKU_T*2;        
    if      (RawSignal.Pulses[4*i+1]<j && RawSignal.Pulses[4*i+2]>j && RawSignal.Pulses[4*i+3]<j && RawSignal.Pulses[4*i+4]>j) {bitstream=(bitstream >> 1);} // 0
    else if (RawSignal.Pulses[4*i+1]<j && RawSignal.Pulses[4*i+2]>j && RawSignal.Pulses[4*i+3]>j && RawSignal.Pulses[4*i+4]<j) {bitstream=(bitstream >> 1 | (1 << (KAKU_CodeLength-1))); }// 1
    else if (RawSignal.Pulses[4*i+1]<j && RawSignal.Pulses[4*i+2]>j && RawSignal.Pulses[4*i+3]<j && RawSignal.Pulses[4*i+4]<j) {bitstream=(bitstream >> 1); Event->Par1=2;} // Short 0, Groep commando op 2e bit.
    else {return false;} // foutief signaal
    }
 
  if ((bitstream&0x600)!=0x600)return false; // twee vaste bits van KAKU gebruiken als checksum

  Event->Par2          = bitstream & 0xFF;
  Event->Par1         |= (bitstream >> 11) & 0x01;
  Event->Command       = CMD_PROTOCOL_1;
  Event->SourceUnit    = 0; // Komt niet van een Nodo unit af.
  return true;
  }

#ifdef NODO_MEGA

 /*******************************************************************************************************\
 * Converteert een Nodo event naar een string volgens de KAKU adressering 'Kaku A1,On'
 \*******************************************************************************************************/
void Protocol_1_EventToString(struct NodoEventStruct *Event, char *OutputString)
  {
  strcpy(OutputString,cmd2str(Event->Command));            // Commando / event 
  strcat(OutputString," ");                

  char t[3];                                               // Mini string
  t[0]='A' + (Event->Par2 & 0x0f);                         // Home A..P
  t[1]= 0;                                                 // en de mini-string afsluiten.
  strcat(OutputString,t);

  if(Event->Par1&2) // als 2e bit in commando staat, dan groep.
    strcat(OutputString,int2str(0));                       // Als Groep, dan adres 0       
  else
    strcat(OutputString,int2str(((Event->Par2 & 0xf0)>>4)+1)); // Anders adres toevoegen             

  if(Event->Par1&1) // Het 1e bit is get aan/uit commando
    strcat(OutputString,",On");  
  else
    strcat(OutputString,",Off");  
  }

 /*******************************************************************************************************\
 * Converteert een string volgens de KAKU adressering 'Kaku A1,On' naar een Nodo Event.
 * Als er geen geldige parameters worden opgegeven dan keert de funktie terug met een false.
 * Eventueel mag in plaats van de HomeAdres notatie 'A1' een decimale waarde van 0..255
 * worden opgegeven.
 \*******************************************************************************************************/
boolean Protocol_1_StringToEvent(char *InputString, struct NodoEventStruct *Event)
  {
  byte grp=0,c;
  byte x=0;       // teller die wijst naar het het te behandelen teken
  byte Home=0;    // KAKU home A..P
  byte Address=0; // KAKU Address 1..16
  byte y=false;   // Notatiewijze
  boolean error=false;

  // Reserveer een kleine string en kap voor de zekerheid de inputstring af om te voorkomen dat er
  // buiten gereserveerde geheugen gewerkt wordt.
  char *TempStr=(char*)malloc(26);
  InputString[25]=0;
  
  // eerste parameter bevat adres volgens codering A0..P16
  if(GetArgv(InputString,TempStr,2)) 
    {
    while((c=tolower(TempStr[x++]))!=0)
      {
      if(c>='0' && c<='9'){Address=Address*10;Address=Address+c-'0';}
      if(c>='a' && c<='p'){Home=c-'a';} // KAKU home A is intern 0
      }
  
    if(Address==0)
      {// groep commando is opgegeven: 0=alle adressen
      Event->Par1=2; // 2e bit setten voor groep.
      Event->Par2=Home;
      }
    else
      Event->Par2= Home | ((Address-1)<<4);        
    }
  else
    error=true;

  // Het door de gebruiker ingegeven tweede parameter bevat het on/off commando
  if(GetArgv(InputString,TempStr,3))
    Event->Par1 |= str2cmd(TempStr)==VALUE_ON; 
  else
    error=true;

  free(TempStr);
  return !error; // Keer terug met een true als het gelukt is.
  }

#endif // NODO_MEGA
#endif // PROTOCOL_1


