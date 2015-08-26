

unsigned long RepeatingTimer=0L;
unsigned long EventHashPrevious=0,SignalHash,SignalHashPrevious=0L;

boolean ScanEvent(struct NodoEventStruct *Event)                                // Deze routine maakt deel uit van de hoofdloop en wordt iedere 125uSec. doorlopen
  {
  byte Fetched=0;
  static byte Focus=0;
  static boolean BlockRepeatsStatus=false;
  unsigned long Timer=millis()+SCAN_HIGH_TIME;

  while(Timer>millis() || RepeatingTimer>millis())
    {
    #if HARDWARE_NRF24L01
    if(Focus==0 || Focus==VALUE_SOURCE_NRF24L01)
      {
      if(Port_NRF24L01_EventReceived(Event))
        {
        Fetched=VALUE_SOURCE_NRF24L01;
        Focus=Fetched;
        }
      }
    #endif

    #if HARDWARE_I2C
    if(Focus==0 || Focus==VALUE_SOURCE_I2C)
      {
      if(Port_I2C_EventReceived(Event))
        {
        Fetched=VALUE_SOURCE_I2C;
        Focus=Fetched;
        }
      }
    #endif
  
    #if HARDWARE_INFRARED
    if(Focus==0 || Focus==VALUE_SOURCE_IR)
      {
      if(!HW_Status(HW_PULSE) && FetchSignal(PIN_IR_RX_DATA,LOW))               // IR: *************** kijk of er data start **********************
        {
        if(AnalyzeRawSignal(Event))
          {
          Fetched=VALUE_SOURCE_IR;
          Focus=Fetched;
          }
        }
      }
  	#endif

    #if HARDWARE_RF433
    if(Focus==0 || Focus==VALUE_SOURCE_RF)
      {
      if(FetchSignal(PIN_RF_RX_DATA,HIGH))                                      // RF: *************** kijk of er data start **********************
        {
        // PrintRawSignal(); //???
        if(AnalyzeRawSignal(Event))
          {
          Fetched=VALUE_SOURCE_RF;
          Focus=Fetched;
          }
        }
      }
    #endif

    if(Fetched)
      {
      SignalHash=(Event->Command<<24 | Event->Type<<16 | Event->Par1<<8) ^ Event->Par2;
      Event->Port=Fetched;
      Event->Direction=VALUE_DIRECTION_INPUT;
      Fetched=0;
      
      #if HARDWARE_RAWSIGNAL || HARDWARE_RF433 || HARDWARE_INFRARED
      if(RawSignal.RepeatChecksum)RawSignal.Repeats=true;
      #endif  
          
      // Er zijn een aantal situaties die moeten leiden te een event. Echter er zijn er ook die (nog) niet mogen leiden 
      // tot een event en waar het binnengekomen signaal moet worden onderdrukt.
      
      // 1. Het is een (niet repeterend) Nodo signaal of is de herkomst I2C => Alle gevallen doorlaten
      if(Event->Type==NODO_TYPE_EVENT || Event->Type==NODO_TYPE_COMMAND || Event->Type==NODO_TYPE_SYSTEM || Event->Port==VALUE_SOURCE_I2C || Event->Port==VALUE_SOURCE_NRF24L01)
        Fetched=1;      

      #if HARDWARE_RAWSIGNAL || HARDWARE_RF433 || HARDWARE_INFRARED
      // 2. Het (mogelijk repeterend) binnenkomende signaal is niet recent eerder binnengekomen, zoals plugin signalen als KAKU, NewKAKU, ... => Herhalingen onderdrukken  
      else if(!RawSignal.RepeatChecksum && (SignalHash!=SignalHashPrevious || RepeatingTimer<millis())) 
        Fetched=2;

      // 3. Het is een herhalend signaal waarbij een herhaling wordt gebruikt als checksum zoals RAwSignals => Pas na twee gelijke signalen een event.
      else if(RawSignal.RepeatChecksum && SignalHash==SignalHashPrevious && (SignalHash!=EventHashPrevious || RepeatingTimer<millis())) 
        Fetched=3;
      #endif

      SignalHashPrevious=SignalHash;
      RepeatingTimer=millis()+SIGNAL_REPEAT_TIME; 
      
      if(Fetched)
        {
        // als het informatie uitwisseling tussen Nodo's betreft...
        if(Event->Type==NODO_TYPE_EVENT || Event->Type==NODO_TYPE_COMMAND || Event->Type==NODO_TYPE_SYSTEM)
          {
          bitWrite(BusyNodo, Event->SourceUnit,(Event->Flags&TRANSMISSION_BUSY)>0);

          // registreer welke Nodo's op welke poort zitten en actualiseer tabel.
          // Hiermee kan later automatisch bij verzenden van events de juiste poort worden geselecteerd.
          // Het kan echter zijn dat een Nodo event binnenkomt op een andere poort dan
          // de geregistreerde voorkeurspoort. In dit geval wordt het event NIET doorgelaten omdat anders mogelijk 
          // het event vaker binnenkomt. Nodos communiceren onderling met de voorkeurspoort.

          if(NodoOnline(Event->SourceUnit,Event->Port,true)!=Event->Port)
            return false;
          }
                  
        if(Event->DestinationUnit==0 || Event->DestinationUnit==Settings.Unit)
          {
          EventHashPrevious=SignalHash;
          // PrintNodoEvent("DEBUG: Ontvangen event:",Event);//???
          return true;
          }
        }
      Fetched=0; 
      }
    }// while
  Focus=0;
  return false;
  }

#if NODO_MEGA
#if HARDWARE_CLOCK && HARDWARE_I2C
boolean ScanAlarm(struct NodoEventStruct *Event)
  {
  unsigned long Mask;
  byte y,z;
    
  for(byte x=0;x<ALARM_MAX;x++)
    {
    if((Settings.Alarm[x]>>20)&1)                                               // Als alarm enabled is, dan ingestelde alarmtijd vergelijke met de echte tijd.
      {
      
      // Twee opties moeten afwijkend worden behandeld: Werkdagen en weekend.
      z=Time.Day;                                                               // Dag van vandaag
      y=(Settings.Alarm[x]>>16)&0xf;                                            // De door de gebruiker opgegeven dag

      if(y==8 && z>=2 && z<=6)                                                  // als werkdag
        z=y;                                                                    // dan event

      if(y==9 && (z==1 || z==7))                                                // als weekend dag
        z=y;                                                                    // dan event

      // stel een vergelijkingswaarde op
      unsigned long Cmp=Time.Minutes%10 | (unsigned long)(Time.Minutes/10)<<4 | (unsigned long)(Time.Hour%10)<<8 | (unsigned long)(Time.Hour/10)<<12 | (unsigned long)z<<16 | 1UL<<20;

                                                                                // In het ingestelde alarm kunnen zich wildcards bevinden. 
                                                                                // Maskeer de posities met 0xF wildcard nibble. 
                                                                                // Loop de acht nibbles van de 32-bit Par2 langs        
      for(byte y=0;y<8;y++)
        {          
        if(((Settings.Alarm[x]>>(y*4))&0xf) == 0xf)                             // als in nibble y een wildcard waarde 0xf staat
          {
          Mask=0xffffffff  ^ (0xFUL <<(y*4));                                   // Mask maken om de nibble positie y te wissen.
          Cmp&=Mask;                                                            // Maak nibble leeg
          Cmp|=(0xFUL<<(y*4));                                                  // vul met wildcard waarde 0xf
          }
        }
      
     if(Settings.Alarm[x]==Cmp)                                                 // Als ingestelde alarmtijd overeen komt met huidige tijd.
       {
       if(true || AlarmPrevious[x]!=Time.Minutes)                                       // Als alarm niet eerder is afgegaan
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
#endif clock
#endif