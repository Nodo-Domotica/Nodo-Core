  /**************************************************************************\
    This file is part of Nodo Due, Â© Copyright Paul Tonkes

    Nodo Due is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Nodo Due is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Nodo Due.  If not, see <http://www.gnu.org/licenses/>.
  \**************************************************************************/

/*********************************************************************************************\
 * Eenvoudige check of event een Nodo commando is die voor deze Nodo bestemd is.
 * Test NIET op geldigheid van de parameters
 * geeft de waarde 0, NODO_TYPE_EVENT of NODO_TYPE_COMMAND terug.
 \*********************************************************************************************/
byte NodoType(unsigned long Content)
  {
  byte x;

  // als het geen Nodo event of commando was dan zowieso geen commando
  if(((Content>>28)&0xf)!=SIGNAL_TYPE_NODO)
    return false;
  
  // als het een UserEvent was, dan is die altijd voor deze Nodo
  if(((Content>>16)&0xff)==CMD_USEREVENT)
    return NODO_TYPE_EVENT;

  // als Unit deel ongelijk is aan eigen adres en ongelijk aan wildcard unit=0, dan was het een event voor een andere Nodo
  x=(Content>>24)&0xf;
  if(x!=S.Unit && x!=0)
    return false;

  x=(Content>>16)&0xff;

  if(x>=FIRST_EVENT && x<=LAST_EVENT)
    return NODO_TYPE_EVENT;

  if(x>=FIRST_COMMAND && x<=LAST_COMMAND)
    return NODO_TYPE_COMMAND;

  return false;
  }  
  

/*********************************************************************************************\
 * Deze functie checked of de code die ontvangen is een geldige uitvoerbare opdracht is
 * Als het een correct commando is wordt een false teruggegeven 
 * in andere gevallen een foutcode
 \*********************************************************************************************/
#define ERROR_PAR1     1
#define ERROR_PAR2     2
#define ERROR_COMMAND  3

byte CommandError(unsigned long Content)
  {
  byte x;
  
  x=NodoType(Content);
  if(x!=NODO_TYPE_COMMAND && x!=NODO_TYPE_EVENT)
    return ERROR_COMMAND;
  
  byte Command      = (Content>>16)&0xff;
  byte Par1         = (Content>>8)&0xff;
  byte Par2         = Content&0xff;
  

  switch(Command)
    {
    //test; geen, altijd goed
    case CMD_LOGFILE_SHOW:
    case CMD_LOGFILE_ERASE:
    case CMD_ERROR:
    case CMD_REBOOT:
    case CMD_VARIABLE_SAVE:
    case CMD_WAITFREERF: 
    case CMD_USERPLUGIN: 
    case CMD_VARIABLE_EVENT:    
    case CMD_DLS_EVENT:
    case CMD_CLOCK_EVENT_DAYLIGHT:
    case CMD_CLOCK_EVENT_ALL:
    case CMD_CLOCK_EVENT_SUN:
    case CMD_CLOCK_EVENT_MON:
    case CMD_CLOCK_EVENT_TUE:
    case CMD_CLOCK_EVENT_WED:
    case CMD_CLOCK_EVENT_THU:
    case CMD_CLOCK_EVENT_FRI:
    case CMD_CLOCK_EVENT_SAT:
    case CMD_STATUS:
    case CMD_DELAY:
    case CMD_SOUND: 
    case CMD_SEND_SIGNAL:
    case CMD_USEREVENT:
    case CMD_SEND_USEREVENT:
    case CMD_ANALYSE_SETTINGS:
    case CMD_RAWSIGNAL_GET:
    case CMD_RAWSIGNAL_PUT:
    case CMD_KAKU:
    case CMD_SEND_KAKU:
      return false;
 
    case CMD_SEND_KAKU_NEW:
    case CMD_KAKU_NEW:    
      if(Par2==VALUE_ON || Par2==VALUE_OFF || Par2<=16)return false;
      return ERROR_PAR2;

    // Generiek: geen par1 of par2 ingevuld.
    case CMD_BOOT_EVENT:
    case CMD_EVENTLIST_WRITE:
    case CMD_EVENTLIST_SHOW:
    case CMD_RESET:
    case CMD_EVENTLIST_ERASE: 
      if(Par1!=0)return ERROR_PAR1;    
      if(Par2!=0)return ERROR_PAR2;    
      return false; 
      
    case CMD_VARIABLE_SET:   
    case CMD_TIMER_SET_SEC:
    case CMD_TIMER_SET_MIN:
      if(Par1>USER_VARIABLES_MAX)return ERROR_PAR1;
      return false;

    // test:Par1 binnen bereik maximaal beschikbare variabelen
    case CMD_VARIABLE_INC: 
    case CMD_VARIABLE_DEC: 
    case CMD_BREAK_ON_VAR_NEQU:
    case CMD_BREAK_ON_VAR_MORE:
    case CMD_BREAK_ON_VAR_LESS:
    case CMD_BREAK_ON_VAR_EQU:
      if(Par1<1 || Par1>USER_VARIABLES_MAX)return ERROR_PAR1;
      return false;
      
    // test:Par1 en Par2 binnen bereik maximaal beschikbare variabelen
    case CMD_VARIABLE_USEREVENT:
    case CMD_VARIABLE_SEND_USEREVENT:
    case CMD_VARIABLE_VARIABLE:
      if(Par1<1 || Par1>USER_VARIABLES_MAX)return ERROR_PAR1;
      if(Par2<1 || Par2>USER_VARIABLES_MAX)return ERROR_PAR2;
      return false;
        
    // test:Par1 binnen bereik maximaal beschikbare variabelen, Par2 is een geldige WIRED_IN
    case CMD_VARIABLE_WIRED_ANALOG:
      if(Par1<1 || Par1>USER_VARIABLES_MAX)return ERROR_PAR1;
      if(Par2<1 || Par2>WIRED_PORTS)return ERROR_PAR2;
      return false;
 
    // test:Par1 binnen bereik maximaal beschikbare timers
    case CMD_TIMER_EVENT:
    case CMD_TIMER_RANDOM:
      if(Par1<1 || Par1>TIMER_MAX)return ERROR_PAR1;
      return false;

    // test:Par1 binnen bereik maximaal beschikbare variabelen,0 mag ook (=alle variabelen)
    case CMD_WIRED_RANGE:
      if(Par1<1 || Par1>WIRED_PORTS)return ERROR_PAR1; // poort
      if(Par2>WIRED_PORTS)return ERROR_PAR2; // range
      return false;

    // Par1 alleen 0,1 of 7
    case CMD_SIMULATE_DAY:
      if(Par1!=0 && Par1!=1 && Par1!=7)return ERROR_PAR1;
      return false;
      
    // geldig jaartal
    case CMD_CLOCK_YEAR:
      if(Par1>21)return ERROR_PAR1;
      return false;
    
    // geldige tijd    
    case CMD_CLOCK_TIME:
      if(Par1>23)return ERROR_PAR1;
      if(Par2>59)return ERROR_PAR2;
      return false;

    // geldige datum
    case CMD_CLOCK_DATE: // data1=datum, data2=maand
      if(Par1>31 || Par1<1)return ERROR_PAR1;
      if(Par2>12 || Par2<1)return ERROR_PAR2;
      return false;

    case CMD_CLOCK_DOW:
      if(Par1<1 || Par1>7)return ERROR_PAR1;
      return false;
       
    // test:Par1 binnen bereik maximaal beschikbare wired poorten, Par2 [0..255]
    case CMD_WIRED_IN_EVENT:
    case CMD_WIRED_ANALOG:
    case CMD_WIRED_THRESHOLD:
    case CMD_WIRED_SMITTTRIGGER:
      if(Par1<1 || Par1>WIRED_PORTS)return ERROR_PAR1;
      return false;

    // test:Par1 binnen bereik maximaal beschikbare wired poorten EN Par2 is ON of OFF
    case CMD_WIRED_OUT:
    case CMD_WIRED_PULLUP:
      if(Par1<1 || Par1>WIRED_PORTS)return ERROR_PAR1;
      if(Par2!=VALUE_ON && Par2!=VALUE_OFF)return ERROR_PAR2;
      return false;

//    case CMD_RAWSIGNAL_COPY: //??? kan dit weg of herstellen wat verpunkt is?
//      if(Par1!=VALUE_IP && Par1!=VALUE_EVENTGHOST && Par1!=VALUE_OFF)return ERROR_PAR1;
//      return false;

    case CMD_WAITBUSY:
    case CMD_SENDBUSY:
      if(Par1!=VALUE_OFF && Par1!=VALUE_ON && Par1!=VALUE_ALL)return ERROR_PAR1;
      return false;

    case CMD_COMMAND_WILDCARD:
      switch(Par1)
        {
        case VALUE_ALL:
        case VALUE_SOURCE_IR:
        case VALUE_SOURCE_RF:
        case VALUE_SOURCE_SERIAL:
        case VALUE_SOURCE_WIRED:
        case VALUE_SOURCE_EVENTLIST:
        case VALUE_SOURCE_SYSTEM:
        case VALUE_SOURCE_TIMER:
        case VALUE_SOURCE_VARIABLE:
        case VALUE_SOURCE_CLOCK:
          break;
        default:
          return ERROR_PAR1;
        }

      switch(Par2)
        {
        case VALUE_ALL:
        case CMD_ERROR:
        case CMD_KAKU:
        case CMD_KAKU_NEW:
        case CMD_USEREVENT:
          break;
        default:
          return ERROR_PAR2;
        } 
      return false;

     // par1 alleen On of Off.
    case CMD_TERMINAL:
    case CMD_TRANSMIT_RF:
    case CMD_TRANSMIT_IR:
      if(Par1!=VALUE_OFF && Par1!=VALUE_ON)return ERROR_PAR1;
      return false;

    case CMD_DIVERT:   
     if(Par1>UNIT_MAX)return ERROR_PAR1;
     return false;

    case CMD_UNIT:
      if(Par1<1 || Par1>UNIT_MAX)return ERROR_PAR1;
      return false;

    default:
      return ERROR_COMMAND;
    }
  }

 
/*********************************************************************************************\
 * Deze functie checked of de code die ontvangen is een uitvoerbare opdracht is/
 * Als het een correct commando is wordt deze uitgevoerd en 
 * true teruggegeven. Zo niet dan wordt er een 'false' retour gegeven.
 * 
 \*********************************************************************************************/

boolean ExecuteCommand(unsigned long Content, int Src, unsigned long PreviousContent, int PreviousSrc)
  {
  unsigned long Event, Action;  
  int x,y;

  byte error        = false;
  byte Command      = (Content>>16)&0xff;
  byte Par1         = (Content>>8)&0xff;
  byte Par2         = Content&0xff;
  byte Type         = (Content>>28)&0x0f;
  byte PreviousType = (PreviousContent>>28)&0x0f;
  

  if(error=CommandError(Content))// als er een error is, dan een error-event genereren en verzenden.
    {
    RaiseError(ERROR_01);
    return false;
    }
  else // geen fouten, dan verwerken
    {        
    switch(Command)
      {   
      case CMD_SEND_KAKU:
        TransmitCode(command2event(CMD_KAKU,Par1,Par2),SIGNAL_TYPE_KAKU);
        break;
        
      case CMD_SEND_KAKU_NEW:
        TransmitCode(command2event(CMD_KAKU_NEW,Par1,Par2),SIGNAL_TYPE_NEWKAKU);
        break;
        
      case CMD_VARIABLE_INC: 
        if((UserVar[Par1-1]+Par2)<=255) // alleen ophogen als variabele nog niet de maximale waarde heeft.
          UserVar[Par1-1]+=Par2;
        break;        
  
      case CMD_VARIABLE_DEC: 
        if((UserVar[Par1-1]-Par2)>=0) // alleen decrement als variabele hierdoor niet negatief wordt
          UserVar[Par1-1]-=Par2;
        break;        
  
      case CMD_VARIABLE_SAVE:   
        for(byte x=0;x<USER_VARIABLES_MAX;x++)
          S.UserVar[x]=UserVar[x];
        SaveSettings();
        break;        
    
      case CMD_VARIABLE_SET:   
        if(Par1==0)
          for(byte x=0;x<USER_VARIABLES_MAX;x++)
            UserVar[x]=Par2;
        else
          UserVar[Par1-1]=Par2;
        break;        
    
      case CMD_VARIABLE_VARIABLE:
        UserVar[Par1-1]=UserVar[Par2-1];
        break;        
  
      case CMD_VARIABLE_WIRED_ANALOG:
        UserVar[Par1-1]=WiredAnalog(Par2-1);
        break;        
  
      case CMD_BREAK_ON_VAR_EQU:
        if(UserVar[Par1-1]==Par2)
          error=true;
        break;
        
      case CMD_BREAK_ON_VAR_NEQU:
        if(UserVar[Par1-1]!=Par2)
          error=true;
        break;        
  
      case CMD_BREAK_ON_VAR_MORE:
        if(UserVar[Par1-1]>Par2)
          error=true;
        break;        
  
      case CMD_BREAK_ON_VAR_LESS:
        if(UserVar[Par1-1]<Par2)
          error=true;
        break;  
  
      case CMD_SEND_USEREVENT:
        // Voeg Unit=0 want een UserEvent is ALTIJD voor ALLE Nodo's. Verzend deze vervolgens.
        TransmitCode(command2event(CMD_USEREVENT,Par1,Par2),SIGNAL_TYPE_NODO);// Maak Unit=0 want een UserEvent is ALTIJD voor ALLE Nodo's.;
        break;
  
      case CMD_VARIABLE_SEND_USEREVENT:
        // Maak Unit=0 want een UserEvent is ALTIJD voor ALLE Nodo's. Verzend deze vervolgens.
        TransmitCode(command2event(CMD_USEREVENT,UserVar[Par1-1],UserVar[Par2-1])&0xf0ffffff,SIGNAL_TYPE_NODO);// Maak Unit=0 want een UserEvent is ALTIJD voor ALLE Nodo's.;
        break;

      case CMD_VARIABLE_USEREVENT:
        // stel een nieuw userevent samen uit twee variabelen en voer deze uit.
        Event=command2event(CMD_USEREVENT,UserVar[Par1-1],UserVar[Par2-1])&0xf0ffffff;

        ProcessEvent(Event,VALUE_DIRECTION_INTERNAL,VALUE_SOURCE_SYSTEM,0,0);
        break;

      case CMD_WIRED_ANALOG:
        // Lees de analoge waarde uit en verzend deze
        TransmitCode(command2event(CMD_WIRED_ANALOG,Par1,WiredAnalog(Par1-1)),SIGNAL_TYPE_NODO);
        break;

      case CMD_SIMULATE_DAY:
        if(Par1==0)Par1=1;
        SimulateDay(Par1); 
        break;     
  
      case CMD_SEND_SIGNAL:
        TransmitCode(0L,SIGNAL_TYPE_UNKNOWN);
        break;        
  
      case CMD_CLOCK_YEAR:
        x=Par1*100+Par2;
        Time.Year=x;
        ClockSet();
        ClockRead();
        break;
      
      case CMD_CLOCK_TIME:
        Time.Hour=Par1;
        Time.Minutes=Par2;
        Time.Seconds=0;
        ClockSet();
        break;
  
      case CMD_CLOCK_DATE: // data1=datum, data2=maand
        Time.Date=Par1;
        Time.Month=Par2;
        ClockSet();
        ClockRead();
        break;
  
      case CMD_CLOCK_DOW:
        Time.Day=Par1;
        ClockSet();
        break;

      case CMD_TIMER_SET_MIN:
        // Par1=timer, Par2=minuten. Timers werken op een resolutie van seconden maar worden door de gebruiker ingegeven in minuten        
        TimerSet(Par1,int(Par2)*60);
        break;
        
      case CMD_TIMER_SET_SEC:
        // Par1=timer, Par2=seconden. Timers werken op een resolutie van seconden.            
        TimerSet(Par1,Par2);
        break;

      case CMD_TIMER_RANDOM:
        UserTimer[Par1-1]=millis()+random(Par2)*60000;// Par1=timer, Par2=maximaal aantal minuten
        break;
  
      case CMD_DELAY:
        if(Par1)
          {
          HoldTimer=millis()+((unsigned long)(Par1))*1000;
          if(Par2==VALUE_OFF) // Niet opslaan in de queue, maar direct een 'dode' pause uitvoeren.
            {
            while(HoldTimer>millis())        
              digitalWrite(MonitorLedPin,(millis()>>7)&0x01);
            }
          else
            {
            // start een nieuwe recursieve loop() om zo de events die voorbij komen te plaatsen in de queue.
            // deze recursieve aanroep wordt beëindigd als HoldTimer==0L
            Hold=CMD_DELAY;
            loop();
            }
          }        
        else
          HoldTimer=0L; //  Wachttijd is afgelopen;
        break;        
        
      case CMD_SOUND: 
        Alarm(Par1,Par2);
        break;     
    
      case CMD_WIRED_PULLUP:
        S.WiredInputPullUp[Par1-1]=Par2==VALUE_ON; // Par1 is de poort[1..4], Par2 is de waarde [0..1]
        digitalWrite(14+WiredAnalogInputPin_1+Par1-1,S.WiredInputPullUp[Par1-1]==VALUE_ON);// Zet de pull-up weerstand van 20K voor analoge ingangen. Analog-0 is gekoppeld aan Digital-14 
        // ??? uitzoeken hoe bovenstaande werkt voor de ATMega2260 !
        SaveSettings();
        break;
                   
      case CMD_WIRED_THRESHOLD:
        S.WiredInputThreshold[Par1-1]=Par2; // Par1 is de poort[1..4], Par2 is de waarde [0..255]
        SaveSettings();
        break;
  
      case CMD_WIRED_RANGE:
        S.WiredInputRange[Par1-1]=Par2; // Par1 is de poort[1..4], Par2 is de range [0..4]
        SaveSettings();
        break;

      case CMD_WIRED_OUT:
        digitalWrite(WiredDigitalOutputPin_1+Par1-1,Par2==VALUE_ON);
        WiredOutputStatus[Par1-1]=Par2==VALUE_ON;
        PrintEvent(Content,VALUE_SOURCE_WIRED,VALUE_DIRECTION_OUTPUT);
        break;
                    
      case CMD_WIRED_SMITTTRIGGER:
        S.WiredInputSmittTrigger[Par1-1]=Par2; // Par1 is de poort[1..4], Par2 is de waarde [0..255]
        SaveSettings();
        break;
       
      case CMD_WAITFREERF: 
        S.WaitFreeRF_Delay=Par1;
        S.WaitFreeRF_Window=Par2;
        SaveSettings();
        break;
  
      case CMD_RAWSIGNAL_COPY:
        if(Par1==VALUE_RF_2_IR)CopySignalRF2IR(Par2);      
        if(Par1==VALUE_IR_2_RF)CopySignalIR2RF(Par2);
        break;        
  
      case CMD_SENDBUSY:
        if(Par1==VALUE_ALL)
          {
          S.SendBusy=Par1;
          SaveSettings();
          }
        else
          if(S.SendBusy==VALUE_ALL)
            {
            S.SendBusy=VALUE_OFF;
            SaveSettings();
            }
          else
            TransmitCode(command2event(CMD_BUSY,Par1,0),SIGNAL_TYPE_NODO);
        break;
        
      case CMD_WAITBUSY:
        if(Par1==VALUE_ALL)
          S.WaitBusy=Par1;
        else
          if(S.WaitBusy==VALUE_ALL)
            S.WaitBusy=VALUE_OFF;
          else
            {
            Hold=CMD_BUSY;
            loop(); // deze recursieve aanroep wordt beëindigd als BusyNodo==0
            }
        break;

      case CMD_TRANSMIT_IR:
        S.TransmitIR=Par1;
        if(Par2)S.TransmitRepeatIR=Par2;
        SaveSettings();
        break;

      case CMD_TRANSMIT_RF:
        S.TransmitRF=Par1;
        if(Par2)S.TransmitRepeatRF=Par2;
        SaveSettings();
        break;
        
      case CMD_USERPLUGIN:
        UserPlugin_Command(Par1,Par2);
        break;        

      case CMD_STATUS:
        x=(Src==VALUE_SOURCE_SERIAL) || (Src==VALUE_SOURCE_TERMINAL);        
        Status(x, Par1, Par2);
        break;

      case CMD_EVENTLIST_SHOW:
        PrintLine(ProgmemString(Text_22));
        for(x=1;x<=EVENTLIST_MAX && Eventlist_Read(x,&Event,&Action);x++)
          PrintEventlistEntry(x,0);
        PrintLine(ProgmemString(Text_22));
        break;
            
      case CMD_UNIT:
        S.Unit=Par1;
        if(Par1>1)
           {
           S.WaitFreeRF_Delay=3 + Par1*3;
           S.WaitFreeRF_Window=3; // 1 eenheid = 100 ms.
           }
        else
           {
           S.WaitFreeRF_Delay=0;
           S.WaitFreeRF_Window=0;
           }
        SaveSettings();
        FactoryEventlist();
        Reset();
    
      case CMD_LOGFILE_ERASE:      
        // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer SDCard chip
        digitalWrite(Ethernetshield_CS_W5100, HIGH);
        digitalWrite(EthernetShield_CS_SDCard,LOW);
        SD.remove(ProgmemString(Text_23));
        // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer W5100 chip
        digitalWrite(Ethernetshield_CS_W5100, LOW);
        digitalWrite(EthernetShield_CS_SDCard,HIGH);
        break;

      case CMD_RAWSIGNAL_GET:
        RawsignalGet=true;
        break;        

      case CMD_REBOOT:
        delay(1000);
        Reset();
        break;        

      case CMD_LOGFILE_SHOW:
        {
        PrintLine(ProgmemString(Text_22));
        // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer SDCard chip
        digitalWrite(Ethernetshield_CS_W5100, HIGH);
        digitalWrite(EthernetShield_CS_SDCard,LOW);
        File dataFile=SD.open(ProgmemString(Text_23));
        if(dataFile) 
          {
          y=0;       
          while(dataFile.available())
            {
            x=dataFile.read();
            if(isprint(x) && y<INPUT_BUFFER_SIZE)
              {
              InputBuffer[y++]=x;
              }
            else
              {
              InputBuffer[y]=0;
              y=0;
              PrintLine(InputBuffer);
              }
            }
          dataFile.close();
          }  
        else 
          RaiseError(ERROR_03);    

        // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer W5100 chip
        digitalWrite(Ethernetshield_CS_W5100, LOW);
        digitalWrite(EthernetShield_CS_SDCard,HIGH);
        PrintLine(ProgmemString(Text_22));
        break;
        }

      case CMD_ANALYSE_SETTINGS:
        S.AnalyseTimeOut=Par1;
        S.AnalyseSharpness=Par2*1000;
        SaveSettings();
        break;

      case CMD_TERMINAL:
        S.Terminal_Enabled=Par1;
        if(Par2!=0)
          S.Terminal_Prompt=Par2;
        SaveSettings();
        break;
      }
    }
  return error?false:true;
  }

 /*******************************************************************************************************\
 * Deze funktie parsed een string en voert de commando uit die daarin voorkomen. Commandos
 * worden gescheiden met een puntkomma.
 \*******************************************************************************************************/
void ExecuteLine(char *Line, byte Port)
  {
  char Command[40], TmpStr[40];
  int PosCommand=0;
  int L;
  byte Error,Par1,Par2,Cmd,z;
  byte State_EventlistWrite;
  unsigned long v,event,action; 

  State_EventlistWrite=0;
  Error=false;  

  L=strlen(Line);
  
  for(int PosLine=0;PosLine<=L;PosLine++)
    {
    byte x=Line[PosLine];

    // puntkomma scheidt opdachten, laatste opdracht hoeft niet met puntkomma afgesloten te worden en een commando moet groter zijn dan drie tekens
    if((x==';' || PosLine==L) && PosCommand>3)
      {
      Command[PosCommand]=0;
      PosCommand=0;
      // string met commando is afgesloten met een puntkomma. Deze bevat nu "commando par1,par2"

      Cmd=0;
      Par1=0;
      Par2=0;

      if(GetArgv(Command,TmpStr,1))
        v=str2val(TmpStr);

      // groter dan een bestaand commando. Dan behandelen als een hex-event
      if(v<=COMMAND_MAX)
        {
        Cmd=v;
        v=0;          

        switch(Cmd)
          {
          // enkele commandos hebben een afwijkende MMI. Deze hier behandelen.
          case CMD_RAWSIGNAL_PUT:
            RawSignalPut(Line);
            break;

          case CMD_EVENTLIST_WRITE:
            State_EventlistWrite=1;
            break;
            
          case CMD_SEND_KAKU_NEW:
          case CMD_KAKU_NEW:
            {
            if(GetArgv(Command,TmpStr,2))
              v=str2val(TmpStr);
              
            if(GetArgv(Command,TmpStr,3))
              Par2=str2val(TmpStr);
              
            if(v>255)
              {
              v=(v&0x0fffffff) | (((unsigned long)SIGNAL_TYPE_NEWKAKU)<<28); //  // Niet Par1 want NewKAKU kan als enige op de Par1 plaats een 28-bit waarde hebben. Hoogste nible wissen en weer vullen met type NewKAKU        
              v|=(Par2==VALUE_ON)<<4; // Stop on/off commando op juiste plek in NewKAKU code
              }
            else
              {
              Par1=v;        
              v=command2event(Cmd,Par1,Par2);
              }
            Error=CommandError(v);
            break;
            }  
            
          case CMD_IP_NODO:
            {
            GetArgv(Command,TmpStr,2);S.Nodo_IP[0]=str2val(TmpStr);
            GetArgv(Command,TmpStr,3);S.Nodo_IP[1]=str2val(TmpStr);
            GetArgv(Command,TmpStr,4);S.Nodo_IP[2]=str2val(TmpStr);
            GetArgv(Command,TmpStr,5);S.Nodo_IP[3]=str2val(TmpStr);        
            SaveSettings();
            Reset();
            break;
            }  
      
          case CMD_EVENT_PORT:
            {
            GetArgv(Command,TmpStr,2);
            S.Event_Port=str2val(TmpStr);
            SaveSettings();
            Reset();
            break;
            }  
            
          case CMD_RESET:
              ResetFactory();
              
          case CMD_EVENTLIST_ERASE:
            Eventlist_Write(1,0L,0L); // maak de eventlist leeg.
            break;        

          case CMD_MONITOR_PORT:
            {
            GetArgv(Command,TmpStr,2);
            S.Terminal_Port=str2val(TmpStr);
            SaveSettings();
            Reset();
            break;
            }  
            
          case CMD_PASSWORD:
            {
            if(GetArgv(Command,S.Password,2))
            SaveSettings();
            break;
            }  

          case CMD_SEND_KAKU:
          case CMD_KAKU://??? hier zit nog een bug 
            {
            if(GetArgv(Command,TmpStr,2))
              {
              z=0;
              Par1=HA2address(TmpStr,&z); // Parameter-1 bevat [A1..P16]. Omzetten naar absolute waarde. z=groep commando
              if(GetArgv(Command,TmpStr,3))
                {
                Par2=(str2val(TmpStr)==VALUE_ON) | (z<<1); // Parameter-2 bevat [On,Off]. Omzetten naar 1,0. tevens op bit-2 het groepcommando zetten.
                v=command2event(Cmd,Par1,Par2);
                }
              }
            Error=CommandError(v);
            break;
            }
            
          default:
            {              
            // standaard commando volgens gewone syntax
            if(GetArgv(Command,TmpStr,2))
              Par1=str2val(TmpStr);
              
            if(GetArgv(Command,TmpStr,3))
              Par2=str2val(TmpStr);
            
            v=command2event(Cmd,Par1,Par2);
            Error=CommandError(v);
            break;
            }
          }
        }  
 
      if(Error)
        {
        if(Error==ERROR_COMMAND) // commando bestaat niet 
          RaiseError(ERROR_01);    
        else // commando bestaat maar parameters niet correct
          RaiseError(ERROR_02);

        // als er een error is, dan stoppen met verwerken van de gehele regel. Dit om stapelen van errors te voorkomen.
        return;
        } 
      else
        {          
        if(State_EventlistWrite==0)// Gewoon uitvoeren
          {
          Nodo_2_RawSignal(v);// bouw een RawSignal op zodat deze later eventueel kan worden verzonden met SendSignal
          ProcessEvent(v,VALUE_DIRECTION_INPUT,Port,0,0);      // verwerk binnengekomen event.
          continue;
          }

        if(State_EventlistWrite==3)
          {
          action=v;
          if(!Eventlist_Write(0,event,action))
            {
            RaiseError(ERROR_06);    
            return;
            }
          State_EventlistWrite=0;
          }

        if(State_EventlistWrite==2)
          {
          event=v;
          State_EventlistWrite=3;
          }

        if(State_EventlistWrite==1)
          {
          State_EventlistWrite=2;
          }
        }
      }
    else
      Command[PosCommand++]=x;
    }
  }
  
  
void RawSignalPut(char *Line, byte start)
  {
  Serial.print("*** RawSignalPut()=");Serial.println(Line);//??? Debug
  Serial.print("*** start=");Serial.println(start,DEC);//??? Debug
  
  
  int L=strlen(Line);
  char ValueStr[40];
  int ValueStrPos=0;
  int PosLine;
  
  for(PosLine=start;PosLine<=L;PosLine++)
    {
    byte x=Line[PosLine];

    if(isxdigit(x))
      {
      // als teken een cijfer is, dan toevoegen aan de string
      ValueStr[ValueStrPos++]=x;
      }
    else
      {
      // ander teken, dus cijfer is klaar.
      ValueStr[ValueStrPos]=0;
      ValueStrPos=0;
      Serial.print("*** ValueStr=");Serial.println(ValueStr);//??? Debug
      }
    }    
  }
  
  

