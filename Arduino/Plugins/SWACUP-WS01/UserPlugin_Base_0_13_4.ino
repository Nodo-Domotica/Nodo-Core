//*********************************************************************************************\
// Author(s)        : This Nodo Userplugin is offered as a voluntary Nodo community development effort.
//                    Development and testing is solely performed by community members.
//                    (Just to name a few: MVDBRO, ARENDST, INTENSIVUS, JACK, PROZA, KABOUTER, PIETERS, OCMEROS, WINO)
//
// Disclaimers      : This program code is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
//                      of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//                    The official Nodo development team will not be responsible for the correct operation of these source code extentions,
//                      nor can they be held responsible for any kind of support.
//
//                    This program source code depends heavily on the main Nodo Source Code. Any changes to the main Nodo Code and additions
//                      to this main Nodo code might have a serious impact on the operation of the software in its entirity.
//
//                    Usage of these source code extensions can have a serious impact on the basic Nodo operation. Before dropping a support request,
//                      it is adviced to re-evaluate your systems operation with the userplugin extensions disabled.
//
//                    Updates will be distributed through the Nodo forum website without any form of regression testing,
//                      since no single member has full access to all supported hardware devices!!!
//
//                    This program code makes use of RAM and EEPROM space without any form of regulation towards the Main Code Devevopment
//                    It also addresses hardware directly in several cases and may interfere with some Nodo commands
//                    Running this code on unmodified hardware can lead to improper operation or even hardware failure!!!
//  
//                    This source code should be considered beta software and it probably will remain as such...
//
// Plugin name      : <choose any name you prefer...>
//
// Date             : 18-11-2012
//
#define UP_VERSION    "0.13.4"
//
// Nodo version     : Beta 467 (or a customized version, based on legacy edition 1.2.1.)
//
// Purpose          : The main purpose of these code extensions is the (limited) support for hardware devices beyond the scope of the original software:
//                    Selected Oregon and Alecto remote weather sensors for temperature, humidity, windspeed, ..
//                    Home Easy EU devices (similar to KAKU, but different protocol)
//                    Selected LCD Displays (supported I2C types only!)
//                    DHT11 one-wire Temperature and Humidity sensor
//                    BMP085 I2C Barometric pressure sensor
//                    ID-12 RFID tag reader
//                    Receiving and broadcasting UDP messages
//                    I2C MultiMaster Communication between Nodo Units
//
// Known Issues
//                    Nodo Timers may intermittently stop working (still under investigation)
//                    Nodo Sendto command might lead to a complete system crash on the Central Mega Nodo (still under investigation)
//                    I2C Multimaster traffic can cause unaddressed conflicts on the I2C bus.
//
// Usage conditions & Limitations:
//                    Home Easy devices, only On/Off is supported for a maximum of 16 channels
//                    LCD display needs library <LiquidCrystal_I2C.h>
//                    ID-12 sensor has only been tested on a custom Nodo system and needs a software serial library
//                    BMP085 sensor needs library <Adafruit_BMP085.h>
//                    Using UDP sockets leads to a temporary disruption of HTTP traffic in case of concurrent telnet usage.
//*********************************************************************************************/

//*********************************************************************************************/
// Defines for conditinal compilation:
//*********************************************************************************************/

//#define USE_RAWSIGNAL_2_USERPLUGIN // Some weather systems (like WS3500, WS1200) need this setting ENABLED

                              //   Custom SW        Custom HW
//#define WEATHER_SENSOR      //     N                 N
//#define OREGON_V2           //     N                 N
//#define ALECTO_V1           //     N                 N
//#define ALECTO_V2           //     N                 N/Y (Some systems need a 868MHz receiver)
//#define ALECTO_V3           //     N                 N
//#define HOME_EASY_SEND      //     N                 N
//#define HOME_EASY_RECV      //     N                 N
//#define UDP_BROADCAST       //     N                 N
//#define LCD_I2C             //     N                 Y (I2C LCD connected to SDA/SCL)
//#define DHT11               //     N                 Y (DHT 11 sensor connected to D7 on Small or D on Mega)
//#define BMP085              //     N                 Y (BMP085 sensor connected to SDA/SCL)
//#define PULSECOUNTERS       //     Y                 Y (121 Custom version only)
//#define PULSECOUNTER1       //     Y                 Y (121 Custom version only, sensor connected to Pin D2 on Small)
//#define PULSECOUNTER2       //     Y                 Y (121 Custom version only, sensor connected to Pin D3 on Small)
//#define PULSECOUNTER3       //     Y                 Y (121 Custom version only, sensor connected to Pin A0 on Small)
//#define RFID                //     Y                 Y (RFID serial out connected to Pin D3 on Small boards)
//#define I2C                 //     N                 N
//#define I2C_MASTER          //     N                 N
//#define I2C_SLAVE           //     N                 N
//#define I2C_POLLING         //     N                 N
  #define MINI_MMI            //     N                 N
  #define AUTORESET           //     Y                 N (Pin D6 connected to reset pin, no sound device!)

//*********************************************************************************************/
// Defines for variables used by this userplugin
//*********************************************************************************************/


//===================================================================================================================================
// End of user configurable settings, do not change anything below this section!
//===================================================================================================================================
#ifdef USER_PLUGIN

#ifdef NODO_CLASSIC
#else
#if NODO_MEGA
#else
// Nodo Small stuff
#define INPUT_BUFFER_SIZE          40  // Buffer waar de karakters van de seriele/IP poort in worden opgeslagen.
#endif
#endif

// temp workaround, needs work...
#ifdef NODO_CLASSIC
#else
char TempString[INPUT_BUFFER_SIZE+2];
#endif

byte userPluginSendMode = 1;

/*********************************************************************************************\ 
 * Dit deel van de plugin wordt eenmalig aangeroepen direct na boot en tonen welkomsttekst
 \*********************************************************************************************/
void UserPlugin_Init(void)
{
#ifdef PULSECOUNTERS
  UserPlugin_PC_Init();
#endif

#ifdef UDP_BROADCAST
  UserPlugin_UDP_Init();
#endif

#ifdef LCD_I2C
  UserPlugin_LCD_Init();
#endif

#ifdef BMP085
  UserPlugin_BMP_Init();
#endif

#ifdef RFID
  UserPlugin_RFID_Init();
#endif

#ifdef I2C
  UserPlugin_I2C_Init();
#endif

#ifdef NODO_CLASSIC
  // init vars, 32767 means not in use
  for (byte x=0; x <= 15; x++) UserVar[x]=32767;
#endif

}

/*********************************************************************************************\ 
 * Dit deel van de plugin wordt aangeroepen met het Nodo commando "UserPlugin Par1,Par2"
 * 
 * TIPS:
 * - Par1 en Par2 zijn twee parameters die je kunt meegeven. Deze kunnen beide een waarde hebben van 0..255
 * - De userplugin kan worden aangeroepen als commando of als actie vanuit de eventlist.
 * - Gebruikersvariabelen kunnen worden gelezen, waarbij S.UserVar[0] gelijk is aan de eerste gebruikersvariabele.
 \*********************************************************************************************/

void UserPlugin_Command(int Par1, int Par2)
{
#ifdef WEATHER_SENSOR
  UserPlugin_WS_Command(Par1, Par2);
#endif

#ifdef I2C
  UserPlugin_I2C_Command(Par1, Par2);
#endif

#ifdef HOME_EASY_SEND
  UserPlugin_HE_Command(Par1, Par2);
#endif

#ifdef DHT11
  UserPlugin_DHT_Command(Par1, Par2);
#endif

#ifdef BMP085
  UserPlugin_BMP_Command(Par1, Par2);
#endif

#ifdef RFID
  UserPlugin_RFID_Command(Par1, Par2);
#endif

  if (Par1 == 254) UserPlugin_FreeMemory();

  if (Par1 == 255) userPluginCompileInfo();

}

/*********************************************************************************************\ 
 * Dit deel van de plugin wordt altijd aangeroepen direct na ontvangst van een signaal
 * 
 * TIPS:
 * - Rawsignal is de tabel met de ontvangen impulsen van IR of RF. Deze kan worden gebruikt 
 *   voor verwerken van signalen.
 * - Gebruik deze functie alleen voor onderscheppen van events
 * - Deze functie wordt bij ieder binnenkomend event doorlopen, zorg er voor dat hier
 *   geen onnodige en/of tijdrovende verwerking plaatsvindt.
 \*********************************************************************************************/

boolean UserPlugin_Receive(unsigned long Event)
{

#ifdef WEATHER_SENSOR
  if (!UserPlugin_WS_Receive(Event)) return false;
#endif

#ifdef UDP_BROADCAST
  if (!UserPlugin_UDP_Receive(Event)) return false;
#endif

#ifdef I2C
  if (!UserPlugin_I2C_Receive(Event)) return false;
#endif

  return true;
}

/*********************************************************************************************\ 
 * UserPlugin_Periodically() wordt ongeveer eenmaal per seconde aangeroepen.
 * Het het bedoeld voor taken die korte doorlooptijd vragen zoals checken van zaken.
 *
 * TIPS:
 * - door millis() op te slaan in variabelen, kunnen exacte tijden worden berekend.
 * - De variabele PulseCounter houdt bij hoeveel pulsen hoog-laag-hoog pulsen er zijn geweest.
 * - In deze functie geen tijdrovende taken verrichten anders worden en RF of IR signalen gemist
 \*********************************************************************************************/

void UserPlugin_Periodically()
{
#ifdef UDP_BROADCAST
  UserPlugin_UDP_Periodically();
#endif

#ifdef RFID
  UserPlugin_RFID_Periodically();
#endif

#ifdef I2C
  UserPlugin_I2C_Periodically();
#endif

#ifdef MINI_MMI
  UserPlugin_MMI_Periodically();
#endif

  return;
}

/*********************************************************************************************\ 
 * Called directly after RF message received
 \*********************************************************************************************/
unsigned long RawSignal_2_UserPlugin(int Port)
{
  unsigned long code = 0;

#ifdef WEATHER_SENSOR
  if (code == 0 ) code = RawSignal_2_UserPlugin_WS(Port);
#endif

#ifdef HOME_EASY_RECV
  if (code == 0 ) code = RawSignal_2_UserPlugin_HE(Port);
#endif

  // return 0    = Nodo continues to analyze rawsignal
  // return 1    = Nodo stops processing for this event
  // return <> 0 = Node continues with returncode as Event
  return code; 
}

/*********************************************************************************************\ 
 * Information about which modules are selected at compile time
 \*********************************************************************************************/
void userPluginCompileInfo(void)
{
#ifdef NODO_CLASSIC
  TempString[0]=0;
  strcat(TempString,"Custom:");
#ifdef KAKU_ENABLED
  strcat(TempString,"KK,");
#endif
#ifdef SOUND_ENABLED
  strcat(TempString,"SND,");
#endif
#ifdef CLOCK_ENABLED
  strcat(TempString,"CLK,");
#endif
#ifdef RF_ENABLED
  strcat(TempString,"RF,");
#endif
#ifdef IR_ENABLED
  strcat(TempString,"IR,");
#endif
#ifdef RF2_ENABLED
  strcat(TempString,"RF2,");
#endif
#ifdef ANALYZE_ENABLED
  strcat(TempString,"AN,");
#endif
#ifdef PULSECOUNTER3
  strcat(TempString,"PC,");
#endif
  userPluginPrintTerminal(TempString);
#endif // NODO CLASSIC

#ifdef NODO_CUSTOM
  TempString[0]=0;
  strcat(TempString,"Custom:");
  strcat(TempString,NODO_CUSTOM);
  userPluginPrintTerminal(TempString);
#endif

  TempString[0]=0;
  strcat(TempString,"UP ");
  strcat(TempString,UP_VERSION);
  strcat(TempString," :");

#ifdef OREGON_V2
  strcat(TempString,"OG2,");
#endif
#ifdef ALECTO_V1
  strcat(TempString,"AL1,");
#endif
#ifdef ALECTO_V2
  strcat(TempString,"AL2,");
#endif
#ifdef ALECTO_V3
  strcat(TempString,"AL3,");
#endif
#ifdef HOME_EASY_SEND
  strcat(TempString,"HES,");
#endif
#ifdef HOME_EASY_RECV
  strcat(TempString,"HER,");
#endif
#ifdef UDP_BROADCAST
  strcat(TempString,"UDP,");
#endif
#ifdef LCD_I2C
  strcat(TempString,"LCD,");
#endif
#ifdef DHT11
  strcat(TempString,"DHT,");
#endif
#ifdef PULSECOUNTERS
  strcat(TempString,"PC,");
#endif
#ifdef PULSECOUNTER1
  strcat(TempString,"PC1,");
#endif
#ifdef PULSECOUNTER2
  strcat(TempString,"PC2,");
#endif
#ifdef PULSECOUNTER3
  strcat(TempString,"PC3,");
#endif
#ifdef RFID
  strcat(TempString,"RFID,");
#endif
#ifdef I2C_MASTER
  strcat(TempString,"I2CM,");
#endif
#ifdef I2C_SLAVE
  strcat(TempString,"I2CS,");
#endif
#ifdef I2C_POLLING
  strcat(TempString,"I2CP,");
#endif
  userPluginPrintTerminal(TempString);
}

/*********************************************************************************************\ 
 * Send userplugin event data
 \*********************************************************************************************/
void userPluginSendEvent(unsigned long Event)
{

#if NODO_MEGA
  if (userPluginSendMode == 1)
  {
    SendHTTPEvent(Event);
    PrintEvent(Event,VALUE_SOURCE_HTTP,VALUE_DIRECTION_OUTPUT);
  }
#endif

  //#ifdef NODO_CLASSIC
  if (userPluginSendMode == 2)
  {
    Nodo_2_RawSignal(Event);
    RawSendRF();
    delay(1000);
  }
  //#endif

#ifdef I2C_MASTER
userPluginSendEvent_I2C(Event);
#endif

}

/*********************************************************************************************\
 * Custom printroutine (LCD, UDP)
 \*********************************************************************************************/

void userPluginPrintTerminal(char* LineToPrint)
{

  PrintTerminal(LineToPrint);

#ifdef UDP_BROADCAST
  // broadcast sensor data through UDP packet
  BroadcastUDP(LineToPrint);
#endif

#ifdef LCD_I2C
  lcdprint(LineToPrint);
#endif

}

#if NODO_MEGA
/**********************************************************************************************\
 * Save data to file
 \*********************************************************************************************/
boolean userPluginSaveToSDCard(char *FileName, unsigned int Data)
{
  boolean r;
  byte Data1 = 0;
  byte Data2 = 0;

  // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer SDCard chip
  digitalWrite(Ethernetshield_CS_W5100, HIGH);
  digitalWrite(EthernetShield_CS_SDCard,LOW);

  File DataFile = SD.open(FileName, FILE_WRITE);
  if(DataFile) 
  {
    r=true;
    Data1 = Data / 256;
    Data2 = Data - (256 * Data1);
    DataFile.seek(0);
    DataFile.write(Data1);
    DataFile.write(Data2);
    DataFile.close();
  }
  else 
    r=false;

  // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer W510 chip
  digitalWrite(EthernetShield_CS_SDCard,HIGH);
  digitalWrite(Ethernetshield_CS_W5100, LOW);

  return r;
}

/**********************************************************************************************\
 * Read data from file
 \*********************************************************************************************/
unsigned int userPluginReadFromSDCard(char *FileName)
{
  unsigned int Data=0;
  byte TempData=0;

  // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer SDCard chip
  digitalWrite(Ethernetshield_CS_W5100, HIGH);
  digitalWrite(EthernetShield_CS_SDCard,LOW);

  File DataFile = SD.open(FileName, FILE_READ);
  if(DataFile) 
  {
    TempData = DataFile.read();
    Data = 256 * TempData;
    TempData = DataFile.read();
    Data = Data + TempData;    
    DataFile.close();
  }

  // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer W510 chip
  digitalWrite(EthernetShield_CS_SDCard,HIGH);
  digitalWrite(Ethernetshield_CS_W5100, LOW);

  return Data;
}
#endif


/*********************************************************************************************\
 * EEPROM stuff
 \*********************************************************************************************/
void userPluginEeprom_save(int address, unsigned long Value)
{
  EEPROM.write(address,(Value>>24  & 0xFF));
  EEPROM.write(address+1,(Value>>16  & 0xFF));
  EEPROM.write(address+2,(Value>> 8  & 0xFF));
  EEPROM.write(address+3,(Value      & 0xFF));
}

/*********************************************************************************************\
 * EEPROM stuff
 \*********************************************************************************************/
unsigned long userPluginEeprom_load(int address)
{
  unsigned long Value=0;
  Value  = ((unsigned long)(EEPROM.read(address))) << 24;
  Value |= ((unsigned long)(EEPROM.read(address+1))) << 16;
  Value |= ((unsigned long)(EEPROM.read(address+2))) <<  8;
  Value |= ((unsigned long)(EEPROM.read(address+3)))      ;
  return Value;
}

/*********************************************************************************************\
 * Statistics, free memory info
 \*********************************************************************************************/
uint8_t *up_heapptr, *up_stackptr;
void UserPlugin_FreeMemory(void) 
{
  up_stackptr = (uint8_t *)malloc(4);          // use stackptr temporarily
  up_heapptr = up_stackptr;                     // save value of heap pointer
  free(up_stackptr);      // free up the memory again (sets stackptr to 0)
  up_stackptr =  (uint8_t *)(SP);           // save value of stack pointer
  Serial.print("Free RAM:");
  Serial.println(up_stackptr-up_heapptr,DEC);
}

#ifdef NODO_CLASSIC
#else
#if NODO_MEGA
#else
// Nodo Small stuff...

char* int2str(unsigned long x)
{
  static char OutputLine[12];
  char* OutputLinePosPtr=&OutputLine[10];
  int y;

  *OutputLinePosPtr=0;

  if(x==0)
  {
    *--OutputLinePosPtr='0';
  }

  else if(x<=10000)  // getallen weergeven als een integer in decimaal formaat
  {
    while(x>0)
    {
      *--OutputLinePosPtr='0'+(x%10);
      x/=10;
    }
  }

  else // getallen weergeven als een integer in hexadecimaal formaat
  {
    while(x>0)
    {
      y=x&0xf;

      if(y<10)
        *--OutputLinePosPtr='0'+y;
      else
        *--OutputLinePosPtr='A'+(y-10);

      x=x>>4;
      ;
    }
    *--OutputLinePosPtr='x';
    *--OutputLinePosPtr='0';
  }

  return OutputLinePosPtr;
}

void PrintTerminal(char *Line)
{
  Serial.println(Line);
}

#endif  // MEGA
#endif  // CLASSIC


#ifdef AUTORESET
void UserPlugin_AutoReset(void)
{
  // Custom autoreset feature, needs pin 12 connected to reset pin
  // triggers on value 0x84 (init bootloader command as it's seen on 57600 baud, 0x30 becomes 0x84)  
  if(Serial.available())
  {
    if (Serial.peek() == 0x84)
    {
      pinMode(6, OUTPUT);
      digitalWrite(6, LOW);
    }
  }  
}
#endif

unsigned long AnalogInt2event(int wi, byte port, byte cmd)
  {
  boolean high=false;
  boolean sign=false;
  
  if(wi<0)
    {
    sign=true;
    wi=-wi;
    }
    
  if(wi>=1000)
    {
    high=true;
    wi=wi/10;
    }

  return ((unsigned long)SIGNAL_TYPE_NODO)<<28   |
         ((unsigned long)Settings.Unit)<<24             | 
         ((unsigned long)cmd)<<16                |
         ((unsigned long)port)<<12               |
         ((unsigned long)high)<<11               |
         ((unsigned long)sign)<<10               |
         ((unsigned long)(wi & 0x3ff));
  }
  
#endif // USER_PLUGIN

