#if NODO_MEGA

/*********************************************************************************************\
 * Op het Ethernetshield kunnen de W5100 chip en de SDCard niet gelijktijdig worden gebruikt
 * Deze funktie zorgt voor de juiste chipselect. Default wordt in de Nodo software uitgegaan dat
 * de ethernetchip W5100 is geselecteerd. Met deze routine kan de SDCard worden geselecteerd.
 * input: true = SD_Card geselecteerd.
 \*********************************************************************************************/
void SelectSDCard(boolean sd)
  {
  digitalWrite(EthernetShield_CS_W5100, sd);
  digitalWrite(EthernetShield_CS_SDCard, !sd);
  }

/**********************************************************************************************\
 * Wis een file
 \*********************************************************************************************/
void SDCardInit(void)
  {
  SelectSDCard(true);
  if(SD.begin(EthernetShield_CS_SDCard))
    {
    SD.mkdir(ProgmemString(Text_28)); // maak directory aan waar de Rawsignal KEY bestanden in worden opgeslagen
    bitWrite(HW_Config,HW_SDCARD,1);
    }
  SelectSDCard(false);
  }

 /*********************************************************************************************\
 * Wis een file
 \*********************************************************************************************/
void FileErase(char *FileName)
  {
  SelectSDCard(true);
  SD.remove(FileName);
  SelectSDCard(false);
  }

 /*********************************************************************************************\
 * 
 \*********************************************************************************************/
boolean AddFileSDCard(char *FileName, char *Line)
  {
  boolean r;

  SelectSDCard(true);
  File LogFile = SD.open(FileName, FILE_WRITE);
  if(LogFile)
    {
    r=true;
    //??? vervangen of heeft het cons. voor binnenhalen BodyTexk etc. ? ==> LogFile.write((uint8_t*)Line,strlen(Line));
    
    for(int x=0;x<strlen(Line);x++)
      if(isprint(Line[x]))
        LogFile.write(Line[x]);
        
    LogFile.write('\n'); // nieuwe regel
    LogFile.close();
    }
  else
    r=false;

  SelectSDCard(false);
  return r;
  }

/**********************************************************************************************\
 * Voeg een regel toe aan de logfile.
 \*********************************************************************************************/
boolean SaveEventlistSDCard(char *FileName)
 {
  int x;
  boolean r=true;
  char *TempString=(char*)malloc(100);

  // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer SDCard chip
  SelectSDCard(true);

  SD.remove(FileName); // eerst bestand wissen, anders wordt de data toegevoegd

  File EventlistFile = SD.open(FileName, FILE_WRITE);
  if(EventlistFile) 
    {
    strcpy(TempString,cmd2str(CMD_EVENTLIST_ERASE));
    EventlistFile.write((uint8_t*)TempString,strlen(TempString));      
    EventlistFile.write('\n'); // nieuwe regel

    x=1;
    while(EventlistEntry2str(x++,0,TempString,true))
      {

      if(TempString[0]!=0)// als de string niet leeg is, dan de regel weg schrijven
        {
        EventlistFile.write((uint8_t*)TempString,strlen(TempString));      
        EventlistFile.write('\n'); // nieuwe regel
        }
      }
    EventlistFile.close();
    }
  else 
    {
    r=false; // niet meer weer proberen weg te schrijven.
    }

  SelectSDCard(false);

  free(TempString);
  return r;
  }


boolean FileList(char *rootdir, boolean Erase, byte Port)
  {
  byte error=0;
  File root;
  File entry;
  char *TempString=(char*)malloc(30);

  SelectSDCard(true);
  if(root = SD.open(rootdir))
    {
    if(!Erase)
      {
      SelectSDCard(false);
      PrintString(ProgmemString(Text_22), Port);
      SelectSDCard(true);
      }
  
    root.rewindDirectory();
    while(entry = root.openNextFile())
      {
      if(!entry.isDirectory())
        {
        if(Erase)
          {
          strcpy(TempString,rootdir);
          strcat(TempString,"/");
          strcat(TempString,entry.name());
          FileErase(TempString);
          }
        else
          {
          TempString[0]=0;
          // Als de funktie is aangeroepen vanuit RawSignalList, dan voor de bestandnamen 0x plakken omdat de bestandsnamen een 
          // hexadecimale waarde representeren. Niet netjes op deze wijze maar bespaart code. 
          if(strcasecmp(rootdir,"/RAW")==0)
            strcat(TempString,"0x");
          strcat(TempString,entry.name());
          TempString[StringFind(TempString,".")]=0;
          SelectSDCard(false);
          PrintString(TempString,Port);
          SelectSDCard(true);
          }
        }
      entry.close();
      }
    root.close();

    SelectSDCard(false);
    if(!Erase)
      PrintString(ProgmemString(Text_22),Port);
    }
  else
    error=MESSAGE_SDCARD_ERROR;
    
  free(TempString);  
  return error;
  }


byte FileExecute(char* FileName, boolean ContinueOnError, byte PrintPort)
  {
  int x,y;
  byte error=0;
  static boolean FileExecuteActive=false;// voorkom nesting van fileexecute
      
  char *TmpStr=(char*)malloc(INPUT_BUFFER_SIZE+1);
  
Serial.print("*** FileExecute() File=");Serial.println(FileName);//???
Serial.print("*** FileExecute() FileExecuteActive=");Serial.println(FileExecuteActive);//???

  if(FileExecuteActive)
    {
    RaiseMessage(MESSAGE_NESTING_ERROR,0);
    }
  else
    {
    Led(RED);
    FileExecuteActive=true;
    
    SelectSDCard(true);
    File dataFile=SD.open(FileName);
    
    if(dataFile) 
      {
      y=0;       
      while(dataFile.available() && !error)
        {
        x=dataFile.read();
        if(isprint(x) && y<INPUT_BUFFER_SIZE)
          TmpStr[y++]=x;
        else
          {
          TmpStr[y]=0;
          y=0;
          SelectSDCard(false);
          if(PrintPort)
            PrintString(TmpStr, PrintPort);
Serial.print("*** FileExecute() Voer regel uit =");Serial.println(TmpStr);//???

          error=ExecuteLine(TmpStr,VALUE_SOURCE_FILE);
          SelectSDCard(true);
  
          if(ContinueOnError)
            error=0;
  
          if(error)
            break;
          }
        }
      dataFile.close();
      }  
    else
      error=MESSAGE_UNABLE_OPEN_FILE;
    }

  FileExecuteActive=false;
  free(TmpStr);    
  SelectSDCard(false);

  return error;
  }    


byte FileShow(char *FileName, byte Port)
  {
  char *TmpStr2=(char*)malloc(INPUT_BUFFER_SIZE+2);
  byte error=0;
  

Serial.print("*** FileShow() File=");Serial.println(FileName);//???

  SelectSDCard(true);

  File dataFile=SD.open(FileName);
  if(dataFile) 
    {
    SelectSDCard(false);
    PrintString(ProgmemString(Text_22), Port);
    SelectSDCard(true);

    // Als de file groter is dan 10K, dan alleen laatste stuk laten zien
    unsigned long a=dataFile.size();
    if(a>100000UL)
      {
      unsigned long w=dataFile.seek(a-100000UL);                    
      while(dataFile.available() && isprint(dataFile.read()));
      SelectSDCard(false);
      PrintString(ProgmemString(Text_09),Port);
      SelectSDCard(true);
      }

    TmpStr2[0]=0;
    int y=0;       
    while(dataFile.available())
      {
      byte x=dataFile.read();
      if(isprint(x) && y<INPUT_BUFFER_SIZE)
        {
        TmpStr2[y++]=x;
        }
      else
        {
        TmpStr2[y]=0;
        y=0;
        SelectSDCard(false);
        PrintString(TmpStr2,Port);
        SelectSDCard(true);
        }
      }
    dataFile.close();
    SelectSDCard(false);
    PrintString(ProgmemString(Text_22),Port);
    }  
  else
    error=MESSAGE_UNABLE_OPEN_FILE;


  SelectSDCard(false);
  free(TmpStr2);
  return error;
  }

#endif
