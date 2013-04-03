#ifdef NODO_MEGA

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

/**********************************************************************************************\
 * Wis een file
 \*********************************************************************************************/
void FileErase(char *FileName)
  {
  SelectSDCard(true);
  SD.remove(FileName);
  SelectSDCard(false);
  }


boolean AddFileSDCard(char *FileName, char *Line)
  {
  boolean r;

  SelectSDCard(true);
  File LogFile = SD.open(FileName, FILE_WRITE);
  if(LogFile)
    {
    r=true;
    LogFile.write((uint8_t*)Line,strlen(Line));
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


boolean FileList(char *rootdir, boolean Erase)
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
      PrintTerminal(ProgmemString(Text_22));
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
          // Als de fuktie is aangeroepen vanuit RawSignalList, dan voor de bestandnamen 0x plakken omdat de bestandsnamen een 
          // hexadecimale warde representeren. Niet nietjes op deze wijze maar bespaart code. 
          if(strcasecmp(rootdir,"/RAW")==0)
            strcat(TempString,"0x");
          strcat(TempString,entry.name());
          TempString[StringFind(TempString,".")]=0;
          SelectSDCard(false);
          PrintTerminal(TempString);
          SelectSDCard(true);
          }
        }
      entry.close();
      }
    root.close();

    SelectSDCard(false);
    if(!Erase)
      PrintTerminal(ProgmemString(Text_22));
    }
  else
    error=MESSAGE_14;
    
  free(TempString);  
  return error;
  }


byte FileExecute(char* FileName, boolean ContinueOnError)
  {
  int x,y;
  char *TmpStr=(char*)malloc(INPUT_BUFFER_SIZE+1);
  byte error=0;

  Led(RED);

  strcpy(TmpStr,FileName);
  strcat(TmpStr,".dat");
  SelectSDCard(true);
  File dataFile=SD.open(TmpStr);
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
        PrintTerminal(TmpStr);
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
    error=MESSAGE_03;

  free(TmpStr);
  SelectSDCard(false);
  return error;
  }    


byte FileShow(char *FileName)
  {
  SelectSDCard(true);
  char *TmpStr2=(char*)malloc(INPUT_BUFFER_SIZE+2);
  byte error=0;
  
  File dataFile=SD.open(FileName);
  if(dataFile) 
    {
    SelectSDCard(false);
    PrintTerminal(ProgmemString(Text_22));
    SelectSDCard(true);

    // Als de file groter is dan 10K, dan alleen laatste stuk laten zien
    unsigned long a=dataFile.size();
    if(a>10000)
      {
      unsigned long w=dataFile.seek(a-10000UL);                    
      while(dataFile.available() && isprint(dataFile.read()));
      SelectSDCard(false);
      PrintTerminal(ProgmemString(Text_09));
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
        PrintTerminal(TmpStr2);
        SelectSDCard(true);
        }
      }
    dataFile.close();
    SelectSDCard(false);
    PrintTerminal(ProgmemString(Text_22));
    }  
  else
    error=MESSAGE_03;


  SelectSDCard(false);
  free(TmpStr2);
  return error;
  }

/*********************************************************************************************\
 * Kijk of voor de opgegeven Hex-event (Code) een rawsignal file op de SDCard bestaat.
 * Als deze bestaat dan return met 'true'
 \*********************************************************************************************/
boolean RawSignalExist(unsigned long Code)
  {
  int exist=false;  
  char *TempString=(char*)malloc(25);

  SelectSDCard(true);
  sprintf(TempString,"%s/%s.raw",ProgmemString(Text_28),int2strhex(Code)+2); // +2 omdat dan de tekens '0x' niet worden meegenomen. anders groter dan acht posities in filenaam.

  File dataFile=SD.open(TempString);
  if(dataFile) 
    {
    exist=true;
    dataFile.close();
    }  

  // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer W5100 chip
  SelectSDCard(false);
  free(TempString);
  return exist;
  }

/*********************************************************************************************\
 * Sla de pulsen in de buffer Rawsignal op op de SDCard
 \*********************************************************************************************/
byte RawSignalSave(unsigned long Key)
  {
  boolean error=false;
  char *TempString=(char*)malloc(40);

  if(Key!=0)
    {
    // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer SDCard chip
    SelectSDCard(true);
  
    // Sla Raw-pulsenreeks op in bestand met door gebruiker gekozen nummer als filenaam
  
  
    sprintf(TempString,"%s/%s.raw",ProgmemString(Text_28),int2strhex(Key)+2);
  
    SD.remove(TempString); // eventueel bestaande file wissen, anders wordt de data toegevoegd.    
    File KeyFile = SD.open(TempString, FILE_WRITE);
    if(KeyFile) 
      {
      for(int x=0;x<=RawSignal.Number;x++)
        {
        TempString[0]=0;
        if(x>1)
          strcat(TempString,",");
        strcat(TempString,int2str(RawSignal.Pulses[x]));
        KeyFile.write(TempString);
        }
      strcpy(TempString,"\n");
      KeyFile.write(TempString);
      KeyFile.close();
      }
    else 
      error=true;
  
    // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer W5100 chip
    SelectSDCard(false);
  
    free(TempString);
    }

  if(error)
    RaiseMessage(MESSAGE_14);

  return !error;
  }


/*********************************************************************************************\
 * Haal de RawSignal pulsen op uit het bestand <key>.raw en sla de reeks op in de 
 * RawSignal buffer, zodat deze vervolgens weer kan worden gebruikt om te verzenden.
 \*********************************************************************************************/
boolean RawSignalLoad(unsigned long Key)
  {
  int x,y,z;
  boolean Ok;
  char *TempString=(char*)malloc(INPUT_BUFFER_SIZE+1);

  // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer SDCard chip
  SelectSDCard(true);
  sprintf(TempString,"%s/%s.raw",ProgmemString(Text_28),int2strhex(Key)+2);

  File dataFile=SD.open(TempString);
  if(dataFile) 
    {
    y=0;
    z=0;
    while(dataFile.available())
      {
      x=dataFile.read();
      if(isDigit(x) && y<INPUT_BUFFER_SIZE)
        {
        TempString[y++]=x;
        }
      else if(x=='\n' || isPunct(x))
        {
        TempString[y]=0;
        y=0;
        RawSignal.Pulses[z++]=str2int(TempString);
        }
      }
    dataFile.close();
    Ok=true;
    RawSignal.Number=z-1;
    }
  else
    Ok=false;

  // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer W5100 chip
  SelectSDCard(false);

  free(TempString);
  return Ok;
  }

#endif
