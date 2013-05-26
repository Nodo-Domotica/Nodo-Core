
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
 #define DEVICE_06  // DHT11Read         : Uitlezen temperatuur & vochtigheidsgraad sensor DHT-11
 #define DEVICE_07  // Reserved!         : UserEvents van de oude Nodo. t.b.v. compatibiliteit reeds geprogrammeerde universele afstandsbedieningen.
 #define DEVICE_08  // AlectoV1          : Dit protocol zorgt voor ontvangst van Alecto weerstation buitensensoren met protocol V1
 #define DEVICE_09  // AlectoV2          : Dit protocol zorgt voor ontvangst van Alecto weerstation buitensensoren met protocol V2
 #define DEVICE_10  // AlectoV3          : Dit protocol zorgt voor ontvangst van Alecto weerstation buitensensoren met protocol V3
 #define DEVICE_11  // Reserved!         : OpenTherm (SWACDE-11-V10)
 #define DEVICE_12  // OregonV2          : Dit protocol zorgt voor ontvangst van Oregon buitensensoren met protocol versie V2
 #define DEVICE_13  // FA20RF            : Dit protocol zorgt voor ontvangst van Flamingo FA20RF rookmelder signalen
 #define DEVICE_14  // FA20RFSend        : Dit protocol zorgt voor aansturen van Flamingo FA20RF rookmelder
 #define DEVICE_15  // HomeEasy          : Dit protocol zorgt voor ontvangst HomeEasy EU zenders die werken volgens de automatische codering (Ontvangers met leer-knop)
 #define DEVICE_16  // HomeEasySend      : Dit protocol stuurt HomeEasy EU ontvangers aan die werken volgens de automatische codering (Ontvangers met leer-knop)
 #define DEVICE_17  // Reserved!         : Laat continue analoge metingen zien van alle Wired-In poorten. 
 #define DEVICE_18  // RawSignalAnalyze  : Geeft bij een binnenkomend signaal informatie over de pulsen weer. (SWACDE-18-V10)
 #define DEVICE_19  // RFID              : Innovations ID-12 RFID Tag reader (SWACDE-19-V10) 
 #define DEVICE_20  // Reserved!         : BMP085 Barometric pressure sensor (SWACDE-20-V10)
 #define DEVICE_21  // LCDI2CWrite       : DFRobot LCD I2C/TWI 1602 display
 #define DEVICE_22  // HCSR04_Read       : Ultrasoon afstandsmeter HC-SR04
 #define DEVICE_23  // PWM               : LED-Dimmer PWM driver (SWACDE-18-V10)
 #define DEVICE_24 
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
      
    #if NODO_MEGA
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
      unsigned long Bitstream;
      RawSignal.Multiply=50;
      RawSignal.Repeats=7;                   // KAKU heeft minimaal vijf herhalingen nodig om te schakelen.
      RawSignal.Delay=20;                    // Tussen iedere pulsenreeks enige tijd rust.
      RawSignal.Number=KAKU_CodeLength*4+2;
      event->Port=VALUE_ALL;                 // Signaal mag naar alle door de gebruiker met [Output] ingestelde poorten worden verzonden.
     
      Bitstream = event->Par2 | (0x600 | ((event->Par1&1/*Commando*/) << 11)); // Stel een bitstream samen
      
      // loop de 12-bits langs en vertaal naar pulse/space signalen.  
      for (byte i=0; i<KAKU_CodeLength; i++)
        {
        RawSignal.Pulses[4*i+1]=KAKU_T/RawSignal.Multiply;
        RawSignal.Pulses[4*i+2]=(KAKU_T*3)/RawSignal.Multiply;
    
        if (((event->Par1>>1)&1) /* Groep */ && i>=4 && i<8) 
          {
          RawSignal.Pulses[4*i+3]=KAKU_T/RawSignal.Multiply;
          RawSignal.Pulses[4*i+4]=KAKU_T/RawSignal.Multiply;
          } // short 0
        else
          {
          if((Bitstream>>i)&1)// 1
            {
            RawSignal.Pulses[4*i+3]=(KAKU_T*3)/RawSignal.Multiply;
            RawSignal.Pulses[4*i+4]=KAKU_T/RawSignal.Multiply;
            }
          else //0
            {
            RawSignal.Pulses[4*i+3]=KAKU_T/RawSignal.Multiply;
            RawSignal.Pulses[4*i+4]=(KAKU_T*3)/RawSignal.Multiply;          
            }          
          }
        }
      SendEvent(event,true,true,true);
      success=true;
      break;
      }
    #endif // DEVICE_CORE_02
      
    #if NODO_MEGA
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
      int P0,P1,P2,P3;
      event->Par1=0;
      
      // nieuwe KAKU bestaat altijd uit start bit + 32 bits + evt 4 dim bits. Ongelijk, dan geen NewKAKU
      if (RawSignal.Number==NewKAKU_RawSignalLength || RawSignal.Number==NewKAKUdim_RawSignalLength)
        {
        // RawSignal.Number bevat aantal pulsRawSignal.Multiplyen * 2  => negeren
        // RawSignal.Pulses[1] bevat startbit met tijdsduur van 1T => negeren
        // RawSignal.Pulses[2] bevat lange space na startbit met tijdsduur van 8T => negeren
        i=3; // RawSignal.Pulses[3] is de eerste van een T,xT,T,xT combinatie
        
        do 
          {
          P0=RawSignal.Pulses[i]    * RawSignal.Multiply;
          P1=RawSignal.Pulses[i+1]  * RawSignal.Multiply;
          P2=RawSignal.Pulses[i+2]  * RawSignal.Multiply;
          P3=RawSignal.Pulses[i+3]  * RawSignal.Multiply;
          
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
        event->SourceUnit    = 0;                     // Komt niet van een Nodo unit af, dus unit op nul zetten
        RawSignal.Repeats    = true;                  // het is een herhalend signaal. Bij ontvangst herhalingen onderdrukken.
        success=true;
        }   
      break;
      }
      
    case DEVICE_COMMAND:
      break;
    #endif // DEVICE_CORE_03
      
    #if NODO_MEGA
    case DEVICE_MMI_IN:
      {
      char* str=(char*)malloc(40);
      string[25]=0; // kap voor de zekerheid de string af.
    
      if(GetArgv(string,str,1))
        {
        if(strcasecmp(str,DEVICE_NAME)==0)
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
      SendEvent(event,true,true,true);
      success=true;
      break;
      }
    #endif // DEVICE_CORE_04
      
    #if NODO_MEGA
    case DEVICE_MMI_IN:
      {
      char* str=(char*)malloc(40);
      string[25]=0; // kap voor de zekerheid de string af.
    
      if(GetArgv(string,str,1))
        {
        if(strcasecmp(str,DEVICE_NAME)==0)
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
 * Na uitgelezen van de waarde wordt in de opgegeven variabele de temperatuur opgeslagen. 
 * Na uitlezing wordt er een event gegenereerd.
 *
 * Auteur             : Paul Tonkes, p.k.tonkes@gmail.com
 * Support            : <website>
 * Datum              : Feb.2013
 * Versie             : 1.1
 * Nodo productnummer : ???
 * Compatibiliteit    : Vanaf Nodo build nummer 508
 * Syntax             : "TempRead <Par1:Poortnummer>, <Par2:Variabele>"
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
 * Dit betekent max. Ã©Ã©n sensor per poort. Dit om (veel) geheugen te besparen.  *
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
  static byte Call_Status = 0x00; // Each bit represents one relative port. 0=not called before, 1=already called before. 
  
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
      byte RelativePort=event->Par1-1;
      
      // De Dallas sensor kan worden aangesloten op iedere digitale poort van de Arduino. In ons geval kiezen we er voor
      // om de sensor aan te sluiten op de Wired-Out poorten van de Nodo. Met Par2 is de Wired poort aangegeven.
      // 1 = WiredOut poort 1.  
      DallasPin=PIN_WIRED_OUT_1+event->Par1-1;
      
      ClearEvent(event);                                      // Ga uit van een default schone event. Oude eventgegevens wissen.        
  
      noInterrupts();
      while (!(bitRead(Call_Status, RelativePort)))
        {
        // if this is the very first call to the sensor on this port, reset it to wake it up 
        boolean present=DS_reset();
        bitSet(Call_Status, RelativePort);
        }        
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
      break;
      }
    #endif // DEVICE_CORE_05
      
    #if NODO_MEGA
    case DEVICE_MMI_IN:
      {
      char *TempStr=(char*)malloc(26);
      string[25]=0;

      if(GetArgv(string,TempStr,1))
        {
        if(strcasecmp(TempStr,DEVICE_NAME)==0)
          {
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
//######################## DEVICE-06 Temperatuur en Luchtvochtigheid sensor DHT 11 ######################
//#######################################################################################################

/*********************************************************************************************\
 * Deze funktie leest een DHT11 temperatuur en luchtvochtigheidssensor uit.
 * Deze funktie kan worden gebruikt voor alle digitale poorten van de Arduino.
 * De uitgelezen temperatuur waarde wordt in de opgegeven variabele opgeslagen.
 * De uitgelezen luchtvochtigheidsgraad wordt in de opgegeven variabele +1 opgeslagen.
 * 
 * Auteur             : Nodo-team (Martinus van den Broek) www.nodo-domotica.nl
 * Datum              : Mrt.2013
 * Compatibiliteit    : Vanaf Nodo build nummer 508
 * Syntax             : "DHT11 <Par1:Poortnummer>, <Par2:Basis Variabele>"
 *********************************************************************************************
 * Technische informatie:
 * De DHT11 sensor is een 3 pin sensor met een bidirectionele datapin
 * Het principe is "onewire" maar dit protocol is NIET compatible met het bekende Dallas onewire protocol
 * Dit protocol gebruikt twee variabelen, 1 voor temperatuur en 1 voor luchtvochtigheid
 \*********************************************************************************************/

#ifdef DEVICE_06
#define DEVICE_ID 06
#define DEVICE_NAME "DHT11Read"
uint8_t DHT11_Pin;

boolean Device_06(byte function, struct NodoEventStruct *event, char *string)
{

  boolean success=false;

  switch(function)
  {
#ifdef DEVICE_CORE_06
  case DEVICE_RAWSIGNAL_IN:
    break;

  case DEVICE_COMMAND:
    {
    DHT11_Pin=PIN_WIRED_OUT_1+event->Par1-1;
    byte dht11_dat[5];
    byte dht11_in;
    byte i;
    byte temperature=0;
    byte humidity=0;

    pinMode(DHT11_Pin,OUTPUT);

    // DHT11 start condition, pull-down i/o pin for 18ms
    digitalWrite(DHT11_Pin,LOW);               // Pull low
    delay(18);
    digitalWrite(DHT11_Pin,HIGH);              // Pull high
    delayMicroseconds(40);
    pinMode(DHT11_Pin,INPUT);                  // change pin to input
    delayMicroseconds(40);

    dht11_in = digitalRead(DHT11_Pin);
    if(dht11_in) return false;

    delayMicroseconds(80);
    dht11_in = digitalRead(DHT11_Pin);
    if(!dht11_in) return false;

    delayMicroseconds(40);                     // now ready for data reception
    for (i=0; i<5; i++)
      dht11_dat[i] = read_dht11_dat();
      
    // Checksum calculation is a Rollover Checksum by design!
    byte dht11_check_sum = dht11_dat[0]+dht11_dat[1]+dht11_dat[2]+dht11_dat[3];// check check_sum
    if(dht11_dat[4]!= dht11_check_sum) return false;

    temperature = dht11_dat[2];
    humidity = dht11_dat[0];

    byte VarNr = event->Par2; // De originele Par1 tijdelijk opslaan want hier zit de variabelenummer in waar de gebruiker de uitgelezen waarde in wil hebben
      ClearEvent(event);                                    // Ga uit van een default schone event. Oude eventgegevens wissen.
    event->Command      = CMD_VARIABLE_SET;                 // Commando "VariableSet"
    event->Par1         = VarNr;                            // Par1 is de variabele die we willen vullen.
    event->Par2         = float2ul(float(temperature));
    QueueAdd(event);                                        // Event opslaan in de event queue, hierna komt de volgende meetwaarde
    event->Par1         = VarNr+1;                          // Par1+1 is de variabele die we willen vullen voor luchtvochtigheid
    event->Par2         = float2ul(float(humidity));
    QueueAdd(event);
    success=true;

    break;
    }
#endif // DEVICE_CORE_06

#if NODO_MEGA
  case DEVICE_MMI_IN:
    {
    char *TempStr=(char*)malloc(26);
    string[25]=0;

    if(GetArgv(string,TempStr,1))
      {
      if(strcasecmp(TempStr,DEVICE_NAME)==0)
        {
        if(event->Par1>0 && event->Par1<WIRED_PORTS && event->Par2>0 && event->Par2<=USER_VARIABLES_MAX-1)
          success=true;
        }
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

#ifdef DEVICE_CORE_06
/*********************************************************************************************\
 * DHT11 sub to get an 8 bit value from the receiving bitstream
 \*********************************************************************************************/
byte read_dht11_dat()
{
  byte i = 0;
  byte result=0;
  noInterrupts();
  for(i=0; i< 8; i++)
  {
    while(!digitalRead(DHT11_Pin));  // wait for 50us
    delayMicroseconds(30);
    if(digitalRead(DHT11_Pin)) 
      result |=(1<<(7-i));
    while(digitalRead(DHT11_Pin));  // wait '1' finish
  }
  interrupts();
  return result;
}
#endif //DEVICE_CORE_06
#endif //DEVICE_06


//#######################################################################################################
//############################### Generic code for all Alecto devices  ##################################
//#######################################################################################################

#if defined(DEVICE_CORE_08) || defined(DEVICE_CORE_09) || defined(DEVICE_CORE_10)
byte ProtocolAlectoValidID[5];
byte ProtocolAlectoVar[5];
unsigned int ProtocolAlectoRainBase=0;

/*********************************************************************************************\
 * Calculates CRC-8 checksum
 * reference http://lucsmall.com/2012/04/29/weather-station-hacking-part-2/
 *           http://lucsmall.com/2012/04/30/weather-station-hacking-part-3/
 *           https://github.com/lucsmall/WH2-Weather-Sensor-Library-for-Arduino/blob/master/WeatherSensorWH2.cpp
 \*********************************************************************************************/
uint8_t ProtocolAlectoCRC8( uint8_t *addr, uint8_t len)
{
  uint8_t crc = 0;
  // Indicated changes are from reference CRC-8 function in OneWire library
  while (len--) {
    uint8_t inbyte = *addr++;
    for (uint8_t i = 8; i; i--) {
      uint8_t mix = (crc ^ inbyte) & 0x80; // changed from & 0x01
      crc <<= 1; // changed from right shift
      if (mix) crc ^= 0x31;// changed from 0x8C;
      inbyte <<= 1; // changed from right shift
    }
  }
  return crc;
}

/*********************************************************************************************\
 * Check for valid sensor ID
 \*********************************************************************************************/
byte ProtocolAlectoCheckID(byte checkID)
{
  for (byte x=0; x<5; x++) if (ProtocolAlectoValidID[x] == checkID) return ProtocolAlectoVar[x];
  return 0;
}
#endif // ALECTO Basic stuff...

//#######################################################################################################
//##################################### Device-08 AlectoV1  #############################################
//#######################################################################################################

/*********************************************************************************************\
 * Dit protocol zorgt voor ontvangst van Alecto weerstation buitensensoren met protocol V1
 * 
 * Auteur             : Nodo-team (Martinus van den Broek) www.nodo-domotica.nl
 * Support            : www.nodo-domotica.nl
 * Datum              : Mrt.2013
 * Versie             : 1.0
 * Nodo productnummer : n.v.t. meegeleverd met Nodo code.
 * Compatibiliteit    : Vanaf Nodo build nummer 508
 * Syntax             : "AlectoV1 <Par1:Sensor ID>, <Par2:Basis Variabele>"
 *********************************************************************************************
 * Technische informatie:
 * Message Format: (9 nibbles, 36 bits):
 *
 * Format for Temperature Humidity
 *   AAAAAAAA BBBB CCCC CCCC CCCC DDDDDDDD EEEE
 *   RC       Type Temperature___ Humidity Checksum
 *   A = Rolling Code
 *   B = Message type (xyyx = temp/humidity if yy <> '11')
 *   C = Temperature (two's complement)
 *   D = Humidity BCD format
 *   E = Checksum
 *
 * Format for Rain
 *   AAAAAAAA BBBB CCCC DDDD DDDD DDDD DDDD EEEE
 *   RC       Type      Rain                Checksum
 *   A = Rolling Code
 *   B = Message type (xyyx = NON temp/humidity data if yy = '11')
 *   C = fixed to 1100
 *   D = Rain (bitvalue * 0.25 mm)
 *   E = Checksum
 *
 * Format for Windspeed
 *   AAAAAAAA BBBB CCCC CCCC CCCC DDDDDDDD EEEE
 *   RC       Type                Windspd  Checksum
 *   A = Rolling Code
 *   B = Message type (xyyx = NON temp/humidity data if yy = '11')
 *   C = Fixed to 1000 0000 0000
 *   D = Windspeed  (bitvalue * 0.2 m/s, correction for webapp = 3600/1000 * 0.2 * 100 = 72)
 *   E = Checksum
 *
 * Format for Winddirection & Windgust
 *   AAAAAAAA BBBB CCCD DDDD DDDD EEEEEEEE FFFF
 *   RC       Type      Winddir   Windgust Checksum
 *   A = Rolling Code
 *   B = Message type (xyyx = NON temp/humidity data if yy = '11')
 *   C = Fixed to 111
 *   D = Wind direction
 *   E = Windgust (bitvalue * 0.2 m/s, correction for webapp = 3600/1000 * 0.2 * 100 = 72)
 *   F = Checksum
 \*********************************************************************************************/
 
#ifdef DEVICE_08
#define DEVICE_ID 8
#define DEVICE_NAME "AlectoV1"

#define WS3500_PULSECOUNT 74

boolean Device_08(byte function, struct NodoEventStruct *event, char *string)
{
  boolean success=false;

  switch(function)
  {
#ifdef DEVICE_CORE_08
  case DEVICE_RAWSIGNAL_IN:
    {
      if (RawSignal.Number != WS3500_PULSECOUNT) return false;

      const byte PTMF=50;
      unsigned long bitstream=0;
      byte nibble0=0;
      byte nibble1=0;
      byte nibble2=0;
      byte nibble3=0;
      byte nibble4=0;
      byte nibble5=0;
      byte nibble6=0;
      byte nibble7=0;
      byte checksum=0;
      int temperature=0;
      byte humidity=0;
      unsigned int rain=0;
      byte windspeed=0;
      byte windgust=0;
      int winddirection=0;
      byte checksumcalc = 0;
      byte rc=0;
      byte basevar=0;

      for(byte x=2; x<=64; x=x+2)
        {
        if(RawSignal.Pulses[x]*PTMF > 0xA00) bitstream = ((bitstream >> 1) |(0x1L << 31)); 
        else bitstream = (bitstream >> 1);
        }

      for(byte x=66; x<=72; x=x+2)
        {
        if(RawSignal.Pulses[x]*PTMF > 0xA00) checksum = ((checksum >> 1) |(0x1L << 3)); 
        else checksum = (checksum >> 1);
        }

      nibble7 = (bitstream >> 28) & 0xf;
      nibble6 = (bitstream >> 24) & 0xf;
      nibble5 = (bitstream >> 20) & 0xf;
      nibble4 = (bitstream >> 16) & 0xf;
      nibble3 = (bitstream >> 12) & 0xf;
      nibble2 = (bitstream >> 8) & 0xf;
      nibble1 = (bitstream >> 4) & 0xf;
      nibble0 = bitstream & 0xf;

      // checksum calculations
      if ((nibble2 & 0x6) != 6) {
        checksumcalc = (0xf - nibble0 - nibble1 - nibble2 - nibble3 - nibble4 - nibble5 - nibble6 - nibble7) & 0xf;
      }
      else
        {
        // Alecto checksums are Rollover Checksums by design!
        if (nibble3 == 3)
          checksumcalc = (0x7 + nibble0 + nibble1 + nibble2 + nibble3 + nibble4 + nibble5 + nibble6 + nibble7) & 0xf;
        else
          checksumcalc = (0xf - nibble0 - nibble1 - nibble2 - nibble3 - nibble4 - nibble5 - nibble6 - nibble7) & 0xf;
        }

      if (checksum != checksumcalc) return false;
      rc = bitstream & 0xff;

      basevar = ProtocolAlectoCheckID(rc);

      event->Par1=rc;
      event->Par2=basevar;
      event->SourceUnit    = 0;                     // Komt niet van een Nodo unit af, dus unit op nul zetten
      event->Port          = VALUE_SOURCE_RF;

      if (basevar == 0) return true;

      if ((nibble2 & 0x6) != 6) {

        temperature = (bitstream >> 12) & 0xfff;
        //fix 12 bit signed number conversion
        if ((temperature & 0x800) == 0x800) temperature = temperature - 0x1000;
        UserVar[basevar -1] = (float)temperature/10;

        humidity = (10 * nibble7) + nibble6;
        UserVar[basevar +1 -1] = (float)humidity;
        RawSignal.Number=0;
        return true;
      }
      else
      {
        if (nibble3 == 3)
        {
          rain = ((bitstream >> 16) & 0xffff);
          // check if rain unit has been reset!
          if (rain < ProtocolAlectoRainBase) ProtocolAlectoRainBase=rain;
          if (ProtocolAlectoRainBase > 0)
          {
            UserVar[basevar +2 -1] += ((float)rain - ProtocolAlectoRainBase) * 0.25;
          }
          ProtocolAlectoRainBase = rain;
          return true;
        }

        if (nibble3 == 1)
        {
          windspeed = ((bitstream >> 24) & 0xff);
          UserVar[basevar +3 -1] = (float)windspeed * 0.72;
          RawSignal.Number=0;
          return true;
        }

        if ((nibble3 & 0x7) == 7)
        {
          winddirection = ((bitstream >> 15) & 0x1ff) / 45;
          UserVar[basevar +4 -1] = (float)winddirection;

          windgust = ((bitstream >> 24) & 0xff);
          UserVar[basevar +5 -1] = (float)windgust * 0.72;

          RawSignal.Number=0;
          return true;
        }
      }
      success = true;
      break;
    }
  case DEVICE_COMMAND:
    {
    if ((event->Par2 > 0) && (ProtocolAlectoCheckID(event->Par1) == 0))
      {
      for (byte x=0; x<5; x++)
        {
        if (ProtocolAlectoValidID[x] == 0)
          {
          ProtocolAlectoValidID[x] = event->Par1;
          ProtocolAlectoVar[x] = event->Par2;
          break;
          }
        }
      }
    break;
    }
#endif // DEVICE_CORE_08

#if NODO_MEGA
  case DEVICE_MMI_IN:
    {
    char *TempStr=(char*)malloc(26);
    string[25]=0;

    if(GetArgv(string,TempStr,1))
      {
      if(strcasecmp(TempStr,DEVICE_NAME)==0)
        {
        if(event->Par1>0 && event->Par1<255 && event->Par2>0 && event->Par2<=USER_VARIABLES_MAX)
          success=true;
        }
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
#endif //DEVICE_08


//#######################################################################################################
//##################################### Device-09 AlectoV2  #############################################
//#######################################################################################################

/*********************************************************************************************\
 * Dit protocol zorgt voor ontvangst van Alecto weerstation buitensensoren
 * 
 * Auteur             : Nodo-team (Martinus van den Broek) www.nodo-domotica.nl
 *                      Support ACH2010 en code optimalisatie door forumlid: Arendst
 * Support            : www.nodo-domotica.nl
 * Datum              : Mrt.2013
 * Versie             : 1.0
 * Nodo productnummer : n.v.t. meegeleverd met Nodo code.
 * Compatibiliteit    : Vanaf Nodo build nummer 508
 * Syntax             : "AlectoV2 <Par1:Sensor ID>, <Par2:Basis Variabele>"
 *********************************************************************************************
 * Technische informatie:
 * DKW2012 Message Format: (11 Bytes, 88 bits):
 * AAAAAAAA AAAABBBB BBBB__CC CCCCCCCC DDDDDDDD EEEEEEEE FFFFFFFF GGGGGGGG GGGGGGGG HHHHHHHH IIIIIIII
 *                         Temperature Humidity Windspd_ Windgust Rain____ ________ Winddir  Checksum
 * A = start/unknown, first 8 bits are always 11111111
 * B = Rolling code
 * C = Temperature (10 bit value with -400 base)
 * D = Humidity
 * E = windspeed (* 0.3 m/s, correction for webapp = 3600/1000 * 0.3 * 100 = 108))
 * F = windgust (* 0.3 m/s, correction for webapp = 3600/1000 * 0.3 * 100 = 108))
 * G = Rain ( * 0.3 mm)
 * H = winddirection (0 = north, 4 = east, 8 = south 12 = west)
 * I = Checksum, calculation is still under investigation
 *
 * WS3000 and ACH2010 systems have no winddirection, message format is 8 bit shorter
 * Message Format: (10 Bytes, 80 bits):
 * AAAAAAAA AAAABBBB BBBB__CC CCCCCCCC DDDDDDDD EEEEEEEE FFFFFFFF GGGGGGGG GGGGGGGG HHHHHHHH
 *                         Temperature Humidity Windspd_ Windgust Rain____ ________ Checksum
 * 
 * DCF Time Message Format: (NOT DECODED!, we already have time sync through webapp)
 * AAAAAAAA BBBBCCCC DDDDDDDD EFFFFFFF GGGGGGGG HHHHHHHH IIIIIIII JJJJJJJJ KKKKKKKK LLLLLLLL MMMMMMMM
 * 	    11                 Hours   Minutes  Seconds  Year     Month    Day      ?        Checksum
 * B = 11 = DCF
 * C = ?
 * D = ?
 * E = ?
 * F = Hours BCD format (7 bits only for this byte, MSB could be '1')
 * G = Minutes BCD format
 * H = Seconds BCD format
 * I = Year BCD format (only two digits!)
 * J = Month BCD format
 * K = Day BCD format
 * L = ?
 * M = Checksum
 \*********************************************************************************************/

#ifdef DEVICE_09
#define DEVICE_ID 9
#define DEVICE_NAME "AlectoV2"

#define DKW2012_PULSECOUNT 176
#define ACH2010_MIN_PULSECOUNT 160 // reduce this value (144?) in case of bad reception
#define ACH2010_MAX_PULSECOUNT 160

boolean Device_09(byte function, struct NodoEventStruct *event, char *string)
{
  boolean success=false;

  switch(function)
  {
#ifdef DEVICE_CORE_09
  case DEVICE_RAWSIGNAL_IN:
    {
      if (!(((RawSignal.Number >= ACH2010_MIN_PULSECOUNT) && (RawSignal.Number <= ACH2010_MAX_PULSECOUNT)) || (RawSignal.Number == DKW2012_PULSECOUNT))) return false;

      const byte PTMF=50;
      byte c=0;
      byte rfbit;
      byte data[9]; 
      byte msgtype=0;
      byte rc=0;
      unsigned int rain=0;
      byte checksum=0;
      byte checksumcalc=0;
      byte basevar;
      byte maxidx = 8;

      if(RawSignal.Number > ACH2010_MAX_PULSECOUNT) maxidx = 9;  
      // Get message back to front as the header is almost never received complete for ACH2010
      byte idx = maxidx;
      for(byte x=RawSignal.Number; x>0; x=x-2)
        {
          if(RawSignal.Pulses[x-1]*PTMF < 0x300) rfbit = 0x80; else rfbit = 0;  
          data[idx] = (data[idx] >> 1) | rfbit;
          c++;
          if (c == 8) 
          {
            if (idx == 0) break;
            c = 0;
            idx--;
          }   
        }

      checksum = data[maxidx];
      checksumcalc = ProtocolAlectoCRC8(data, maxidx);
  
      msgtype = (data[0] >> 4) & 0xf;
      rc = (data[0] << 4) | (data[1] >> 4);

      if (checksum != checksumcalc) return false;
  
      basevar = ProtocolAlectoCheckID(rc);

      event->Par1=rc;
      event->Par2=basevar;
      event->SourceUnit    = 0;                     // Komt niet van een Nodo unit af, dus unit op nul zetten
      event->Port          = VALUE_SOURCE_RF;

      if (basevar == 0) return true;
      if ((msgtype != 10) && (msgtype != 5)) return true;
  
      UserVar[basevar -1] = (float)(((data[1] & 0x3) * 256 + data[2]) - 400) / 10;
      UserVar[basevar +1 -1] = (float)data[3];

      rain = (data[6] * 256) + data[7];
      // check if rain unit has been reset!
      if (rain < ProtocolAlectoRainBase) ProtocolAlectoRainBase=rain;
      if (ProtocolAlectoRainBase > 0)
      {
        UserVar[basevar +2 -1] += ((float)rain - ProtocolAlectoRainBase) * 0.30;
      }
      ProtocolAlectoRainBase = rain;

      UserVar[basevar +3 -1] = (float)data[4] * 1.08;
      UserVar[basevar +4 -1] = (float)data[5] * 1.08;
      if (RawSignal.Number == DKW2012_PULSECOUNT) UserVar[basevar +5 -1] = (float)(data[8] & 0xf);

      success = true;
      break;
    }
  case DEVICE_COMMAND:
    {
    if ((event->Par2 > 0) && (ProtocolAlectoCheckID(event->Par1) == 0))
      {
      for (byte x=0; x<5; x++)
        {
        if (ProtocolAlectoValidID[x] == 0)
          {
            ProtocolAlectoValidID[x] = event->Par1;
            ProtocolAlectoVar[x] = event->Par2;
            break;
          }
        }
      }
    break;
    }
#endif // DEVICE_CORE_09

#if NODO_MEGA
  case DEVICE_MMI_IN:
    {
    char *TempStr=(char*)malloc(26);
    string[25]=0;

    if(GetArgv(string,TempStr,1))
      {
      if(strcasecmp(TempStr,DEVICE_NAME)==0)
        {
        if(event->Par1>0 && event->Par1<255 && event->Par2>0 && event->Par2<=USER_VARIABLES_MAX)
          success=true;
        }
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
#endif //DEVICE_09


//#######################################################################################################
//##################################### Device-10 AlectoV3  #############################################
//#######################################################################################################

/*********************************************************************************************\
 * Dit protocol zorgt voor ontvangst van Alecto weerstation buitensensoren
 * 
 * Auteur             : Nodo-team (Martinus van den Broek) www.nodo-domotica.nl
 *                      Support WS1100 door forumlid: Arendst
 * Support            : www.nodo-domotica.nl
 * Datum              : Mrt.2013
 * Versie             : 1.0
 * Nodo productnummer : n.v.t. meegeleverd met Nodo code.
 * Compatibiliteit    : Vanaf Nodo build nummer 508
 * Syntax             : "AlectoV3 <Par1:Sensor ID>, <Par2:Basis Variabele>"
 *********************************************************************************************
 * Technische informatie:
 * Decodes signals from Alecto Weatherstation outdoor unit, type 3 (94/126 pulses, 47/63 bits, 433 MHz).
 * WS1100 Message Format: (7 bits preamble, 5 Bytes, 40 bits):
 * AAAAAAA AAAABBBB BBBB__CC CCCCCCCC DDDDDDDD EEEEEEEE
 *                        Temperature Humidity Checksum
 * A = start/unknown, first 8 bits are always 11111111
 * B = Rolling code
 * C = Temperature (10 bit value with -400 base)
 * D = Checksum
 * E = Humidity
 *
 * WS1200 Message Format: (7 bits preamble, 7 Bytes, 56 bits):
 * AAAAAAA AAAABBBB BBBB__CC CCCCCCCC DDDDDDDD DDDDDDDD EEEEEEEE FFFFFFFF 
 *                        Temperature Rain LSB Rain MSB ???????? Checksum
 * A = start/unknown, first 8 bits are always 11111111
 * B = Rolling code
 * C = Temperature (10 bit value with -400 base)
 * D = Rain ( * 0.3 mm)
 * E = ?
 * F = Checksum
 \*********************************************************************************************/
 
#ifdef DEVICE_10
#define DEVICE_ID 10
#define DEVICE_NAME "AlectoV3"

#define WS1100_PULSECOUNT 94
#define WS1200_PULSECOUNT 126

boolean Device_10(byte function, struct NodoEventStruct *event, char *string)
{
  boolean success=false;

  switch(function)
  {
#ifdef DEVICE_CORE_10
  case DEVICE_RAWSIGNAL_IN:
    {
      if ((RawSignal.Number != WS1100_PULSECOUNT) && (RawSignal.Number != WS1200_PULSECOUNT)) return false;

      RawSignal.Multiply=50;
      unsigned long bitstream1=0;
      unsigned long bitstream2=0;
      byte rc=0;
      int temperature=0;
      byte humidity=0;
      unsigned int rain=0;
      byte checksum=0;
      byte checksumcalc=0;
      byte basevar=0;
      byte data[6];

      // get first 32 relevant bits
      for(byte x=15; x<=77; x=x+2) if(RawSignal.Pulses[x]*RawSignal.Multiply < 0x300) bitstream1 = (bitstream1 << 1) | 0x1; 
      else bitstream1 = (bitstream1 << 1);
      // get second 32 relevant bits
      for(byte x=79; x<=141; x=x+2) if(RawSignal.Pulses[x]*RawSignal.Multiply < 0x300) bitstream2 = (bitstream2 << 1) | 0x1; 
      else bitstream2 = (bitstream2 << 1);

      data[0] = (bitstream1 >> 24) & 0xff;
      data[1] = (bitstream1 >> 16) & 0xff;
      data[2] = (bitstream1 >>  8) & 0xff;
      data[3] = (bitstream1 >>  0) & 0xff;
      data[4] = (bitstream2 >> 24) & 0xff;
      data[5] = (bitstream2 >> 16) & 0xff;

      if (RawSignal.Number == WS1200_PULSECOUNT)
      {
        checksum = (bitstream2 >> 8) & 0xff;
        checksumcalc = ProtocolAlectoCRC8(data, 6);
      }
      else
      {
        checksum = (bitstream2 >> 24) & 0xff;
        checksumcalc = ProtocolAlectoCRC8(data, 4);
      }

      rc = (bitstream1 >> 20) & 0xff;

      if (checksum != checksumcalc) return false;
      basevar = ProtocolAlectoCheckID(rc);

      event->Par1=rc;
      event->Par2=basevar;
      event->SourceUnit    = 0;                     // Komt niet van een Nodo unit af, dus unit op nul zetten
      event->Port          = VALUE_SOURCE_RF;

      if (basevar == 0) return true;

      temperature = ((bitstream1 >> 8) & 0x3ff) - 400;
      UserVar[basevar-1] = (float)temperature / 10;

      if (RawSignal.Number == WS1200_PULSECOUNT)
      {
        rain = (((bitstream2 >> 24) & 0xff) * 256) + ((bitstream1 >> 0) & 0xff);
        // check if rain unit has been reset!
        if (rain < ProtocolAlectoRainBase) ProtocolAlectoRainBase=rain;
        if (ProtocolAlectoRainBase > 0)
        {
          UserVar[basevar+1 -1] += ((float)rain - ProtocolAlectoRainBase) * 0.30;
        }
        ProtocolAlectoRainBase = rain;
      }
      else
      {
        humidity = bitstream1 & 0xff;
        UserVar[basevar+1 -1] = (float)humidity / 10;
      }

      RawSignal.Number=0;
      success = true;
      break;
    }
  case DEVICE_COMMAND:
    {
    if ((event->Par2 > 0) && (ProtocolAlectoCheckID(event->Par1) == 0))
      {
      for (byte x=0; x<5; x++)
        {
        if (ProtocolAlectoValidID[x] == 0)
          {
          ProtocolAlectoValidID[x] = event->Par1;
          ProtocolAlectoVar[x] = event->Par2;
          break;
          }
        }
      }
    break;
    }
#endif // DEVICE_CORE_10

#if NODO_MEGA
  case DEVICE_MMI_IN:
    {
    char *TempStr=(char*)malloc(26);
    string[25]=0;

    if(GetArgv(string,TempStr,1))
      {
      if(strcasecmp(TempStr,DEVICE_NAME)==0)
        {
        if(event->Par1>0 && event->Par1<255 && event->Par2>0 && event->Par2<=USER_VARIABLES_MAX)
          success=true;
        }
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
#endif //DEVICE_10


//#######################################################################################################
//##################################### Device-12 OregonV2  #############################################
//#######################################################################################################

/*********************************************************************************************\
 * Dit protocol zorgt voor ontvangst van Oregon buitensensoren
 * 
 * Auteur             : Nodo-team (Martinus van den Broek) www.nodo-domotica.nl
 *                      Support THGN132N en code optimalisatie door forumlid: Arendst
 * Support            : www.nodo-domotica.nl
 * Datum              : Mrt.2013
 * Versie             : 1.0
 * Nodo productnummer : n.v.t. meegeleverd met Nodo code.
 * Compatibiliteit    : Vanaf Nodo build nummer 508
 * Syntax             : "OregonV2 <Par1:Sensor ID>, <Par2:Basis Variabele>"
 *********************************************************************************************
 * Technische informatie:
 * Only supports Oregon V2 protocol messages, message format consists of 18 or 21 nibbles:
 * THN132
 * AAAA AAAA AAAA AAAA BBBB CCCC CCCC CCCC CCCC DDDD EEEE EEEE FFFF GGGG GGGG GGGG HHHH IIII IIII
 *                          0    1    2    3    4    5    6    7    8    9    10   11   12   13
 *   A = preamble, B = sync bits, C = ID, D = Channel, E = RC, F = Flags,
 *   G = Measured value, 3 digits BCD encoded, H = sign, bit3 set if negative temperature
 *   I = Checksum, sum of nibbles C,D,E,F,G,H
 *
 * THGN132
 * AAAA AAAA AAAA AAAA BBBB CCCC CCCC CCCC CCCC DDDD EEEE EEEE FFFF GGGG GGGG GGGG HHHH IIII IIII JJJJ KKKK KKKK
 *                          0    1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16
 *   A = preamble, B = sync bits, C = ID, D = Channel, E = RC, F = Flags,
 *   G = Measured value, 3 digits BCD encoded, H = sign, bit3 set if negative temperature
 *   I = Humidity value
 *   J = Unknown
 *   K = Checksum, sum of nibbles C,D,E,F,G,H,I,J
 **********************************************************************************************/
 
#ifdef DEVICE_12
#define DEVICE_ID 12
#define DEVICE_NAME   "OregonV2"

#define THN132N_ID              1230
#define THGN123N_ID              721
#define THGR810_ID             17039
#define THN132N_MIN_PULSECOUNT   196
#define THN132N_MAX_PULSECOUNT   205
#define THGN123N_MIN_PULSECOUNT  228
#define THGN123N_MAX_PULSECOUNT  238

byte ProtocolOregonValidID[5];
byte ProtocolOregonVar[5];

boolean Device_12(byte function, struct NodoEventStruct *event, char *string)
{
  boolean success=false;

  switch(function)
  {
#ifdef DEVICE_CORE_12
  case DEVICE_RAWSIGNAL_IN:
    {
      RawSignal.Multiply=50;
      byte nibble[17]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
      byte y = 1;
      byte c = 1;
      byte rfbit = 1;
      byte sync = 0; 
      int id = 0;
      byte checksum = 0;
      byte checksumcalc = 0;
      int datavalue = 0;
      byte basevar=0;

      if (!((RawSignal.Number >= THN132N_MIN_PULSECOUNT && RawSignal.Number <= THN132N_MAX_PULSECOUNT) || (RawSignal.Number >= THGN123N_MIN_PULSECOUNT && RawSignal.Number <= THGN123N_MAX_PULSECOUNT))) return false;

      for(byte x=1;x<=RawSignal.Number;x++)
      {
        if(RawSignal.Pulses[x]*RawSignal.Multiply < 600)
        {
          rfbit = (RawSignal.Pulses[x]*RawSignal.Multiply < RawSignal.Pulses[x+1]*RawSignal.Multiply);
          x++;
          y = 2;
        }
        if (y%2 == 1)
        {
          // Find sync pattern as THN132N and THGN132N have different preamble length
          if (c == 1)
          {
            sync = (sync >> 1) | (rfbit << 3);
            sync = sync & 0xf;
            if (sync == 0xA) 
            {
              c = 2;
              if (x < 40) return false;
            }
          }
          else
          {
            if (c < 70) nibble[(c-2)/4] = (nibble[(c-2)/4] >> 1) | rfbit << 3;
            c++;
          }
        }
        y++;
      }
      // if no sync pattern match found, return
      if (c == 1) return false;
      
      // calculate sensor ID
      id = (nibble[3] << 16) |(nibble[2] << 8) | (nibble[1] << 4) | nibble[0];
 
      // calculate and verify checksum
      for(byte x=0; x<12;x++) checksumcalc += nibble[x];
      checksum = (nibble[13] << 4) | nibble[12];
      if ((id == THGN123N_ID) || (id == THGR810_ID))                           // Units with humidity sensor have extra data
        {
          checksum = (nibble[16] << 4) | nibble[15];
          for(byte x=13; x<16;x++) checksumcalc += nibble[x];
        }
      if (checksum != checksumcalc) return false;

      basevar = ProtocolOregonCheckID((nibble[6] << 4) | nibble[5]);

      event->Par1=(nibble[6] << 4) | nibble[5];
      event->Par2=basevar;
      event->SourceUnit    = 0;                     // Komt niet van een Nodo unit af, dus unit op nul zetten
      event->Port          = VALUE_SOURCE_RF;

      if (basevar == 0) return true;

      // if valid sensor type, update user variable and process event
      if ((id == THGN123N_ID) || (id == THGR810_ID) || (id == THN132N_ID))
      {
        datavalue = ((1000 * nibble[10]) + (100 * nibble[9]) + (10 * nibble[8]));
        if ((nibble[11] & 0x8) == 8) datavalue = -1 * datavalue;
        UserVar[basevar -1] = (float)datavalue/100;
        if ((id == THGN123N_ID) || (id == THGR810_ID))
        {
          datavalue = ((1000 * nibble[13]) + (100 * nibble[12]));
          UserVar[basevar + 1 -1] = (float)datavalue/100;
        }
      }
      success = true;
      break;
    }
  case DEVICE_COMMAND:
    {
    if ((event->Par2 > 0) && (ProtocolOregonCheckID(event->Par1) == 0))
      {
      for (byte x=0; x<5; x++)
        {
        if (ProtocolOregonValidID[x] == 0)
          {
          ProtocolOregonValidID[x] = event->Par1;
          ProtocolOregonVar[x] = event->Par2;
          break;
          }
        }
      }
    break;
    }
#endif // DEVICE_CORE_12

#if NODO_MEGA
  case DEVICE_MMI_IN:
    {
    char *TempStr=(char*)malloc(26);
    string[25]=0;

    if(GetArgv(string,TempStr,1))
      {
      if(strcasecmp(TempStr,DEVICE_NAME)==0)
        {
        if(event->Par1>0 && event->Par1<255 && event->Par2>0 && event->Par2<=USER_VARIABLES_MAX)
          success=true;
        }
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

#ifdef DEVICE_CORE_12
/*********************************************************************************************\
 * Check for valid sensor ID
 \*********************************************************************************************/
byte ProtocolOregonCheckID(byte checkID)
{
  for (byte x=0; x<5; x++) if (ProtocolOregonValidID[x] == checkID) return ProtocolOregonVar[x];
  return 0;
}
#endif //DEVICE_CORE_12
#endif //DEVICE_12


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

#ifdef DEVICE_13
#define DEVICE_ID 13
#define DEVICE_NAME "SmokeAlert"

boolean Device_13(byte function, struct NodoEventStruct *event, char *string)
{
  boolean success=false;

  switch(function)
  {
#ifdef DEVICE_CORE_13
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
#endif // DEVICE_CORE_13

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

#ifdef DEVICE_14
#define DEVICE_ID 14
#define DEVICE_NAME "SmokeAlertSend"

boolean Device_14(byte function, struct NodoEventStruct *event, char *string)
  {
  boolean success=false;

  switch(function)
  {
#ifdef DEVICE_CORE_14
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
#endif // DEVICE_CORE_14

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


//#######################################################################################################
//#################################### Device-15: HomeEasy EU ###########################################
//#######################################################################################################

/*********************************************************************************************\
 * Dit protocol zorgt voor ontvangst HomeEasy EU zenders
 * die werken volgens de automatische codering (Ontvangers met leer-knop)
 *
 * LET OP: GEEN SUPPORT VOOR DIRECTE DIMWAARDES!!!
 *
 * Auteur             : Nodo-team (Martinus van den Broek) www.nodo-domotca.nl
 * Support            : www.nodo-domotica.nl
 * Datum              : Mrt.2013
 * Versie             : 1.0
 * Nodo productnummer : n.v.t. meegeleverd met Nodo code.
 * Compatibiliteit    : Vanaf Nodo build nummer 508
 * Syntax             : "HomeEasy <Adres>,<On|Off|>
 *********************************************************************************************
 * Technische informatie:
 * Analyses Home Easy Messages and convert these into NewKaku compatible eventcode
 * Only new EU devices with automatic code system are supported
 * Only  On / Off status is decoded, no DIM values
 * Only tested with Home Easy HE300WEU transmitter, doorsensor and PIR sensor
 * Home Easy message structure, by analyzing bitpatterns so far ...
 * AAAAAAAAAAABBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBCCCCDDEEFFFFFFG
 *       A = startbits/preamble, B = address, C = ?, D = Command, E = ?, F = Channel, G = Stopbit
 \*********************************************************************************************/
 
#define HomeEasy_LongLow    0x490    // us
#define HomeEasy_ShortHigh  200      // us
#define HomeEasy_ShortLow   150      // us

#ifdef DEVICE_15
#define DEVICE_ID 15
#define DEVICE_NAME "HomeEasy"

boolean Device_15(byte function, struct NodoEventStruct *event, char *string)
{
  boolean success=false;

  switch(function)
  {
#ifdef DEVICE_CORE_15
  case DEVICE_RAWSIGNAL_IN:
    {
      unsigned long address = 0;
      unsigned long bitstream = 0;
      int counter = 0;
      byte rfbit =0;
      byte state = 0;
      unsigned long channel = 0;
      RawSignal.Multiply=50;

      // valid messages are 116 pulses          
      if (RawSignal.Number != 116) return false;

      for(byte x=1;x<=RawSignal.Number;x=x+2)
      {
        if ((RawSignal.Pulses[x]*RawSignal.Multiply < 500) & (RawSignal.Pulses[x+1]*RawSignal.Multiply > 500)) 
          rfbit = 1;
        else
          rfbit = 0;

        if ((x>=23) && (x<=86)) address = (address << 1) | rfbit;
        if ((x>=87) && (x<=114)) bitstream = (bitstream << 1) | rfbit;

      }
      state = (bitstream >> 8) & 0x3;
      channel = (bitstream) & 0x3f;

      // Add channel info to base address, first shift channel info 6 positions, so it can't interfere with bit 5
      channel = channel << 6;
      address = address + channel;

      // Set bit 5 based on command information in the Home Easy protocol
      if (state == 1) address = address & 0xFFFFFEF;
      else address = address | 0x00000010;

      event->Par1=((address>>4)&0x01)?VALUE_ON:VALUE_OFF; // On/Off bit omzetten naar een Nodo waarde. 
      event->Par2=address &0x0FFFFFCF;         // Op hoogste nibble zat vroeger het signaaltype. 
      event->SourceUnit    = 0;                     // Komt niet van een Nodo unit af, dus unit op nul zetten
      RawSignal.Repeats    = true; // het is een herhalend signaal. Bij ontvangst herhalingen onderdrukken.
      
      return true;      
      break;
    }

  case DEVICE_COMMAND:
    break;
#endif // DEVICE_CORE_15

#if NODO_MEGA
  case DEVICE_MMI_IN:
    {
    char* str=(char*)malloc(40);
    string[25]=0; // kap voor de zekerheid de string af.

    if(GetArgv(string,str,1))
      {
      if(strcasecmp(str,DEVICE_NAME)==0)
        {
        if(GetArgv(string,str,2))
          {
          event->Par2=str2int(str);    
          if(GetArgv(string,str,3))
            {
            // Vul Par1 met het HomeEasy commando. Dit kan zijn: VALUE_ON, VALUE_OFF, Andere waarden zijn ongeldig.

            // haal uit de tweede parameter een 'On' of een 'Off'.
            if(event->Par1=str2cmd(str))
              success=true;
            }
          }
        }
      }
      free(str);
      break;
    }

  case DEVICE_MMI_OUT:
    {
    strcpy(string,DEVICE_NAME);            // Eerste argument=het commando deel
    strcat(string," ");

    // In Par3 twee mogelijkheden: Het bevat een door gebruiker ingegeven adres 0..255 of een volledig HomeEasy adres.
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
#endif //NODO_MEGA
  }      
  return success;
}
#endif //DEVICE_15


//#######################################################################################################
//#################################### Device-16: SendHome Easy EU ######################################
//#######################################################################################################

/*********************************************************************************************\
 * Dit protocol zorgt voor aansturing van Home Easy EU ontvangers
 * die werken volgens de automatische codering (Ontvangers met leer-knop)
 *
 * LET OP: GEEN SUPPORT VOOR DIRECTE DIMWAARDES!!!
 * 
 * Auteur             : Nodo-team (Martinus van den Broek) www.nodo-domotca.nl
 * Support            : www.nodo-domotica.nl
 * Datum              : Mrt.2013
 * Versie             : 1.0
 * Nodo productnummer : n.v.t. meegeleverd met Nodo code.
 * Compatibiliteit    : Vanaf Nodo build nummer 508
 * Syntax             : "HomeEasySend <Adres>,<On|Off|>
 \*********************************************************************************************/

#ifdef DEVICE_16
#define DEVICE_ID 16
#define DEVICE_NAME "HomeEasySend"

boolean Device_16(byte function, struct NodoEventStruct *event, char *string)
{
  boolean success=false;

  switch(function)
  {
#ifdef DEVICE_CORE_16
  case DEVICE_RAWSIGNAL_IN:
    break;

  case DEVICE_COMMAND:
    {
    unsigned long bitstream=0L;
    byte address = 0;
    byte channel = 0;
    byte channelcode = 0;
    byte command = 0;
    byte i=1; // bitcounter in stream
    byte y; // size of partial bitstreams

    RawSignal.Multiply=50;

    address = (event->Par2 >> 4) & 0x7;   // 3 bits address (higher bits from HomeEasy address, bit 7 not used
    channel = event->Par2 & 0xF;    // 4 bits channel (lower bits from HomeEasy address
    command = event->Par1 & 0xF;    // 12 = on, 0 = off

    if (channel == 0) channelcode = 0x8E;
    else if (channel == 1) channelcode = 0x96;
    else if (channel == 2) channelcode = 0x9A;
    else if (channel == 3) channelcode = 0x9C;
    else if (channel == 4) channelcode = 0xA6;
    else if (channel == 5) channelcode = 0xAA;
    else if (channel == 6) channelcode = 0xAC;
    else if (channel == 7) channelcode = 0xB2;
    else if (channel == 8) channelcode = 0xB4;
    else if (channel == 9) channelcode = 0xB8;
    else if (channel == 10) channelcode = 0xC6;
    else if (channel == 11) channelcode = 0xCA;
    else if (channel == 12) channelcode = 0xCC;
    else if (channel == 13) channelcode = 0xD2;
    else if (channel == 14) channelcode = 0xD4;
    else if (channel == 15) channelcode = 0xD8;

    y=11; // bit uit de bitstream, startbits
    bitstream = 0x63C;
    for (i=1;i<=22;i=i+2)
      {
      RawSignal.Pulses[i] = HomeEasy_ShortHigh/RawSignal.Multiply;
      if((bitstream>>(y-1))&1)          // bit 1
          RawSignal.Pulses[i+1] = HomeEasy_LongLow/RawSignal.Multiply;
      else                              // bit 0
      RawSignal.Pulses[i+1] = HomeEasy_ShortLow/RawSignal.Multiply;
      y--;
      }

    y=32; // bit uit de bitstream, address
    bitstream = 0xDAB8F56C + address;

    for (i=23;i<=86;i=i+2)
      {
      RawSignal.Pulses[i] = HomeEasy_ShortHigh/RawSignal.Multiply;
      if((bitstream>>(y-1))&1)          // bit 1
          RawSignal.Pulses[i+1] = HomeEasy_LongLow/RawSignal.Multiply;
      else                              // bit 0
      RawSignal.Pulses[i+1] = HomeEasy_ShortLow/RawSignal.Multiply;
      y--;
      }

    y=15; // bit uit de bitstream, other stuff

    bitstream = 0x5C00;  // bit 10 on, bit 11 off indien OFF
    if (event->Par1==VALUE_OFF) bitstream = 0x5A00;

    bitstream = bitstream + channelcode;

    for (i=87;i<=116;i=i+2)
      {
      RawSignal.Pulses[i] = HomeEasy_ShortHigh/RawSignal.Multiply;
      if((bitstream>>(y-1))&1)          // bit 1
          RawSignal.Pulses[i+1] = HomeEasy_LongLow/RawSignal.Multiply;
      else                              // bit 0
      RawSignal.Pulses[i+1] = HomeEasy_ShortLow/RawSignal.Multiply;
      y--;
      }

    RawSignal.Pulses[116]=0;
    RawSignal.Number=116; // aantal bits*2 die zich in het opgebouwde RawSignal bevinden  unsigned long bitstream=0L;
    event->Port=VALUE_ALL; // Signaal mag naar alle door de gebruiker met [Output] ingestelde poorten worden verzonden.
    RawSignal.Repeats=5;   // vijf herhalingen.
    RawSignal.Delay=20; // Tussen iedere pulsenreeks enige tijd rust.
    SendEvent(event,true,true,true);
    success=true;
    break;
    }
#endif // DEVICE_CORE_16

#if NODO_MEGA
  case DEVICE_MMI_IN:
    {
    char* str=(char*)malloc(40);
    string[25]=0; // kap voor de zekerheid de string af.

    if(GetArgv(string,str,1))
      {
      if(strcasecmp(str,DEVICE_NAME)==0)
        {
        if(GetArgv(string,str,2))
          {
            event->Par2=str2int(str);    
            if(GetArgv(string,str,3))
            {
              // Vul Par1 met het HomeEasy commando. Dit kan zijn: VALUE_ON, VALUE_OFF, Andere waarden zijn ongeldig.
              // haal uit de tweede parameter een 'On' of een 'Off'.
              if(event->Par1=str2cmd(str))
                success=true;
            }
          }
        }
      }
      free(str);
      break;
    }

  case DEVICE_MMI_OUT:
    {
    strcpy(string,DEVICE_NAME);            // Eerste argument=het commando deel
    strcat(string," ");

    // In Par3 twee mogelijkheden: Het bevat een door gebruiker ingegeven adres 0..255 of een volledig HomeEasy adres.
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
#endif //NODO_MEGA
  }      
  return success;
}
#endif //DEVICE_16



//#######################################################################################################
//#################################### Device-18: Signal Analyzer   #####################################
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
 
#ifdef DEVICE_18

#define DEVICE_ID 18
#define DEVICE_NAME "RawSignalAnalyze"

boolean Device_18(byte function, struct NodoEventStruct *event, char *string)
  {
  boolean success=false;
  
  switch(function)
    {
    #ifdef DEVICE_CORE_18
    case DEVICE_RAWSIGNAL_IN:
      {
      return false;
      }      

    case DEVICE_COMMAND:
      {
      if(RawSignal.Number<8)return false;
      
      int x;
      unsigned int y,z;
    
      // zoek naar de langste kortst puls en de kortste lange puls
      unsigned int MarkShort=50000;
      unsigned int MarkLong=0;
      for(x=5;x<RawSignal.Number;x+=2)
        {
        y=RawSignal.Pulses[x]*RawSignal.Multiply;
        if(y<MarkShort)
          MarkShort=y;
        if(y>MarkLong)
          MarkLong=y;
        }
      z=true;
      while(z)
        {
        z=false;
        for(x=5;x<RawSignal.Number;x+=2)
          {
          y=RawSignal.Pulses[x]*RawSignal.Multiply;
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
      for(x=4;x<RawSignal.Number;x+=2)
        {
        y=RawSignal.Pulses[x]*RawSignal.Multiply;
        if(y<SpaceShort)
          SpaceShort=y;
        if(y>SpaceLong)
          SpaceLong=y;
        }
      z=true;
      while(z)
        {
        z=false;
        for(x=4;x<RawSignal.Number;x+=2)
          {
          y=RawSignal.Pulses[x]*RawSignal.Multiply;
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
          y=RawSignal.Pulses[x]*RawSignal.Multiply;
          if(y>MarkMid)
            Serial.write('1');
          else
            Serial.write('0');
          }
        Serial.print(F(", Type=PWM"));
        }
      if(y==2)
        {
        for(x=2;x<RawSignal.Number;x+=2)
          {
          y=RawSignal.Pulses[x]*RawSignal.Multiply;
          if(y>SpaceMid)
            Serial.write('1');
          else
            Serial.write('0');
          }
        Serial.print(F(", Type=PDM"));
        }
      if(y==3)
        {
        for(x=1;x<RawSignal.Number;x+=2)
          {
          y=RawSignal.Pulses[x]*RawSignal.Multiply;
          if(y>MarkMid)
            Serial.write('1');
          else
            Serial.write('0');
          
          y=RawSignal.Pulses[x+1]*RawSignal.Multiply;
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
        Serial.print(RawSignal.Pulses[x]*RawSignal.Multiply); 
        Serial.write(',');       
        }
      Serial.println();
      
//      int dev=250;  
//      for(x=1;x<=RawSignal.Number;x+=2)
//        {
//        for(y=1+int(RawSignal.Pulses[x])*RawSignal.Multiply/dev; y;y--)
//          Serial.write('M');  // Mark  
//        for(y=1+int(RawSignal.Pulses[x+1])*RawSignal.Multiply/dev; y;y--)
//          Serial.write('_');  // Space  
//        }    
//      Serial.println();

      break;
      }      
    #endif // CORE
      
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
      break;
      }
    #endif //NODO_MEGA
    }      
  return success;
  }
#endif //DEVICE_18

//#######################################################################################################
//#################################### Device-19: ID-12 RFID Tag Reader #################################
//#######################################################################################################

/*********************************************************************************************\
 * Dit protocol zorgt voor verwerking van ID-12 RFID Tag Readers
 * 
 * Auteur             : Martinus van den Broek
 * Support            : www.nodo-domotica.nl
 * Datum              : Mrt.2013
 * Versie             : 1.0
 * Nodo productnummer : 
 * Compatibiliteit    : Vanaf Nodo build nummer 508
 * Syntax             : "RFID <customer ID>,<tag ID>
 ***********************************************************************************************
 * Technische beschrijving:
 *
 * De ID-12 Tag Reader is een device die 125KHz EM4001 compatible tags kan uitlezen. De data wordt serieel aangeboden
 * baudrate is 9600 BAUD
 * We maken gebruik van een analoge input die we als digitale input gebruiken
 * Op de ATMEGA328P platformen gebruiken we poort A3
 * Op de ATMEGA2560 platformen gebruiken we poort A15
 * Elke EM4001 tag levert een unieke code, bestaande uit een 8 bits customer of version ID en een 32 bits serienummer
 * Par1 bevat het customer ID
 * Par2 bevat het 32 bits serienummer
 * Dit device genereert een event, zodat actie kan worden ondernomen via de Nodo eventlist indien een bekende tag wordt gebruikt
 \*********************************************************************************************/
#ifdef DEVICE_19
#define DEVICE_ID       19
#define DEVICE_NAME "RFID"

#if NODO_MEGA
  #define RFID_PIN         A15 // A0-A7 are not PCINT capable on a ATMega2560!
#else
  #define RFID_PIN         A3
#endif

#include <avr/interrupt.h>
#define _SS_MAX_RX_BUFF       16 // RX buffer size
#define RX_DELAY_CENTERING   114
#define RX_DELAY_INTRABIT    236
#define RX_DELAY_STOPBIT     236

#ifdef DEVICE_CORE_19
uint16_t _buffer_overflow;
uint8_t _receivePin=RFID_PIN;
uint8_t _receiveBitMask;
volatile uint8_t *_receivePortRegister;
char _receive_buffer[_SS_MAX_RX_BUFF]; 
volatile uint8_t _receive_buffer_tail = 0;
volatile uint8_t _receive_buffer_head = 0;
#endif

boolean Device_19(byte function, struct NodoEventStruct *event, char *string)
{
  boolean success=false;

  switch(function)
  {
#ifdef DEVICE_CORE_19
  case DEVICE_INIT:
    {
      // Init IO pin
      pinMode(_receivePin, INPUT);
      digitalWrite(_receivePin, HIGH);
      _receivePin = _receivePin;
      _receiveBitMask = digitalPinToBitMask(_receivePin);
      uint8_t port = digitalPinToPort(_receivePin);
      _receivePortRegister = portInputRegister(port);
  
      if (digitalPinToPCICR(_receivePin))
        {
          *digitalPinToPCICR(_receivePin) |= _BV(digitalPinToPCICRbit(_receivePin));
          *digitalPinToPCMSK(_receivePin) |= _BV(digitalPinToPCMSKbit(_receivePin));
        }
      break;
    }

  case DEVICE_ONCE_A_SECOND:
    {
      if(rfid_available() > 0)
        {
          byte val = 0;
          byte code[6];
          byte checksum = 0;
          byte bytesread = 0;
          byte tempbyte = 0;

          if((val = rfid_read()) == 2)
            {                  // check for header 
              bytesread = 0; 
              while (bytesread < 12) {                        // read 10 digit code + 2 digit checksum
              if( rfid_available() > 0) { 
                val = rfid_read();
                if((val == 0x0D)||(val == 0x0A)||(val == 0x03)||(val == 0x02)) {
                  // if header or stop bytes before the 10 digit reading 
                  break;
                 }

               // Do Ascii/Hex conversion:
              if ((val >= '0') && (val <= '9')) {
                val = val - '0';
              } 
              else if ((val >= 'A') && (val <= 'F')) {
                val = 10 + val - 'A';
              }

              // Every two hex-digits, add byte to code:
              if (bytesread & 1 == 1) {
                // make some space for this hex-digit by
                // shifting the previous hex-digit with 4 bits to the left:
                code[bytesread >> 1] = (val | (tempbyte << 4));

                if (bytesread >> 1 != 5) {                // If we're at the checksum byte,
                  checksum ^= code[bytesread >> 1];       // Calculate the checksum... (XOR)
                  };
                } 
                else {
                  tempbyte = val;                           // Store the first hex digit first...
                };
              bytesread++;                                // ready to read next digit
          } 
        }
      }

      // Output to Serial:
      if (bytesread == 12)
        {
          if (code[5]==checksum)
            {
              struct NodoEventStruct TempEvent;
              ClearEvent(&TempEvent);
              TempEvent.SourceUnit = 0;
              TempEvent.Direction  = VALUE_DIRECTION_OUTPUT;
              TempEvent.Command    = CMD_DEVICE_FIRST+DEVICE_ID;
              TempEvent.Port       = VALUE_ALL;
              TempEvent.Par1       = code[0];
              TempEvent.Par2       = 0;
              for (byte i=1; i<5; i++) TempEvent.Par2 = TempEvent.Par2 | (((unsigned long) code[i] << ((4-i)*8)));
              ProcessEvent2(&TempEvent);
            }
        }

      } // available 
      break;
    }
    
  case DEVICE_RAWSIGNAL_IN:
    {
      break;
    }

  case DEVICE_COMMAND:
    {
      break;
    }
#endif // DEVICE_CORE_19

#if NODO_MEGA
  case DEVICE_MMI_IN:
    {
    char* str=(char*)malloc(40);
    string[25]=0; // kap voor de zekerheid de string af.
  
    if(GetArgv(string,str,1))
      {
      if(strcasecmp(str,DEVICE_NAME)==0)
        {
        if(GetArgv(string,str,2))
          {
          event->Par1=str2int(str);    
          if(GetArgv(string,str,3))
            {
            event->Par2=str2int(str);    
            success=true;
            }
          }
        }
      }
    free(str);
    break;
    }

  case DEVICE_MMI_OUT:
    {
    strcpy(string,DEVICE_NAME);            // Eerste argument=het commando deel
    strcat(string," ");
    strcat(string,int2str(event->Par1)); 
    strcat(string,",");
    strcat(string,int2strhex(event->Par2));

    break;
    }
#endif //NODO_MEGA
  }      
  return success;
}
#ifdef DEVICE_CORE_19
/*********************************************************************/
inline void rfidDelay(uint16_t delay) { 
/*********************************************************************/
  uint8_t tmp=0;

  asm volatile("sbiw    %0, 0x01 \n\t"
    "ldi %1, 0xFF \n\t"
    "cpi %A0, 0xFF \n\t"
    "cpc %B0, %1 \n\t"
    "brne .-10 \n\t"
    : "+r" (delay), "+a" (tmp)
    : "0" (delay)
    );
}

/*********************************************************************/
void rfid_recv()
/*********************************************************************/
{
  uint8_t d = 0;

  // If RX line is high, then we don't see any start bit, so interrupt is probably not for us
  if (!rfid_rx_pin_read())
  {
    // Wait approximately 1/2 of a bit width to "center" the sample
    rfidDelay(RX_DELAY_CENTERING);

    // Read each of the 8 bits
    for (uint8_t i=0x1; i; i <<= 1)
    {
      rfidDelay(RX_DELAY_INTRABIT);
      uint8_t noti = ~i;
      if (rfid_rx_pin_read())
        d |= i;
      else // else clause added to ensure function timing is ~balanced
        d &= noti;
    }

    // skip the stop bit
    rfidDelay(RX_DELAY_STOPBIT);

    // if buffer full, set the overflow flag and return
    if ((_receive_buffer_tail + 1) % _SS_MAX_RX_BUFF != _receive_buffer_head) 
    {
      // save new data in buffer: tail points to where byte goes
      _receive_buffer[_receive_buffer_tail] = d; // save new byte
      _receive_buffer_tail = (_receive_buffer_tail + 1) % _SS_MAX_RX_BUFF;
    } 
    else 
    {
      _buffer_overflow = true;
    }
  }
}

/*********************************************************************/
uint8_t rfid_rx_pin_read()
/*********************************************************************/
{
  return *_receivePortRegister & _receiveBitMask;
}

/*********************************************************************/
int rfid_read()
/*********************************************************************/
{
  // Empty buffer?
  if (_receive_buffer_head == _receive_buffer_tail)
    return -1;

  // Read from "head"
  uint8_t d = _receive_buffer[_receive_buffer_head]; // grab next byte
  _receive_buffer_head = (_receive_buffer_head + 1) % _SS_MAX_RX_BUFF;
  return d;
}

/*********************************************************************/
int rfid_available()
/*********************************************************************/
{
  return (_receive_buffer_tail + _SS_MAX_RX_BUFF - _receive_buffer_head) % _SS_MAX_RX_BUFF;
}

/*********************************************************************/
inline void rfid_handle_interrupt()
/*********************************************************************/
{
  rfid_recv();
}

#if defined(PCINT0_vect)
ISR(PCINT0_vect) { rfid_handle_interrupt(); }
#endif

#if defined(PCINT1_vect)
ISR(PCINT1_vect) { rfid_handle_interrupt(); }
#endif

#if defined(PCINT2_vect)
ISR(PCINT2_vect) { rfid_handle_interrupt(); }
#endif

#if defined(PCINT3_vect)
ISR(PCINT3_vect) { rfid_handle_interrupt(); }
#endif

#endif //DEVICE_CORE_19
#endif //DEVICE_19


//#######################################################################################################
//######################## DEVICE-20 BMP0685 I2C Barometric Pressure Sensor  ############################
//#######################################################################################################

/*********************************************************************************************\
 * Deze funktie leest een BMP085 Luchtdruk sensor uit.
 * Deze funktie moet worden gebruikt via de I2C bus van de Nodo.
 * De uitgelezen temperatuur waarde wordt in de opgegeven variabele opgeslagen.
 * De uitgelezen luchtvochtigheidsgraad wordt in de opgegeven variabele +1 opgeslagen.
 * 
 * Auteur             : Nodo-team (Martinus van den Broek) www.nodo-domotica.nl
 * Support            : www.nodo-domotica.nl
 * Datum              : Mrt.2013
 * Compatibiliteit    : Vanaf Nodo build nummer 508
 * Syntax             : "BMP085Read <Par2:Basis Variabele>"
 *********************************************************************************************
 * Technische informatie:
 * De BMP085 is een sensor die via I2C moet worden aangesloten
 * Dit protocol gebruikt twee variabelen, 1 voor temperatuur en 1 voor luchtdruk
 \*********************************************************************************************/

#ifdef DEVICE_20
#define DEVICE_ID 20
#define DEVICE_NAME "BMP085Read"

#define BMP085_I2CADDR           0x77
#define BMP085_ULTRAHIGHRES         3
#define BMP085_CAL_AC1           0xAA  // R   Calibration data (16 bits)
#define BMP085_CAL_AC2           0xAC  // R   Calibration data (16 bits)
#define BMP085_CAL_AC3           0xAE  // R   Calibration data (16 bits)    
#define BMP085_CAL_AC4           0xB0  // R   Calibration data (16 bits)
#define BMP085_CAL_AC5           0xB2  // R   Calibration data (16 bits)
#define BMP085_CAL_AC6           0xB4  // R   Calibration data (16 bits)
#define BMP085_CAL_B1            0xB6  // R   Calibration data (16 bits)
#define BMP085_CAL_B2            0xB8  // R   Calibration data (16 bits)
#define BMP085_CAL_MB            0xBA  // R   Calibration data (16 bits)
#define BMP085_CAL_MC            0xBC  // R   Calibration data (16 bits)
#define BMP085_CAL_MD            0xBE  // R   Calibration data (16 bits)
#define BMP085_CONTROL           0xF4 
#define BMP085_TEMPDATA          0xF6
#define BMP085_PRESSUREDATA      0xF6
#define BMP085_READTEMPCMD       0x2E
#define BMP085_READPRESSURECMD   0x34

uint8_t oversampling = BMP085_ULTRAHIGHRES;
int16_t ac1, ac2, ac3, b1, b2, mb, mc, md;
uint16_t ac4, ac5, ac6;

boolean Device_20(byte function, struct NodoEventStruct *event, char *string)
{

  boolean success=false;

  switch(function)
  {
#ifdef DEVICE_CORE_20

  case DEVICE_INIT:
    {
      bmp085_begin();
      break;
    }

  case DEVICE_RAWSIGNAL_IN:
    break;

  case DEVICE_COMMAND:
    {
    byte VarNr = event->Par2; // De originele Par1 tijdelijk opslaan want hier zit de variabelenummer in waar de gebruiker de uitgelezen waarde in wil hebben
      ClearEvent(event);                                    // Ga uit van een default schone event. Oude eventgegevens wissen.
    event->Command      = CMD_VARIABLE_SET;                 // Commando "VariableSet"
    event->Par1         = VarNr;                            // Par1 is de variabele die we willen vullen.
    event->Par2         = float2ul(float(bmp085_readTemperature()));
    QueueAdd(event);                                        // Event opslaan in de event queue, hierna komt de volgende meetwaarde
    event->Par1         = VarNr+1;                          // Par1+1 is de variabele die we willen vullen voor luchtvochtigheid
    event->Par2         = float2ul(float(bmp085_readPressure()-100000));
    QueueAdd(event);
    success=true;
    }
#endif // DEVICE_CORE_20

#if NODO_MEGA
  case DEVICE_MMI_IN:
    {
    char *TempStr=(char*)malloc(26);
    string[25]=0;

    if(GetArgv(string,TempStr,1))
      {
      if(strcasecmp(TempStr,DEVICE_NAME)==0)
        {
        if(event->Par1 >0 && event->Par1<=USER_VARIABLES_MAX-1)
          success=true;
        }
      }
      free(TempStr);
      break;
    }

  case DEVICE_MMI_OUT:
    {
    strcpy(string,DEVICE_NAME);            // Eerste argument=het commando deel
    strcat(string," ");
    strcat(string,int2str(event->Par1));

    break;
    }
#endif //NODO_MEGA
  }      
  return success;
}

#ifdef DEVICE_CORE_20
/*********************************************************************/
boolean bmp085_begin()
/*********************************************************************/
{
  if (bmp085_read8(0xD0) != 0x55) return false;

  /* read calibration data */
  ac1 = bmp085_read16(BMP085_CAL_AC1);
  ac2 = bmp085_read16(BMP085_CAL_AC2);
  ac3 = bmp085_read16(BMP085_CAL_AC3);
  ac4 = bmp085_read16(BMP085_CAL_AC4);
  ac5 = bmp085_read16(BMP085_CAL_AC5);
  ac6 = bmp085_read16(BMP085_CAL_AC6);

  b1 = bmp085_read16(BMP085_CAL_B1);
  b2 = bmp085_read16(BMP085_CAL_B2);

  mb = bmp085_read16(BMP085_CAL_MB);
  mc = bmp085_read16(BMP085_CAL_MC);
  md = bmp085_read16(BMP085_CAL_MD);
}

/*********************************************************************/
uint16_t bmp085_readRawTemperature(void)
/*********************************************************************/
{
  bmp085_write8(BMP085_CONTROL, BMP085_READTEMPCMD);
  delay(5);
  return bmp085_read16(BMP085_TEMPDATA);
}

/*********************************************************************/
uint32_t bmp085_readRawPressure(void)
/*********************************************************************/
{
  uint32_t raw;

  bmp085_write8(BMP085_CONTROL, BMP085_READPRESSURECMD + (oversampling << 6));

  delay(26);

  raw = bmp085_read16(BMP085_PRESSUREDATA);
  raw <<= 8;
  raw |= bmp085_read8(BMP085_PRESSUREDATA+2);
  raw >>= (8 - oversampling);

  return raw;
}

/*********************************************************************/
int32_t bmp085_readPressure(void)
/*********************************************************************/
{
  int32_t UT, UP, B3, B5, B6, X1, X2, X3, p;
  uint32_t B4, B7;

  UT = bmp085_readRawTemperature();
  UP = bmp085_readRawPressure();

  // do temperature calculations
  X1=(UT-(int32_t)(ac6))*((int32_t)(ac5))/pow(2,15);
  X2=((int32_t)mc*pow(2,11))/(X1+(int32_t)md);
  B5=X1 + X2;

  // do pressure calcs
  B6 = B5 - 4000;
  X1 = ((int32_t)b2 * ( (B6 * B6)>>12 )) >> 11;
  X2 = ((int32_t)ac2 * B6) >> 11;
  X3 = X1 + X2;
  B3 = ((((int32_t)ac1*4 + X3) << oversampling) + 2) / 4;

  X1 = ((int32_t)ac3 * B6) >> 13;
  X2 = ((int32_t)b1 * ((B6 * B6) >> 12)) >> 16;
  X3 = ((X1 + X2) + 2) >> 2;
  B4 = ((uint32_t)ac4 * (uint32_t)(X3 + 32768)) >> 15;
  B7 = ((uint32_t)UP - B3) * (uint32_t)( 50000UL >> oversampling );

  if (B7 < 0x80000000) {
    p = (B7 * 2) / B4;
  } else {
    p = (B7 / B4) * 2;
  }
  X1 = (p >> 8) * (p >> 8);
  X1 = (X1 * 3038) >> 16;
  X2 = (-7357 * p) >> 16;

  p = p + ((X1 + X2 + (int32_t)3791)>>4);
  return p;
}

/*********************************************************************/
float bmp085_readTemperature(void)
/*********************************************************************/
{
  int32_t UT, X1, X2, B5;     // following ds convention
  float temp;

  UT = bmp085_readRawTemperature();

  // step 1
  X1 = (UT - (int32_t)ac6) * ((int32_t)ac5) / pow(2,15);
  X2 = ((int32_t)mc * pow(2,11)) / (X1+(int32_t)md);
  B5 = X1 + X2;
  temp = (B5+8)/pow(2,4);
  temp /= 10;
  
  return temp;
}

/*********************************************************************/
uint8_t bmp085_read8(uint8_t a)
/*********************************************************************/
{
  uint8_t ret;

  Wire.beginTransmission(BMP085_I2CADDR); // start transmission to device 
  Wire.write(a); // sends register address to read from
  Wire.endTransmission(); // end transmission
  
  Wire.beginTransmission(BMP085_I2CADDR); // start transmission to device 
  Wire.requestFrom(BMP085_I2CADDR, 1);// send data n-bytes read
  ret = Wire.read(); // receive DATA
  Wire.endTransmission(); // end transmission

  return ret;
}

/*********************************************************************/
uint16_t bmp085_read16(uint8_t a)
/*********************************************************************/
{
  uint16_t ret;

  Wire.beginTransmission(BMP085_I2CADDR); // start transmission to device 
  Wire.write(a); // sends register address to read from
  Wire.endTransmission(); // end transmission
  
  Wire.beginTransmission(BMP085_I2CADDR); // start transmission to device 
  Wire.requestFrom(BMP085_I2CADDR, 2);// send data n-bytes read
  ret = Wire.read(); // receive DATA
  ret <<= 8;
  ret |= Wire.read(); // receive DATA
  Wire.endTransmission(); // end transmission

  return ret;
}

/*********************************************************************/
void bmp085_write8(uint8_t a, uint8_t d)
/*********************************************************************/
{
  Wire.beginTransmission(BMP085_I2CADDR); // start transmission to device 
  Wire.write(a); // sends register address to read from
  Wire.write(d);  // write data
  Wire.endTransmission(); // end transmission
}
#endif //DEVICE_CORE_20
#endif //DEVICE_20


//#######################################################################################################
//#################################### Device-21: LCD I2C 1602 ##########################################
//#######################################################################################################

/*********************************************************************************************\
 * Dit protocol zorgt voor communicatie met een DFRobot LCD I2C/TWI 1602 display
 * 
 * Auteur             : Martinus van den Broek
 * Support            : www.nodo-domotica.nl
 * Datum              : Mrt.2013
 * Versie             : 1.0
 * Nodo productnummer : 
 * Compatibiliteit    : Vanaf Nodo build nummer 508
 * Syntax             : "LCDI2CWrite <row>,<message id>
 ***********************************************************************************************
 * Technische beschrijving:
 *
 * De LCDI2C1602 is een LCD Display van twee regels en 16 tekens per regel.
 * De aansturing vindt plaats via de standaard TwoWire interface van de Nodo (I2C)
 * Het display heeft een vast adres van 0x27
 * Er kan dus maar 1 display per I2C bus worden aangesloten.
 \*********************************************************************************************/

#ifdef DEVICE_21
#define DEVICE_ID 21
#define DEVICE_NAME "LCDI2CWrite"

prog_char PROGMEM LCD_01[] = "Nodo Domotica";
prog_char PROGMEM LCD_02[] = "Mega R:%03d U:%d";
prog_char PROGMEM LCD_03[] = "Small R:%03d U:%d";
prog_char PROGMEM LCD_04[] = "Alarm On";
prog_char PROGMEM LCD_05[] = "Alarm Off";
prog_char PROGMEM LCD_06[] = "6";
prog_char PROGMEM LCD_07[] = "7";
prog_char PROGMEM LCD_08[] = "8";
prog_char PROGMEM LCD_09[] = "9";
prog_char PROGMEM LCD_10[] = "10";
#define LCDI2C_MSG_MAX        10

PROGMEM const char *LCDText_tabel[]={LCD_01,LCD_02,LCD_03,LCD_04,LCD_05,LCD_06,LCD_07,LCD_08,LCD_09,LCD_10};

#define LCD_I2C_ADDRESS 0x27

#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En B00000100  // Enable bit
#define Rw B00000010  // Read/Write bit
#define Rs B00000001  // Register select bit

#ifdef DEVICE_CORE_21
uint8_t _displayfunction;
uint8_t _displaycontrol;
uint8_t _displaymode;
uint8_t _numlines;
uint8_t _backlightval=LCD_BACKLIGHT;
#endif

boolean Device_21(byte function, struct NodoEventStruct *event, char *string)
{
  boolean success=false;

  switch(function)
  {
#ifdef DEVICE_CORE_21
    case DEVICE_INIT:
      {
        _displayfunction = LCD_2LINE;
        _numlines = 2;
        delay(50); 
        // Now we pull both RS and R/W low to begin commands
        LCD_I2C_expanderWrite(_backlightval);	// reset expanderand turn backlight off (Bit 8 =1)
        delay(1000);

        //put the LCD into 4 bit mode, this is according to the hitachi HD44780 datasheet, figure 24, pg 46
        LCD_I2C_write4bits(0x03 << 4);        // we start in 8bit mode, try to set 4 bit mode
        delayMicroseconds(4500);              // wait min 4.1ms
        LCD_I2C_write4bits(0x03 << 4);        // second try
        delayMicroseconds(4500);              // wait min 4.1ms
        LCD_I2C_write4bits(0x03 << 4);        // third go!
        delayMicroseconds(150);
        LCD_I2C_write4bits(0x02 << 4);        // finally, set to 4-bit interface
        LCD_I2C_command(LCD_FUNCTIONSET | _displayfunction);              // set # lines, font size, etc.
        _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;   // turn the display on with no cursor or blinking default
        LCD_I2C_display();
        LCD_I2C_clear();                                                  // clear it off
        _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;           // Initialize to default text direction (for roman languages)
        LCD_I2C_command(LCD_ENTRYMODESET | _displaymode);                 // set the entry mode
        LCD_I2C_home();

        LCD_I2C_printline(0,ProgmemString(LCD_01));
        char TempString[18];
        #if NODO_MEGA
         sprintf(TempString,ProgmemString(LCD_02), NODO_BUILD, Settings.Unit);
        #else
          sprintf(TempString,ProgmemString(LCD_03), NODO_BUILD, Settings.Unit);
        #endif
        LCD_I2C_printline(1,TempString);
      }
   case DEVICE_COMMAND:
      {
      if ((event->Par1 == 1) || (event->Par1 == 2))
        {
          if (event->Par2 == 0) LCD_I2C_printline(event->Par1-1,"");
          if ((event->Par2 > 0) && (event->Par2 <= LCDI2C_MSG_MAX))
            {
              char TempString[18];
              strcpy_P(TempString,(char*)pgm_read_word(&(LCDText_tabel[event->Par2-1])));
              LCD_I2C_printline(event->Par1-1, TempString);
            }
          success=true;
        }
      break;
      }
#endif // DEVICE_CORE_21

    #if NODO_MEGA
    case DEVICE_MMI_IN:
      {
      char *TempStr=(char*)malloc(26);
      string[25]=0;

      if(GetArgv(string,TempStr,1))
        {
        if(strcasecmp(TempStr,DEVICE_NAME)==0)
          {
          if(event->Par1>0 && event->Par1<=2 && event->Par2>0 && event->Par2<=LCDI2C_MSG_MAX)
            success=true;
          }
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

#ifdef DEVICE_CORE_21
/*********************************************************************/
void LCD_I2C_printline(byte row, char* message)
/*********************************************************************/
{
  LCD_I2C_setCursor(0,row);
  for (byte x=0; x<16; x++) LCD_I2C_write(' ');
  LCD_I2C_setCursor(0,row);
  for (byte x=0; x<16; x++)
    {
      if (message[x] != 0) LCD_I2C_write(message[x]);
      else break;
    }
}

/*********************************************************************/
inline size_t LCD_I2C_write(uint8_t value)
/*********************************************************************/
{
	LCD_I2C_send(value, Rs);
	return 0;
}

/*********************************************************************/
void LCD_I2C_display() {
/*********************************************************************/
  _displaycontrol |= LCD_DISPLAYON;
  LCD_I2C_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

/*********************************************************************/
void LCD_I2C_clear(){
/*********************************************************************/
  LCD_I2C_command(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
  delayMicroseconds(2000);  // this command takes a long time!
}

/*********************************************************************/
void LCD_I2C_home(){
/*********************************************************************/
  LCD_I2C_command(LCD_RETURNHOME);  // set cursor position to zero
  delayMicroseconds(2000);  // this command takes a long time!
}

/*********************************************************************/
void LCD_I2C_setCursor(uint8_t col, uint8_t row){
/*********************************************************************/
  int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
  if ( row > _numlines ) {
 	row = _numlines-1;    // we count rows starting w/0
  }
  LCD_I2C_command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

/*********************************************************************/
inline void LCD_I2C_command(uint8_t value) {
/*********************************************************************/
  LCD_I2C_send(value, 0);
}

/*********************************************************************/
void LCD_I2C_send(uint8_t value, uint8_t mode) {
/*********************************************************************/
  uint8_t highnib=value&0xf0;
  uint8_t lownib=(value<<4)&0xf0;
  LCD_I2C_write4bits((highnib)|mode);
  LCD_I2C_write4bits((lownib)|mode); 
}

/*********************************************************************/
void LCD_I2C_write4bits(uint8_t value) {
/*********************************************************************/
  LCD_I2C_expanderWrite(value);
  LCD_I2C_pulseEnable(value);
}

/*********************************************************************/
void LCD_I2C_expanderWrite(uint8_t _data){                                        
/*********************************************************************/
  Wire.beginTransmission(LCD_I2C_ADDRESS);
  Wire.write((int)(_data) | _backlightval);
  Wire.endTransmission();   
}

/*********************************************************************/
void LCD_I2C_pulseEnable(uint8_t _data){
/*********************************************************************/
  LCD_I2C_expanderWrite(_data | En);	// En high
  delayMicroseconds(1);		// enable pulse must be >450ns
	
  LCD_I2C_expanderWrite(_data & ~En);	// En low
  delayMicroseconds(50);		// commands need > 37us to settle
} 
#endif //DEVICE_CORE_21
#endif //DEVICE_21

          

//#######################################################################################################
//#################################### Device-22: HCSR04_Read  ##########################################
//#######################################################################################################

/*********************************************************************************************\
 * Funktionele beschrijving:
 * 
 * Deze device code zorgt voor uitlezing van een ultrasone afstandsmeter HC-SR04. Dit is een low-cost
 * sensor die met behulp van ultrasone pulsen de afstand tot een object kan meten met een bereik van
 * 2 tot 400 centimeter. Kan worden gebruikt als bijvoorbeeld parkeerhulp of als beveiliging van een  
 * ruimte of object. Na aanroep van dit device wordt de afstand tot het object in centimeters geplaatst
 * in de opgegeven variabele.
 *
 * Iedere keer als [HCSR04_Read <variabele>] wordt aangeroepen, zal de opgegeven variabele worden gevuld
 * met de gemeten afstand. De variabele kan vervolgens worden gebruikt voor eigen toepassing.
 * 
 * Auteur             : Paul Tonkes, p.k.tonkes@gmail.com
 * Support            : www.nodo-domotica.nl
 * Datum              : 23-04-2013
 * Versie             : 1.0
 * Nodo productnummer : Device-22 HC-SR04 Distance sensor (SWACDE-22-V10)
 * Compatibiliteit    : Nodo release 3.5.0
 * Syntax             : "HCSR04_Read <variabele>"
 *
 ***********************************************************************************************
 * Technische beschrijving:
 *
 * Compiled size      : 675 bytes voor een Mega en 660 voor een Small.
 * Externe funkties   : float2ul(), GetArgv()
 *
 * De sensor heeft vier aansluitingen:
 * VCC  => +5
 * TRIG => Aansluiten op WiredOut-1
 * ECHO => Aansluiten op WiredOut-2 (sluit uit veiligheid aan via een weerstand van 1K Ohm)
 * GND  => Massa
 *
 \*********************************************************************************************/
 
#ifdef DEVICE_22
#define DEVICE_NAME_22 "HCSR04_Read"

boolean Device_22(byte function, struct NodoEventStruct *event, char *string)
  {
  #ifdef DEVICE_CORE_22
  boolean success=false;
  switch(function)
    {        
    case DEVICE_COMMAND:
      {
      // start de meting en zend een trigger puls van 10mSec.
      noInterrupts();
      digitalWrite(PIN_WIRED_OUT_1, LOW);
      delayMicroseconds(2);
      digitalWrite(PIN_WIRED_OUT_1, HIGH);
      delayMicroseconds(10);
      digitalWrite(PIN_WIRED_OUT_1, LOW);
      
      // meet de tijd van de echo puls. Uit dit gegeven berekenen we de afstand.
      float distance=pulseIn(PIN_WIRED_OUT_1+1,HIGH);
      interrupts();
      
      distance=distance/58;
      
      event->Command      = CMD_VARIABLE_SET;     // Commando "VariableSet"
      event->Par2         = float2ul(distance);   // Waarde terugstoppen in de variabele
      success=true;
      break;
      }
      
    case DEVICE_INIT:
      {
      pinMode(PIN_WIRED_OUT_1  , OUTPUT); // TRIG
      pinMode(PIN_WIRED_OUT_1+1, INPUT ); // ECHO
      break;
      }

    #if NODO_MEGA
    case DEVICE_MMI_IN:
      {
      char *TempStr=(char*)malloc(26);
      string[25]=0;

      if(GetArgv(string,TempStr,1))
        {
        if(strcasecmp(TempStr,DEVICE_NAME_22)==0)
          {
          // Par1 en Par2 hoeven niet te worden geparsed omdat deze default al door de MMI invoer van de Nodo 
          // worden gevuld indien het integer waarden zijn. Toetsen op bereikenmoet nog wel plaats vinden.
          if(event->Par1>0 && event->Par1<=USER_VARIABLES_MAX)
            success=true;
          }
        }
      free(TempStr);
      break;
      }

    case DEVICE_MMI_OUT:
      {
      strcpy(string,DEVICE_NAME_22);            // Eerste argument=het commando deel
      strcat(string," ");
      strcat(string,int2str(event->Par1));
      strcat(string,",");
      strcat(string,int2str(event->Par2));
      break;
      }
    #endif //NODO_MEGA

    #endif //CORE_22
    }
  return success;
  }
#endif //DEVICE_22


//#######################################################################################################
//#################################### Device-23: PWM Led dimmer ########################################
//#######################################################################################################


/*********************************************************************************************\
 * Funktionele beschrijving: Dit device stuurt een PWM signaal uit waarmee een RGB-led kan worden aangestuurd.
 *                           Als de Arduino uitgang wordt gebruikt om een MOSFET of een andere schakeling
 *                           aan te sturen, dan kunnen hiermee led-strips worden aangestuurd.
 *
 *                           
 * Auteur             : P.K.Tonkes
 * Support            : P.K.Tonkes@gmail.com
 * Datum              : 26-05-2013
 * Versie             : 1.0 (Beta)
 * Nodo productnummer : Device-23 PWM Led-dimmer (SWACDE-23-V10)
 * Compatibiliteit    : Vanaf Nodo build nummer 530
 * Syntax             : PWM <R>,<G>,<B>, <Fade: On | Off>
 *                      De opgegeven waarden R,G en B kunnen een waarde hebben van 0..255
 *                      Laatste parameter <fade> is optioneel en kan [On] of [Off] zijn.
 ***********************************************************************************************
 * Technische beschrijving:
 *
 * Compiled size      : 1225 bytes
 * Externe funkties   : str2cmd(), GetArgv(), cmd2str(), str2int(), int2str().
 *
 * De dimfunktie werkt op basis van Pulsbreedte modulatie (PWM) Dit is een standaard methode voor 
 * dimmen van LED's, mmaar wrdt ook gebruikt voor regelen van snelheid van motoren. 
 * Omdat de PWM funkties niet op alle pinnen van de Arduino beschikbaar zijn,
 * zijn deze in dit device hard toegewezen is de pinkeuze geen gebruikers optie.
 *
 * Nodo Mega:  Rood  = Arduino pen PWM-5
 *             Groen = Arduino pen PWM-6
 *             Blauw = Arduino pen PWM-7
 *
 * Nodo Small: Rood  = Arduino pen D9
 *             Groen = Arduino pen D10
 *             Blauw = Arduino pen D11 (LET OP: Op een standaard Nodo is dit de IR zend-led. Deze verwijderen!)
 *             
 \*********************************************************************************************/
 
#ifdef DEVICE_23
#define DEVICE_ID 23
#define DEVICE_NAME_23 "LED"
#if NODO_MEGA
#define PWM_R     5
#define PWM_G     6
#define PWM_B     7
#else
#define PWM_R     9
#define PWM_G    10
#define PWM_B    11
#endif

boolean Device_23(byte function, struct NodoEventStruct *event, char *string)
  {
  boolean success=false;
  static byte InputLevelR=0, InputLevelG=0, InputLevelB=0;
  static byte OutputLevelR=0, OutputLevelG=0, OutputLevelB=0;
  static byte fade=VALUE_OFF;
  
  #ifdef DEVICE_CORE_23
  switch(function)
    { 
    case DEVICE_ONCE_A_SECOND:
      {
      if(fade==VALUE_ON)
        {
        if(InputLevelR>OutputLevelR)OutputLevelR++;
        if(InputLevelG>OutputLevelG)OutputLevelG++;
        if(InputLevelB>OutputLevelB)OutputLevelB++;
  
        if(InputLevelR<OutputLevelR)OutputLevelR--;
        if(InputLevelG<OutputLevelG)OutputLevelG--;
        if(InputLevelB<OutputLevelB)OutputLevelB--;
  
        analogWrite(PWM_R,OutputLevelR);
        analogWrite(PWM_G,OutputLevelG);
        analogWrite(PWM_B,OutputLevelB);

        if(InputLevelR==OutputLevelR && InputLevelG==OutputLevelG && InputLevelB==OutputLevelB)
          fade=VALUE_OFF;
        }
      break;
      }
      
    case DEVICE_COMMAND:
      {
      InputLevelR=(event->Par2 >>16)&0xff;
      InputLevelG=(event->Par2 >> 8)&0xff;
      InputLevelB=(event->Par2     )&0xff;  
      fade=event->Par1;
      
      if(fade!=VALUE_ON)
        {
        OutputLevelR=InputLevelR;
        OutputLevelG=InputLevelG;
        OutputLevelB=InputLevelB;

        analogWrite(PWM_R,OutputLevelR);
        analogWrite(PWM_G,OutputLevelG);
        analogWrite(PWM_B,OutputLevelB);
        }
      break;
      }
      
    #if NODO_MEGA
    case DEVICE_MMI_IN:
      {
      char *TempStr=(char*)malloc(26);
      string[25]=0;

      if(GetArgv(string,TempStr,1))
        {
        if(strcasecmp(TempStr,DEVICE_NAME_23)==0)
          {
          if(GetArgv(string,TempStr,2)) 
            {
            event->Par2=str2int(TempStr)<<16;
            if(GetArgv(string,TempStr,3))
              {
              event->Par2|=str2int(TempStr)<<8;
              if(GetArgv(string,TempStr,4))
                {
                event->Par2|=str2int(TempStr);
                success=true;
                }
              if(GetArgv(string,TempStr,5))
                event->Par1=str2cmd(TempStr);
              else
                event->Par1=VALUE_OFF;
              }
            }
          }
        }
      free(TempStr);
      break;
      }

    case DEVICE_MMI_OUT:
      {
      strcpy(string,DEVICE_NAME_23);            // Commando 
      strcat(string," ");
      strcat(string,int2str((event->Par2 >>16)&0xff));      // Parameter-1 = R (8-bit)
      strcat(string,",");
      strcat(string,int2str((event->Par2 >> 8)&0xff));      // Parameter-2 = G (8-bit)
      strcat(string,",");
      strcat(string,int2str((event->Par2     )&0xff));      // Parameter-3 = B (8-bit)
      strcat(string,",");
      strcat(string,cmd2str(event->Par1));                  // Parameter-4 = Fading <On | Off> (8-bit)
      break;
      }
    #endif //NODO_MEGA
    }      
  #endif // CORE
  return success;
  }
#endif //DEVICE_23


