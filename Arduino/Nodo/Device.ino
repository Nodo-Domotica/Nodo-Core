
/*********************************************************************************************\

 De Nodo biedt ruimte voor ondersteuning van meerdere devices en protocollen. De gebruiker kan aangeven welke 
 devices mee gecompileerd moeten worden. Dit vindt plaats door in het tabblad 'Nodo' de
 definitie op te nemen die er voor zorgen dat de juiste code wordt gecompileerd. 

 De volgende devices kunnen worden gekozen:

 #define DEVICE_01  // Kaku              : Klik-Aan-Klik-Uit / HomeEasy protocol ontvangst
 #define DEVICE_02  // SendKaku          : Klik-Aan-Klik-Uit / HomeEasy protocol verzenden 
 #define DEVICE_03  // NewKAKU           : Klik-Aan-Klik-Uit ontvangst van signalen met automatische codering. Tevens bekend als Intertechno.
 #define DEVICE_04  // SendNewKAKU       : Klik-Aan-Klik-Uit ontvangst van signalen met automatische codering. Tevens bekend als Intertechno. 
 #define DEVICE_05  // TempSensor        : Temperatuursensor Dallas DS18B20. (Let op; -B- variant, dus niet DS1820)
 #define DEVICE_13  // UserEventOld      : UserEvents van de oude Nodo. t.b.v. compatibiliteit reeds geprogrammeerde universele afstandsbedieningen.
 #define DEVICE_14  // MonitorAnalog     : Laat continue analoge metingen zien van alle Wired-In poorten. 
 #define DEVICE_15  // RawSignalAnalyze  : Geeft bij een binnenkomend signaal informatie over de pulsen weer.
 #define DEVICE_99  // UserDevice        : Device voor eigen toepassing door gebruiker te bouwen.
 
 \***********************************************************************************************/





//#######################################################################################################
//#################################### Device-01: Kaku   ################################################
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

#ifdef DEVICE_01
#define KAKU_CodeLength    12  // aantal data bits
#define KAKU_T            350  // us

boolean Device_01(byte function, struct NodoEventStruct *event, char *string)
  {
  boolean success=false;
  
  switch(function)
    {
    #ifdef DEVICE_CORE_01
    case DEVICE_RAWSIGNAL_IN:
      {
      int i,j;
      unsigned long bitstream=0;
      
      // conventionele KAKU bestaat altijd uit 12 data bits plus stop. Ongelijk, dan geen KAKU!
      if (RawSignal.Number!=(KAKU_CodeLength*4)+2)return false;
      RawSignal.Repeats=5;
    
      for (i=0; i<KAKU_CodeLength; i++)
        {
        j=(KAKU_T*2)/RawSignal.Pulses[0]; // Deelfactor zit is eerste element van de array.   
        
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
        event->Command       = CMD_DEVICE_FIRST+1; // KAKU is device 1;
        event->SourceUnit    = 0; // Komt niet van een Nodo unit af.
        success=true;
        }
      break;
      }
      
    case DEVICE_COMMAND:
      {
      // Niet gebruikt voor dit device.
      break;
      }
    #endif //DEVICE_CORE_01
      
    #ifdef NODO_MEGA
    case DEVICE_MMI_IN:
      {
      // Reserveer een kleine string en kap voor de zekerheid de inputstring af om te voorkomen dat er
      // buiten gereserveerde geheugen gewerkt wordt.
      char *TempStr=(char*)malloc(26);
      string[25]=0;

      // Hier aangekomen bevat string het volledige commando. Test als eerste of het opgegeven commando overeen komt met "Kaku"
      // Dit is het eerste argument in het commando.
      if(GetArgv(string,TempStr,1))
        {
        if(strcasecmp(TempStr,"Kaku")==0)
          {
          event->Command=CMD_DEVICE_FIRST+1; // KAKU is device 1
            
          byte grp=0,c;
          byte x=0;       // teller die wijst naar het het te behandelen teken
          byte Home=0;    // KAKU home A..P
          byte Address=0; // KAKU Address 1..16
          byte y=false;   // Notatiewijze
        
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
              success=true;
              }
            }
          }
        }
      free(TempStr);
      break;
      }

    case DEVICE_MMI_OUT:
      {
      if(event->Command==CMD_DEVICE_FIRST+1) // KAKU is device 1
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
        }
      break;
      }
    #endif //NODO_MEGA
    }      
  return success;
  }
#endif //DEVICE_01



//#######################################################################################################
//#################################### Device-02: SendKaku   ############################################
//#######################################################################################################


/*********************************************************************************************\
 * Dit device zorgt voor aansturing van Klik-Aan-Klik-Uit ontvangers. 
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
 * Compiled size      : 1100 bytes voor een Mega, 540 bytes voor een Small
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

#ifdef DEVICE_02
#define KAKU_CodeLength    12  // aantal data bits
#define KAKU_T            350  // us

boolean Device_02(byte function, struct NodoEventStruct *event, char *string)
  {
  boolean success=false;
  
  switch(function)
    {
    #ifdef DEVICE_CORE_02
    case DEVICE_RAWSIGNAL_IN:
      {
      // Niet in gebruik. Een device is altijd In of Uit.
      break;
      }
      
    case DEVICE_COMMAND:
      {
      if(event->Command==CMD_DEVICE_FIRST+2) // SendKAKU is device 2
        {
        unsigned long Bitstream;
        byte PTMF=50;
        
        RawSignal.Pulses[0]=PTMF;
        RawSignal.Repeats=5;                   // KAKU heeft vijf herhalingen nodig om te schakelen.
        RawSignal.Number=KAKU_CodeLength*4+2;
        event->Port=VALUE_ALL;                 // Signaal mag naar alle door de gebruiker met [Output] ingestelde poorten worden verzonden.
       
        Bitstream = event->Par2 | (0x600 | ((event->Par1&1/*Commando*/) << 11)); // Stel een bitstream samen
        
        // loop de 12-bits langs en vertaal naar pulse/space signalen.  
        for (byte i=0; i<KAKU_CodeLength; i++)
          {
          RawSignal.Pulses[4*i+1]=KAKU_T/PTMF;
          RawSignal.Pulses[4*i+2]=(KAKU_T*3)/PTMF;
      
          if (((event->Par1>>1)&1) /* Groep */ && i>=4 && i<8) 
            {
            RawSignal.Pulses[4*i+3]=KAKU_T/PTMF;
            RawSignal.Pulses[4*i+4]=KAKU_T/PTMF;
            } // short 0
          else
            {
            if((Bitstream>>i)&1)// 1
              {
              RawSignal.Pulses[4*i+3]=(KAKU_T*3)/PTMF;
              RawSignal.Pulses[4*i+4]=KAKU_T/PTMF;
              }
            else //0
              {
              RawSignal.Pulses[4*i+3]=KAKU_T/PTMF;
              RawSignal.Pulses[4*i+4]=(KAKU_T*3)/PTMF;          
              }          
            }
          }
        RawSignal.Pulses[(KAKU_CodeLength*4)+1] = KAKU_T/PTMF;
        RawSignal.Pulses[(KAKU_CodeLength*4)+2] = (KAKU_T*48)/PTMF;// space van de stopbit nodig voor pauze tussen herhalingen.@@@
        SendEvent(event,true,true);
        success=true;
        }
      break;
      }
    #endif // DEVICE_CORE_02
      
    #ifdef NODO_MEGA
    case DEVICE_MMI_IN:
      {
      // Reserveer een kleine string en kap voor de zekerheid de inputstring af om te voorkomen dat er
      // buiten gereserveerde geheugen gewerkt wordt.
      char *TempStr=(char*)malloc(26);
      string[25]=0;

      // Hier aangekomen bevat string het volledige commando. Test als eerste of het opgegeven commando overeen komt met "SendKaku"
      // Dit is het eerste argument in het commando.
      if(GetArgv(string,TempStr,1))
        {
        if(strcasecmp(TempStr,"SendKaku")==0)
          {
          event->Command=CMD_DEVICE_FIRST+2; //Send KAKU is device 2
            
          byte grp=0,c;
          byte x=0;       // teller die wijst naar het het te behandelen teken
          byte Home=0;    // KAKU home A..P
          byte Address=0; // KAKU Address 1..16
          byte y=false;   // Notatiewijze
        
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
              success=true;
              }
            }
          }
        }
      free(TempStr);
      break;
      }

    case DEVICE_MMI_OUT:
      {
      if(event->Command==CMD_DEVICE_FIRST+2) // KAKU is device 2
        {
        strcpy(string,"SendKaku");            // Commando / event 
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
        }
      break;
      }
    #endif //NODO_MEGA
    }      
  return success;
  }
#endif //DEVICE_02

  
//#######################################################################################################
//#################################### Device-03: NewKAKU ###############################################
//#######################################################################################################

/*********************************************************************************************\
 * Dit protocol zorgt voor ontvangst Klik-Aan-Klik-Uit zenders
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
 * Syntax             : "NewKAKU <Adres>,<On|Off|dimlevel 1..15>
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

#ifdef DEVICE_03
#define DEVICE_ID 03
#define DEVICE_NAME "NewKAKU"
#define NewKAKU_RawSignalLength      132
#define NewKAKUdim_RawSignalLength   148
#define NewKAKU_1T                   275        // us
#define NewKAKU_mT                   500        // us, midden tussen 1T en 4T 
#define NewKAKU_4T                  1100        // us
#define NewKAKU_8T                  2200        // us, Tijd van de space na de startbit

boolean Device_03(byte function, struct NodoEventStruct *event, char *string)
  {
  boolean success=false;
  
  switch(function)
    {
    #ifdef DEVICE_CORE_03
    case DEVICE_RAWSIGNAL_IN:
      {
      unsigned long bitstream=0L;
      boolean Bit;
      int i;
      int PTMF=RawSignal.Pulses[0]; // in eerste element zit de waarde waarmee vermenigvuldigd moet worden om te komen tot de echte pulstijden in uSec.
      
      int P0,P1,P2,P3;
      event->Par1=0;
      
      // nieuwe KAKU bestaat altijd uit start bit + 32 bits + evt 4 dim bits. Ongelijk, dan geen NewKAKU
      if (RawSignal.Number==NewKAKU_RawSignalLength && (RawSignal.Number!=NewKAKUdim_RawSignalLength))
        {
        // RawSignal.Number bevat aantal pulsen * 2  => negeren
        // RawSignal.Pulses[1] bevat startbit met tijdsduur van 1T => negeren
        // RawSignal.Pulses[2] bevat lange space na startbit met tijdsduur van 8T => negeren
        i=3; // RawSignal.Pulses[3] is de eerste van een T,xT,T,xT combinatie
        
        do 
          {
          P0=RawSignal.Pulses[i]    * PTMF;
          P1=RawSignal.Pulses[i+1]  * PTMF;
          P2=RawSignal.Pulses[i+2]  * PTMF;
          P3=RawSignal.Pulses[i+3]  * PTMF;
          
          if     (P0<NewKAKU_mT && P1<NewKAKU_mT && P2<NewKAKU_mT && P3>NewKAKU_mT)Bit=0; // T,T,T,4T
          else if(P0<NewKAKU_mT && P1>NewKAKU_mT && P2<NewKAKU_mT && P3<NewKAKU_mT)Bit=1; // T,4T,T,T
          else if(P0<NewKAKU_mT && P1<NewKAKU_mT && P2<NewKAKU_mT && P3<NewKAKU_mT)       // T,T,T,T Deze hoort te zitten op i=111 want: 27e NewKAKU bit maal 4 plus 2 posities voor startbit
            {
            if(RawSignal.Number!=NewKAKUdim_RawSignalLength) // als de dim-bits er niet zijn
              return false;
            }
          else
            return false; // andere mogelijkheden zijn niet geldig in NewKAKU signaal.  
            
          if(i<130) // alle bits die tot de 32-bit pulstrein behoren 32bits * 4posities per bit + pulse/space voor startbit
            bitstream=(bitstream<<1) | Bit;
          else // de resterende vier bits die tot het dimlevel behoren 
            event->Par1=(event->Par1<<1) | Bit;
       
          i+=4;// volgende pulsenquartet
          }while(i<RawSignal.Number-2); //-2 omdat de space/pulse van de stopbit geen deel meer van signaal uit maakt.
            
        // Adres deel:
        if(bitstream>0xffff)                         // Is het signaal van een originele KAKU zender afkomstig, of van een Nodo ingegeven door de gebruiker ?
        #if NODO_30_COMPATIBLE
          event->Par2=bitstream &0x0FFFFFCF;         // Op hoogste nibble zat vroeger het signaaltype. 
        #else
          event->Par2=bitstream &0xFFFFFFCF;         // dan hele adres incl. unitnummer overnemen. Alleen de twee commando-bits worden er uit gefilterd
        #endif
        
        else                                         // Het is van een andere Nodo afkomstig. 
          event->Par2=(bitstream>>6)&0xff;           // Neem dan alleen 8bit v/h adresdeel van KAKU signaal over
          
        // Commando en Dim deel
        if(i>140)
          event->Par1++;                             // Dim level. +1 omdat gebruiker dim level begint bij één.
        else
          event->Par1=((bitstream>>4)&0x01)?VALUE_ON:VALUE_OFF; // On/Off bit omzetten naar een Nodo waarde. 
        event->Command       = CMD_DEVICE_FIRST+DEVICE_ID;
        event->SourceUnit    = 0;                     // Komt niet van een Nodo unit af, dus unit op nul zetten
        RawSignal.Repeats    = true;                  // het is een herhalend signaal. Bij ontvangst herhalingen onderdrukken.
        success=true;
        }
      break;
      }
      
    case DEVICE_COMMAND:
      break;
    #endif // DEVICE_CORE_03
      
    #ifdef NODO_MEGA
    case DEVICE_MMI_IN:
      {
      char* str=(char*)malloc(40);
      string[25]=0; // kap voor de zekerheid de string af.
    
      if(GetArgv(string,str,1))
        {
        if(strcasecmp(str,DEVICE_NAME)==0)
          {
          // Hier wordt de tekst van de protocolnaam gekoppeld aan het device ID.
          event->Command=CMD_DEVICE_FIRST+DEVICE_ID;
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
                if(event->Par1>=1 && event->Par1<=16) // geldig dim bereik 1..16 ?
                   success=true;
                }
              }
            }
          }
        }
      free(str);
      break;
      }

    case DEVICE_MMI_OUT:
      {
      if(event->Command==CMD_DEVICE_FIRST+DEVICE_ID)
        {
        strcpy(string,DEVICE_NAME);            // Eerste argument=het commando deel
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
        success=true;
        }
      break;
      }
    #endif //NODO_MEGA
    }      
  return success;
  }
#endif //DEVICE_03


//#######################################################################################################
//#################################### Device-04: SendNewKAKU ###########################################
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

#ifdef DEVICE_04
#define DEVICE_ID 04
#define DEVICE_NAME "SendNewKAKU"
#define NewKAKU_RawSignalLength      132
#define NewKAKUdim_RawSignalLength   148
#define NewKAKU_1T                   275        // us
#define NewKAKU_mT                   500        // us, midden tussen 1T en 4T 
#define NewKAKU_4T                  1100        // us
#define NewKAKU_8T                  2200        // us, Tijd van de space na de startbit

boolean Device_04(byte function, struct NodoEventStruct *event, char *string)
  {
  boolean success=false;
  
  switch(function)
    {
    #ifdef DEVICE_CORE_04
    case DEVICE_RAWSIGNAL_IN:
      break;
      
    case DEVICE_COMMAND:
      {
      unsigned long bitstream=0L;
      byte Bit, i=1;
      byte x; /// aantal posities voor pulsen/spaces in RawSignal
    
      const byte PTMF=25;
      RawSignal.Pulses[0]=PTMF;
    
      // bouw het KAKU adres op. Er zijn twee mogelijkheden: Een adres door de gebruiker opgegeven binnen het bereik van 0..255 of een lange hex-waarde
      if(event->Par2<=255)
        bitstream=1|(event->Par2<<6);  // Door gebruiker gekozen adres uit de Nodo_code toevoegen aan adres deel van de KAKU code. 
      else
        bitstream=event->Par2 & 0xFFFFFFCF; // adres geheel over nemen behalve de twee bits 5 en 6 die het schakel commando bevatten.
    
      event->Port=VALUE_ALL; // Signaal mag naar alle door de gebruiker met [Output] ingestelde poorten worden verzonden.
      RawSignal.Repeats=5;   // vijf herhalingen.
    
      if(event->Par1==VALUE_ON || event->Par1==VALUE_OFF)
        {
        bitstream|=(event->Par1==VALUE_ON)<<4; // bit-5 is het on/off commando in KAKU signaal
        x=130;// verzend startbit + 32-bits = 130
        }
      else
        x=146;// verzend startbit + 32-bits = 130 + 4dimbits = 146
     
      // bitstream bevat nu de KAKU-bits die verzonden moeten worden.
    
      for(i=3;i<=x;i++)RawSignal.Pulses[i]=NewKAKU_1T/PTMF;  // De meeste tijden in signaal zijn T. Vul alle pulstijden met deze waarde. Later worden de 4T waarden op hun plek gezet
      
      i=1;
      RawSignal.Pulses[i++]=NewKAKU_1T/PTMF; //pulse van de startbit
      RawSignal.Pulses[i++]=NewKAKU_8T/PTMF; //space na de startbit
      
      byte y=31; // bit uit de bitstream
      while(i<x)
        {
        if((bitstream>>(y--))&1)
          RawSignal.Pulses[i+1]=NewKAKU_4T/PTMF;     // Bit=1; // T,4T,T,T
        else
          RawSignal.Pulses[i+3]=NewKAKU_4T/PTMF;     // Bit=0; // T,T,T,4T
    
        if(x==146)  // als het een dim opdracht betreft
          {
          if(i==111) // Plaats van de Commando-bit uit KAKU 
            RawSignal.Pulses[i+3]=NewKAKU_1T/PTMF;  // moet een T,T,T,T zijn bij een dim commando.
          if(i==127)  // als alle pulsen van de 32-bits weggeschreven zijn
            {
            bitstream=(unsigned long)event->Par1-1; //  nog vier extra dim-bits om te verzenden. -1 omdat dim niveau voor gebruiker begint bij 1
            y=3;
            }
          }
        i+=4;
        }
      RawSignal.Pulses[i++]=NewKAKU_1T/PTMF; //pulse van de stopbit
      RawSignal.Pulses[i]=NewKAKU_1T*20/PTMF; //space van de stopbit tevens pause tussen signalen
      RawSignal.Number=i; // aantal bits*2 die zich in het opgebouwde RawSignal bevinden
      SendEvent(event,true,true);
      success=true;
      break;
      }
    #endif // DEVICE_CORE_04
      
    #ifdef NODO_MEGA
    case DEVICE_MMI_IN:
      {
      char* str=(char*)malloc(40);
      string[25]=0; // kap voor de zekerheid de string af.
    
      if(GetArgv(string,str,1))
        {
        if(strcasecmp(str,DEVICE_NAME)==0)
          {
          // Hier wordt de tekst van de protocolnaam gekoppeld aan het device ID.
          event->Command=CMD_DEVICE_FIRST+DEVICE_ID;
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
                if(event->Par1>=1 && event->Par1<=16) // geldig dim bereik 1..16 ?
                   success=true;
                }
              }
            }
          }
        }
      free(str);
      break;
      }

    case DEVICE_MMI_OUT:
      {
      if(event->Command==CMD_DEVICE_FIRST+DEVICE_ID)
        {
        strcpy(string,DEVICE_NAME);            // Eerste argument=het commando deel
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
        success=true;
        }
      break;
      }
    #endif //NODO_MEGA
    }      
  return success;
  }
#endif //DEVICE_04



//#######################################################################################################
//#################################### Device-05: TempSensor Dallas DS18B20  ############################
//#######################################################################################################


/*********************************************************************************************\
 * Deze funktie leest een Dallas temperatuursensor uit. 
 * Na uitgelezen van de waard wordt in de opgegeven variabele de temperatuur opgeslagen. 
 * Na uitlezing wordt er een event gegenereerd.
 *
 * Auteur             : Paul Tonkes, p.k.tonkes@gmail.com
 * Support            : <website>
 * Datum              : Feb.2013
 * Versie             : 1.1
 * Nodo productnummer : ???
 * Compatibiliteit    : Vanaf Nodo build nummer 508
 * Syntax             : "TempSensor <Par1:Poortnummer>, <Par2:Variabele>"
 *
 ***********************************************************************************************
 * Technische beschrijving:
 *
 * Compiled size      : ??? bytes voor een Mega en ??? voor een Small.
 * Externe funkties   : float2ul(), 
 *
 * De sensor kan volgens de paracitaire mode worden aangesloten. De signaallijn tevens verbinden met een 4K7 naar de Vcc/+5
 * Deze fucntie kan worden gebruikt voor alle Wired poorten van de Nodo.
 * Er wordt gebruik gemaakt van de ROM-skip techniek, dus er worden geen adressering gebruikt.
 * Dit betekent max. één sensor per poort. Dit om (veel) geheugen te besparen.  *
 \*********************************************************************************************/

#ifdef DEVICE_05

// Ieder device heeft een uniek ID. Deze worden onderhouden door het Nodo team. Als je een device hebt geprogrammeerd
// die van waarde kan zijn voor andere gebruikers, meldt deze dan aan bij het Nodo team zodat deze kan worden meegenomen
// in de Nodo-releases. Device 99 is een "knutsel" device voor de gebruiker.
uint8_t DallasPin;
#define DEVICE_ID 05

// Een device heeft naast een uniek ID ook een eigen MMI string die de gebruiker kan invoeren via Telnet, Serial, HTTP 
// of een script. Geef hier de naam op. De afhandeling is niet hoofdletter gevoelig.
#define DEVICE_NAME "TempRead"

boolean Device_05(byte function, struct NodoEventStruct *event, char *string)
  {
  boolean success=false;
  static byte RepeatFirstTime=2;        // Sommige varianten van de Dallas sensor geven de eerste uitlezing een 0 retour. Daarom de eerste keer tweemaal aanroepen.
  
  switch(function)
    {
    #ifdef DEVICE_CORE_05
    case DEVICE_RAWSIGNAL_IN:
      break;
      
    case DEVICE_COMMAND:
      {
      int DSTemp;                           // Temperature in 16-bit Dallas format.
      byte ScratchPad[12];                  // Scratchpad buffer Dallas sensor.   
      byte var=event->Par2;                 // Variabele die gevuld moet worden.
      
      // De Dallas sensor kan worden aangesloten op iedere digitale poort van de Arduino. In ons geval kiezen we er voor
      // om de sensor aan te sluiten op de Wired-Out poorten van de Nodo. Met Par2 is de Wired poort aangegeven.
      // 1 = WiredOut poort 1.  
      DallasPin=PIN_WIRED_OUT_1+event->Par1-1;
    
      ClearEvent(event);                                      // Ga uit van een default schone event. Oude eventgegevens wissen.
    
      do
        {
        noInterrupts();
        boolean present=DS_reset();DS_write(0xCC /* rom skip */); DS_write(0x44 /* start conversion */);
        interrupts();
        
        if(present)
          {
          delay(800);     // uitleestijd die de sensor nodig heeft
      
          noInterrupts();
          DS_reset(); DS_write(0xCC /* rom skip */); DS_write(0xBE /* Read Scratchpad */);
      
          // Maak de lijn floating zodat de sensor de data op de lijn kan zetten.
          digitalWrite(DallasPin,LOW);
          pinMode(DallasPin,INPUT);
      
          for (byte i = 0; i < 9; i++)            // copy 8 bytes
            ScratchPad[i] = DS_read();
          interrupts();
        
          DSTemp = (ScratchPad[1] << 8) + ScratchPad[0];  
      
          event->Command      = CMD_VARIABLE_SET;                 // Commando "VariableSet"
          event->Par1         = var;                              // Variabele die gevuld moet worden.
          event->Par2         = float2ul(float(DSTemp)*0.0625);   // DS18B20 variant. Waarde terugstoppen in de variabele
          success=true;
          }
        if(RepeatFirstTime)
          RepeatFirstTime--;
        }while(RepeatFirstTime);
      break;
      }
    #endif // DEVICE_CORE_05
      
    #ifdef NODO_MEGA
    case DEVICE_MMI_IN:
      {
      char *TempStr=(char*)malloc(26);
      string[25]=0;

      if(GetArgv(string,TempStr,1))
        {
        if(strcasecmp(TempStr,DEVICE_NAME)==0)
          {
          event->Command=CMD_DEVICE_FIRST+DEVICE_ID;
          // Par1 en Par2 hoeven niet te worden geparsed omdat deze default al door de MMI invoer van de Nodo 
          // worden gevuld indien het integer waarden zijn. Toetsen op bereikenmoet nog wel plaats vinden.
          if(event->Par1>0 && event->Par1<WIRED_PORTS && event->Par2>0 && event->Par2<=USER_VARIABLES_MAX)
            success=true;
          }
        }
      free(TempStr);
      break;
      }

    case DEVICE_MMI_OUT:
      {
      // De code die zich hier bevindt zorgt er voor dan een event met het unieke ID in de struct [event] weer wordt
      // omgezet naar een leesbaar event voor de gebruiker. het resultaat moet worden geplaatst in de string [string]
      // let op dat het totale commando niet meer dan 25 posities in beslag neemt.
      // Dit deel van de code wordt alleen uitgevoerd door een Nodo Mega, omdat alleen deze over een MMI beschikt.
      if(event->Command==CMD_DEVICE_FIRST+DEVICE_ID)
        {
        strcpy(string,DEVICE_NAME);            // Eerste argument=het commando deel
        strcat(string," ");
        strcat(string,int2str(event->Par1));
        strcat(string,",");
        strcat(string,int2str(event->Par2));
        // Vervolgens kan met strcat() hier de parameters aan worden toegevoegd      
        }
      break;
      }
    #endif //NODO_MEGA
    }      
  return success;
  }
#endif //DEVICE_05

#ifdef DEVICE_CORE_05
uint8_t DS_read(void)
  {
  uint8_t bitMask;
  uint8_t r = 0;
  uint8_t BitRead;
  
  for (bitMask = 0x01; bitMask; bitMask <<= 1)
    {
    pinMode(DallasPin,OUTPUT);
    digitalWrite(DallasPin,LOW);
    delayMicroseconds(3);

    pinMode(DallasPin,INPUT);// let pin float, pull up will raise
    delayMicroseconds(10);
    BitRead = digitalRead(DallasPin);
    delayMicroseconds(53);

    if (BitRead)
      r |= bitMask;
    }
  return r;
  }

void DS_write(uint8_t ByteToWrite) 
  {
  uint8_t bitMask;

  pinMode(DallasPin,OUTPUT);
  for (bitMask = 0x01; bitMask; bitMask <<= 1)
    {// BitWrite
    digitalWrite(DallasPin,LOW);
    if(((bitMask & ByteToWrite)?1:0) & 1)
      {
      delayMicroseconds(5);// Dallas spec.= 5..15 uSec.
      digitalWrite(DallasPin,HIGH);
      delayMicroseconds(55);// Dallas spec.= 60uSec.
      }
    else
      {
      delayMicroseconds(55);// Dallas spec.= 60uSec.
      digitalWrite(DallasPin,HIGH);
      delayMicroseconds(5);// Dallas spec.= 5..15 uSec.
      }
    }
  }

uint8_t DS_reset()
  {
  uint8_t r;
  uint8_t retries = 125;
  
  pinMode(DallasPin,INPUT);
  do  {  // wait until the wire is high... just in case
      if (--retries == 0) return 0;
      delayMicroseconds(2);
      } while ( !digitalRead(DallasPin));

  pinMode(DallasPin,OUTPUT); digitalWrite(DallasPin,LOW);
  delayMicroseconds(492); // Dallas spec. = Min. 480uSec. Arduino 500uSec.
  pinMode(DallasPin,INPUT);//Float
  delayMicroseconds(40);
  r = !digitalRead(DallasPin);// Dallas sensor houdt voor 60uSec. de bus laag nadat de Arduino de lijn heeft vrijgegeven.
  delayMicroseconds(420);
  return r;
  }
#endif // DEVICE_05_CORE

          

//#######################################################################################################
//#################################### Device-15: Signal Analyzer   #####################################
//#######################################################################################################


/*********************************************************************************************\
 * Dit device geeft een pulsenreeks weer die op RF of IR is binnengekomen. Het is bedoeld
 * als hulpmiddel om signalen te analyseren in geval je niet beschikt over een scope of
 * een logic analyzer. Signaal wordt alleen weergegeven op Serial/USB. Manchster laat alle
 * Mark & Spaces zien. 
 *
 * Auteur             : Paul Tonkes
 * Support            : www.nodo-domotica.nl
 * Datum              : 10-03-2013
 * Versie             : 0.1
 * Nodo productnummer : <Nodo productnummer. Toegekend door Nodo team>
 * Compatibiliteit    : R513
 * Syntax             : "SignalAnalyze"
 *
 ***********************************************************************************************
 * Technische beschrijving:
 *
 * Compiled size      : 1200 bytes voor een Mega.
 * Externe funkties   : <geef hier aan welke funkties worden gebruikt. 
 *
 * 
 \*********************************************************************************************/
 
#ifdef DEVICE_15

#define DEVICE_ID 15
#define DEVICE_NAME "RawSignalAnalyze"

boolean Device_15(byte function, struct NodoEventStruct *event, char *string)
  {
  boolean success=false;
  
  switch(function)
    {
    #ifdef DEVICE_CORE_15
    case DEVICE_RAWSIGNAL_IN:
      {
      return false;
      }      

    case DEVICE_COMMAND:
      {
      if(RawSignal.Number<8)return false;
      
      int x;
      unsigned int y,z;
      byte PTMF=RawSignal.Pulses[0];
    
      // zoek naar de langste kortst puls en de kortste lange puls
      unsigned int MarkShort=50000;
      unsigned int MarkLong=0;
      for(x=1;x<RawSignal.Number;x+=2)
        {
        y=RawSignal.Pulses[x]*PTMF;
        if(y<MarkShort)
          MarkShort=y;
        if(y>MarkLong)
          MarkLong=y;
        }
      z=true;
      while(z)
        {
        z=false;
        for(x=1;x<RawSignal.Number;x+=2)
          {
          y=RawSignal.Pulses[x]*PTMF;
          if(y>MarkShort && y<(MarkShort+MarkShort/2))
            {
            MarkShort=y;
            z=true;
            }
          if(y<MarkLong && y>(MarkLong-MarkLong/2))
            {
            MarkLong=y;
            z=true;
            }
          }
        }
      unsigned int MarkMid=((MarkLong-MarkShort)/2)+MarkShort;
  
      // zoek naar de langste kortst puls en de kortste lange puls
      unsigned int SpaceShort=50000;
      unsigned int SpaceLong=0;
      for(x=2;x<RawSignal.Number;x+=2)
        {
        y=RawSignal.Pulses[x]*PTMF;
        if(y<SpaceShort)
          SpaceShort=y;
        if(y>SpaceLong)
          SpaceLong=y;
        }
      z=true;
      while(z)
        {
        z=false;
        for(x=2;x<RawSignal.Number;x+=2)
          {
          y=RawSignal.Pulses[x]*PTMF;
          if(y>SpaceShort && y<(SpaceShort+SpaceShort/2))
            {
            SpaceShort=y;
            z=true;
            }
          if(y<SpaceLong && y>(SpaceLong-SpaceLong/2))
            {
            SpaceLong=y;
            z=true;
            }
          }
        }
      int SpaceMid=((SpaceLong-SpaceShort)/2)+SpaceShort;
    
      // Bepaal soort signaal
      y=0;
      if(MarkLong  > (2*MarkShort  ))y=1; // PWM
      if(SpaceLong > (2*SpaceShort ))y+=2;// PDM

      Serial.print(F( "Bits="));

      if(y==0)Serial.println(F("?"));
      if(y==1)
        {
        for(x=1;x<RawSignal.Number;x+=2)
          {
          y=RawSignal.Pulses[x]*PTMF;
          if(y>MarkMid)
            Serial.write('1');
          else
            Serial.write('0');
          }
        Serial.print(F("Type=PWM, "));
        }
      if(y==2)
        {
        for(x=2;x<RawSignal.Number;x+=2)
          {
          y=RawSignal.Pulses[x]*PTMF;
          if(y>SpaceMid)
            Serial.write('1');
          else
            Serial.write('0');
          }
        Serial.print(F("Type=PDM, "));
        }
      if(y==3)
        {
        for(x=1;x<RawSignal.Number;x+=2)
          {
          y=RawSignal.Pulses[x]*PTMF;
          if(y>MarkMid)
            Serial.write('1');
          else
            Serial.write('0');
          
          y=RawSignal.Pulses[x+1]*PTMF;
          if(y>SpaceMid)
            Serial.write('1');
          else
            Serial.write('0');
          }
        Serial.print(F( ", Type=Manchester"));
        }

      Serial.print(F(", Pulses="));
      Serial.print(RawSignal.Number/2);

      Serial.print(F(", Pulses(uSec)="));      
      for(x=1;x<RawSignal.Number;x++)
        {
        Serial.print(RawSignal.Pulses[x]*PTMF); 
        Serial.write(',');       
        }
      Serial.println();
      
//      int dev=250;  
//      for(x=1;x<=RawSignal.Number;x+=2)
//        {
//        for(y=1+int(RawSignal.Pulses[x])*PTMF/dev; y;y--)
//          Serial.write('M');  // Mark  
//        for(y=1+int(RawSignal.Pulses[x+1])*PTMF/dev; y;y--)
//          Serial.write('_');  // Space  
//        }    
//      Serial.println();
//      PrintTerminal(ProgmemString(Text_22));

      break;
      }      
    #endif // CORE
      
    #ifdef NODO_MEGA
    case DEVICE_MMI_IN:
      {
      char *TempStr=(char*)malloc(26);
      string[25]=0;
      if(GetArgv(string,TempStr,1))
        {
        if(strcasecmp(TempStr,DEVICE_NAME)==0)
          {
          event->Command=CMD_DEVICE_FIRST+DEVICE_ID;
          success=true;
          }
        }
      free(TempStr);
      break;
      }

    case DEVICE_MMI_OUT:
      {
      if(event->Command==CMD_DEVICE_FIRST+DEVICE_ID)
        {
        strcpy(string,DEVICE_NAME);            // Eerste argument=het commando deel
        strcat(string," ");
        strcat(string,int2str(event->Par1));
        }
      break;
      }
    #endif //NODO_MEGA
    }      
  return success;
  }
#endif //DEVICE_15


          

//#######################################################################################################
//#################################### Device-99: Leeg   ################################################
//#######################################################################################################


/*********************************************************************************************\
 * Funktionele beschrijving:
 * 
 * <Geef hier een beschrijving van de funktionele werking van het device en hoe deze door de gebruiker
 * moet worden aangeroepen.>
 *
 * Auteur             : <naam en EMail adres van de ontwikkelaar>
 * Support            : <website>
 * Datum              : <datum>
 * Versie             : <verise>
 * Nodo productnummer : <Nodo productnummer. Toegekend door Nodo team>
 * Compatibiliteit    : Vanaf Nodo build nummer <build nummer>
 * Syntax             : "MijnDevice <Par1>, <Par2>"
 *
 ***********************************************************************************************
 * Technische beschrijving:
 *
 * Compiled size      : <grootte> bytes voor een Mega en <grootte> voor een Small.
 * Externe funkties   : <geef hier aan welke funkties worden gebruikt. 
 *
 * <Geef hier een technische toelichting op de werking van het device en eventueel gebruikte protocol>
 * 
 * Tips en aandachtspunten voor programmeren van een protocol:
 * 
 * -  Geheugen is beperkt. Programmeer compact en benut iedere byte RAM en PROGMEM. De Arduino heeft niet de luxe van een PC!
 *    Het is verantwoordelijkheid van de programmeur om te bewaken. Vrij geheugen is opvraagbaar met [Status FreeMem].
 * -  Bouw geen lange wachtlussen in de code. Dit kan leiden tot timings-promlemen waaronder missen van events.
 *    Deze funktie bevindt zich ik een tijdkritische loop. Detecteer zo snel mogelijk of het ontvangen signaal
 *    ook bij dit protocol hoort. 
 * -  De array RawSignal.Pulses[] bevat alle Mark en Space tijden in microseconden. Te beginnen vanaf element [1].
 *    Deze array is reeds gevuld bij aankomst. Element [0] bevat een vermenigvuldigingsfactor voor omrekenen naar
 *    echte microseconden.
 * -  RawSignal.Pulses bevat het aantal posities die een mark/space bevatten. Let op dat de waarde RAW_BUFFER_SIZE
 *    nooit overschreden wordt. Anders gegarandeerd vastlopers! Positie [1] bevat de 'mark' van de startbit. 
 * -  De struct NodoEventStruct bevat alle informatie die nodig is voor verwerking en weergave van het event
 *    dat is ontvangen of moet worden weergegeven. 
 *    of er voldoende geheugen over blijft voor stabiele werking van de Nodo.
 * -  Om uitwisselbaar te blijven met andere Nodo versies en Nodo gebruikers, geen aanpassingen aan de Code maken.
 *    Let op dat je bij gebruik van functies uit de Nodo code je je eigen code gevoelig makt voor onderhoud
 *    bij uitbrengen van nieuwe releases.
 * -  Maak geen gebruik van interrupt driven routines, dit verstoort (mogelijk) de werking van de I2C, Serial en ethernet
 *    communicatie.
 * -  Maak slecht in uitzonderlijke gevallen gebruik van Globals en pas geen waarden van globals uit de Nodo code aan.
 * -  Besteed uitgebreid aandacht aan de documentatie van het protocol. Indien mogelijk verwijzen naar originele
 *    specificatie.
 *
 * Voorbeelden van userdevices:
 * - Digitale temperatuur sensoren (Zoals Dallas DS18B20)
 * - Digitale vochtigheidssensoren (Zoals DTH-11)
 * - Vergroten van aantal digitale wired met een multiplexer. Tot 8-ingangen per Wired poort met bv. een 74151.
 * - Acht verschillende analoge ingangen meten met eén WiredIn met een LTC1380.
 * - WiredOut uitgangen uitbreiden tot 8, 16, 32, 64 verschillende digitale uitgangen met behulp van 74HCT595
 * - I2C devices aansturen via de SLC en SDA lijnen van de arduino.
 * - etcetera.
 *
 * De Wired poorten en de additionele IO poorten op de Mega in uw eigen code gebruiken aan de hand van de naam zoals deze zijn gedefinieerd
 * in de code:
 *
 * PIN_WIRED_OUT_n staat voor WiredOut poort, waarbij n overeen komt met het nummer van de WiredOut poort te beginnen met 1.
 * PIN_WIRED_IN_n staat voor WiredOut poort, waarbij n overeen komt met het nummer van de WiredOut poort te beginnen met 1.
 * PIN_IO_n staat voor additionele IO-poort, waarbij n overeen komt met het nummer van deze poort, te beginnen met 1 (Alleen beschikbaar op de Mega).
 * 
 * Voor de verwijzing naar de Arduino pinnummers: zie schema of declaraties in tabblad "Base"
 * ANDERE POORTEN NIET GEBRUIKEN OMDAT DEZE (IN DE TOEKOMST) EEN SPECIFIEKE FUNKTIE HEBBEN.
 * Let bij het ontwerp van de Hardware en de software op dat geen gebruik wordt gemaakt van de Arduino pinnen die al een voorgedefinieerde
 * Nodo funktie hebben, De WiredIn en WiredOut kunnen wel vrij worden gebruikt met die kanttekening dat Nodo commando's de lijnen eveneens 
 * Besturen wat kan leiden tot beschadiging van hardware. Let hier in het bijzonder op als een Arduino pin wordt gedefinieerd als een output.
 * Op de Nodo Mega zijn nog vier extra communicatielijnen die gebruikt kunnen worden voor User input/output: PIN_IO_1 t/m PIN_IO_4 (Arduino pin 38 t/m 41)
 * Besef dat niet alle pennen van de Arduino gebruikt kunnen worden daar vele Arduino pinnen al een voorgedefinieerde
 * Nodo funktie hebben, De WiredIn en WiredOut kunnen wel vrij worden gebruikt met die kanttekening dat Nodo commando's de lijnen eveneens 
 * Besturen wat kan leiden tot beschadiging van hardware. Let hier in het bijzonder op als een Arduino pin wordt gedefinieerd als een output.
 * Op de Nodo Mega zijn nog vier extra communicatielijnen die gebruikt kunnen worden voor User input/output: PIN_IO_1 t/m PIN_IO_4 (Arduino pin 38 t/m 41)
 \*********************************************************************************************/
 
// Compileer de code van dit divice alleen als de gebruiker in het Nodo tabblad de definitie DEVICE_99 heeft
// opgenomen. Ins dit niet het geval, dan zal de code ook niet worden gecompileerd en geen geheugenruimte in beslag nemen.
#ifdef DEVICE_99

// Ieder device heeft een uniek ID. Deze worden onderhouden door het Nodo team. Als je een device hebt geprogrammeerd
// die van waarde kan zijn voor andere gebruikers, meldt deze dan aan bij het Nodo team zodat deze kan worden meegenomen
// in de Nodo-releases. Device 99 is een "knutsel" device voor de gebruiker.
#define DEVICE_ID 99

// Een device heeft naast een uniek ID ook een eigen MMI string die de gebruiker kan invoeren via Telnet, Serial, HTTP 
// of een script. Geef hier de naam op. De afhandeling is niet hoofdletter gevoelig.
#define DEVICE_NAME "UserDevice"

// Deze device code waidt vanuit meerdere plaatsen in de Nodo code aangeroepen, steeds met een doel. Dit doel bevindt zich
// in de variabele [function]. De volgende doelen zijn gedefinieerd:
// DEVICE_RAWSIGNAL_IN    => Afhandeling van een via RF/IR ontvangen event
// DEVICE_COMMAND   => Commando voor afhandelen/uitsturen van een event.
// DEVICE_MMI_IN      => Invoer van de gebruiker/script omzetten naar een event. (Alleen voor mega)
// DEVICE_MMI_OUT     => Omzetten van een event naar een voor de gebruiker leesbare tekst (Alleen voor Mega)
//
// De struct [event] is het inhoudelijke event. Deze bevat twee parameters die vrij voor een device gebruikt kunnen worden. 
// Deze zijn: (byte) event->Par1 en (unsigned long) event->par2. De string [string] wordt gebruikt voor binnenkomende of uitgaande
// MMI tekst.
boolean Device_99(byte function, struct NodoEventStruct *event, char *string)
  {
  boolean success=false;
  
  switch(function)
    {
    #ifdef DEVICE_CORE_99
    case DEVICE_INIT:
      {
      // Code hier wordt eenmalig aangeroepen na een reboot van de Nodo.
      break;
      }
    case DEVICE_RAWSIGNAL_IN:
      {
      // Code op deze plaats wordt uitgevoerd zodra er een event via RF of IR is binnengekomen
      // De RawSignal buffer is gevuld met pulsen. de struct [event] moet hier worden samengesteld.      
      // Als decoderen is gelukt, dan de variabele [success] vullen met een true. De Nodo zal het 
      // event vervolgens als een regulier event afhandelen.
      break;
      }
      
    case DEVICE_COMMAND:
      {
      // Als er vanuit de gebruiker, script of eventlist dit device een event moet uitsturen, dan is op het
      // moment dat de code hier wordt aangeroepen, de struct [event] gevuld en gereed voor verwerking.
      // Als voor verlaten de struct [event] is gevuld met een ander event, dan wordt deze uitgevoerd als een nieuw
      // event. Dit kan bijvoorbeeld worden benut als een variabele wordt uitgelezen en de waarde verder verwerkt
      // moet worden.
      break;
      }
      
    #ifdef NODO_MEGA
    case DEVICE_MMI_IN:
      {
      // Zodra er via een script, HTTP, Telnet of Serial een commando wordt ingevoerd, wordt dit deel van de code langs gelopen.
      // Op deze plet kan de invoer [string] worden gepardsed en omgezet naar een struct [event]. Als parsen van de invoerstring [string]
      // is gelukt en de struct is gevuld, dan de variabele [success] vullen met true zodat de Nod zorg kan dragen voor verdere verwerking van het event.

      char *TempStr=(char*)malloc(26);
      string[25]=0;

      // Met DEVICE_MMI_IN wordt de invoer van de gebruiker (string) omgezet naar een event zodat de Nodo deze intern kan verwerken.
      // Hier aangekomen bevat string het volledige commando. Test als eerste of het opgegeven commando overeen komt met DEVICE_NAME
      // Dit is het eerste argument in het commando. 
      if(GetArgv(string,TempStr,1))
        {
        if(strcasecmp(TempStr,DEVICE_NAME)==0)
          {
          // Hier wordt de tekst van de protocolnaam gekoppeld aan het device ID.
            event->Command=CMD_DEVICE_FIRST+DEVICE_ID;
                    
          // Vervolgens tweede parameter gebruiken
          if(GetArgv(string,TempStr,2)) 
            {
            // plaats hier je code die de tweede parameter die zich in [TempStr] bevindt verder uitparsed
            // De byte Par1 en unsigned long Par2 die zic in de struct [event] bevindt kunnen worden gebruikt.
              
            if(GetArgv(string,TempStr,3))
              {
              // indien gewenst kan een tweede parameter worden gebruikt (=derde argument)
              // Plaats wederom de code paar uitparsen van de parameter hier.
              // heb je het laatste parameter geparsen, dan de variabele [success] vullen 
              // met een true zodat verdere verwerking van het event plaats kan vinden.
              }
            }
          }
        }
      free(TempStr);
      break;
      }
    #endif // MEGA

    case DEVICE_MMI_OUT:
      {
      // De code die zich hier bevindt zorgt er voor dan een event met het unieke ID in de struct [event] weer wordt
      // omgezet naar een leesbaar event voor de gebruiker. het resultaat moet worden geplaatst in de string [string]
      // let op dat het totale commando niet meer dan 25 posities in beslag neemt.
      // Dit deel van de code wordt alleen uitgevoerd door een Nodo Mega, omdat alleen deze over een MMI beschikt.
      if(event->Command==CMD_DEVICE_FIRST+DEVICE_ID)
        {
        strcpy(string,DEVICE_NAME);            // Eerste argument=het commando deel

        // Vervolgens kan met strcat() hier de parameters aan worden toegevoegd      
        }
      break;
      }
    #endif //NODO_MEGA
    }      
  return success;
  }
#endif //DEVICE_99



