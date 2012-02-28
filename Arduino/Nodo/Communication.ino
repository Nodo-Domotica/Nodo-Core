#define IP_INPUT_BUFFER_SIZE      256
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
 * Deze functie verzendt een regel als event naar een EventGhost EventGhostServer. De Payload wordt niet
 * gebruikt en is leeg. Er wordt een false teruggegeven als de communicatie met de EventGhost EventGhostServer
 * niet tot stand gebracht kon worden.
 \*******************************************************************************************************/
boolean SendEventGhost(char* event, byte* SendToIP)
  {
  byte InByteIP;
  int  InByteIPCount=0;
  int x,y,Try;
  byte EventGhostClientState=0; 
  char str2[80];
  unsigned long Timeout=millis()+2000;
  
  const int MaxBufferSize=80;//??? mag deze kleiner? 
  char InputBuffer_IP[MaxBufferSize];
    
  IPAddress EGServerIP(SendToIP[0],SendToIP[1],SendToIP[2],SendToIP[3]);
  EthernetClient EGclient;

  Try=0;
  do
    {
    if(EGclient.connect(EGServerIP,S.Port))
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
              InputBuffer_IP[InByteIPCount++]=InByteIP;// vul de string aan met het binnengekomen teken.
  
            // check op tekens die een regel afsluiten
            if(InByteIP==0x0a && InByteIPCount!=0) // als de ontvangen regel met een 0x0A wordt afgesloten, is er een lege regel. Deze niet verwerken.
              {
              InputBuffer_IP[InByteIPCount]=0;
              InByteIPCount=0;
  
              // Serial.print("*** debug: SendEventGhost(): InputBuffer_IP=");Serial.println(InputBuffer_IP);//??? Debug
  
              // Over IP ontvangen regel is compleet 
              // volgende fase in ontvangstproces  
              // wacht op "accept"
              if(EventGhostClientState==1)
                {
                if(strcasecmp(InputBuffer_IP,PROGMEM2str(Text_18))==0) // accept
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
  
                strcpy(TempString,InputBuffer_IP);
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
    else
      {
      Serial.println();
      Serial.print(F("****** Error connecting EventGhost server.")); /// ??? Nog mooi weergeven en loggen.
      Serial.println(F("*******"));            
      }
    EGclient.stop();    // close the connection:
    EGclient.flush();    // close the connection:
    delay(250);
    }while(++Try<5);
  return false;
  }
    

 /*******************************************************************************************************\
 *
 *
 \*******************************************************************************************************/
boolean SendHTTPRequestResponse(char* Response)
  {
  boolean r;
  char* ResponseString=(char*)malloc(INPUT_BUFFER_SIZE+1);

  strcpy(ResponseString,"&response=");
  strcat(ResponseString,Response);
  r=xSendHTTPRequestStr(ResponseString);

  free(ResponseString);
  
  return r;
  }


 /*******************************************************************************************************\
 *
 *
 \*******************************************************************************************************/
boolean SendHTTPRequestEvent(unsigned long event)
  {
  boolean r;
  char* EventString=(char*)malloc(INPUT_BUFFER_SIZE+1);

  strcpy(EventString,"&unit=");
  strcat(EventString,int2str((event>>28)&0x0f));
  strcat(EventString,"&event=");
  strcat(EventString,Event2str(event));
  r=xSendHTTPRequestStr(EventString);

  free(EventString);
  return r;
  }


byte xSendHTTPRequestStr(char* StringToSend)
  {
  int InByteCounter;
  byte InByte,x;
  unsigned long TimeoutTimer;
  char s[2];
  const int TimeOut=10000;
  EthernetClient IPClient;                            // Client class voor HTTP sessie.
  char *IPBuffer=(char*)malloc(IP_INPUT_BUFFER_SIZE+1);
  char *TmpStr=(char*)malloc(INPUT_BUFFER_SIZE+1);
  
  byte State=0;// 0 als start, 
               // 1 als 200 OK voorbij is gekomen,
               // 2 als &file= is gevonden en eerstvolgende lege regel moet worden gedetecteerd
               // 3 als lege regel is gevonden en capture moet starten. 
               
  // Haal uit het HTTP request URL de Host. Alles tot aan het '/' teken.
  strcpy(TmpStr,S.HTTPRequest);
  x=StringFind(TmpStr,"/");
  TmpStr[x]=0;

  strcpy(IPBuffer,"GET ");
  strcat(IPBuffer,S.HTTPRequest+x);
  strcpy(TempString,"?id=");
  strcat(TempString,S.ID);
  strcat(TempString,"&password=");
  strcat(TempString,S.Password);
  strcat(TempString,"&version=");
  strcat(TempString,int2str(S.Version));
  strcat(TempString,StringToSend);

  // event toevoegen aan tijdelijke string, echter alle spaties vervangen door + conform URL notatie ??? nodig?  
  for(x=0;x<strlen(TempString);x++)
    {            
    if(TempString[x]==32)
      strcat(IPBuffer,"%20");
    else
      {
      s[0]=TempString[x];
      s[1]=0;
      strcat(IPBuffer,s);
      }
    }          
  strcat(IPBuffer," HTTP/1.1");
  
  if(S.Debug==VALUE_ON)
    PrintTerminal(IPBuffer);

  if(IPClient.connect(TmpStr,S.Port))
    {
    IPClient.getRemoteIP(ClientIPAddress);  
    IPClient.println(IPBuffer);

    strcpy(IPBuffer,"Host: ");
    strcat(IPBuffer,TmpStr);
    IPClient.println(IPBuffer);
    IPClient.println(F("Connection: Close"));
    IPClient.println();

    TimeoutTimer=millis()+TimeOut; // Als er twee seconden geen datatransport is, dan wordt aangenomen dat de verbinding (om wat voor reden dan ook) is afgebroken.
    IPBuffer[0]=0;
    InByteCounter=0;
    
    while(TimeoutTimer>millis() && IPClient.connected())
      {
      if(IPClient.available())
        {
        InByte=IPClient.read();
        if(S.Debug==VALUE_ON)
          Serial.write(InByte);

        if(isprint(InByte) && InByteCounter<INPUT_BUFFER_SIZE)
          IPBuffer[InByteCounter++]=InByte;
          
        else if(InByte==0x0A)
          {
          IPBuffer[InByteCounter]=0;
          TimeoutTimer=millis()+TimeOut; // er is nog data transport, dus de timeout timer weer op max. zetten.
          // De regel is binnen
  
          if(State==2 && InByteCounter==0) // als lege regel in HTTP request gevonden
            State=3;
            
          else if(State==3)
            AddFileSDCard(TmpStr,IPBuffer); // Extra logfile op verzoek van gebruiker   @1
          
          else if(State==0 && StringFind(IPBuffer,"HTTP")!=-1)
            {
            // Response n.a.v. HTTP-request is ontvangen
            if(StringFind(IPBuffer,"200")!=-1)
              {
              State=1;
              // pluk de filename uit het http request als die er is, dan de body text van het HTTP-request opslaan.
              if(ParseHTTPRequest(StringToSend,"file", TmpStr))
                {
                State=2;
                strcat(TmpStr,".dat");
                // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer SDCard chip
                digitalWrite(Ethernetshield_CS_W5100, HIGH);
                digitalWrite(EthernetShield_CS_SDCard,LOW);
                // evntueel vorig bestand wissen
                SD.remove(TmpStr);
                // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer W5100 chip
                digitalWrite(EthernetShield_CS_SDCard,HIGH);
                digitalWrite(Ethernetshield_CS_W5100, LOW);
                }
              }
            IPBuffer[InByteCounter]=0;
            }
          InByteCounter=0;          
          }
        }
      }
    }
  free(TmpStr);
  free(IPBuffer);
  IPClient.stop();
  return State;
  }
  
    
/*********************************************************************************************\
* Deze routine haalt uit een http request de waarden die bij de opgegeven parameter hoort
* Niet case-sinsitive.
\*********************************************************************************************/
boolean ParseHTTPRequest(char* HTTPRequest,char* Keyword, char* ResultString)
  {
  int x,y,z;
  int Keyword_len=strlen(Keyword);
  int HTTPRequest_len=strlen(HTTPRequest);

  ResultString[0]=0;
  
  if(HTTPRequest_len<5) // doe geen moeite als de string te weinig tekens heeft
    return -1;
  
  for(x=0; x<=(HTTPRequest_len-Keyword_len); x++)
    {
    y=0;
    while(y<Keyword_len && (tolower(HTTPRequest[x+y])==tolower(Keyword[y])))
      y++;

    z=x+y;
    if(y==Keyword_len && HTTPRequest[z]=='=' && (HTTPRequest[x-1]=='?' || HTTPRequest[x-1]=='&' || HTTPRequest[x-1]==' ')) // als tekst met een opvolgend '=' teken is gevonden
      {
      // Keyword gevonden. sla spaties en '=' teken over.
      
      //Test tekens voor Keyword
      while(z<HTTPRequest_len && (HTTPRequest[z]=='=' || HTTPRequest[z]==' '))z++;

      x=0; // we komen niet meer terug in de 'for'-loop, daarom kunnen we x hier even gebruiken.

      while(z<HTTPRequest_len && HTTPRequest[z]!='&' && HTTPRequest[z]!=' ')
        {
        if(HTTPRequest[z]=='+')
          ResultString[x]=' ';
        else if(HTTPRequest[z]=='%' && HTTPRequest[z+1]=='2' && HTTPRequest[z+2]=='0')
          {
          ResultString[x]=' ';
          z+=2;
          }
        else
          ResultString[x]=HTTPRequest[z];

        z++;
        x++;
        }
      ResultString[x]=0;
      return true;
      }
    }
  return false;
  }


 /**********************************************************************************************\
 *
 *
 *
 *
 \*********************************************************************************************/

void ExecuteIP(void)
  {
  char InByte;
  boolean RequestCompleted=false;  
  boolean Completed=false;
  int InByteCounter;
  byte Protocol=0;
  byte EGState=0;
  char Cookie[8];
  char FileName[15];
  boolean RequestEvent=false;
  boolean RequestFile=false;
  int x,y;
  unsigned long TimeoutTimer=millis()+2000; // Na twee seconden moet de gehele transactie gereed zijn, anders 'hik' in de lijn.

  // reserver een inputbuffer
  char *Event=(char*)malloc(INPUT_BUFFER_SIZE+1);
  char *InputBuffer_IP=(char*)malloc(IP_INPUT_BUFFER_SIZE+1);
  char *str_2=(char*)malloc(INPUT_BUFFER_SIZE+1);

  Event[0]=0; // maak de string leeg.
  
  EthernetClient IPClient = IPServer.available();
  
  if(IPClient)
    {
    InByteCounter=0;
    IPClient.getRemoteIP(ClientIPAddress);  
    
    while(IPClient.connected()  && !Completed && TimeoutTimer>millis())
      {
      if(IPClient.available()) 
        {
        InByte=IPClient.read();
        //Serial.write(InByte);//??? Debug
      
        if(isprint(InByte) && InByteCounter<IP_INPUT_BUFFER_SIZE)
          InputBuffer_IP[InByteCounter++]=InByte;
    
        else if((InByte==0x0D || InByte==0x0A))
          {
          InputBuffer_IP[InByteCounter]=0;
          InByteCounter=0;
          
          // Kijk wat voor soort protocol het is HTTP of APOP/EventGhost
          if(Protocol==0)
            {
            if(StringFind(InputBuffer_IP,"GET")!=-1)
              Protocol=VALUE_SOURCE_HTTP;// HTTP-Request

            if(StringFind(InputBuffer_IP,PROGMEM2str(Text_20))!=-1) // "quintessence"??
              Protocol=VALUE_SOURCE_EVENTGHOST;// EventGhost
            }
          
          if(Protocol==VALUE_SOURCE_HTTP)
            {
            if(!RequestCompleted)
              {
              Completed=true;
              if(S.Debug==VALUE_ON)
                {
                Serial.print(F("*** HTTP Request received="));
                Serial.println(InputBuffer_IP);
                }
              if(ParseHTTPRequest(InputBuffer_IP,"password",TempString))
                {
                if(strcmp(S.Password,TempString)!=0)
                  {
                  IPClient.println(F("HTTP/1.1 403 Forbidden"));
                  }
                else
                  {    
                  if(ParseHTTPRequest(InputBuffer_IP,"id",TempString))
                    {
                    if(strcmp(S.ID,TempString)!=0)
                      {
                      IPClient.println(F("HTTP/1.1 403 Forbidden"));
                      }
                    else
                      {
                      if(ParseHTTPRequest(InputBuffer_IP,"event",Event))
                        RequestEvent=true;
                       
                      if(ParseHTTPRequest(InputBuffer_IP,"file",TempString))
                        {
                        TempString[9]=0; // voorkom dat een file meer dan 8 posities heeft (en een afsluitende 0)
                        strcpy(FileName,TempString);
                        strcat(FileName,".dat");
                        RequestFile=true;
                        }
                        
                      if(ParseHTTPRequest(InputBuffer_IP,"confirm",TempString))
                        ConfirmHTTP=true;
                        
                      if(RequestFile || RequestEvent)
                        {
                        RequestCompleted=true;
                        strcpy(TempString,"HTTP/1.1 200 Ok"); //??? omzetten naar ProgMEM
                        IPClient.println(TempString);
                        }
                      else
                        IPClient.println(F("HTTP/1.1 400 Bad Request"));
                      }
                    }
                  }
                }
              }
            IPClient.println(F("Content-Type: text/html"));
            IPClient.print(F("Server: Nodo/"));
            IPClient.println(int2str(S.Version));             
            if(Time.Day)
              {
              IPClient.print(F("Date: "));
              IPClient.println(DateTimeString());             
              }
            IPClient.println(""); // HTTP Request wordt altijd afgesloten met een lege regel

            if(RequestFile)
              {              
              // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer SDCard chip
              digitalWrite(Ethernetshield_CS_W5100, HIGH);
              digitalWrite(EthernetShield_CS_SDCard,LOW);
              File dataFile=SD.open(FileName);
              if(dataFile) 
                {
                y=0;       
                while(dataFile.available())
                  {
                  x=dataFile.read();
                  if(isprint(x) && y<INPUT_BUFFER_SIZE)
                    {
                    TempString[y++]=x;
                    }
                  else
                    {
                    TempString[y]=0;
                    y=0;
                    digitalWrite(EthernetShield_CS_SDCard,HIGH);
                    digitalWrite(Ethernetshield_CS_W5100, LOW);
                    if(RequestFile)
                      {
                      IPClient.print(ProgmemString(Text_22));
                      IPClient.println("<br />");
                      RequestFile=false;// gebruiken we even als vlag om de eerste keer de regel met asteriks af te drukken omdat deze variabele toch verder niet meer nodig is
                      }
                    IPClient.print(TempString);
                    IPClient.println("<br />");
                    digitalWrite(Ethernetshield_CS_W5100, HIGH);
                    digitalWrite(EthernetShield_CS_SDCard,LOW);
                    }
                  }
                dataFile.close();
                digitalWrite(EthernetShield_CS_SDCard,HIGH);
                digitalWrite(Ethernetshield_CS_W5100, LOW);
                IPClient.println(ProgmemString(Text_22));
                }  
              else 
                IPClient.println(cmd2str(ERROR_03));
              }
            } // einde HTTP-request

          if(Protocol==VALUE_SOURCE_EVENTGHOST) // EventGhost
            {
            if(EGState==2)
              {
              // password uitwisseling via MD5 is gelukt en accept is verzonden
              // Nu kunnen de volgende regels voorbij komen:
              // - payload.....
              // - close
              // - <event>
        
              // Regels met "Payload" worden door de Bridge/Nodo niet gebruikt ->negeren.
              if(StringFind(InputBuffer_IP,PROGMEM2str(Text_17))>=0)// payload
                {
                ; // negeren. Bridge doet niets met de payload functie.          
                }
              else if(strcasecmp(InputBuffer_IP,PROGMEM2str(Text_19))==0) // "close"
                {
                // Regel "close", dan afsluiten van de communicatie met EventGhost  
                IPClient.stop();
                TemporyEventGhostError=false; 
                Completed=true;
                break;
                }
              else
                {
                // Event van EG ontvangen.
                strcpy(Event,InputBuffer_IP);
                RequestEvent=true;
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
          
              // vergelijk hash-waarden en bevestig de EventGhostClient bij akkoord
              if(strcasecmp(TempString,InputBuffer_IP)==0)
                {
                // MD5-hash code matched de we hebben een geverifiëerde EventGhostClient
                strcpy(TempString,PROGMEM2str(Text_18));
                strcat(TempString,"\n");
                IPClient.print(TempString); // "accept"

                // Wachtwoord correct. Bewaar IP adres indien nodig
                if(S.AutoSaveEventGhostIP==VALUE_AUTO)
                  {
                  if( S.EventGhostServer_IP[0]!=ClientIPAddress[0] ||
                      S.EventGhostServer_IP[1]!=ClientIPAddress[1] ||
                      S.EventGhostServer_IP[2]!=ClientIPAddress[2] ||
                      S.EventGhostServer_IP[3]!=ClientIPAddress[3] )
                    {
                    S.EventGhostServer_IP[0]=ClientIPAddress[0];
                    S.EventGhostServer_IP[1]=ClientIPAddress[1];
                    S.EventGhostServer_IP[2]=ClientIPAddress[2];
                    S.EventGhostServer_IP[3]=ClientIPAddress[3];
                    SaveSettings();
                    if(S.Debug)
                      Serial.print("*** debug: EventGhostServer saved.");
                    }
                  }
                }
              else
                {
                Completed=true;
                Protocol=0;
                RaiseError(ERROR_08);
                break;
                }
              // volgende state, 
              EGState=2;                    
              }
              
            else if(EGState==0)
              {
              IPClient.read(); // er kan nog een \r in de buffer zitten.
  
              // Kijk of de input een connect verzoek is vanuit EventGhost
              if(strcasecmp(InputBuffer_IP,PROGMEM2str(Text_20))==0) // "quintessence" 
                {   
                // Een wachtwoord beveiligd verzoek vanuit een EventGhost EventGhostClient (PC, Andoid, IPhone)
                // De EventGhostClient is een EventGhost sender.  
                // maak een 16-bits cookie en verzend deze
                strcpy(str_2,PROGMEM2str(Text_05));
                for(x=0;x<4;x++)
                  Cookie[x]=str_2[int(random(0,0xf))];
                Cookie[x]=0; // sluit string af;
                strcpy(TempString,Cookie);
                strcat(TempString,"\n");
                IPClient.print(TempString);          
    
                // ga naar volgende state: Haal MD5 en verwerk deze
                EGState=1;
                }
              }
            } // einde InputType==EVENTGHOST              
          InputBuffer_IP[0]=0;          //???
          }
        }
      }
    }
  delay(1);  // give the web browser time to receive the data
  IPClient.stop();
  free(str_2);
  free(InputBuffer_IP);

  if(RequestEvent)
    ExecuteLine(Event, Protocol);
  ConfirmHTTP=false; // geen monitor weergave meer als HTTP-request versturen.
  free(Event);
  return;
  }

