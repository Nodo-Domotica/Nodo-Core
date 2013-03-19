
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

  #ifdef NODO_MEGA
  if(FileWriteMode!=0)
    return 0;
  LastReceived=*Event;            // LastReceived wordt gebruikt om later de status van laatste ontvangen event te kunnen gebruiken t.b.v. substitutie van vars.
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
    TempEvent.Flags                 = TRANSMISSION_SYSTEM;    // Event is niet voor de gebruiker bedoeld
    TempEvent.Command               = SYSTEM_COMMAND_CONFIRMED;
    TempEvent.Par1                  = RequestForConfirm;
    SendEvent(&TempEvent, false,false,false);

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

  ExecutionDepth++;
      
  if(Settings.Lock) // verkassen naar ProcessEvent1 of scanevent???
    {
    if((Event->Port==VALUE_SOURCE_RF || Event->Port==VALUE_SOURCE_IR) && millis()>60000) // de eerste minuut is de lock nog niet actief. Ontsnapping voor als abusievelijk ingesteld
      {
      switch(Event->Command) // command
        {
        // onderstaande mogen WEL worden doorgelaten als de lock aan staat
        case CMD_LOCK:                // om weer te kunnen unlocken
        case CMD_USEREVENT:           // Noodzakelijk voor uitwisseling userevents tussen Nodo.
        case CMD_STATUS:              // uitvragen status is onschuldig en kan handig zijn.
        case CMD_MESSAGE:             // Voorkomt dat een message van een andere Nodo een error genereert
        case CMD_BOOT_EVENT:          // Voorkomt dat een boot van een adere Nodo een error genereert
          break;
        //??? devices hier nog opnemen!          
        default:
          error=MESSAGE_09;    
        }
      }
    }

  if(Continue && (Event->Command==CMD_SENDTO)) // Is dit event de eerste uit een [SendTo] reeks?
    {
    QueueReceive(Event);
    Continue=false;
    }

  if(Continue && (Event->Flags & TRANSMISSION_SYSTEM))
    {
    // PrintNodoEvent("*** Debug: System commando", Event);//???
    Continue=false;
    }

  if(Continue && (Event->Flags & TRANSMISSION_QUEUE))
    {
    QueueAdd(Event);
    Continue=false;
    }


  #ifdef NODO_MEGA
  if(bitRead(HW_Config,HW_SDCARD))
    if(Event->Command==CMD_RAWSIGNAL && Settings.RawSignalSave==VALUE_ON)
      if(!RawSignalExist(Event->Par2))
        RawSignalSave(Event->Par2);
  #endif    
  
  if(Continue && error==0)
    {
    PrintEvent(Event);  // geef event weer op Terminal
    LastReceived=*Event;// Bewaar event als -vorig- event. 
  
    if(ExecutionDepth>=MACRO_EXECUTION_DEPTH)
      {
      QueuePosition=0;
      error=MESSAGE_05; // bij geneste loops ervoor zorgen dat er niet meer dan MACRO_EXECUTION_DEPTH niveaus diep macro's uitgevoerd worden
      }
  
    else
      {
      // ############# Verwerk event ################  

      // kijk of het een Device betreft. De device ID's bevinden zich in een kleine tabel. Zoek din de tabel naar het betreffende ID en voer 
      // de DEVICE_COMMAND opdracht uit. Als het betreffende device geen DEVICE_COMMAND code heeft, dan wordt er ook niets uitgevoerd.
      for(x=0; x<DEVICE_MAX; x++)
        if(Device_id[x]==(Event->Command-CMD_DEVICE_FIRST))
          Device_ptr[x](DEVICE_COMMAND,Event,0);

      // als het een Nodo event is en een geldig commando, dan deze uitvoeren
      if(NodoType(Event)==NODO_TYPE_COMMAND)
        { // Er is een geldig Commando voor deze Nodo binnengekomen                   
        error=ExecuteCommand(Event);
        }        
      else
        {// Er is een Event binnengekomen  
        // loop de gehele eventlist langs om te kijken of er een treffer is.   
        struct NodoEventStruct EventlistEvent, EventlistAction;   
    
        x=0;
        while(Eventlist_Read(++x,&EventlistEvent,&EventlistAction)) // Zolang er nog regels zijn in de eventlist...
          {      
          if(CheckEvent(Event,&EventlistEvent)) // Als er een match is tussen het binnengekomen event en de regel uit de eventlist.
            {        
            EventlistAction.Port = VALUE_SOURCE_EVENTLIST;
            
            #ifdef NODO_MEGA
            // op dit punt in de code worden de Checksum, Type niet gebruikt.
            // Wellicht niet mooi, maar deze misbruiken we even om de positie in de eventlist en de nesting
            // diepte aan te geven.
            EventlistAction.Checksum=x/256;
            EventlistAction.Direction=x%256;
            #endif
    
            // De actie uit de eventlist kan van het type commando of event zijn. 
            if(NodoType(&EventlistAction)==NODO_TYPE_COMMAND) // is de ontvangen code een uitvoerbaar commando?
              {
              PrintEvent(&EventlistAction);
              if(error=ExecuteCommand(&EventlistAction))
                break;
              }
            else
              {// het is een (nieuw) event;
              if(error=ProcessEvent2(&EventlistAction))
                break;
              }
            }
          }
        }      
      }
    }

  // abort is geen fatale error. Deze niet verder behandelen als een error.
  if(error==MESSAGE_15)
    error=0;
    
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
    {
    if(MacroEvent.Command)
      {
      if(CheckEvent(Event,&MacroEvent))
        return x; // match gevonden
      }
    }
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

  // ### WILDCARD:      
  if(MacroEvent->Command == CMD_COMMAND_WILDCARD) // is regel uit de eventlist een WildCard?
    if( MacroEvent->Par1==VALUE_ALL        ||  MacroEvent->Par1==Event->Port)
      if((MacroEvent->Par2&0xff)==VALUE_ALL  || (MacroEvent->Par2&0xff)==Event->Command)
        if(((MacroEvent->Par2>>8)&0xff)==0       || ((MacroEvent->Par2>>8)&0xff)==Event->SourceUnit)
          return true;

  // USEREVENT:
  // beschouw bij een UserEvent een 0 voor Par1 of Par2 als een wildcard.
  if(Event->Command==CMD_USEREVENT && MacroEvent->Command==CMD_USEREVENT)// Command
    {
    if( (Event->Par1==MacroEvent->Par1 || MacroEvent->Par1==0 || Event->Par1==0)  // Par1 deel een match
     && (Event->Par2==MacroEvent->Par2 || MacroEvent->Par2==0 || Event->Par2==0)) // Par2 deel een match
        return true; 
    }

  // Herkomst van een andere Nodo, dan er niets meer mee doen TENZIJ het een UserEvent is.
  if(Event->SourceUnit!=0  && Event->SourceUnit!=Settings.Unit)
    return false;
    
  // #### EXACT: als huidige event exact overeenkomt met het event in de regel uit de Eventlist, dan een match
  if(MacroEvent->Command == Event->Command &&
     MacroEvent->Par1    == Event->Par1    &&
     MacroEvent->Par2    == Event->Par2    )
       return true; 
       
  // ### TIME:
  if(Event->Command==CMD_TIME) // het binnengekomen event is een clock event.//???
    {
    // Structuur technisch hoort onderstaande regel hier thuis, maar qua performance niet wenselijk!
    unsigned long Cmp=MacroEvent->Par2; //???Time.Minutes%10 | (unsigned long)(Time.Minutes/10)<<4 | (unsigned long)(Time.Hour%10)<<8 | (unsigned long)(Time.Hour/10)<<12 | (unsigned long)Time.Day<<16;
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
        E.Command=Queue[x+1].Command;
        E.Par1=Queue[x+1].Par1;
        E.Par2=Queue[x+1].Par2;

        struct NodoEventStruct A;
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
        Event.Command=Queue[x].Command;
        Event.Par1=Queue[x].Par1;
        Event.Par2=Queue[x].Par2;
        Event.Direction=VALUE_DIRECTION_INPUT;
        Event.Port=Queue[x].Port;
        Event.Flags=Queue[x].Flags;
        
        ProcessEvent2(&Event);      // verwerk binnengekomen event.
        }
      }
    }
  QueuePosition=0;
  }

#ifdef NODO_MEGA  
/*********************************************************************************************\
 * Deze routine verzendt de inhoud van de queue naar een andere Nodo.
 \*********************************************************************************************/
byte QueueSend(byte DestUnit)
  {
  byte x,Port,error=true,Retry=0;
  unsigned long ID=millis();
  struct NodoEventStruct Event;

  // We maken tijdelijk gebruik van een SendQueue zodat de regliere queue zijn werk kan blijven doen.
  struct QueueStruct SendQueue[EVENT_QUEUE_MAX];
  for(x=0;x<QueuePosition;x++)
    SendQueue[x]=Queue[x];
  byte SendQueuePosition=QueuePosition;
  QueuePosition=0;
  
  // De port waar de SendTo naar toe moet halen we uit de lijst met Nodo's die wordt onderhouden door de funktie NodoOnline();
  Port=NodoOnline(DestUnit,0);

  // als de Nodo nog niet bekend is, dan nemen we aan dat deze via RF benaderbaar is.
  if(Port==0)
    error=MESSAGE_12;
  else
    { 
    // Eerste fase: Zorg dat de inhoud van de queue correct aan komt op de slave.
    do
      {
      ClearEvent(&Event);
      Event.DestinationUnit     = DestUnit;
      Event.SourceUnit          = Settings.Unit;
      Event.Port                = Port;
      Event.Command             = CMD_SENDTO;      
      Event.Par1                = SendQueuePosition;
      Event.Par2                = ID;
      Event.Flags               = TRANSMISSION_SENDTO | TRANSMISSION_NEXT | TRANSMISSION_LOCK | TRANSMISSION_SYSTEM; 
      SendEvent(&Event,false,false,false);
            
      // Verzend alle events uit de queue. Alleen de bestemmings Nodo zal deze events in de queue plaatsen
      for(x=0;x<SendQueuePosition;x++)
        {
        // Alleen de eerste vooraf laten gaan door een WaitFree;
        if(x>0)
          Settings.WaitFree=VALUE_OFF;
              
        // laatste verzonden event markeren als laatste in de sequence.
        if(x==SendQueuePosition-1)
          Event.Flags = TRANSMISSION_SENDTO;
        
        Event.Command             = SendQueue[x].Command;
        Event.Par1                = SendQueue[x].Par1;
        Event.Par2                = SendQueue[x].Par2;
        SendEvent(&Event,false,false,false);
        }
      
      // De ontvangende Nodo verzendt als het goed is een bevestiging dat het is ontvangen en het aantal commando's
      ClearEvent(&Event);
      Event.SourceUnit          = DestUnit;
      Event.Command             = SYSTEM_COMMAND_CONFIRMED;
  
      if(Wait(30,false,&Event,false))
        if(x==Event.Par1)
          error=false;
  
      // Als er een timeout was of het aantel events is niet correct bevestigd, dan de gebruiker een waarschuwing tonen
      if(error)
        PrintTerminal(ProgmemString(Text_08));
  
      }while((++Retry<10) && error);    
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
  Wait(10, false,0 , true);  
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
        // bitClear(Queue[x].Flags,TRANSMISSION_SENDTO);//???
        Queue[x].Flags=0;
        
      QueueProcess();
      }
    }
  QueuePosition=0;

  struct NodoEventStruct TempEvent;
  ClearEvent(&TempEvent);
  TempEvent.DestinationUnit     = Event->SourceUnit;
  TempEvent.SourceUnit          = Settings.Unit;
  TempEvent.Command             = SYSTEM_COMMAND_CONFIRMED;      
  TempEvent.Par1                = count;
  TempEvent.Port                = Event->Port;
  TempEvent.Flags               = TRANSMISSION_SYSTEM; 

  SendEvent(&TempEvent,false, false, false);
  Transmission_NodoOnly=false;
  }
