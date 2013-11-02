#define NODO_PULSE_0                 500  // PWM: Tijdsduur van de puls bij verzenden van een '0' in uSec.
#define NODO_PULSE_MID              1000  // PWM: Pulsen langer zijn '1'
#define NODO_PULSE_1                1500  // PWM: Tijdsduur van de puls bij verzenden van een '1' in uSec. (3x NODO_PULSE_0)
#define NODO_SPACE                   500  // PWM: Tijdsduur van de space tussen de bitspuls bij verzenden van een '1' in uSec.   

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
    noInterrupts();//???
    while(x<RawSignal.Number)
      {
      digitalWrite(PIN_RF_TX_DATA,HIGH);
      delayMicroseconds(RawSignal.Pulses[x++]*RawSignal.Multiply); 
      digitalWrite(PIN_RF_TX_DATA,LOW);
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
 * RawSignal.Bits het aantal pulsen*2+startbit*2
 \*********************************************************************************************/

// Definieer een datablock die gebruikt wordt voor de gegevens die via de ether verzonden moeten worden.
struct DataBlockStruct
  {
  byte Version;
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

  Checksum(Event);

  DataBlock.SourceUnit      = Event->SourceUnit | (Settings.Home<<5);  
  DataBlock.DestinationUnit = Event->DestinationUnit;
  DataBlock.Flags           = Event->Flags;
  DataBlock.Type            = Event->Type;
  DataBlock.Command         = Event->Command;
  DataBlock.Par1            = Event->Par1;
  DataBlock.Par2            = Event->Par2;
  DataBlock.Checksum        = Event->Checksum;;
  DataBlock.Version         = NODO_VERSION_MINOR;
  
  byte *B=(byte*)&DataBlock;

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

boolean AnalyzeRawSignal(struct NodoEventStruct *E)
  {
  ClearEvent(E);

  if(RawSignal.Number==RAW_BUFFER_SIZE)// Als het signaal een volle buffer beslaat is het zeer waarschijnlijk ruis of ongeldig signaal
    return false;     

  if(RawSignal_2_Nodo(E))           // Is het een Nodo signaal
    {
    // Als er een Nodo signaal is binnengekomen, dan weten we zeker dat er een Nodo in het landschap is die tijd nodig heeft om
    // weer terug te schakelen naar de ontvangstmode. Dit kost (helaas) enige tijd. Zorg er voor dat er gedurende deze dtijd
    // even geen Nodo event wordt verzonden anders wordt deze vrijwel zeker gemist.
    HoldTransmission=millis()+NODO_TX_TO_RX_SWITCH_TIME;
    return true;
    }
  
  if(Transmission_NodoOnly)
    return false;

  // Loop de devices langs. Indien een device dit nodig heeft, zal deze het rawsignal gebruiken en omzetten naar een
  // geldig event.
  if(PluginCall(PLUGIN_RAWSIGNAL_IN,E,0))
    return true;
    
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
  int x;
  unsigned int MinPulse=0xffff;
  unsigned int MinSpace=0xffff;
  unsigned long CodeM=0L;
  unsigned long CodeS=0L;

  // In enkele gevallen is uitzoeken van het RawSignal zinloos
  if(RawSignal.Number < MIN_RAW_PULSES) return false;  
  
  // zoek de kortste tijd (PULSE en SPACE). Start niet direct vanaf de eerste puls omdat we anders kans 
  // lopen een onvolledige startbit te pakken.
  for(x=5;x<RawSignal.Number;x+=2)
    {
    if(RawSignal.Pulses[x]  < MinPulse)MinPulse=RawSignal.Pulses[x]; // Zoek naar de kortste pulstijd.
    if(RawSignal.Pulses[x+1]< MinSpace)MinSpace=RawSignal.Pulses[x+1]; // Zoek naar de kortste spacetijd.
    }

  // De kortste pulsen zijn gevonden. Dan een 'opslag' zodat alle korte pulsen er royaal
  // onder vallen maar niet de lengte van een lange puls passeren.
  MinPulse+=(MinPulse*RAWSIGNAL_TOLERANCE)/100;
  MinSpace+=(MinSpace*RAWSIGNAL_TOLERANCE)/100;

  // Data kan zowel in de mark als de space zitten. Daarom pakken we beide voor data opbouw.
  for(x=3;x<RawSignal.Number;x+=2)
    {
    CodeM = (CodeM<<1) | (RawSignal.Pulses[x]   > MinPulse);
    CodeS = (CodeS<<1) | (RawSignal.Pulses[x+1] > MinSpace);    
    }

  // Data kan zowel in de mark als de space zitten. We nemen de grootste waarde voor de data.
  if(CodeM > CodeS)  
    event->Par2=CodeM;
  else
    event->Par2=CodeS;

  event->SourceUnit=0;  
  event->DestinationUnit=0;
  event->Type=NODO_TYPE_EVENT;
  event->Command=EVENT_RAWSIGNAL;
  event->Par1=0;
  RawSignal.Repeats=true;

  return true;
  }


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
        b|=1<<y; //LSB in signaal wordt als eerste verzonden
      z+=2;
      }
    *(B+x)=b;
    }

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
  Event->Version=DataBlock.Version;
  Event->Checksum=DataBlock.Checksum;

  if(Checksum(Event))
    return true;

  return false; 
  }

#if NODO_MEGA

/*********************************************************************************************\
 * Kijk of voor de opgegeven Hex-event (Code) een rawsignal file op de SDCard bestaat.
 * Als deze bestaat dan return met 'true'
 \*********************************************************************************************/
boolean RawSignalExist(unsigned long Code)
  {
  int exist=false;  
  char *TempString=(char*)malloc(25);

  SelectSDCard(true);
  sprintf(TempString,"%s/%s.raw",ProgmemString(Text_28),int2strhex(Code)+2); // +2 omdat dan de tekens '0x' niet worden meegenomen. anders groter dan acht posities in filenaam.

  File dataFile=SD.open(TempString);
  if(dataFile) 
    {
    exist=true;
    dataFile.close();
    }  

  // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer W5100 chip
  SelectSDCard(false);
  free(TempString);
  return exist;
  }

/*********************************************************************************************\
 * Sla de pulsen in de buffer Rawsignal op op de SDCard
 \*********************************************************************************************/
byte RawSignalWrite(unsigned long Key)
  {
  boolean error=false;
  char *TempString=(char*)malloc(25);

  int x;

  if(Key!=0)
    {
    // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer SDCard chip
    SelectSDCard(true);
  
    // Sla Raw-pulsenreeks op in bestand met door gebruiker gekozen nummer als filenaam
  
    sprintf(TempString,"%s/%s.raw",ProgmemString(Text_28),int2strhex(Key)+2);
    SD.remove(TempString); // eventueel bestaande file wissen, anders wordt de data toegevoegd.    
    File KeyFile = SD.open(TempString, FILE_WRITE);
    if(KeyFile) 
      {
      byte b,*B=(byte*)&RawSignal;
      for(x=0;x<sizeof(struct RawSignalStruct);x++)
        {
        b=*(B+x); 
        KeyFile.write(b);
        }
      KeyFile.close();
      }
    else 
      error=true;
  
    // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer W5100 chip
    SelectSDCard(false);
  
    free(TempString);
    }

  if(error)
    RaiseMessage(MESSAGE_SDCARD_ERROR,0);

  return !error;
  }


/*********************************************************************************************\
 * Haal de RawSignal pulsen op uit het bestand <key>.raw en sla de reeks op in de 
 * RawSignal buffer, zodat deze vervolgens weer kan worden gebruikt om te verzenden.
 \*********************************************************************************************/
boolean RawSignalLoad(unsigned long Key)
  {
  boolean Ok;
  char *TempString=(char*)malloc(25);

  SelectSDCard(true);
  sprintf(TempString,"%s/%s.raw",ProgmemString(Text_28),int2strhex(Key)+2);

  File dataFile=SD.open(TempString);
  if(dataFile) 
    {
    byte b,*B=(byte*)&RawSignal;
    int x=0;
    
    while(dataFile.available() && x<sizeof(struct RawSignalStruct))
      {
      b=dataFile.read();
      *(B+x)=b; 
      x++;
      }
    dataFile.close();
    Ok=true;
    }
  else
    Ok=false;

  // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer W5100 chip
  SelectSDCard(false);

  free(TempString);
  return Ok;
  }


/*********************************************************************************************\
 * Laat de inhoud van de RawSignalBuffer zien. Als er een hexcode is opgegeven, dan wordt  
 * een poging gedaan om deze op te halen vanaf de SDCard. Als deze daar niet wordt gevonden
 * wordt er een foutmelding gegeven.   
 \*********************************************************************************************/
boolean RawSignalShow(unsigned long Code)
  {

  // Als een hex-code opgegeven, dan deze van SDCard halen. anders de huidige inhoud van de RawSignal buffer gebruiken
  if(Code!=0)
    {
    if(!RawSignalLoad(Code))
      {
      RaiseMessage(MESSAGE_UNABLE_OPEN_FILE,0);
      return false;
      }
    }
  else
    {
    struct NodoEventStruct TempEvent;
    ClearEvent(&TempEvent);
    RawSignal_2_32bit(&TempEvent);
    Code=TempEvent.Par2;
    }
  
  if(Code!=0)
    {
    Serial.print(F("HEX-Code=0x"));
    Serial.print(Code,HEX);
  
    int x;
    unsigned int y,z;
  
    // zoek naar de langste korte puls en de kortste lange puls
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
  
    // zoek naar de langste korte puls en de kortste lange puls
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
  
    Serial.print(F( ", Bits="));
  
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
      }
    if(y==2)
      {
      for(x=2;x<=RawSignal.Number;x+=2)
        {
        y=RawSignal.Pulses[x]*RawSignal.Multiply;
        if(y>SpaceMid)
          Serial.write('1');
        else
          Serial.write('0');
        }
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
    return true;
    }
  }      

#endif