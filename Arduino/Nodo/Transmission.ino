#define NODO_PULSE_0               500  // PWM: Tijdsduur van de puls bij verzenden van een '0' in uSec.
#define NODO_PULSE_MID            1000  // PWM: Pulsen langer zijn '1'
#define NODO_PULSE_1              1500  // PWM: Tijdsduur van de puls bij verzenden van een '1' in uSec. (3x NODO_PULSE_0)
#define NODO_SPACE                 500  // PWM: Tijdsduur van de space tussen de bitspuls bij verzenden van een '1' in uSec.   
#define SIGNAL_ANALYZE_SHARPNESS    50  // Scherpte c.q. foutmarge die gehanteerd wordt bij decoderen van RF/IR signaal.
#define MIN_PULSE_LENGTH           100  // pulsen korter dan deze tijd uSec. worden als stoorpulsen beschouwd.
#define MIN_RAW_PULSES              32  // =16 bits. Minimaal aantal ontvangen bits*2 alvorens cpu tijd wordt besteed aan decodering, etc. Zet zo hoog mogelijk om CPU-tijd te sparen en minder 'onzin' te ontvangen.
#define SWITCH_TIME_RX_TX_IR        50  // Minimale tijd tussen omschakelen van ontvangen naar zenden. 
#define SWITCH_TIME_RX_TX_RF       500  // Minimale tijd tussen omschakelen van ontvangen naar zenden. 
#define SWITCH_TIME_RX_TX_I2C       50  // Minimale tijd tussen omschakelen van ontvangen naar zenden. 
#define MIN_TIME_BETWEEN_TX        100
#define WAIT_FREE_RX_WINDOW        250

boolean AnalyzeRawSignal(struct NodoEventStruct *E)
  {
  if(RawSignal.Number==RAW_BUFFER_SIZE)return false;     // Als het signaal een volle buffer beslaat is het zeer waarschijnlijk ruis of ongeldig signaal
  ClearEvent(E);

  //  #ifdef USER_PLUGIN
  //  Code=RawSignal_2_UserPlugin(RawSignal.Source);
  //  if(Code==1)
  //    return 0;
  //  else if(Code>1)
  //    return Code;
  //  #endif // weer herstellen.???

  if(RawSignal_2_Nodo(E))           // Is het een Nodo signaal
    return true;

#if NODO_30_COMPATIBLE
  if(RawSignal_2_Nodo_OLD(E))       // Is het een Nodo signaal: oude 32-bit format.
    return true;
#endif

  if(I2C_EventReceived)
   {
    // Er is een I2C event binnen gekomen. De gegevens kunnen dan eenvoudig uit de binnengekomen struct worden gekopieerd. 
    *E=I2C_Event;
    bitWrite(HW_Config,HW_I2C,true);
    I2C_EventReceived    = false;
    return true;
   }

  if(Transmission_NodoOnly)
    return false;

  for(byte x=0;x<DEVICE_MAX && E->Command==0; x++)
    if(Device_ptr[x]!=0)
      if(Device_ptr[x](DEVICE_RAWSIGNAL_IN,E,0))
        return true;

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
  static unsigned long Previous,PreviousTime; // t.b.v. onderdrukken herhaal binnengekomen signalen.

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

  event->Par2=(event->Par2&0x0fffffff)|((unsigned long)SIGNAL_TYPE_UNKNOWN<<28);

  // Indien het een RawSignal was, dan kunnen we niet achterhalen of het een zinvol en netjes afgerond signaal was
  // we gebruiken daarom de herhalingen die de zender uitzendt als checksum. Komt het event een tweede maal binnen
  // dan kan worden aangenomen dat het een goed signaal was.
  if(event->Par2!=Previous || PreviousTime>millis())
    {
    Previous=event->Par2;
    return false;
    }
    
  RawSignal.Repeats    = true; // het is een herhalend signaal. Bij ontvangst herhalingen onderdrukken.
  event->SourceUnit=0;  
  event->DestinationUnit=0;
  event->Command=CMD_RAWSIGNAL;
  event->Par1=0;
  PreviousTime=millis()+SIGNAL_REPEAT_TIME;
  Previous=0L;
  return true;
  }


/**********************************************************************************************\
 * Deze functie wacht totdat de 433 band vrij is of er een timeout heeft plaats gevonden 
 * Window en delay tijd in milliseconden
 \*********************************************************************************************/
#define WAITFREE_TIMEOUT             30000 // tijd in ms. waarna het wachten wordt afgebroken als er geen ruimte in de vrije ether komt
void WaitFree(int Window)
  {
  unsigned long Timer, TimeOutTimer;  
  
  // dan kijken of de ether vrij is.
  Timer=millis()+Window;                                                   // reset de timer.
  TimeOutTimer=millis()+WAITFREE_TIMEOUT;                                  // tijd waarna de routine wordt afgebroken in milliseconden

  while(Timer>millis() && TimeOutTimer>millis())
    {
    if((*portInputRegister(IRport)&IRbit)==0)                              // Kijk of er iets op de IR poort binnenkomt. (Pin=LAAG als signaal in de ether).
      if(FetchSignal(PIN_IR_RX_DATA,LOW,SIGNAL_TIMEOUT_IR))                // Als het een duidelijk IR signaal was
        Timer=0;                                                           // zorg dan dat de timer weer gereset wordt.

    if((*portInputRegister(RFport)&RFbit)==RFbit)                          // Kijk if er iets op de RF poort binnenkomt. (Pin=HOOG als signaal in de ether). 
      if(FetchSignal(PIN_RF_RX_DATA,HIGH,SIGNAL_TIMEOUT_RF))               // Als het een duidelijk signaal was
        Timer=0;                                                           // zorg dan dat de timer weer gereset wordt.
        
    if(Timer==0)
      {
      Led(BLUE);
//      delay((Settings.Unit-1)*100);                                        // Unit afhankelijke wachttijd om te voorkomen dat alle Nodo's tegelijk gaan zenden.
      Timer=millis()+Window;                                               // reset de timer weer.
      }
    }

  Led(RED);
  }


/**********************************************************************************************\
 * Wacht totdat de pin verandert naar status state. Geeft de tijd in uSec. terug. 
 * Als geen verandering, dan wordt na timeout teruggekeerd met de waarde 0L
 \*********************************************************************************************/
unsigned long WaitForChangeState(uint8_t pin, uint8_t state, unsigned long timeout)
  {
  uint8_t bit = digitalPinToBitMask(pin);
  uint8_t port = digitalPinToPort(pin);
  uint8_t stateMask = (state ? bit : 0);
  unsigned long numloops = 0; // keep initialization out of time critical area
  unsigned long maxloops = microsecondsToClockCycles(timeout) / 19;

  // wait for the pulse to stop. One loop takes 19 clock-cycles
  while((*portInputRegister(port) & bit) == stateMask)
    if (numloops++ == maxloops)
      return 0;//timeout opgetreden
  return clockCyclesToMicroseconds(numloops * 19 + 16); 
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
  byte PTMF=RawSignal.Pulses[0];

  delay(5);// kleine pause om de zender de tijd te geven om stabiel te worden 
  noInterrupts();

  for(byte y=0; y<RawSignal.Repeats; y++) // herhaal verzenden RF code
    {
    x=1;
    while(x<RawSignal.Number)
      {
      digitalWrite(PIN_RF_TX_DATA,HIGH); // 1
      delayMicroseconds(RawSignal.Pulses[x++]*PTMF); 
      digitalWrite(PIN_RF_TX_DATA,LOW); // 0
      delayMicroseconds(RawSignal.Pulses[x++]*PTMF); 
      }
    }

  digitalWrite(PIN_RF_TX_VCC,LOW); // zet de 433Mhz zender weer uit
  digitalWrite(PIN_RF_RX_VCC,HIGH); // Spanning naar de RF ontvanger weer aan.

#ifdef NODO_MEGA
  // Board specifiek: Genereer een korte puls voor omschakelen van de Aurel tranceiver van TX naar RX mode.
  //  if(HW_Config&0xf==BIC_HWMESH_NES_V1X) ??? Nog in testfase.
  //    {
  delayMicroseconds(36);
  digitalWrite(PIN_BSF_0,LOW);
  delayMicroseconds(16);
  digitalWrite(PIN_BSF_0,HIGH);
  //    }
#endif

  interrupts();
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
  int repeat;
  byte PTMF=RawSignal.Pulses[0];
  
  // kleine pause zodat verzenden event naar de USB poort gereed is, immers de IRQ's worden tijdelijk uitgezet
  delay(10);

  for(repeat=0; repeat<RawSignal.Repeats; repeat++) // herhaal verzenden IR code
    {
    pulse=1;
    noInterrupts(); // interrupts tijdelijk uitschakelen om zo en zuiverder signaal te krijgen
    while(pulse<RawSignal.Number)
      {
      // Mark verzenden. Bereken hoeveel pulsen van 26uSec er nodig zijn die samen de lengte van de mark zijn.
      mod=(RawSignal.Pulses[pulse++]*PTMF)/26; // delen om aantal pulsen uit te rekenen

      while(mod)
        {
        // Hoog
        #ifdef NODO_MEGA
        bitWrite(PORTH,0, HIGH);
        #else
        bitWrite(PORTB,3, HIGH);
        #endif
  
        delayMicroseconds(12);
        __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");// per nop 62.6 nano sec. @16Mhz
  
        // Laag
        #ifdef NODO_MEGA
        bitWrite(PORTH,0, LOW);    
        #else
        bitWrite(PORTB,3, LOW);
        #endif
        delayMicroseconds(12);
        __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");// per nop 62.6 nano sec. @16Mhz
        mod--;
        }
      // Laag
      delayMicroseconds(RawSignal.Pulses[pulse++]*PTMF);
    }
  interrupts(); // interupts weer inschakelen.
  }
}

/*********************************************************************************************\
 * Deze routine berekend de RAW pulsen van een Nodo event en plaatst deze in de buffer RawSignal
 * RawSignal.Bits het aantal pulsen*2+startbit*2 ==> 130
 \*********************************************************************************************/

// Definieer een datablock die gebruikt wordt voor de gegevens die via de ether verzonden moeten worden.
// Zo kunnen exact die gevens worden verzonden die nodig zijn en niets teveel.  
struct DataBlockStruct
  {
  byte SourceUnit;
  byte DestinationUnit;
  byte Flags;
  byte Checksum;
  byte Command;
  byte Par1;
  unsigned long Par2;
  };  

void Nodo_2_RawSignal(struct NodoEventStruct *Event)
  {
  byte BitCounter=1;
  RawSignal.Repeats=1;
  const byte PTMF=100;
  RawSignal.Pulses[0]=PTMF;

  struct DataBlockStruct DataBlock;
  DataBlock.SourceUnit=Event->SourceUnit;  
  DataBlock.DestinationUnit=Event->DestinationUnit;
  DataBlock.Flags=Event->Flags;
  DataBlock.Checksum=Event->Checksum;
  DataBlock.Command=Event->Command;
  DataBlock.Par1=Event->Par1;
  DataBlock.Par2=Event->Par2;

  // bereken checksum: crc-8 uit alle bytes in de struct
  byte c=0,*B=(byte*)&DataBlock;
  for(byte x=0;x<sizeof(struct DataBlockStruct);x++)
    c^=*(B+x); 
  DataBlock.Checksum=c;

  // begin met een lange startbit. Veilige timing gekozen zodat deze niet gemist kan worden
  RawSignal.Pulses[BitCounter++]=(NODO_PULSE_1*4)/PTMF; 
  RawSignal.Pulses[BitCounter++]=(NODO_SPACE*2)/PTMF;

  for(byte x=0;x<sizeof(struct DataBlockStruct);x++)
    {
    for(byte Bit=0; Bit<=7; Bit++)
      {
      if((*(B+x)>>Bit)&1)
        RawSignal.Pulses[BitCounter++]=NODO_PULSE_1/PTMF; 
      else
        RawSignal.Pulses[BitCounter++]=NODO_PULSE_0/PTMF;   
      RawSignal.Pulses[BitCounter++]=NODO_SPACE/PTMF;   
      }
    }

  RawSignal.Pulses[BitCounter-1]=NODO_SPACE/PTMF; // pauze tussen de pulsreeksen
  RawSignal.Number=BitCounter;
  }


/**********************************************************************************************\
 * Haal de pulsen en plaats in buffer. Op het moment hier aangekomen is de startbit actief.
 * bij de TSOP1738 is in rust is de uitgang hoog. StateSignal moet LOW zijn
 * bij de 433RX is in rust is de uitgang laag. StateSignal moet HIGH zijn
 * 
 \*********************************************************************************************/

boolean FetchSignal(byte DataPin, boolean StateSignal, int TimeOut)
  {
  int RawCodeLength=1;
  unsigned long PulseLength=0;
  const byte PTMF=50;
  RawSignal.Pulses[0]=PTMF;


  do{// lees de pulsen in microseconden en plaats deze in een tijdelijke buffer
    PulseLength=WaitForChangeState(DataPin, StateSignal, TimeOut)/PTMF;

    // bij kleine stoorpulsen die geen betekenig hebben zo snel mogelijk weer terug
    if(PulseLength<(MIN_PULSE_LENGTH/PTMF))
      return false;

    RawSignal.Pulses[RawCodeLength++]=PulseLength;
    PulseLength=WaitForChangeState(DataPin, !StateSignal, TimeOut)/PTMF;
    RawSignal.Pulses[RawCodeLength++]=PulseLength;
    }
  while(RawCodeLength<RAW_BUFFER_SIZE && PulseLength!=0);// Zolang nog niet alle bits ontvangen en er niet vroegtijdig een timeout plaats vindt

  if(RawCodeLength>=MIN_RAW_PULSES)
    {
    RawSignal.Number=RawCodeLength-1;
    if(DataPin==PIN_IR_RX_DATA)
      {
      // er is een IR signaal binnen gekomen. Zet de hardware vlag voor IR
      bitWrite(HW_Config,HW_IR_RX,1);

      // dit houdt dan ook in dat er geen pulsen worden geteld. Schakel de pulsentellerfaciliteit uit
      bitWrite(HW_Config,HW_IR_PULSE,0);
      detachInterrupt(PULSE_IRQ); // IRQ behorende bij PIN_IR_RX_DATA
      }
    if(DataPin==PIN_RF_RX_DATA)
      bitWrite(HW_Config,HW_RF_RX,1);
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
  }  
}

/**********************************************************************************************\
 * verzendt een event en geeft dit tevens weer op SERIAL
 * Als UseRawSignal=true, dan wordt er geen signaal opgebouwd, maar de actuele content van de
 * RawSignal buffer gebruikt. In dit geval werkt de WaitFree niet.
 \*********************************************************************************************/
boolean SendEvent(struct NodoEventStruct *ES, boolean UseRawSignal, boolean Display)
  {  
  ES->Direction=VALUE_DIRECTION_OUTPUT;
  byte Port=ES->Port;

  // Als een andere Nodo actief is en excusief communiceert met een andere Nodo, c.q. de ruimte geclaimd is, dan mag deze Nodo niet zenden.
  // In dit geval resteert deze Nodo niets anders dan even te wachten tot de lijn weer vrijgegeven wordt of de wachttijd verlopen is.
  // Als er een timeout optreedt, dan de blokkade opheffen. Dit ter voorkoming dat Nodo's oneindig wachten op vrije lijn.
  // Uitzondering is als de Nodo zelf de master was, dan deze mag altijd zenden.

  if(Transmission_SelectedUnit!=0 && Transmission_SelectedUnit!=Settings.Unit && !Transmission_ThisUnitIsMaster)
    if(!Wait(30,true,0,false))
    {
    Transmission_SelectedUnit=0;//??? testen 
    Transmission_NodoOnly=false;
    }

  // Stuur afhankelijk van de instellingen het event door naar I2C, RF, IR. Eerst wordt het event geprint,daarna een korte wachttijd om
  // te zorgen dat er een minimale wachttijd tussen de signlen zit. Tot slot wordt het signaal verzonden.

  // Verstuur signaal als I2C
  if(Port==VALUE_SOURCE_I2C || (bitRead(HW_Config,HW_I2C) && Port==VALUE_ALL))
    {
    ES->Port=VALUE_SOURCE_I2C;
    if(Display)PrintEvent(ES);
    DelayTransmission(VALUE_SOURCE_I2C,false);
    SendI2C(ES);
    delay(MIN_TIME_BETWEEN_TX);//???
    }

  if(Port==VALUE_SOURCE_RF || Port==VALUE_SOURCE_IR ||(Settings.TransmitRF==VALUE_ON && Port==VALUE_ALL))
    if(Settings.WaitFree==VALUE_ON && UseRawSignal==false)
      WaitFree(WAIT_FREE_RX_WINDOW); //??? unit afhankelijke pauze

  if(!UseRawSignal)
    Nodo_2_RawSignal(ES);

  // Verstuur signaal als IR
  if(Settings.TransmitIR==VALUE_ON && (Port==VALUE_SOURCE_IR || Port==VALUE_ALL))
    { 
    ES->Port=VALUE_SOURCE_IR;
    if(Display)PrintEvent(ES);
    DelayTransmission(VALUE_SOURCE_IR,false);
    RawSendIR();
    delay(MIN_TIME_BETWEEN_TX);
    }

  // Verstuur signaal als RF
  if(Settings.TransmitRF==VALUE_ON && (Port==VALUE_SOURCE_RF || Port==VALUE_ALL))
    {
    ES->Port=VALUE_SOURCE_RF;
    if(Display)PrintEvent(ES);
    DelayTransmission(VALUE_SOURCE_RF,false);
    RawSendRF();
    delay(MIN_TIME_BETWEEN_TX);
    }

#ifdef NODO_MEGA
  // Verstuur signaal als HTTP-event.
  if(bitRead(HW_Config,HW_ETHERNET))// Als Ethernet shield aanwezig.
    {
    if(Settings.TransmitIP==VALUE_ON && (Port==VALUE_SOURCE_HTTP || Port==VALUE_ALL))
      {
      SendHTTPEvent(ES);
      ES->Port=VALUE_SOURCE_HTTP;
      if(Display)PrintEvent(ES);
      }
    }
#endif 
  }
  
#ifdef NODO_MEGA
#endif


/*********************************************************************************************\
 * Deze routine berekent de uit een RawSignal een NODO code
 * Geeft een false retour als geen geldig NODO signaal
 \*********************************************************************************************/
boolean RawSignal_2_Nodo(struct NodoEventStruct *Event)
  {
  byte b,x,y,z;
  const byte PTMF=RawSignal.Pulses[0];

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
      if((RawSignal.Pulses[z]*PTMF)>NODO_PULSE_MID)      
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
    
  if(b==0)
    {
    RawSignal.Repeats    = false; // het is een herhalend signaal. Bij ontvangst herhalingen onderdrukken.
    Event->SourceUnit=DataBlock.SourceUnit;  
    Event->DestinationUnit=DataBlock.DestinationUnit;
    Event->Flags=DataBlock.Flags;
    Event->Checksum=DataBlock.Checksum;
    Event->Command=DataBlock.Command;
    Event->Par1=DataBlock.Par1;
    Event->Par2=DataBlock.Par2;
    return true;
    }
  else
    Trace("Checksum error.",b);
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
 * Hoewel dit wel realiseerbaar is, is hier niet voor gekozen omdat dit tegen de i2C-specificaties
 * in druist. Daarom wordt een event altijd naar alle I2C adressen van alle Nodo's gestuurd.
 * Voor 32 unit nummers neemt deze actie 18 milliseconden in beslag. Nog steeds zeer snel ten 
 * opzichte van RF, HTTP en IR.
 \*********************************************************************************************/
boolean SendI2C(struct NodoEventStruct *EventBlock)
{  
  byte x;

  // bereken checksum: crc-8 uit alle bytes in de queue.
  byte b,*B=(byte*)EventBlock;
  delay(10);
  for(int y=0;y<UNIT_MAX;y++)
  {            
    // verzend Event 
    Wire.beginTransmission(I2C_START_ADDRESS+y);
    byte Checksum=0;
    for(x=0;x<sizeof(struct NodoEventStruct);x++)
    {
      b=*(B+x); 
      Wire.write(b);
      Checksum^=b; 
    }
    Wire.write(Checksum); 
    Wire.endTransmission(false); // verzend de data, sluit af maar geef de bus NIET vrij
  }
  Wire.endTransmission(true); // Geef de bus vrij
}

//#######################################################################################################
//##################################### Transmission: HTTP  #############################################
//#######################################################################################################

#define IP_BUFFER_SIZE            256

#ifdef NODO_MEGA
boolean EthernetInit(void)
  {
  int x;
  boolean Ok=false;
  byte Ethernet_MAC_Address[6];                                // MAC adres van de Nodo.

  if(!bitRead(HW_Config,HW_ETHERNET))
    return false;
    
  Ethernet_MAC_Address[0]=0xAA;
  Ethernet_MAC_Address[1]=0xBB;
  Ethernet_MAC_Address[2]=0xCC;
  Ethernet_MAC_Address[3]='_';
  Ethernet_MAC_Address[4]=(Settings.Unit/10)+'0';
  Ethernet_MAC_Address[5]=(Settings.Unit%10)+'0';

  // Initialiseer ethernet device  
  if((Settings.Nodo_IP[0] + Settings.Nodo_IP[1] + Settings.Nodo_IP[2] + Settings.Nodo_IP[3])==0)// Als door de user IP adres is ingesteld op 0.0.0.0 dan IP adres ophalen via DHCP
    {
    if(Ethernet.begin(Ethernet_MAC_Address)!=0) // maak verbinding en verzoek IP via DHCP
      Ok=true;
    else
      Ok=false;
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
  EthernetClient HTTPClient;                            // Client class voor HTTP sessie.
  byte State=0;// 0 als start, 
  // 1 als 200 OK voorbij is gekomen,
  // 2 als &file= is gevonden en eerstvolgende lege regel moet worden gedetecteerd
  // 3 als lege regel is gevonden en file-capture moet starten.                

  char *IPBuffer=(char*)malloc(IP_BUFFER_SIZE+1);
  char *TempString=(char*)malloc(INPUT_BUFFER_SIZE+1);

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
    Serial.println(IPBuffer);

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
          // DEBUG *** Serial.write(InByte);//???
          if(isprint(InByte) && InByteCounter<IP_BUFFER_SIZE)
            IPBuffer[InByteCounter++]=InByte;

          else if(InByte==0x0A)
          {
            IPBuffer[InByteCounter]=0;
            if(Settings.Debug==VALUE_ON)
            {
              strcpy(TempString,"DEBUG HTTP Input: ");
              strcat(TempString,IPBuffer);
              Serial.println(TempString);
            }

            TimeoutTimer=millis()+TimeOut; // er is nog data transport, dus de timeout timer weer op max. zetten.

            // De regel is binnen 
            if(State==2 && InByteCounter==0) // als lege regel in HTTP request gevonden
              State=3;

            else if(State==3)
              AddFileSDCard(filename,IPBuffer); // Capture de bodytext uit het HTTP-request en sla regels op in opgegeven filename

            else if(State==0 && StringFind(IPBuffer,"HTTP")!=-1)
            {
              // Response n.a.v. HTTP-request is ontvangen
              if(StringFind(IPBuffer,"200")!=-1)
              {
                State=1;
                // pluk de filename uit het http request als die er is, dan de body text van het HTTP-request opslaan.
                if(ParseHTTPRequest(Request,"file", TempString))
                {
                  State=2;
                  TempString[8]=0; // voorkom dat filenaam meer dan acht posities heeft
                  strcpy(filename,TempString);                
                  strcat(filename,".dat");

                  // evntueel vorig bestand wissen
                  FileErase(filename);
                }
              }
              IPBuffer[InByteCounter]=0;
            }
            InByteCounter=0;          
          }
        }
      }
      delay(100);
      HTTPClient.flush();// Verwijder eventuele rommel in de buffer.
      HTTPClient.stop();
      State=true;
    }
    else
    {
      // niet gelukt om de TCP-IP verbinding op te zetten. Genereerd error en herinitialiseer de ethernetkaart.
      State=false;
      delay(1000); // korte pause tussen de nieuwe pogingen om verbinding te maken.
      if(EthernetInit())
        CookieTimer=1;// gelijk een nieuwe cookie versturen.
    }
  }
  while(!State && ++Try<3);

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
 * Deze routine haalt uit een http request de waarden die bij de opgegeven parameter hoort
 * Niet case-sinsitive.
 \*********************************************************************************************/
boolean ParseHTTPRequest(char* HTTPRequest,char* Keyword, char* ResultString)
{
  int x,y,z;
  int Keyword_len=strlen(Keyword);
  int HTTPRequest_len=strlen(HTTPRequest);

  ResultString[0]=0;

  if(HTTPRequest_len<3) // doe geen moeite als de string te weinig tekens heeft
    return -1;

  for(x=0; x<=(HTTPRequest_len-Keyword_len); x++)
  {
    y=0;
    while(y<Keyword_len && (tolower(HTTPRequest[x+y])==tolower(Keyword[y])))
      y++;

    z=x+y;
    if(y==Keyword_len && HTTPRequest[z]=='=' && (HTTPRequest[x-1]=='?' || HTTPRequest[x-1]=='&' || HTTPRequest[x-1]==' ')) // als tekst met een opvolgend '=' teken is gevonden
    {
      // Keyword gevonden. sla spaties en '=' teken over.

      //Test tekens voor Keyword
      while(z<HTTPRequest_len && (HTTPRequest[z]=='=' || HTTPRequest[z]==' '))z++;

      x=0; // we komen niet meer terug in de 'for'-loop, daarom kunnen we x hier even gebruiken.
      while(z<HTTPRequest_len && HTTPRequest[z]!='&' && HTTPRequest[z]!=' ')
      {
        if(HTTPRequest[z]=='+')
          ResultString[x]=' ';
        else if(HTTPRequest[z]=='%' && HTTPRequest[z+1]=='2' && HTTPRequest[z+2]=='0')
        {
          ResultString[x]=' ';
          z+=2;
        }
        else
          ResultString[x]=HTTPRequest[z];

        z++;
        x++;
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
  boolean SendBodyText=false;
  int x,y;
  unsigned long TimeoutTimer=millis() + 5000; // Na enkele seconden moet de gehele transactie gereed zijn, anders 'hik' in de lijn.

  char *InputBuffer_IP = (char*) malloc(IP_BUFFER_SIZE+1);
  char *Event          = (char*) malloc(INPUT_BUFFER_SIZE+1);
  char *TmpStr1        = (char*) malloc(INPUT_BUFFER_SIZE+1);
  char *TmpStr2        = (char*) malloc(40); 

  Event[0]=0; // maak de string leeg.

  EthernetClient HTTPClient=HTTPServer.available();

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
              Serial.println(InputBuffer_IP);

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

                  if(ParseHTTPRequest(InputBuffer_IP,"key",TmpStr1))
                    {
                    if(strcmp(TmpStr2,TmpStr1)==0)
                      x=true;
                    }
                  }
                else
                  x=true;

                if(x)
                  {                
                  if(ParseHTTPRequest(InputBuffer_IP,"event",Event))
                    {
                    RequestEvent=true;
                    RequestCompleted=true;
                    strcpy(TmpStr1,"HTTP/1.1 200 Ok");
                    HTTPClient.println(TmpStr1);
                    HTTPClient.println(TmpStr1);
                    
                    // De resultaten die anders naar de terminal waren gestuurd nu eveneens naar file schrijven
                    // De filehandler is globaal gedefinieerd en schrijven vindt plaatsin de funktie PrintTerminal();
                    SelectSDCard(true);
                    HTTPResultFile = SD.open(ProgmemString(Text_06), FILE_WRITE);
                    SelectSDCard(false);

                    // Voer binnengekomen event uit
                    RaiseMessage(ExecuteLine(Event, Protocol));
                    
                    // Sluit logging naar file weer uit zodat deze als bodytext retour gestuurd kan worden
                    RequestEvent=true;
                    SelectSDCard(true);
                    HTTPResultFile.close();
                    SelectSDCard(false);
                    SendBodyText=true;
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
              if(SendBodyText)
                {              
                // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer SDCard chip
                SelectSDCard(true);
                HTTPResultFile=SD.open(ProgmemString(Text_06));
                if(HTTPResultFile) 
                  {
                  y=0;       
                  while(HTTPResultFile.available())
                    {
                    x=HTTPResultFile.read();
                    if(isprint(x) && y<INPUT_BUFFER_SIZE)
                      {
                      TmpStr1[y++]=x;
                      }
                    else
                      {
                      TmpStr1[y]=0;
                      y=0;                    
                      SelectSDCard(false);
                      HTTPClient.print(TmpStr1);
                      HTTPClient.print("<br>");
                      //Serial.print(F("*** debug: BODY TEXT -> "));Serial.println(TmpStr1); //??? Debug
                      SelectSDCard(true);
                      }
                    }
                  HTTPResultFile.close();
                  SD.remove(ProgmemString(Text_06));
                  SelectSDCard(false);
                  }  
                else 
                  HTTPClient.println(cmd2str(MESSAGE_03));
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

//  if(RequestEvent)
//    {
//    ExecutionDepth=0;
//    RaiseMessage(ExecuteLine(Event, Protocol));
//    }

  free(Event);
  return;
  }  
#endif

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
  const byte PTMF=RawSignal.Pulses[0];

  static unsigned long PreviousTime=0;
  static unsigned long PreviousBitstream=0;

  // NODO signaal bestaat uit start bit + 32 bits. Als ongelijk aan 66, dan geen Nodo signaal
  if(RawSignal.Number!=66)return 0L;

  // 0=aantal, 1=startpuls, 2=space na startpuls, 3=1e pulslengte. Dus start loop met drie.
  z=0;
  for(x=3;x<=RawSignal.Number;x+=2)
    {
    if((RawSignal.Pulses[x]*PTMF)>NODO_PULSE_MID)      
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
  Event->Checksum=0;
  Event->Command=CMD_USEREVENT;
  Event->Par1=(bitstream>>8)&0xff;
  Event->Par2=bitstream&0xff;

  // Het oude Nodo signaal herhaald zich enkele malen. Deze herhalingen zijn nodig om een valide checksum op te leveren maar
  // kunnen onterecht ook nieuwe events opleveren. Om deze reden moet er voor dit type event wachttijd worden toegepast

  return true;  
  }


#ifdef NODO_MEGA
boolean SendEventDirect(struct NodoEventStruct *ES, byte Port)
  {
  // Verstuur signaal als I2C
  if(Port==VALUE_SOURCE_I2C)
    SendI2C(ES);

  // Verstuur signaal als HTTP-event.
  else if(Port==VALUE_SOURCE_HTTP && bitRead(HW_Config,HW_ETHERNET))// Als Ethernet shield aanwezig.
    SendHTTPEvent(ES);

  else
    {
    Nodo_2_RawSignal(ES);
  
    // Verstuur signaal als RF
    if(Port==VALUE_SOURCE_RF)
      RawSendRF();
  
    // Verstuur signaal als IR
    else if(Port==VALUE_SOURCE_IR)
      RawSendIR();
    }
  delay(MIN_TIME_BETWEEN_TX);
  }
#endif 
  
  
  
  
