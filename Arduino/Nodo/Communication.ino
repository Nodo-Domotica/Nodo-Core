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
  
#if NODO_MEGA

 

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
  
  #if TRACE
  Trace(9,0,0);
  #endif

  strcpy(HttpRequest,"?id=");
  strcat(HttpRequest,Settings.ID);  
  strcat(HttpRequest,"&file=");
  strcat(HttpRequest,filename);

  if(Settings.HTTP_Pin==VALUE_ON)
    {
    // pin-code genereren en meesturen in het http-request
    strcpy(TempString,HTTPCookie);
    strcat(TempString,":");
    strcat(TempString,Settings.Password);  
    md5(TempString);
    strcat(HttpRequest,"&key=");
    strcat(HttpRequest,TempString); 
    }

  Ok=SendHTTPRequest(HttpRequest);

  free(HttpRequest);
  free(TempString);
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
  char *TempString=(char*)malloc(INPUT_BUFFER_SIZE+1);

  #if TRACE
  Trace(10,0,0);
  #endif

  strcpy(HttpRequest,"?id=");
  strcat(HttpRequest,Settings.ID);  

  strcat(HttpRequest,"&unit=");
  if(((event>>28)&0xf)==((unsigned long)(SIGNAL_TYPE_NODO)))
    Unit=(event>>24)&0x0f;
  else
    Unit=0;

  strcat(HttpRequest,int2str(Unit));  
  
  if(Settings.HTTP_Pin==VALUE_ON)
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

  x=SendHTTPRequest(HttpRequest);
  free(HttpRequest);
  free(TempString);
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

  #if TRACE
  Trace(11,0,0);
  #endif

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
  unsigned long TimeoutTimer;
  char *IPBuffer=(char*)malloc(IP_BUFFER_SIZE+1);
  char *TempString=(char*)malloc(INPUT_BUFFER_SIZE+1);
  char filename[13];
  const int TimeOut=10000;
  EthernetClient IPClient;                            // Client class voor HTTP sessie.
  byte State=0;// 0 als start, 
               // 1 als 200 OK voorbij is gekomen,
               // 2 als &file= is gevonden en eerstvolgende lege regel moet worden gedetecteerd
               // 3 als lege regel is gevonden en file-capture moet starten.                

  
  #if TRACE
  Trace(12,0,0);
  #endif

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
    {
    strcpy(TempString,"# HTTP Output: ");
    strcat(TempString,IPBuffer);// Ruimte IPBuffer is groter dan TempString, maar kan hier geen kwaad
    Serial.println(TempString);
    }

  strcpy(TempString,Settings.HTTPRequest);
  TempString[SlashPos]=0;

  #if TRACE
  Trace(12,1,Settings.PortClient);
  AddFileSDCard("TRACE.DAT", TempString);
  AddFileSDCard("TRACE.DAT", IPBuffer);
  #endif

  SelectSD(false); // Voor de zekerheid, mocht de chipselect niet goed staan.
  if(IPClient.connect(TempString,Settings.PortClient))
    {
    IPClient.getRemoteIP(ClientIPAddress);  

    #if TRACE
    AddFileSDCard("TRACE.DAT", ip2str(ClientIPAddress));
    #endif

    IPClient.println(IPBuffer);

    IPClient.print(F("Host: "));
    IPClient.println(TempString);

    IPClient.print(F("User-Agent: Nodo/Build="));
    IPClient.println(int2str(NODO_BUILD));             
 
    IPClient.println(F("Connection: Close"));
  
    IPClient.println();// Afsluiten met een lege regel is verplicht in http protocol/

    TimeoutTimer=millis()+TimeOut; // Als er te lange tijd geen datatransport is, dan wordt aangenomen dat de verbinding (om wat voor reden dan ook) is afgebroken.
    IPBuffer[0]=0;
    InByteCounter=0;
    
    while(TimeoutTimer>millis() && IPClient.connected())
      {
      if(IPClient.available())
        {
        InByte=IPClient.read();

        if(isprint(InByte) && InByteCounter<IP_BUFFER_SIZE)
          IPBuffer[InByteCounter++]=InByte;
          
        else if(InByte==0x0A)
          {
          IPBuffer[InByteCounter]=0;
          if(Settings.Debug==VALUE_ON)
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
                 
                #if TRACE
                Trace(12,2,0);
                #endif
                
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
    IPClient.stop();
    }
  else
    State=false;

  free(TempString);
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

  #if TRACE
  Trace(13,0,0);
  #endif

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
  unsigned long TimeoutTimer=millis()+5000; // Na twee seconden moet de gehele transactie gereed zijn, anders 'hik' in de lijn.
  
  char *InputBuffer_IP = (char*) malloc(IP_BUFFER_SIZE+1);
  char *Event          = (char*) malloc(INPUT_BUFFER_SIZE+1);
  char *TmpStr1        = (char*) malloc(INPUT_BUFFER_SIZE+1);
  char *TmpStr2        = (char*) malloc(40); 

  #if TRACE
  Trace(14,0,0);
  #endif

  Event[0]=0; // maak de string leeg.
  
  EthernetClient IPClient = IPServer.available();
  
  if(IPClient)
    {
    IPClient.getRemoteIP(ClientIPAddress);  

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
      while(IPClient.connected()  && !Completed && TimeoutTimer>millis())
        {
        if(IPClient.available()) 
          {
          InByte=IPClient.read();
          
          if(isprint(InByte) && InByteCounter<IP_BUFFER_SIZE)
            InputBuffer_IP[InByteCounter++]=InByte;
      
          else if((InByte==0x0D || InByte==0x0A))
            {
            InputBuffer_IP[InByteCounter]=0;
            InByteCounter=0;
            
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
                if(Settings.Debug==VALUE_ON)
                  Serial.println(InputBuffer_IP);
                
                // als de beveiliging aan staat, dan kijken of de juiste pin ip meegegeven in het http-request. x is vlag voor toestemming verwerking event
                x=false;
                if(Settings.HTTP_Pin==VALUE_ON)
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
                    IPClient.println(TmpStr1);
                    }
                  else
                    IPClient.println(F("HTTP/1.1 400 Bad Request"));
                  }
                else                    
                  IPClient.println(F("HTTP/1.1 403 Forbidden"));
                }

              IPClient.println(F("Content-Type: text/html"));
              IPClient.print(F("Server: Nodo/Build="));
              IPClient.println(int2str(NODO_BUILD));             
              if(bitRead(HW_Config,HW_CLOCK))
                {
                IPClient.print(F("Date: "));
                IPClient.println(DateTimeString());             
                }
              IPClient.println(""); // HTTP Request wordt altijd afgesloten met een lege regel
  
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
                        IPClient.println();//??? Verzoek van Martin "<br />" verwijderd
                        RequestFile=false;// gebruiken we even als vlag om de eerste keer de regel met asteriks af te drukken omdat deze variabele toch verder niet meer nodig is
                        }
                      IPClient.print(TmpStr1);
                      IPClient.println();//??? Verzoek van Martin "<br />" verwijderd

                      SelectSD(true);
                      }
                    }
                  dataFile.close();
                  digitalWrite(EthernetShield_CS_SDCard,HIGH);
                  digitalWrite(Ethernetshield_CS_W5100, LOW);
                  }  
                else 
                  IPClient.println(cmd2str(MESSAGE_03));
                }
              } // einde HTTP-request
            }  
          }
        }
      }
    delay(5);  // korte pauze om te voorkomen dat de verbinding wordt verbroken alvorens alle data door client verwerkt is.
    IPClient.stop();
    }

  if(RequestEvent)
    ExecuteLine(Event, Protocol);

  free(TmpStr1);
  free(TmpStr2);
  free(InputBuffer_IP);
  free(Event);
  return;
  }
  
#endif
