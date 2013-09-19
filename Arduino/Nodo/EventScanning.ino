#define MIN_PULSE_LENGTH            50  // pulsen korter dan deze tijd uSec. worden als stoorpulsen beschouwd.
#define SIGNAL_TIMEOUT_RF            5 // na deze tijd in mSec. wordt één RF signaal als beëindigd beschouwd.
#define SIGNAL_TIMEOUT_IR           10 // na deze tijd in mSec. wordt één IR signaal als beëindigd beschouwd.
#define SIGNAL_REPEAT_TIME        1000 // Tijd in mSec. waarbinnen hetzelfde event niet nogmaals via RF/IR mag binnenkomen. Onderdrukt ongewenste herhalingen van signaal

boolean I2C_EventReceived=false;       // Als deze vlag staat, is er een I2C event binnengekomen.
struct NodoEventStruct I2C_Event;

boolean ScanEvent(struct NodoEventStruct *Event)// Deze routine maakt deel uit van de hoofdloop en wordt iedere 125uSec. doorlopen
  {
  byte Fetched=0;
  static unsigned long BlockReceivingTimer=0;
  
  // I2C: *************** kijk of er data is binnengekomen op de I2Cbus **********************
  if(I2C_EventReceived)
   {
   // Er is een I2C event binnen gekomen. De gegevens kunnen dan eenvoudig uit de binnengekomen struct worden gekopieerd. 
   *Event=I2C_Event;
   bitWrite(HW_Config,HW_I2C,true);
   I2C_EventReceived    = false;
   Fetched=VALUE_SOURCE_I2C;
   }

  // IR: *************** kijk of er data start **********************
  else if((*portInputRegister(IRport)&IRbit)==0)
    {
    if(FetchSignal(PIN_IR_RX_DATA,LOW,SIGNAL_TIMEOUT_IR))
      {
       if(AnalyzeRawSignal(Event))
        {
        // Als geen repeterend signaal OF Vorige is al enige tijd geleden, dan hebben we een event
        bitWrite(HW_Config,HW_IR_RX,true);
        if(BlockReceivingTimer>millis())
          {
          WaitFree(VALUE_SOURCE_IR, 2000);
          }
        else
          {
          Fetched=VALUE_SOURCE_IR;
          if(RawSignal.Repeats)
            BlockReceivingTimer=millis()+SIGNAL_REPEAT_TIME;
          }
        }
      }
    }

  // RF: *************** kijk of er data start **********************
  else if((*portInputRegister(RFport)&RFbit)==RFbit)
    {
    if(FetchSignal(PIN_RF_RX_DATA,HIGH,SIGNAL_TIMEOUT_RF))
      {
      if(AnalyzeRawSignal(Event))
        {
        bitWrite(HW_Config,HW_RF_RX,true);
        // Als geen repeterend signaal OF Vorige is al enige tijd geleden, dan hebben we een event
        if(BlockReceivingTimer>millis())
          {
          WaitFree(VALUE_SOURCE_RF, 2000);
          }
        else
          {
          Fetched=VALUE_SOURCE_RF;
          if(RawSignal.Repeats)
            BlockReceivingTimer=millis()+SIGNAL_REPEAT_TIME;
          }
        }
      }
    }
  
  if(Fetched)
    {
    Event->Port=Fetched;
    Event->Direction=VALUE_DIRECTION_INPUT;

    // Toets of de versienummers corresponderen. Is dit niet het geval, dan zullen er verwerkingsfouten optreden! Dan een waarschuwing tonen en geen verdere verwerking.
    // Er is een uitzondering: De eerste commando/eventnummers zijn stabiel en mogen van oudere versienummers zijn.
    if(Event->Version!=0 && Event->Version!=NODO_VERSION_MINOR && Event->Command>COMMAND_MAX_FIXED)
      {
      #if NODO_MEGA
      Event->Command=CMD_DUMMY;
      Event->Type=NODO_TYPE_EVENT;
      Event->Par1=0;
      Event->Par2=0;
      PrintEvent(Event,VALUE_ALL);
      RaiseMessage(MESSAGE_VERSION_ERROR);
      #endif
      return false;
      }     

    // Een event kan een verzoek bevatten om bevestiging. Doe dit dan pas na verwerking.
    if(Event->Flags & TRANSMISSION_CONFIRM)
      RequestForConfirm=true;

    // registreer welke Nodo's op welke poort zitten en actualiseer tabel.
    // Wordt gebruikt voor SendTo en I2C communicatie op de Mega.
    // Hiermee kan later automatisch de juiste poort worden geselecteerd met de SendTo en kan in
    // geval van I2C communicatie uitsluitend naar de I2C verbonden Nodo's worden gecommuniceerd.
    NodoOnline(Event->SourceUnit,Event->Port);

    // Als er een specifieke Nodo is geselecteerd, dan moeten andere Nodo's worden gelocked.
    // Hierdoor is het mogelijk dat een master en een slave Nodo tijdelijk exclusief gebruik kunnen maken van de bandbreedte
    // zodat de communicatie niet wordt verstoord.  
    if(Event->DestinationUnit!=0)
      Transmission_SelectedUnit = Event->DestinationUnit;

    // Als het Nodo event voor deze unit bestemd is, dan klaar. Zo niet, dan terugkeren met een false
    // zodat er geen verdere verwerking plaatsvindt.
    if(Event->DestinationUnit==0 || Event->DestinationUnit==Settings.Unit)
      return true;

    }// fetched
  return false;
  }


#if NODO_MEGA
boolean ScanAlarm(struct NodoEventStruct *Event)
  {
  unsigned long Mask;
  
  for(byte x=0;x<ALARM_MAX;x++)
    {
    if((Settings.Alarm[x]>>20)&1) // Als alarm enabled is, dan ingestelde alarmtijd vergelijke met de echte tijd.
      {
      // stel een vergelijkingswaarde op
      unsigned long Cmp=Time.Minutes%10 | (unsigned long)(Time.Minutes/10)<<4 | (unsigned long)(Time.Hour%10)<<8 | (unsigned long)(Time.Hour/10)<<12 | (unsigned long)Time.Day<<16 | 1UL<<20;

      // In het ingestelde alarm kunnen zich wildcards bevinden. Maskeer de posities met 0xF wildcard nibble        
      for(byte y=0;y<8;y++)// loop de acht nibbles van de 32-bit Par2 langs
        {          
        if(((Settings.Alarm[x]>>(y*4))&0xf) == 0xf) // als in nibble y een wildcard waarde 0xf staat
          {
          Mask=0xffffffff  ^ (0xFUL <<(y*4)); // Mask maken om de nibble positie y te wissen.
          Cmp&=Mask;                          // Maak nibble leeg
          Cmp|=(0xFUL<<(y*4));                // vul met wildcard waarde 0xf
          }
        }
      
     if(Settings.Alarm[x]==Cmp) // Als ingestelde alarmtijd overeen komt met huidige tijd.
       {
       if(AlarmPrevious[x]!=Time.Minutes) // Als alarm niet eerder is afgegaan
         {
         AlarmPrevious[x]=Time.Minutes;

         ClearEvent(Event);
         Event->Direction=VALUE_DIRECTION_INPUT;
         Event->Port=VALUE_SOURCE_CLOCK;
         Event->Type=NODO_TYPE_EVENT;
         Event->Command=EVENT_ALARM;
         Event->Par1=x+1;
         return true;
         }
       }
     else
       AlarmPrevious[x]=0xFF;
     }
   }
 return false; 
 }
#endif

