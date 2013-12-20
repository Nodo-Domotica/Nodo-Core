//#######################################################################################################
//#################################### Plugin-01: Kaku   ################################################
//#######################################################################################################

/*********************************************************************************************\
 * Dit protocol zorgt voor ontvangst van Klik-Aan-Klik-Uit zenders (Handzender, PIR, etc.)
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
 * Compatibiliteit    : Vanaf Nodo build nummer 507
 * Compiled size      : 796 bytes voor een Mega
 * Vereiste library   : - geen -
 * Externe funkties   : strcat(),  strcpy(),  cmd2str(),  strcasecmp(),  GetArgv()
 *
 ***********************************************************************************************
 * Het signaal bestaat drie soorten reeksen van vier pulsen, te weten: 
 * 0 = T,3T,T,3T, 1 = T,3T,3T,T, short 0 = T,3T,T,T Hierbij is iedere pulse (T) 350us PWDM
 *
 * KAKU ondersteund:
 *        on/off, waarbij de pulsreeks er als volgt uit ziet: 000x en x staat voor Off / On
 *    all on/off, waarbij de pulsreeks er als volgt uit ziet: 001x en x staat voor All Off / All On 
 *
 * Interne gebruik van de parameters in het Nodo event:
 * 
 * Cmd  : Hier zit het commando SendKAKU of het event KAKU in. Deze gebruiken we verder niet.
 * Par1 : Groep commando (true of false)
 * Par2 : Hier zit het KAKU commando (On/Off) in. True of false
 * Par3 : Adres en Home code. Acht bits AAAAHHHH
 *
 \*********************************************************************************************/

#define KAKU_CodeLength    12  // aantal data bits
#define KAKU_T            350  // us

boolean Plugin_001(byte function, struct NodoEventStruct *event, char *string)
  {
  boolean success=false;
  
  switch(function)
    {
    #ifdef PLUGIN_001_CORE

    case PLUGIN_RAWSIGNAL_IN:
      {
      int i,j;
      unsigned long bitstream=0;
      
      // conventionele KAKU bestaat altijd uit 12 data bits plus stop. Ongelijk, dan geen KAKU!
      if (RawSignal.Number!=(KAKU_CodeLength*4)+2)return false;
    
      for (i=0; i<KAKU_CodeLength; i++)
        {
        j=(KAKU_T*2)/RawSignal.Multiply; // Deelfactor zit is eerste element van de array.   
        
        if      (RawSignal.Pulses[4*i+1]<j && RawSignal.Pulses[4*i+2]>j && RawSignal.Pulses[4*i+3]<j && RawSignal.Pulses[4*i+4]>j) {bitstream=(bitstream >> 1);} // 0
        else if (RawSignal.Pulses[4*i+1]<j && RawSignal.Pulses[4*i+2]>j && RawSignal.Pulses[4*i+3]>j && RawSignal.Pulses[4*i+4]<j) {bitstream=(bitstream >> 1 | (1 << (KAKU_CodeLength-1))); }// 1
        else if (RawSignal.Pulses[4*i+1]<j && RawSignal.Pulses[4*i+2]>j && RawSignal.Pulses[4*i+3]<j && RawSignal.Pulses[4*i+4]<j) {bitstream=(bitstream >> 1); event->Par1=2;} // Short 0, Groep commando op 2e bit.
        else {return false;} // foutief signaal
        }
     
      // twee vaste bits van KAKU gebruiken als checksum
      if ((bitstream&0x600)==0x600)
        {    
        // Alles is in orde, bouw event op   
        RawSignal.Repeats    = true; // het is een herhalend signaal. Bij ontvangst herhalingen onderdrukken.
        event->Par2          = bitstream & 0xFF;
        event->Par1         |= (bitstream >> 11) & 0x01;
        event->SourceUnit    = 0; // Komt niet van een Nodo unit af.
        event->Type          = NODO_TYPE_PLUGIN_EVENT;
        event->Command       = 1; // nummer van dit device
        success=true;
        }
      break;
      }
      
    #endif //PLUGIN_CORE_001
      
    #if NODO_MEGA
    case PLUGIN_MMI_IN:
      {
      // Reserveer een kleine string en kap voor de zekerheid de inputstring af om te voorkomen dat er
      // buiten gereserveerde geheugen gewerkt wordt.
      char *TempStr=(char*)malloc(INPUT_COMMAND_SIZE);

      // Hier aangekomen bevat string het volledige commando. Test als eerste of het opgegeven commando overeen komt met "Kaku"
      // Dit is het eerste argument in het commando.
      if(GetArgv(string,TempStr,1))
        {
        if(strcasecmp(TempStr,"Kaku")==0)
          {
          byte c;
          byte x=0;       // teller die wijst naar het het te behandelen teken
          byte Home=0;    // KAKU home A..P
          byte Address=0; // KAKU Address 1..16
        
          // eerste parameter bevat adres volgens codering A0..P16
          if(GetArgv(string,TempStr,2)) 
            {
            while((c=tolower(TempStr[x++]))!=0)
              {
              if(c>='0' && c<='9'){Address=Address*10;Address=Address+c-'0';}
              if(c>='a' && c<='p'){Home=c-'a';} // KAKU home A is intern 0
              }
          
            if(Address==0)
              {// groep commando is opgegeven: 0=alle adressen
              event->Par1=2; // 2e bit setten voor groep.
              event->Par2=Home;
              }
            else
              event->Par2= Home | ((Address-1)<<4);        
   
            // Het door de gebruiker ingegeven tweede parameter bevat het on/off commando
            if(GetArgv(string,TempStr,3))
              {
              event->Par1 |= str2cmd(TempStr)==VALUE_ON; 
              event->Type  = NODO_TYPE_PLUGIN_EVENT;
              event->Command = 1; // Plugin nummer  
              success=true;
              }
            }
          }
        }
      free(TempStr);
      break;
      }

    case PLUGIN_MMI_OUT:
      {
      strcpy(string,"Kaku");            // Commando / event 
      strcat(string," ");                
    
      char t[3];                                               // Mini string
      t[0]='A' + (event->Par2 & 0x0f);                         // Home A..P
      t[1]= 0;                                                 // en de mini-string afsluiten.
      strcat(string,t);
    
      if(event->Par1&2) // als 2e bit in commando staat, dan groep.
        strcat(string,int2str(0));                       // Als Groep, dan adres 0       
      else
        strcat(string,int2str(((event->Par2 & 0xf0)>>4)+1)); // Anders adres toevoegen             
    
      if(event->Par1&1) // Het 1e bit is get aan/uit commando
        strcat(string,",On");  
      else
        strcat(string,",Off");  

      break;
      }
    #endif //MMI
    }      
  return success;
  }
