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



/*********************************************************************************************\
 * Deze functie checked of de code die ontvangen is een uitvoerbare opdracht is/
 * Als het een correct commando is wordt deze uitgevoerd en 
 * true teruggegeven. Zo niet dan wordt er een 'false' retour gegeven.
 * 
 \*********************************************************************************************/
boolean ExecuteCommand(unsigned long Content, int Src, int Type, unsigned long PreviousContent, int PreviousSrc, int PreviousType)
  {
  unsigned long Event, Action;  
  int Command,Par1,Par2,x,y,error=false;
  
  Command=(Content>>16)&0xff;
  Par1  =(Content>>8)&0xff;
  Par2  =Content&0xff;

  if(Trace)
    {
    // geen ACTION afdrukken voor commando's die dit zelf als doen volgens afwijkend formaat
    if(Command!=CMD_SEND_IR && Command!=CMD_SEND_RF && Command!=CMD_SEND_RAW_IR && Command!=CMD_SEND_RAW_RF)
      {
      PrintEvent(DIRECTION_EXECUTE,0,0,Content);
      PrintTerm();
      }
    }
    
  switch(Command)
   {           
    case CMD_SEND_IR:
      if(PreviousSrc!=CMD_PORT_IR)
        {
        PrintEvent(DIRECTION_OUT,CMD_PORT_IR,PreviousType,PreviousContent);
        PrintTerm();
        RawCodeCreate(PreviousContent); 
        RawCodeSend_IR(Par1==0?5:Par1);
        }
      break;

    case CMD_SEND_RF:
      if(PreviousSrc==CMD_PORT_RF)break;           // geen RF verzenden als herkomst ook RF was. Dit leidt tot vervuiling van de 433 band.
      if(IsCommand(PreviousContent)==CMD_KAKU_NEW) {        // als vorige event een NewKAKU commando was, dan verzenden
        NewKAKU_2_RawSignal(PreviousContent);
      } else if(IsCommand(PreviousContent)==CMD_KAKU) {     // als vorige event een KAKU commando was, dan verzenden
        KAKU_2_RawSignal(PreviousContent);
      } else if(IsCommand(PreviousContent)==CMD_X10) {      // als vorige event een X10 commando was, dan verzenden
        X10_2_RawSignal(PreviousContent);
      } else {
        RawCodeCreate(PreviousContent);
      }
      if(Par2)WaitForFreeRF();              // wacht tot de ether vrij is
      RawCodeSend_RF(Par1==0?5:Par1);      
      PrintEvent(DIRECTION_OUT,CMD_PORT_RF,PreviousType,PreviousContent);
      PrintTerm();
      break;

    case CMD_VARIABLE_INC: 
      if(Par1<=USER_VARIABLES_MAX && (S.UserVar[Par1-1]+Par2)<=255)
        {      
        S.UserVar[Par1-1]+=Par2;
        SaveSettings();
        }
      else
         error=true;
      break;        

   case CMD_VARIABLE_DEC: 
      if(Par1<=USER_VARIABLES_MAX && (S.UserVar[Par1-1]-Par2)>=0)
        {
        S.UserVar[Par1-1]-=Par2;
        SaveSettings();
        }
      else
         error=true;
      break;        

   case CMD_VARIABLE_SET:   
     if(Par1>0 && Par1<=USER_VARIABLES_MAX)
       {
       S.UserVar[Par1-1]=Par2;
       SaveSettings();
       }
     else
       error=true;
     break;

   case CMD_VARIABLE_DAYLIGHT:   
     if(Par1>0 && Par1<=USER_VARIABLES_MAX)
       {
       S.UserVar[Par1-1]=Time.Daylight;
       SaveSettings();
       }
     else
       error=true;
     break;


   case CMD_VARIABLE_VARIABLE:
      if(Par1>=1 && Par1<=USER_VARIABLES_MAX && Par2>=1 && Par2<=USER_VARIABLES_MAX) 
        {
        S.UserVar[Par1-1]=S.UserVar[Par2-1];
        SaveSettings();
        }
      else
        error=true;
      break;        

   case CMD_VARIABLE_WIRED_ANALOG:
      if(Par1>=1 && Par1<=USER_VARIABLES_MAX && Par2>=1 && Par2<=4) 
        {
        S.UserVar[Par1-1]=analogRead(WiredAnalogInputPin_1+(Par2-1))>>2;
        SaveSettings();
        }
      else
         error=true;
      break;        

   case CMD_BREAK_ON_VAR_EQU:
      if(Par1>=1 && Par1<=USER_VARIABLES_MAX) 
        {
        if(S.UserVar[Par1-1]==Par2)
          {
          if(Trace)
            {
            Serial.print(Text(Text_09));
            PrintTerm();
            }
          return false;
          }
        }
      else
        error=true;
      break;        

   case CMD_BREAK_ON_VAR_NEQU:
      if(Par1>=1 && Par1<=USER_VARIABLES_MAX) 
        {
        if(S.UserVar[Par1-1]!=Par2)
          {
          if(Trace)
            {
            Serial.print(Text(Text_09));
            PrintTerm();
            }
          return false;
          }
        }
      else
         error=true;
      break;        

   case CMD_BREAK_ON_VAR_MORE:
      if(Par1>=1 && Par1<=USER_VARIABLES_MAX) 
        {
        if(S.UserVar[Par1-1]>Par2)
          {
          if(Trace)
            {
            Serial.print(Text(Text_09));
            PrintTerm();
            }
          return false;
          }
        }
      else
         error=true;
      break;        

   case CMD_BREAK_ON_VAR_LESS:
      if(Par1>=1 && Par1<=USER_VARIABLES_MAX) 
        {
        if(S.UserVar[Par1-1]<Par2)
          {
          if(Trace)
            {
            Serial.print(Text(Text_09));
            PrintTerm();
            }
          return false;
          }
        }
      else
         error=true;
      break;  
      
  case CMD_TIMER_RESET:// Deze heeft betrekking op alle timers: cancellen.
     if(Par1==0)
       {
       for(x=0;x<USER_TIMER_MAX;x++)
         UserTimer[x]=0L;
       }
     else
       {
       if(Par1>0 && Par1<=USER_TIMER_MAX)// voor de gebruiker beginnen de timers bij één.
         UserTimer[Par1-1]=0L;
       else
         error=true;
       }
    break;

  case CMD_SEND_RAW_IR:
    PrintEvent(DIRECTION_OUT,CMD_PORT_IR,0,0);
    PrintRawSignal();
    PrintTerm();
    RawCodeSend_IR(Par1==0?5:Par1);
    break;        

  case CMD_SEND_RAW_RF:
    if(Par2)WaitForFreeRF(); 
    PrintEvent(DIRECTION_OUT,CMD_PORT_RF,0,0);
    PrintRawSignal();
    PrintTerm();
    RawCodeSend_RF(Par1==0?5:Par1);
    break;              

  case CMD_CLOCK_DLS: 
     if(S.DaylightSaving!=Par1)// alleen verzetten als de setting veranderd, anders wordt de klok twee uur verzet!
        {
        S.DaylightSaving=Par1;
        if(S.DaylightSaving==1)
          Time.Hour=Time.Hour<23?Time.Hour+1:0;
        else
          Time.Hour=Time.Hour==0?23:Time.Hour-1;
        ClockSet();
        SaveSettings();
        }
     break;        

  case CMD_RESET:
     VariableClear(0);// wis alle variabelen
     delay(500);// kleine pauze, anders fout bij seriële communicatie met EventGhost.
     Reset();
     break;        

  case CMD_CLOCK_TIME:
     if(Par1>=0 && Par1<=23)
       {
       Time.Hour=Par1;
       if(Par2>=0 && Par2<=59)
         {
         Time.Minutes=Par2;
         Time.Seconds=0;
         ClockSet();
         break;
         }
       }
     error=true;
     break;        

  case CMD_CLOCK_DATE: // data1=datum, data2=maand
     if(Par1>=1 && Par1<=31)
       {
       Time.Date=Par1;
       if(Par2>=1 && Par2<=12)
         {
         Time.Month=Par2;
         ClockSet();
         break;
         }
       }
     error=true;
     break;        

  case CMD_CLOCK_YEAR:
     x=Par1*100+Par2;
     if(x>=2000 && x<=2090)// nieuwe millenium bug, maar dan ben ik al 130 jaar ;-)
       {
       Time.Year=x;
       ClockSet();
       break;
       }
     error=true;
     break;        
       
  case CMD_TIMER_SET:
    if(Par1>0 && Par1<=USER_TIMER_MAX)// voor de gebruiker beginnen de timers bij één.
      UserTimer[Par1-1]=millis()+(unsigned long)(Par2)*60000L;// Par1=timer, Par2=minuten
    else
      error=true;
    break;

  case CMD_TIMER_RANDOM:
    if(Par1>0 && Par1<=USER_TIMER_MAX)
      UserTimer[Par1-1]=millis()+(unsigned long)random(Par2)*60000L;// Par1=timer, Par2=maximaal aantal minuten
    else
      error=true;
    break;

  case CMD_DELAY:
     delay(Par1*1000);     
     break;        
 
  case CMD_SOUND: 
     Alarm(Par1,Par2);
     break;     

  case CMD_RECEIVE_REPEATS_RF: 
     S.RFRepeatChecksum=Par1;
     S.RFRepeatSuppress=Par2;
     SaveSettings();
     break;     

  case CMD_RECEIVE_REPEATS_IR: 
     S.IRRepeatChecksum=Par1;
     S.IRRepeatSuppress=Par2;
     SaveSettings();
     break;     

  case CMD_SIMULATE_DAY:
     if(Par1==0)Par1=1;
     if(Par1!=1)Par1=7;
     SimulateDay(Par1); 
     break;     

  case CMD_WIRED_PULLUP:
     if(Par1>0 && Par1<5)
        {
        S.WiredInputPullUp[Par1-1]=Par2; // Par1 is de poort[1..4], Par2 is de waarde [0..1]
        SaveSettings();
        }
     else
       error=true;
     digitalWrite(14+WiredAnalogInputPin_1+Par1-1,S.WiredInputPullUp[Par1-1]?HIGH:LOW);// Zet de pull-up weerstand van 20K voor analoge ingangen. Analog-0 is gekoppeld aan Digital-14
     break;
           
  case CMD_VARIABLE_CLEAR:
     error=VariableClear(Par1);
     break;
     
  case CMD_WIRED_THRESHOLD:
     if(Par1>=1 && Par1<=4)
       {
       S.WiredInputThreshold[Par1-1]=Par2; // Par1 is de poort[1..4], Par2 is de waarde [0..255]
       SaveSettings();
       }
     break;

  case CMD_WIRED_OUT:
     if(Par1>=1 && Par1<=4 && Par2>=0 && Par2<=1)
        {
        digitalWrite(WiredDigitalOutputPin_1+Par1-1,Par2);
        WiredOutputStatus[Par1-1]=Par2;
        }
      else
         error=true;
     break;

  case CMD_FORWARD:
     Forward=Par1;
     break;        

  case CMD_USER:
     ExecuteEventlist(command2event(CMD_USER,Par1,Par2),CMD_SOURCE_SYSTEM,CMD_TYPE_COMMAND,PreviousContent,PreviousSrc,PreviousType);
     break;        
                
  case CMD_WIRED_SMITTTRIGGER:
     if(Par1>0 && Par1<5)
       {
       S.WiredInputSmittTrigger[Par1-1]=Par2; // Par1 is de poort[1..4], Par2 is de waarde [0..255]
       SaveSettings();
       }
     break;

  case CMD_STATUS_EVENT:  
     x=Par1;// bevat het commando waarvoor de status opgehaald moet worden
     Par1=Par2;
     if(GetStatus(&x,&Par1,&Par2))// let op: call by reference. Gegevens komen terug in Par1 en Par2
        {
        Event=command2event(x,Par1,Par2);
        PrintEvent(DIRECTION_INTERNAL,CMD_SOURCE_SYSTEM,CMD_TYPE_COMMAND,Event);
        PrintTerm();
        ExecuteEventlist(Event,CMD_SOURCE_SYSTEM,CMD_TYPE_COMMAND,PreviousContent,PreviousSrc,PreviousType);
        }
     else
       error=true;
     break;    
      
  default:
     error=true;
   }   

   if(error)
     {
     Serial.print(Text(Text_06));
     PrintCode(Content);
     PrintTerm();
     return false;
     }
   return true;
   }


