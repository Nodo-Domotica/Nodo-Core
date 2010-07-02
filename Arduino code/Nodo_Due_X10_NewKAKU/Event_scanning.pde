/**************************************************************************\

    This file is part of Nodo Due, © Copyright Paul Tonkes

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


#define Loop_INTERVAL_0            50  // tijdsinterval in ms. voor achtergrondtaken.
#define Loop_INTERVAL_1           500  // tijdsinterval in ms. voor achtergrondtaken.
#define Loop_INTERVAL_2          5000  // tijdsinterval in ms. voor achtergrondtaken.

void EventScan(void) 
  {
  byte x,y,z, WiredCounter=0, VariableCounter;
  static unsigned long PauseTimerRF;
  static unsigned long PauseTimerIR;
  unsigned long LoopIntervalTimer_0=millis();
  unsigned long LoopIntervalTimer_1=millis();// millis() maakt dat de intervallen van 1 en 2 niet op zelfde moment vallen => 1 en 2 nu asynchroon
  unsigned long LoopIntervalTimer_2=0L;
  static unsigned long Checksum;

  uint8_t RFbit,RFport,IRbit,IRport;
  RFbit=digitalPinToBitMask(RF_ReceiveDataPin);
  RFport=digitalPinToPort(RF_ReceiveDataPin);  
  IRbit=digitalPinToBitMask(IR_ReceiveDataPin);
  IRport=digitalPinToPort(IR_ReceiveDataPin);

  E.Type=0;
  
  // VARIABLE: *************** Behandel gewijzigde variabelen als en binnengekomen event ******************************
  for(x=0;x<USER_VARIABLES_MAX;x++)
    {
    if(S.UserVar[x]!=UserVarPrevious[x]) // de eerste gewijzigde variabele
      {
      UserVarPrevious[x]=S.UserVar[x];
      E.Content=command2event(CMD_VARIABLE_EVENT,x+1,S.UserVar[x]);
      E.Port=CMD_SOURCE_VARIABLE;
      E.Type=CMD_TYPE_EVENT;
      E.Direction=DIRECTION_INTERNAL;
      return;
      }
    }

  Checksum=0L;
  SerialHold(false);

  while(true)
    {            
    // SERIAL: *************** kijk of er data klaar staat op de seriële poort **********************
    if(Serial.available()>0)
      {
      if(E.Content=Receive_Serial())
        {
        E.Port=CMD_PORT_SERIAL;
        E.Direction=DIRECTION_IN;
        return;
        }
      else
        SerialHold(false);      
      }

    // IR: *************** kijk of er data start op IR en genereer event als er een code ontvangen is **********************
    if((*portInputRegister(IRport)&IRbit)==0)// Kijk if er iets op de IR poort binnenkomt. (Pin=LAAG als signaal in de ether).
      {
      LoopIntervalTimer_0=millis()+Loop_INTERVAL_0; // reset de timer (schakelt RF ontvangst tijdelijk uit)
      if(IRFetchSignal())// Als het een duidelijk signaal was en geen stoorpuls
        {
        if(E.Content=AnalyzeSignal())// Bereken uit de tabel met de pulstijden de 32-bit code
          {
          if((E.Content==Checksum || !S.IRRepeatChecksum) && (S.IRRepeatSuppress ||E.ContentPrevious!=E.Content || millis()>(PauseTimerIR+IR_ENDSIGNAL_TIME)))
             {
             E.Port=CMD_PORT_IR;
             E.Direction=DIRECTION_IN;
             PauseTimerIR=millis(); // huidige tijd voor detecteren van zelfde code zodat herhalingen niet opnieuw opgepikt worden
             return;// dan terugkeren en afhandelen als Nodo event
             }
          Checksum=E.Content;
          }
        }
      }

    // RF: *************** kijk of er data start op RF en genereer event als er een code ontvangen is **********************
    if(LoopIntervalTimer_0<millis()) {
      if((*portInputRegister(RFport)&RFbit)==RFbit)// Kijk if er iets op de RF poort binnenkomt. (Pin=HOOG als signaal in de ether). 
        {
        if(RFFetchSignal())// Als het een duidelijk signaal was en geen stoorpuls
          {
          if(E.Content=AnalyzeSignal())// Bereken uit de tabel met de pulstijden de 32-bit code
            {
            if((E.Content==Checksum || !S.RFRepeatChecksum) && (!S.RFRepeatSuppress || E.ContentPrevious!=E.Content || millis()>(PauseTimerRF+RF_ENDSIGNAL_TIME)))
               {
               E.Port=CMD_PORT_RF;
               E.Direction=DIRECTION_IN;
               PauseTimerRF=millis(); // huidige tijd voor detecteren van zelfde code zodat herhalingen niet opnieuw opgepikt worden
               return;// dan terugkeren en afhandelen als Nodo event
               }
            Checksum=E.Content;
            }
          }
        }
    }

    
    // 2: niet tijdkritische processen die periodiek uitgevoerd moeten worden
    if(LoopIntervalTimer_2<millis())
      {
      LoopIntervalTimer_2=millis()+Loop_INTERVAL_2; // reset de timer

      // CLOCK: **************** Lees periodiek de realtime klok uit en check op events  ***********************
      E.Content=ClockRead(); // Lees de Real Time Clock waarden in de struct Time
      E.Port=CMD_SOURCE_CLOCK;
      E.Type=CMD_TYPE_EVENT;
      if(CheckEventlist(E.Content) && EventTimeCodePrevious!=E.Content)
        {
        EventTimeCodePrevious=E.Content; 
        E.PortPrevious=E.Port;
        E.TypePrevious=E.Type;
        E.Direction=DIRECTION_INTERNAL;
        return;
        }
      else
        E.Content=0L;
      
      // DAYLIGHT: **************** Check zonsopkomst & zonsondergang  ***********************
      CheckDaylight();
      if(Time.Daylight!=DaylightPrevious)// er heeft een zonsondergang of zonsopkomst event voorgedaan
        {
        E.Content=command2event(CMD_CLOCK_EVENT_DAYLIGHT,Time.Daylight,0L);
        E.Port=CMD_SOURCE_CLOCK;
        E.Type=CMD_TYPE_EVENT;
        E.Direction=DIRECTION_INTERNAL;
        DaylightPrevious=Time.Daylight;
        return;
        }
      }

    // 1: niet tijdkritische processen die periodiek uitgevoerd moeten worden
    if(LoopIntervalTimer_1<millis())
      {
      LoopIntervalTimer_1=millis()+Loop_INTERVAL_1; // reset de timer 
      
      // WIRED: *************** kijk of statussen gewijzigd zijn op WIRED **********************
      if(WiredCounter<3)
        WiredCounter++;
      else
        WiredCounter=0;

      // als de huidige waarde groter dan threshold EN de vorige keer was dat nog niet zo DAN verstuur code
      z=false; // vlag om te kijken of er een wijziging is die verzonden moet worden.
      y=analogRead(WiredAnalogInputPin_1+WiredCounter)>>2;
      
      if(y>S.WiredInputThreshold[WiredCounter]+S.WiredInputSmittTrigger[WiredCounter] && !WiredInputStatus[WiredCounter])
        {
        WiredInputStatus[WiredCounter]=true;
        z=true;
        }
      if(y<S.WiredInputThreshold[WiredCounter]-S.WiredInputSmittTrigger[WiredCounter] && WiredInputStatus[WiredCounter])
        {
        WiredInputStatus[WiredCounter]=false;
        z=true;
        }
    
      if(z)// er is een verandering van status op de ingang. 
        {    
        E.Content=command2event(CMD_WIRED_IN_EVENT,WiredCounter+1,WiredInputStatus[WiredCounter]);
        E.Port=CMD_PORT_WIRED;
        E.Type=CMD_TYPE_EVENT;
        E.Direction=DIRECTION_IN;
        return;
        }

    // TIMER: **************** Genereer event als één van de Timers voor de gebruiker afgelopen is ***********************
    if(TimerCounter<USER_TIMER_MAX-1)
      TimerCounter++;
    else
      TimerCounter=0;
    if(UserTimer[TimerCounter]!=0L)// als de timer actief is
        {
        if(UserTimer[TimerCounter]<millis()) // als de timer is afgelopen.
          {
          UserTimer[TimerCounter]=0L;// zet de timer op inactief.
          E.Content=command2event(CMD_TIMER_EVENT,TimerCounter+1,0);
          E.Port=CMD_SOURCE_TIMER;
          E.Type=CMD_TYPE_EVENT;
          E.Direction=DIRECTION_INTERNAL;
          return;
          }
        }
      }
    }
  }

