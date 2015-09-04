//#######################################################################################################
//#####################################     Event processing      #######################################
//#######################################################################################################

#ifndef NODO_DEBUG_QUEUE
#define NODO_DEBUG_QUEUE                    false
#endif

byte ProcessEvent(struct NodoEventStruct *Event)
  {
  struct NodoEventStruct TempEvent;
  byte error=0;
  boolean Continue=true;
  int x;

  if(Event->Command==0)
    return error;
  
  if(Event->Type == NODO_TYPE_SYSTEM)
    {                       
    switch(Event->Command)
      {
      // Enkele systeem-commandos hoeven geheel niet te worden afgehandeld
      case SYSTEM_COMMAND_BUSY:
      case SYSTEM_COMMAND_CONFIRMED:
        return error;
        break;

      case SYSTEM_COMMAND_QUEUE_SENDTO:
        ProcessingStatus(true);
        QueueReceive(Event);
        break;

      #if NODO_MEGA
      case SYSTEM_COMMAND_QUEUE_EVENTLIST_SHOW:
        ProcessingStatus(true);
        Wait(5, false,0 , false);                                               // in de wait() routine worden de regels weergegeven  
        break;
      #endif
      }
    Continue=false;
    }

  if(Settings.WaitBusyNodo==VALUE_ON)
    NodoBusyWait(true);
  
  ProcessingStatus(true);

  #if HARDWARE_SDCARD
  if(FileWriteMode!=0)
    return 0;
  #endif

  if(++ExecutionDepth>=MACRO_EXECUTION_DEPTH)
    {
    QueuePosition=0;
    Continue=false;
    error=MESSAGE_NESTING_ERROR;                                                // bij geneste loops ervoor zorgen dat er niet meer dan MACRO_EXECUTION_DEPTH niveaus diep macro's uitgevoerd worden
    }


  // Events die nu nog de TRANSMISSION_SENDTO vlag hebben staan, hoorden oorspronkelijk tot een reeks die verzonden is
  // met een SendTo. Deze events hebben hier geen betekenis en dus volledig negeren.
  if(Continue && (Event->Flags & TRANSMISSION_SENDTO))
    Continue=false;

  // Als in het event een verzoek zit om een bevestiging te verzenden...
  if(Event->Flags & TRANSMISSION_CONFIRM)
    RequestForConfirm=true;

  if(Continue && (Event->Flags & TRANSMISSION_QUEUE))
    {
    QueueAdd(Event);
    if(Event->Flags & TRANSMISSION_QUEUE_NEXT)
      Wait(5, true,0,false);  
    Continue=false;
    }

  #if NODO_MEGA  
  // Alleen weergeven zonder event af te handelen
  if(Continue && (Event->Flags & TRANSMISSION_VIEW))
    Continue=false;
  #endif

  PluginCall(PLUGIN_EVENT_IN, Event,0);                                         // loop de plugins langs voor eventuele afhandeling van dit event.

  
  #if (HARDWARE_RAWSIGNAL || HARDWARE_RF433 || HARDWARE_INFRARED) && HARDWARE_SDCARD
  if(Continue && HW_Status(HW_SDCARD))
    if(Event->Command==EVENT_RAWSIGNAL && Settings.RawSignalReceive==VALUE_ON)
      if(!RawSignalExist(Event->Par2))
        RawSignalWrite(Event->Par2);
  #endif    

  if(Event->Command==EVENT_VARIABLE && Event->Type==NODO_TYPE_EVENT && Event->SourceUnit!=Settings.Unit)            
    {                                                                           // Als er een Variabele event is binnengekomen
    x=UserVariable(Event->Par1,0);
    if(x!=-1)
      if(UserVarGlobal[x])                                                      // en op deze Nodo is het een globale variabele
        UserVariableSet(Event->Par1,ul2float(Event->Par2),false);               // Dan de variabele actualiseren
    }
    
  if(Continue && error==0)
    {
    #if NODO_MEGA
     PrintEvent(Event, VALUE_ALL);
    
    // Enkele commando's moet de WebApp terug ontvangen omdat anders statussen niet correct zijn.
    // Dit is een tijdelijke oplossing.  (???)
    x=false;
    if(Settings.TransmitHTTP==VALUE_ON && Event->Type==NODO_TYPE_COMMAND)
      {
      switch(Event->Command)
        {
        case CMD_ALARM_SET:
        case CMD_WIRED_OUT:
          x=true;
        }
      }

    if(Settings.TransmitHTTP==VALUE_ALL || x)
      {
      TempEvent=*Event;
      TempEvent.Port=VALUE_SOURCE_HTTP;
      SendEvent(&TempEvent,false,true);
      }
    #endif
    
    // ############# Verwerk event ################  

    if(Event->Type==NODO_TYPE_COMMAND)
      error=ExecuteCommand(Event);

    else if(Event->Type==NODO_TYPE_PLUGIN_COMMAND)
      PluginCall(PLUGIN_COMMAND,Event,0);
      
    else
      {
      struct NodoEventStruct EventlistEvent, EventlistAction;                   // het is een ander soort event. Loop de gehele eventlist langs om te kijken of er een treffer is.   
      LastReceived=*Event;                                                      // sla event op voor later gebruik (o.a. SendEvent)

      x=0;

      while(Eventlist_Read(++x,&EventlistEvent,&EventlistAction) && error==0)   // Zolang er nog regels zijn in de eventlist...
        {      
        if(CheckEvent(Event,&EventlistEvent))                                   // Als er een match is tussen het binnengekomen event en de regel uit de eventlist.
          {        
          ExecutionLine=x;
          error=ProcessEvent(&EventlistAction);
          }
        }

      if(error==MESSAGE_BREAK)                                                  // abort is geen fatale error maar een break. Deze dus niet verder behandelen als een error.
        error=0;

      if(error)
        RaiseMessage(error,EventlistAction.Par2);
      }      

    #if NODO_MEGA
    if(!Transmission_SendToAll)Transmission_SendToUnit=0;                       // Als de SendTo niet permanent is ingeschakeld, dan deze weer uitzetten
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
        return x;                                                               // match gevonden

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

  // PrintNodoEvent("\nDEBUG: CheckEvent(), Input",Event);
  // PrintNodoEvent("DEBUG: CheckEvent(), Eventlist",MacroEvent);
    
  // ### WILDCARD:      
  if(MacroEvent->Command == EVENT_WILDCARD)                                                                                 // is regel uit de eventlist een WildCard?
    if( MacroEvent->Par1==VALUE_ALL          ||   MacroEvent->Par1==Event->Port)                                            // Correspondeert de poort of mogen alle poorten?
      if((MacroEvent->Par2&0xff)==VALUE_ALL  ||  (MacroEvent->Par2&0xff)==Event->Command && (Event->Type==NODO_TYPE_EVENT || Event->Type==NODO_TYPE_RAWSIGNAL))   // Correspondeert het commando deel
        if(((MacroEvent->Par2>>8)&0xff)==0   || ((MacroEvent->Par2>>8)&0xff)==Event->SourceUnit)                       // Correspondeert het unitnummer of is deze niet opgegeven
          return true;          

  // ### USEREVENT: beschouw bij een UserEvent een 0 voor Par1 of Par2 als een wildcard.
  if(Event->Type==NODO_TYPE_EVENT)
    if(Event->Command==EVENT_USEREVENT && MacroEvent->Command==EVENT_USEREVENT)                             // Is het een UserCommand?
      if( (Event->Par1==MacroEvent->Par1 || MacroEvent->Par1==0 || Event->Par1==0)                          // Par1 deel een match?
       && (Event->Par2==MacroEvent->Par2 || MacroEvent->Par2==0 || Event->Par2==0))                         // Par2 deel een match?
         return true; 
    
  // Events die niet voor deze Nodo bestemd zijn worden, worden niet doorgelaten door EventScanning(), echter events die voor alle Nodo's bestemd zijn
  // horen NIET langs de eventlist te worden gehaald. Deze daarom niet verder behandelen TENZIJ het een UserEvent is of behandeling door Wildcard. 
  // Die werden hierboven al behandeld.
  if(Event->SourceUnit!=0  && Event->SourceUnit!=Settings.Unit)
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
    unsigned long Cmp=MacroEvent->Par2;                                         // Event zoals door gebruiker ingevoerd in de eventlist
    unsigned long Inp=Event->Par2;                                              // Event zoals gevuld door de klok

    // Twee opties moeten afwijkend worden behandeld: Werkdagen en weekend.
    byte y=(Cmp>>16)&0xf;                                                       // Te checken dag uit event opgegeven in de eventlist
    byte z=(Inp>>16)&0xf;                                                       // Te checken dag uit de kloktijd opgegeven dag
    if( (y==8 && z>=2 && z<=6)     ||                                           // als opgegeven dag in eventlist WRK en kloktijd is een werkdag,
        (y==9 && (z==1 || z==7))    )                                           // als opgegeven dag in eventlist WND en kloktijd is een weekend dag
        Cmp|=(0xfL<<16);                                                        // dan match op dag door maskeren met wildcard                                      

    // In het event in de eventlist kunnen zich wildcards bevinden. Maskeer de posities met 0xF wildcard nibble 
    // doe dit kruislings omdat zowel het invoer event als het event in de eventlist wildcards kunnen bevatten.
    for(y=0;y<8;y++)// loop de acht nibbles van de 32-bit Par2 langs
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
  return false;                                                                 // geen match gevonden
  }


//#######################################################################################################
//##################################### Processing: Queue ###############################################
//#######################################################################################################

// De queue is een tijdelijke wachtrij voor ontvangen of te verzenden events. 
// De queue bevat event gegevens behalve de transmissie gegevens. 
struct NodoEventStruct Queue[EVENT_QUEUE_MAX];


 /**********************************************************************************************\
 * Voeg event toe aan de queue.
 \*********************************************************************************************/
boolean QueueAdd(struct NodoEventStruct *Event)
  {
  if(QueuePosition<EVENT_QUEUE_MAX)
    {
    // PrintNodoEvent("DEBUG: QueueAdd()",Event);

    Event->Flags&=~TRANSMISSION_QUEUE;                                          //  Haal eventuele QUEUE vlag er af. Anders loopt de queue vol door recursiviteit;
    
    Queue[QueuePosition++]=*Event; 

    // Een EventlistShow staat welliswaar in de queue, maar die kunnen we als de informatie compleet is gelijk weergeven
    // en vervolgens weer uit de queue halen. Deze voorziening is er alleen voor de Mega omdag een Small geen MMI heeft.
    #if NODO_MEGA
    char *TempString=(char*)malloc(INPUT_COMMAND_SIZE);
    char *TempString2=(char*)malloc(INPUT_LINE_SIZE);
    
    if(Event->Flags & TRANSMISSION_VIEW)                                        // Als alleen weergeven, dan direct doen en niet in de queue plaatsen.
      {
      Event2str(Event, TempString);  

      #if HARDWARE_SDCARD
      if(Settings.Alias==VALUE_ON)
        Alias(TempString,false);
      #endif // HARDWARE_SDCARD

      PrintString(TempString,VALUE_ALL);
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
        
        strcpy(TempString2,int2str(Queue[QueuePosition-3].Par1)); 
        strcat(TempString2,": ");

        // geef het event weer
        E = Queue[QueuePosition-2];
        Event2str(&E, TempString);


        #if HARDWARE_SDCARD
        if(Settings.Alias==VALUE_ON)
          Alias(TempString,false);
        #endif // HARDWARE_SDCARD

        strcat(TempString2, TempString);

        // geef het action weer
        A = Queue[QueuePosition-1];
        strcat(TempString2,"; ");
        Event2str(&A, TempString);  

        #if HARDWARE_SDCARD
        if(Settings.Alias==VALUE_ON)
          Alias(TempString,false);
        #endif // HARDWARE_SDCARD

        strcat(TempString2,TempString);
        
        PrintString(TempString2,VALUE_ALL);

        QueuePosition=0;
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
  
      if(Queue[0].Command==SYSTEM_COMMAND_QUEUE_EVENTLIST_WRITE && QueuePosition==3) // cmd
        {
        E.Type        = Queue[1].Type;
        E.Command     = Queue[1].Command;
        E.Par1        = Queue[1].Par1;
        E.Par2        = Queue[1].Par2;
        A.Type        = Queue[2].Type;
        A.Command     = Queue[2].Command;
        A.Par1        = Queue[2].Par1;
        A.Par2        = Queue[2].Par2;
        Eventlist_Write(Queue[0].Par1, &E, &A);
        QueuePosition=0;
        }
      }

    if(QueuePosition)
      {
      for(x=0;x<QueuePosition;x++)
        {      
        Event=Queue[x];
        Event.Direction  = VALUE_DIRECTION_INPUT;
        ProcessEvent(&Event);                                                   // verwerk binnengekomen event.
        }
      }
    }
  QueuePosition=0;                                                              // Aan het einde van verwerken van de Queue deze weer leeg maken.
  }

#if NODO_MEGA  
/*********************************************************************************************\
 * Deze routine verzendt de inhoud van de queue naar een andere Nodo.
 \*********************************************************************************************/
byte QueueSend(boolean fast)
  {
  byte x,error=0, Retry=10,Success=false;
  unsigned long ID=millis();
  struct NodoEventStruct Event, SendQueue[EVENT_QUEUE_MAX];                     // We maken tijdelijk gebruik van een SendQueue zodat de reguliere queue zijn werk kan blijven doen.
  byte SendQueuePosition=QueuePosition;
  byte Port=NodoOnline(Transmission_SendToUnit,0,false);                        // Port waar SendTo naar toe moet halen we uit lijst met Nodo's onderhouden door NodoOnline();

  if(fast && Port==0)                                                           // Als we de Nodo niet kennen en fast mode
    Port=VALUE_ALL;                                                             // dan event naar alle poorten sturen.

  if(Port==0)
    return MESSAGE_NODO_NOT_FOUND;

  NodoBusyWait(true);                                                  // wacht zolang er nog Nodo's bezig zijn. Het SendTo proces mag niet worden verstoord.

  for(x=0;x<QueuePosition;x++)
    {
    SendQueue[x]=Queue[x];                                                      // Kopieer de inhoud van de queue omdat de queue wordt gebruikt door de Wait() funktie.
    
    if(Queue[x].Type==NODO_TYPE_COMMAND)                                        // Er zijn enkele commando's die geen reactie teruggeven. Zet in deze gevallen de fast mode aan.
      {
      switch(Queue[x].Command)
        {
        case CMD_UNIT_SET:
        case CMD_RESET:
        case CMD_REBOOT:
          fast=true;
        }
      }
    }
    
  QueuePosition=0;

  // Eerste fase: Zorg dat de inhoud van de queue correct aan komt op de slave. Activeer aan slave zijde de QueueReceive()
  // Verzend in deze activering tevens het aantal events dat vanuit de queue verzonden zal worden naar de Slave. Eveneens wordt
  // er een ID verzonden. Deze unieke waarde zorgt er voor dat bij een eventuele re-send de reeks niet nogmaals aankomt op de 
  // slave.

  do
    {
    if(!fast)
      {
      #if NODO_DEBUG_QUEUE
      #endif
      
      ClearEvent(&Event);
      Event.DestinationUnit     = Transmission_SendToUnit;
      Event.SourceUnit          = Settings.Unit;
      Event.Port                = Port;
      Event.Flags               = TRANSMISSION_BUSY;
      Event.Type                = NODO_TYPE_SYSTEM;      
      Event.Command             = SYSTEM_COMMAND_QUEUE_SENDTO;      
      Event.Par1                = SendQueuePosition;                            // Aantal te verzenden events in de queue. Wordt later gecheckt en teruggezonden in de confirm. +1 omdat DIT event ook in de queue komt.
      Event.Par2                = ID;
      SendEvent(&Event,false,false);

      #if NODO_DEBUG_QUEUE
      Trace("QueueSend(); Queu_SendTo commando verzonden.",0);//???
      #endif
      }

    for(x=0;x<SendQueuePosition;x++)                                            // Verzend alle events uit de queue. Alleen de bestemmings Nodo zal deze events in de queue plaatsen
      {
      #if NODO_DEBUG_QUEUE
      Trace("QueueSend(); Verzend event uit de queue:",x);//???
      #endif
      
      Event=SendQueue[x];
      Event.DestinationUnit     = Transmission_SendToUnit;
      Event.SourceUnit          = Settings.Unit;
      Event.Port                = Port;

      if(x==(SendQueuePosition-1))
        Event.Flags = TRANSMISSION_QUEUE;                                       // Verzendvlaggen geven aan dat er nog events verzonden gaan worden en dat de ether tijdelijk gereserveerd is
      else
        Event.Flags = TRANSMISSION_QUEUE | TRANSMISSION_QUEUE_NEXT | TRANSMISSION_BUSY;

      if(!fast)
        Event.Flags = Event.Flags | TRANSMISSION_SENDTO;
        
      SendEvent(&Event,false,false);
      }
      
    if(!fast)
      {
      // De ontvangende Nodo verzendt als het goed is direct een bevestiging (Met ID, status en aantal events ontvangen)

      #if NODO_DEBUG_QUEUE
      Trace("QueueSend(); Wacht op bevestiging ontvangst",0);//???
      #endif
      
      ClearEvent(&Event);
      Event.SourceUnit          = Transmission_SendToUnit;
      Event.Command             = SYSTEM_COMMAND_CONFIRMED;
      Event.Type                = NODO_TYPE_SYSTEM;
  
      if(Wait(3,false,&Event,false))
        {        
        if(x==Event.Par1 && ID==Event.Par2)                                     // Verzonden events gelijk aan ontvangen events en ID is correct? 
          {
          if(Event.Payload==1)                                                  // Status: 1=ontvangen en gereed voor verwerking, 0=gereed.
            {
            #if NODO_DEBUG_QUEUE
            Trace("QueueSend(); Verwerking start aan slave zijde.",0);//???
            #endif
          
            // We weten nu dat de slave Nodo de events goed heeft ontvangen en aan het verwerken is.
            // zodra de verwerking gereed is zal de slave Nodo wederom een bevestiging sturen die de 'busy' status
            // opheft. Zodra deze van de slave is ontvangen mogen we weer verder met verwerking aan de master zijde.
            NodoBusyWait(false); 
            }
          Success=true;
          }
        }
      }
    else
      Success=true;                                                             // Bij fast optie hoeven we niet te wachten op bevestiging van de slave
      
    }while((--Retry>0) && !Success);   

  if(Retry==0)
    error=MESSAGE_SENDTO_ERROR;
    
  return error;
  }
#endif


void QueueReceive(NodoEventStruct *Event)
  {
  static unsigned long PreviousID=0L;
  struct NodoEventStruct TempEvent;
  byte x, Received;

  Transmission_NodoOnly=true;                                                   // Uitsluitend Nodo events. Andere signalen worden tijdelijk volledig genegeerd.
  Wait(5, false, 0, true);                                                      // Last de Wait routine de queue vullen.
  Transmission_NodoOnly=false;
  Received=QueuePosition; 

  #if NODO_DEBUG_QUEUE
  Trace("QueueReceiveroutine Wait() verlaten.",0);//???
  #endif

  TempEvent.DestinationUnit     = Event->SourceUnit;
  TempEvent.SourceUnit          = Settings.Unit;
  TempEvent.Port                = Event->Port;
  TempEvent.Type                = NODO_TYPE_SYSTEM; 
  TempEvent.Command             = SYSTEM_COMMAND_CONFIRMED;      
  TempEvent.Flags               = TRANSMISSION_BUSY;                            // Geef aan dat deze Nodo bezig is.
  TempEvent.Par1                = Received;                                     // Aantal ontvangen events.
  TempEvent.Par2                = Event->Par2;                                  // =ID
 
  if(PreviousID!=Event->Par2 && Received==Event->Par1)                          // Als aantal regels in de queue overeenkomt en queue is nog niet eerder binnen gekomen (ID)
    {
    PreviousID=Event->Par2;

    for(x=0;x<QueuePosition;x++)                                                
      Queue[x].Flags=0;                                                         // Haal de QUEUE vlag van de events af.

    TempEvent.Payload= 1;                                                       // Status (1=Data in goede orde ontvangen maar nog niet verwerkt, 2=verwerkt)
    SendEvent(&TempEvent,false, false);                                         // Stuur vervolgens de master ter bevestiging het aantal ontvangen events.

    #if NODO_DEBUG_QUEUE
    Trace("QueueReceive(); leegdraaien: ",Event->Par2);//???
    #endif

    delay(DELAY_BETWEEN_TRANSMISSIONS);
    QueueProcess();

    #if NODO_DEBUG_QUEUE
    Trace("QueueReceive(); leegdraaien gereed: ",Event->Par2);//???
    #endif
    }

  TempEvent.Payload = 2;                                                        // Status (1=Data in goede orde ontvangen maar nog niet verwerkt, 2=verwerkt)
  TempEvent.Flags   = 0;                                                        // Geef alle Nodos aan dat de ether weer vrij is.
  SendEvent(&TempEvent,false, false);                                           // Stuur vervolgens de master ter bevestiging het aantal ontvangen events.

  #if NODO_DEBUG_QUEUE
  Trace("QueueReceive(); bevestiging leeggedraaid verzonden. ID=",Event->Par2); //???
  #endif

  QueuePosition=0;                                                              // Omdat ProcessQueue de queue heeft leeggedraaid, kan de queue positie op 0 worden gezet.
  }
  
  
void ProcessingStatus(boolean Processing)
  {
  static boolean PreviousProcessing=false;
  
  if(PreviousProcessing!=Processing)
    {
    PreviousProcessing=Processing;
    if(Processing)
      {
      #if HARDWARE_SERIAL_1
      Serial.write(XOFF);
      #endif
      
      #if HARDWARE_STATUS_LED || HARDWARE_STATUS_LED_RGB
      Led(RED);
      #endif
      }
    else
      {
      QueueProcess();                                                           // Verwerk eventuele events die in de queue zijn geplaatst.    

      if(RequestForConfirm)                                                     // Een event kan een verzoek bevatten om bevestiging. Doe dit dan pas na alle verwerking. 
        {  
        struct NodoEventStruct TempEvent;
        ClearEvent(&TempEvent);    
        TempEvent.Port                  = VALUE_ALL;
        TempEvent.Type                  = NODO_TYPE_SYSTEM;
        TempEvent.Command               = SYSTEM_COMMAND_CONFIRMED;
        TempEvent.Par1                  = RequestForConfirm;
        SendEvent(&TempEvent, false,false);
        RequestForConfirm=false;
        }

      #if HARDWARE_STATUS_LED || HARDWARE_STATUS_LED_RGB
      Led(GREEN);
      #endif


      #if HARDWARE_SERIAL_1
      Serial.write(XON);
      #endif
      }  
    }
  }

//#######################################################################################################
//#####################################  Busy Nodo afhandeling    #######################################
//#######################################################################################################

boolean NodoBusyWait(boolean Display)
  {
  byte x;
  byte error=false;
  
  if(BusyNodo!=0)                                                               // Als een Nodo heeft aangegeven busy te zijn, dan even wachten.
    {
    #if HARDWARE_SERIAL_1 && NODO_MEGA
    if(Display)
      {
      for(x=1;x<=UNIT_MAX;x++)
        if(BusyNodo&(1<<x))
          {
          strcpy(StrTmp, ProgmemString(Text_10));
          strcat(StrTmp, int2str(x));
          PrintString(StrTmp,VALUE_ALL);
          }
      }
    #endif
    
    if(!Wait(30,true,0,false))
      {
      for(x=1;x<=UNIT_MAX;x++)
        if(BusyNodo&(1<<x))
          RaiseMessage(MESSAGE_BUSY_TIMEOUT,x);
      error=true;
      BusyNodo=0;
      }
    }
  return error;
  }  

  

  