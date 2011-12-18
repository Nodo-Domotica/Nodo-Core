#define XON                       0x11
#define XOFF                      0x13

void SerialHold(boolean x)
  {
  static boolean previous=true;
  
  if(x==previous)
    return;
  else
    {
    if(x)
      Serial.write(XOFF);
    else
      Serial.write(XON);
    previous=x;
    }
  }
  
 /*******************************************************************************************************\
 * Deze functie luistert naar de seriële poort en vult de globale array SerialBuffer[]
 * Als een complete regel ontvangen (afgesloten met CR / LF) is wordt true teruggeven.
 \*******************************************************************************************************/
int GetLineSerial(char *Buffer)
  {
  #define SERIAL_TIMEOUT 10000 
    
  if(!Serial.available())
    return false;

  unsigned long TimeoutTimer=millis()+SERIAL_TIMEOUT;
  int InByte;
  int Pos=0;  

  while(millis()<TimeoutTimer)
    {
    if(Serial.available())
      {
      InByte=Serial.read();
      if(isprint(InByte) && Pos<INPUT_BUFFER_SIZE) // alleen de printbare tekens zijn zinvol.
        {
        Buffer[Pos++]=InByte;
        TimeoutTimer=millis()+SERIAL_TIMEOUT;
        }
      else if(InByte=='\n') 
        {
        Buffer[Pos]=0; // serieel ontvangen regel is compleet
        return true; // Regel gereed
        }
      }
    }
  Buffer[Pos]=0; // serieel ontvangen regel is compleet
  return false; // er stonden geen tekens klaar in de serial buffer.
  }


 /**********************************************************************************************\
 * Deze routine leest een regel die van een willekeurige EventClient over IP binnenkomt.
 * Let op dat het aantal opgegeven tekens in Buffersie niet de werkelijk beschikbare ruimte
 * overschrijdt!
 *
 \*********************************************************************************************/

boolean EventGhostReceive(char *Buffer)
  {
  const int TimeOut=2000;
  unsigned long TimeoutTimer;
  int InByteIP,InByteIPCount=0;
  byte EGState=0;
  int x,y,z;
  char str_1[INPUT_BUFFER_SIZE],str_2[INPUT_BUFFER_SIZE];
  char Cookie[8];

  // Luister of er een EventClient is die verbinding wil maken met de Nodo EG EventServer
  TimeoutTimer=millis()+TimeOut;  
  EventClient = EventServer.available();
  if(EventClient) 
    {
    // we hebben een EventClient. Leg IP vast.
    EventClient.getRemoteIP(IP);

    //  sprintf(TempString,"Client=%u.%u.%u.%u",IP[0],IP[1],IP[2],IP[3]);
    // Serial.println("*** IP client verzoekt contact");//???

    // Kijk vervolgens of er data binnen komt.
    while((InByteIPCount<INPUT_BUFFER_SIZE) && TimeoutTimer>millis() )
      {
      if(EventClient.available())
        {
  //      Serial.println("*** Client.read");//???
        InByteIP=EventClient.read();

        if(InByteIP==0x0a)
          {
          Buffer[InByteIPCount]=0;          
          TimeoutTimer=millis()+TimeOut;  
  
          // regel is compleet. 
//          Serial.print("*** Ontvangen regel van IP client =");Serial.println(Buffer);//??? debug
        
          if(EGState==2)
            {
            // password uitwisseling via MD5 is gelukt en accept is verzonden
            // Nu kunnen de volgende regels voorbij komen:
            // - payload.....
            // - close
            // - <event>
      
            // Regels met "Payload" worden door de Bridge/Nodo niet gebruikt ->negeren.
            if(StringFind(Buffer,PROGMEM2str(Text_17))>=0)// payload
              {
              ; // negeren. Bridge doet niets met de payload functie.          
              }
            else if(strcasecmp(Buffer,PROGMEM2str(Text_19))==0) // "close"
              {
              // Regel "close", dan afsluiten van de communicatie met EventGhost
              strcpy(Buffer,str_1);
              return true;
              }
            else
              {
              // Event van EG ontvangen. Parkeer deze tijdelijk in str_1 omdat er nog een close ontvangen moet worden
              strcpy(str_1,Buffer);
              }
            }
  
          else if(EGState==1)
            {
//  Serial.println("*** EGState==1");//???
            // Cookie is verzonden en regel met de MD5 hash is ontvangen
            // Stel de string samen waar de MD5-hash aan de Nodo zijde voor gegenereerd moet worden
            strcpy(str_1,Cookie);
            strcat(str_1,":");
            strcat(str_1,S.Password);
  
//  Serial.println("*** String samengesteld voor berekenen MD5");//???
        
            // Bereken eigen MD5-Hash uit de string "<cookie>:<password>"                
            md5((struct md5_hash_t*)&MD5HashCode, str_1);  
  
//  Serial.println("*** MD5 hash berekend");//???
  
            strcpy(str_2,PROGMEM2str(Text_05));              
            y=0;
            for(x=0; x<16; x++)
              {
              str_1[y++]=str_2[MD5HashCode[x]>>4  ];
              str_1[y++]=str_2[MD5HashCode[x]&0x0f];
              }
            str_1[y]=0;
        
            // vergelijk hash-waarden en bevestig de EventClient bij akkoord
            if(strcasecmp(str_1,Buffer)==0)
              {
//  Serial.println("*** Ontvangen MD5 matched met berekende MD5");//???
              // MD5-hash code matched de we hebben een geverifiëerde EventClient
              strcpy(str_1,PROGMEM2str(Text_18));
              strcat(str_1,"\n");
              EventClient.print(str_1); // "accept"
//  Serial.println("*** Accept verzonden.");//???
  
              // Wachtwoord correct. Checken of het IP adres van de EventClient al bekend is.
              // zo niet dan opslaan in tabel met EventClients waar events naar toe moeten worden verzonden
              for(x=0;x<SERVER_IP_MAX;x++)
                {
                // Is de EventServer in de tabel gelijk aan de EventClient?
                if(IP[0]==S.Server_IP[x][0] && IP[1]==S.Server_IP[x][1] && IP[2]==S.Server_IP[x][2] && IP[3]==S.Server_IP[x][3])
                  break;
                }
    
              // als het adres niet voor komt in de EventServer tabel, dan deze opslaan in de eerste vrije plaats
              if(x>=SERVER_IP_MAX)
                {
                  // zoek eerste lege plaats op in de tabel
                for(y=0;y<SERVER_IP_MAX;y++)
                  {
                  // Is de plek in de tabel leeg?
                  if((S.Server_IP[y][0] + S.Server_IP[y][1] + S.Server_IP[y][2] + S.Server_IP[y][3])==0)
                    {
                    // Sla IP adres dan op in deze lege plaats
                    S.Server_IP[y][0]=IP[0];
                    S.Server_IP[y][1]=IP[1];
                    S.Server_IP[y][2]=IP[2];
                    S.Server_IP[y][3]=IP[3];
//???                    SaveSettings();
                    break;
                    }
                  }
                }
//  Serial.println("*** 1");//???
              }
            else
              {
//  Serial.println("*** Wachtwoord error.");//???
              RaiseError(ERROR_08);
              return false;
              }
            // volgende state, 
//  Serial.println("*** 2");//???
            EGState=2;                    
            }
            
          else if(EGState==0)
            {
//  Serial.println("*** EGState==0");//???
            EventClient.read(); // er kan nog een \r in de buffer zitten.
            
//  Serial.println("*** eventuele <cr> uitgelezen");//???
            // Kijk of de input een connect verzoek is vanuit EventGhost
            if(strcasecmp(InputBuffer,PROGMEM2str(Text_20))==0) // "quintessence" 
              { 
              // Een wachtwoord beveiligd verzoek vanuit een EventGhost EventClient (PC, Andoid, IPhone)
              // De EventClient is een EventGhost sender.  
              
//  Serial.println("*** quintessence ontvangen");//???
  
              // maak een 16-bits cookie en verzend deze
              strcpy(str_2,PROGMEM2str(Text_05));
              for(x=0;x<4;x++)
                Cookie[x]=str_2[int(random(0,0xf))];
              Cookie[x]=0; // sluit string af;
              strcpy(str_1,Cookie);
              strcat(str_1,"\n");
  
//  Serial.print("*** Cookie gemaakt=");Serial.println(str_1);//??? Debug
  
              EventClient.print(str_1);          
  
//  Serial.println("*** Cookie verzonden.");//???
  
              // ga naar volgende state: Haal MD5 en verwerk deze
              EGState=1;
              }
            }
          Buffer[0]=0;          
          InByteIPCount=0;          
          }
        if(isprint(InByteIP))
          {
          Buffer[InByteIPCount++]=InByteIP;
          }
        }
      }
    EventClient.flush();
    RaiseError(ERROR_07);
    return false;
    }
  else
    return false;
  }


 /*******************************************************************************************************\
 * Deze functie verzendt een regel als event naar een EventGhost EventServer. De Payload wordt niet
 * gebruikt en is leeg. Er wordt een false teruggegeven als de communicatie met de EventGhost EventServer
 * niet tot stand gebracht kon worden.
 \*******************************************************************************************************/
boolean EventGhostSend(char* event, byte* IP)
  {
  byte InByteIP;
  int  InByteIPCount=0;
  int x,y;
  byte EventGhostClientState=0; 
  char str1[80],str2[80];
  unsigned long Timeout=millis()+2000; //binnen deze tijd moet de gehele verzending gereed zijn, anders is er iets fout gegaan
  
  EthernetClient EGclient;  // (&IP[0], S.Event_Port);
  IPAddress server(IP[0],IP[1],IP[2],IP[3]);

//  sprintf(TempString,"%s=%u.%u.%u.%u",ProgmemString(Text_10),IP[0],IP[1],IP[2],IP[3]);//??? debugging
//  PrintLine(TempString);

  while(Timeout > millis())
    {
    if(EGclient.connect(server,S.Event_Port))
      {
      // verzend verzoek om verbinding met de EventGhost Server
      strcpy(str1,PROGMEM2str(Text_20));
      strcat(str1,"\n\r");
      EGclient.print(str1); //  "quintessence"
    
      // Haal de Cookie op van de server
      while(Timeout > millis()) 
        {
        if(EGclient.available())
          {
          InByteIP = EGclient.read();
          if(InByteIP)
            {
            if(InByteIPCount<INPUT_BUFFER_SIZE && InByteIP>=32 && InByteIP<=126)
              InputBuffer[InByteIPCount++]=InByteIP;// vul de string aan met het binnengekomen teken.

            // check op tekens die een regel afsluiten
            if(InByteIP==0x0a && InByteIPCount!=0) // als de ontvangen regel met een 0x0A wordt afgesloten, is er een lege regel. Deze niet verwerken.
              {
              InputBuffer[InByteIPCount]=0;
              InByteIPCount=0;

              // Over IP ontvangen regel is compleet 

              // volgende fase in ontvangstproces  
              // wacht op "accept"
              if(EventGhostClientState==1)
                {
                if(strcasecmp(InputBuffer,PROGMEM2str(Text_18))==0) // accept
                  {
                  // "accept" is ontvangen dus wachtwoord geaccepteerd
                  // Verbinding is geaccepteerd. schrijf weg ??? nog uitwerken
                  
                  // - payload.....
                  strcpy(str1,PROGMEM2str(Text_21)); // "Payload withoutRelease"
                  strcat(str1,"\n");
                  EGclient.print(str1);

                  // - <event>
                  strcat(event,"\n");
                  EGclient.print(event);

                  // - "close"
                  strcpy(str1,PROGMEM2str(Text_19)); 
                  strcat(str1,"\n");
                  EGclient.print(str1);

                  // klaar met verzenden en verbreek de verbinding;
                  EGclient.stop();    // close the connection:
                  return true;
                  }
                }

              if(EventGhostClientState==0)
                {
                // Cookie is door de bridge ontvangen en moet worden beantwoord met een MD5-hash
                // Stel de string samen waar de MD5 hash voor gegenereerd moet worden

                strcpy(str1,InputBuffer);
                strcat(str1,":");
                strcat(str1,S.Password);
 
                // Bereken MD5-Hash uit de string "<cookie>:<password>"                
                md5((struct md5_hash_t*)&MD5HashCode, str1);  
                strcpy(str2,PROGMEM2str(Text_05));              
                y=0;
                
                for(x=0; x<16; x++)
                  {
                  str1[y++]=str2[MD5HashCode[x]>>4  ];
                  str1[y++]=str2[MD5HashCode[x]&0x0f];
                  }
                str1[y++]=0x0a;
                str1[y]=0;

                // verzend hash-waarde
                EGclient.print(str1);
                EventGhostClientState=1;
                }
              }
            }
          }
        }
      }
    }
  return false;
  }
    
  
 /**********************************************************************************************\
 * Deze routine leest een regel die van een terminal client over IP binnenkomt.
 * Timeout is de tijd in milliseconden dat de functie moet wachten op afronding van de verzending
 * door de client. Is Timeout gelijk aan nul, dan is deze functie non-blocking
 * Let op dat het aantal opgegeven tekens in Buffersie niet de werkelijk beschikbare ruimte
 * overschrijdt!
 *
 \*********************************************************************************************/

boolean TerminalReceive(char *Buffer)
  {
  int InByteIP;
  static int InByteIPCount=0;
  
  // check even of ergens buiten deze funktie om de inputbuffer leeg is gemaak
  if(Buffer[0]==0)
    InByteIP=0;
  
  while(TerminalClient.available())
    {
    InByteIP=TerminalClient.read();
  
    if(InByteIP==0x0a || InByteIP==0x0d)
      {
      Buffer[InByteIPCount]=0;
      InByteIPCount=0;
      return true;
      }
    if(isprint(InByteIP))
      {
      if(S.Terminal_Prompt==VALUE_ON)
        TerminalClient.write(InByteIP); // Echo teken terug naar de terminal
      Buffer[InByteIPCount++]=InByteIP;
      }
    }
  return false;
  }

