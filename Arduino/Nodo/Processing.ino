
 /**********************************************************************************************\
 * Voert alle relevante acties in de eventlist uit die horen bij het binnengekomen event
 * Doorlopen van een volledig gevulde eventlist duurt ongeveer 15ms inclusief printen naar serial
 * maar exclusief verwerking n.a.v. een 'hit' in de eventlist
 \*********************************************************************************************/
byte ProcessEvent1(struct NodoEventStruct *Event)
  {
  struct NodoEventStruct TempEvent;
  ExecutionDepth=0;               // nesting nog niet aan de orde. Dit is het eerste begin.
  byte error=0;
  
  SerialHold(true);  // als er een regel ontvangen is, dan binnenkomst van signalen stopzetten met een seriele XOFF
  Led(RED); // LED aan als er iets verwerkt wordt      

  #if NODO_MEGA
  if(FileWriteMode!=0)
    return 0;
  #endif


  // Verwerk het binnengekomen event
  error=ProcessEvent2(Event);
    
  // Verwerk eventuele events die in de queue zijn geplaatst.
  QueueProcess();    

  // Een event kan een verzoek bevatten om bevestiging. Doe dit dan pas na alle verwerking.
  if(RequestForConfirm)
    {  
    // Initialiseer een Event en Transmissie
    ClearEvent(&TempEvent);    
    TempEvent.DestinationUnit       = Event->SourceUnit;
    TempEvent.Port                  = Event->Port;
    TempEvent.Type                  = NODO_TYPE_SYSTEM;    // Event is niet voor de gebruiker bedoeld
    TempEvent.Command               = SYSTEM_COMMAND_CONFIRMED;
    TempEvent.Par1                  = RequestForConfirm;
    SendEvent(&TempEvent, false,false,Settings.WaitFree==VALUE_ON);
    RequestForConfirm=0;
    }
  return error;
  }
  
byte ProcessEvent2(struct NodoEventStruct *Event)
  {
  int x,y;
  byte error=0;
  boolean Continue=true;
    
  if(Event->Command==0)
    return error;

  if(++ExecutionDepth>=MACRO_EXECUTION_DEPTH)
    {
    QueuePosition=0;
    Continue=false;
    error=MESSAGE_05; // bij geneste loops ervoor zorgen dat er niet meer dan MACRO_EXECUTION_DEPTH niveaus diep macro's uitgevoerd worden
    }

  //  PrintNodoEvent("ProcessEvent2();",Event);//???
  
  // Komt er een SendTo event voorbij, dan deze en opvolgende separaat afhandelen
  if(Continue && (Event->Command==CMD_SENDTO)) // Is dit event de eerste uit een [SendTo] reeks?
    {
    QueueReceive(Event);
    Continue=false;
    }

  // Als er een LOCK actief is, dan commando's blokkeren behalve...
  if(Settings.Lock && (Event->Port==VALUE_SOURCE_RF || Event->Port==VALUE_SOURCE_IR ))
    {
    if(millis()>60000) // de eerste minuut is de lock nog niet actief. Ontsnapping voor als abusievelijk ingesteld
      {
      switch(Event->Command) // command
        {
        // onderstaande mogen WEL worden doorgelaten als de lock aan staat
        case CMD_LOCK:                // om weer te kunnen unlocken
        case EVENT_USEREVENT:           // Noodzakelijk voor uitwisseling userevents tussen Nodo.
        case CMD_STATUS:              // uitvragen status is onschuldig en kan handig zijn.
        case EVENT_MESSAGE:             // Voorkomt dat een message van een andere Nodo een error genereert
        case EVENT_BOOT:          // Voorkomt dat een boot van een adere Nodo een error genereert
          break;

        default:
          Continue=false;
          RaiseMessage(MESSAGE_10);
        }
      }
    }

  // systeem commando's niet uitvoeren
  if(Continue && (Event->Type == NODO_TYPE_SYSTEM))
    {
    Continue=false;
    }

  // Als de queue vlag staat, dan direct in de queue stoppen en verder niets mee doen.
  if(Continue && (Event->Flags & TRANSMISSION_QUEUE))
    {
    QueueAdd(Event);
    Continue=false;
    }

  // restanten van een niet correct verwekt SndTo niet uitvoeren
  if(Continue && (Event->Flags & TRANSMISSION_SENDTO))
    {
    Continue=false;
    }


  #if NODO_MEGA
  if(Continue && bitRead(HW_Config,HW_SDCARD))
    if(Event->Command==EVENT_RAWSIGNAL && Settings.RawSignalSave==VALUE_ON)
      if(!RawSignalExist(Event->Par2))
        RawSignalSave(Event->Par2);
  #endif    
  
  if(Continue && error==0)
    {
    #if NODO_MEGA
    PrintEvent(Event, VALUE_ALL);
    #endif
    
    // ############# Verwerk event ################  

    // Check of het een binnengekomen device event is. Deze kan een commando zijn of een event. Voer device opdracht uit als een commando.
    if(Event->Type==NODO_TYPE_DEVICE_COMMAND)
      {
      error=MESSAGE_17;
      for(x=0;Device_ptr[x]!=0 && x<DEVICE_MAX; x++)
        {
        if(Device_id[x]==Event->Command)
          {
          error=0;// Device een error terug laten geven?
          if(Device_ptr[x](DEVICE_COMMAND,Event,0)!=true)
            {
            Serial.print(F("*** debug: Na DEVICE_COMMAND foutcode retour van device "));Serial.println(x); //??? Debug
            }
          else
            {
            Serial.print(F("*** debug: DEVICE_COMMAND met success uitgevoerd voor device "));Serial.println(x); //??? Debug
            }
          }
        }
      }
   
    // als het een Nodo event is en een geldig commando, dan deze uitvoeren
    if(error==0)
      {
      if(Event->Type==NODO_TYPE_COMMAND)
        {
        error=ExecuteCommand(Event);
        }
      else
        {
        // loop de gehele eventlist langs om te kijken of er een treffer is.   
        struct NodoEventStruct EventlistEvent, EventlistAction;   
  
        // sla event op voor later gebruik in SendEvent en VariableEvent.
        LastReceived=*Event;
  
        x=0;
        while(Eventlist_Read(++x,&EventlistEvent,&EventlistAction) && error==0) // Zolang er nog regels zijn in de eventlist...
          {      
          if(CheckEvent(Event,&EventlistEvent)) // Als er een match is tussen het binnengekomen event en de regel uit de eventlist.
            {        
            ExecutionLine=x;
            error=ProcessEvent2(&EventlistAction);
            }
          }
        // abort is geen fatale error/break. Deze niet verder behandelen als een error.
        if(error==MESSAGE_15)
          error=0;
        }      
      // Als de SendTo niet permanent is ingeschakeld, dan deze weer uitzetten
      if(Transmission_SendToAll!=VALUE_ALL)Transmission_SendToUnit=0;
      }
    }
    
  ExecutionDepth--;
  return error;
  }

 /**********************************************************************************************\
 * Toetst of Event ergens als entry voorkomt in de Eventlist. Geeft False als de opgegeven code niet bestaat.
 * geeft positie terug bij een match.
 \*********************************************************************************************/
byte CheckEventlist(struct NodoEventStruct *Event)
  {
  struct NodoEventStruct MacroEvent,MacroAction;

  int x=1;
  while(Eventlist_Read(x++,&MacroEvent,&MacroAction))
    if(MacroEvent.Command)
      if(CheckEvent(Event,&MacroEvent))
        return x; // match gevonden

  return false;
  }


 /**********************************************************************************************\
 * Vergelijkt twee events op matching voor uitvoering
 \*********************************************************************************************/
boolean CheckEvent(struct NodoEventStruct *Event, struct NodoEventStruct *MacroEvent)
  {  
  // geen lege events verwerken
  if(MacroEvent->Command==0 || Event->Command==0)
    return false;  
    
  // #### EXACT: als huidige event exact overeenkomt met het event in de regel uit de Eventlist, dan een match
  if(MacroEvent->Type    == Event->Type    &&
     MacroEvent->Command == Event->Command &&
     MacroEvent->Par1    == Event->Par1    &&
     MacroEvent->Par2    == Event->Par2    )
       return true; 

  // ### WILDCARD:      
  if(MacroEvent->Command == EVENT_WILDCARD)                                                                                 // is regel uit de eventlist een WildCard?
    if( MacroEvent->Par1==VALUE_ALL          ||   MacroEvent->Par1==Event->Port)                                            // Correspondeert de poort of mogen alle poorten?
      if((MacroEvent->Par2&0xff)==VALUE_ALL  ||  (MacroEvent->Par2&0xff)==Event->Command && Event->Type==NODO_TYPE_EVENT)   // Correspondeert het commando deel als het een commando is, of mogen alle
        if(((MacroEvent->Par2>>8)&0xff)==0   || ((MacroEvent->Par2>>8)&0xff)==Event->SourceUnit)                            // Correspondeert het unitnummer of is deze niet opgegeven
          return true;
          
  // Als de typen events niet corresponderen, dan gelijk terug
  if(MacroEvent->Type != Event->Type)
    return false;

  // ### USEREVENT: beschouw bij een UserEvent een 0 voor Par1 of Par2 als een wildcard.
  if(Event->Type==NODO_TYPE_EVENT)
    if(Event->Command==EVENT_USEREVENT && MacroEvent->Command==EVENT_USEREVENT)                             // Is het een UserCommand?
      if( (Event->Par1==MacroEvent->Par1 || MacroEvent->Par1==0 || Event->Par1==0)                          // Par1 deel een match?
       && (Event->Par2==MacroEvent->Par2 || MacroEvent->Par2==0 || Event->Par2==0))                         // Par2 deel een match?
         return true; 
    
  // Herkomst van een andere Nodo, dan er niets meer mee doen TENZIJ het een UserEvent is. Die werd hierboven 
  // al afgevangen.
  if(Event->SourceUnit!=0  && Event->SourceUnit!=Settings.Unit)
    return false;


  // ### TIME:
  if(Event->Command==EVENT_TIME) // het binnengekomen event is een clock event.
    {
    // Structuur technisch hoort onderstaande regel hier thuis, maar qua performance niet optimaal!
    unsigned long Cmp=MacroEvent->Par2;
    unsigned long Inp=Event->Par2;

    // In het event in de eventlist kunnen zich wildcards bevinden. Maskeer de posities met 0xF wildcard nibble 
    // doe dit kruislinks omdat zowel het invoer event als het event in de eventlist wildcards kunnen bevatten.
    for(byte y=0;y<8;y++)// loop de acht nibbles van de 32-bit Par2 langs
      {          
      unsigned long Mask=0xffffffff  ^ (0xFUL <<(y*4)); // Masker maken om de nibble positie y te wissen.
      if(((Inp>>(y*4))&0xf) == 0xf)                     // als in nibble y een wildcard waarde 0xf staat
        {
        Cmp&=Mask;                                      // Maak nibble leeg
        Cmp|=(0xFUL<<(y*4));                            // vul met wildcard waarde 0xf
        }
      if(((Cmp>>(y*4))&0xf) == 0xf)                     // als in nibble y een wildcard waarde 0xf staat
        {
        Inp&=Mask;                                      // Maak nibble leeg
        Inp|=(0xFUL<<(y*4));                            // vul met wildcard waarde 0xf
        }
      }
            
     if(Inp==Cmp) // Als ingestelde alarmtijd overeen komt met huidige tijd.
       return true;
     }

  return false; // geen match gevonden
  }


//#######################################################################################################
//##################################### Processing: Queue ###############################################
//#######################################################################################################

// De queue is een tijdelijke wachtrij voor ontvangen of te verzenden events. 
// De queue bevat event gegevens behalve de transmissie gegevens. 
struct QueueStruct
  {
  byte Flags;
  byte Port;
  byte Unit;
  byte Type;
  byte Command;
  byte Par1;
  unsigned long Par2;
  }Queue[EVENT_QUEUE_MAX];

 /**********************************************************************************************\
 * Voeg event toe aan de queue.
 \*********************************************************************************************/
boolean QueueAdd(struct NodoEventStruct *Event)
  {
  if(QueuePosition<EVENT_QUEUE_MAX)
    {
    Queue[QueuePosition].Flags   = Event->Flags;
    Queue[QueuePosition].Port    = Event->Port;
    Queue[QueuePosition].Unit    = Event->SourceUnit;
    Queue[QueuePosition].Type    = Event->Type;
    Queue[QueuePosition].Command = Event->Command;
    Queue[QueuePosition].Par1    = Event->Par1;
    Queue[QueuePosition].Par2    = Event->Par2;
    QueuePosition++;           
    return true;
    }
  return false;
  }


 /**********************************************************************************************\
 * Verwerk de inhoud van de queue
 * Geeft het aantal verwerkte events uit de queue terug.
 \*********************************************************************************************/
void QueueProcess(void)
  {
  byte x;

  // Initialiseer een Event en Transmissie
  struct NodoEventStruct Event;
  ClearEvent(&Event);

  if(QueuePosition>0)
    {
    for(x=0;x<QueuePosition;x++)
      {  
      // Als er in de queue een EventlistWrite commando zit, dan moeten de twee opvolgende posities uit de queue
      // worden weggeschreven naar de eventlist.      
      if(Queue[x].Command==CMD_EVENTLIST_WRITE && x<(QueuePosition-2)) // cmd
        {
        struct NodoEventStruct E;
        E.Type=Queue[x+1].Type;
        E.Command=Queue[x+1].Command;
        E.Par1=Queue[x+1].Par1;
        E.Par2=Queue[x+1].Par2;

        struct NodoEventStruct A;
        A.Type=Queue[x+2].Type;
        A.Command=Queue[x+2].Command;
        A.Par1=Queue[x+2].Par1;
        A.Par2=Queue[x+2].Par2;
       
        if(Eventlist_Write(Queue[x].Par1, &E, &A))
          x+=2;
        else
          RaiseMessage(MESSAGE_06);    
        }
      else
        {
        Event.SourceUnit=Queue[x].Unit;
        Event.Type=Queue[x].Type;
        Event.Command=Queue[x].Command;
        Event.Par1=Queue[x].Par1;
        Event.Par2=Queue[x].Par2;
        Event.Direction=VALUE_DIRECTION_INPUT;
        Event.Port=Queue[x].Port;
        Event.Flags=Queue[x].Flags;
        
        RaiseMessage(ProcessEvent2(&Event));      // verwerk binnengekomen event.
        }
      }
    }
  QueuePosition=0;
  }

#if NODO_MEGA  
/*********************************************************************************************\
 * Deze routine verzendt de inhoud van de queue naar een andere Nodo.
 \*********************************************************************************************/
byte QueueSend(void)
  {
  byte x,Port,error=MESSAGE_11, Retry=0;
  unsigned long ID=millis();
  struct NodoEventStruct Event;

  // We maken tijdelijk gebruik van een SendQueue zodat de reguliere queue zijn werk kan blijven doen.
  struct QueueStruct SendQueue[EVENT_QUEUE_MAX];
  for(x=0;x<QueuePosition;x++)
    SendQueue[x]=Queue[x];

  byte SendQueuePosition=QueuePosition;
  QueuePosition=0;
  
  // De port waar de SendTo naar toe moet halen we uit de lijst met Nodo's die wordt onderhouden door de funktie NodoOnline();
  Port=NodoOnline(Transmission_SendToUnit,0);

  // als de Nodo nog niet bekend is, dan nemen we aan dat deze via RF benaderbaar is.
  if(Port==0)
    error=MESSAGE_12;
  else
    { 
    // Eerste fase: Zorg dat de inhoud van de queue correct aan komt op de slave.
    do
      {
      ClearEvent(&Event);
      Event.SourceUnit          = Settings.Unit;
      Event.Port                = Port;
      Event.Type                = NODO_TYPE_SYSTEM;      
      Event.Command             = CMD_SENDTO;      
      Event.Par1                = SendQueuePosition;   // Aantal te verzenden events in de queue. Wordt later gecheckt en teruggezonden in de confirm.
      Event.Par2                = ID;
      Event.Flags               = TRANSMISSION_SENDTO | TRANSMISSION_NEXT | TRANSMISSION_LOCK;
      SendEvent(&Event,false,false,Settings.WaitFree==VALUE_ON);         // Alleen de eerste vooraf laten gaan door een WaitFree;
            
      // Verzend alle events uit de queue. Alleen de bestemmings Nodo zal deze events in de queue plaatsen
      for(x=0;x<SendQueuePosition;x++)
        {
        ClearEvent(&Event);
        Event.SourceUnit          = Settings.Unit;
        Event.Port                = Port;
        Event.Type                = SendQueue[x].Type;
        Event.Command             = SendQueue[x].Command;
        Event.Par1                = SendQueue[x].Par1;
        Event.Par2                = SendQueue[x].Par2;

        // laatste verzonden event markeren als laatste in de sequence.
        if(x==SendQueuePosition-1)
          Event.Flags = TRANSMISSION_SENDTO;
        else        
          Event.Flags = TRANSMISSION_SENDTO | TRANSMISSION_NEXT | TRANSMISSION_LOCK;

        SendEvent(&Event,false,false,false);
        }
      
      // De ontvangende Nodo verzendt als het goed is een bevestiging dat het is ontvangen en het aantal commando's
      ClearEvent(&Event);
      Event.SourceUnit          = Transmission_SendToUnit;
      Event.Command             = SYSTEM_COMMAND_CONFIRMED;
      Event.Type                = NODO_TYPE_SYSTEM;


      if(Wait(10,false,&Event,false))
        if(x==Event.Par1)
          error=0;

      // Als er een timeout was of het aantel events is niet correct bevestigd, dan de gebruiker een waarschuwing tonen
      if(error)
        PrintString(ProgmemString(Text_08),VALUE_ALL);

      }while((++Retry<5) && error);    
    }
  return error;
  }
#endif


void QueueReceive(NodoEventStruct *Event)
  {
  static unsigned long PreviousID=0L;

  // Alle Nodo events die nu binnen komen op slaan in de queue.
  Transmission_NodoOnly=true;
  QueuePosition=0;
  Wait(10, false, 0, true);  
  byte count=QueuePosition;
  
  // Als aantal regels in de queue overeenkomt met wat in het SendTo verzoek is vermeldt
  // EN de queue is nog niet eerder binnen gekomen (ID) dan uitvoeren.
  // Stuur vervolgens de master ter bevestiging het aantal ontvangen events die zich nu in de queue bevinden.

  if(PreviousID!=Event->Par2)
    {
    if(QueuePosition==Event->Par1)
      {
      PreviousID=Event->Par2;

      // Haal de SendTo vlag van de events af.
      for(byte x=0;x<QueuePosition;x++)
        {
        bitClear(Queue[x].Flags,TRANSMISSION_SENDTO);// Haal SENDTO bit er af, anders worden de events niet verwerkt n.a.v. onderdrukken incomplete SendTo reeksen.
        Queue[x].Flags=0;
        }
      QueueProcess();
      }
    }

  QueuePosition=0;

// delay(1000);//??? test

  struct NodoEventStruct TempEvent;
  ClearEvent(&TempEvent);
  TempEvent.DestinationUnit     = Event->SourceUnit;
  TempEvent.SourceUnit          = Settings.Unit;
  TempEvent.Port                = Event->Port;
  TempEvent.Type                = NODO_TYPE_SYSTEM; 
  TempEvent.Command             = SYSTEM_COMMAND_CONFIRMED;      
  TempEvent.Par1                = count;
  SendEvent(&TempEvent,false, false, false);

  Transmission_NodoOnly=false;
  }
