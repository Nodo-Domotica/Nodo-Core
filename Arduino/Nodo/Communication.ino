#define IP_BUFFER_SIZE            256
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
  
#ifdef NODO_MEGA
boolean EthernetInit(void)
  {
  int x;
  boolean Ok=false;

  if(!bitRead(HW_Config,HW_ETHERNET))
    return false;

  // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer W5100 chip
  SelectSD(false);
  
  // Initialiseer ethernet device
  if((Settings.Nodo_IP[0] + Settings.Nodo_IP[1] + Settings.Nodo_IP[2] + Settings.Nodo_IP[3])==0)// Als door de user IP adres is ingesteld op 0.0.0.0 dan IP adres ophalen via DHCP
    {
    if(Ethernet.begin(Ethernet_MAC_Address)==0) // maak verbinding en verzoek IP via DHCP
      Serial.println(F("Error: Failed to configure Ethernet using DHCP"));
    }
  else      
    {
    Ethernet.begin(Ethernet_MAC_Address, Settings.Nodo_IP, Settings.DnsServer, Settings.Gateway, Settings.Subnet);
    }
  if((Ethernet.localIP()[0]+Ethernet.localIP()[1]+Ethernet.localIP()[2]+Ethernet.localIP()[3])!=0); // Als er een IP adres is, dan HTTP en TelNet servers inschakelen
    {
    // Start server voor Terminalsessies via TelNet
    TerminalServer=EthernetServer(23);
    TerminalServer.begin(); 

    // Start Server voor ontvangst van HTTP-Events
    HTTPServer=EthernetServer(Settings.HTTPServerPort);
    HTTPServer.begin(); 
    
    if(Settings.TransmitIP==VALUE_ON)
      {
      // Haal IP adres op van de Host waar de nodo de HTTP events naar verzendt zodat niet iedere transactie een DNS-resolve plaats hoeft te vinden.
      // Haal uit het HTTP request URL de Host. Zoek naar de eerste slash in de opgegeven HTTP-Host adres
      char *TempString=(char*)malloc(80);
      x=StringFind(Settings.HTTPRequest,"/");
      strcpy(TempString,Settings.HTTPRequest);
      TempString[x]=0;
      EthernetClient HTTPClient;
      if(HTTPClient.connect(TempString,Settings.PortClient))   
        {
        HTTPClient.getRemoteIP(HTTPClientIP);
        Ok=true;
        delay(10); //even wachten op response van de server.
        HTTPClient.flush(); // gooi alles weg, alleen IP adres was van belang.
        HTTPClient.stop();
        }
      else
        {
        HTTPClientIP[0]=0;
        HTTPClientIP[1]=0;
        HTTPClientIP[2]=0;
        HTTPClientIP[3]=0;
        }
      free(TempString);    
      }
    else
      Ok=true;
    }
  return Ok;
  }

 /*******************************************************************************************************\
 * Haal via een HTTP-request een file op
 * De content van de file bevindt zich in de body text die de server terugstuurt en wordt opgevangen
 * in de funktie SendHTTPRequest()
 \*******************************************************************************************************/
byte GetHTTPFile(char* filename)
  {
  char *HttpRequest=(char*)malloc(INPUT_BUFFER_SIZE+1);
  char *TempString=(char*)malloc(INPUT_BUFFER_SIZE+1);
  byte Ok;
  
  strcpy(HttpRequest,"?id=");
  strcat(HttpRequest,Settings.ID);  
  strcat(HttpRequest,"&file=");
  strcat(HttpRequest,filename);

  if(Settings.Password[0]!=0)
    {
    // pin-code genereren en meesturen in het http-request
    strcpy(TempString,HTTPCookie);
    strcat(TempString,":");
    strcat(TempString,Settings.Password);  
    md5(TempString);
    strcat(HttpRequest,"&key=");
    strcat(HttpRequest,TempString); 
    }

  free(TempString);
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

  if(Settings.TransmitIP!=VALUE_ON)
    return false;
  
  char *HttpRequest=(char*)malloc(INPUT_BUFFER_SIZE+1);
  char *TempString=(char*)malloc(INPUT_BUFFER_SIZE+1);

  strcpy(HttpRequest,"?id=");
  strcat(HttpRequest,Settings.ID);  

  strcat(HttpRequest,"&unit=");
  if(((event>>28)&0xf)==((unsigned long)(SIGNAL_TYPE_NODO)))
    Unit=(event>>24)&0x0f;
  else
    Unit=0;

  strcat(HttpRequest,int2str(Unit));  
  
  if(Settings.Password[0]!=0)
    {
    // pin-code genereren en meesturen in het http-request
    strcpy(TempString,HTTPCookie);
    strcat(TempString,":");
    strcat(TempString,Settings.Password);
    md5(TempString);
    strcat(HttpRequest,"&key=");
    strcat(HttpRequest,TempString);    
    }
    
  strcat(HttpRequest,"&event=");
  Event2str(event,TempString);
  strcat(HttpRequest,TempString);

  free(TempString);
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

  if(Settings.TransmitIP!=VALUE_ON)
    return false;

  char *HttpRequest=(char*)malloc(INPUT_BUFFER_SIZE+1);

  strcpy(HttpRequest,"?id=");
  strcat(HttpRequest,Settings.ID);  

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
  byte Try=0;
  unsigned long TimeoutTimer;
  char filename[13];
  const int TimeOut=10000;
  EthernetClient HTTPClient;                            // Client class voor HTTP sessie.
  byte State=0;// 0 als start, 
               // 1 als 200 OK voorbij is gekomen,
               // 2 als &file= is gevonden en eerstvolgende lege regel moet worden gedetecteerd
               // 3 als lege regel is gevonden en file-capture moet starten.                


  if(Settings.TransmitIP!=VALUE_ON)
    return false;

  char *IPBuffer=(char*)malloc(IP_BUFFER_SIZE+1);
  char *TempString=(char*)malloc(INPUT_BUFFER_SIZE+1);
  
  strcpy(IPBuffer,"GET ");

  // Haal uit het HTTP request URL de Host. 
  // zoek naar de eerste slash in de opgegeven HTTP-Host adres
  SlashPos=StringFind(Settings.HTTPRequest,"/");
  if(SlashPos!=-1)
    strcat(IPBuffer,Settings.HTTPRequest+SlashPos);

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

  if(Settings.Debug==VALUE_ON)
    Serial.println(IPBuffer);

  strcpy(TempString,Settings.HTTPRequest);
  TempString[SlashPos]=0;
    
  SelectSD(false); // Voor de zekerheid, mocht de chipselect niet goed staan.

  do
    {
    if(HTTPClient.connect(HTTPClientIP,Settings.PortClient))
      {
      ClientIPAddress[0]=HTTPClientIP[0];
      ClientIPAddress[1]=HTTPClientIP[1];
      ClientIPAddress[2]=HTTPClientIP[2];
      ClientIPAddress[3]=HTTPClientIP[3];
    
      HTTPClient.println(IPBuffer);
      HTTPClient.print(F("Host: "));
      HTTPClient.println(TempString);
      HTTPClient.print(F("User-Agent: Nodo/Build="));
      HTTPClient.println(int2str(NODO_BUILD));             
      HTTPClient.println(F("Connection: Close"));
      HTTPClient.println();// Afsluiten met een lege regel is verplicht in http protocol/
  
      TimeoutTimer=millis()+TimeOut; // Als er te lange tijd geen datatransport is, dan wordt aangenomen dat de verbinding (om wat voor reden dan ook) is afgebroken.
      IPBuffer[0]=0;
      InByteCounter=0;
      
      while(TimeoutTimer>millis() && HTTPClient.connected())
        {
        if(HTTPClient.available())
          {
          InByte=HTTPClient.read();
          // DEBUG *** Serial.write(InByte);//???
          if(isprint(InByte) && InByteCounter<IP_BUFFER_SIZE)
            IPBuffer[InByteCounter++]=InByte;
            
          else if(InByte==0x0A)
            {
            IPBuffer[InByteCounter]=0;
            if(Settings.Debug==VALUE_ON)
              {
              strcpy(TempString,"DEBUG HTTP Input: ");
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
  
                  // evntueel vorig bestand wissen
                  SelectSD(true);
                  SD.remove(filename);
                  SelectSD(false);
                  }
                }
              IPBuffer[InByteCounter]=0;
              }
            InByteCounter=0;          
            }
          }
        }
      delay(10);
      HTTPClient.flush();// Verwijder eventuele rommel in de buffer.
      HTTPClient.stop();
      State=true;
      }
    else
      {
      // niet gelukt om de TCP-IP verbinding op te zetten. Genereerd error en herinitialiseer de ethernetkaart.
      State=false;
      delay(1000); // korte pause tussen de nieuwe pogingen om verbinding te maken.
      if(EthernetInit())
        CookieTimer=1;// gelijk een nieuwe cookie versturen.
      }
    }while(!State && ++Try<3);

  free(TempString);
  free(IPBuffer);
  
  if(!State)
    {
    x=Settings.TransmitIP; // HTTP tijdelijk uitzetten want die deed het immers niet.
    Settings.TransmitIP=VALUE_OFF; // HTTP tijdelijk uitzetten want die deed het immers niet.
    RaiseMessage(MESSAGE_07);
    Settings.TransmitIP=x; // HTTP weer terugzetten naar oorspronkelijke waarde.
    }
    
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
  
  if(HTTPRequest_len<3) // doe geen moeite als de string te weinig tekens heeft
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


 /*********************************************************************************************\
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
  int Protocol=0;
  int InByteCounter;
  char FileName[13];
  boolean RequestEvent=false;
  boolean RequestFile=false;
  int x,y;
  unsigned long TimeoutTimer=millis() + 5000; // Na enkele seconden moet de gehele transactie gereed zijn, anders 'hik' in de lijn.
  
  if(Settings.TransmitIP!=VALUE_ON)
    return;

  char *InputBuffer_IP = (char*) malloc(IP_BUFFER_SIZE+1);
  char *Event          = (char*) malloc(INPUT_BUFFER_SIZE+1);
  char *TmpStr1        = (char*) malloc(INPUT_BUFFER_SIZE+1);
  char *TmpStr2        = (char*) malloc(40); 

  Event[0]=0; // maak de string leeg.
  
  EthernetClient HTTPClient=HTTPServer.available();
  
  if(HTTPClient)
    {
    HTTPClient.getRemoteIP(ClientIPAddress);  

    // Controleer of het IP adres van de Client geldig is. 
    if((Settings.Client_IP[0]!=0 && ClientIPAddress[0]!=Settings.Client_IP[0]) ||
       (Settings.Client_IP[1]!=0 && ClientIPAddress[1]!=Settings.Client_IP[1]) ||
       (Settings.Client_IP[2]!=0 && ClientIPAddress[2]!=Settings.Client_IP[2]) ||
       (Settings.Client_IP[3]!=0 && ClientIPAddress[3]!=Settings.Client_IP[3]))
      {
      RaiseMessage(MESSAGE_10);
      }
    else
      {
      InByteCounter=0;
      while(HTTPClient.connected()  && !Completed && TimeoutTimer>millis())
        {
        if(HTTPClient.available()) 
          {
          InByte=HTTPClient.read();

          if(isprint(InByte) && InByteCounter<IP_BUFFER_SIZE)
            {
            InputBuffer_IP[InByteCounter++]=InByte;
            }
      
          else if((InByte==0x0D || InByte==0x0A))
            {
            InputBuffer_IP[InByteCounter]=0;
            InByteCounter=0;

            if(Settings.Debug==VALUE_ON)
              Serial.println(InputBuffer_IP);

            // Kijk of het een HTTP-request is
            if(Protocol==0)
              {
              if(StringFind(InputBuffer_IP,"GET")!=-1)
                Protocol=VALUE_SOURCE_HTTP;// HTTP-Request
              }
            
            if(Protocol==VALUE_SOURCE_HTTP)
              {
              if(!RequestCompleted)
                {
                Completed=true;
                
                // als de beveiliging aan staat, dan kijken of de juiste pin ip meegegeven in het http-request. x is vlag voor toestemming verwerking event
                x=false;
                if(Settings.Password[0]!=0)
                  {
                  sprintf(TmpStr2,"%s:%s",HTTPCookie,Settings.Password);  
                  md5(TmpStr2);
                  
                  if(ParseHTTPRequest(InputBuffer_IP,"key",TmpStr1))
                    {
                    if(strcmp(TmpStr2,TmpStr1)==0)
                      x=true;
                    }
                  }
                else
                  x=true;

                if(x)
                  {                
                  if(ParseHTTPRequest(InputBuffer_IP,"event",Event))
                    RequestEvent=true;
                   
                  if(ParseHTTPRequest(InputBuffer_IP,"file",TmpStr1))
                    {
                    TmpStr1[8]=0; // voorkom dat een file meer dan 8 posities heeft (en een afsluitende 0)
                    strcpy(FileName,TmpStr1);
                    strcat(FileName,".dat");
                    RequestFile=true;
                    }
                    
                  if(RequestFile || RequestEvent)
                    {
                    RequestCompleted=true;
                    strcpy(TmpStr1,"HTTP/1.1 200 Ok");
                    HTTPClient.println(TmpStr1);
                    }
                  else
                    HTTPClient.println(F("HTTP/1.1 400 Bad Request"));
                  }
                else                    
                  HTTPClient.println(F("HTTP/1.1 403 Forbidden"));
                }

              HTTPClient.println(F("Content-Type: text/html"));
              HTTPClient.print(F("Server: Nodo/Build="));
              HTTPClient.println(int2str(NODO_BUILD));             
              if(bitRead(HW_Config,HW_CLOCK))
                {
                HTTPClient.print(F("Date: "));
                HTTPClient.println(DateTimeString());             
                }
              HTTPClient.println(""); // HTTP Request wordt altijd afgesloten met een lege regel
  
              if(RequestFile)
                {              
                // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer SDCard chip
                SelectSD(true);
                File dataFile=SD.open(FileName);
                if(dataFile) 
                  {
                  y=0;       
                  while(dataFile.available())
                    {
                    x=dataFile.read();
                    if(isprint(x) && y<INPUT_BUFFER_SIZE)
                      {
                      TmpStr1[y++]=x;
                      }
                    else
                      {
                      TmpStr1[y]=0;
                      y=0;                    
                      SelectSD(false);
                      
                      if(RequestFile)
                        {
                        HTTPClient.println();
                        RequestFile=false;// gebruiken we even als vlag om de eerste keer de regel met asteriks af te drukken omdat deze variabele toch verder niet meer nodig is
                        }
                      HTTPClient.print(TmpStr1);
                      HTTPClient.println();//??? Verzoek van Martin "<br />" verwijderd

                      SelectSD(true);
                      }
                    }
                  dataFile.close();
                  digitalWrite(EthernetShield_CS_SDCard,HIGH);
                  digitalWrite(Ethernetshield_CS_W5100, LOW);
                  }  
                else 
                  HTTPClient.println(cmd2str(MESSAGE_03));
                }
              } // einde HTTP-request
            }
          else
            {
            // Er is geen geldig teken binnen gekomen. Even wachten en afbreken.
            delay(1000);
            Completed=true;
            }
          }
        }
      }
    delay(10);  // korte pauze om te voorkomen dat de verbinding wordt verbroken alvorens alle data door client verwerkt is.
    HTTPClient.flush();// Verwijder eventuele rommel in de buffer.
    HTTPClient.stop();
    }

  free(TmpStr1);
  free(TmpStr2);
  free(InputBuffer_IP);

  if(RequestEvent)
    ExecuteLine(Event, Protocol);

  free(Event);
  return;
  }  
#endif
