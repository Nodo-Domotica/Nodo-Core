/*

          // terugtellen blokkade voor invoer wachtwoord.
          if(TerminalLocked>3)TerminalLocked--;


AI=is een unsigned int waarin signed analoge waarde verwerkt

string => AI



wiredanalogcalibrate 1 high 10


Serial.print("*** debug: =");Serial.println(,DEC);//??? Debug
Serial.println("*** debug: ");//???

if((millis()>>12)&0x01==1)Serial.println("*** debug: ...");//???


//  pinMode(22, OUTPUT);//???
//  digitalWrite(22,HIGH);           // ??? Debug: T.b.v. verrichten meting met Logic Analyser
//  digitalWrite(22,LOW);            // ??? Debug: T.b.v. verrichten meting met Logic Analyser


void PrintRawSignal(void)
  {
  Serial.print("*** RawSignal=");//???
  for(int x=1;x<=RawSignal.Number;x++)
    {
    Serial.print(RawSignal.Pulses[x],DEC);//??? Debug  
    Serial.print(",");//???
    }
  Serial.println();//???    
  Serial.print("*** RawSignal.Number=");Serial.println(RawSignal.Number,DEC);//??? Debug
  }
  


int calibrated2int(byte Par1, byte Par2)
  {
  int Value;
  
//Serial.print("3*** Par1=");Serial.println(Par1,BIN);//??? Debug
//Serial.print("3*** Par2=");Serial.println(Par2,BIN);//??? Debug  
  
  Value=(Par1 | Par2<<8) & 0x3ff; // 10-bit waarde
  if(Par2&0x4) // 11e bit in de 16-bits combinatie van Par2 Par1 is het sign-teken.
    Value=-(Value);

Serial.print("3*** Value=");Serial.println(Value,DEC);//??? Debug

  return Value;
  }



int event2wiredport(unsigned long event)
  {
  return ((Par2>>12)&0xf);    
  }



/*unsigned long AnalogRead2event(int Port)
  {
  byte P1,P2;
  int Value;
  unsigned long event;

   Serial.print("*** debug: AnalogRead2Event(); Port=");Serial.println(Port,DEC);//??? Debug

   // lees analoge waarde. Dit is een 10-bit waarde, unsigned 0..1023
   Value=analogRead(Port+WiredAnalogInputPin_1-1);
   Serial.print("*** debug: AnalogRead2Event(); value uitgelezen=");Serial.println(Value,DEC);//??? Debug

  // omrekenen naar gekalibreerde waarde
  
  Value=map(Value,S.WiredInput_Calibration_IL[Port-1],S.WiredInput_Calibration_IH[Port-1],S.WiredInput_Calibration_OL[Port-1],S.WiredInput_Calibration_OH[Port-1]);
  
  Serial.print("*** debug: Map(");
  Serial.print(Value,DEC);
  Serial.print(", ");
  Serial.print(S.WiredInput_Calibration_IL[Port-1],DEC);
  Serial.print(", ");
  Serial.print(S.WiredInput_Calibration_IH[Port-1],DEC);
  Serial.print(", ");
  Serial.print(S.WiredInput_Calibration_OL[Port-1],DEC);
  Serial.print(", ");
  Serial.print(S.WiredInput_Calibration_OH[Port-1],DEC);
  Serial.print(")=");
  Serial.println(Value,DEC);//??? Debug

  // Value moet worden omgezet naar een 16-bit waarde die later analoog kan worden weergegeven met cijfer achter de komma
  P1=(Value   )&0xff;     // 1e acht bits
  P2=(Value>>8)&0x03;     // laatste twee bits, samen 10-bit 
  if(Value<0)             // als negatief getal...
    P2=P2 | 0x04;         // dan set 11e bit om aan te geven dat de waarde negatief is
  P2=P2 | (Port<<4);      // poort op bit 13..16 plaatsen. 

  return(command2event(CMD_WIRED_ANALOG,P1,P2),SIGNAL_TYPE_NODO);
  }




int ppp2analogint(byte Par1, byte Par2)
  {
  static char str[15];
  int x;
  
 // nog uitwerken: aantal decimalen achter de komma afhankelijk maken van grootte van de waarde

  x=(Par1 | Par2<<8) & 0x3ff; // 10-bit waarde

  str[0]=0;
  if(Par2&0x4) // 11e bit in de 16-bits combinatie van Par2 Par1 is het sign-teken.
    strcat(str,"-");
    
  strcat(str,int2str(x/10));
  strcat(str,".");
  x%=10;
  strcat(str,int2str(abs(x)));              
  strcat(str,")");

  return str;
  }






 /*******************************************************************************************************\
 * Deze functie verzendt een regel als event naar een EventGhost EventServer. De Payload wordt niet
 * gebruikt en is leeg. Er wordt een false teruggegeven als de communicatie met de EventGhost EventServer
 * niet tot stand gebracht kon worden.
 \*******************************************************************************************************/
/*boolean HTTP_Request(char* event)
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


 /*********************************************************************************************\
 * Sla de regel op die gebruikt wordt bij verzenden va een HTTP request (OutputHTTP)
 \*********************************************************************************************/
/*boolean HTTPRequestSave(char *Request)
  {
  boolean error=false;

  // Serial.print("*** debug: HTTPRequestSave(); Schrijven naar bestand=");Serial.println(Request);//??? Debug

  if(SDCardPresent)
    {
    // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer SDCard chip
    digitalWrite(Ethernetshield_CS_W5100, HIGH);
    digitalWrite(EthernetShield_CS_SDCard,LOW);

    SD.remove(ProgmemString(Text_29)); // eventueel bestaande file wissen, anders wordt de data toegevoegd.    
    File dataFile = SD.open(ProgmemString(Text_29), FILE_WRITE);
    if(dataFile) 
      {
      dataFile.write(Request);
      dataFile.close();
      }
    else
      {
      TransmitCode(command2event(CMD_ERROR,ERROR_03,0),SIGNAL_TYPE_NODO);
      error=true;
      }

    // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer W5100 chip
    digitalWrite(Ethernetshield_CS_W5100, LOW);
    digitalWrite(EthernetShield_CS_SDCard,HIGH);
    }
  return error;
  }


    
      

      if(TerminalServer.available())
        {
        if(!TerminalConnected)
          {
          // we hebben een nieuwe Terminal client
          TerminalConnected=true;
          TerminalClient=TerminalServer.available();
          if(S.Terminal_Enabled==VALUE_ON)
            {
            PrintWelcome();
            }
          else
            {
            TerminalClient.println(cmd2str(ERROR_10));
            RaiseError(ERROR_10); 
            }              
          }
  
        if(TerminalClient.connected() && TerminalClient.available()) // er staat data van de terminal klaar
          {
          if(TerminalReceive(Inputbuffer_Terminal))
            {
            if(S.Terminal_Enabled==VALUE_ON)
              {
              ExecuteLine(Inputbuffer_Terminal, VALUE_SOURCE_TERMINAL);    
              Serial.print("*** debug: Inputbuffer_Terminal=");Serial.println(Inputbuffer_Terminal);//??? Debug
              }
            else
              {
              TerminalClient.println(cmd2str(ERROR_10));
              RaiseError(ERROR_10); 
              }
            }
          }
  

  
    // event toevoegen aan tijdelijke string, echter alle spaties vervangen door %20 conform URL notatie
    for(byte x=0;x<strlen(event);x++)
      {
      if(event[x]==32)
        strcat(TempString,"%20");
      else
        {
        s[0]=event[x];
        strcat(TempString,s);
        }
      }      



*/
