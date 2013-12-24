/**********************************************************************************************\
 * Deze functie wacht totdat de 433 en IR band vrij zijn of er een timeout heeft plaats gevonden 
 * Window en delay tijd in milliseconden
 \*********************************************************************************************/
void WaitFree(byte Port, int TimeOut)
  {
  unsigned long WindowTimer, TimeOutTimer=millis()+TimeOut;  // tijd waarna de routine wordt afgebroken in milliseconden

        BodyTextFile.close();
        SelectSDCard(false);
        }        
      delay(100);
      IPClient.flush();// Verwijder eventuele rommel in de buffer.
      IPClient.stop();
      }
    else
      {
      // niet gelukt om de TCP-IP verbinding op te zetten. Genereer error en her-initialiseer de ethernetkaart.
      State=0;
      delay(3000); // korte pause tussen de nieuwe pogingen om verbinding te maken.
      if(EthernetInit())
        CookieTimer=1;// gelijk een nieuwe cookie versturen.
      }
    }
  while(State==0 && ++Try<3);

  free(TempString);
  free(IPBuffer);

  if(!State)
    {
    x=Settings.TransmitIP; // HTTP tijdelijk uitzetten want die deed het immers niet.
    Settings.TransmitIP=VALUE_OFF; // HTTP tijdelijk uitzetten want die deed het immers niet.
    RaiseMessage(MESSAGE_TCPIP_FAILED,0);
    Settings.TransmitIP=x; // HTTP weer terugzetten naar oorspronkelijke waarde.
    }

  return State;
  }


/*********************************************************************************************\
 * Deze routine haalt uit een string de waarde die bij de opgegeven parameter hoort
 * er wordt niet case sensitief gezocht naar KeyWord. Opvolgende spaties worden verwijderd en de tekens
 * tot aan de eerste worden toegevoed aan ResultString
 * eventuele %20 tekens worden omgezet naar spaties.
 \*********************************************************************************************/
boolean ParseHTTPRequest(char* StringToParse,char* Keyword, char* ResultString,char* BreakChars)
  {
  int x,y,z;
  int Keyword_len=strlen(Keyword);
  int StringToParse_len=strlen(StringToParse);

  ResultString[0]=0;

  if(StringToParse_len<3) // doe geen moeite als de string te weinig tekens heeft.
    return -1;

  for(x=0; x<=(StringToParse_len-Keyword_len); x++)
    {
    y=0;
    while(y<Keyword_len && (tolower(StringToParse[x+y])==tolower(Keyword[y])))
      y++;

    z=x+y;
    if(y==Keyword_len)
      {
      // tekens die overgeslagen kunnen worden
      while(z<StringToParse_len && StringToParse[z]==' ')z++;

      x=0; // we komen niet meer terug in de 'for'-loop, daarom kunnen we x hier even gebruiken.
      y=0; // Vlag die aangeeft if een break teken is gevonden
      // Tekens waarbij afgebroken moet worden
      while(z<StringToParse_len && y<100)
        {
        if(StringToParse[z]=='+')
          ResultString[x]=' ';
        else if(StringToParse[z]=='%' && StringToParse[z+1]=='2' && StringToParse[z+2]=='0')
          {
          ResultString[x]=' ';
          z+=2;
          }
        else
          ResultString[x]=StringToParse[z];

        z++;
        x++;
        
        for(y=0;y<strlen(BreakChars);y++)
          if(BreakChars[y]==StringToParse[z])
            y=100; // stop tekens toevoegen aan resultstring
        }        
      ResultString[x]=0;
      
      return true;
      }
    }
  return false;
  }

void ExecuteIP(void)
  {
  char InByte;
  boolean RequestCompleted=false;  
  boolean Completed=false;
  int Protocol=0;
  int InByteCounter;
  boolean RequestEvent=false;
  boolean ExecuteEvent=false;
  int x;
  unsigned long TimeoutTimer=millis() + 60000; //
 
  char *InputBuffer_IP = (char*) malloc(IP_BUFFER_SIZE);
  char *Event          = (char*) malloc(INPUT_LINE_SIZE);
  char *TmpStr1        = (char*) malloc(INPUT_LINE_SIZE);
  char *TmpStr2        = (char*) malloc(40); 

  Event[0]=0; // maak de string leeg.

  IPClient=IPServer.available();

  if(IPClient)
    {
    IPClient.getRemoteIP(ClientIPAddress);  

    // Controleer of het IP adres van de Client geldig is. 
    if((Settings.Client_IP[0]!=0 && ClientIPAddress[0]!=Settings.Client_IP[0]) ||
      (Settings.Client_IP[1]!=0 && ClientIPAddress[1]!=Settings.Client_IP[1]) ||
      (Settings.Client_IP[2]!=0 && ClientIPAddress[2]!=Settings.Client_IP[2]) ||
      (Settings.Client_IP[3]!=0 && ClientIPAddress[3]!=Settings.Client_IP[3]))
      {
      RaiseMessage(MESSAGE_ACCESS_DENIED,0);
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
            {
            InputBuffer_IP[InByteCounter++]=InByte;
            }

          else if((InByte==0x0D || InByte==0x0A))
            {
            InputBuffer_IP[InByteCounter]=0;
            InByteCounter=0;

            if(Settings.Debug==VALUE_ON)
              {
              Serial.print(F("Debug: HTTP-Input="));
              Serial.println(InputBuffer_IP);
              }

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

                  if(ParseHTTPRequest(InputBuffer_IP,"key=",TmpStr1,"&? "))
                    {
                    if(strcmp(TmpStr2,TmpStr1)==0)
                      x=true;
                    }
                  }
                else
                  x=true;

                if(x)
                  {                
                  if(ParseHTTPRequest(InputBuffer_IP,"event=",Event,"&? "))
                    {
                    RequestEvent=true;
                    RequestCompleted=true;
                    strcpy(TmpStr1,"HTTP/1.1 200 Ok");
                    IPClient.println(TmpStr1);
                    IPClient.println(TmpStr1);
                    ExecuteEvent=true;
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
              #if CLOCK
              if(bitRead(HW_Config,HW_CLOCK))
                {
                IPClient.print(F("Date: "));
                IPClient.println(DateTimeString());             
                }
              #endif clock
              
              IPClient.println(""); // HTTP Request wordt altijd afgesloten met een lege regel

              // Haal nu de resultaten op van het verwerken van de binnengekomen HTTP-regel. Stuur de inhoud als bodytext terug
              // naar de client. De WebApp zal deze content uitparsen. Indien toegang via browser, dan wordt het verwerkings-
              // resultaat getoond in de browser.
              if(ExecuteEvent)
                {              
                // Voer binnengekomen event uit
                ExecuteLine(Event, Protocol);
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
    delay(100);  // korte pauze om te voorkomen dat de verbinding wordt verbroken alvorens alle data door client verwerkt is.
    IPClient.flush();// Verwijder eventuele rommel in de buffer.
    IPClient.stop();
    }

  free(TmpStr1);
  free(TmpStr2);
  free(InputBuffer_IP);
  free(Event);
  return;
  }  

#endif //ethernetserver_h
#endif //MEGA

//#######################################################################################################
//##################################### Transmission: SERIAL  ###########################################
//#######################################################################################################

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

/*********************************************************************************************\
 * Deze funktie berekend de CRC-8 checksum uit van een NodoEventStruct. 
 * Als de Checksum al correct gevuld was wordt er een true teruggegeven. Was dit niet het geval
 * dan wordt NodoEventStruct.Checksum gevuld met de juiste waarde en een false teruggegeven.
 \*********************************************************************************************/

boolean Checksum(NodoEventStruct *event)
  {
  byte OldChecksum=event->Checksum;
  byte NewChecksum=NODO_VERSION_MAJOR;  // Verwerk versie in checksum om communicatie tussen verschillende versies te voorkomen

  event->Checksum=0; // anders levert de beginsituatie een andere checksum op

  for(int x=0;x<sizeof(struct NodoEventStruct);x++)
    NewChecksum^(*((byte*)event+x)); 

  event->Checksum=NewChecksum;
  return(OldChecksum==NewChecksum);
  }
