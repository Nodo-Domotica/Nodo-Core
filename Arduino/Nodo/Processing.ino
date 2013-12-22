
 /**********************************************************************************************\
 * Voert alle relevante acties in de eventlist uit die horen bij het binnengekomen event
 * Doorlopen van een volledig gevulde eventlist duurt ongeveer 15ms inclusief printen naar serial
 * maar exclusief verwerking n.a.v. een 'hit' in de eventlist
 \*********************************************************************************************/
byte ProcessEventExt(struct NodoEventStruct *Event)
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
  error=ProcessEvent(Event);
    
  // Verwerk eventuele events die in de queue zijn geplaatst.
  QueueProcess();    

  // Een event kan een verzoek bevatten om bevestiging. Doe dit dan pas na alle verwerking.
  if(RequestForConfirm)
    {  
    ClearEvent(&TempEvent);    
    TempEvent.DestinationUnit       = Event->SourceUnit;
    TempEvent.Port                  = Event->Port;
    TempEvent.Type                  = NODO_TYPE_SYSTEM;    // Event is niet voor de gebruiker bedoeld
    TempEvent.Command               = SYSTEM_COMMAND_CONFIRMED;
    TempEvent.Par1                  = RequestForConfirm;
    SendEvent(&TempEvent, false,false,false);
    RequestForConfirm=0;
    }  

  return error;
  }
    
byte ProcessEvent(struct NodoEventStruct *Event)
  {
  int x;
  byte error=0;
  boolean Continue=true;

  // PrintNodoEvent("ProcessEvent() ",Event);//$$$

    
  if(Event->Command==0)
    return error;

  if(++ExecutionDepth>=MACRO_EXECUTION_DEPTH)
    {
    QueuePosition=0;
    Continue=false;
    error=MESSAGE_NESTING_ERROR; // bij geneste loops ervoor zorgen dat er niet meer dan MACRO_EXECUTION_DEPTH niveaus diep macro's uitgevoerd worden
    }

  // loop de plugins langs voor eventuele afhandeling van dit event.
  PluginCall(PLUGIN_EVENT_IN, Event,0);

  // Als er een LOCK actief is, dan commando's blokkeren behalve...
  if(Settings.Lock && (Event->Port==VALUE_SOURCE_RF || Event->Port==VALUE_SOURCE_IR ))
    {
    switch(Event->Command)                                                      // onderstaande mogen WEL worden doorgelaten als de lock aan staat
      {
      case CMD_LOCK:                                                            // om weer te kunnen unlocken
      case SYSTEM_COMMAND_QUEUE_SENDTO:                                         // om weer te kunnen unlocken
      case EVENT_VARIABLE:                                                      // Maakt ontvangst van variabelen mogelijk
      case EVENT_USEREVENT:                                                     // Noodzakelijk voor uitwisseling userevents tussen Nodo.
      case EVENT_MESSAGE:                                                       // Voorkomt dat een message van een andere Nodo een error genereert
      case EVENT_BOOT:                                                          // Voorkomt dat een boot van een andere Nodo een error genereert
        break;

      default:
        Continue=false;
        RaiseMessage(MESSAGE_ACCESS_DENIED,0);
      }
    }
  
  // Events die de QUEUE vlag hebben staan hoorden oorspronkelijk tot een reeks die verzonden is
  // met QueueSend(). Op deze plaats in de code hebben deze events geen betekenis en dus volledig negeren.
  if(Continue && (Event->Flags & TRANSMISSION_QUEUE))
    {
    QueueReceive(Event);
    Continue=false;
    }

  if(Event->Type == NODO_TYPE_SYSTEM)
    {                       
    Continue=false;
    }


  #if NODO_MEGA  
  // Alleen weergeven zonder event af te handelen
  if(Continue && (Event->Flags & TRANSMISSION_VIEW_SPECIAL))
    {
    Continue=false;
    //??? waar wordt deze weergegeven?
    }
  #endif
  
  #if NODO_MEGA
  if(Continue && bitRead(HW_Config,HW_SDCARD))
    if(Event->Command==EVENT_RAWSIGNAL && Settings.RawSignalSave==VALUE_ON)
      if(!RawSignalExist(Event->Par2))
        RawSignalWrite(Event->Par2);
  #endif    
  
  if(Continue && error==0)
    {
    #if NODO_MEGA
    PrintEvent(Event, VALUE_ALL);
    #endif
    
    // ############# Verwerk event ################  
    if(Event->Type==NODO_TYPE_COMMAND)
      {
      error=ExecuteCommand(Event);
      }

    else if(Event->Type==NODO_TYPE_PLUGIN_COMMAND)
      {
      PluginCall(PLUGIN_COMMAND,Event,0);
      }
      
    else
      {
      // het is een ander soort event. Loop de gehele eventlist langs om te kijken of er een treffer is.   
      struct NodoEventStruct EventlistEvent, EventlistAction;   

      // sla event op voor later gebruik in SendEvent en VariableEvent.
      LastReceived=*Event;

      x=0;
      while(Eventlist_Read(++x,&EventlistEvent,&EventlistAction) && error==0) // Zolang er nog regels zijn in de eventlist...
        {      
        if(CheckEvent(Event,&EventlistEvent)) // Als er een match is tussen het binnengekomen event en de regel uit de eventlist.
          {        
          ExecutionLine=x;
          error=ProcessEvent(&EventlistAction);
          }
        }
      // abort is geen fatale error/break. Deze niet verder behandelen als een error.
      if(error==MESSAGE_BREAK)
        error=0;
      }      

    // Als de SendTo niet permanent is ingeschakeld, dan deze weer uitzetten
    #if NODO_MEGA
    if(!Transmission_SendToAll)Transmission_SendToUnit=0;
    #endif
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
  //PrintNodoEvent("CheckEvent(), Event-1",Event);//$$$
  //PrintNodoEvent("CheckEvent(), Event-2",MacroEvent);//$$$

  // geen lege events verwerken
  if(MacroEvent->Command==0 || Event->Command==0)
    return false;  
    
  // ### WILDCARD:      
  if(MacroEvent->Command == EVENT_WILDCARD)                                                                                 // is regel uit de eventlist een WildCard?
    if( MacroEvent->Par1==VALUE_ALL          ||   MacroEvent->Par1==Event->Port)                                            // Correspondeert de poort of mogen alle poorten?
      if((MacroEvent->Par2&0xff)==VALUE_ALL  ||  (MacroEvent->Par2&0xff)==Event->Command && Event->Type==NODO_TYPE_EVENT)   // Correspondeert het commando deel
        if(((MacroEvent->Par2>>8)&0xff)==0   || ((MacroEvent->Par2>>8)&0xff)==Event->DestinationUnit)                            // Correspondeert het unitnummer of is deze niet opgegeven
          return true;          

  // ### USEREVENT: beschouw bij een UserEvent een 0 voor Par1 of Par2 als een wildcard.
  if(Event->Type==NODO_TYPE_EVENT)
    if(Event->Command==EVENT_USEREVENT && MacroEvent->Command==EVENT_USEREVENT)                             // Is het een UserCommand?
      if( (Event->Par1==MacroEvent->Par1 || MacroEvent->Par1==0 || Event->Par1==0)                          // Par1 deel een match?
       && (Event->Par2==MacroEvent->Par2 || MacroEvent->Par2==0 || Event->Par2==0))                         // Par2 deel een match?
         return true; 
    
  // Herkomst van een andere Nodo, dan er niets meer mee doen TENZIJ het een UserEvent is of behandeling door Wildcard. Die werden hierboven 
  // al afgevangen.
  if(Event->DestinationUnit!=0  && Event->DestinationUnit!=Settings.Unit)
    return false;


  // #### EXACT: als huidige event exact overeenkomt met het event in de regel uit de Eventlist, dan een match. 
  if(MacroEvent->Command == Event->Command &&
     MacroEvent->Type    == Event->Type    &&
     MacroEvent->Par1    == Event->Par1    &&
     MacroEvent->Par2    == Event->Par2    )
       return true; 

  // ### TIME:
  if(Event->Type==NODO_TYPE_EVENT && Event->Command==EVENT_TIME)                // het binnengekomen event is een clock event.
    {
    // Structuur technisch hoort onderstaande regel hier thuis, maar qua performance niet optimaal!
    unsigned long Cmp=MacroEvent->Par2;
    unsigned long Inp=Event->Par2;

    // In het event in de eventlist kunnen zich wildcards bevinden. Maskeer de posities met 0xF wildcard nibble 
    // doe dit kruislings omdat zowel het invoer event als het event in de eventlist wildcards kunnen bevatten.
    for(byte y=0;y<8;y++)// loop de acht nibbles van de 32-bit Par2 langs
      {          
      unsigned long Mask=0xffffffff  ^ (0xFUL <<(y*4));                         // Masker maken om de nibble positie y te wissen.
      if(((Inp>>(y*4))&0xf) == 0xf)                                             // als in nibble y een wildcard waarde 0xf staat
        {
        Cmp&=Mask;                                                              // Maak nibble leeg
        Cmp|=(0xFUL<<(y*4));                                                    // vul met wildcard waarde 0xf
        }                                                 
      if(((Cmp>>(y*4))&0xf) == 0xf)                                             // als in nibble y een wildcard waarde 0xf staat
        {
        Inp&=Mask;                                                              // Maak nibble leeg
        Inp|=(0xFUL<<(y*4));                                                    // vul met wildcard waarde 0xf
        }
      }
            
     if(Inp==Cmp)                                                               // Als ingestelde alarmtijd overeen komt met huidige tijd.
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
    Event->Flags&=~TRANSMISSION_QUEUE; //  Haal eventuele QUEUE vlag er af. Anders loopt de queue vol door recursiviteit;
    
    Queue[QueuePosition].Flags   = Event->Flags; 
    Queue[QueuePosition].Port    = Event->Port;
    Queue[QueuePosition].Unit    = Event->SourceUnit;
    Queue[QueuePosition].Type    = Event->Type;
    Queue[QueuePosition].Command = Event->Command;
    Queue[QueuePosition].Par1    = Event->Par1;
    Queue[QueuePosition].Par2    = Event->Par2;
    QueuePosition++;           

    // Een EventlistShow staat welliswaar in de queue, maar die kunnen we als het de informatie compleet is gelijk weergeven
    // en vervolgens weer uit de queue halen. Deze voorziening is er alleen voor de Mega omdag een Small geen MMI heeft.
    #if NODO_MEGA
    char *TempString=(char*)malloc(INPUT_COMMAND_SIZE);
    char *TempString2=(char*)malloc(INPUT_LINE_SIZE);
    
    if(Event->Type!=NODO_TYPE_SYSTEM && Event->Flags&TRANSMISSION_VIEW)
      {
      Event2str(Event, TempString);  
      if(Settings.Alias==VALUE_ON)
        Alias(TempString,false);
      PrintString(TempString,VALUE_ALL);//??? nog alleen naar de poort van herkomst laten weergeven
      QueuePosition--;
      }

    else if(QueuePosition>=3)
      {
      if(Queue[QueuePosition-3].Type==NODO_TYPE_SYSTEM && Queue[QueuePosition-3].Command==SYSTEM_COMMAND_QUEUE_EVENTLIST_SHOW)
        {
        struct NodoEventStruct E;
        struct NodoEventStruct A;
        ClearEvent(&E);
        ClearEvent(&A);
        
        strcpy(TempString2,int2str(Queue[0].Par1)); 
        strcat(TempString2,": ");

        // geef het event weer
        E.Type=Queue[QueuePosition-2].Type;
        E.Command=Queue[QueuePosition-2].Command;
        E.Par1=Queue[QueuePosition-2].Par1;
        E.Par2=Queue[QueuePosition-2].Par2;
        Event2str(&E, TempString);
        if(Settings.Alias==VALUE_ON)
          Alias(TempString,false);
        strcat(TempString2, TempString);
    
        // geef het action weer
        A.Type=Queue[QueuePosition-1].Type;
        A.Command=Queue[QueuePosition-1].Command;
        A.Par1=Queue[QueuePosition-1].Par1;
        A.Par2=Queue[QueuePosition-1].Par2;
        strcat(TempString2,"; ");
        Event2str(&A, TempString);  
        if(Settings.Alias==VALUE_ON)
          Alias(TempString,false);
        strcat(TempString2,TempString);
        
        PrintString(TempString2,VALUE_ALL);//??? nog alleen naar de poort van herkomst laten weergeven

        QueuePosition-=3;
        }
      }    
    free(TempString2);
    free(TempString);
    #endif
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
  struct NodoEventStruct E;
  struct NodoEventStruct A;

  // De queue moet worden verwerkt. Er kunnen zich twee mogelijkheden voor doen:
  // A: de queue bevat een willekeurige reeks events/commando's of,
  // B: de queue bevat een reeks events/commando's die een speciale behandeling nodig hebben zoals SendTo of EventlistShow. In dit 
  //   geval bevat het eerste element een systeemcommando met bijzondere gegevens die benodigd zijn voor verdere verwerking.

  if(QueuePosition)
    {
    if(Queue[0].Type==NODO_TYPE_SYSTEM)
      {
      // Als er in de queue een EventlistWrite commando zit, dan moeten de twee opvolgende posities uit de queue
      // worden weggeschreven naar de eventlist. De rest uit de queue negeren we omdat een EventlistWrite altijd 
      // in een nieuwe verzending zit. Op de eerste positie (=0) zit het systeem commando QUEUE_SEND, die moeten
      // we negeren. Op de tweede zit het systeemcommando EventlistWrite, daarna het Event en de Actie.    
  
      if(Queue[1].Command==SYSTEM_COMMAND_QUEUE_EVENTLIST_WRITE && QueuePosition==4) // cmd
        {
        E.Type=Queue[2].Type;
        E.Command=Queue[2].Command;
        E.Par1=Queue[2].Par1;
        E.Par2=Queue[2].Par2;
        A.Type=Queue[3].Type;
        A.Command=Queue[3].Command;
        A.Par1=Queue[3].Par1;
        A.Par2=Queue[3].Par2;
       
        if(Eventlist_Write(Queue[1].Par1, &E, &A))
          UndoNewNodo();
        QueuePosition=0;
        }
      }

    if(QueuePosition)
      {
      for(x=0;x<QueuePosition;x++)
        {      
        // Serial.print("QueueProcess(); Uitvoer event uit queue nummer=");Serial.println(x);//$$$
        Event.SourceUnit=Queue[x].Unit;
        Event.Type=Queue[x].Type;
        Event.Command=Queue[x].Command;
        Event.Par1=Queue[x].Par1;
        Event.Par2=Queue[x].Par2;
        Event.Direction=VALUE_DIRECTION_INPUT;
        Event.Port=Queue[x].Port;
        Event.Flags=Queue[x].Flags;
        // PrintNodoEvent("ProcessQueue();",&Event);//$$$
        ProcessEvent(&Event);      // verwerk binnengekomen event.
        }
      }
    }
    
  // Aan het einde van het verwerken van de Queue deze weer leeg maken.
  QueuePosition=0;
  }

#if NODO_MEGA  
/*********************************************************************************************\
 * Deze routine verzendt de inhoud van de queue naar een andere Nodo.
 \*********************************************************************************************/
byte QueueSend(boolean fast)
  {
  byte x,Port,error=MESSAGE_SENDTO_ERROR, Retry=0;
  unsigned long ID=millis();
  struct NodoEventStruct Event;

  // We maken tijdelijk gebruik van een SendQueue zodat de reguliere queue zijn werk kan blijven doen.
  struct QueueStruct SendQueue[EVENT_QUEUE_MAX];
  for(x=0;x<QueuePosition;x++)
    {
    // Kopieer de inhoud van de queue.
    SendQueue[x]=Queue[x];

    // Er zijn enkele commando's die geen reacte teruggeven. Zet in deze gevallen de fast mode aan.
    if(Queue[x].Type==NODO_TYPE_COMMAND)
      {
      switch(Queue[x].Command)
        {
        case CMD_RESET:
        case CMD_REBOOT:
          fast=true;
        }
      }
    }

  byte SendQueuePosition=QueuePosition;
  QueuePosition=0;
  // Serial.print("QueueSend() Te verzenden aantal events=");Serial.println(SendQueuePosition);//$$$  
  
  // De port waar de SendTo naar toe moet halen we uit de lijst met Nodo's die wordt onderhouden door de funktie NodoOnline();
  // als de Nodo nog niet bekend is, dan nemen we aan dat deze via RF benaderbaar is.
  Port=NodoOnline(Transmission_SendToUnit,0);
  if(Port==0)
    Port=VALUE_SOURCE_RF;

  // Eerste fase: Zorg dat de inhoud van de queue correct aan komt op de slave. Activeer aan slave zijde de QueueReceive()
  // Verzend in deze activering tevens het aantal events datdat vanuit de queue verzonden zal worden naar de Slave. Eveneens wordt
  // er een ID verzonden. Deze unieke waarde zorgt er voor dat bij een eventuele re-send de reeks niet nogmaals aankomt op de 
  // slave.
  do
    {
    ClearEvent(&Event);
    Event.DestinationUnit     = Transmission_SendToUnit;
    Event.SourceUnit          = Settings.Unit;
    Event.Port                = Port;
    Event.Type                = NODO_TYPE_SYSTEM;      
    Event.Command             = SYSTEM_COMMAND_QUEUE_SENDTO;      
    Event.Par1                = SendQueuePosition+1;   // Aantal te verzenden events in de queue. Wordt later gecheckt en teruggezonden in de confirm. +1 omdat DIT event ook in de queue komt.
    Event.Par2                = ID;
    Event.Flags               = TRANSMISSION_LOCK | TRANSMISSION_QUEUE | TRANSMISSION_QUEUE_WAIT;
    SendEvent(&Event,false,false,Settings.WaitFree==VALUE_ON);         // Alleen de eerste vooraf laten gaan door een WaitFree (indien setting zo staat ingesteld);

  
    // Verzend alle events uit de queue. Alleen de bestemmings Nodo zal deze events in de queue plaatsen
    for(x=0;x<SendQueuePosition;x++)
      {
      ClearEvent(&Event);
      Event.DestinationUnit     = Transmission_SendToUnit;
      Event.SourceUnit          = Settings.Unit;
      Event.Port                = Port;
      Event.Type                = SendQueue[x].Type;
      Event.Command             = SendQueue[x].Command;
      Event.Par1                = SendQueue[x].Par1;
      Event.Par2                = SendQueue[x].Par2;

      // Verzendvlaggen geven aan dat er nog events verzonden gaan worden en dat de ether tijdelijk gereserveerd is
      // voor deze gegevensuitwisseling. 
      if(x==(SendQueuePosition-1))
        Event.Flags = TRANSMISSION_QUEUE | TRANSMISSION_LOCK;
      else
        Event.Flags = TRANSMISSION_QUEUE | TRANSMISSION_QUEUE_NEXT | TRANSMISSION_LOCK;

      // In geval van verzending naar queue zal deze tijd niet van toepassing zijn omdat er dan geen verwerkingstijd nodig is.
      // Tussen de events die de queue in gaan een kortere delaytussen verzendingen.
      HoldTransmission=DELAY_BETWEEN_TRANSMISSIONS_Q+millis();
        
      SendEvent(&Event,false,false,false);
      }
    
    if(fast)
      {
      error=0;
      }
    else
      {
      // De ontvangende Nodo verzendt als het goed is een bevestiging dat het is ontvangen en het aantal commando's. De slave haalt tevens de Lock
      // van de Nodo's af zodat de ether weer door alle Nodo's gebruikt kunnen worden.
      ClearEvent(&Event);
      Event.SourceUnit          = Transmission_SendToUnit;
      Event.Command             = SYSTEM_COMMAND_CONFIRMED;
      Event.Type                = NODO_TYPE_SYSTEM;
  
      if(Wait(10,false,&Event,false))
        if(x==(Event.Par1-1))// Verzonden events gelijk aan ontvangen events? -1 omdat aan de Slave zijde het eerste element in de queue geen deel uit maakt van de SendTo events.
          error=0;
  
      // Als er een timeout was of het aantal events is niet correct bevestigd, dan de gebruiker een waarschuwing tonen
      if(error)
        delay(1000); // kleine pauze om zeker te weten dat de ontvanger van de ontvangende Nodo weer gereed staat voor ontvangst (starttijd RF module)
      }
    }while((++Retry<5) && error);    
  return error;
  }
#endif


void QueueReceive(NodoEventStruct *Event)
  {
  static unsigned long PreviousID=0L;
  struct NodoEventStruct TempEvent;
  byte x, count;

  // Onderstaande vlag zorgt er voor dat de Nodo uitsluitend Nodo events
  // oppikt. Andere signalen worden tijdelijk volledig genegeerd.
  Transmission_NodoOnly=true;
  
  // Alle Nodo events die nu binnen komen op slaan in de queue. We beginnen met een schone queue.
  QueuePosition=0;
  QueueAdd(Event);
  
  
  Wait(5, false, 0, true);// ??? niet uitvoeren als de NEXT flag niet staat.
  count=QueuePosition;
  // Serial.print("QueueReceive() Ontvangen en in queue geplaatst=");Serial.println(count);//$$$  


  // Haal de QUEUE vlag van de events af.
  for(x=0;x<QueuePosition;x++)
    {
    bitClear(Queue[x].Flags,TRANSMISSION_QUEUE);// Haal QUEUE vlag er af, anders worden de events niet verwerkt n.a.v. onderdrukken incomplete SendTo reeksen.
    Queue[x].Flags=0;
    }


  // Er kunnen zich twee situaties voordoen:
  // A: Er bevinden zich een willekeurige reeks events in de queue of,
  // B: Er bevindt zich een reeks in de queue die deel uit maakt van een queue-opdracht zoals SendTo of EventlistShow. In dit geval bevat het eerste
  //    event in de queue gegevens over de inhoud van de queue die gebruikt moeten worden.
  ClearEvent(&TempEvent);
  
  if(Event->Type==NODO_TYPE_SYSTEM)
    {
    if(Event->Command==SYSTEM_COMMAND_QUEUE_SENDTO)
      {
      // Serial.println("QueueReceive() Ontvangen SendTo...");//$$$
      // Als aantal regels in de queue overeenkomt met wat in het SendTo verzoek is vermeldt
      // EN de queue is nog niet eerder binnen gekomen (ID) dan uitvoeren.
      // Stuur vervolgens de master ter bevestiging het aantal ontvangen events die zich nu in de queue bevinden.
      if(PreviousID!=Event->Par2)
        {
        if(QueuePosition==Event->Par1)
          {
          PreviousID=Event->Par2;
          // Queue gelijk verwerken alvorens een bevestiging te versturen zorgt er voor dat de Master
          // pas doorgaan met verwerking als deze Slave gereed is.
          if(Event->Flags & TRANSMISSION_QUEUE_WAIT)
            QueueProcess();
          }
        }
      TempEvent.DestinationUnit     = Event->SourceUnit;
      TempEvent.SourceUnit          = Settings.Unit;
      TempEvent.Port                = Event->Port;
      TempEvent.Type                = NODO_TYPE_SYSTEM; 
      TempEvent.Command             = SYSTEM_COMMAND_CONFIRMED;      
      TempEvent.Par1                = count;
      SendEvent(&TempEvent,false, false, false);
      }
    else if(Event->Command==SYSTEM_COMMAND_QUEUE_EVENTLIST_SHOW)
      {
      QueueProcess();
      }
    }
  else
    {
    QueueProcess();
    }    
    
  // Omdat ProcessQueue de queue heeft leeggedraaid, kan de queue positie op 0 worden gezet.
  QueuePosition=0;
  Transmission_NodoOnly=false;
  }
