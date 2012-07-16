
 /**********************************************************************************************\
 * Verwerk de inhoud van de queue
 \*********************************************************************************************/
void ProcessQueue(void)
  {
  byte x;

  if(QueuePos)
    {
    #if NODO_MEGA
    PrintTerminal(ProgmemString(Text_26));
    #endif

    // alvorens de queue leeg te draaien eerst wachten op vrije ether omdat de master nodo nog bezig kan zijn met verzending en
    // een eventueel RF commando niet op de master kan aankomen.
    if(S.WaitFreeRF_Delay==0 && S.WaitFreeRF_Window==0)
      WaitFreeRF(0,50);

    for(x=0;x<QueuePos;x++)
      {
      if(((QueueEvent[x]>>16)&0xff)==CMD_EVENTLIST_WRITE && ((QueueEvent[x]>>24)&0xf)==S.Unit && x<(QueuePos-2)) // cmd
        {
        if(Eventlist_Write(((QueueEvent[x]>>8)&0xff),QueueEvent[x+1],QueueEvent[x+2]))
          x+=2;
        else
          RaiseMessage(MESSAGE_06);    
        }
      else
        ProcessEvent2(QueueEvent[x],VALUE_DIRECTION_INPUT,QueuePort[x],0,0);      // verwerk binnengekomen event.
      }
    QueuePos=0;
    }
  }

 /**********************************************************************************************\
 * Voert alle relevante acties in de eventlist uit die horen bij het binnengekomen event
 * Doorlopen van een volledig gevulde eventlist duurt ongeveer 15ms inclusief printen naar serial
 * maar exclusief verwerking n.a.v. een 'hit' in de eventlist
 \*********************************************************************************************/
boolean ProcessEvent(unsigned long IncommingEvent, byte Direction, byte Port, unsigned long PreviousContent, byte PreviousPort)
  {
  byte x;
  SerialHold(true);  // als er een regel ontvangen is, dan binnenkomst van signalen stopzetten met een seriele XOFF

  Led(RED); // LED aan als er iets verwerkt wordt  
  
  #ifdef USER_PLUGIN
  if(!UserPlugin_Receive(IncommingEvent))
    return true;
  #endif
  
  #if NODO_MEGA
  CheckRawSignalKey(&IncommingEvent); // check of er een RawSignal key op de SDCard aanwezig is en vul met Nodo Event. Call by reference!

  // Als de RAW pulsen worden opgevraagd door de gebruiker...
  // dan de pulsenreeks weergeven en er verder niets mee doen
  if(RawSignal.Key!=-1)
    {
    if(SaveRawSignal(RawSignal.Key))
      PrintTerminal(ProgmemString(Text_13));
    RawSignal.Key=-1;
    return true;
    }
  #endif

  // Verwerk het binnengekomen event
  ProcessEvent2(IncommingEvent,Direction,Port,PreviousContent,PreviousPort);

  // Verwerk eventuele events die in de queue zijn geplaatst.
  ProcessQueue();

  if(BusyOnSent)
    {
    TransmitCode(command2event(S.Unit,CMD_BUSY,VALUE_OFF,0),VALUE_ALL);
    BusyOnSent=false;
    }
  }
  
boolean ProcessEvent2(unsigned long IncommingEvent, byte Direction, byte Port, unsigned long PreviousContent, byte PreviousPort)
  {
  unsigned long Event_1, Event_2;
  int x;
  byte Cmd=(IncommingEvent>>16)&0xff; // Command
  byte Par1=(IncommingEvent>>8 )&0xff; // Par1
  byte Par2=(IncommingEvent    )&0xff; // Par2

  if(S.Lock)
    {
    if(Port==VALUE_SOURCE_RF || Port==VALUE_SOURCE_IR)
      {
      switch(Cmd) // command
        {
        // onderstaande mogen WEL worden doorgelaten als de lock aan staat
        case CMD_TRANSMIT_QUEUE:      // anders kan de Nodo niets ontvangen
        case CMD_LOCK:                // om weer te kunnen unlocken
        case CMD_USEREVENT:           // Noodzakelijk voor uitwisseling userevents tussen Nodo.
        case CMD_STATUS_SEND:         // uitvragen status is onschuldig en kan handig zijn.
        case CMD_BUSY:                // Busy status nodig voor verwerking
        case CMD_MESSAGE:             // Voorkomt dat een message van een andere Nodo een error genereert
        case CMD_BOOT_EVENT:          // Voorkomt dat een boot van een adere Nodo een error genereert
          break;
          
        default:
          RaiseMessage(MESSAGE_09);    
          return false;
        }
      }
    }

  if(Cmd==CMD_BUSY) // ??? verhuizen naar Commands?
    NodoBusy(IncommingEvent);          

  // er is een verzoek ontvangen om de queue te vullen. 
  if(Cmd==CMD_TRANSMIT_QUEUE) //??? verhuizen naar Commands?
    {
    if(QueueReceive(Par1,Par2))
      {
      if(S.SendBusy==VALUE_ALL && !BusyOnSent)
        {
//???@1        Nodo_2_RawSignal(command2event(S.Unit,CMD_BUSY,VALUE_ON,0));
//        RawSendRF();
        TransmitCode(command2event(S.Unit,CMD_BUSY,VALUE_ON,0),VALUE_ALL);
        BusyOnSent=true;
        }
      }
    else
      {
      RaiseMessage(MESSAGE_12);
      return false;
      }
    }

  // print regel. Als Debug aan, dan alle regels die vanuit de eventlist worden verwerkt weergeven
  if(ExecutionDepth==0 || S.Debug==VALUE_ON)
    PrintEvent(IncommingEvent,Direction,Port);  // geef event weer op Serial

  if(ExecutionDepth++>=MACRO_EXECUTION_DEPTH)
    {
    RaiseMessage(MESSAGE_05);
    ExecutionDepth=0;
    return false; // bij geneste loops ervoor zorgen dat er niet meer dan MACRO_EXECUTION_DEPTH niveaus diep macro's uitgevoerd worden
    }

  // ############# Verwerk event ################  
  // als het een Nodo event is en een geldig commando, dan deze uitvoeren
  if(NodoType(IncommingEvent)==NODO_TYPE_COMMAND)
    { // Er is een geldig Commando voor deze Nodo binnengekomen       
    if(S.SendBusy==VALUE_ALL && !BusyOnSent)
      {
      TransmitCode(command2event(S.Unit,CMD_BUSY,VALUE_ON,0),VALUE_ALL);
      BusyOnSent=true;
      }
            
    if(!ExecuteCommand(IncommingEvent,Port,PreviousContent,PreviousPort))
      {
      ExecutionDepth--;
      return false;
      }
    }
  else
    {// Er is een Event binnengekomen  
    // loop de gehele eventlist langs om te kijken of er een treffer is. 255 regels checken duurt 180 milliseconde
    for(x=1; x<=EVENTLIST_MAX; x++)
      {
      Eventlist_Read(x,&Event_1,&Event_2);
      if(CheckEvent(IncommingEvent,Event_1,Port))
        {

#if NODO_MEGA
        if(S.Debug==VALUE_ON)
          {
          EventlistEntry2str(x,ExecutionDepth,TempString,false);
          Serial.println(TempString);
          }
#endif

        if(NodoType(Event_2)==NODO_TYPE_COMMAND) // is de ontvangen code een uitvoerbaar commando?
          {
          if(!ExecuteCommand(Event_2, VALUE_SOURCE_EVENTLIST,IncommingEvent,Port))
            {
            ExecutionDepth--;
            return false;// false terug als het commando niet geldig of niet uitvoerbaar was
            }
          }
        else
          {// het is een ander soort event;
          if(Event_1!=command2event(S.Unit,CMD_COMMAND_WILDCARD,0,0))
            {
            if(!ProcessEvent2(Event_2,VALUE_DIRECTION_INTERNAL,VALUE_SOURCE_EVENTLIST,IncommingEvent,Port))
              {
              ExecutionDepth--;
              return true;
              }
            }
          }
        }
      }// for
    }
  ExecutionDepth--;
  return true;
  }


 /**********************************************************************************************\
 * Toetst of de Code ergens voorkomt in de Eventlist. Geeft False als de opgegeven code niet bestaat.
 * Een eventlist met een lengte van 255 regels is volledig gechecked binnen 10 milliseconden
 \*********************************************************************************************/
boolean CheckEventlist(unsigned long Code,byte Port)
  {
  unsigned long Event, Action;

  for(int x=1; x<=EVENTLIST_MAX;x++)
    {
    Eventlist_Read(x,&Event,&Action);
    if(CheckEvent(Code,Event,Port))
      {
      return true; // match gevonden
      
      }
    }
  return false;
  }


 /**********************************************************************************************\
 * Vergelijkt twee events op matching voor uitvoering Eventlist
 \*********************************************************************************************/
boolean CheckEvent(unsigned long Event, unsigned long MacroEvent, byte Port)
  {  
  byte x,z;
  byte Command; 

  // geen lege events zoeken en verwerken
  if(MacroEvent==0 || Event==0)
    return false;  
  
  // ### exacte match: 
  // als huidige event exact overeenkomt met het event in de regel uit de Eventlist, dan een match
  if(MacroEvent==Event)
     return true; 

  // ### excate match zonder type/unit.  
  // als huidige event (met wegfilterde unit) gelijk is aan MacroEvent, dan een match
  Event&=0x00ffffff;
  MacroEvent&=0x00ffffff;
  if(MacroEvent==Event)return true; 


  // ### CLOCK:
  // is er een match met een CLOCK_EVENT_ALL event?
  x=(Event>>16)&0xff;
  if(x>=CMD_CLOCK_EVENT_SUN && x<=CMD_CLOCK_EVENT_SAT) // het binnengekomen event is een clock event.
    {
    if(((MacroEvent>>16)&0xff)==CMD_CLOCK_EVENT_ALL) // als het event uit de eventlist is een ClockAll event.
      if((MacroEvent&0x0000ffff)==(Event&0x0000ffff)) // en tijdstippen kloppen
        return true;
    }

  // ### WILDCARD:      
  if(((MacroEvent>>16)&0xff)==CMD_COMMAND_WILDCARD) // is regel uit de eventlist een WildCard?
    {
    Command=(Event>>16)&0xff; 
    z=(MacroEvent>>8)&0xff; // Par1 deel van de Wildcard bevat de poort
    if(z!=VALUE_ALL && z!=Port)return false;

    switch(Command) // Command deel van binnengekomen event.
      {
      case CMD_KAKU:
      case CMD_KAKU_NEW:
      case CMD_MESSAGE:
      case CMD_USEREVENT:
        x=Command;
        break;
      default:
        x=0;
      }
    z=MacroEvent&0xff; // Par2 deel Wildcard bevat type event
    if(z!=VALUE_ALL && z!=x)return false;
    return true;
    }

  // USEREVENT:
  // beschouw bij een UserEvent een 0 voor Par1 of Par2 als een wildcard.
  if(((Event>>16)&0xff)==CMD_USEREVENT)// Command
    {
    if(((Event>>8)&0xff)==0 || ((MacroEvent>>8)&0xff)==0){Event&=0xf0ff00ff;MacroEvent&=0xf0ff00ff;}
    if(((Event   )&0xff)==0 || ((MacroEvent   )&0xff)==0){Event&=0xf0ffff00;MacroEvent&=0xf0ffff00;}
    if(MacroEvent==Event)return true; 
    }
  return false; // geen match gevonden
  }


