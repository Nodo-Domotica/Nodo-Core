/*********************************************************************************************\
 * wachtloop die wordt afgebroken als:
 * - <Timeout> seconden zijn voorbij. In dit geval geeft deze funktie een <false> terug.
 * - <BreakNoBusyNodo>==true en alle Nodo's na een "Busy On" ook weer een "Busy Off" hebben verzonden.
 * - <BreakEvent> is opgegeven en dit specifiek opgegeven event is ontvangen.
 * - de Nodo expliciet een "Delay Off" opdracht heeft ontvangen.
 * - Het eerste Event dat voldoet aan Unit en Command is voorbij gekomen.
 \*********************************************************************************************/
boolean WaitAndQueue(int Timeout, boolean BreakNoBusyNodo, unsigned long BreakEvent, int Unit, int Command)
  {
  unsigned long TimeoutTimer=millis() + (unsigned long)(Timeout)*1000;
  unsigned long Event=0L;
  int x,y,z,Port;
  boolean error=false, BusyReceived=false;

  Led(BLUE);
  Queue.Position=0;    

  #ifdef NODO_MEGA
  PrintTerminal(ProgmemString(Text_24));
  #endif

  while(TimeoutTimer>millis())
    {
    if(BreakNoBusyNodo && Busy.Status==0)
      break; // Geen Busy Nodo meer

    if(GetEvent_IRRF(&Event,&Port))
      {      
      //TimeoutTimer=millis() + (unsigned long)(Timeout)*1000;

      #ifdef NODO_MEGA
      CheckRawSignalKey(&Event); // check of er een RawSignal key op de SDCard aanwezig is en vul met Nodo Event. Call by reference!
      PrintEvent(Event,VALUE_DIRECTION_INPUT,Port);
      #endif

      x=(byte)(EventPartCommand(Event)); // cmd
      y=(byte)(EventPartUnit(Event)); // unit
      z=(byte)(EventPartPar1(Event)); // par1

      if(x==CMD_BUSY) // command
        NodoBusy(Event,0);

      if(BreakEvent!=0 && Event==BreakEvent)
        break;

      else if(x==CMD_DELAY) // command
        {
        if(y==Settings.Unit) // Als commando voor deze unit bestemd
          {
          if(z==0) // Par1
            break;// QueueAndWait weer uitschakelen.
          }
        }

      // Het is geen Busy event of Queue commando event, dan deze in de queue plaatsen.
      else if(x!=CMD_BUSY )
        {
        // sla event op in de queue. De Nodo-Small heeft de queue is een kleine array <QueueEvent> staan.
        if(Queue.Position<EVENT_QUEUE_MAX)
          {
          Queue.Event[Queue.Position]=Event;
          Queue.Port[Queue.Position]=Port;
          Queue.Position++;           
          }       
        else
          error=true;
        }

      if((Unit!=0 && (Unit==y || Unit==VALUE_ALL)) && (Command==0 || Command==x || Command==VALUE_ALL))
        break;
      }   
    }
  if(TimeoutTimer<=millis())
    error=true;

  return !error;
  }


/*********************************************************************************************\
 * Controleert of er een Nodo busy is.
 * <Event> event t.b.v. actualiseren status.
 * <Wait> als ongelijk 0, dan wordt opgegeven tijd (seconden) gewacht tot alle Nodos vrij zijn.
 * geeft een true terug als er nog een Nodo busy is.
 \*********************************************************************************************/
boolean NodoBusy(unsigned long Event, int Wait)
{
  int PreviousNodoBusyStatus=Busy.Status;

  if(Event)
  {
    // Check of het event een Busy xxx event is en zet de betreffende status van de Nodo
    if(((Event>>16)&0xff)==CMD_BUSY) // command
    {
      if(((Event>>8)&0xff)==VALUE_ON) // Par1
        Busy.Status  |=  (1<<((Event>>24)&0xf)); // unit nummer
      else
        Busy.Status  &= ~(1<<((Event>>24)&0xf)); // unit nummer

      Busy.ResetTimer=Settings.WaitBusyAll;// Na ingestelde tijd automtische reset van de statussen om te voorkomen dat deze Nodo hangt a.g.v. een gemist Busy Off signaal.
    }
  }

#ifdef NODO_MEGA
  if(Busy.Status!=0 && Settings.TransmitIP==VALUE_ON)
  {
    // geef ook aan de WebApp te kennen dat de Nodo busy is.
    // Alleen naar de WebApp omdat de bron dit al via RF heeft verzorgd.
    TransmitCode(command2event(Settings.Unit, CMD_BUSY,VALUE_ON,0),VALUE_SOURCE_HTTP);
    Busy.BusyOnSent=VALUE_SOURCE_HTTP;
  }
#endif

  if(Wait>0 && Busy.Status!=0)
  {
#ifdef NODO_MEGA
    char *TempString=(char*)malloc(INPUT_BUFFER_SIZE+1);
    if(Settings.Debug==VALUE_ON)
    {
      // Geef weer op welke Nodo(s) wordt gewacht
      strcpy(TempString, ProgmemString(Text_06));
      for(byte x=1;x<=UNIT_MAX;x++)
      {
        if((Busy.Status>>x)&0x1)
        {
          strcat(TempString, int2str(x));  
          strcat(TempString, "  ");         
        }
      }
      PrintTerminal(TempString);
    }
    free(TempString);
#endif

    if(!WaitAndQueue(Wait,true,0,0,0))
    {
      RaiseMessage(MESSAGE_13);
      Busy.Status=0;
    }
  }
  return Busy.Status!=0;
}


/*********************************************************************************************\
 * Zet het hoogste nibbel van een event zodat het event het Type kenmerk krijgt
 \*********************************************************************************************/
unsigned long SetEventType(unsigned long Event, byte Type)
{
  return (Event&0x0fffffff) | (unsigned long)(Type)<<28;
}



/*********************************************************************************************\
 * (re)set een timer. Eenheden in seconden. 
 * Timer 1..15. Timer=0 is een wildcard voor alle timers
 * Als de timer op 0 wordt gezet, wordt er geen event gegenereerd.
 \*********************************************************************************************/
void TimerSet(byte Timer, int Time)
{
  if(Timer==0)// 0=wildcard voor alle timers
  {
    for(int x=0;x<TIMER_MAX;x++)
    {
      if(Time==0)
        UserTimer[x]=0L;
      else
        UserTimer[x]=millis()+(unsigned long)(Time)*1000L;
    }
  }
  else
  {
    if(Time==0)
      UserTimer[Timer-1]=0L;
    else
      UserTimer[Timer-1]=millis()+(unsigned long)Time*1000L;
  }
}

/*********************************************************************************************\
 * Haal voor het opgegeven Command de status op.
 * Let op: call by reference!
 \*********************************************************************************************/
boolean GetStatus(byte *Command, byte *Par1, byte *Par2, boolean ReturnStatus)
  { 
  int x;
  byte xPar1=*Par1,xPar2=*Par2;
  unsigned long event,UL;

  *Par1=0;
  *Par2=0;

  // Eerst alleen een check of het een uitvraagbare stutus is.
  switch (*Command)
    {
    case VALUE_EVENTLIST_COUNT:
    case VALUE_BUILD: 
    case VALUE_HWCONFIG: 
    case CMD_WAITFREERF: 
    case CMD_UNIT: 
    case CMD_SENDBUSY:
    case CMD_DEBUG:
    case CMD_WAITBUSY:
    case CMD_CLOCK_EVENT_DAYLIGHT:
    case CMD_OUTPUT:
    case CMD_VARIABLE_SET:
    case CMD_CLOCK_DATE:
    case CMD_CLOCK_TIME:
    case CMD_CLOCK_DOW:
    case CMD_CLOCK_YEAR:
    case CMD_TIMER_SET_MIN:
    case CMD_WIRED_PULLUP:
    case CMD_WIRED_ANALOG:
    case CMD_PULSE_TIME:
    case CMD_PULSE_COUNT:
    case CMD_WIRED_THRESHOLD:
    case CMD_WIRED_SMITTTRIGGER:
    case CMD_WIRED_IN_EVENT:
    case CMD_WIRED_OUT:
    case CMD_LOCK:
#ifdef NODO_MEGA
    case CMD_LOG:
    case CMD_NODO_IP:
    case CMD_GATEWAY:
    case CMD_SUBNET:
    case CMD_DNS_SERVER:
    case CMD_PORT_SERVER:
    case CMD_PORT_CLIENT:
    case CMD_HTTP_REQUEST:
    case CMD_ID:
    case CMD_TEMP:
#endif
      break;
  
    default:
      return false;
    }

  if(!ReturnStatus)
    return true;

  switch (*Command)
  {
  case VALUE_EVENTLIST_COUNT:
    for(x=1; x<=EVENTLIST_MAX;x++)
      if(Eventlist_Read(x,&event,&UL))// event en UL worden hier gebruikt als dummy
        *Par1=*Par1+1;
    break;

  case CMD_WAITFREERF: 
    *Par1=Settings.WaitFreeRF;
    break;

  case CMD_UNIT: 
    *Par1=Settings.Unit;
    break;        

  case VALUE_BUILD:
    event=NODO_BUILD&0xffff;
    *Par1=EventPartPar1(event);      
    *Par2=EventPartPar2(event);      
    break;        

  case VALUE_HWCONFIG: 
    event=HW_Config&0xffff;
    *Par1=EventPartPar1(event);      
    *Par2=EventPartPar2(event);      
    break;        

  case CMD_SENDBUSY:
    *Par1=Settings.SendBusy;
    break;

  case CMD_DEBUG:
    *Par1=Settings.Debug;
    break;

  case CMD_WAITBUSY:
    *Par1=Settings.WaitBusyAll;
    break;

  case CMD_CLOCK_EVENT_DAYLIGHT:
    *Par1=Time.Daylight;
    break;

  case CMD_OUTPUT:
    *Par1=xPar1;
    switch(xPar1)
    {
    case VALUE_SOURCE_IR:
      *Par2=Settings.TransmitIR;
      break;

    case VALUE_SOURCE_RF:
      *Par2=Settings.TransmitRF;
      break;

#ifdef NODO_MEGA
    case VALUE_SOURCE_HTTP:
      *Par2=Settings.TransmitIP;
      break;
#endif 
    }
    break;

  case CMD_VARIABLE_SET:
    event=float2event(UserVar[xPar1-1], xPar1-1, 0);
    *Par1=(event>>8)&0xff;
    *Par2=event&0xff;
    break;

  case CMD_CLOCK_DATE:
    if(!bitRead(HW_Config,HW_CLOCK))return false;
    *Par1=Time.Date;
    *Par2=Time.Month;
    break;

  case CMD_CLOCK_TIME:
    if(!bitRead(HW_Config,HW_CLOCK))return false;
    *Par1=Time.Hour;
    *Par2=Time.Minutes;
    break;

  case CMD_CLOCK_DOW:
    if(!bitRead(HW_Config,HW_CLOCK))return false;
    *Par1=Time.Day;
    break;

  case CMD_CLOCK_YEAR:
    if(!bitRead(HW_Config,HW_CLOCK))return false;
    *Par1=Time.Year/100;
    *Par2=Time.Year-2000;
    break;

  case CMD_TIMER_SET_MIN:
    *Par1=xPar1;
    if(UserTimer[xPar1-1])
      *Par2=(byte)((UserTimer[xPar1-1]-millis()+59999UL)/60000UL);
    else
      *Par2=0;
    break;

  case CMD_WIRED_PULLUP:
    *Par1=xPar1;
    *Par2=Settings.WiredInputPullUp[xPar1-1];
    break;

  case CMD_WIRED_ANALOG:
    // lees analoge waarde. Dit is een 10-bit waarde, unsigned 0..1023
    x=analogRead(PIN_WIRED_IN_1+xPar1-1);
    event=((unsigned long)x)&0x3ff | ((unsigned long)(xPar1-1)<<12);
    *Par1=EventPartPar1(event);      
    *Par2=EventPartPar2(event);      

    break;

  case CMD_WIRED_THRESHOLD:
    event=((unsigned long)Settings.WiredInputThreshold[xPar1-1])&0x3ff | ((unsigned long)(xPar1-1)<<12);
    *Par1=EventPartPar1(event);      
    *Par2=EventPartPar2(event);      
    break;

  case CMD_WIRED_SMITTTRIGGER:
    event=((unsigned long)Settings.WiredInputSmittTrigger[xPar1-1])&0x3ff | ((unsigned long)(xPar1-1)<<12);
    *Par1=EventPartPar1(event);      
    *Par2=EventPartPar2(event);      
    break;

  case CMD_PULSE_TIME:    
    //???
    event=PulseTime/PULSE_TIME_DIVIDE;
    event=event<=60000?event:60000;
    *Par1=EventPartPar1(event);      
    *Par2=EventPartPar2(event);      
    break;

  case CMD_PULSE_COUNT:
    event=PulseCount&0xffff;
    *Par1=EventPartPar1(event);      
    *Par2=EventPartPar2(event);      
    break;

  case CMD_WIRED_IN_EVENT:
    *Par1=xPar1;
    *Par2=(WiredInputStatus[xPar1-1])?VALUE_ON:VALUE_OFF;
    break;

  case CMD_WIRED_OUT:
    *Par1=xPar1;
    *Par2=(WiredOutputStatus[xPar1-1])?VALUE_ON:VALUE_OFF;
    break;

  case CMD_LOCK:
    *Par1=Settings.Lock==0?0:0x80;// uit 16-bit combi van Par1+Par2 staat de on/off in bit 15.
    break;

#ifdef NODO_MEGA
  case CMD_LOG:
    *Par1=Settings.Log;
    break;

    // pro-forma de commando's die geen fout op mogen leveren omdat deze elders in de statusafhandeling worden weergegeven
  case CMD_NODO_IP:
  case CMD_GATEWAY:
  case CMD_SUBNET:
  case CMD_DNS_SERVER:
  case CMD_PORT_SERVER:
  case CMD_PORT_CLIENT:
  case CMD_HTTP_REQUEST:
  case CMD_ID:
  case CMD_TEMP:
    *Par1=0;
    *Par2=0;
    break;
#endif      
  default:
    return false;
  }
  return true;
}


/**********************************************************************************************\
 * Deze functie haalt een tekst op uit PROGMEM en geeft als string terug
 \*********************************************************************************************/
char* ProgmemString(prog_char* text)
{
  byte x=0;
  static char buffer[90];

  do
  {
    buffer[x]=pgm_read_byte_near(text+x);
  }
  while(buffer[x++]!=0);
  return buffer;
}


/*********************************************************************************************\
 * Sla alle settings op in het EEPROM geheugen.
 \*********************************************************************************************/
void Save_Settings(void)  
  {
  Led(BLUE);
  char ByteToSave,*pointerToByteToSave=pointerToByteToSave=(char*)&Settings;    //pointer verwijst nu naar startadres van de struct. 

  for(int x=0; x<sizeof(struct SettingsStruct) ;x++)
    {
    EEPROM.write(x,*pointerToByteToSave); 
    pointerToByteToSave++;
    }  
  }

/*********************************************************************************************\
 * Laad de settings uit het EEPROM geheugen.
 \*********************************************************************************************/
boolean LoadSettings()
{
  byte x;

  char ByteToSave,*pointerToByteToRead=(char*)&Settings;    //pointer verwijst nu naar startadres van de struct.

  for(int x=0; x<sizeof(struct SettingsStruct);x++)
  {
    *pointerToByteToRead=EEPROM.read(x);
    pointerToByteToRead++;// volgende byte uit de struct
  }

#if NODO_MEGA
  for(x=0;x<USER_VARIABLES_MAX;x++)
    UserVar[x]=Settings.UserVar[x];
#endif
}


/*********************************************************************************************\
 * Alle settings van de Nodo weer op default.
 \*********************************************************************************************/
void ResetFactory(void)
  {
  int x,y;
  Led(BLUE);
  Beep(2000,2000);

  FactoryEventlist();

  Settings.Version                    = SETTINGS_VERSION;
  Settings.NewNodo                    = true;
  Settings.Lock                       = 0;
  Settings.TransmitIR                 = VALUE_OFF;
  Settings.TransmitRF                 = VALUE_ON;
  Settings.SendBusy                   = VALUE_OFF;
  Settings.WaitBusyAll                = 30;
  Settings.Unit                       = UNIT_NODO;

#ifdef NODO_MEGA
  Settings.TransmitIP                 = VALUE_OFF;
  Settings.WaitFreeRF                 = VALUE_OFF;
  Settings.Debug                      = VALUE_OFF;
  Settings.HTTPRequest[0]             = 0; // string van het HTTP adres leeg maken
  Settings.Client_IP[0]               = 0;
  Settings.Client_IP[1]               = 0;
  Settings.Client_IP[2]               = 0;
  Settings.Client_IP[3]               = 0;
  Settings.Nodo_IP[0]                 = 0;
  Settings.Nodo_IP[1]                 = 0;
  Settings.Nodo_IP[2]                 = 0;
  Settings.Nodo_IP[3]                 = 0;
  Settings.Gateway[0]                 = 0;
  Settings.Gateway[1]                 = 0;
  Settings.Gateway[2]                 = 0;
  Settings.Gateway[3]                 = 0;
  Settings.Subnet[0]                  = 255;
  Settings.Subnet[1]                  = 255;
  Settings.Subnet[2]                  = 255;
  Settings.Subnet[3]                  = 0;
  Settings.DnsServer[0]               = 0;
  Settings.DnsServer[1]               = 0;
  Settings.DnsServer[2]               = 0;
  Settings.DnsServer[3]               = 0;
  Settings.HTTPServerPort             = 6636;
  Settings.PortClient                 = 80;
  Settings.ID[0]                      = 0; // string leegmaken
  Settings.Temp[0]                    = 0; // string leegmaken
  Settings.EchoSerial                 = VALUE_ON;
  Settings.EchoTelnet                 = VALUE_OFF;  
  Settings.Log                        = VALUE_OFF;  
  Settings.Password[0]                = 0;

#else
  Settings.WaitFreeRF                 = VALUE_ON;
#endif

  // zet analoge waarden op default
  for(x=0;x<WIRED_PORTS;x++)
    {
    Settings.WiredInputThreshold[x]=512; 
    Settings.WiredInputSmittTrigger[x]=10;
    Settings.WiredInputPullUp[x]=VALUE_ON;
    }

  #if NODO_MEGA
  // maak alle variabelen leeg
  for(byte x=0;x<USER_VARIABLES_MAX;x++)
    Settings.UserVar[x]=0;     
  #endif

  Save_Settings();

  delay(250);// kleine pauze, anders kans fout bij seriële communicatie
  Reset();
}

/**********************************************************************************************\
 * Maak de Eventlist leeg en herstel de default inhoud
 \*********************************************************************************************/
void FactoryEventlist(void)
  {
  // maak de eventlist leeg.
  for(int x=1;x<=EVENTLIST_MAX;x++)
    Eventlist_Write(x,0L,0L);

  // schrijf default regels.
  Eventlist_Write(0,command2event(Settings.Unit,CMD_BOOT_EVENT,Settings.Unit,0),command2event(Settings.Unit,CMD_SOUND,7,0)); // geluidssignaal na opstarten Nodo
  }


/**********************************************************************************************\
 * Geeft de status weer of verzendt deze.
 * verzend de status van een setting als event.
 * Par1 = Command
 * Par2 = selectie (poort, variabele, timer) Als nul, dan waldcard voor alle.
 \**********************************************************************************************/
void Status(byte Par1, byte Par2, byte Transmit)
{
  byte CMD_Start,CMD_End;
  byte Par1_Start,Par1_End;
  byte x,P1,P2; // in deze variabele wordt de waarde geplaats (call by reference)
  boolean s;

#ifdef NODO_MEGA          
  char *TempString=(char*)malloc(INPUT_BUFFER_SIZE+1);
#endif

  if(Par1==0)
    return;

  if(Par2==VALUE_ALL)
    Par2==0;

  if(Par1==CMD_BOOT_EVENT || Par1==0)
    {
    PrintWelcome();
    return;
    }

  if(Par1==VALUE_ALL)
    {
    Par2=0;
    if(!Transmit)
      PrintWelcome();
    CMD_Start=FIRST_COMMAND;
    CMD_End=COMMAND_MAX;
    }
  else
    {
    if(!GetStatus(&Par1,&P1,&P2,false))// kijk of voor de opgegeven parameter de status opvraagbaar is. Zo niet dan klaar.
      return;
    CMD_Start=Par1;
    CMD_End=Par1;
    }

#ifdef NODO_MEGA          
  boolean dhcp=(Settings.Nodo_IP[0] + Settings.Nodo_IP[1] + Settings.Nodo_IP[2] + Settings.Nodo_IP[3])==0;
#endif

  for(x=CMD_Start; x<=CMD_End; x++)
  {
    s=false;
    if(!Transmit)
    {
      s=true;
      switch (x)
      {
#ifdef NODO_MEGA          
      case CMD_CLIENT_IP:
        sprintf(TempString,"%s %u.%u.%u.%u",cmd2str(CMD_CLIENT_IP),Settings.Client_IP[0],Settings.Client_IP[1],Settings.Client_IP[2],Settings.Client_IP[3]);
        PrintTerminal(TempString);
        break;

      case CMD_NODO_IP:
        sprintf(TempString,"%s %u.%u.%u.%u",cmd2str(CMD_NODO_IP), Ethernet.localIP()[0],Ethernet.localIP()[1],Ethernet.localIP()[2],Ethernet.localIP()[3]);
        if(dhcp)
          strcat(TempString,"(DHCP)");
        PrintTerminal(TempString);
        break;

      case CMD_GATEWAY:
        // Gateway
        if(!dhcp)
        {
          sprintf(TempString,"%s %u.%u.%u.%u",cmd2str(CMD_GATEWAY),Settings.Gateway[0],Settings.Gateway[1],Settings.Gateway[2],Settings.Gateway[3]);
          PrintTerminal(TempString);
        }
        break;

      case CMD_SUBNET:
        // Subnetmask
        if(!dhcp)
        {
          sprintf(TempString,"%s %u.%u.%u.%u",cmd2str(CMD_SUBNET),Settings.Subnet[0],Settings.Subnet[1],Settings.Subnet[2],Settings.Subnet[3]);
          PrintTerminal(TempString);
        }
        break;

      case CMD_DNS_SERVER:
        if(!dhcp)
        {
          // DnsServer
          sprintf(TempString,"%s %u.%u.%u.%u",cmd2str(CMD_DNS_SERVER),Settings.DnsServer[0],Settings.DnsServer[1],Settings.DnsServer[2],Settings.DnsServer[3]);
          PrintTerminal(TempString);
        }
        break;

      case CMD_PORT_SERVER:
        sprintf(TempString,"%s %d",cmd2str(CMD_PORT_SERVER), Settings.HTTPServerPort);
        PrintTerminal(TempString);
        break;

      case CMD_PORT_CLIENT:
        sprintf(TempString,"%s %d",cmd2str(CMD_PORT_CLIENT), Settings.PortClient);
        PrintTerminal(TempString);
        break;

      case CMD_HTTP_REQUEST:
        sprintf(TempString,"%s %s",cmd2str(CMD_HTTP_REQUEST),Settings.HTTPRequest);
        PrintTerminal(TempString);
        break;

      case CMD_ID:
        sprintf(TempString,"%s %s",cmd2str(CMD_ID), Settings.ID);
        PrintTerminal(TempString);
        break;

      case CMD_TEMP:
        sprintf(TempString,"%s %s",cmd2str(CMD_TEMP), Settings.Temp);
        PrintTerminal(TempString);
        break;

#endif

      default:
        s=false; 
        break;
      }
    }

    if(!s && GetStatus(&x,&P1,&P2,false)) // Als het een geldige uitvraag is. let op: call by reference !
      {
      if(Par2==0) // Als in het commando 'Status Par1, Par2' Par2 niet is gevuld met een waarde
        {
        switch(x)
          {
        case CMD_OUTPUT:
          Par1_Start=VALUE_SOURCE_IR;
          #ifdef NODO_MEGA
          Par1_End=VALUE_SOURCE_HTTP;
          #else
          Par1_End=VALUE_SOURCE_RF;
          #endif
          break;

        case CMD_WIRED_ANALOG:
        case CMD_WIRED_OUT:
        case CMD_WIRED_PULLUP:
        case CMD_WIRED_SMITTTRIGGER:
        case CMD_WIRED_THRESHOLD:
        case CMD_WIRED_IN_EVENT:
          Par1_Start=1;
          Par1_End=WIRED_PORTS;
          break;      

        case CMD_VARIABLE_SET:
          Par1_Start=1;
          Par1_End=USER_VARIABLES_MAX;
          break;

        case CMD_TIMER_SET_MIN:
          Par1_Start=1;
          Par1_End=TIMER_MAX;
          break;

        default:
          Par1_Start=0;
          Par1_End=0;
          }
        }
      else
        {
        Par1_Start=Par2;
        Par1_End=Par2;
        }

      for(byte y=Par1_Start;y<=Par1_End;y++)
        {
        P1=y;
        P2=Par1; // Leen deze variabele om in GetStatus() te kunnen achterhalen of de status uitvraag naar aanleiding van een [Status All] plaats vond.
        GetStatus(&x,&P1,&P2,true); // haal status op. Call by Reference!

        if(Transmit)
          TransmitCode(command2event(Settings.Unit,x,P1,P2),VALUE_ALL); // verzend als event

        #ifdef NODO_MEGA
        else
          {
          Event2str(command2event(Settings.Unit,x,P1,P2),TempString);
          PrintTerminal(TempString);
          }
        #endif
        }
      }
    }

#ifdef NODO_MEGA
  if(!Transmit && Par1==VALUE_ALL)
    PrintTerminal(ProgmemString(Text_22));

  free(TempString);
#endif
}


/*********************************************************************************************\
 * Deze routine parsed string en geeft het opgegeven argument nummer Argc terug in Argv
 * argumenten worden van elkaar gescheiden door een komma of een spatie.
 * Let op dat de ruimte in de doelstring voldoende is EN dat de bron string netjes is afgesloten 
 * met een 0-byte.
 \*********************************************************************************************/
boolean GetArgv(char *string, char *argv, int argc)
{
  int string_pos=0,argv_pos=0,argc_pos=0; 
  char c,d;

  boolean InArg=false;

  while(string_pos<strlen(string))
  {
    c=string[string_pos];
    d=string[string_pos+1];

    if       (c==' ' && d==' '){
    }
    else if  (c==' ' && d==','){
    }
    else if  (c==',' && d==' '){
    }
    else if  (c==' ' && d>=33 && d<=126){
    }
    else 
    {
      argv[argv_pos++]=c;
      argv[argv_pos]=0;          

      if(d==' ' || d==',' || d==0)
      {
        // We waren bezig met toevoegen van tekens aan een argument, maar kwamen een scheidingsteken tegen
        InArg=false;
        argv[argv_pos]=0;
        argc_pos++;

        if(argc_pos==argc)
          return true;

        argv[0]=0;
        argv_pos=0;
        string_pos++;
      }
    }
    string_pos++;
  }
  return false;
}


/**********************************************************************************************\
 * Geef een geluidssignaal met toonhoogte van 'frequentie' in Herz en tijdsduur 'duration' in milliseconden.
 * LET OP: toonhoogte is slechts een grove indicatie. Deze routine is bedoeld als signaalfunctie
 * en is niet bruikbaar voor toepassingen waar de toonhoogte zuiver/exact moet zijn. Geen PWM.
 * Definieer de constante:
 * #define PIN_SPEAKER <LuidsprekerAansluiting>
 * Neem in setup() de volgende regel op:
 * pinMode(PIN_SPEAKER, OUTPUT);
 * Routine wordt verlaten na beeindiging van de pieptoon.
 * Revision 01, 13-02-2009, P.K.Tonkes@gmail.com
 \*********************************************************************************************/

void Beep(int frequency, int duration)//Herz,millisec 
  {
  long halfperiod=500000L/frequency;
  long loops=(long)duration*frequency/(long)1000;

  for(loops;loops>0;loops--) 
    {
    digitalWrite(PIN_SPEAKER, HIGH);
    delayMicroseconds(halfperiod);
    digitalWrite(PIN_SPEAKER, LOW);
    delayMicroseconds(halfperiod);
    }
  }

/**********************************************************************************************\
 * Geeft een belsignaal.
 * Revision 01, 09-03-2009, P.K.Tonkes@gmail.com
 \*********************************************************************************************/
void Alarm(int Variant,int Option)
{
  byte x,y;

  switch (Variant)
  { 
  case 1:// four beeps
    for(y=1;y<=(Option>1?Option:1);y++)
    {
      Beep(3000,30);
      delay(100);
      Beep(3000,30);
      delay(100);
      Beep(3000,30);
      delay(100);
      Beep(3000,30);
      delay(1000);
    }    
    break;

  case 2: // whoop up
    for(y=1;y<=(Option>1?Option:1);y++)
    {
      for(x=1;x<=50;x++)
        Beep(250*x/4,20);
    }          
    break;

  case 3: // whoop down
    for(y=1;y<=(Option>1?Option:1);y++)
    {
      for(x=50;x>0;x--)
        Beep(250*x/4,20);
    }          
    break;

  case 4:// Settings.O.Settings.
    for(y=1;y<=(Option>1?Option:1);y++)
    {
      Beep(1200,50);
      delay(100);
      Beep(1200,50);
      delay(100);
      Beep(1200,50);
      delay(200);
      Beep(1200,300);
      delay(100);
      Beep(1200,300);
      delay(100);
      Beep(1200,300);
      delay(200);
      Beep(1200,50);
      delay(100);
      Beep(1200,50);
      delay(100);
      Beep(1200,50);
      if(Option>1)delay(500);
    }
    break;

  case 5:// ding-dong
    for(x=1;x<=(Option>1?Option:1);x++)
    {
      if(x>1)delay(2000);
      Beep(1500,500);
      Beep(1200,500);
    }    
    break;

  case 6: // phone ring
    for(x=1;x<(15*(Option>1?Option:1));x++)
    {
      Beep(1000,40);
      Beep(750,40);
    }
    break;

  case 7: // boot
    Beep(1500,100);
    Beep(1000,100);
    break;

  default:// beep
    if(Variant==0)
      Variant=5; // tijdsduur

    if(Option==0)
      Option=20; // toonhoogte

    Beep(100*Option,Variant*10);
    break;
  }
}


/*********************************************************************************************\
 * Deze routine parsed string en zoekt naar keyword. Geeft de startpositie terug waar het keyword
 * gevonden is. -1 indien niet gevonden. Niet casesensitive.
 \*********************************************************************************************/
int StringFind(char *string, char *keyword)
{
  int x,y;
  int keyword_len=strlen(keyword);
  int string_len=strlen(string);

  if(keyword_len>string_len) // doe geen moeite als het te vinden eyword langer is dan de string.
    return -1;

  for(x=0; x<=(string_len-keyword_len); x++)
  {
    y=0;
    while(y<keyword_len && (tolower(string[x+y])==tolower(keyword[y])))
      y++;

    if(y==keyword_len)
      return x;
  }
  return -1;
}

/**********************************************************************************************\
 * Schrijft een event in de Eventlist. Deze Eventlist bevindt zich in het EEPROM geheugen.
 \*********************************************************************************************/
boolean Eventlist_Write(int address, unsigned long Event, unsigned long Action)// LET OP: eerste adres=1
  {
  unsigned long TempEvent,TempAction;

  // als adres=0, zoek dan de eerste vrije plaats.
  if(address==0)
    {
    address++;
    while(Eventlist_Read(address,&TempEvent,&TempAction))address++;
    }

  if(address>EVENTLIST_MAX)
    return false;// geen geldig adres meer c.q. alles is vol.

  address--;// echte adressering begint vanaf nul. voor de user vanaf 1.
  address=address*8+sizeof(struct SettingsStruct);     // Eerste deel van het EEPROM geheugen is voor de settings. Reserveer deze bytes. Deze niet te gebruiken voor de Eventlist!

  EEPROM.write(address++,(Event>>24  & 0xFF));
  EEPROM.write(address++,(Event>>16  & 0xFF));
  EEPROM.write(address++,(Event>> 8  & 0xFF));
  EEPROM.write(address++,(Event      & 0xFF));
  EEPROM.write(address++,(Action>>24 & 0xFF));
  EEPROM.write(address++,(Action>>16 & 0xFF));
  EEPROM.write(address++,(Action>> 8 & 0xFF));
  EEPROM.write(address++,(Action     & 0xFF));

  return true;
  }

/**********************************************************************************************\
 * 
 * Revision 01, 09-12-2009, P.K.Tonkes@gmail.com
 \*********************************************************************************************/
boolean Eventlist_Read(int address, unsigned long *Event, unsigned long *Action)// LET OP: eerste adres=1
{
  if(address>EVENTLIST_MAX)return(false);

  address--;// echte adressering begint vanaf nul. voor de user vanaf 1.
  address=address*8+sizeof(struct SettingsStruct);     // Eerste deel van het EEPROM geheugen is voor de settings. Reserveer deze bytes. Deze niet te gebruiken voor de Eventlist!

  *Event  =((unsigned long)(EEPROM.read(address++))) << 24;
  *Event|= ((unsigned long)(EEPROM.read(address++))) << 16;
  *Event|= ((unsigned long)(EEPROM.read(address++))) <<  8;
  *Event|= ((unsigned long)(EEPROM.read(address++)))      ;
  *Action =((unsigned long)(EEPROM.read(address++))) << 24;
  *Action|=((unsigned long)(EEPROM.read(address++))) << 16;
  *Action|=((unsigned long)(EEPROM.read(address++))) <<  8;
  *Action|=((unsigned long)(EEPROM.read(address  )))      ;
  if(*Event==0L)
    return(false);
  else
    return(true);
}


void RaiseMessage(byte MessageCode)
  {
  unsigned long eventcode;
  int x;

  eventcode=command2event(Settings.Unit,CMD_MESSAGE, Settings.Unit, MessageCode);
  PrintEvent(eventcode,VALUE_DIRECTION_INTERNAL,VALUE_SOURCE_SYSTEM);  // geef event weer op Serial
  TransmitCode(eventcode,VALUE_ALL);
  ProcessEvent2(eventcode,VALUE_DIRECTION_INTERNAL,VALUE_SOURCE_SYSTEM,0,0);
  LastMessage=MessageCode;
  }


/**********************************************************************************************\
 * Stuur de RGB-led.
 *
 * Voor de Nodo geldt:
 *
 * Groen = Nodo in rust en wacht op een event.
 * Rood = Nodo verwerkt event of commando.
 * Blauw = Bijzondere modus Nodo waarin Nodo niet in staat is om events te ontvangen of genereren.
 \*********************************************************************************************/
void Led(byte Color)
{
#ifdef NODO_MEGA
  digitalWrite(PIN_LED_RGB_R,Color==RED);
  digitalWrite(PIN_LED_RGB_B,Color==BLUE);
  digitalWrite(PIN_LED_RGB_G,Color==GREEN);
#else
  digitalWrite(PIN_LED_RGB_R,(Color==RED || Color==BLUE));
#endif
}

#ifdef NODO_MEGA
/**********************************************************************************************\
 * Voeg een regel toe aan de logfile.
 \*********************************************************************************************/
boolean AddFileSDCard(char *FileName, char *Line)
{
  boolean r;

  SelectSD(true);
  File LogFile = SD.open(FileName, FILE_WRITE);
  if(LogFile)
  {
    r=true;
    LogFile.write((uint8_t*)Line,strlen(Line));
    LogFile.write('\n'); // nieuwe regel
    LogFile.close();
  }
  else
    r=false;

  SelectSD(false);
  return r;
}

/**********************************************************************************************\
 * Voeg een regel toe aan de logfile.
 \*********************************************************************************************/
boolean SaveEventlistSDCard(char *FileName)
{
  int x;
  boolean r=true;
  char *TempString=(char*)malloc(INPUT_BUFFER_SIZE+1);

  // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer SDCard chip
  digitalWrite(Ethernetshield_CS_W5100, HIGH);
  digitalWrite(EthernetShield_CS_SDCard,LOW);

  SD.remove(FileName); // eerst bestand wissen, anders wordt de data toegevoegd

  File EventlistFile = SD.open(FileName, FILE_WRITE);
  if(EventlistFile) 
  {
    strcpy(TempString,cmd2str(CMD_EVENTLIST_ERASE));
    EventlistFile.write((uint8_t*)TempString,strlen(TempString));      
    EventlistFile.write('\n'); // nieuwe regel

    for(x=1;x<=EVENTLIST_MAX;x++)
    {
      if(EventlistEntry2str(x,0,TempString,true))
      {
        EventlistFile.write((uint8_t*)TempString,strlen(TempString));      
        EventlistFile.write('\n'); // nieuwe regel
      }
    }
    EventlistFile.close();
  }
  else 
    {
    r=false; // niet meer weer proberen weg te schrijven.
    LastMessage=MESSAGE_03;
    TransmitCode(command2event(Settings.Unit,CMD_MESSAGE, Settings.Unit, LastMessage),VALUE_ALL);
    }

  // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer W510 chip
  digitalWrite(Ethernetshield_CS_W5100, LOW);
  digitalWrite(EthernetShield_CS_SDCard,HIGH);

  free(TempString);
  return r;
}


boolean FileList(void)
{
  boolean x=false;
  File root;
  File entry;
  char *TempString=(char*)malloc(15);

  PrintTerminal(ProgmemString(Text_22));
  SelectSD(true);
  if(root = SD.open("/"))
  {
    root.rewindDirectory();
    while(entry = root.openNextFile())
    {
      if(!entry.isDirectory())
      {
        strcpy(TempString,entry.name());
        TempString[StringFind(TempString,".")]=0;
        SelectSD(false);
        PrintTerminal(TempString);
        SelectSD(true);
      }
      entry.close();
    }
    root.close();
    x=true;
  }
  SelectSD(false);
  PrintTerminal(ProgmemString(Text_22));

  free(TempString);  
  return x;
}


/**********************************************************************************************\
 * Geeft een string terug met een cookie op basis van een random HEX-waarde van 32-bit.
 \*********************************************************************************************/
void RandomCookie(char* Ck)
{
  byte  x,y;

  for(x=0;x<8;x++)
  {
    y=random(0x0, 0xf);
    Ck[x]=y<10?(y+'0'):((y-10)+'A');
  }
  Ck[8]=0; // afsluiten string
}


/*********************************************************************************************\
 * Op het Ethernetshield kunnen de W5100 chip en de SDCard niet gelijktijdig worden gebruikt
 * Deze funktie zorgt voor de juiste chipselect. Default wordt in de Nodo software uitgegaan dat
 * de ethernetchip W5100 is geselecteerd. Met deze routine kan de SDCard worden geselecteerd.
 * input: true = SD_Card geselecteerd.
 \*********************************************************************************************/
void SelectSD(boolean sd)
{
  digitalWrite(Ethernet_shield_CS_W5100, sd);
  digitalWrite(Ethernet_shield_CS_SDCard, !sd);
}


/*********************************************************************************************\
 * Funties op dit tabblad worden gebruikt voor het genereren van een MD5-hascode
 * voor het veilig kunnen checken van een wachtwoord.
 \*********************************************************************************************/
uint32_t md5_T[] PROGMEM = {
  0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf,
  0x4787c62a, 0xa8304613, 0xfd469501, 0x698098d8, 0x8b44f7af,
  0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e,
  0x49b40821, 0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
  0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8, 0x21e1cde6,
  0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8,
  0x676f02d9, 0x8d2a4c8a, 0xfffa3942, 0x8771f681, 0x6d9d6122,
  0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
  0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039,
  0xe6db99e5, 0x1fa27cf8, 0xc4ac5665, 0xf4292244, 0x432aff97,
  0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d,
  0x85845dd1, 0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
  0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 };


#define MD5_HASH_BITS  128
#define MD5_HASH_BYTES (MD5_HASH_BITS/8)
#define MD5_BLOCK_BITS 512
#define MD5_BLOCK_BYTES (MD5_BLOCK_BITS/8)
#define MD5_HASH_BITS  128

struct md5_ctx_t 
{
  uint32_t a[4];
  int32_t counter;
};

// typedef uint8_t md5_hash_t[MD5_HASH_BYTES];

void md5_init(struct md5_ctx_t *s){
  s->counter = 0;
  s->a[0] = 0x67452301;
  s->a[1] = 0xefcdab89;
  s->a[2] = 0x98badcfe;
  s->a[3] = 0x10325476;
}

static
uint32_t md5_F(uint32_t x, uint32_t y, uint32_t z){
  return ((x&y)|((~x)&z));
}

static
uint32_t md5_G(uint32_t x, uint32_t y, uint32_t z){
  return ((x&z)|((~z)&y));
}

static
uint32_t md5_H(uint32_t x, uint32_t y, uint32_t z){
  return (x^y^z);
}

static
uint32_t md5_I(uint32_t x, uint32_t y, uint32_t z){
  return (y ^ (x | (~z)));
}

typedef uint32_t md5_func_t(uint32_t, uint32_t, uint32_t);

#define ROTL32(x,n) (((x)<<(n)) | ((x)>>(32-(n))))  

static void md5_core(uint32_t* a, void* block, uint8_t as, uint8_t s, uint8_t i, uint8_t fi){
  uint32_t t;
  md5_func_t* funcs[]={
    md5_F, md5_G, md5_H, md5_I  };
  as &= 0x3;
  /* a = b + ((a + F(b,c,d) + X[k] + T[i]) <<< s). */
  t = a[as] + funcs[fi](a[(as+1)&3], a[(as+2)&3], a[(as+3)&3])
    + *((uint32_t*)block) + pgm_read_dword(md5_T+i) ;
  a[as]=a[(as+1)&3] + ROTL32(t, s);
}

void md5_nextBlock(struct md5_ctx_t *state, const void* block){
  uint32_t	a[4];
  uint8_t		m,n,i=0;
  /* this requires other mixed sboxes */
  a[0]=state->a[0];
  a[1]=state->a[1];
  a[2]=state->a[2];
  a[3]=state->a[3];

  /* round 1 */
  uint8_t s1t[]={
    7,12,17,22  }; // 1,-1   1,4   2,-1   3,-2
  for(m=0;m<4;++m){
    for(n=0;n<4;++n){
      md5_core(a, &(((uint32_t*)block)[m*4+n]), 4-n, s1t[n],i++,0);
    }
  }
  /* round 2 */
  uint8_t s2t[]={
    5,9,14,20  }; // 1,-3   1,1   2,-2   2,4
  for(m=0;m<4;++m){
    for(n=0;n<4;++n){
      md5_core(a, &(((uint32_t*)block)[(1+m*4+n*5)&0xf]), 4-n, s2t[n],i++,1);
    }
  }
  /* round 3 */
  uint8_t s3t[]={
    4,11,16,23  }; // 0,4   1,3   2,0   3,-1
  for(m=0;m<4;++m){
    for(n=0;n<4;++n){
      md5_core(a, &(((uint32_t*)block)[(5-m*4+n*3)&0xf]), 4-n, s3t[n],i++,2);
    }
  }
  /* round 4 */
  uint8_t s4t[]={
    6,10,15,21  }; // 1,-2   1,2   2,-1   3,-3
  for(m=0;m<4;++m){
    for(n=0;n<4;++n){
      md5_core(a, &(((uint32_t*)block)[(0-m*4+n*7)&0xf]), 4-n, s4t[n],i++,3);
    }
  }
  state->a[0] += a[0];
  state->a[1] += a[1];
  state->a[2] += a[2];
  state->a[3] += a[3];
  state->counter++;
}

void md5_lastBlock(struct md5_ctx_t *state, const void* block, uint16_t length_b){
  uint16_t l;
  uint8_t b[64];
  while (length_b >= 512){
    md5_nextBlock(state, block);
    length_b -= 512;
    block = ((uint8_t*)block) + 512/8;
  }
  memset(b, 0, 64);
  memcpy(b, block, length_b/8);
  /* insert padding one */
  l=length_b/8;
  if(length_b%8){
    uint8_t t;
    t = ((uint8_t*)block)[l];
    t |= (0x80>>(length_b%8));
    b[l]=t;
  }
  else{
    b[l]=0x80;
  }
  /* insert length value */
  if(l+sizeof(uint64_t) >= 512/8){
    md5_nextBlock(state, b);
    state->counter--;
    memset(b, 0, 64-8);
  }
  *((uint64_t*)&b[64-sizeof(uint64_t)]) = (state->counter * 512) + length_b;
  md5_nextBlock(state, b);
}

void md5_ctx2hash(struct md5_hash_t* dest, const struct md5_ctx_t* state){
  memcpy(dest, state->a, MD5_HASH_BYTES);
}

void md5(char* dest)
{
  const void* src=dest;
  uint32_t length_b = strlen((char*)src) * 8;
  struct md5_ctx_t ctx;
  char *Str=(char*)malloc(20);

  md5_init(&ctx);
  while(length_b>=MD5_BLOCK_BITS)
  {
    md5_nextBlock(&ctx, src);
    src = (uint8_t*)src + MD5_BLOCK_BYTES;
    length_b -= MD5_BLOCK_BITS;
  }
  md5_lastBlock(&ctx, src, length_b);
  md5_ctx2hash((md5_hash_t*)&MD5HashCode, &ctx);

  strcpy(Str,PROGMEM2str(Text_05));              
  int y=0;
  for(int x=0; x<16; x++)
  {
    dest[y++]=Str[MD5HashCode[x]>>4  ];
    dest[y++]=Str[MD5HashCode[x]&0x0f];
  }
  dest[y]=0;

  free(Str);
}


boolean FileExecute(char* FileName, boolean ContinueOnError)
  {
  int x,y;
  char *TmpStr=(char*)malloc(INPUT_BUFFER_SIZE+1);
  boolean error=false;

  Led(RED);

  strcpy(TmpStr,FileName);
  strcat(TmpStr,".dat");
  SelectSD(true);
  File dataFile=SD.open(TmpStr);
  if(dataFile) 
    {
    y=0;       
    while(dataFile.available() && !error)
      {
      x=dataFile.read();
      if(isprint(x) && y<INPUT_BUFFER_SIZE)
        TmpStr[y++]=x;
      else
        {
        TmpStr[y]=0;
        y=0;
        SelectSD(false);
        PrintTerminal(TmpStr);
        ExecuteLine(TmpStr,VALUE_SOURCE_FILE);
        SelectSD(true);
        if(!ContinueOnError && LastMessage!=MESSAGE_00)
          {// stoppen met de rest van het script, maar niet terugkeren met nogmaals een error.
          error=false;
          break;
          }
        }
      }
    dataFile.close();
    }  
  else
    error=true;
    
  free(TmpStr);
  SelectSD(false);
  return error;
  }    
#endif

uint8_t *heapptr, *stackptr;
void Trace(int Func, int Pos, unsigned long Value)
{
  stackptr = (uint8_t *)malloc(4);        // use stackptr temporarily
  heapptr = stackptr;                     // save value of heap pointer
  free(stackptr);                         // free up the memory again (sets stackptr to 0)
  stackptr =  (uint8_t *)(SP);            // save value of stack pointer

#ifdef NODO_MEGA
  char* str=(char*)malloc(80);
  strcpy(str,"=> Trace: Seconds=");
  strcat(str,int2str(millis()/1000));
  strcat(str,", Func=");
  strcat(str,int2str(Func));
  strcat(str,", Pos=");
  strcat(str,int2str(Pos));
  strcat(str,", Value=");
  strcat(str,int2str(Value));
  strcat(str,", Memory=");
  strcat(str,int2str(stackptr-heapptr));
  AddFileSDCard("TRACE.DAT", str);
  free(str);

#else
  Serial.print(F("=> Trace: Seconds="));
  Serial.print(millis()/1000);
  Serial.print(F(", Func="));
  Serial.print(Func);
  Serial.print(F(", Pos="));
  Serial.print(Pos);
  Serial.print(F(", Value="));
  Serial.print(Value);
  Serial.print(F(", Memory="));
  Serial.println(stackptr-heapptr);

#endif

}

void PulseCounterISR()
  {
  static unsigned long PulseTimePrevious=0L;                // Tijdsduur tussen twee pulsen teller in milliseconden: vorige meting

  // in deze interrupt service routine staat millis() stil. Dit is echter geen bezwaar voor de meting.
  PulseTime=millis()-PulseTimePrevious;
  if(PulseTime>=PULSE_DEBOUNCE_TIME)
    PulseCount++;
  else
    PulseTime=0;

  PulseTimePrevious=millis();
  }     


#ifdef NODO_MEGA
//################### Calculate #################################

#define CALCULATE_OK                            0
#define CALCULATE_ERROR_STACK_OVERFLOW          1
#define CALCULATE_ERROR_BAD_OPERATOR            2
#define CALCULATE_ERROR_PARENTHESES_MISMATCHED  3
#define CALCULATE_ERROR_UNKNOWN_TOKEN           4
#define STACK_SIZE 50
#define TOKEN_MAX 20

float stack[STACK_SIZE];
float *sp = stack-1;
float *sp_max = &stack[STACK_SIZE-1];

#define is_operator(c)  (c == '+' || c == '-' || c == '*' || c == '/' )

int push(float value)
{
  if(sp != sp_max) // Full
  {
    *(++sp) = value;
    return 0;
  }
  else 
    return CALCULATE_ERROR_STACK_OVERFLOW;
}

float pop()
{
  if(sp != (stack-1)) // empty
    return *(sp--);
}

float apply_operator(char op, float first, float second)
{
  switch(op)
  {
  case '+': 
    return first + second;
  case '-': 
    return first - second;
  case '*': 
    return first * second;
  case '/': 
    return first / second;
    return 0;
  }  
}

char *next_token(char *linep)
{
  while(isspace(*(linep++)));
  while(*linep && !isspace(*(linep++)));
  return linep;
}

int RPNCalculate(char* token)
{
  if(token[0]==0)
    return 0; // geen moeite doen voor een lege string

  if(is_operator(token[0]))
  {
    float second = pop();
    float first = pop();

    if(push(apply_operator(token[0], first, second)))
      return CALCULATE_ERROR_STACK_OVERFLOW;
  }
  else // Als er nog een is, dan deze ophalen
  if(push(atof(token))) // is het een waarde, dan op de stack plaatsen
      return CALCULATE_ERROR_STACK_OVERFLOW;

  return 0;
}

// operators
// precedence   operators         associativity
// 3            !                 right to left
// 2            * / %             left to right
// 1            + - ^             left to right
int op_preced(const char c)
{
  switch(c)    
  {
  case '*':  
  case '/': 
    return 2;
  case '+': 
  case '-': 
    return 1;
  }
  return 0;
}

bool op_left_assoc(const char c)
{
  switch(c)
  { 
  case '*': 
  case '/': 
  case '+': 
  case '-': 
    return true;     // left to right
    //case '!': return false;    // right to left
  }
  return false;
}

unsigned int op_arg_count(const char c)
{
  switch(c)  
  {
  case '*': 
  case '/': 
  case '+': 
  case '-': 
    return 2;
    //case '!': return 1;
  }
  return 0;
}


int Calculate(const char *input, float* result)
{
  const char *strpos = input, *strend = input + strlen(input);
  char token[25];
  char c, *TokenPos = token;
  char stack[32];       // operator stack
  unsigned int sl = 0;  // stack length
  char     sc;          // used for record stack element
  int error=0;

  *sp=0;

  while(strpos < strend)   
  {
    // read one token from the input stream
    c = *strpos;
    if(c != ' ')
    {
      // If the token is a number (identifier), then add it to the token queue.
      if((c >= '0' && c <= '9') || c=='.')
      {
        *TokenPos = c; 
        ++TokenPos;
      }

      // If the token is an operator, op1, then:
      else if(is_operator(c))
      {
        *(TokenPos)=0;
        error=RPNCalculate(token);
        TokenPos=token;
        if(error)return error;
        while(sl > 0)
        {
          sc = stack[sl - 1];
          // While there is an operator token, op2, at the top of the stack
          // op1 is left-associative and its precedence is less than or equal to that of op2,
          // or op1 has precedence less than that of op2,
          // The differing operator priority decides pop / push
          // If 2 operators have equal priority then associativity decides.
          if(is_operator(sc) && ((op_left_assoc(c) && (op_preced(c) <= op_preced(sc))) || (op_preced(c) < op_preced(sc))))
          {
            // Pop op2 off the stack, onto the token queue;
            *TokenPos = sc; 
            ++TokenPos;
            *(TokenPos)=0;
            error=RPNCalculate(token);
            TokenPos=token; 
            if(error)return error;
            sl--;
          }
          else
            break;
        }
        // push op1 onto the stack.
        stack[sl] = c;
        ++sl;
      }
      // If the token is a left parenthesis, then push it onto the stack.
      else if(c == '(')
      {
        stack[sl] = c;
        ++sl;
      }
      // If the token is a right parenthesis:
      else if(c == ')')
      {
        bool pe = false;
        // Until the token at the top of the stack is a left parenthesis,
        // pop operators off the stack onto the token queue
        while(sl > 0)
        {
          *(TokenPos)=0;
          error=RPNCalculate(token);
          TokenPos=token; 
          if(error)return error;
          sc = stack[sl - 1];
          if(sc == '(')
          {
            pe = true;
            break;
          }
          else  
          {
            *TokenPos = sc; 
            ++TokenPos;
            sl--;
          }
        }
        // If the stack runs out without finding a left parenthesis, then there are mismatched parentheses.
        if(!pe)  
          return CALCULATE_ERROR_PARENTHESES_MISMATCHED;

        // Pop the left parenthesis from the stack, but not onto the token queue.
        sl--;

        // If the token at the top of the stack is a function token, pop it onto the token queue.
        if(sl > 0)
          sc = stack[sl - 1];

      }
      else
        return CALCULATE_ERROR_UNKNOWN_TOKEN;
    }
    ++strpos;
  }
  // When there are no more tokens to read:
  // While there are still operator tokens in the stack:
  while(sl > 0)
  {
    sc = stack[sl - 1];
    if(sc == '(' || sc == ')')
      return CALCULATE_ERROR_PARENTHESES_MISMATCHED;

    *(TokenPos)=0;
    error=RPNCalculate(token);
    TokenPos=token; 
    if(error)return error;
    *TokenPos = sc; 
    ++TokenPos;
    --sl;
  }

  *(TokenPos)=0;
  error=RPNCalculate(token);
  TokenPos=token; 
  if(error)
  {
    *result=0;
    return error;
  }  
  *result=*sp;
  return CALCULATE_OK;
}
//################### Einde Calculate #################################

boolean Substitute(char* Input)
  {
  boolean Grab=false;
  byte Res;
  byte x;

  char *Output=(char*)malloc(INPUT_BUFFER_SIZE);
  char *TmpStr=(char*)malloc(INPUT_BUFFER_SIZE);
  char *TmpStr2=(char*)malloc(25);
  char* InputPos  = Input;
  char* OutputPos = Output;
  char* TmpStrPos = TmpStr;
  boolean error=false;

  Res=0;
  while(*(InputPos)!=0)// zolang einde van de string Input nog niet bereikt
    {
    if(*InputPos=='%') 
      {
      if(!Grab)
        {
        Grab=true;
        TmpStrPos=TmpStr;
        }
      else
        {
        Grab=false;
        *TmpStrPos=0;// Sluit string af
        
        // Haal de status van de variabele
        byte Cmd=0;
        GetArgv(TmpStr,TmpStr2,1);
        Cmd=str2cmd(TmpStr2); // commando deel
        TmpStr2[0]=0;
        
        if(Cmd!=0)
          {
          // Er zijn twee type mogelijk: A)Direct te vullen omdat ze niet met status opvraagbaar zijn, B)Op te vragen met status
          byte Par1=0;
          byte Par2=0;
          Res=2;

          // A)Direct te vullen omdat ze niet met status opvraagbaar zijn
          switch(Cmd)
            {
            case CMD_ID:
              strcpy(TmpStr2,Settings.ID);
              break;    

            case CMD_TEMP:
              strcpy(TmpStr2,Settings.Temp);
              break;    
  
            case VALUE_THISUNIT:
              strcpy(TmpStr2,int2str(Settings.Unit));
              break;    
  
            case CMD_MESSAGE:
              strcpy(TmpStr2,int2str(LastMessage));
              break;    
  
            case CMD_UNIT: // Hier lenen we "Unit", maar bij dit het unitnummer van en binnengekomen event.
              strcpy(TmpStr2,int2str(EventPartUnit(Received)));
              break;    
  
            case VALUE_RECEIVED_EVENT:
              Event2str(Received,TmpStr);
              GetArgv(TmpStr,TmpStr2,1);
              break;    
  
            case VALUE_RECEIVED_PAR1:
              Event2str(Received,TmpStr);
              GetArgv(TmpStr,TmpStr2,2);
              break;    
  
            case VALUE_RECEIVED_PAR2:
              Event2str(Received,TmpStr);
              GetArgv(TmpStr,TmpStr2,3);
              break;    
  
            default:
              {
              // B) Op te vragen met status  
              if(GetArgv(TmpStr,TmpStr2,2))
                {
                Par1=str2cmd(TmpStr2);
                if(!Par1)
                  Par1=str2int(TmpStr2);
                }
              if(GetStatus(&Cmd,&Par1,&Par2,true)) // haal status op. Call by Reference!
                {
                Event2str(command2event(0, Cmd ,Par1,Par2),TmpStr);
                if(!GetArgv(TmpStr,TmpStr2,3)) // Als de waarde niet in de 3e parameter zat...
                  GetArgv(TmpStr,TmpStr2,2);   // dan moet hij in de tweede zitten.
                }
              }
            }
          // plak de opgehaalde waarde aan de output string
          for(x=0;x<strlen(TmpStr2);x++)
            *OutputPos++=TmpStr2[x];
          }
        }     
      }
    else if(Grab)
      *TmpStrPos++=*InputPos;// Voeg teken toe aan variabele      
    else
      *OutputPos++=*InputPos;// Voeg teken toe aan outputstring

    InputPos++;  
    }
  *OutputPos=0;// Sluit string af.

  if(TmpStr[0]!=0)
    {
    strcpy(Input,Output);  
    if(Settings.Debug==VALUE_ON)
      {
      Serial.print(F("Substituted: "));
      Serial.println(Input);
      }
    strcpy(Input,Output);  
    }

  if(Grab) // Als % niet correct afgesloten...
    error=true;
  else
  {
    // Nu zijn de formules aan de beurt.
    InputPos  = Input;
    OutputPos = Output;
    TmpStrPos = TmpStr;
    Res=0;

    while(*(InputPos)!=0)// zolang einde van de string Input nog niet bereikt
    {
      if(*InputPos=='#') 
      {
        if(!Grab)
        {
          Grab=true;
          TmpStrPos=TmpStr;
        }
        else
        {
          Grab=false;
          *TmpStrPos=0;// Sluit string af
          float result;
          if(Calculate(TmpStr,&result)==CALCULATE_OK)
          {
            floatToString(TmpStr,result,2,0);
            x=StringFind(TmpStr,".00");          // de overbodige nullen weghalen
            if(x>0)
              TmpStr[x]=0;

            // plak de opgehaalde waarde aan de output string
            for(x=0;x<strlen(TmpStr);x++)
              *OutputPos++=TmpStr[x];

            Res=true;
          }
          else
            Res=false;

        }     
      }
      else if(Grab)
        *TmpStrPos++=*InputPos;// Voeg teken toe aan variabele      
      else
        *OutputPos++=*InputPos;// Voeg teken toe aan outputstring
      InputPos++;  
    }
    *OutputPos=0;// Sluit string af.

    if(Grab) // Als % niet correct afgesloten...
      error=true;

    if(Res && !error)
    {
      strcpy(Input,Output);  
      if(Settings.Debug==VALUE_ON)
      {
        Serial.print("Calculated: ");
        Serial.println(Input);
      }
      strcpy(Input,Output);  
    }
  }
  free(TmpStr2);
  free(TmpStr);
  free(Output);

  return error;
}
#endif

 /**********************************************************************************************\
 * Indien het een vers geresette Nodo is, dan ongedaan maken van deze status.
 \*********************************************************************************************/
void UndoNewNodo(void)
  {
  // Als Eventlist voor eerste keer beschreven wordt, dan is het geen vers geresette Nodo meer. Na beschrijven dan ook geen NewNodo event meer sturen
  if(Settings.NewNodo)
    {
    Settings.NewNodo=false;
    Save_Settings();
    }
  }

