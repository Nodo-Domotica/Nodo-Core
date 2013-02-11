
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

//??? Rawsignal later uitwerken.  CheckRawSignalKey(&Event32); // check of er een RawSignal key op de SDCard aanwezig is en vul met Nodo Event. Call by reference!
//  // Als de RAW pulsen worden opgevraagd door de gebruiker...
//  if(RawSignal.Key!=-1 && Event32)
//    {
//    if(SaveRawSignal(RawSignal.Key))
//      {
//      PrintEvent_OLD(Event32,Event->Direction,Event->Port);  // geef event weer op Terminal
//      PrintTerminal(ProgmemString(Text_13));
//      }
//    RawSignal.Key=-1;
//    return true;
//    }

  #endif

  // In het transmissie deel van event kan zijn aangegeven dat dit event er één is uit een reeks events direct achter elkaar worden
  // verzonden. Is dit het geval, dan event(s) niet direct verwerken maar in de queue plaatsen zodat er geen events worden gemist.
  // In andere gevallen direct verwerken.
  if(Event->Flags & TRANSMISSION_QUEUE) // Is dit event de eerste uit een reeks?
    {
    QueueAdd(Event);
    Transmission_NodoOnly=true;            // Zorg er voor dat de Nodo tijdelijk uitsluitend Nodo events kan ontvangen.
    
    // Als er nog meer events worden verwacht, sla dan alle voorbijkomende events op in de queue. Breek af zodra TRANSMISSION_SEQUENCE vlag niet meer aanwezig of timeout.
    // Als er een timeout was, dan de queue weer leeg maken omdat we data niet kunnen vertrouwen. In dit geval zal de confirm nul teruggeven en dus aan de master
    // zijde een foutmelding opleveren..
    if(Event->Flags&TRANSMISSION_NEXT)
      if(!Wait(10, false,0 , true))
        QueuePosition=0;
    
    Transmission_NodoOnly=false;
    // De queue is nu gevuld met de events van de master Nodo. Sla aantal events in de queue op t.b.v. verzenden van bevestiging naar de master.
    // Op basis van deze terugkoppeling weet de master of er geen events verloren zijn gegaan of extra opgetreden.
    RequestForConfirm=QueuePosition;    
    }
  else
    {
    // Verwerk het binnengekomen event
    error=ProcessEvent2(Event);
    }
    
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

    SendEvent(&TempEvent, false,false);
    RequestForConfirm=0;
    }
  return error;
  }
  
byte ProcessEvent2(struct NodoEventStruct *Event)
  {
  int x,y;
  byte error=0;
//  PrintNodoEventStruct("ProcessEvent();", Event);//???

  ExecutionDepth++;
  
  if(Event->Flags & TRANSMISSION_SYSTEM)// als het een systeem event is
    {
    ;
    // PrintNodoEventStruct("System commando!", Event);//???
    }  

  if(Settings.Lock) //??? verkassen naar ProcessEvent1 of scanevent???
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
        case CMD_PROTOCOL_1:
        case CMD_PROTOCOL_2:
          break;
          
        default:
          error=MESSAGE_09;    
        }
      }
    }

//  #ifdef USER_PLUGIN
//  if(!UserPlugin_Receive(Event32))
//    return true;
//  #endif
// ??? UserPlugin Entry herstellen
  if(error==0)
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
            EventlistAction.Port        = VALUE_SOURCE_EVENTLIST;
            
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
//      PrintNodoEventStruct("CheckEventlist();",&MacroEvent);//???
      if(CheckEvent(Event,&MacroEvent))
        {
//Serial.print(F("*** debug: GEVONDEN: Event.Par2="));Serial.print(Event->Par2,HEX); //??? Debug
//Serial.print(F(", Action.Par2="));Serial.println(MacroEvent.Par2,HEX); //??? Debug

        return x; // match gevonden
        }
      }
    }
  return false;
  }


 /**********************************************************************************************\
 * Vergelijkt twee events op matching voor uitvoering
 \*********************************************************************************************/
boolean CheckEvent(struct NodoEventStruct *Event, struct NodoEventStruct *MacroEvent)
  {  
  // geen lege events zoeken en verwerken
  if(MacroEvent->Command==0 || Event->Command==0)
    return false;  
    
  // ### WILDCARD:      
  if(MacroEvent->Command == CMD_COMMAND_WILDCARD) // is regel uit de eventlist een WildCard?
    {
    if( MacroEvent->Par1!=VALUE_ALL        &&  MacroEvent->Par1!=Event->Port)                return false;
    if((MacroEvent->Par2)&0xff!=VALUE_ALL  && (MacroEvent->Par2)&0xff!=Event->Command)       return false;
    if((MacroEvent->Par2>>8)&0xff!=0       && (MacroEvent->Par2>>8)&0xff!=Event->SourceUnit) return false;
    return true;
    }

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
//Serial.print(F("*** debug: CheckEvent(); Event="));Serial.print(Event->Par2,HEX); //??? Debug
//Serial.print(F(", MacroEvent"));Serial.println(MacroEvent->Par2,HEX); //??? Debug

  if(Event->Command==CMD_TIME) // het binnengekomen event is een clock event.//???
    {
    // Structuur technisch hoort onderstaande regel hier thuis, maar que performance niet wenselijk!
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

#ifdef NODO_MEGA  
/*********************************************************************************************\
 * Deze routine verzend de inhoud van de queue naar een andere Nodo.
 \*********************************************************************************************/
byte QueueSend(byte DestUnit)
  {
  byte x,Port,ReturnCode;

  // De port waar de SendTo naar toe moet halen we uit de lijst met Nodo's die wordt onderhouden door de funktie  NodoOnline();
  Port=NodoOnline(DestUnit,0);

  // indien te snel opvolgend, dan korte wachttijd.
  if(Port!=0)
    {
    // Zet WaitFreeRF aan en de Transmit poorten aan. Bewaar originele instelligen van de gebruiker.
    byte OrgWaitFreeRF  =Settings.WaitFreeRF;
  
    // Initialiseer een Event en Transmissie
    struct NodoEventStruct Event;
    ClearEvent(&Event);
  
    // Geef aan dat er een reeks wordt verzonden, dat er na verzending nog een event volgt en reserveer de band exclusief voor deze Nodo.
    Event.Flags = TRANSMISSION_QUEUE | TRANSMISSION_NEXT | TRANSMISSION_LOCK; 
    
    // Verzend alle events uit de queue. Alleen de bestemmings Nodo zal deze events in de queue plaatsen
    for(x=0;x<QueuePosition;x++)
      {    
      // laatste verzonden event markeren als laatste in de sequence.
      if(x==QueuePosition-1)
        Event.Flags = TRANSMISSION_QUEUE | TRANSMISSION_CONFIRM; 
      
      Event.DestinationUnit     = DestUnit;
      Event.SourceUnit          = Settings.Unit;
      Event.Port                = Port;
      Event.Command             = Queue[x].Command;
      Event.Par1                = Queue[x].Par1;
      Event.Par2                = Queue[x].Par2;
  
      SendEvent(&Event, false, true);//??? is niet weergeven netter?
      }

    // Inhoud van de queue is verzonden. Maak de queue nu weer leeg zodat tijdens het achten op de 
    // bevestiging van de slave Nodo weer events in de queue geplaatst kunnen worden.
    // bewaar huidig aantal posities in de queue om te controleren of de salev Nodo alle events heeft opgepikt.
    x=QueuePosition;           
    QueuePosition=0;           
  
    // Zet originele instelligen van de gebruiker terug.
    Settings.WaitFreeRF  = OrgWaitFreeRF;
  
    // De ontvangende Nodo verzendt als het goed is een bevestiging dat het is ontvangen en het aantal commando's
    // wacht hierop, maar stop de voorbijkomende events in de queue.
    ClearEvent(&Event);
    Event.SourceUnit          = DestUnit;
    Event.Command             = SYSTEM_COMMAND_CONFIRMED;
    if(Wait(10,false,&Event,false))
      {
      if(x==Event.Par1)
        ReturnCode=0;
      else
        ReturnCode=MESSAGE_13;
      }
    else
      ReturnCode=MESSAGE_11;      
    }
  else
    ReturnCode=MESSAGE_12;
    
  if(ReturnCode)
    QueuePosition=0;

  return ReturnCode;
  }
#endif

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
byte QueueProcess(void)
  {
  byte x;
  byte Processed=0;

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
          {
          UndoNewNodo();// Status NewNodo verwijderen indien van toepassing
          x+=2;
          Processed+=2;
          }
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
        
        if(ProcessEvent2(&Event)==0);      // verwerk binnengekomen event.
          Processed++;
        }
      }
    QueuePosition=0;
    }

  return Processed;
  }

