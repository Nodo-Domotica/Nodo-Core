#define NODO_PULSE_0               500  // PWM: Tijdsduur van de puls bij verzenden van een '0' in uSec.
#define NODO_PULSE_MID            1000  // PWM: Pulsen langer zijn '1'
#define NODO_PULSE_1              1500  // PWM: Tijdsduur van de puls bij verzenden van een '1' in uSec. (3x NODO_PULSE_0)
#define NODO_SPACE                 500  // PWM: Tijdsduur van de space tussen de bitspuls bij verzenden van een '1' in uSec.   
#define SIGNAL_ANALYZE_SHARPNESS    50  // Scherpte c.q. foutmarge die gehanteerd wordt bij decoderen van RF/IR signaal.
#define MIN_RAW_PULSES              16  // =8 bits. Minimaal aantal ontvangen bits*2 alvorens cpu tijd wordt besteed aan decodering, etc. Zet zo hoog mogelijk om CPU-tijd te sparen en minder 'onzin' te ontvangen.
#define SWITCH_TIME_RX_TX_IR       100  // Minimale tijd tussen omschakelen van ontvangen naar zenden. 
#define SWITCH_TIME_RX_TX_RF       750  // Minimale tijd tussen omschakelen van ontvangen naar zenden. 
#define SWITCH_TIME_RX_TX_I2C       25  // Minimale tijd tussen omschakelen van ontvangen naar zenden. 
#define WAIT_FREE_RX_WINDOW        250  // minimale wachttijd wanneer wordt gewacht op een vrije RF of IR band.

boolean AnalyzeRawSignal(struct NodoEventStruct *E)
  {
  ClearEvent(E);

  if(RawSignal.Number==RAW_BUFFER_SIZE)// Als het signaal een volle buffer beslaat is het zeer waarschijnlijk ruis of ongeldig signaal
    return false;     
  
  if(RawSignal_2_Nodo(E))           // Is het een Nodo signaal
    return true;
  
  #if NODO_30_COMPATIBLE
  if(RawSignal_2_Nodo_OLD(E))       // Is het een Nodo signaal: oude 32-bit format.
    return true;
  #endif

  if(Transmission_NodoOnly)
    return false;

  // Loop de devices langs. Indien een device dit nodig heeft, zal deze het rawsignal gebruiken en omzetten naar een
  // geldig event.
  for(byte x=0;x<DEVICE_MAX && Device_ptr[x]!=0; x++)
    {
    if(Device_ptr[x](DEVICE_RAWSIGNAL_IN,E,0))
      {
      E->Command=Device_id[x];
      E->Type=NODO_TYPE_DEVICE;
      return true;
      }
    }

  // als er geen enkel geldig signaaltype uit de pulsenreeks kon worden gedestilleerd, dan resteert niets anders
  // dan deze weer te geven als een RawSignal.
  if(Settings.RawSignalReceive==VALUE_ON)
    if(RawSignal_2_32bit(E))
      return true;

  return false;   
  }


/**********************************************************************************************\
 * Deze functie genereert uit een willekeurig gevulde RawSignal afkomstig van de meeste 
 * afstandsbedieningen een (vrijwel) unieke bit code.
 * Zowel breedte van de pulsen als de afstand tussen de pulsen worden in de berekening
 * meegenomen zodat deze functie geschikt is voor PWM, PDM en Bi-Pase modulatie.
 * LET OP: Het betreft een unieke hash-waarde zonder betekenis van waarde.
 \*********************************************************************************************/
boolean RawSignal_2_32bit(struct NodoEventStruct *event)
  {
  int x,y,z;
  int Counter_pulse=0,Counter_space=0;
  byte MinPulse=0xff;
  byte MinSpace=0xff;
  unsigned long CodeP=0L;
  unsigned long CodeS=0L;

  // zoek de kortste tijd (PULSE en SPACE)
  x=5; // 0=aantal, 1=startpuls, 2=space na startpuls, 3=1e puls
  while(x<=RawSignal.Number-4)
    {
    if(RawSignal.Pulses[x]<MinPulse)MinPulse=RawSignal.Pulses[x]; // Zoek naar de kortste pulstijd.
    x++;
    if(RawSignal.Pulses[x]<MinSpace)MinSpace=RawSignal.Pulses[x]; // Zoek naar de kortste spacetijd.
    x++;
    }

  MinPulse+=(MinPulse*SIGNAL_ANALYZE_SHARPNESS)/100;
  MinSpace+=(MinSpace*SIGNAL_ANALYZE_SHARPNESS)/100;

  x=3; // 0=aantal, 1=startpuls, 2=space na startpuls, 3=1e pulslengte
  z=0; // bit in de Code die geset moet worden 
  do{
    if(z>31)
      {
      CodeP=CodeP>>1;
      CodeS=CodeS>>1;
      }

    if(RawSignal.Pulses[x]>MinPulse)
      {
      if(z<=31)// de eerste 32 bits vullen in de 32-bit variabele
        CodeP|=(long)(1L<<z); //LSB in signaal wordt  als eerste verzonden
      else // daarna de resterende doorschuiven
      CodeP|=0x80000000L;
      Counter_pulse++;
      }
    x++;

    if(RawSignal.Pulses[x]>MinSpace)
      {
      if(z<=31)// de eerste 32 bits vullen in de 32-bit variabele
        CodeS|=(long)(1L<<z); //LSB in signaal wordt als eerste verzonden
      else // daarna de resterende doorschuiven
      CodeS|=0x80000000L;
      Counter_space++;
      }
    x++;
    z++;
    }
  while(x<RawSignal.Number);

  if(Counter_pulse>=1 && Counter_space<=1)
    event->Par2=CodeP; // data zat in de pulsbreedte
  else if(Counter_pulse<=1 && Counter_space>=1)
    event->Par2=CodeS; // data zat in de pulse afstand
  else
    event->Par2=CodeS^CodeP; // data zat in beide = bi-phase, maak er een leuke mix van.

  event->Par2&=0x0fffffff;
  
  // Indien ehet signaal via RF is binnengekomen, dan gebruik maken van het feit dat RF signalen herhaald worden verzonden.
  // Herhalingen vormen dan de checksum. Voor IR is dit niet nodig omdat de kans op ruis veel kleiner is
  RawSignal.Repeats=(RawSignal.Source==VALUE_SOURCE_RF);
  event->SourceUnit=0;  
  event->DestinationUnit=0;
  event->Type=NODO_TYPE_EVENT;
  event->Command=EVENT_RAWSIGNAL;
  event->Par1=0;
  return true;
  }


/**********************************************************************************************\
 * Deze functie wacht totdat de 433 en IR band vrij zijn of er een timeout heeft plaats gevonden 
 * Window en delay tijd in milliseconden
 \*********************************************************************************************/
void WaitFree(byte Port, int TimeOut)
  {
  unsigned long WindowTimer, TimeOutTimer=millis()+TimeOut;  // tijd waarna de routine wordt afgebroken in milliseconden

  Led(BLUE);

  // luister een tijdwindow of er pulsen wijn
  WindowTimer=millis()+WAIT_FREE_RX_WINDOW;

  while(WindowTimer>millis() && TimeOutTimer>millis())
    {
    // Luister of er pulsen zijn
    if( (( Port==VALUE_SOURCE_IR || Port==VALUE_ALL) && pulseIn(PIN_IR_RX_DATA,LOW  ,1000) > MIN_PULSE_LENGTH)   ||
        (( Port==VALUE_SOURCE_RF || Port==VALUE_ALL) && pulseIn(PIN_RF_RX_DATA,HIGH ,1000) > MIN_PULSE_LENGTH)   )

        WindowTimer=millis()+WAIT_FREE_RX_WINDOW;
    }
  Led(RED);
  }


/*********************************************************************************************\
 * Deze routine zendt een RAW code via RF. 
 * De inhoud van de buffer RawSignal moet de pulstijden bevatten. 
 * RawSignal.Number het aantal pulsen*2
 \*********************************************************************************************/
void RawSendRF(void)
  {
  int x;
  digitalWrite(PIN_RF_RX_VCC,LOW);   // Spanning naar de RF ontvanger uit om interferentie met de zender te voorkomen.
  digitalWrite(PIN_RF_TX_VCC,HIGH); // zet de 433Mhz zender aan

  delay(5);// kleine pause om de zender de tijd te geven om stabiel te worden 

  // LET OP: In de Arduino versie 1.0.1 zit een bug in de funktie delayMicroSeconds(). Als deze wordt aangeroepen met een nul dan zal er
  // een pause optreden van 16 milliseconden. Omdat het laatste element van RawSignal af sluit met een nul (omdat de space van de stopbit 
  // feitelijk niet bestaat) zal deze bug optreden. Daarom wordt deze op 1 gezet om de bug te omzeilen. 
  RawSignal.Pulses[RawSignal.Number]=1;
  for(byte y=0; y<RawSignal.Repeats; y++) // herhaal verzenden RF code
    {
    x=1;
    noInterrupts();
    while(x<RawSignal.Number)
      {
      digitalWrite(PIN_RF_TX_DATA,HIGH); // 1
      delayMicroseconds(RawSignal.Pulses[x++]*RawSignal.Multiply); 
      digitalWrite(PIN_RF_TX_DATA,LOW); // 0
      delayMicroseconds(RawSignal.Pulses[x++]*RawSignal.Multiply); 
      }
    interrupts();
    delay(RawSignal.Delay);// Delay buiten het gebied waar de interrupts zijn uitgeschakeld! Anders werkt deze funktie niet
    }

  digitalWrite(PIN_RF_TX_VCC,LOW); // zet de 433Mhz zender weer uit
  digitalWrite(PIN_RF_RX_VCC,HIGH); // Spanning naar de RF ontvanger weer aan.

  #if NODO_MEGA
  //Board specifiek: Genereer een korte puls voor omschakelen van de Aurel tranceiver van TX naar RX mode.
  if((HW_Config&0xf) == BIC_HWMESH_NES_V1X)
   {
   delayMicroseconds(36);
   digitalWrite(PIN_BSF_0,LOW);
   delayMicroseconds(16);
   digitalWrite(PIN_BSF_0,HIGH);
   }
  #endif
  }


/*********************************************************************************************\
 * Deze routine zendt een 32-bits code via IR. 
 * De inhoud van de buffer RawSignal moet de pulstijden bevatten. 
 * RawSignal.Number het aantal pulsen*2
 * Pulsen worden verzonden op en draaggolf van 38Khz.
 *
 * LET OP: Deze routine is speciaal geschreven voor de Arduino Mega1280 of Mega2560 met een
 * klokfrequentie van 16Mhz. De IR pin is D17.
 \*********************************************************************************************/

void RawSendIR(void)
  {
  int pulse;  // pulse (bestaande uit een mark en een space) uit de RawSignal tabel die moet worden verzonden
  int mod;    // pulsenteller van het 38Khz modulatie signaal
  
  // kleine pause zodat verzenden event naar de USB poort gereed is, immers de IRQ's worden tijdelijk uitgezet
  delay(10);
  
  // LET OP: In de Arduino versie 1.0.1 zit een bug in de funktie delayMicroSeconds(). Als deze wordt aangeroepen met een nul dan zal er
  // een pause optreden van 16 milliseconden. Omdat het laatste element van RawSignal af sluit met een nul (omdat de space van de stopbit 
  // feitelijk niet bestaat) zal deze bug optreden. Daarom wordt deze op 1 gezet om de bug te omzeilen. 
  RawSignal.Pulses[RawSignal.Number]=1;
  
  
  for(int repeat=0; repeat<RawSignal.Repeats; repeat++) // herhaal verzenden IR code
    {
    pulse=1;
    noInterrupts();
    while(pulse<(RawSignal.Number))
      {
      // Mark verzenden. Bereken hoeveel pulsen van 26uSec er nodig zijn die samen de lengte van de mark/space zijn.
      mod=(RawSignal.Pulses[pulse++]*RawSignal.Multiply)/26; // delen om aantal pulsen uit te rekenen

      while(mod)
        {
        // Hoog
        #if NODO_MEGA
        bitWrite(PORTH,0, HIGH);
        #else
        bitWrite(PORTB,3, HIGH);
        #endif

        delayMicroseconds(12);
        __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");// per nop 62.6 nano sec. @16Mhz
  
        // Laag
        #if NODO_MEGA
        bitWrite(PORTH,0, LOW);    
        #else
        bitWrite(PORTB,3, LOW);
        #endif
        delayMicroseconds(12);
        __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");// per nop 62.6 nano sec. @16Mhz
        mod--;
        }

      // Laag
      delayMicroseconds(RawSignal.Pulses[pulse++]*RawSignal.Multiply);
      }
    interrupts(); // interupts weer inschakelen.  
    delay(RawSignal.Delay);// Delay buiten het gebied waar de interrupts zijn uitgeschakeld! Anders werkt deze funktie niet
    }
  }

/*********************************************************************************************\
 * Deze routine berekend de RAW pulsen van een Nodo event en plaatst deze in de buffer RawSignal
 * RawSignal.Bits het aantal pulsen*2+startbit*2 ==> 130
 \*********************************************************************************************/

// Definieer een datablock die gebruikt wordt voor de gegevens die via de ether verzonden moeten worden.
// Zo kunnen exact die gegevens worden verzonden die nodig zijn en niets teveel.  
struct DataBlockStruct
  {
  byte SourceUnit;
  byte DestinationUnit;
  byte Flags;
  byte Type;
  byte Command;
  byte Par1;
  unsigned long Par2;
  byte Checksum;
  };  

void Nodo_2_RawSignal(struct NodoEventStruct *Event)
  {
  struct DataBlockStruct DataBlock;

  byte BitCounter           = 1;
  RawSignal.Repeats         = 1;
  RawSignal.Delay           = 0;
  RawSignal.Multiply        = 100;

  DataBlock.SourceUnit      = Event->SourceUnit | (Settings.Home<<5);  
  DataBlock.DestinationUnit = Event->DestinationUnit;
  DataBlock.Flags           = Event->Flags;
  DataBlock.Type            = Event->Type;
  DataBlock.Command         = Event->Command;
  DataBlock.Par1            = Event->Par1;
  DataBlock.Par2            = Event->Par2;

  // bereken checksum: crc-8 uit alle bytes in de struct
  byte c=0,*B=(byte*)&DataBlock;
  for(byte x=0;x<sizeof(struct DataBlockStruct);x++)
    c^=*(B+x);
  c^=(SETTINGS_VERSION & 0xff); // Verwerk build in checksum om communicatie ussen verschillende versies te voorkomen 
  DataBlock.Checksum=c;

  // begin met een lange startbit. Veilige timing gekozen zodat deze niet gemist kan worden
  RawSignal.Pulses[BitCounter++]=(NODO_PULSE_1*4)/RawSignal.Multiply; 
  RawSignal.Pulses[BitCounter++]=(NODO_SPACE*2)/RawSignal.Multiply;

  for(byte x=0;x<sizeof(struct DataBlockStruct);x++)
    {
    for(byte Bit=0; Bit<=7; Bit++)
      {
      if((*(B+x)>>Bit)&1)
        RawSignal.Pulses[BitCounter++]=NODO_PULSE_1/RawSignal.Multiply; 
      else
        RawSignal.Pulses[BitCounter++]=NODO_PULSE_0/RawSignal.Multiply;   
      RawSignal.Pulses[BitCounter++]=NODO_SPACE/RawSignal.Multiply;   
      }
    }

  RawSignal.Pulses[BitCounter-1]=NODO_SPACE/RawSignal.Multiply; // pauze tussen de pulsreeksen
  RawSignal.Number=BitCounter;
  }


/**********************************************************************************************\
 * Haal de pulsen en plaats in buffer. 
 * bij de TSOP1738 is in rust is de uitgang hoog. StateSignal moet LOW zijn
 * bij de 433RX is in rust is de uitgang laag. StateSignal moet HIGH zijn
 * 
 \*********************************************************************************************/
inline boolean FetchSignal(byte DataPin, boolean StateSignal, int TimeOut)
  {
  uint8_t bit = digitalPinToBitMask(DataPin);
  uint8_t port = digitalPinToPort(DataPin);
  uint8_t stateMask = (StateSignal ? bit : 0);

  // Kijk of er een signaal binnen komt. Zo niet, dan direct deze funktie verlaten.
  if((*portInputRegister(port) & bit) != stateMask)
    return false;

  int RawCodeLength=1;
  unsigned long PulseLength=0;
  unsigned long numloops = 0;
  const unsigned long LoopsPerMilli=400; // Aantal while() *A* loops binnen een milliseconde inc. compensatie overige overhead binnen de while() *B* loop. Uitgeklokt met een analyser 16Mhz ATMega.
  unsigned long maxloops = (unsigned long)TimeOut * LoopsPerMilli;
  boolean toggle=false;
  RawSignal.Multiply=50;

  do{// lees de pulsen in microseconden en plaats deze in de tijdelijke buffer RawSignal
    numloops = 0;
    while(((*portInputRegister(port) & bit) == stateMask) ^ toggle) // while() loop *A*
      if(numloops++ == maxloops)
        break;//timeout opgetreden

    PulseLength=(numloops * 1000) / LoopsPerMilli;// Bevat nu de pulslengte in microseconden

    // bij kleine stoorpulsen die geen betekenis hebben zo snel mogelijk weer terug
    if(PulseLength<MIN_PULSE_LENGTH)
      return false;

    toggle=!toggle;    

    // sla op in de tabel RawSignal
    RawSignal.Pulses[RawCodeLength++]=PulseLength/(unsigned long)RawSignal.Multiply;
    }
  while(RawCodeLength<RAW_BUFFER_SIZE && numloops<=maxloops);// loop *B* Zolang nog ruimte in de buffer

  if(RawCodeLength>=MIN_RAW_PULSES)
    {
    RawSignal.Number=RawCodeLength-1;
    return true;
    }
  RawSignal.Number=0;
  return false;
  }

/**********************************************************************************************\
 * Bij uitwisseling van eventss tussen Nodo's moeten aan beide zijde rekening worden gehouden 
 * met processingtijd en setup-tijd van de RF ontvangers. eze funktie houdt timers bij en voorziet
 * in wachtloops die de setup-tijd en processing tijden rspecteren, maar niet voor onnodige
 * vertraging zorgen.
 *
 * Na zenden timers setten, voor verzenden een wachttijd inlassen.
 \*********************************************************************************************/
void DelayTransmission(byte Port, boolean Set)
  {
  static unsigned long LastTransmitTime_RF=0L;
  static unsigned long LastTransmitTime_IR=0L;
  static unsigned long LastTransmitTime_I2C=0L;

  // Als set, dan alleen tijdstip markeren
  if(Set)
    {
    switch(Port)
      {
      case VALUE_SOURCE_RF:
        LastTransmitTime_RF=millis();
        break;
  
      case VALUE_SOURCE_IR:
        LastTransmitTime_IR=millis();
        break;
  
      case VALUE_SOURCE_I2C:
        LastTransmitTime_I2C=millis();
        break;
      }
    }
  else    
    {
    switch(Port)
      {
      case VALUE_SOURCE_RF:
        while((LastTransmitTime_RF+SWITCH_TIME_RX_TX_RF) > millis());
        break;
  
      case VALUE_SOURCE_IR:
        while((LastTransmitTime_IR+SWITCH_TIME_RX_TX_IR) > millis());
        break;
  
      case VALUE_SOURCE_I2C:
        while((LastTransmitTime_I2C+SWITCH_TIME_RX_TX_I2C) > millis());
        break;
      }
    delay(MIN_TIME_BETWEEN_TX);// Korte wachttijd tussen alle zendacties.
    }    
  }

/**********************************************************************************************\
 * verzendt een event en geeft dit tevens weer op SERIAL
 * Als UseRawSignal=true, dan wordt er geen signaal opgebouwd, maar de actuele content van de
 * RawSignal buffer gebruikt. In dit geval werkt de WaitFree niet.
 \*********************************************************************************************/
boolean SendEvent(struct NodoEventStruct *ES, boolean UseRawSignal, boolean Display, boolean WaitForFree)
  {  
  ES->Direction=VALUE_DIRECTION_OUTPUT;
  ES->DestinationUnit=Transmission_SendToUnit;
  byte Port=ES->Port;

  // Als een Nodo actief is en excusief communiceert met een andere Nodo, c.q. de ether heeft geclaimd is, dan mag deze Nodo niet zenden.
  // In dit geval resteert deze Nodo niets anders dan even te wachten tot de lijn weer vrijgegeven wordt of de wachttijd verlopen is.
  // Als er een timeout optreedt, dan de blokkade opheffen. Dit ter voorkoming dat Nodo's oneindig wachten op vrije lijn.
  // Uitzondering is als de Nodo zelf de master was, dan deze mag altijd zenden.


  if(Transmission_SelectedUnit!=0 && Transmission_SelectedUnit!=Settings.Unit && !Transmission_ThisUnitIsMaster)
    {
    #if NODO_MEGA
    char* TempString=(char*)malloc(25);
    sprintf(TempString,ProgmemString(Text_10), Transmission_SelectedUnit);    
    PrintString(TempString, VALUE_ALL);
    free(TempString);
    #endif

    if(!Wait(120,true,0,false))
      {
      Transmission_SelectedUnit=0;
      Transmission_NodoOnly=false;
      }
    }

  // Stuur afhankelijk van de instellingen het event door naar I2C, RF, IR. Eerst wordt het event geprint,daarna een korte wachttijd om
  // te zorgen dat er een minimale wachttijd tussen de signlen zit. Tot slot wordt het signaal verzonden.

  if(WaitForFree)
    if(Port==VALUE_SOURCE_RF || Port==VALUE_SOURCE_IR ||(Settings.TransmitRF==VALUE_ON && Port==VALUE_ALL))
      if(Settings.WaitFree==VALUE_ON)
        WaitFree(VALUE_ALL, WAITFREE_TIMEOUT);

  if(!UseRawSignal)
    Nodo_2_RawSignal(ES);

  // Verstuur signaal als IR
  if(Settings.TransmitIR==VALUE_ON && (Port==VALUE_SOURCE_IR || Port==VALUE_ALL))
    { 
    ES->Port=VALUE_SOURCE_IR;
    if(Display)PrintEvent(ES,VALUE_ALL);
    DelayTransmission(VALUE_SOURCE_IR,false);
    RawSendIR();
    }

  // Verstuur signaal als RF
  if(Settings.TransmitRF==VALUE_ON && (Port==VALUE_SOURCE_RF || Port==VALUE_ALL))
    {
    ES->Port=VALUE_SOURCE_RF;
    if(Display)PrintEvent(ES,VALUE_ALL);
    DelayTransmission(VALUE_SOURCE_RF,false);
    RawSendRF();
    }


  #if NODO_MEGA
  // Verstuur signaal als HTTP-event.
  if(bitRead(HW_Config,HW_ETHERNET))// Als Ethernet shield aanwezig.
    {
    if(Settings.TransmitIP==VALUE_ON && (Port==VALUE_SOURCE_HTTP || Port==VALUE_ALL))
      {
      SendHTTPEvent(ES);
      ES->Port=VALUE_SOURCE_HTTP;
      if(Display)PrintEvent(ES,VALUE_ALL);
      }
    }
  #endif 

  // Verstuur event via I2C
  // LET OP:  Voor I2C geldt een uitzondering: Als een device een signaal verzendt, dan mag dit commando alleen er toe leiden dat het
  //          RawSignal wordt verzonden via RF/IR. Anders zal het commando worden verstuurd over I2C waarna de Nodo's op I2C het commando nogmaal
  //          zullen uitvoeren. Zo zal er een ongewenste loop ontstaan.
  
  if(!UseRawSignal)
    {
    if((Port==VALUE_SOURCE_I2C || Port==VALUE_ALL) && bitRead(HW_Config,HW_I2C))
      {
      ES->Port=VALUE_SOURCE_I2C;
      if(Display)PrintEvent(ES,VALUE_ALL);
      DelayTransmission(VALUE_SOURCE_I2C,false);
      SendI2C(ES);
      }
    }
  }
  
#if NODO_MEGA
#endif


/*********************************************************************************************\
 * Deze routine berekent de uit een RawSignal een NODO code
 * Geeft een false retour als geen geldig NODO signaal
 \*********************************************************************************************/
boolean RawSignal_2_Nodo(struct NodoEventStruct *Event)
  {
  byte b,x,y,z;

  if(RawSignal.Number!=16*sizeof(struct DataBlockStruct)+2) // Per byte twee posities + startbit.
    return false;
    
  struct DataBlockStruct DataBlock;
  byte *B=(byte*)&DataBlock; // B wijst naar de eerste byte van de struct
  z=3;  // RwaSignal pulse teller: 0=aantal, 1=startpuls, 2=space na startpuls, 3=1e pulslengte. Dus start loop met drie.

  for(x=0;x<sizeof(struct DataBlockStruct);x++) // vul alle bytes van de struct 
    {
    b=0;
    for(y=0;y<=7;y++) // vul alle bits binnen een byte
      {
      if((RawSignal.Pulses[z]*RawSignal.Multiply)>NODO_PULSE_MID)      
        b|=1<<y; //LSB in signaal wordt  als eerste verzonden
      z+=2;
      }
    *(B+x)=b;
    }

  // bereken checksum: crc-8 uit alle bytes in de queue.
  // Een correcte Checksum met alle bytes levert een nul omdat de XOR van alle bytes in het datablok zit.
  b=0;
  for(x=0;x<sizeof(struct DataBlockStruct);x++)
    b^=*(B+x); 
  b^=(SETTINGS_VERSION & 0xff); // Verwerk build in checksum om communicatie ussen verschillende versies te voorkomen 
    
  if(b==0)
    {
    if(DataBlock.SourceUnit>>5!=Settings.Home)
      return false;

    RawSignal.Repeats    = false; // het is geen herhalend signaal. Bij ontvangst hoeven herhalingen dus niet onderdrukt te worden.
    Event->SourceUnit=DataBlock.SourceUnit&0x1F;  // Maskeer de bits van het Home adres.
    Event->DestinationUnit=DataBlock.DestinationUnit;
    Event->Flags=DataBlock.Flags;
    Event->Type=DataBlock.Type;
    Event->Command=DataBlock.Command;
    Event->Par1=DataBlock.Par1;
    Event->Par2=DataBlock.Par2;
    return true;
    }
  return false; 
  }


//#######################################################################################################
//##################################### Transmission: I2C  ##############################################
//#######################################################################################################


// Deze routine wordt vanuit de Wire library aangeroepen zodra er data op de I2C bus verschijnt die voor deze nodo bestemd is.
// er vindt geen verdere verwerking plaats, slechts opslaan van het event. 
void ReceiveI2C(int n)
  {
  byte b,*B=(byte*)&I2C_Event;
  byte Checksum=0;
  int x=0;

  while(Wire.available()) // Haal de bytes op
    {
    b=Wire.read(); 
    if(x<sizeof(struct NodoEventStruct))
      {
      *(B+x)=b; 
      Checksum^=b; 
      }
    Checksum^=(SETTINGS_VERSION & 0xff); // Verwerk build in checksum om communicatie ussen verschillende versies te voorkomen 
    x++;
    }

  // laatste ontvangen byte bevat de checksum. Als deze gelijk is aan de berekende checksum, dan event uitvoeren
  if(b==Checksum)    
    {   
    bitWrite(HW_Config,HW_I2C,true);
    I2C_EventReceived=true;    
    }
  else
    I2C_EventReceived=false;
}

/**********************************************************************************************\
 * Verstuur een Event naar de I2C bus. Omdat I2C geen generiek adres kent voor alle devices
 * ontkomen we er niet aan om een event te distribueren naar alle Nodo adressen. 
 * Hoewel dit wel realiseerbaar is, is hier niet voor gekozen omdat dit tegen de i2C-specifices
 * in druist. Daarom wordt een event altijd naar alle I2C adressen van alle Nodo's gestuurd.
 * Voor 32 unit nummers neemt deze actie 18 milliseconden in beslag. Nog steeds zeer snel ten 
 * opzichte van RF, HTTP en IR.
 \*********************************************************************************************/
boolean SendI2C(struct NodoEventStruct *EventBlock)
  {  
  byte Checksum, x;

  // bereken checksum: crc-8 uit alle bytes in de queue.
  byte b,*B=(byte*)EventBlock;
  delay(10);//??? kan deze weg???
  for(int y=0;y<UNIT_MAX;y++)
    {            
    // verzend Event 
    Wire.beginTransmission(I2C_START_ADDRESS+y);
    Checksum=0;
    for(x=0;x<sizeof(struct NodoEventStruct);x++)
      {
      b=*(B+x); 
      Wire.write(b);
      Checksum^=b; 
      }
    Checksum^=(SETTINGS_VERSION & 0xff); // Verwerk build in checksum om communicatie ussen verschillende versies te voorkomen 
    Wire.write(Checksum); 
    Wire.endTransmission(false); // verzend de data, sluit af maar geef de bus NIET vrij
    }
  Wire.endTransmission(true); // Geef de bus vrij
  }

//#######################################################################################################
//##################################### Transmission: HTTP  #############################################
//#######################################################################################################

#define IP_BUFFER_SIZE            256

#if NODO_MEGA
boolean EthernetInit(void)
  {
  int x;
  boolean Ok=false;
  byte Ethernet_MAC_Address[6];                                // MAC adres van de NodO

  if(!bitRead(HW_Config,HW_ETHERNET))
    return false;
    
  Ethernet_MAC_Address[0]=0xCC;
  Ethernet_MAC_Address[1]=0xBB;
  Ethernet_MAC_Address[2]=0xAA;
  Ethernet_MAC_Address[3]=(Settings.Home%10)+'0';
  Ethernet_MAC_Address[4]=(Settings.Unit/10)+'0';
  Ethernet_MAC_Address[5]=(Settings.Unit%10)+'0';

  // Initialiseer ethernet device  
  if((Settings.Nodo_IP[0] + Settings.Nodo_IP[1] + Settings.Nodo_IP[2] + Settings.Nodo_IP[3])==0)// Als door de user IP adres is ingesteld op 0.0.0.0 dan IP adres ophalen via DHCP
    {
    if(Ethernet.begin(Ethernet_MAC_Address)!=0) // maak verbinding en verzoek IP via DHCP
      Ok=true;
    }
  else      
    {
    Ethernet.begin(Ethernet_MAC_Address, Settings.Nodo_IP, Settings.DnsServer, Settings.Gateway, Settings.Subnet);
    Ok=true;
    }

  if(Ok) // Als er een IP adres is, dan HTTP en TelNet servers inschakelen
    {
    // Start server voor Terminalsessies via TelNet
    TerminalServer=EthernetServer(TERMINAL_PORT);
    TerminalServer.begin(); 

    // Start Server voor ontvangst van HTTP-Events
    HTTPServer=EthernetServer(Settings.OutputPort);
    HTTPServer.begin(); 

    if(Settings.TransmitIP==VALUE_ON && Settings.HTTPRequest[0]!=0)
      {
      // Haal IP adres op van de Host waar de nodo de HTTP events naar verzendt zodat niet iedere transactie een DNS-resolve plaats hoeft te vinden.
      // Haal uit het HTTP request URL de Host. Zoek naar de eerste slash in de opgegeven HTTP-Host adres
      char *TempString=(char*)malloc(80);
      x=StringFind(Settings.HTTPRequest,"/");
      strcpy(TempString,Settings.HTTPRequest);
      TempString[x]=0;
      EthernetClient HTTPClient;
      if(HTTPClient.connect(TempString,Settings.PortClient))   
        {
        HTTPClient.getRemoteIP(HTTPClientIP);
        Ok=true;
        bitWrite(HW_Config,HW_WEBAPP,1);
        delay(10); //even wachten op response van de server.
        HTTPClient.flush(); // gooi alles weg, alleen IP adres was van belang.
        HTTPClient.stop();
        }
      else
        {
        HTTPClientIP[0]=0;
        HTTPClientIP[1]=0;
        HTTPClientIP[2]=0;
        HTTPClientIP[3]=0;
        Serial.println(F("Error: No TCP/IP connection to host."));
        Ok=false;
        }
      free(TempString);    
      }
    else
      Ok=true;
    }
  return Ok;
  }

/*******************************************************************************************************\
 * Haal via een HTTP-request een file op
 * De content van de file bevindt zich in de body text die de server terugstuurt en wordt opgevangen
 * in de funktie SendHTTPRequest()
 *
 * Het naar de Nodo schrijven van een scriptfile werkt als volgt:
 *
 * 1. Webserver: Stuurt een regulier http-request naar de Nodo met daarin het commando 'FileGetHttp <filename>'
 * 2. Nodo: Stuurt een http-request terug met '&file=<filename>'
 * 3. WebServer: geeft een standaard response OK 200 terug en verzendt in dezelfde sessie de file als http-bodytext
 * 4. Nodo: ontvangt de OK 200 en slaat alle regels in de bodytext op in de opgegeven bestandsnaam
 * 5. Sessie wordt door server beeindigd en file op de Nodo wordt gesloten.
 * 
 * Een Nodo file bestaat uit acht posities zonder spaties, andere leestekens en zonder extentie.
 * 
 \*******************************************************************************************************/
byte GetHTTPFile(char* filename)
  {
  char *HttpRequest=(char*)malloc(INPUT_BUFFER_SIZE+1);
  char *TempString=(char*)malloc(INPUT_BUFFER_SIZE+1);
  byte Ok;

  strcpy(HttpRequest,"?id=");
  strcat(HttpRequest,Settings.ID);  
  strcat(HttpRequest,"&file=");
  strcat(HttpRequest,filename);

  if(Settings.Password[0]!=0)
    {
    // pin-code genereren en meesturen in het http-request
    strcpy(TempString,HTTPCookie);
    strcat(TempString,":");
    strcat(TempString,Settings.Password);  
    md5(TempString);
    strcat(HttpRequest,"&key=");
    strcat(HttpRequest,TempString); 
    }

  free(TempString);

  Ok=SendHTTPRequest(HttpRequest);
  free(HttpRequest);
  return Ok;
  }

/*******************************************************************************************************\
 * Verzend een event als HTTP-request 
 *
 \*******************************************************************************************************/
byte SendHTTPEvent(struct NodoEventStruct *Event)
  {
  byte Unit,x;

  if(Settings.TransmitIP!=VALUE_ON || Settings.HTTPRequest[0]==0)
    return false;

  char *HttpRequest=(char*)malloc(INPUT_BUFFER_SIZE+1);
  char *TempString=(char*)malloc(INPUT_BUFFER_SIZE+1);

  strcpy(HttpRequest,"?id=");
  strcat(HttpRequest,Settings.ID);  

  strcat(HttpRequest,"&unit=");
  strcat(HttpRequest,int2str(Event->SourceUnit));  

  if(Settings.Password[0]!=0)
    {
    // pin-code genereren en meesturen in het http-request
    strcpy(TempString,HTTPCookie);
    strcat(TempString,":");
    strcat(TempString,Settings.Password);
    md5(TempString);
    strcat(HttpRequest,"&key=");
    strcat(HttpRequest,TempString);    
    }

  strcat(HttpRequest,"&event=");
  Event2str(Event,TempString);
  strcat(HttpRequest,TempString);

  free(TempString);
  x=SendHTTPRequest(HttpRequest);
  free(HttpRequest);
  return x;
}


/*******************************************************************************************************\
 * Verzend een nieuwe cookie als HTTP request.
 *
 \*******************************************************************************************************/
boolean SendHTTPCookie(void)
{
  boolean Status;

  if(Settings.TransmitIP!=VALUE_ON)
    return false;

  char *HttpRequest=(char*)malloc(INPUT_BUFFER_SIZE+1);

  strcpy(HttpRequest,"?id=");
  strcat(HttpRequest,Settings.ID);  

  // Verzend tevens een nieuwe cookie voor het eerstvolgende event.
  RandomCookie(HTTPCookie);
  strcat(HttpRequest,"&cookie=");
  strcat(HttpRequest,HTTPCookie);

  Status=SendHTTPRequest(HttpRequest);
  free(HttpRequest);
  return Status;
  }


boolean SendHTTPRequest(char* Request)
  {
  if(Settings.TransmitIP!=VALUE_ON)
    return false;

  int InByteCounter,x,y,SlashPos;
  byte InByte;
  byte Try=0;
  unsigned long TimeoutTimer;
  char filename[13];
  const int TimeOut=5000;
  EthernetClient HTTPClient; // Client class voor HTTP sessie.
  byte State=0;
  char *IPBuffer=(char*)malloc(IP_BUFFER_SIZE+1);
  char *TempString=(char*)malloc(INPUT_BUFFER_SIZE+1);
  File BodyTextFile;
  struct NodoEventStruct TempEvent;

  // pluk de filename uit het http request als die er is.
  filename[0]=0;
  if(ParseString(Request,"file=", TempString,"&? "))
    {
    TempString[8]=0; // voorkom dat filenaam meer dan acht posities heeft
    strcpy(filename,TempString);                
    strcat(filename,".dat");
    SelectSDCard(true);
    FileErase(filename);
    SelectSDCard(false);
    }  

  strcpy(IPBuffer,"GET ");

  // Haal uit het HTTP request URL de Host. 
  // zoek naar de eerste slash in de opgegeven HTTP-Host adres
  SlashPos=StringFind(Settings.HTTPRequest,"/");
  if(SlashPos!=-1)
    strcat(IPBuffer,Settings.HTTPRequest+SlashPos);

  // Alle spaties omzetten naar %20 en toevoegen aan de te verzenden regel.
  y=strlen(IPBuffer);
  for(x=0;x<strlen(Request);x++)
    {            
    if(Request[x]==32)
      {
      IPBuffer[y++]='%';
      IPBuffer[y++]='2';
      IPBuffer[y++]='0';
      }
    else
      {
      IPBuffer[y++]=Request[x];
      }
    }
  IPBuffer[y]=0;

  // Sluit HTTP-request af met protocol versienummer
  strcat(IPBuffer," HTTP/1.1");

  // IPBuffer bevat nu het volledige HTTP-request, gereed voor verzending.

  if(Settings.Debug==VALUE_ON)
    {
    Serial.print(F("DEBUG: HTTP-Output="));
    Serial.println(IPBuffer);
    }

  strcpy(TempString,Settings.HTTPRequest);
  TempString[SlashPos]=0;

  do
    {
    if(HTTPClient.connect(HTTPClientIP,Settings.PortClient))
      {
      ClientIPAddress[0]=HTTPClientIP[0];
      ClientIPAddress[1]=HTTPClientIP[1];
      ClientIPAddress[2]=HTTPClientIP[2];
      ClientIPAddress[3]=HTTPClientIP[3];

      HTTPClient.println(IPBuffer);
      HTTPClient.print(F("Host: "));
      HTTPClient.println(TempString);
      HTTPClient.print(F("User-Agent: Nodo/Build="));
      HTTPClient.println(int2str(NODO_BUILD));             
      HTTPClient.println(F("Connection: Close"));
      HTTPClient.println();// Afsluiten met een lege regel is verplicht in http protocol/

      TimeoutTimer=millis()+TimeOut; // Als er te lange tijd geen datatransport is, dan wordt aangenomen dat de verbinding (om wat voor reden dan ook) is afgebroken.
      IPBuffer[0]=0;
      InByteCounter=0;

      while(TimeoutTimer>millis() && HTTPClient.connected())
        {
        if(HTTPClient.available())
          {
          InByte=HTTPClient.read();
          if(isprint(InByte) && InByteCounter<IP_BUFFER_SIZE)
            IPBuffer[InByteCounter++]=InByte;

          else if(InByte==0x0A)
            {
            IPBuffer[InByteCounter]=0;
            // De regel is binnen 

            if(Settings.Debug==VALUE_ON)
              {
              strcpy(TempString,"DEBUG: HTTP-Input=");
              strcat(TempString,IPBuffer);
              Serial.println(TempString);
              }
            TimeoutTimer=millis()+TimeOut; // er is nog data transport, dus de timeout timer weer op max. zetten.

            // State 0: wachten totdat in de initial-request line OK/200 voorbij komt en er dus response is van de server.
            if(State==0)
              {
              if(StringFind(IPBuffer,"200")!=-1 && StringFind(IPBuffer,"HTTP")!=-1)
                {
                State=1;
                }
              }
    
            // State 1: In deze state komen de header-lines voorbij. Deze state is klaar zodra er een lege header-line voorbij komt.
            else if(State==1)
              {            
              if(InByteCounter==0)
                {
                State=2;
                if(filename[0])
                  {
                  // Openen nieuw bestand.
                  SelectSDCard(true);
                  BodyTextFile = SD.open(filename, FILE_WRITE);
                  SelectSDCard(false);
                  }
                }
              else
                {
                // als de tijd wordt meegeleverd, dan de clock gelijkzetten.
                if(ClockSyncHTTP)
                  {
                  if(ParseString(IPBuffer,"time=", TempString,";, "))
                    {
                    ClearEvent(&TempEvent);
                    TempEvent.Command=CMD_CLOCK_TIME;
                    TempEvent.Par2=str2ultime(TempString);
                    if(TempEvent.Par2<0xffffffff)
                      ExecuteCommand(&TempEvent);

                    }            
                  if(ParseString(IPBuffer,"date=", TempString,";, "))
                    {
                    ClearEvent(&TempEvent);
                    TempEvent.Command=CMD_CLOCK_DATE;
                    TempEvent.Par2=str2uldate(TempString);
                    if(TempEvent.Par2<0xffffffff)
                      ExecuteCommand(&TempEvent);
                    }            
                  }            
                }
              }

            // State 2: In deze state komen de regels van de body-text voorbij
            else if(State==2)
              {
              // als bodytext moet worden opgeslagen in en bestand
              SelectSDCard(true);
              if(BodyTextFile)
                {
                BodyTextFile.write((uint8_t*)IPBuffer,strlen(IPBuffer));
                BodyTextFile.write('\n'); // nieuwe regel
                }
              SelectSDCard(false);
              }
            InByteCounter=0;          
            }
          }
        }

      // eventueel geopende bestand op SDCard afsluiten
      if(BodyTextFile)
        {
        SelectSDCard(true);
        BodyTextFile.close();
        SelectSDCard(false);
        }        
      delay(100);
      HTTPClient.flush();// Verwijder eventuele rommel in de buffer.
      HTTPClient.stop();
      }
    else
      {
      // niet gelukt om de TCP-IP verbinding op te zetten. Genereer error en her-initialiseer de ethernetkaart.
      State=0;
      delay(3000); // korte pause tussen de nieuwe pogingen om verbinding te maken.
      if(EthernetInit())
        CookieTimer=1;// gelijk een nieuwe cookie versturen.
      }
    }
  while(State==0 && ++Try<3);

  free(TempString);
  free(IPBuffer);

  if(!State)
    {
    x=Settings.TransmitIP; // HTTP tijdelijk uitzetten want die deed het immers niet.
    Settings.TransmitIP=VALUE_OFF; // HTTP tijdelijk uitzetten want die deed het immers niet.
    RaiseMessage(MESSAGE_07);
    Settings.TransmitIP=x; // HTTP weer terugzetten naar oorspronkelijke waarde.
    }

  return State;
  }


/*********************************************************************************************\
 * Deze routine haalt uit een string de waarde die bij de opgegeven parameter hoort
 * er wordt niet case sensitief gezocht naar KeyWord. Opvolgende spaties worden verwijderd en de tekens
 * tot aan de eerste worden toegevoed aan ResultString
 \*********************************************************************************************/
boolean ParseString(char* StringToParse,char* Keyword, char* ResultString,char* BreakChars)
  {
  int x,y,z;
  int Keyword_len=strlen(Keyword);
  int StringToParse_len=strlen(StringToParse);

  ResultString[0]=0;

  if(StringToParse_len<3) // doe geen moeite als de string te weinig tekens heeft.
    return -1;

  for(x=0; x<=(StringToParse_len-Keyword_len); x++)
    {
    y=0;
    while(y<Keyword_len && (tolower(StringToParse[x+y])==tolower(Keyword[y])))
      y++;

    z=x+y;
    if(y==Keyword_len)
      {
      // tekens die overgeslagen kunnen worden
      while(z<StringToParse_len && StringToParse[z]==' ')z++;

      x=0; // we komen niet meer terug in de 'for'-loop, daarom kunnen we x hier even gebruiken.
      y=0; // Vlag die aangeeft if een break teken is gevonden
      // Tekens waarbij afgebroken moet worden
      while(z<StringToParse_len && y<100)
        {
        if(StringToParse[z]=='+')
          ResultString[x]=' ';
        else if(StringToParse[z]=='%' && StringToParse[z+1]=='2' && StringToParse[z+2]=='0')
          {
          ResultString[x]=' ';
          z+=2;
          }
        else
          ResultString[x]=StringToParse[z];

        z++;
        x++;
        
        for(y=0;y<strlen(BreakChars);y++)
          if(BreakChars[y]==StringToParse[z])
            y=100; // stop tekens toevoegen aan resultstring
        }
        
      ResultString[x]=0;
      
      return true;
      }
    }
  return false;
  }

void ExecuteIP(void)
  {
  char InByte;
  boolean RequestCompleted=false;  
  boolean Completed=false;
  int Protocol=0;
  int InByteCounter;
  boolean RequestEvent=false;
  boolean ExecuteEvent=false;
  int x,y;
  unsigned long TimeoutTimer=millis() + 60000; //
 
  char *InputBuffer_IP = (char*) malloc(IP_BUFFER_SIZE+1);
  char *Event          = (char*) malloc(INPUT_BUFFER_SIZE+1);
  char *TmpStr1        = (char*) malloc(INPUT_BUFFER_SIZE+1);
  char *TmpStr2        = (char*) malloc(40); 

  Event[0]=0; // maak de string leeg.

  HTTPClient=HTTPServer.available();

  if(HTTPClient)
    {
    HTTPClient.getRemoteIP(ClientIPAddress);  

    // Controleer of het IP adres van de Client geldig is. 
    if((Settings.Client_IP[0]!=0 && ClientIPAddress[0]!=Settings.Client_IP[0]) ||
      (Settings.Client_IP[1]!=0 && ClientIPAddress[1]!=Settings.Client_IP[1]) ||
      (Settings.Client_IP[2]!=0 && ClientIPAddress[2]!=Settings.Client_IP[2]) ||
      (Settings.Client_IP[3]!=0 && ClientIPAddress[3]!=Settings.Client_IP[3]))
      {
      RaiseMessage(MESSAGE_10);
      }
    else
      {
      InByteCounter=0;
      while(HTTPClient.connected()  && !Completed && TimeoutTimer>millis())
        {
        if(HTTPClient.available()) 
          {
          InByte=HTTPClient.read();

          if(isprint(InByte) && InByteCounter<IP_BUFFER_SIZE)
            {
            InputBuffer_IP[InByteCounter++]=InByte;
            }

          else if((InByte==0x0D || InByte==0x0A))
            {
            InputBuffer_IP[InByteCounter]=0;
            InByteCounter=0;

            if(Settings.Debug==VALUE_ON)
              {
              Serial.print(F("DEBUG: HTTP-Input="));
              Serial.println(InputBuffer_IP);
              }

            // Kijk of het een HTTP-request is
            if(Protocol==0)
              {
              if(StringFind(InputBuffer_IP,"GET")!=-1)
                Protocol=VALUE_SOURCE_HTTP;// HTTP-Request
              }

            if(Protocol==VALUE_SOURCE_HTTP)
              {
              if(!RequestCompleted)
                {
                Completed=true;

                // als de beveiliging aan staat, dan kijken of de juiste pin ip meegegeven in het http-request. x is vlag voor toestemming verwerking event
                x=false;
                if(Settings.Password[0]!=0)
                  {
                  sprintf(TmpStr2,"%s:%s",HTTPCookie,Settings.Password);  
                  md5(TmpStr2);

                  if(ParseString(InputBuffer_IP,"key=",TmpStr1,"&? "))
                    {
                    if(strcmp(TmpStr2,TmpStr1)==0)
                      x=true;
                    }
                  }
                else
                  x=true;

                if(x)
                  {                
                  if(ParseString(InputBuffer_IP,"event=",Event,"&? "))
                    {
                    RequestEvent=true;
                    RequestCompleted=true;
                    strcpy(TmpStr1,"HTTP/1.1 200 Ok");
                    HTTPClient.println(TmpStr1);
                    HTTPClient.println(TmpStr1);
                    ExecuteEvent=true;
                    }
                  else
                    HTTPClient.println(F("HTTP/1.1 400 Bad Request"));
                  }
                else                    
                  HTTPClient.println(F("HTTP/1.1 403 Forbidden"));
                }

              HTTPClient.println(F("Content-Type: text/html"));
              HTTPClient.print(F("Server: Nodo/Build="));
              HTTPClient.println(int2str(NODO_BUILD));             
              if(bitRead(HW_Config,HW_CLOCK))
                {
                HTTPClient.print(F("Date: "));
                HTTPClient.println(DateTimeString());             
                }
              HTTPClient.println(""); // HTTP Request wordt altijd afgesloten met een lege regel

              // Haal nu de resultaten op van het verwerken van de binnengekomen HTTP-regel. Stuur de inhoud als bodytext terug
              // naar de client. De WebApp zal deze content uitparsen. Indien toegang via browser, dan wordt het verwerkings-
              // resultaat getoond in de browser.
              if(ExecuteEvent)
                {              
                // Voer binnengekomen event uit
                RaiseMessage(ExecuteLine(Event, Protocol));
                }
              } // einde HTTP-request
            }
          else
            {
            // Er is geen geldig teken binnen gekomen. Even wachten en afbreken.
            delay(1000);
            Completed=true;
            }
          }
        }
      }
    delay(100);  // korte pauze om te voorkomen dat de verbinding wordt verbroken alvorens alle data door client verwerkt is.
    HTTPClient.flush();// Verwijder eventuele rommel in de buffer.
    HTTPClient.stop();
    }

  free(TmpStr1);
  free(TmpStr2);
  free(InputBuffer_IP);
  free(Event);
  return;
  }  

#endif //MEGA

//#######################################################################################################
//##################################### Transmission: SERIAL  ###########################################
//#######################################################################################################

#define XON                       0x11
#define XOFF                      0x13

void SerialHold(boolean x)
{
  static boolean previous=true;

  if(x==previous)
    return;
  else
  {
    if(x)
      Serial.write(XOFF);
    else
      Serial.write(XON);
    previous=x;
  }
}


//#######################################################################################################
//##################################### Transmission: Legacy!  ###########################################
//#######################################################################################################

/*********************************************************************************************\
* Deze routine berekent de uit een RawSignal een NODO code van het type UserEvent
* Geeft een false retour als geen geldig NODO signaal.
* Deze funktie is opgenomen om compatibibel te zijn met universele afstandsbedieningen
* die de gebruiker met de oude Nodo versies heeft geprogrammeerd.
\*********************************************************************************************/
boolean RawSignal_2_Nodo_OLD(struct NodoEventStruct *Event)
  {
  unsigned long bitstream=0L;
  int x,y,z;

  static unsigned long PreviousTime=0;
  static unsigned long PreviousBitstream=0;

  // NODO signaal bestaat uit start bit + 32 bits. Als ongelijk aan 66, dan geen Nodo signaal
  if(RawSignal.Number!=66)return 0L;

  // 0=aantal, 1=startpuls, 2=space na startpuls, 3=1e pulslengte. Dus start loop met drie.
  z=0;
  for(x=3;x<=RawSignal.Number;x+=2)
    {
    if((RawSignal.Pulses[x]*RawSignal.Multiply)>NODO_PULSE_MID)      
      bitstream|=(long)(1L<<z); //LSB in signaal wordt  als eerste verzonden
    z++;
    }
  
  // We hoeven alleen maar compatible te zijn met de userevents van de oude Nodo.
  // in code 100 heeft in de vorige versies altijd het userevent gezeten.
  if(((bitstream>>16)&0xff)!=100) 
    return false;

  // Indien het een RawSignal was, dan kunnen we niet achterhalen of het een zinvol en netjes afgerond signaal was
  // we gebruiken daarom de herhalingen die de zender uitzendt als checksum. Komt het event een tweede maal binnen
  // dan kan worden aangenomen dat het een goed signaal was.
  if(bitstream!=PreviousBitstream || PreviousTime>millis())
    {
    PreviousBitstream=bitstream;
    return false;
    }    
  PreviousTime=millis()+SIGNAL_REPEAT_TIME;
  PreviousBitstream=0L;

  ClearEvent(Event);
  Event->SourceUnit=(bitstream>>24)&0xf;
  Event->DestinationUnit=0;
  Event->Flags=0;
  Event->Command=EVENT_USEREVENT;
  Event->Par1=(bitstream>>8)&0xff;
  Event->Par2=bitstream&0xff;

  // Het oude Nodo signaal herhaald zich enkele malen. Deze herhalingen zijn nodig om een valide checksum op te leveren maar
  // kunnen onterecht ook nieuwe events opleveren. Om deze reden moet er voor dit type event wachttijd worden toegepast

  return true;  
  }


