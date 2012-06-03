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
  unsigned long Timeout=millis()+5000;

  char* InputBuffer_IP=(char*)malloc(INPUT_BUFFER_SIZE+1);

  IPAddress EGServerIP(SendToIP[0],SendToIP[1],SendToIP[2],SendToIP[3]);
  EthernetClient EGclient;

  Try=0;
  do
    {
    if(EGclient.connect(EGServerIP,S.PortClient))
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
            if(InByteIPCount<INPUT_BUFFER_SIZE && isprint(InByteIP))
              InputBuffer_IP[InByteIPCount++]=InByteIP;// vul de string aan met het binnengekomen teken.
  
            // check op tekens die een regel afsluiten
            if(InByteIP==0x0a && InByteIPCount!=0) // als de ontvangen regel met een 0x0A wordt afgesloten, is er een lege regel. Deze niet verwerken.
              {
              InputBuffer_IP[InByteIPCount]=0;
              InByteIPCount=0;
  
              // Over IP ontvangen regel is compleet 
              // volgende fase in ontvangstproces  
              // wacht op "accept"
              if(EventGhostClientState==1)
                {
                if(strcasecmp(InputBuffer_IP,PROGMEM2str(Text_18))==0) // accept
                  {
                  // "accept" is ontvangen dus wachtwoord geaccepteerd
                  
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

                  free(InputBuffer_IP);
                  return true;
                  }
                }
  
              if(EventGhostClientState==0)
                {
                // Cookie is door de Nodo ontvangen en moet worden beantwoord met een MD5-hash
                // Stel de string samen waar de MD5 hash voor gegenereerd moet worden
                // Bereken MD5-Hash uit de string "<cookie>:<password>" en verzend deze
  
                strcpy(TempString,InputBuffer_IP);
                strcat(TempString,":");
                strcat(TempString,S.Password);   
                md5(TempString);  
                EGclient.print(TempString);
                EventGhostClientState=1;
                }
              }
            }
          }
        }
      }
    else
      RaiseError(ERROR_07);

    EGclient.stop();    // close the connection:
    EGclient.flush();    // close the connection:
    delay(250);
    }while(++Try<5);

  free(InputBuffer_IP);
  return false;
  }
 

 /*******************************************************************************************************\
 * Haal via een HTTP-request een file op
 * De content van de file bevindt zich in de body text die de server terugstuurt en wordt opgevangen
 * in de funktie SendHTTPRequest()
 \*******************************************************************************************************/
byte GetHTTPFile(char* filename)
  {
  char *HttpRequest=(char*)malloc(INPUT_BUFFER_SIZE+1);
  byte Ok;
  
  strcpy(HttpRequest,"?id=");
  strcat(HttpRequest,S.ID);  

  strcat(HttpRequest,"&file=");
  strcat(HttpRequest,filename);

  if(S.HTTP_Pin==VALUE_ON)
    {
    // pin-code genereren en meesturen in het http-request
    sprintf(TempString,"%s:%s",HTTPCookie,S.Password);  
    md5(TempString);
    strcat(HttpRequest,"&key=");
    strcat(HttpRequest,TempString);    
    }
    
  Ok=SendHTTPRequest(HttpRequest);
  free(HttpRequest);
  return Ok;
  }

 /*******************************************************************************************************\
 * Verzend een event als HTTP-request 
 *
 \*******************************************************************************************************/
byte SendHTTPEvent(unsigned long event)
  {
  byte Unit,x;
  char *HttpRequest=(char*)malloc(INPUT_BUFFER_SIZE+1);

  strcpy(HttpRequest,"?id=");
  strcat(HttpRequest,S.ID);  

  strcat(HttpRequest,"&unit=");
  if(((event>>28)&0xf)==((unsigned long)(SIGNAL_TYPE_UNKNOWN)))
    Unit=0;
  else
    Unit=(event>>24)&0x0f;
  strcat(HttpRequest,int2str(Unit));  
  
  if(S.HTTP_Pin==VALUE_ON)
    {
    // pin-code genereren en meesturen in het http-request
    sprintf(TempString,"%s:%s",HTTPCookie,S.Password);  
    md5(TempString);
    strcat(HttpRequest,"&key=");
    strcat(HttpRequest,TempString);    
    }
    
  strcat(HttpRequest,"&event=");
  strcat(HttpRequest,Event2str(event));

  x=SendHTTPRequest(HttpRequest);
  free(HttpRequest);
  return x;
  }


 /*******************************************************************************************************\
 * Verzend een nieuwe cookie als HTTP request.
 *
 \*******************************************************************************************************/
boolean SendHTTPCookie(void)
  {
  boolean Status;

  char *HttpRequest=(char*)malloc(INPUT_BUFFER_SIZE+1);

  strcpy(HttpRequest,"?id=");
  strcat(HttpRequest,S.ID);  

  // Verzend tevens een nieuwe cookie voor het eerstvolgende event.
  RandomCookie(HTTPCookie);
  strcat(HttpRequest,"&cookie=");
  strcat(HttpRequest,HTTPCookie);
  
  Status=SendHTTPRequest(HttpRequest);
  free(HttpRequest);
  return Status;
  }


boolean SendHTTPRequest(char* Request)
  {
  int InByteCounter,x,y,SlashPos;
  byte InByte;
  unsigned long TimeoutTimer;
  char* IPBuffer=(char*)malloc(IP_INPUT_BUFFER_SIZE+1);
  char filename[13];
  const int TimeOut=10000;
  EthernetClient IPClient;                            // Client class voor HTTP sessie.
  byte State=0;// 0 als start, 
               // 1 als 200 OK voorbij is gekomen,
               // 2 als &file= is gevonden en eerstvolgende lege regel moet worden gedetecteerd
               // 3 als lege regel is gevonden en file-capture moet starten.                


  strcpy(IPBuffer,"GET ");

  // Haal uit het HTTP request URL de Host. 
  // zoek naar de eerste slash in de opgegeven HTTP-Host adres
  SlashPos=StringFind(S.HTTPRequest,"/");
  if(SlashPos!=-1)
    strcat(IPBuffer,S.HTTPRequest+SlashPos);

  // Alle spaties omzetten naar %20 en toevoegen aan de te verzenden regel.
  y=strlen(IPBuffer);
  
  for(x=0;x<strlen(Request);x++)
    {            
    if(Request[x]==32)
      {
      IPBuffer[y++]='%';
      IPBuffer[y++]='2';
      IPBuffer[y++]='0';
      }
    else
      {
      IPBuffer[y++]=Request[x];
      }
    }
  IPBuffer[y]=0;

  // Sluit HTTP-request af met protocol versienummer
  strcat(IPBuffer," HTTP/1.1");

  // IPBuffer bevat nu het volledige HTTP-request, gereed voor verzending.

  if(S.Debug==VALUE_ON)
    {
    strcpy(TempString,"# HTTP Output: ");
    strcat(TempString,IPBuffer);
    Serial.println(TempString);
    }

  strcpy(TempString,S.HTTPRequest);
  TempString[SlashPos]=0;

  if(IPClient.connect(TempString,S.PortClient))
    {
    IPClient.getRemoteIP(ClientIPAddress);  
    IPClient.println(IPBuffer);
    strcpy(IPBuffer,"Host: ");
    strcat(IPBuffer,TempString);
    IPClient.println(IPBuffer);
    strcpy(IPBuffer,"User-Agent: Nodo/");
    strcat(IPBuffer,int2str(S.Version));
    IPClient.println(IPBuffer);
    IPClient.println(F("Connection: Close"));    
    IPClient.println();// Afsluiten met een lege regel is verplicht in http protocol/

    TimeoutTimer=millis()+TimeOut; // Als er twee seconden geen datatransport is, dan wordt aangenomen dat de verbinding (om wat voor reden dan ook) is afgebroken.
    IPBuffer[0]=0;
    InByteCounter=0;
    
    while(TimeoutTimer>millis() && IPClient.connected())
      {
      if(IPClient.available())
        {
        InByte=IPClient.read();

        if(isprint(InByte) && InByteCounter<INPUT_BUFFER_SIZE)
          IPBuffer[InByteCounter++]=InByte;
          
        else if(InByte==0x0A)
          {
          IPBuffer[InByteCounter]=0;
          if(S.Debug==VALUE_ON)
            {
            strcpy(TempString,"# HTTP Input: ");
            strcat(TempString,IPBuffer);
            Serial.println(TempString);
            }

          TimeoutTimer=millis()+TimeOut; // er is nog data transport, dus de timeout timer weer op max. zetten.
          // De regel is binnen
  
          if(State==2 && InByteCounter==0) // als lege regel in HTTP request gevonden
            State=3;
            
          else if(State==3)
            AddFileSDCard(filename,IPBuffer); // Capture de bodytext uit het HTTP-request en sla regels op in opgegeven filename
          
          else if(State==0 && StringFind(IPBuffer,"HTTP")!=-1)
            {
            // Response n.a.v. HTTP-request is ontvangen
            if(StringFind(IPBuffer,"200")!=-1)
              {
              State=1;
              // pluk de filename uit het http request als die er is, dan de body text van het HTTP-request opslaan.
              if(ParseHTTPRequest(Request,"file", TempString))
                {
                State=2;
                TempString[8]=0; // voorkom dat filenaam meer dan acht posities heeft
                strcpy(filename,TempString);                
                strcat(filename,".dat");
                // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer SDCard chip
                digitalWrite(Ethernetshield_CS_W5100, HIGH);
                digitalWrite(EthernetShield_CS_SDCard,LOW);
                // evntueel vorig bestand wissen
                SD.remove(filename);
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
    IPClient.stop();
    }
  else
    State=false;

  free(IPBuffer);
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
  char FileName[13];
  boolean RequestEvent=false;
  boolean RequestFile=false;
  int x,y;
  unsigned long TimeoutTimer=millis()+5000; // Na twee seconden moet de gehele transactie gereed zijn, anders 'hik' in de lijn.
  char EGCookie[10];
  
  char *Event=(char*)malloc(INPUT_BUFFER_SIZE+1);
  char *InputBuffer_IP=(char*)malloc(IP_INPUT_BUFFER_SIZE+1);
  char *TmpStr2=(char*)malloc(INPUT_BUFFER_SIZE+1);

  Event[0]=0; // maak de string leeg.
  
  EthernetClient IPClient = IPServer.available();
  
  if(IPClient)
    {
    IPClient.getRemoteIP(ClientIPAddress);  

    // Controleer of het IP adres van de Client geldig is. 
    if((S.Client_IP[0]!=0 && ClientIPAddress[0]!=S.Client_IP[0]) ||
       (S.Client_IP[1]!=0 && ClientIPAddress[1]!=S.Client_IP[1]) ||
       (S.Client_IP[2]!=0 && ClientIPAddress[2]!=S.Client_IP[2]) ||
       (S.Client_IP[3]!=0 && ClientIPAddress[3]!=S.Client_IP[3]))
      {
      RaiseError(ERROR_10);
      }
    else
      {
      InByteCounter=0;
      while(IPClient.connected()  && !Completed && TimeoutTimer>millis())
        {
        if(IPClient.available()) 
          {
          InByte=IPClient.read();
          
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
                  strcpy(TempString,"# HTTP Input: ");
                  strcat(TempString,InputBuffer_IP);
                  Serial.println(TempString);
                  }
                
                // als de beveiliging aan staat, dan kijken of de juiste pin ip meegegeven in het http-request. x is vlag voor toestemming verwerking event
                x=false;
                if(S.HTTP_Pin==VALUE_ON)
                  {
                  sprintf(TmpStr2,"%s:%s",HTTPCookie,S.Password);  
                  md5(TmpStr2);
                  
                  if(ParseHTTPRequest(InputBuffer_IP,"key",TempString))
                    {
                    if(strcmp(TmpStr2,TempString)==0)
                      x=true;
                    }
                  }
                else
                  x=true;

                if(x)
                  {                
                  if(ParseHTTPRequest(InputBuffer_IP,"event",Event))
                    RequestEvent=true;
                   
                  if(ParseHTTPRequest(InputBuffer_IP,"file",TempString))
                    {
                    TempString[8]=0; // voorkom dat een file meer dan 8 posities heeft (en een afsluitende 0)
                    strcpy(FileName,TempString);
                    strcat(FileName,".dat");
                    RequestFile=true;
                    }
                    
                  if(RequestFile || RequestEvent)
                    {
                    RequestCompleted=true;
                    strcpy(TempString,"HTTP/1.1 200 Ok");
                    IPClient.println(TempString);
                    }
                  else
                    IPClient.println(F("HTTP/1.1 400 Bad Request"));
                  }
                else                    
                  IPClient.println(F("HTTP/1.1 403 Forbidden"));
                }

              IPClient.println(F("Content-Type: text/html"));
              IPClient.print(F("Server: Nodo/"));
              IPClient.println(int2str(S.Version));             
              if(bitRead(HW_Config,HW_CLOCK))
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
                // Bereken eigen MD5-Hash uit de string "<cookie>:<password>"                
                sprintf(TempString,"%s:%s",EGCookie,S.Password);            
                md5(TempString); 
            
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
                  // maak een cookie en verzend deze
                  RandomCookie(EGCookie);
                  IPClient.print(EGCookie);          
      
                  // ga naar volgende state: Haal MD5 en verwerk deze
                  EGState=1;
                  }
                }
              } // einde InputType==EVENTGHOST              
            InputBuffer_IP[0]=0;
            }
          }
        }
      }
    }

  delay(5);  // korte pauze om te voorkomen dat de verbinding wordt verbroken alvorens alle data door client verwerkt is.
  IPClient.stop();

  if(RequestEvent)
    ExecuteLine(Event, Protocol);
  ConfirmHTTP=false; // geen monitor weergave meer als HTTP-request versturen.

  free(TmpStr2);
  free(InputBuffer_IP);
  free(Event);
  return;
  }
  

 /*********************************************************************************************\
 * Verzend via RF een commando regel naar een andere Nodo.
 * Line = string met commando(s)
 * Dest = unitnummer waar het commando naar verzonden moet worden.
 * Deze routine maakt tevens gebruik van de WaitFreeRF routine.
 \*********************************************************************************************/
boolean SendLineRF(char* Line, byte Dest)
  {
  int Length=strlen(Line);
  byte Checksum=0;
  int x,y;
  unsigned long Event;
  unsigned long TimeoutTimer;
  unsigned int Cookie=0;
  boolean Ok=false;
  
  // Protocol:
  // Stap-1. Master Nodo stuurt ReceiveLine verzoek naar slave om klaar te staan voor ontvangst: Bestemming-Unit, checksum en stringlengte (regulier Nodo commando)
  // Stap-2. Slave beantwoordt ter bevestiging een stuurt 16-bits key in Par1 en Par2.  (regulier Nodo commando)
  // Stap-3. Master verzendt blok met: String met commando en MD5-Hash op basis van wachtwoord en cookie (Speciale codering afwijkend van reguliere commando)
  // Stap-4. Slave beantwoord met OK of ERROR. (regulier Nodo commando)
  // MD5-Hash wordt berekend a.d.h.v. Lengte string, Checksum string, Password en Cookie.  Klopt 1 van de 4 niet, dan voert slave regel niet uit.
  
  // bereken de checksum en voeg toe aan de string
  for(x=0;x<Length;x++)
    Checksum=(Checksum + Line[x])%256; // bereken checksum (crc-8)
      
  // Stap-1. Master Nodo stuurt ReceiveLine verzoek naar slave om klaar te staan voor ontvangst: Bestemming-Unit, checksum en stringlengte (regulier Nodo commando)
  Event=  ((unsigned long)SIGNAL_TYPE_NODO)<<28   | 
          ((unsigned long)Dest)<<24               | 
          ((unsigned long)CMD_RECEIVE_LINE)<<16   | 
          ((unsigned long)Checksum)<<8            | 
           (unsigned long)Length;

  Nodo_2_RawSignal(Event);  
  RawSendRF();

  // Stap-2: Wacht tot Slave beantwoord heeft met een CMD_RECEIVE_LINE_READY met een Cookie
  TimeoutTimer=millis()+3000; 
  while(TimeoutTimer>millis() && !Cookie)
    {
    if((*portInputRegister(RFport)&RFbit)==RFbit)// Kijk if er iets op de RF poort binnenkomt. (Pin=HOOG als signaal in de ether). 
      {
      if(FetchSignal(PIN_RF_RX_DATA,HIGH,SIGNAL_TIMEOUT_RF))// Als het een duidelijk RF signaal was
        {
        Event=AnalyzeRawSignal(); // Bereken uit de tabel met de pulstijden de 32-bit code. 
        if((Event&0xffff0000)==(((unsigned long)SIGNAL_TYPE_NODO)<<28 | ((unsigned long)Dest)<<24 | ((unsigned long)CMD_RECEIVE_LINE_READY)<<16))
          Cookie=Event&0x0000ffff;
        }
      }
    }

  if(millis()<TimeoutTimer)
    {
    // Stap-3. Master verzendt blok met: String met commando. (Speciale codering afwijkend van reguliere commando)

    // Encrypt het signaal met de MD5-Hash
    sprintf(TempString,"%d:%s",Cookie,S.Password);
    md5(TempString); 
    y=0;
    for(x=0; x<Length; x++)
      {
      Line[x]=Line[x] ^ MD5HashCode[y++];
      if(y>15)y=0;
      }

    // Verzend de string
    digitalWrite(PIN_RF_RX_VCC,LOW);  // Spanning naar de RF ontvanger uit om interferentie met de zender te voorkomen.
    digitalWrite(PIN_RF_TX_VCC,HIGH); // zet de 433Mhz zender aan
    noInterrupts();

    // Aanloopsignaal houdt RF band bezet en geeft slave mogelijkheid om gereed te gaan staan
    // De ontvangers hebben ook even de tijd nodig om na inschakelen van de voedspanning even stabiel te worden.
    // Tijdsduur emperisch bepaald.
    for(x=0;x<500;x++)
      {
      digitalWrite(PIN_RF_TX_DATA,HIGH);
      delayMicroseconds(NODO_SPACE); 
      digitalWrite(PIN_RF_TX_DATA,LOW);
      delayMicroseconds(NODO_SPACE); 
      }

    // Verzend startbit
    digitalWrite(PIN_RF_TX_DATA,HIGH);
    delayMicroseconds(NODO_PULSE_1*2); 
    digitalWrite(PIN_RF_TX_DATA,LOW);
    delayMicroseconds(NODO_SPACE*2);
 
    for(x=0;x<Length;x++)
      {    
      for(y=0;y<=7;y++)
        {
        if(Line[x]&(1<<y))
          {
          digitalWrite(PIN_RF_TX_DATA,HIGH); // 1
          delayMicroseconds(NODO_PULSE_1); 
          digitalWrite(PIN_RF_TX_DATA,LOW);
          delayMicroseconds(NODO_SPACE); 
          }
        else
          {
          digitalWrite(PIN_RF_TX_DATA,HIGH); // 0
          delayMicroseconds(NODO_PULSE_0); 
          digitalWrite(PIN_RF_TX_DATA,LOW);
          delayMicroseconds(NODO_SPACE); 
          }
        }
      }
    digitalWrite(PIN_RF_TX_VCC,LOW); // zet de 433Mhz zender weer uit
    digitalWrite(PIN_RF_RX_VCC,HIGH); // Spanning naar de RF ontvanger weer aan.
    interrupts();
    Ok=true;
    }
  return Ok;
  }


 /*********************************************************************************************\
 * Verzend via RF een commando regel naar een andere Nodo.
 * Line = string met commando(s)
 * Dest = unitnummer waar het commando naar verzonden moet worden.
 * Deze routine maakt tevens gebruik van de WaitFreeRF routine.
 \*********************************************************************************************/
boolean ReceiveLineRF(byte Checksum, byte Length, char *ReturnString)
  {
  unsigned long Event,TimeoutTimer;
  unsigned int Cookie;
  byte ReceivedByte, ReceivedByteCounter;  
  int PulseTime,x,y;
  boolean Ok=false;
  char *TmpStr=(char*)malloc(INPUT_BUFFER_SIZE+1);
  
  // Stap-1. Deze is reeds uitgevoerd door de Master. ReceiveLine verzoek.

  // Stap-2. Slave beantwoordt ter bevestiging een 16-bits random Cookie in Par1 en Par2.  (regulier Nodo commando)
  randomSeed(millis());
  Cookie=random(0xFFFF);
  Event=  ((unsigned long)SIGNAL_TYPE_NODO)<<28         | 
          ((unsigned long)S.Unit)<<24                   | 
          ((unsigned long)CMD_RECEIVE_LINE_READY)<<16   | 
          (unsigned long)Cookie;
          
  WaitFreeRF(0, 100);
  Nodo_2_RawSignal(Event);  
  RawSendRF();

  // Stap-3. Ontvang van de Master verzendt blok met: String met commando en MD5-Hash op basis van wachtwoord en cookie
  ReturnString[0]=0; // Maak de ontvangen string leeg;
  TimeoutTimer=millis()+2000;
  ReceivedByteCounter=0;
  ReceivedByte=0;
  x=0;
  
  while(TimeoutTimer>millis())
    {
    // hier aangekomen als er een puls (startbit) is gestart en ingang dus hoog is.
    PulseTime=WaitForChangeState(PIN_RF_RX_DATA, HIGH, SIGNAL_TIMEOUT_RF);
    if(PulseTime>2500 && PulseTime<3500)// meet hoe lang het duurt totdat de RF uitgang weer LOW wordt. als de puls een duidelijke startbit is
      {
      while(TimeoutTimer>millis())
        {
        PulseTime = WaitForChangeState(PIN_RF_RX_DATA,HIGH, SIGNAL_TIMEOUT_RF);
        if(PulseTime>MIN_PULSE_LENGTH)
          {        
          if(PulseTime>NODO_PULSE_MID)
            ReceivedByte |= (1<<x);

          if(++x>7)
            {
            ReturnString[ReceivedByteCounter++]=ReceivedByte;
            ReceivedByte=0;
            x=0;
            if(ReceivedByteCounter==Length)
              {
              ReturnString[ReceivedByteCounter]=0; // sluit string af.
              TimeoutTimer=0L;// stop de while() loop.
                          
              // Decrypt het signaal met de MD5-Hash
              sprintf(TmpStr,"%d:%s",Cookie,S.Password);
              md5(TmpStr); 
              y=0;
              for(x=0; x<Length; x++)
                {
                ReturnString[x]=ReturnString[x] ^ MD5HashCode[y++];
                if(y>15)y=0;
                }
          
              // bereken de checksum van de ontvangen string.
              y=0;
              for(x=0;x<strlen(ReturnString);x++)
                y=(y + ReturnString[x])%256; // bereken checksum (crc-8)

              if(y==Checksum)
                Ok=true;
              }
            }
          }
        }
      }
    }  
  free(TmpStr);
  return Ok;    
  }

