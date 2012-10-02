boolean QueueReceive(int Pos, int ChecksumOrg)
  {
  int x,y,Checksum;
  unsigned long Mark, Space, Timeout, ul=0L;

  // Hier aangekomen is de master nodo nog steeds bezig met het zenden van het aanloopsignaal bestaande uit de korte pulsenreeks.
  // Wacht totdat het aanloopsignaal een duidelijke startbit bevat.
  Timeout=millis() + 2000L; // binnen twee seconden moet het blok met gegevens zijn aangekomen.
  x=0;
  while(Timeout>millis() && !(x==50 && Mark>NODO_PULSE_MID))
    {
    Space = WaitForChangeState(PIN_RF_RX_DATA, LOW,  SIGNAL_TIMEOUT_RF);
    Mark  = WaitForChangeState(PIN_RF_RX_DATA, HIGH, SIGNAL_TIMEOUT_RF);
    if(x<50)
      {
      if(Space>MIN_PULSE_LENGTH && Space<NODO_PULSE_MID && Mark>MIN_PULSE_LENGTH && Mark<NODO_PULSE_MID)
        x++;
      else
        x=0;
      }
    }

  // aanloopsignaal is gereed en de startbit is verzonden. Eerst bit van de pulsenreeks komt er direct aan.
  Queue.Position=0;
  x=0;

  while(Timeout>millis() && Queue.Position<Pos)
    {
    Mark = WaitForChangeState(PIN_RF_RX_DATA,HIGH, SIGNAL_TIMEOUT_RF);
    if(Mark>MIN_PULSE_LENGTH)
      {        
      if(Mark>NODO_PULSE_MID)
        ul=ul | (unsigned long)(1L<<x);

      if(++x>31)
        {
        x=0;
        Queue.Event[Queue.Position]=ul;
        Queue.Port[Queue.Position++]=VALUE_SOURCE_RF;
        ul=0L;
        }
      }
    }

  // bereken checksum: crc-8 uit alle bytes in de queue.
  Checksum=0;
  byte *B=(byte*)&(Queue.Event[0]);    //pointer verwijst nu naar eerste byte van de queue
  for(x=0;x<Queue.Position*4;x++) // maal vier, immers 4 bytes in een unsigned long
    Checksum^=*(B+x); 

  if(ChecksumOrg == Checksum)
    {
    // Korte wachttijd anders is de RF ontvanger van de master (mogelijk) nog niet gereed voor ontvangst. 
    delay(RECEIVER_STABLE);

    // Verzend [Busy On] zodat de master weet dat alles in goede orde ontvangen en verwerking start.
    ul=command2event(Settings.Unit,CMD_BUSY,VALUE_ON,0);
    Nodo_2_RawSignal(ul);
    RawSendRF();
    Busy.Sent=true;

    if(Settings.Debug==VALUE_ON)
      PrintEvent(ul,VALUE_DIRECTION_OUTPUT, VALUE_SOURCE_RF);

    // Verwerk de inhoud van de Queue, Eerst Korte wachttijd anders is de RF ontvanger van de master (mogelijk) nog niet gereed voor ontvangst.
    delay(RECEIVER_STABLE);

    ProcessQueue();
    return true;
    }
  else
    {
    delay(1000);
    Queue.Position=0;
    return false;
    }
  }

#if NODO_MEGA  
boolean QueueSend(byte DestUnit)
  {
  int x,y,Checksum=0;
  boolean Bit;
  unsigned long Event,TimeoutTimer;
  
  Led(BLUE);

  // Eerst wachten op bezige Nodo. Als gebruiker deze setting niet heeft geactiveerd, dan tijdelijk hiervoor 30 sec. nemen.
  if(Settings.WaitBusyAll<30)
    x=30;
  else
    x=Settings.WaitBusyAll;    
  NodoBusy(0L, x);

  delay(RECEIVER_STABLE);
  if(Settings.WaitFreeRF==VALUE_ON)
    WaitFreeRF();  

  // bereken checksum: crc-8 uit alle bytes in de queue.
  byte *B=(byte*)&(Queue.Event[0]);    //pointer verwijst nu naar eerste byte van de queue
  for(x=0;x<Queue.Position*4;x++) // maal vier, immers 4 bytes in een unsigned long
    Checksum^=*(B+x); 

  Nodo_2_RawSignal(command2event(DestUnit,CMD_TRANSMIT_QUEUE,Queue.Position,Checksum));
  RawSendRF();
    
  // Verzend een aanloopsignaal bestaande uit reeks korte pulsen om slave tijd te geven klaar te staan voor ontvangst en voorkomen dat andere Nodo vrije ruimte in RF benut
  digitalWrite(PIN_RF_RX_VCC,LOW);   // Spanning naar de RF ontvanger uit om interferentie met de zender te voorkomen.
  digitalWrite(PIN_RF_TX_VCC,HIGH); // zet de 433Mhz zender aan

  for(x=0; x<100; x++)// Houd de 433 band bezet en geef slave gelegenheid om klaar voor ontvangst data uit queue.
    {
    digitalWrite(PIN_RF_TX_DATA,HIGH); // 1
    delayMicroseconds(NODO_PULSE_0); 
    digitalWrite(PIN_RF_TX_DATA,LOW); // 0
    delayMicroseconds(NODO_SPACE); 
    }

  // verzend startbit
  digitalWrite(PIN_RF_TX_DATA,HIGH); // 1
  delayMicroseconds(NODO_PULSE_1*3); 
  digitalWrite(PIN_RF_TX_DATA,LOW); // 0
  delayMicroseconds(NODO_SPACE); 

  // Verzend de content van de queue
  for(x=0; x<Queue.Position; x++)
    {
    for(y=0; y<32; y++)
      {
      if(((Queue.Event[x])>>y)&1)
        {// 1
        digitalWrite(PIN_RF_TX_DATA,HIGH);
        delayMicroseconds(NODO_PULSE_1); 
        digitalWrite(PIN_RF_TX_DATA,LOW);
        delayMicroseconds(NODO_SPACE); 
        }
      else
        {// 0
        digitalWrite(PIN_RF_TX_DATA,HIGH);
        delayMicroseconds(NODO_PULSE_0); 
        digitalWrite(PIN_RF_TX_DATA,LOW);
        delayMicroseconds(NODO_SPACE); 
        }
      }
    }
  delayMicroseconds(NODO_SPACE*10); 
  digitalWrite(PIN_RF_TX_VCC,LOW); // zet de 433Mhz zender weer uit
  digitalWrite(PIN_RF_RX_VCC,HIGH); // Spanning naar de RF ontvanger weer aan.    

  // Queue is verzonden. Wacht op bevestiging van de slave.
  // Op dit moment kunnen er drie situaties voordoen:
  // 1. master ontvangt Busy On van slave Nodo, hieruit kan worden afgeleid dat de Slave start met verwerking van de queue 
  // 2. master ontvangt niets. Dan keert WaitAndQueue terug met een false.
  // 3. master ontvangt error message van de slave.

  if(WaitAndQueue(4,false,command2event(DestUnit,CMD_BUSY,VALUE_ON,0)))
    {
    WaitAndQueue(30,true,0L);
    return true;
    }
  else
    {
    return false;
    }
  }
#endif

unsigned long GetEvent_IRRF(unsigned long *Content, int *Port)
  {
  unsigned long Checksum=0L;                           // Als gelijk aan Event dan tweemaal dezelfde code ontvangen: checksum funktie.
  unsigned long StaySharpTimer=millis();                      // timer die start bij ontvangen van een signaal. Dwingt om enige tijd te luisteren naar dezelfde poort.
  static unsigned long Previous, PreviousTimer=0L;
  
  // IR: *************** kijk of er data staat op IR en genereer een event als er een code ontvangen is **********************
  do
    {
    if((*portInputRegister(IRport)&IRbit)==0)// Kijk if er iets op de poort binnenkomt. (Pin=LAAG als signaal in de ether). 
      {
      if(FetchSignal(PIN_IR_RX_DATA,LOW,Settings.AnalyseTimeOut))// Als het een duidelijk IR signaal was
        {
        *Content=AnalyzeRawSignal(); // Bereken uit de tabel met de pulstijden de 32-bit code. 
        if(*Content)// als AnalyzeRawSignal een event heeft opgeleverd
          {
          StaySharpTimer=millis()+SHARP_TIME;
          if(PreviousTimer<millis())
            Previous=0L;
          if(Previous!=*Content)
            {
            RawSignal.Source=VALUE_SOURCE_IR;
            *Port=VALUE_SOURCE_IR;
            PreviousTimer=millis()+SIGNAL_REPEAT_TIME;
            Previous=*Content;
            return true;
            }
          Checksum=*Content;
          }
        }
      }
    }while(StaySharpTimer>millis());

  // RF: *************** kijk of er data start op RF en genereer een event als er een code ontvangen is **********************
  do
    {
    if((*portInputRegister(RFport)&RFbit)==RFbit)// Kijk if er iets op de RF poort binnenkomt. (Pin=HOOG als signaal in de ether). 
      {
      if(FetchSignal(PIN_RF_RX_DATA,HIGH,SIGNAL_TIMEOUT_RF))// Als het een duidelijk RF signaal was
        {
        *Content=AnalyzeRawSignal(); // Bereken uit de tabel met de pulstijden de 32-bit code. 
        if(*Content)// als AnalyzeRawSignal een event heeft opgeleverd
          {
          StaySharpTimer=millis()+SHARP_TIME;          
          if(*Content==Checksum) // tweemaal hetzelfde event ontvangen
            {
            if(PreviousTimer<millis())
              Previous=0L;
            if(Previous!=Checksum)
              {
              RawSignal.Source=VALUE_SOURCE_RF;
              *Port=VALUE_SOURCE_RF;
              PreviousTimer=millis()+SIGNAL_REPEAT_TIME;
              Previous=Checksum;
              return true;
              }
            }
          Checksum=*Content;
          }
        }
      }
    }while(StaySharpTimer>millis());
  return false;
  }

unsigned long AnalyzeRawSignal(void)
  {
  unsigned long Code=0L;
  
  if(RawSignal.Number==RAW_BUFFER_SIZE)return 0L;     // Als het signaal een volle buffer beslaat is het zeer waarschijnlijk ruis of ongeldig signaal

  #if USER_PLUGIN
  Code=RawSignal_2_UserPlugin(RawSignal.Source);
  if(Code==1)
    return 0;
  else if(Code>1)
    return Code;
  #endif
  
  if(!(Code=RawSignal_2_Nodo()))           // Is het een Nodo signaal?
    if(!(Code=RawSignal_2_KAKU()))         // Of een KAKU?
      if(!(Code=RawSignal_2_NewKAKU()))    // Of een NewKAKU?
        Code=RawSignal_2_32bit();          // Geen Nodo, KAKU of NewKAKU code. Genereer dan uit het onbekende signaal een (vrijwel) unieke 32-bit waarde.
      
  return Code;   
  }

/*********************************************************************************************\
* Deze routine berekent de uit een RawSignal een NODO code
* Geeft een false retour als geen geldig NODO signaal
\*********************************************************************************************/
unsigned long RawSignal_2_Nodo(void)
  {
  unsigned long bitstream=0L;
  int x,y,z;

  // NODO signaal bestaat uit start bit + 32 bits. Als ongelijk aan 66, dan geen Nodo signaal
  if(RawSignal.Number!=66)return 0L;

  // 0=aantal, 1=startpuls, 2=space na startpuls, 3=1e pulslengte. Dus start loop met drie.
  z=0;
  for(x=3;x<=RawSignal.Number;x+=2)
    {
    if(RawSignal.Pulses[x]>NODO_PULSE_MID)      
      bitstream|=(long)(1L<<z); //LSB in signaal wordt  als eerste verzonden
    z++;
    }

  if(((bitstream>>28)&0xf) == SIGNAL_TYPE_NODO)// is het type-nibble uit het signaal gevuld met de aanduiding NODO.
    {
    RawSignal.Type=SIGNAL_TYPE_NODO;
    return bitstream;
    }

  return 0L;  
  }


 /**********************************************************************************************\
 * Deze functie genereert uit een willekeurig gevulde RawSignal afkomstig van de meeste 
 * afstandsbedieningen een (vrijwel) unieke bit code.
 * Zowel breedte van de pulsen als de afstand tussen de pulsen worden in de berekening
 * meegenomen zodat deze functie geschikt is voor PWM, PDM en Bi-Pase modulatie.
 * LET OP: Het betreft een unieke hash-waarde zonder betekenis van waarde.
 \*********************************************************************************************/
unsigned long RawSignal_2_32bit(void)
  {
  int x,y,z;
  int Counter_pulse=0,Counter_space=0;
  int MinPulse=0xffff;
  int MinSpace=0xffff;
  unsigned long CodeP=0L;
  unsigned long CodeS=0L;
  unsigned long Event;
  
  // zoek de kortste tijd (PULSE en SPACE)
  x=5; // 0=aantal, 1=startpuls, 2=space na startpuls, 3=1e puls
  while(x<=RawSignal.Number-4)
    {
    if(RawSignal.Pulses[x]<MinPulse)MinPulse=RawSignal.Pulses[x]; // Zoek naar de kortste pulstijd.
    x++;
    if(RawSignal.Pulses[x]<MinSpace)MinSpace=RawSignal.Pulses[x]; // Zoek naar de kortste spacetijd.
    x++;
    }
  MinPulse+=(MinPulse*Settings.AnalyseSharpness)/100;
  MinSpace+=(MinSpace*Settings.AnalyseSharpness)/100;
     
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
    }while(x<RawSignal.Number);

 RawSignal.Type=SIGNAL_TYPE_UNKNOWN;
 
 if(Counter_pulse>=1 && Counter_space<=1)
   Event=CodeP; // data zat in de pulsbreedte
 else if(Counter_pulse<=1 && Counter_space>=1)
   Event=CodeS; // data zat in de pulse afstand
 else
   Event=CodeS^CodeP; // data zat in beide = bi-phase, maak er een leuke mix van.
   
 return (Event&0x0fffffff)|((unsigned long)SIGNAL_TYPE_UNKNOWN<<28);
 }


 /**********************************************************************************************\
 * Deze functie wacht totdat de 433 band vrij is of er een timeout heeft plaats gevonden 
 * Window en delay tijd in milliseconden
 \*********************************************************************************************/
# define WAITFREERF_TIMEOUT             30000 // tijd in ms. waarna het wachten wordt afgebroken als er geen ruimte in de vrije ether komt

void WaitFreeRF(void)
  {
  unsigned long Timer, TimeOutTimer;  
      
  // eerst de 'dode' wachttijd
  Led(BLUE);
  delay((Settings.Unit-1)*100);
    
  // dan kijken of de ether vrij is.
  Timer=millis()+300; // reset de timer.
  TimeOutTimer=millis()+WAITFREERF_TIMEOUT; // tijd waarna de routine wordt afgebroken in milliseconden

  while(Timer>millis() && TimeOutTimer>millis())
    {
    if((*portInputRegister(RFport)&RFbit)==RFbit)// Kijk if er iets op de RF poort binnenkomt. (Pin=HOOG als signaal in de ether). 
      {
      if(FetchSignal(PIN_RF_RX_DATA,HIGH,SIGNAL_TIMEOUT_RF))// Als het een duidelijk signaal was
        Timer=millis()+300; // reset de timer weer.
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

  delay(5);// kleine pause om de zender de tijd te geven om stabiel te worden 
  noInterrupts();
  
  for(byte y=0; y<RF_REPEATS; y++) // herhaal verzenden RF code
    {
    x=1;
    while(x<=RawSignal.Number)
      {
      digitalWrite(PIN_RF_TX_DATA,HIGH); // 1
      delayMicroseconds(RawSignal.Pulses[x++]); 
      digitalWrite(PIN_RF_TX_DATA,LOW); // 0
      delayMicroseconds(RawSignal.Pulses[x++]); 
      }
    }
  
  interrupts();
  
  digitalWrite(PIN_RF_TX_VCC,LOW); // zet de 433Mhz zender weer uit
  digitalWrite(PIN_RF_RX_VCC,HIGH); // Spanning naar de RF ontvanger weer aan.
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

  for(int repeat=0; repeat<IR_REPEATS; repeat++) // herhaal verzenden IR code
    {
    pulse=1;
    noInterrupts(); // interrupts tijdelijk uitschakelen om zo en zuiverder signaal te krijgen
    while(pulse<=RawSignal.Number)
      {
      // Mark verzenden. Bereken hoeveel pulsen van 26uSec er nodig zijn die samen de lengte van de mark zijn.
      mod=RawSignal.Pulses[pulse++]/26; // delen om aantal pulsen uit te rekenen

      do
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
        }while(--mod);
  
      // Laag
      delayMicroseconds(RawSignal.Pulses[pulse++]);
      }
    interrupts(); // interupts weer inschakelen.
    }
  }

 /*********************************************************************************************\
 * Deze routine berekend de RAW pulsen van een 32-bit Nodo-code en plaatst deze in de buffer RawSignal
 * RawSignal.Bits het aantal pulsen*2+startbit*2 ==> 66
 * 
 \*********************************************************************************************/
void Nodo_2_RawSignal(unsigned long Code)
  {
  byte BitCounter,y=1;

  // begin met een lange startbit. Veilige timing gekozen zodat deze niet gemist kan worden
  RawSignal.Pulses[y++]=NODO_PULSE_1*2; 
  RawSignal.Pulses[y++]=NODO_SPACE*2;

  // de rest van de bits LSB als eerste de lucht in
  for(BitCounter=0; BitCounter<=31; BitCounter++)
    {
    if(Code>>BitCounter&1)
      RawSignal.Pulses[y++]=NODO_PULSE_1; 
    else
      RawSignal.Pulses[y++]=NODO_PULSE_0;   
    RawSignal.Pulses[y++]=NODO_SPACE;   
    }
  RawSignal.Pulses[y-1]=NODO_PULSE_1*10; // pauze tussen de pulsreeksen
  RawSignal.Number=66; //  1 startbit bestaande uit een pulse/space + 32-bits is 64 pulse/space = totaal 66
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
  
  do{// lees de pulsen in microseconden en plaats deze in een tijdelijke buffer
    PulseLength=WaitForChangeState(DataPin, StateSignal, TimeOut);

    if(PulseLength<MIN_PULSE_LENGTH)
      return false;

    RawSignalPtr->Pulses[RawCodeLength++]=PulseLength;
    PulseLength=WaitForChangeState(DataPin, !StateSignal, TimeOut);
    RawSignalPtr->Pulses[RawCodeLength++]=PulseLength;
    }while(RawCodeLength<RAW_BUFFER_SIZE && PulseLength!=0);// Zolang nog niet alle bits ontvangen en er niet vroegtijdig een timeout plaats vindt

  if(RawCodeLength>=MIN_RAW_PULSES)
    {
    RawSignalPtr->Number=RawCodeLength-1;
    if(DataPin==PIN_IR_RX_DATA)
      bitWrite(HW_Config,HW_IR_RX,1);
    if(DataPin==PIN_RF_RX_DATA)
      bitWrite(HW_Config,HW_RF_RX,1);
    return true;
    }
  RawSignalPtr->Number=0;
  return false;
  }
  
  
/**********************************************************************************************\
* verzendt een event en geeft dit tevens weer op SERIAL
* als het Event gelijk is aan 0L dan wordt alleen de huidige inhoud van de buffer als RAW
* verzonden.
\**********************************************************************************************/
boolean TransmitCode(unsigned long Event, byte Dest)
  {  
  int x;
  byte SignalType=(Event>>28) & 0x0f;
  byte Command   =(Event>>16) & 0xff;
  
  if(Dest==VALUE_SOURCE_RF || (Settings.TransmitRF==VALUE_ON && Dest==VALUE_ALL))
    if(Settings.WaitFreeRF==VALUE_ON && SignalType!=SIGNAL_TYPE_UNKNOWN)// alleen WaitFreeRF als type bekend is, anders gaat SendSignal niet goed a.g.v. overschrijven buffer
      WaitFreeRF();  

  if(Command==CMD_KAKU)
    KAKU_2_RawSignal(Event);

  else if(Command==CMD_KAKU_NEW)
    NewKAKU_2_RawSignal(Event);

  else if(SignalType==SIGNAL_TYPE_NODO)
    Nodo_2_RawSignal(Event);

  if(Dest==VALUE_SOURCE_IR || (Settings.TransmitIR==VALUE_ON && Dest==VALUE_ALL))
    { 
    PrintEvent(Event,VALUE_DIRECTION_OUTPUT, VALUE_SOURCE_IR);
    RawSendIR();
    }
  else
    delay(250);

  if(Dest==VALUE_SOURCE_RF || (Settings.TransmitRF==VALUE_ON && Dest==VALUE_ALL))
    {
    PrintEvent(Event,VALUE_DIRECTION_OUTPUT, VALUE_SOURCE_RF);
    RawSendRF();
    }
  else
    delay(250);

#if NODO_MEGA
  if(bitRead(HW_Config,HW_ETHERNET))// Als Ethernet shield aanwezig.
    {
    if(Dest==VALUE_SOURCE_HTTP || (Dest==VALUE_ALL && Settings.TransmitIP==VALUE_ON))
      {
      SendHTTPEvent(Event);
      PrintEvent(Event,VALUE_DIRECTION_OUTPUT,VALUE_SOURCE_HTTP);
      }
    }
 #endif 
  }


#if NODO_MEGA
 /*********************************************************************************************\
 * Kijk of voor de opgegeven Hex-event (Code) een rawsignal file op de SDCard bestaat.
 * Als deze bestaat, dan het Hex-event vervangen door het commando "RawSignal <key>"
 * Call by referece !
 \*********************************************************************************************/
void CheckRawSignalKey(unsigned long *Code)
  {
  int x,y;
  char *TempString=(char*)malloc(INPUT_BUFFER_SIZE+1);
  boolean Finished=false;
  
  if(RawSignal.Type==SIGNAL_TYPE_UNKNOWN)
    {
    // kijk of de hex-code toevallig al eerder is opgeslagen als rawsignal op de SDCard
    // maak er dan een Nodo event van. 
  
    // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer SDCard chip
    SelectSD(true);
    sprintf(TempString,"%s/%s.key",ProgmemString(Text_28),int2str(*Code)+2); // +2 omdat dan de tekens '0x' niet worden meegenomen. anders groter dan acht posities in filenaam.
    
    File dataFile=SD.open(TempString);
    if(dataFile) 
      {
      y=0;       
      while(!Finished && dataFile.available())
        {
        x=dataFile.read();
        if(isDigit(x) && y<INPUT_BUFFER_SIZE)
          {
          TempString[y++]=x;
          }
        else if(x=='\n' || isPunct(x))
          {
          TempString[y]=0;
          y=0;
          *Code=command2event(Settings.Unit,CMD_RAWSIGNAL,str2int(TempString),0);
          Finished=true;
          }
        }
      dataFile.close();
      }  
  
    // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer W5100 chip
    SelectSD(false);
    }
  free(TempString);
  }

 /*********************************************************************************************\
 * Sla de pulsen in de buffer Rawsignal op op de SDCard
 \*********************************************************************************************/
boolean SaveRawSignal(byte Key)
  {
  boolean error=false;
  unsigned long Event;
  char *TempString=(char*)malloc(40);

  Event=AnalyzeRawSignal();

  // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer SDCard chip
  SelectSD(true);

  // Sla Raw-pulsenreeks op in bestand met door gebruiker gekozen nummer als filenaam
  sprintf(TempString,"%s/%s.raw",ProgmemString(Text_27),int2str(Key));
  SD.remove(TempString); // eventueel bestaande file wissen, anders wordt de data toegevoegd.    
  File KeyFile = SD.open(TempString, FILE_WRITE);
  if(KeyFile) 
    {
    for(int x=1;x<=RawSignal.Number;x++)
      {
      TempString[0]=0;
      if(x>1)
        strcat(TempString,",");
      strcat(TempString,int2str(RawSignal.Pulses[x]));
      KeyFile.write(TempString);
      }
    strcpy(TempString,"\n");
    KeyFile.write(TempString);
    KeyFile.close();


    // bestand met HEX-event als naam en verwijzing naar door gebruiker gekozen key als inhoud
    sprintf(TempString,"%s/%s.key",ProgmemString(Text_28),int2str(Event)+2); // +2 omdat dan de tekens '0x' niet worden meegenomen. anders groter dan acht posities in filenaam.
    SD.remove(TempString); // eventueel bestaande file wissen, anders wordt de data toegevoegd.      
    KeyFile = SD.open(TempString, FILE_WRITE);
    if(KeyFile) 
      {
      strcpy(TempString,int2str(Key));
      strcat(TempString,";\n");
      KeyFile.write(TempString);
      KeyFile.close();
      }
    else 
      error=true;
    }
  else 
    error=true;
 
  // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer W5100 chip
  SelectSD(false);
    
  free(TempString);

  if(error)
    {
    TransmitCode(command2event(Settings.Unit,CMD_MESSAGE, Settings.Unit, MESSAGE_03),VALUE_ALL);
    return false;
    }
  return true;
  }


 /*********************************************************************************************\
 * Haal de RawSignal pulsen op uit het bestand <key>.raw en sla de reeks op in de 
 * RawSignal buffer, zodat deze vervolgens weer kan worden gebruikt om te verzenden.
 \*********************************************************************************************/
boolean RawSignalGet(int Key)
  {
  int x,y,z;
  boolean Ok;
  char *TempString=(char*)malloc(INPUT_BUFFER_SIZE+1);
  
  // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer SDCard chip
  SelectSD(true);
  sprintf(TempString,"%s/%s.raw",ProgmemString(Text_27),int2str(Key));

  File dataFile=SD.open(TempString);
  if(dataFile) 
    {
    y=0;
    z=1;// [0] van RawSignal.Pulses wordt niet gebruikt
    while(dataFile.available())
      {
      x=dataFile.read();
      if(isDigit(x) && y<INPUT_BUFFER_SIZE)
        {
        TempString[y++]=x;
        }
      else if(x=='\n' || isPunct(x))
        {
        TempString[y]=0;
        y=0;
        RawSignal.Pulses[z++]=str2int(TempString);
        }
      }
    dataFile.close();
    Ok=true;
    RawSignal.Number=z-1;
    }
  else
    Ok=false;
    
  // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer W5100 chip
  SelectSD(false);

  free(TempString);
  return Ok;
  }

#endif
  
