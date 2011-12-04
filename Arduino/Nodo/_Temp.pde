//    // kijk of de hex-code toevallig al eerder is opgeslagen als rawsignal op de SDCard
//    if(SDCardPresent)
//      {
//      // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer SDCard chip
//      digitalWrite(Ethernetshield_CS_W5100, HIGH);
//      digitalWrite(EthernetShield_CS_SDCard,LOW);      
//      sprintf(TempString,"%s/%s.key",ProgmemString(Text_28),int2str(Event)+2); // +2 omdat dan de tekens '0x' niet worden meegenomen. anders groter dan acht posities in filenaam.
//      Serial.print("*** check binnengekomen hex-event in file ");Serial.println(TempString);//??? Debug
//      
//      File dataFile=SD.open(TempString);
//      if(dataFile) 
//        {
//        y=0;       
//        while(dataFile.available())
//          {
//          x=dataFile.read();
//          if(isprint(x) && y<INPUT_BUFFER_SIZE)
//            {
//            InputBuffer[y++]=x;
//            }
//          else
//            {
//            InputBuffer[y]=0;
//            y=0;
//            PrintLine(InputBuffer);
//            }
//          }
//        dataFile.close();
//        }  
//      Serial.print("*** Equivalent key=");Serial.println(InputBuffer);//??? Debug
//
//      // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer W510 chip
//      digitalWrite(Ethernetshield_CS_W5100, LOW);
//      digitalWrite(EthernetShield_CS_SDCard,HIGH);
//      }    



/***************************************************************************

Serial.print("*** =");Serial.println(,DEC);//??? Debug


*/



/*********************************************************************************************\
* Deze functie dient alleen aangeroepen te worden als er tekens beschikbaar zijn op de
* seriële poort. Deze functie haalt de tekens op, parsed commando en parameters er uit en 
* maakt hier een Event van. Als het event een MMI afwijkend commando is of een commando
* waarvan het niet wenselijk is dat dit via RF/IR ontvangen en verwerkt mag worden,
* dan wordt deze tevens uitgevoerd. Alle overige events & commando's worden ter verdere
* verwerking teruggestuurd.
\*********************************************************************************************/

//unsigned long Receive_Serial(void)
//  {
//  unsigned long Event,Action;
//  byte x,y;
//  byte Par1,Par2,Cmd;
//  boolean error=false;
//  
//  // Hier aangekomen staan er tekens klaar. Haal op van seriële poort en maak er een Event van.
//  Event=SerialReadEvent();
//
//  // kijk of het een Nodo-event is.
//  if(((Event>>28)&0xf)!=SIGNAL_TYPE_NODO)
//    return Event; // HEX-waarde. verdere uitvoer hoeft niet hier plaats te vinden.
//    
//  // Als het een commando is, dan checken of er geldige parameters zijn opgegeven
//  Cmd=(Event>>16)&0xff;
//  error=CommandError(Event);
//  if(error)
//    {
//    if(error==ERROR_COMMAND) // commando bestaat niet 
//        TransmitCode(command2event(CMD_ERROR,VALUE_COMMAND,0),SIGNAL_TYPE_NODO);    
//    else // commando bestaat maar parameters niet correct
//        TransmitCode(command2event(CMD_ERROR,Cmd,error),SIGNAL_TYPE_NODO);
//    return 0L;
//    }
//    
//  Par1=(Event>>8)&0xff;
//  Par2=(Event)&0xff;
//
//  switch(Cmd)
//    {
//   
//    case CMD_EVENTLIST_WRITE:
//      // haal event en actie op
//      Event=SerialReadEvent();
//      if(!Event)
//        {
//        TransmitCode(command2event(CMD_ERROR,CMD_EVENTLIST_WRITE,1),SIGNAL_TYPE_NODO);
//        break;
//        }
//        
//      Action=SerialReadEvent();
//      if(!Action)
//        {
//        TransmitCode(command2event(CMD_ERROR,CMD_EVENTLIST_WRITE,2),SIGNAL_TYPE_NODO);
//        break;
//        }
//                
//      // schrijf weg in eventlist      
//      if(!Eventlist_Write(0,Event,Action))
//        {
//        error=true;
//        Par1=CMD_EVENTLIST_WRITE;
//        Par2=VALUE_SOURCE_EVENTLIST;
//        }
//      break;        
//
//    case CMD_DIVERT:   
//      Action=(SerialReadEvent()&0x00ffffff) | ((unsigned long)(Par1))<<24 | ((unsigned long)(SIGNAL_TYPE_NODO))<<28; // Event_1 is het te forwarden event voorzien van nieuwe bestemming unit
//      x=(Action>>24)&0x0f; // unit
//      if(x==0 || x==S.Unit)
//        ProcessEvent(Action,VALUE_DIRECTION_INPUT,VALUE_SOURCE_SERIAL,0,0);      // verwerk binnengekomen event.
//      if(x!=S.Unit)
//        TransmitCode(Action,SIGNAL_TYPE_NODO);
//      break;        
//
//
//    case CMD_RAWSIGNAL_PUT:
//      y=1;
//      do
//        {
//        x=SerialReadBlock(SerialBuffer);
//        RawSignal[y++]=str2val(SerialBuffer);
//        }while(x && y<RAW_BUFFER_SIZE);
//      RawSignal[0]=y-1;
//      Event=AnalyzeRawSignal();
//      TransmitCode(Event,SIGNAL_TYPE_UNKNOWN);
//      break;
//   
//    default:
//      // het is een geldig commando, maar verwerking vond niet hier plaats.
//      return Event; // verdere afhandeling door ExecuteCommand()
//    }
//  return 0L;
//  }
//
//
///*********************************************************************************************\
//* Hier aangekomen staan er tekens klaar op de seriële poort
//* Haal deze op en stel een Event samen. Sommige commando's hebben (helaas) een afwijkende MMI 
//* Of behandeling. De geldigheid van het commando wordt eveneens getoetst.
//\*********************************************************************************************/
//unsigned long SerialReadEvent()
//  {
//  unsigned long Event;
//  byte x,y,z;
//  int Par1,Par2;
//  
//  // haal 1e blok gegevens op.
//  x=SerialReadBlock(SerialBuffer);
//  if(SerialBuffer[0]==0)
//    return 0L; // als de string leeg is (b.v. als er wel tekens klaarstonden maar deze niet geldig waren
//
//  // is het gewoon een getal dat als event moet worden verwerkt? Deze zijn altijd groter dan 255 want 0..255 zijn commando's
//  Event=str2val(SerialBuffer);
//  if(Event>0xff)
//    return SetEventType(Event,SIGNAL_TYPE_UNKNOWN); // alle waarden groter dan 255 mogen gelijk verwerkt worden als een event.
//    
//  // invoer was geen getal. Haal uit de invoer de code van het tekst commando
//  y=str2cmd(SerialBuffer);
//
//  // als het geen geldig HEX-event was en de tekst werd niet herkend als een commando, dan foutmelding.
//  if(y==0)
//    {
//    PrintText(Text_06);
//    PrintTerm();
//    return 0L;
//    }
//    
//  if(y)
//    {
//    Par1=0;
//    Par2=0;
//
//    // NewKAKU events hebben een afwijkende MMI voor invoer. Par2 kan nl. een 28-bit hex-code zijn.
//    if(y==CMD_KAKU_NEW)
//      {// de string is een NewKAKU event. Haal bij het commando behorende parameters op.
//      x=SerialReadBlock(SerialBuffer);
//      if(x)
//        {
//        // er kunnen twee situaties voor doen: een NewKAKU volgens lange HEX-codering of volgens reguliere Nodo codering
//        Event=str2val(SerialBuffer); // bevat waarde op de MMI positie van Par1
//        if(Event>255)
//          {
//          Event=(Event&0x0fffffff) | (((unsigned long)SIGNAL_TYPE_NEWKAKU)<<28); //  // Niet Par1 want NewKAKU kan als enige op de Par1 plaats een 28-bit waarde hebben. Hoogste nible wissen en weer vullen met type NewKAKU        
//          if(x)
//            {
//            x=SerialReadBlock(SerialBuffer);
//            Event|=((str2val(SerialBuffer)==VALUE_ON)<<4); // Stop on/off commando op juiste plek in NewKAKU code
//            return Event;
//            }
//          }
//        else
//          {
//          Par1=Event;        
//          if(x)
//            {
//            x=SerialReadBlock(SerialBuffer);
//            Par2=str2val(SerialBuffer); // Parameter-2 bevat [0..255,On,Off].
//            }
//          }
//        }
//      }
//  
//    // KAKU events hebben een afwijkende MMI voor invoer. Par1 kan nl. A0..P16 bevatten.
//    if(y==CMD_KAKU || y==CMD_SEND_KAKU)
//      {// de string is een KAKU commando. Haal bij het commando behorende parameters op.
//      if(x)
//        {
//        x=SerialReadBlock(SerialBuffer);
//        z=0;
//        Par1=HA2address(SerialBuffer,&z); // Parameter-1 bevat [A1..P16]. Omzetten naar absolute waarde. z=groep commando
//        if(x)
//          {
//          x=SerialReadBlock(SerialBuffer);
//          Par2=(str2val(SerialBuffer)==VALUE_ON) | (z<<1); // Parameter-2 bevat [On,Off]. Omzetten naar 1,0. tevens op bit-2 het groepcommando zetten.
//          }
//        }
//      }
//  
//    if(y>RANGE_VALUE && y<=COMMAND_MAX)
//      {// de string valt in de reeks van een commando of event. Haal bij het commando behorende parameters op.
//      if(x)
//        {
//        x=SerialReadBlock(SerialBuffer);
//        Par1=str2val(SerialBuffer);
//        if(x)
//          {
//          x=SerialReadBlock(SerialBuffer);
//          Par2=str2val(SerialBuffer);
//          }
//        }
//      }
//    Event=command2event(y,Par1,Par2);
//    }
//
//  // op dit punt kan het opgehaalde event + parameters zowel een Nodo-event zijn als een Nodo-commando
//
//  // kijk of het een Nodo-event is.
//  //  if(((Event>>28)&0xf)==SIGNAL_TYPE_NODO)
//  if(CommandError(Event))// als er een error is, dan melding op Serial
//    {
//    PrintText(Text_06);
//    PrintTerm();
//    return 0L;
//    }
//  else
//    return Event; // verdere check hoeft niet plaats te vinden.      
//
//  return 0L;
//  }
//   
//   
///**********************************************************************************************\
// * Haalt uit een seriële reeks met formaat 'aaaa,bbbb,cccc,dddd;' de blokken tekst 
// * geeft een 1 terug als blok afgesloten met een komma of spatie (en er nog meerdere volgen)
// * geeft een 0 terug als hele reeks is afgesloten met een '\n' of een ';'
// \*********************************************************************************************/
//#define SERIAL_TIMEOUT 5000
//byte SerialReadBlock(char *SerialString)
//  {
//  byte StringLength,SerialByte,x;
//  unsigned long TimeOutTimer;
//  
//  // op moment hier aangekomen staat er minimaal een byte klaar in de seriele buffer  
//  SerialHold(false);
//  x=0;
//  StringLength=0;
//  TimeOutTimer=millis()+SERIAL_TIMEOUT;
//
//  do
//    {// lees de seriële poort totdat afsluitteken of een teken voor volgende parameter
//    if(Serial.available()>0)
//      {
//      SerialByte=Serial.read();
//      if(StringLength==0 && (SerialByte==32 || SerialByte==','))continue; // voorloop spaties overslaan
//      if(SerialByte=='(' || SerialByte==')')continue; // deze tekens zijn comment, overslaan.
//      if(SerialByte==';' || SerialByte==',' || SerialByte=='\n' || SerialByte==32)// tekens die parameters of commando's van elkaar onderscheiden
//        x=SerialByte;
//      else
//        if(SerialByte>=32 && SerialByte<=126) // alleen printbare tekens zijn relevant
//          SerialString[StringLength++]=tolower(SerialByte); // voeg karakter toe aan de SerialString (lower-case)
//      }
//    }while(StringLength<SERIAL_BUFFER_SIZE && x==0 && millis()<TimeOutTimer);
//  SerialString[StringLength]=0; // sluit SerialString af met een \0
//
//  SerialHold(true);
//  if(x==',' || x==32)return true;// er komen nog meer blokken
//  return false;// afgesloten met '\n' of een ';', dus commando is compleet
//  }


