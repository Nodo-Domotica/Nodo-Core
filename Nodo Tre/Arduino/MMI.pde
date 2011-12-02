
/**************************************************************************\

    This file is part of Nodo Due, (C) Copyright Paul Tonkes

    Nodo Due is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, for
    (at your option) any later version.

    Nodo Due is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Nodo Due.  If not, see <http://www.gnu.org/licenses/>.

\**************************************************************************/


 /*********************************************************************************************\
 * Print een event volgens formaat:  'EVENT/ACTION: <port>, <type>, <content>
 \*********************************************************************************************/
void PrintEvent(unsigned long Content, byte Port, byte Direction)
  {
  byte x;
  char Line[100];
  Line[0]=0; // als start een lege string
  

  // datum en tijd weergeven
  if(Time.Day) // Time.Day=true want dan is er een RTC aanwezig.
    {   
    strcat(Line,DateTimeString());
    strcat(Line,", ");
    }
    
  // Geef de richting van de communicatie weer
  switch(Direction)
    {
    case VALUE_SOURCE_QUEUE:// testen ???
      strcat(Line,ProgmemString(cmd2str(VALUE_SOURCE_QUEUE)));      
      strcat(Line,"-");
      strcat(Line, int2str(QueuePos+1));
      strcat(Line,"=");
      break;
    
    case VALUE_DIRECTION_INTERNAL:
      strcat(Line,ProgmemString(Text_25));
      break;
      
    case VALUE_DIRECTION_INPUT:
      strcat(Line,ProgmemString(Text_12));
      break;
      
    case VALUE_DIRECTION_OUTPUT:
      strcat(Line,ProgmemString(Text_11));
      break;
    }
      
  if(Port==VALUE_SOURCE_EVENTGHOST)
    strcat(Line, ip2str(IP));
  else
    strcat(Line, cmd2str(Port));
  

  // Voor NIET-nodo signalen is het afdrukken van het unitnummer zinloos
  if(Port!=VALUE_SOURCE_EVENTGHOST)
    {
    if(((Content>>28)&0xf)==SIGNAL_TYPE_NODO && ((Content>>16)&0xff)!=CMD_KAKU_NEW && ((Content>>16)&0xff)!=CMD_KAKU)
      {
      strcat(Line, ", "); 
      strcat(Line, cmd2str(CMD_UNIT));
      strcat(Line, "=");
      strcat(Line, int2str((Content>>24)&0xf)); 
      }
    }
    
  // geef het event weer
  strcat(Line, ", ");
  strcat(Line, ProgmemString(Text_14));
  strcat(Line, Event2str(Content));

  // stuur de regel naar Serial en/of naar Ethernet
  PrintLine(Line);

  // Indien er een SDCard gevonden is, dan loggen naar file
  if(SDCardPresent)
    LogSDCard(Line);
  } 
      

  
 /**********************************************************************************************\
 * Print een regel uit de Eventlist.
 \*********************************************************************************************/
void PrintEventlistEntry(int entry, byte d)
  {
  unsigned long Event, Action;
  String Line;
  
  Eventlist_Read(entry,&Event,&Action); // leesregel uit de Eventlist.    

  // Geef de entry van de eventlist weer
  Line=ProgmemString(Text_03);
    
  if(d>1)
    {
    Line+=d;
    Line+='.';
    }
  Line+=entry;

  // geef het event weer
  Line+=", ";
  Line+=ProgmemString(Text_14);
  Line+=Event2str(Event);

  // geef het action weer
  Line+=", ";
  Line+=ProgmemString(Text_16);
  Line+=Event2str(Action);
  PrintLine(Line);
  }
  
 /**********************************************************************************************\
 * Print actuele dag, datum, tijd.
 \*********************************************************************************************/
char* DateTimeString(void)
    {
    int x;
    static char dt[40];
    char s[5];
    
    for(x=0;x<=2;x++)
      s[x]=(*(ProgmemString(Text_04)+(Time.Day-1)*3+x));
    s[x]=0;

    sprintf(dt,"Date=%d-%02d-%02d, Time=%02d:%02d", Time.Year, Time.Month, Time.Date, Time.Hour, Time.Minutes);
    
    return dt;
    }

 /**********************************************************************************************\
 * Print de welkomsttekst van de Nodo.
 \*********************************************************************************************/
void PrintWelcome(void)
  {
  // Print Welkomsttekst
  PrintLine(ProgmemString(Text_22));
  PrintLine(ProgmemString(Text_01));
  PrintLine(ProgmemString(Text_02));

  // print versienummer
  sprintf(TempString,"Version= (Beta) %d.%d.%d, ThisUnit=%d",S.Version/100, (S.Version%100)/10, S.Version%10,S.Unit);
  PrintLine(TempString);

  // print IP adres van de Nodo
  sprintf(TempString,"%s=%u.%u.%u.%u, TerminalPort=%d, EventPort=%d",ProgmemString(Cmd_051),S.Nodo_IP[0],S.Nodo_IP[1],S.Nodo_IP[2],S.Nodo_IP[3], S.Terminal_Port, S.Event_Port);
  PrintLine(TempString);
  
  // geef melding als de SDCard goed geconnect is
  if(SDCardPresent)
    PrintLine(ProgmemString(Text_24));

  // Geef datum en tijd weer.
  if(Time.Day)
    {
    sprintf(TempString,"%s %s",DateTimeString(), cmd2str(Time.DaylightSaving?CMD_DLS_EVENT:0));
    PrintLine(TempString);
    }
    
  PrintLine(ProgmemString(Text_22));
  }


 /**********************************************************************************************\
 * Print de ethernet settings
 \*********************************************************************************************/
void PrintSettings(void)
  {
  int x;
  
  PrintLine(ProgmemString(Text_22));

  // Toon Password.
  sprintf(TempString,"%s=%s",ProgmemString(Cmd_057),S.Password);
  PrintLine(TempString);

  // Toon Nodo_IP adres
  sprintf(TempString,"%s=%u.%u.%u.%u",ProgmemString(Cmd_051),S.Nodo_IP[0],S.Nodo_IP[1],S.Nodo_IP[2],S.Nodo_IP[3]);
  PrintLine(TempString);
  
  // Toon Event  poort.
  sprintf(TempString,"%s=%u",ProgmemString(Cmd_055),S.Event_Port);
  PrintLine(TempString);

  // Toon Terminal poort.
  sprintf(TempString,"%s=%u",ProgmemString(Cmd_053),S.Terminal_Port);
  PrintLine(TempString);

  // toon de clients die eerder met succes verbonden zijn en nu als server worden beschouwd;
  for(x=0;x<SERVER_IP_MAX;x++)
    {
    if((S.Server_IP[x][0]+S.Server_IP[x][1]+S.Server_IP[x][2]+S.Server_IP[x][3])>0)
      {
      sprintf(TempString,"%s=%u.%u.%u.%u",ProgmemString(Text_10),S.Server_IP[x][0],S.Server_IP[x][1],S.Server_IP[x][2],S.Server_IP[x][3]);
      PrintLine(TempString);
      }
    }
  PrintLine(ProgmemString(Text_22));
  }


 /*********************************************************************************************\
 * print een lijst met de inhoud van de RawSignal buffer. ??? kan weg op termijn?
 \*********************************************************************************************/
void PrintRawSignal(void)
  {
  String Line;
  
  Line=ProgmemString(Text_07);
  for(byte x=1;x<=RawSignal[0];x++)
     {
     if(x>1)
       Line+=',';
     Line+=RawSignal[x];
     }
  PrintLine(Line);
  }


 /**********************************************************************************************\
 * Verzend teken(s) naar de Terminal
 \*********************************************************************************************/
void PrintLine(String LineToPrint)
  {
  if(SerialConnected)
    Serial.println(LineToPrint);
    
  if(S.Terminal_Enabled==VALUE_ON && TerminalConnected)
    {
    if(TerminalClient.connected())
      {
      TerminalClient.println(LineToPrint);
      }
    else
      {
      // als de client niet meer verbonden is, maar deze was dat wel,
      // dan de verbinding netjes afsluiten
      TerminalClient.flush();
      TerminalClient.stop();
      TerminalConnected=false;
      }
    }
  }


