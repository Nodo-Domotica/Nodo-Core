
boolean QueueReceive(byte Pos, byte ChecksumOrg, byte Port)
  {
  int x,y,Checksum;
  unsigned long Mark=0L, Space, Timeout, ul=0L;

  // Zet WaitFreeRF aan en bawaar originele instelling van de gebruiker.
  byte OrgWaitFreeRF=Settings.WaitFreeRF;
  Settings.WaitFreeRF=VALUE_ON;
  
  // De Nodo vangt alle events op en plaatst deze in de queue. Alle andere Nodo's
  // staan in de Wait-stand zonder queueing van events.
  // Klaar met capturing als er een afsluitende TRANSMIT_QUEUE voorbij komt.  

  Serial.println("*** debug: QueueReceive(); wacht op queue.");//???
  if(Wait(true,20,0xffffffff,command2event(Settings.Unit,CMD_TRANSMIT_QUEUE,0,0)))
    {
    Serial.print("*** debug: Wachten gereed. Ontvangen=");Serial.println(Queue.Position); //??? Debug
    if(Queue.Position==Pos)
      {
      TransmitCode(command2event(Settings.Unit,CMD_MESSAGE, Settings.Unit, MESSAGE_00),Port);
      Serial.println("*** debug: OK verzonden. Wacht op vrijgave {Select 0]");//???
      }
    }

  // Zet de originele WaitFreeRF instelling van de gebruiker weer terug
  Settings.WaitFreeRF=OrgWaitFreeRF;

  return false;
  }

#ifdef NODO_MEGA  
/*********************************************************************************************\
 * Deze routine verzend de inhoud van de queue naar een andere Nodo.
 \*********************************************************************************************/
boolean QueueSend(byte DestUnit, byte Port)
  {
  int x,Checksum=0;
//  unsigned long Event;
//
//  // Zet WaitFreeRF aan en bawaar originele instelling van de gebruiker.
//  byte OrgWaitFreeRF=Settings.WaitFreeRF;
//  Settings.WaitFreeRF=VALUE_ON;
//  
//  if(Port==0)
//    Port=VALUE_SOURCE_RF;
//
//  // 1: Zet alle Nodo's on hold.
//  TransmitCode(command2event(DestUnit,CMD_SELECT,DestUnit,0),Port);
//  Serial.println("*** debug: Nodo's on hold.");//???
//
//  // 2: Verzend naar de slave Nodo de opdracht om naar de QueueReceive funktie te gaan
//  Settings.WaitFreeRF=VALUE_OFF;
//  TransmitCode(command2event(DestUnit,CMD_TRANSMIT_QUEUE,Queue.Position,Checksum),Port);
//  Serial.println("*** debug: TransmitQueue verzonden.");//???
//
//  // 3: Verzend alle events uit de queue. Alleen de bestemmings Nodo zal deze events in de queue plaatsen
//  for(x=0;x<Queue.Position;x++)
//    {
//    TransmitCode(Queue.Event[x],Port);
//    }
//  Queue.Position=0;
//
//  // 4: Verzend en afsluitende TransmitQueue zodat de ontvangende Nodo weet dat verzending gereed is.
//  TransmitCode(command2event(DestUnit,CMD_TRANSMIT_QUEUE,0,0),Port);
//  Serial.println("*** debug: Queue verzonden. Wacht op message OK. van Slave Nodo.");//???
//
//  // 5: Wacht op een bevestiging van de slave Nodo
//  x=false;
//  if(Wait(true,5,0xffffffff,command2event(DestUnit,CMD_MESSAGE, DestUnit, MESSAGE_00)))
//    x=true;
//
//  if(x)
//    Serial.println("*** debug: OK ontvangen van slave.");//???
//  else
//    Serial.println("*** debug: OK =NIET= ontvangen van slave.");//???
//  
//  // 6: Haal alle andere Nodo's uit de hold mode    
//  Settings.WaitFreeRF=VALUE_ON;
//  TransmitCode(command2event(Settings.Unit, CMD_SELECT,0,0),Port);
//  Serial.println("*** debug: Select 0 verzonden.");//???
//
//  // Zet de originele WaitFreeRF instelling van de gebruiker weer terug
//  Settings.WaitFreeRF=OrgWaitFreeRF;
//
  return x;
  }
#endif

boolean ScanEvent(struct NodoEventStruct *Event, int *Port)
  {
  static unsigned long Previous,PreviousTimer;
  unsigned long Content, Checksum=0L;                           // Als gelijk aan Event dan tweemaal dezelfde code ontvangen: checksum funktie.
  unsigned long StaySharpTimer=millis();
  byte Fetched=0;

  if(I2C_Event)
    {
    Content=I2C_Event;
    *Port=VALUE_SOURCE_I2C;
    I2C_Event=0L;
    return true;
    }
  
  do
    {
    // IR: *************** kijk of er data staat op IR en genereer een event als er een code ontvangen is **********************
    if((*portInputRegister(IRport)&IRbit)==0)// Kijk of er iets op de poort binnenkomt. (Pin=LAAG als signaal in de ether). 
      if(FetchSignal(PIN_IR_RX_DATA,LOW,SIGNAL_TIMEOUT_RF))// Als het een duidelijk IR signaal was
        Fetched=VALUE_SOURCE_IR;


    // RF: *************** kijk of er data start op RF en genereer een event als er een code ontvangen is **********************
    if((*portInputRegister(RFport)&RFbit)==RFbit)// Kijk if er iets op de RF poort binnenkomt. (Pin=HOOG als signaal in de ether). 
      if(FetchSignal(PIN_RF_RX_DATA,HIGH,SIGNAL_TIMEOUT_RF))// Als het een duidelijk RF signaal was
        Fetched=VALUE_SOURCE_RF;
  
    if(Fetched)
      {
      if(AnalyzeRawSignal(Event))// als AnalyzeRawSignal een event heeft opgeleverd dan is het struct Event gevuld.
        {
        Content=EventStruct2Event(Event);    
        
        // ??? Dit is een tijdelijke oplossing. Nodo signaal wordt eenmaal verzonden terwijl andere signalen worden herhaald waarbij
        // de herhaling wordt gebruikt als checksum. Mgelijk komt hier een alternatief voor. Tot die tijd deze oplossing.
        if(Event->Type==SIGNAL_TYPE_NODO)
          Checksum=Content;
                
        StaySharpTimer=millis()+SHARP_TIME;          
        if(Content==Checksum) // tweemaal hetzelfde event ontvangen
          {
          if((PreviousTimer+SIGNAL_REPEAT_TIME)<millis())
            Previous=0L;
            
          if(Previous!=Checksum)
            {
            RawSignal.Source=Fetched;
            *Port=Fetched;
            PreviousTimer=millis();
            Previous=Checksum;
            #ifdef NODO_MEGA
            Received=Previous; // Received wordt gebruikt om later de status van laatste ontvangen event te kunnen gebruiken t.b.v. substitutie van vars.
            #endif
            return true;
            }
          }
        Checksum=Content;
        }
      Fetched=0;
      }
    }while(StaySharpTimer>millis());

  return false;
  }

boolean AnalyzeRawSignal(struct NodoEventStruct *E)
  {
  unsigned long Code=0L;
  
  if(RawSignal.Number==RAW_BUFFER_SIZE)return false;     // Als het signaal een volle buffer beslaat is het zeer waarschijnlijk ruis of ongeldig signaal

//  #ifdef USER_PLUGIN
//  Code=RawSignal_2_UserPlugin(RawSignal.Source);
//  if(Code==1)
//    return 0;
//  else if(Code>1)
//    return Code;
//  #endif // weer herstellen.
  


if(RawSignal_2_Nodo(E))           // Is het een Nodo signaal?@@
    return true;
    
//  if(!(RawSignal_2_Nodo(E)))           // Is het een Nodo signaal?
//    if(!(Code=RawSignal_2_KAKU()))         // Of een KAKU?
//      if(!(Code=RawSignal_2_NewKAKU()))    // Of een NewKAKU?
//        Code=RawSignal_2_32bit();          // Geen Nodo, KAKU of NewKAKU code. Genereer dan uit het onbekende signaal een (vrijwel) unieke 32-bit waarde.
// ??? herstellen

  return false;   
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
      
  Led(BLUE);

  // eerst de 'dode' wachttijd die afhangt van het unitnummer. Dit voorkomt dat alle units exact op hetzelfde moment gaan zenden als de ether vrij is.
  delay((Settings.Unit-1)*100);

  // Als er recent een code is ontvangen door de Nodo, dan is één van de Nodo's wellicht nog niet volledig omgeschakeld van zenden 
  // naar ontvangen. Dit omdat sommige ontvangers lange opstarttijd nodig hebben voordat deze RF signalen nunnen ontvangen.
  // Daarom wachten totdat RECEIVER_STABLE tijd voorbij is na laatste verzending.
//???  while(millis() < (RawSignal.Timer+RECEIVER_STABLE));        
    
  // dan kijken of de ether vrij is.
  Timer=millis()+350; // reset de timer.
  TimeOutTimer=millis()+WAITFREERF_TIMEOUT; // tijd waarna de routine wordt afgebroken in milliseconden

  while(Timer>millis() && TimeOutTimer>millis())
    {
    if((*portInputRegister(RFport)&RFbit)==RFbit)// Kijk if er iets op de RF poort binnenkomt. (Pin=HOOG als signaal in de ether). 
      {
      if(FetchSignal(PIN_RF_RX_DATA,HIGH,SIGNAL_TIMEOUT_RF))// Als het een duidelijk signaal was
        Timer=millis()+350; // reset de timer weer.
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
  
  for(byte y=0; y<RawSignal.Repeats; y++) // herhaal verzenden RF code
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

  // kleine pause zodat verzenden event naar de USB poort gereed is, immers de IRQ's worden tijdelijk uitgezet
  delay(10);

  for(int repeat=0; repeat<RawSignal.Repeats; repeat++) // herhaal verzenden IR code
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
        }while(--mod);
  
      // Laag
      delayMicroseconds(RawSignal.Pulses[pulse++]);
      }
    interrupts(); // interupts weer inschakelen.
    }
  }

 /*********************************************************************************************\
 * Deze routine berekend de RAW pulsen van een 2x32-bit Nodo-code en plaatst deze in de buffer RawSignal
 * RawSignal.Bits het aantal pulsen*2+startbit*2 ==> 130
 \*********************************************************************************************/
void Nodo_2_RawSignal(struct NodoEventStruct Datablock)
  {
  byte BitCounter=1;
  RawSignal.Repeats=1;
  
  // bereken checksum: crc-8 uit alle bytes in de queue.
  byte c=0,*B=(byte*)&Datablock;
  for(byte x=0;x<sizeof(struct NodoEventStruct);x++)
      c^=*(B+x); 
  Datablock.Checksum=c;
  
  // begin met een lange startbit. Veilige timing gekozen zodat deze niet gemist kan worden
  RawSignal.Pulses[BitCounter++]=NODO_PULSE_1*4; 
  RawSignal.Pulses[BitCounter++]=NODO_SPACE*2;

  for(byte x=0;x<sizeof(struct NodoEventStruct);x++)
    {
    for(byte Bit=0; Bit<=7; Bit++)
      {
      if((*(B+x)>>Bit)&1)
        RawSignal.Pulses[BitCounter++]=NODO_PULSE_1; 
      else
        RawSignal.Pulses[BitCounter++]=NODO_PULSE_0;   
      RawSignal.Pulses[BitCounter++]=NODO_SPACE;   
      }
    }
    
  RawSignal.Pulses[BitCounter-1]=NODO_PULSE_1*10; // pauze tussen de pulsreeksen
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
  
  do{// lees de pulsen in microseconden en plaats deze in een tijdelijke buffer
    PulseLength=WaitForChangeState(DataPin, StateSignal, TimeOut);

    // bij kleine stoorpulsen die geen betekenig hebben zo snel mogelijk weer terug
    if(PulseLength<MIN_PULSE_LENGTH)
      return false;

    RawSignal.Pulses[RawCodeLength++]=PulseLength;
    PulseLength=WaitForChangeState(DataPin, !StateSignal, TimeOut);
    RawSignal.Pulses[RawCodeLength++]=PulseLength;
    }while(RawCodeLength<RAW_BUFFER_SIZE && PulseLength!=0);// Zolang nog niet alle bits ontvangen en er niet vroegtijdig een timeout plaats vindt

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
* verzendt een event en geeft dit tevens weer op SERIAL
* als het Event gelijk is aan 0L dan wordt alleen de huidige inhoud van de buffer als RAW
* verzonden.
\**********************************************************************************************/
boolean TransmitCode(unsigned long Event, byte Dest)
  {  
  struct NodoEventStruct ES=Event2EventStruct(Event,0,0);
  
  static unsigned long LastTransmitTime_RF=0;
  static unsigned long LastTransmitTime_IR=0;
  static unsigned long LastTransmitTime_I2C=0;

  if(Settings.TransmitI2C==VALUE_ON && (Dest==VALUE_SOURCE_I2C || Dest==VALUE_ALL))
    { 
    PrintEvent(Event,VALUE_DIRECTION_OUTPUT, VALUE_SOURCE_I2C);

    // Korte pauze om minimale tijd tussen verzenden te houden
    while((LastTransmitTime_I2C+MIN_TIME_BETWEEN_SEND_I2C) > millis());
    LastTransmitTime_I2C=millis();

    SendI2C(Event,0,0);
    }

  if(Dest==VALUE_SOURCE_RF || (Settings.TransmitRF==VALUE_ON && Dest==VALUE_ALL))
    if(Settings.WaitFreeRF==VALUE_ON && ES.Type!=SIGNAL_TYPE_UNKNOWN)// alleen WaitFreeRF als type bekend is, anders gaat SendSignal niet goed a.g.v. overschrijven buffer
      WaitFreeRF();  

  if(ES.Command==CMD_KAKU)
    KAKU_2_RawSignal(Event);

  else if(ES.Command==CMD_KAKU_NEW)
    NewKAKU_2_RawSignal(Event);

  else if(ES.Type==SIGNAL_TYPE_NODO)
    {
    Nodo_2_RawSignal(ES);
    }

  if(Settings.TransmitRF==VALUE_ON && (Dest==VALUE_SOURCE_RF || Dest==VALUE_ALL))
    {
    PrintEvent(Event,VALUE_DIRECTION_OUTPUT, VALUE_SOURCE_RF);

    // Korte pauze om minimale tijd tussen verzenden te houden
    while((LastTransmitTime_RF+MIN_TIME_BETWEEN_SEND_RF) > millis());
    LastTransmitTime_RF=millis();

    RawSendRF();
    }

  if(Settings.TransmitIR==VALUE_ON && (Dest==VALUE_SOURCE_IR || Dest==VALUE_ALL))
    { 
    PrintEvent(Event,VALUE_DIRECTION_OUTPUT, VALUE_SOURCE_IR);

    // Korte pauze om minimale tijd tussen verzenden te houden
    while((LastTransmitTime_IR+MIN_TIME_BETWEEN_SEND_IR) > millis());
    LastTransmitTime_IR=millis();

    RawSendIR();
    }

  #ifdef NODO_MEGA
  if(bitRead(HW_Config,HW_ETHERNET))// Als Ethernet shield aanwezig.
    {
    if(Settings.TransmitIP==VALUE_ON && (Dest==VALUE_SOURCE_HTTP || Dest==VALUE_ALL))
      {
      SendHTTPEvent(Event);
      PrintEvent(Event,VALUE_DIRECTION_OUTPUT,VALUE_SOURCE_HTTP);
      }
    }
   #endif 

  }


#ifdef NODO_MEGA
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

//???herstellen  Event=AnalyzeRawSignal();

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
    LastMessage=MESSAGE_03;
    TransmitCode(command2event(Settings.Unit,CMD_MESSAGE, Settings.Unit, LastMessage),VALUE_ALL);
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




/*********************************************************************************************\
* Deze routine berekent de uit een RawSignal een NODO code
* Geeft een false retour als geen geldig NODO signaal
\*********************************************************************************************/
boolean RawSignal_2_Nodo(struct NodoEventStruct *Event)
  {
  byte b,x,y,z;
 
  if(RawSignal.Number!=16*sizeof(struct NodoEventStruct)+2) // Per byte twee posities + startbit.
    return false;

  byte *B=(byte*)Event; // B wijst naar de eerste byte van de struct
  z=3;  // RwaSignal pulse teller: 0=aantal, 1=startpuls, 2=space na startpuls, 3=1e pulslengte. Dus start loop met drie.

  for(x=0;x<sizeof(struct NodoEventStruct);x++) // vul alle bytes van de struct 
    {
    b=0;
    for(y=0;y<=7;y++) // vul alle bits binnen een byte
      {
      if(RawSignal.Pulses[z]>NODO_PULSE_MID)      
        b|=1<<y; //LSB in signaal wordt  als eerste verzonden
      z+=2;
      }
    *(B+x)=b;
    }

  // bereken checksum: crc-8 uit alle bytes in de queue.
  // Een correcte Checksum met alle bytes levert een nul omdat de XOR van alle bytes in het datablok zit.
  b=0;
  for(x=0;x<sizeof(struct NodoEventStruct);x++)
    b^=*(B+x); 

//  Serial.print("\n*** debug: b=");Serial.println(b); //??? Debug
//  Serial.print("*** debug: Event->Checksum=");Serial.println(Event->Checksum); //??? Debug
//  Serial.print("*** debug: Event->Type=");Serial.println(Event->Type); //??? Debug
//  Serial.print("*** debug: Event->TransmissionFlags=");Serial.println(Event->TransmissionFlags); //??? Debug
//  Serial.print("*** debug: Event->DestinationUnit=");Serial.println(Event->DestinationUnit); //??? Debug
//  Serial.print("*** debug: Event->SourceUnit=");Serial.println(Event->SourceUnit); //??? Debug
//  Serial.print("*** debug: Event->Command=");Serial.println(Event->Command); //??? Debug
//  Serial.print("*** debug: Event->Par1=");Serial.println(Event->Par1); //??? Debug
//  Serial.print("*** debug: Event->Par2=");Serial.println(Event->Par2); //??? Debug

  if(b==0)
    return true;
    
  return false; 
  }

