
 /**********************************************************************************************\
 * Voert alle relevante acties in de eventlist uit die horen bij het binnengekomen event
 * Doorlopen van een volledig gevulde eventlist duurt ongeveer 15ms inclusief printen naar serial
 * maar exclusief verwerking n.a.v. een 'hit'.
 \*********************************************************************************************/
boolean ProcessEvent(unsigned long IncommingEvent, byte Direction, byte Port, unsigned long PreviousContent, byte PreviousPort)
  {
  byte x;
  boolean CommandForThisNodo=CheckEventlist(IncommingEvent,Port) || NodoType(IncommingEvent)==NODO_TYPE_COMMAND;// kan deze er uit worden geprogrammeerd???? vraagt veel tijd.
  SerialHold(true);  // als er een regel ontvangen is, dan binnenkomst van signalen stopzetten
  boolean SetBusyOff=false;
  
  // geen events oppikken als er een file wordt ontvangen voor de SDCard
  if(FileWriteMode>0)
    return true;

  digitalWrite(PIN_LED_RGB_R,HIGH);           // LED aan als er iets verwerkt wordt  

// uitwerken dat de wordt weergegeven dat dit de queue in gaat en niet feitelijk verwerkt wordt.
//  if(CommandForThisNodo && Hold)
//    PrintLinePB_AddProgMemString(Text_09);

//??? Serial.println("*** debug: ProcessEvent();");//???

  #ifdef USER_PLUGIN
  if(!UserPlugin_Receive(IncommingEvent))
    return true;
  #endif
   
  PrintEvent(IncommingEvent,Port,Direction);  // geef event weer op Serial

  // houdt bij wat de busy status van andere Nodo's is.
  if(((IncommingEvent>>16)&0xff)==CMD_BUSY) // command
    {
    x=(IncommingEvent>>24)&0xf; // unit
    if(((IncommingEvent>>8)&0xff)==VALUE_ON) // Par1
      {
      BusyNodo|=(1<<x);
      HoldTimer=millis()+60000; // timeout teller (opnieuw) zetten voor geval er een 'Busy Off;'event gemist wordt
      }
    else
      BusyNodo&=~(1<<x);
    }

  // Als de RAW pulsen worden opgevraagd door de gebruiker...
  // dan de pulsenreeks weergeven en er verder niets mee doen
  if(RawSignal.Key!=0)
    {
    if(SaveRawSignal(RawSignal.Key))
      PrintLine(ProgmemString(Text_13));
    RawSignal.Key=0;
    //??? PrintRawSignal();
    return true;
    }

  if(S.WaitBusy!=VALUE_OFF)
    {
    Hold=CMD_BUSY;
    loop(); // deze recursieve aanroep wordt beëindigd als BusyNodo==0
    }

  // event behandelen als uitvoerbaar commando voor deze Nodo of er een hit is in de eventlist.
  if(CommandForThisNodo)
    {
    // verzend 'Busy On;'
    if(S.SendBusy==VALUE_ALL && !Hold)
      {
      TransmitCode(command2event(CMD_BUSY,VALUE_ON,0),VALUE_ALL);
      SetBusyOff=true;
      }

    // als de Nodo in de hold modus is gezet, verzamel dan events in de queue
    if(Hold)
      {
      if((IncommingEvent&0xffffff00)==command2event(CMD_DELAY,0,0) ||
         (IncommingEvent&0xffffff00)==command2event(CMD_WAITBUSY,0,0)   )
        {
        HoldTimer=0L; //  Zet de wachttijd op afgelopen;        
        return true;
        }
        
      // als het event voorkomt in de eventlist of het is een Nodo commando voor deze Nodo, dan is het relevant om in queue te plaatsen
      // als er nog plek is in de queue...
      if(QueuePos<EVENT_QUEUE_MAX)
        {
        QueueEvent[QueuePos]=IncommingEvent;
        QueuePort[QueuePos]=Port;
        QueuePos++;           
        }       
      else
        RaiseError(ERROR_04);
      return true;
      }
    else // !Hold
      {
      ProcessEvent2(IncommingEvent,Direction,Port,PreviousContent,PreviousPort);
      }
    }
    
  // Verwerk de events die in de queue zijn geplaatst.
  if(QueuePos)
    {
    x=QueuePos;
    for(x=0;x<QueuePos;x++)
      {
      //??? nog weergeven dat dit event uit de queue komt. PB_AddProgMemString(Text_08);
      PrintEvent(QueueEvent[x],QueuePort[x],VALUE_DIRECTION_INPUT);  // geef event weer op Serial
      ProcessEvent2(QueueEvent[x],VALUE_DIRECTION_INPUT,QueuePort[x],0,0);      // verwerk binnengekomen event.
      }
    QueuePos=0;
    }       

  // verzend 'Busy Off;'
  if(SetBusyOff)
    {
    SetBusyOff=false;
    TransmitCode(command2event(CMD_BUSY,VALUE_OFF,0),VALUE_ALL);
    }
  }

boolean ProcessEvent2(unsigned long IncommingEvent, byte Direction, byte Port, unsigned long PreviousContent, byte PreviousPort)
  {
  unsigned long Event_1, Event_2;
  int w,x;

  // print regel. Als trace aan, dan alle regels die vanuit de eventlist worden verwerkt weergeven
  if(EventlistDepth>0 && S.Debug==VALUE_ON)
    PrintEvent(IncommingEvent,Port,Direction);  // geef event weer op Serial

  if(EventlistDepth++>=MACRO_EXECUTION_DEPTH)
    {
    RaiseError(ERROR_05);
    EventlistDepth=0;
    return false; // bij geneste loops ervoor zorgen dat er niet meer dan MACRO_EXECUTION_DEPTH niveaus diep macro's uitgevoerd worden
    }

  // ############# Verwerk event ################  
  // als het een Nodo event is en een geldig commando, dan deze uitvoeren
  if(NodoType(IncommingEvent)==NODO_TYPE_COMMAND)
    { // Er is een geldig Commando binnengekomen       
    if(!ExecuteCommand(IncommingEvent,Port,PreviousContent,PreviousPort))
      {
      EventlistDepth--;
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
        if(S.Debug==VALUE_ON)
          {
          EventlistEntry2str(x,EventlistDepth,TempString);
          PrintLine(TempString);
          }
          
        if(NodoType(Event_2)==NODO_TYPE_COMMAND) // is de ontvangen code een uitvoerbaar commando?
          {
          if(!ExecuteCommand(Event_2, VALUE_SOURCE_EVENTLIST,IncommingEvent,Port))
            {
            EventlistDepth--;
            return false;// false terug als het commando niet geldig of niet uitvoerbaar was
            }
          }
        else
          {// het is een ander soort event;
          if(Event_1!=command2event(CMD_COMMAND_WILDCARD,0,0))
            {
            if(!ProcessEvent2(Event_2,VALUE_DIRECTION_INTERNAL,VALUE_SOURCE_EVENTLIST,IncommingEvent,Port))
              {
              EventlistDepth--;
              return true;
              }
            }
          }
        }
      }// for
    }
  EventlistDepth--;
  return true;
  }


 /**********************************************************************************************\
 * Toetst of de Code ergens voorkomt in de Eventlist. Geeft False als de opgegeven code niet bestaat.
 * Een eventlist met een lengte van 255 regels is volledig gechecked binnen 10milliseconden
 \*********************************************************************************************/
boolean CheckEventlist(unsigned long Code,byte Port)
  {
  unsigned long Event, Action;

  for(int x=1; x<=EVENTLIST_MAX;x++)
    {
    Eventlist_Read(x,&Event,&Action);
    if(CheckEvent(Code,Event,Port))
      return true; // match gevonden
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
  // Serial.print("CheckEvent() > Event=0x");Serial.print(Event,HEX);Serial.print(", MacroEvent=0x");Serial.print(MacroEvent,HEX);PrintTerm();//???debugging

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

    switch(Command) // Command deel van binnengekomen event. //??? staan ze er allemaal in?
      {
      case CMD_KAKU:
      case CMD_KAKU_NEW:
      case CMD_ERROR:
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


// /**********************************************************************************************\
// * vult een string met een regel uit de Eventlist.
// * geeft false terug als de regel leeg is
// \*********************************************************************************************/
//boolean EventlistEntry2str(int entry, byte d, char* Line)
//  {
//  unsigned long Event, Action;
//    
//  Eventlist_Read(entry,&Event,&Action); // leesregel uit de Eventlist.    
//  if(Event==0)
//    return false;
//
//  // Geef de entry van de eventlist weer
//  strcpy(Line,cmd2str(VALUE_SOURCE_EVENTLIST));
//  strcat(Line," ");
//  strcat(Line,int2str(entry));
//
//  // geef het event weer
//  strcat(Line,"; ");
//  strcat(Line,Event2str(Event));
//
//  // geef het action weer
//  strcat(Line,"; ");
//  strcat(Line,Event2str(Action));
//  return true;
//  }

