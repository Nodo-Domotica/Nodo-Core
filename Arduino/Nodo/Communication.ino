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

boolean EventGhostReceive(char *ResultString)
  {
  const int TimeOut=1000;
  unsigned long TimeoutTimer;
  int InByteIP,InByteIPCount=0;
  byte EGState=0;
  int x,y,z;
  char str_2[INPUT_BUFFER_SIZE];
  char IPInputBuffer[INPUT_BUFFER_SIZE];//???
  char Cookie[8];
  
  // Luister of er een EventClient is die verbinding wil maken met de Nodo EG EventServer
  TimeoutTimer=millis()+TimeOut;  
  EthernetClient EventClient=EventServer.available();
  if(EventClient) 
    {
    // we hebben een EventClient. 
    
    // Kijk vervolgens of er data binnen komt.
    while((InByteIPCount<INPUT_BUFFER_SIZE) && TimeoutTimer>millis() )
      {
      if(EventClient.available())
        {
        InByteIP=EventClient.read();

        if(InByteIP==0x0a)
          {
          IPInputBuffer[InByteIPCount]=0;          
          TimeoutTimer=millis()+TimeOut;  
          // regel is compleet. 
        
          if(EGState==2)
            {
            // password uitwisseling via MD5 is gelukt en accept is verzonden
            // Nu kunnen de volgende regels voorbij komen:
            // - payload.....
            // - close
            // - <event>
      
            // Regels met "Payload" worden door de Bridge/Nodo niet gebruikt ->negeren.
            if(StringFind(IPInputBuffer,PROGMEM2str(Text_17))>=0)// payload
              {
              ; // negeren. Bridge doet niets met de payload functie.          
              }
            else if(strcasecmp(IPInputBuffer,PROGMEM2str(Text_19))==0) // "close"
              {
              // Regel "close", dan afsluiten van de communicatie met EventGhost

              EventClient.stop();
//??? debug
              if(TemporyEventGhostError)
                  Serial.println("*** debug: Tijdelijk opgeheven EventGhost communicatie hersteld.");//???
//??? debug
              TemporyEventGhostError=false; 
              return true;
              }
            else
              {
              // Event van EG ontvangen.
              strcpy(ResultString,IPInputBuffer);
              }
            }
  
          else if(EGState==1)
            {
            // Cookie is verzonden en regel met de MD5 hash is ontvangen
            // Stel de string samen waar de MD5-hash aan de Nodo zijde voor gegenereerd moet worden
            sprintf(TempString,"%s:%s",Cookie,S.Password);
        
            // Bereken eigen MD5-Hash uit de string "<cookie>:<password>"                
            md5((struct md5_hash_t*)&MD5HashCode, TempString); 
            strcpy(str_2,PROGMEM2str(Text_05));              
            y=0;
            for(x=0; x<16; x++)
              {
              TempString[y++]=str_2[MD5HashCode[x]>>4  ];
              TempString[y++]=str_2[MD5HashCode[x]&0x0f];
              }
            TempString[y]=0;
        
            // vergelijk hash-waarden en bevestig de EventClient bij akkoord
            if(strcasecmp(TempString,IPInputBuffer)==0)
              {
              // MD5-hash code matched de we hebben een geverifiëerde EventClient
              strcpy(TempString,PROGMEM2str(Text_18));
              strcat(TempString,"\n");
              EventClient.print(TempString); // "accept"

              // Wachtwoord correct. Bewaar IP adres indien nodig
              EventClient.getRemoteIP(EventClientIP);  
              if(S.AutoSaveEventGhostIP==VALUE_ON)
                {
                if( S.EventGhostServer_IP[0]!=EventClientIP[0] ||
                    S.EventGhostServer_IP[1]!=EventClientIP[1] ||
                    S.EventGhostServer_IP[2]!=EventClientIP[2] ||
                    S.EventGhostServer_IP[3]!=EventClientIP[3] )
                  {
                  S.EventGhostServer_IP[0]=EventClientIP[0];
                  S.EventGhostServer_IP[1]=EventClientIP[1];
                  S.EventGhostServer_IP[2]=EventClientIP[2];
                  S.EventGhostServer_IP[3]=EventClientIP[3];
                  SaveSettings();
                  }
                }
              }
            else
              {
              RaiseError(ERROR_08);
              return false;
              }
            // volgende state, 
            EGState=2;                    
            }
            
          else if(EGState==0)
            {
            EventClient.read(); // er kan nog een \r in de buffer zitten.

            // Kijk of de input een connect verzoek is vanuit EventGhost
            if(strcasecmp(IPInputBuffer,PROGMEM2str(Text_20))==0) // "quintessence" 
              { 
              // sprintf(TempString,"*** debug: EventGhost client maakt verbinding: %u.%u.%u.%u",EventClientIP[0],EventClientIP[1],EventClientIP[2],EventClientIP[3]);//??? Debug
              // PrintLine(TempString); //??? Debug

              // Een wachtwoord beveiligd verzoek vanuit een EventGhost EventClient (PC, Andoid, IPhone)
              // De EventClient is een EventGhost sender.  
              // maak een 16-bits cookie en verzend deze
              strcpy(str_2,PROGMEM2str(Text_05));
              for(x=0;x<4;x++)
                Cookie[x]=str_2[int(random(0,0xf))];
              Cookie[x]=0; // sluit string af;
              strcpy(TempString,Cookie);
              strcat(TempString,"\n");
              EventClient.print(TempString);          
  
              // ga naar volgende state: Haal MD5 en verwerk deze
              EGState=1;
              }
            }
          IPInputBuffer[0]=0;          
          InByteIPCount=0;          
          }
        if(isprint(InByteIP))
          {
          IPInputBuffer[InByteIPCount++]=InByteIP;
          }
        }
      }
    EventClient.flush();
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
boolean SendEventGhost(char* event, byte* SendToIP)
  {
  byte InByteIP;
  int  InByteIPCount=0;
  int x,y,Try;
  byte EventGhostClientState=0; 
  char str2[80];
  unsigned long Timeout;
  
  const int MaxBufferSize=80;//??? mag deze kleiner? 
  char IPInputBuffer[MaxBufferSize];
    
  EthernetClient EGclient;
  IPAddress EGserver(SendToIP[0],SendToIP[1],SendToIP[2],SendToIP[3]);

 // sprintf(TempString,"*** debug: SendEventGhost(): Verzenden event naar IP: %u.%u.%u.%u",SendToIP[0],SendToIP[1],SendToIP[2],SendToIP[3]);//??? debugging
 // Serial.println(TempString);

  Try=0;
  do
    {
    long Timeout=millis()+1000; //binnen deze tijd moet de gehele verzending gereed zijn, anders is er iets fout gegaan
    while(Timeout > millis())
      {
      if(EGclient.connect(EGserver,1024))
        {
        EGclient.flush();
  
        // verzend verzoek om verbinding met de EventGhost Server
        EGclient.print(F("quintessence\n\r")); //  
    
        // Haal de Cookie op van de server
        while(Timeout > millis()) 
          {
          if(EGclient.available())
            {
            InByteIP = EGclient.read();
            if(InByteIP)
              {
              if(InByteIPCount<MaxBufferSize && isprint(InByteIP))
                IPInputBuffer[InByteIPCount++]=InByteIP;// vul de string aan met het binnengekomen teken.
  
              // check op tekens die een regel afsluiten
              if(InByteIP==0x0a && InByteIPCount!=0) // als de ontvangen regel met een 0x0A wordt afgesloten, is er een lege regel. Deze niet verwerken.
                {
                IPInputBuffer[InByteIPCount]=0;
                InByteIPCount=0;

                // Serial.print("*** debug: SendEventGhost(): IPInputBuffer=");Serial.println(IPInputBuffer);//??? Debug
  
                // Over IP ontvangen regel is compleet 
                // volgende fase in ontvangstproces  
                // wacht op "accept"
                if(EventGhostClientState==1)
                  {
                  if(strcasecmp(IPInputBuffer,PROGMEM2str(Text_18))==0) // accept
                    {
                    // "accept" is ontvangen dus wachtwoord geaccepteerd
                    // Verbinding is geaccepteerd. schrijf weg ??? nog uitwerken
                    
                    // - payload.....
                    strcpy(TempString,PROGMEM2str(Text_21)); // "Payload withoutRelease"
                    strcat(TempString,"\n");
                    EGclient.print(TempString);

                    // - <event>
                    strcat(event,"\n");
                    EGclient.print(event);
  
                    // - "close"
                    strcpy(TempString,PROGMEM2str(Text_19)); 
                    strcat(TempString,"\n");
                    EGclient.print(TempString);
  
                    // klaar met verzenden en verbreek de verbinding;
                    EGclient.stop();    // close the connection:
                    return true;
                    }
                  }
  
                if(EventGhostClientState==0)
                  {
                  // Cookie is door de bridge ontvangen en moet worden beantwoord met een MD5-hash
                  // Stel de string samen waar de MD5 hash voor gegenereerd moet worden
  
                  strcpy(TempString,IPInputBuffer);
                  strcat(TempString,":");
                  strcat(TempString,S.Password);
   
                  // Bereken MD5-Hash uit de string "<cookie>:<password>"                
                  md5((struct md5_hash_t*)&MD5HashCode, TempString);  
                  strcpy(str2,PROGMEM2str(Text_05));              
                  y=0;
                  for(x=0; x<16; x++)
                    {
                    TempString[y++]=str2[MD5HashCode[x]>>4  ];
                    TempString[y++]=str2[MD5HashCode[x]&0x0f];
                    }
                  TempString[y++]=0x0a;
                  TempString[y]=0;
  
                  // verzend hash-waarde
                  EGclient.print(TempString);
                  EventGhostClientState=1;
                  }
                }
              }
            }
          }
        }
      }
    
    EGclient.stop();    // close the connection:
    EGclient.flush();    // close the connection:
    }while(++Try<5);
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
      Buffer[InByteIPCount++]=InByteIP;
      }
    }
  return false;
  }


 /*******************************************************************************************************\
 * Deze functie verzendt een regel als event naar een EventGhost EventServer. De Payload wordt niet
 * gebruikt en is leeg. Er wordt een false teruggegeven als de communicatie met de EventGhost EventServer
 * niet tot stand gebracht kon worden.
 \*******************************************************************************************************/
boolean HTTP_Request(char* event)
  {
  boolean error=0;
  int y=0;

  if(SDCardPresent)
    {
    // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer SDCard chip
    digitalWrite(Ethernetshield_CS_W5100, HIGH);
    digitalWrite(EthernetShield_CS_SDCard,LOW);      

    File dataFile=SD.open(ProgmemString(Text_29));
    if(dataFile) 
      {
      TempString[0]=0; // maak tijdelijke string leeg
      while(dataFile.available() && y<INPUT_BUFFER_SIZE)
          TempString[y++]=dataFile.read();
      TempString[y]=0; // sluit de string af
      dataFile.close();
      }
    else
      {
      S.TransmitHTTP=VALUE_OFF;
      error=ERROR_03;
      }

    // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer W5100 chip
    digitalWrite(Ethernetshield_CS_W5100, LOW);
    digitalWrite(EthernetShield_CS_SDCard,HIGH);
    }

  // TempString bevat nu de HTTP regel.
  
  if(!error)
    {
    strcat(TempString,event);
    Serial.print("*** debug: HTTP Request=");Serial.println(TempString);//??? Debug
    }
    
  if(error)
    {     
    RaiseError(error);
    return false;
    }
  return true;  
  }
