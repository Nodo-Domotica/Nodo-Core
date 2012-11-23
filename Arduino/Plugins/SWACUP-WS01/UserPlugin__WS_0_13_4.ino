#ifdef WEATHER_SENSOR
//*********************************************************************************************/
// Defines for variables used by this userplugin
//*********************************************************************************************/

#define OREGON_V2_TEMP_BASE_VAR     11 // for channel 1; channel 2 becomes +1 , channel 3 becomes +2
#define OREGON_V2_HUMID_BASE_VAR    14 // for channel 1; channel 2 becomes +1 , channel 3 becomes +2

#define ALECTO_V1_TEMPERATURE_VAR   14
#define ALECTO_V1_HUMIDITY_VAR      14
#define ALECTO_V1_RAIN_VAR          14
#define ALECTO_V1_WIND_VAR          14
#define ALECTO_V1_WINDGUST_VAR      14
#define ALECTO_V1_WINDDIRECTION_VAR 14

#define ALECTO_V2_TEMPERATURE_VAR   2
#define ALECTO_V2_HUMIDITY_VAR      3
#define ALECTO_V2_RAIN_VAR          4
#define ALECTO_V2_WIND_VAR          7
#define ALECTO_V2_WINDGUST_VAR      8
#define ALECTO_V2_WINDDIRECTION_VAR 9

#define ALECTO_V3_TEMPERATURE_VAR   14
#define ALECTO_V3_HUMIDITY_VAR      14
#define ALECTO_V3_RAIN_VAR          14

//===================================================================================================================================
// End of user configurable settings, do not change anything below this section!
//===================================================================================================================================

#if NODO_MEGA
#define USERPLUGIN_EEPROM_RAINBASE 4000
#else
#define USERPLUGIN_EEPROM_RAINBASE 1000
#endif

byte userPluginValidID[5];
unsigned int userPluginRainBase=0;
unsigned int userPluginRain=0;

//*********************************************************************************************
void UserPlugin_WS_Init(void)
//*********************************************************************************************
{

}

//********************************************************************************************* 
void UserPlugin_WS_Command(int Par1, int Par2)
//*********************************************************************************************
{
  if ((Par1 >= 1) && (Par1 <=5))
  {
    userPluginValidID[Par1-1] = Par2;
    TempString[0]=0;
    strcat(TempString,"RF Valid ID:");
    for (byte x=0; x<5; x++)
    {
      strcat(TempString,int2str(userPluginValidID[x]));
      strcat(TempString,",");
    }
    userPluginPrintTerminal(TempString);
  }

  if (Par1 == 6) userPluginSendMode = Par2;

  if ((Par1 == 7) && (Par2 == 1))
  {
    TempString[0]=0;
    strcat(TempString,"Reset rain and save RainBase Value:");
    strcat(TempString,int2str(userPluginRain));
    userPluginPrintTerminal(TempString);
    userPluginRainBase = userPluginRain;
#if NODO_MEGA
    SD.mkdir("userplug");
    userPluginSaveToSDCard("userplug/rain.dat", userPluginRainBase); 
#else
    userPluginEeprom_save(USERPLUGIN_EEPROM_RAINBASE, userPluginRainBase);
#endif
  }

  if ((Par1 == 7) && (Par2 == 2))
  {
#if NODO_MEGA
    userPluginRainBase = userPluginReadFromSDCard("userplug/rain.dat"); 
#else
    userPluginRainBase = userPluginEeprom_load(USERPLUGIN_EEPROM_RAINBASE);
#endif
    TempString[0]=0;
    strcat(TempString,"Read RainBase Value:");
    strcat(TempString,int2str(userPluginRainBase));
    userPluginPrintTerminal(TempString);
  }
}


//*********************************************************************************************
boolean UserPlugin_WS_Receive(unsigned long Event)
//*********************************************************************************************
{
  byte unit = (Event >> 24) & 0xf;
  byte command = (Event >> 16) & 0xff;
  byte par1 = (Event >> 8) & 0xff;

  byte code=0;

  if(Settings.Debug==VALUE_ON)
  {
    TempString[0]=0;
    strcat(TempString,"RF PacketSize:");
    strcat(TempString,int2str(RawSignal.Number));
    userPluginPrintTerminal(TempString);

    byte pulses = RawSignal.Number;
    if (pulses > 20) pulses = 20;
    UserPluginPrintRawSignal(pulses);
  }

#ifdef USE_RAWSIGNAL_2_USERPLUGIN
#else
#ifdef OREGON_V2
  if ((code == 0) && RawSignal.Number > 195 && RawSignal.Number < 206) code = Oregon_V2_Decode();
  if ((code == 0) && RawSignal.Number > 227 && RawSignal.Number < 239) code = Oregon_V2_Decode();
#endif

#ifdef ALECTO_V1
  if ((code == 0) && RawSignal.Number == 74) code = Alecto_V1_Decode();
#endif

#ifdef ALECTO_V2
  if ((code == 0) && ((RawSignal.Number == 160) || (RawSignal.Number == 176))) code = Alecto_V2_Decode();
#endif

#ifdef ALECTO_V3
  if ((code == 0) && ((RawSignal.Number == 94) || (RawSignal.Number == 126))) code = Alecto_V3_Decode();
#endif

#endif

  // funktie kan op twee manieren worden verlaten:
  // true = Nodo gaat verder met verwerking van het event
  // false = Event wordt niet verder behandeld.
  if (code == 0) return true;
  return false;
}

//*********************************************************************************************
void UserPlugin_WS_Periodically()
//*********************************************************************************************
{
  return;
}

//*********************************************************************************************
unsigned long RawSignal_2_UserPlugin_WS(int Port)
//*********************************************************************************************
{

  if(Settings.Debug==VALUE_ON)
  {
    TempString[0]=0;
    strcat(TempString,"RF PacketSize:");
    strcat(TempString,int2str(RawSignal.Number));
    userPluginPrintTerminal(TempString);

    byte pulses = RawSignal.Number;
    if (pulses > 20) pulses = 20;
    UserPluginPrintRawSignal(pulses);
  }

  unsigned long code = 0;
#ifdef USE_RAWSIGNAL_2_USERPLUGIN
#ifdef OREGON_V2
  if ((code == 0) && RawSignal.Number > 195 && RawSignal.Number < 206) code = Oregon_V2_Decode();
  if ((code == 0) && RawSignal.Number > 227 && RawSignal.Number < 239) code = Oregon_V2_Decode();
#endif

#ifdef ALECTO_V1
  if ((code == 0) && RawSignal.Number == 74) code = Alecto_V1_Decode();
#endif

#ifdef ALECTO_V2
  if ((code == 0) && ((RawSignal.Number == 160) || (RawSignal.Number == 176))) code = Alecto_V2_Decode();
#endif

#ifdef ALECTO_V3
  if ((code == 0) && ((RawSignal.Number == 94) || (RawSignal.Number == 126))) code = Alecto_V3_Decode();
#endif

#ifdef HOME_EASY_RECV
  if ((code == 0) && RawSignal.Number == 116) code = HomeEasy__EU_Decode();
#endif
#endif

  // return 0    = Nodo continues to analyze rawsignal
  // return 1    = Nodo stops processing for this event
  // return <> 0 = Node continues with returncode as Event
  return code; 
}

/*********************************************************************************************\
 * Decodes RF signals from Oregon Scientific remote temperature sensor.
 * Code is in 'beta' stage, use at own risc, side effects to main Nodo Code are unknown.
 * Tested using Oregon Scientific THN132N remote sensor.
 * Other sensor types may work, but no guarantees what-so-ever!
 * Sends a 'rawsignal' through serial bus using message structure: "ID, Channel, Measured Value"
 * Supports only Oregon V2 protocol messages, message format:
 * AAAAAAAAAAAAAAAABBBBCCCCCCCCCCCCCCCCDDDDEEEEEEEEFFFFGGGGGGGGGGGGHHHHIIIIIIII
 *   A = preamble, B = sync bits, C = ID, D = Channel, E = RC, F = Flags,
 *   G = Measured value, 3 digits BCD encoded, H = sign, bit3 set if negative temperature
 *   I = Checksum, sum of nibbles C,D,E,F,G,H
 \*********************************************************************************************/
#ifdef OREGON_V2
unsigned long Oregon_V2_Decode(void)
{
  int y = 1;
  int c = 1;
  int startpulses = 0;
  unsigned long rfbit = 1;
  unsigned long sync = 0; 
  unsigned long id = 0;
  unsigned long channel = 0;
  unsigned long rc = 0;
  unsigned long flags = 0;
  unsigned long data = 0;
  unsigned long sign = 0;
  unsigned long relhum = 0;
  unsigned long unknown = 0;
  unsigned long checksum = 0;
  byte checksumcalc = 0;
  byte digit1, digit2, digit3, digit4, digit5;
  int datavalue = 0;
  unsigned long customEvent;

  for(byte x=1;x<=RawSignal.Number;x++)
  {
    if(RawSignal.Pulses[x] < 600)
    {
      rfbit = (RawSignal.Pulses[x] < RawSignal.Pulses[x+1]);
      x++;
      y = 2;
    }
    if (y%2 == 1)
    {
      // Find sync as THN132N and THGN132N have different preamble length
      if (c == 1)
      {
        sync = (sync >> 1) | (rfbit << 3);
        sync = sync & 0xf;
        if (sync == 0xA) 
        {
          c = 2;
          if (x < 40) return 0L;
        }
      }
      else
      {
        if ((c>=2) && (c<=17)) id = (id >> 1) | (rfbit << 15);
        if ((c>=18) && (c<=21)) channel = (channel >> 1) | (rfbit << 3);
        if ((c>=22) && (c<=29)) rc = (rc >> 1) | (rfbit << 7);
        if ((c>=30) && (c<=33)) flags = (flags >> 1) | (rfbit << 3);
        if ((c>=34) && (c<=45)) data = (data >> 1) | (rfbit << 11);
        if ((c>=46) && (c<=49)) sign = (sign >> 1) | (rfbit << 3);
        if (id == 1230) // THN132N
        {
          if ((c>=50) && (c<=57)) checksum = (checksum >> 1) | (rfbit << 7);
        }
        if ((id == 721) || (id == 17039)) // THGN132N, THGN123N, THGR122NX, THGR810
        {
          if ((c>=50) && (c<=57)) relhum = (relhum >> 1) | (rfbit << 7);
          if ((c>=58) && (c<=61)) unknown = (unknown >> 1) | (rfbit << 3);
          if ((c>=62) && (c<=69)) checksum = (checksum >> 1) | (rfbit << 7);
        }
        c++;
      }
    }
    y++;
  }

  if (c == 1) return 0L;

  digit3 = data & 0xf;
  digit2 = (data >> 4) & 0xf;
  digit1 = (data >> 8) & 0xf;
  checksumcalc = ((id & 0xf000) >> 12) + ((id & 0x0f00) >> 8) + ((id & 0x00f0) >> 4) + (id & 0x000f) + ((rc & 0xf0) >> 4) + (rc & 0x0f) + channel + flags + digit1 + digit2 + digit3 + sign;
  if ((id == 721) || (id == 17039))
  {
    digit5 = relhum & 0xf;
    digit4 = (relhum >> 4) & 0xf;
    checksumcalc = checksumcalc + digit4 + digit5 + unknown;
  }

  if(Settings.Debug==VALUE_ON) userPluginPrintChecksum("OregonV2", checksum, checksumcalc);

  if (checksum != checksumcalc) return 0L;
  if (!userPluginCheckID("OregonV2", rc)) return 0L;

  // if valid sensor type (1230, 721 or 17039), update user variable and process event
  if ((id == 721) || (id == 17039) || (id == 1230))
  {
    // channel code is received as 1,2,4, convert this to 0,1,2
    channel--;
    if (channel == 3) channel = 2;
    if ((channel >= 0) & (channel <= 2))
    {
      datavalue = ((1000 * digit1) + (100 * digit2) + (10 * digit3));
      if ((sign & 0x8) == 8) datavalue = -1 * datavalue;
      UserVar[OREGON_V2_TEMP_BASE_VAR + channel -1] = (float)datavalue/100;
      if ((id == 721) || (id == 17039))
      {
        datavalue = ((1000 * digit4) + (100 * digit5));
        UserVar[OREGON_V2_HUMID_BASE_VAR + channel -1] = (float)datavalue/100;
      }

      if (userPluginSendMode != 0)
      {
        customEvent=float2event(UserVar[OREGON_V2_TEMP_BASE_VAR + channel -1], OREGON_V2_TEMP_BASE_VAR + channel -1, CMD_VARIABLE_EVENT);
        userPluginSendEvent(customEvent);
        if ((id == 721) || (id == 17039))
        {
          customEvent=float2event(UserVar[OREGON_V2_HUMID_BASE_VAR + channel -1], OREGON_V2_HUMID_BASE_VAR + channel -1, CMD_VARIABLE_EVENT);
          userPluginSendEvent(customEvent);
        }
      }

    }
  }

  // Send result to terminal as a string 'OregonV2,<sensortype>,<channel>,<rolling ID code><datatype>,<value>
  TempString[0]=0;
  if ((sign & 0x8) == 8) strcat(TempString,"-");
  strcat(TempString,int2str(digit1));
  strcat(TempString,int2str(digit2));
  strcat(TempString,".");
  strcat(TempString,int2str(digit3));
  userPluginPrintSensorData("OregonV2", id, channel+1, rc, "T", TempString);

  if ((id == 721) || (id == 17039))
  {
    // Send result to terminal as a string 'OregonV2,<sensortype>,<channel>,<rolling ID code><datatype>,<value>
    TempString[0]=0;
    strcat(TempString,int2str(digit4));
    strcat(TempString,int2str(digit5));
    userPluginPrintSensorData("OregonV2", id, channel+1, rc, "H", TempString);
  }

  RawSignal.Number=0;
  return 1L;

}
#endif

/*********************************************************************************************\
 * Decodes signals from Alecto Weatherstation outdoor unit, type 1 (74 pulses, 36 bits, 433 MHz).
 * Code is in beta stage, use at own risc, side effects to main Nodo code are unknown.
 * Tested using Alecto WS3500.
 * Other sensor units may work, but no guarantees what-so-ever!
 * Version 0.0.10
 * 08-10-2012
 * Work to be done:
 *     Improve code
 * Message Format: (9 nibbles, 36 bits):
 *
 * Format for Temperature Humidity
 *   AAAAAAAA BBBB CCCC CCCC CCCC DDDDDDDD EEEE
 *   RC       Type Temperature___ Humidity Checksum
 *   A = Rolling Code
 *   B = Message type (xyyx = temp/humidity if yy <> '11')
 *   C = Temperature (two's complement)
 *   D = Humidity BCD format
 *   E = Checksum
 *
 * Format for Rain
 *   AAAAAAAA BBBB CCCC DDDD DDDD DDDD DDDD EEEE
 *   RC       Type      Rain                Checksum
 *   A = Rolling Code
 *   B = Message type (xyyx = NON temp/humidity data if yy = '11')
 *   C = fixed to 1100
 *   D = Rain (bitvalue * 0.25 mm)
 *   E = Checksum
 *
 * Format for Windspeed
 *   AAAAAAAA BBBB CCCC CCCC CCCC DDDDDDDD EEEE
 *   RC       Type                Windspd  Checksum
 *   A = Rolling Code
 *   B = Message type (xyyx = NON temp/humidity data if yy = '11')
 *   C = Fixed to 1000 0000 0000
 *   D = Windspeed  (bitvalue * 0.2 m/s, correction for webapp = 3600/1000 * 0.2 * 100 = 72)
 *   E = Checksum
 *
 * Format for Winddirection & Windgust
 *   AAAAAAAA BBBB CCCD DDDD DDDD EEEEEEEE FFFF
 *   RC       Type      Winddir   Windgust Checksum
 *   A = Rolling Code
 *   B = Message type (xyyx = NON temp/humidity data if yy = '11')
 *   C = Fixed to 111
 *   D = Wind direction
 *   E = Windgust (bitvalue * 0.2 m/s, correction for webapp = 3600/1000 * 0.2 * 100 = 72)
 *   F = Checksum
 \*********************************************************************************************/
#ifdef ALECTO_V1
unsigned long Alecto_V1_Decode(void)
{
  unsigned long bitstream=0;
  byte nibble0=0;
  byte nibble1=0;
  byte nibble2=0;
  byte nibble3=0;
  byte nibble4=0;
  byte nibble5=0;
  byte nibble6=0;
  byte nibble7=0;
  byte checksum=0;
  long temperature=0;
  long humidity=0;
  unsigned long rain=0;
  byte windspeed=0;
  byte windgust=0;
  unsigned long winddirection=0;
  unsigned long customEvent;
  byte checksumcalc = 0;
  byte rc=0;
  byte channel=0;
  int C1=0;
  int C2=0;

  if (RawSignal.Number != 74) return 0L;

  for(byte x=2; x<=64; x=x+2)
  {
    if(RawSignal.Pulses[x] > 0xA00) bitstream = ((bitstream >> 1) |(0x1L << 31)); 
    else bitstream = (bitstream >> 1);
  }

  for(byte x=66; x<=72; x=x+2)
  {
    if(RawSignal.Pulses[x] > 0xA00) checksum = ((checksum >> 1) |(0x1L << 3)); 
    else checksum = (checksum >> 1);
  }

  nibble7 = (bitstream >> 28) & 0xf;
  nibble6 = (bitstream >> 24) & 0xf;
  nibble5 = (bitstream >> 20) & 0xf;
  nibble4 = (bitstream >> 16) & 0xf;
  nibble3 = (bitstream >> 12) & 0xf;
  nibble2 = (bitstream >> 8) & 0xf;
  nibble1 = (bitstream >> 4) & 0xf;
  nibble0 = bitstream & 0xf;

  // checksum calculations
  if ((nibble2 & 0x6) != 6) {
    checksumcalc = (0xf - nibble0 - nibble1 - nibble2 - nibble3 - nibble4 - nibble5 - nibble6 - nibble7) & 0xf;
  }
  else
  {
    if (nibble3 == 3)
      checksumcalc = (0x7 + nibble0 + nibble1 + nibble2 + nibble3 + nibble4 + nibble5 + nibble6 + nibble7) & 0xf;
    else
      checksumcalc = (0xf - nibble0 - nibble1 - nibble2 - nibble3 - nibble4 - nibble5 - nibble6 - nibble7) & 0xf;
  }

  if(Settings.Debug==VALUE_ON) userPluginPrintChecksum("AlectoV1", checksum, checksumcalc);

  if (checksum != checksumcalc) return 0L;

  rc = bitstream & 0xff;
  channel = (bitstream >> 4) & 0x3;

  if (!userPluginCheckID("AlectoV1", rc)) return 0L;

  // temporary workaround, channel bit order seems reversed ?? Or this is not really meant to work the way we think...???
  // it seems that channel bits are stuffed into the sensor id to make sure that the ID will at least be unique per channel 
  if (channel == 1) channel = 2;
  else
    if (channel == 2) channel = 1;

  if ((nibble2 & 0x6) != 6) {

    temperature = (bitstream >> 12) & 0xfff;
    //fix 12 bit signed number conversion
    if ((temperature & 0x800) == 0x800) temperature = temperature - 0x1000;
    UserVar[ALECTO_V1_TEMPERATURE_VAR-1] = (float)temperature/10;

    TempString[0]=0;
    if (temperature < 0) {
      temperature = -temperature;
      strcat(TempString,"-");
    } 
    C1 = temperature / 10;
    C2 = temperature - (C1 * 10);
    strcat(TempString,int2str(C1));
    strcat(TempString,".");
    strcat(TempString,int2str(C2));
    userPluginPrintSensorData("AlectoV1", 0, channel, rc, "T", TempString);
    if (userPluginSendMode == 1)
    {
      customEvent=float2event(UserVar[ALECTO_V1_TEMPERATURE_VAR-1], ALECTO_V1_TEMPERATURE_VAR -1, CMD_VARIABLE_EVENT);
      userPluginSendEvent(customEvent);
    }

    humidity = (10 * nibble7) + nibble6;
    userPluginPrintSensorData("AlectoV1", 0, channel, rc, "H", int2str(humidity));
    UserVar[ALECTO_V1_HUMIDITY_VAR-1] = (float)humidity;
    if (userPluginSendMode == 1)
    {
      customEvent=float2event(UserVar[ALECTO_V1_HUMIDITY_VAR-1], ALECTO_V1_HUMIDITY_VAR -1, CMD_VARIABLE_EVENT);
      userPluginSendEvent(customEvent);
    }
    RawSignal.Number=0;
    return 1L;
  }
  else
  {
    if (nibble3 == 3)
    {
      userPluginRain = ((bitstream >> 16) & 0xffff);
      rain = userPluginRain - userPluginRainBase;
      userPluginPrintSensorData("AlectoV1", 0, channel, rc, "R", int2str(rain));
      UserVar[ALECTO_V1_RAIN_VAR-1] = (float)rain/4;
      if (userPluginSendMode == 1)
      {
        customEvent=float2event(UserVar[ALECTO_V1_RAIN_VAR-1], ALECTO_V1_RAIN_VAR -1, CMD_VARIABLE_EVENT);
        userPluginSendEvent(customEvent);
      }
      RawSignal.Number=0;
      return 1L;
    }
    if (nibble3 == 1)
    {
      windspeed = ((bitstream >> 24) & 0xff);
      C1 = windspeed / 10;
      C2 = windspeed - (C1 * 10);
      TempString[0]=0;
      strcat(TempString,int2str(C1));
      strcat(TempString,".");
      strcat(TempString,int2str(C2));
      userPluginPrintSensorData("AlectoV1", 0, channel, rc, "WS", TempString);
      UserVar[ALECTO_V1_WIND_VAR-1] = (float)windspeed * 0.72;
      if (userPluginSendMode != 0)
      {
        customEvent=float2event(UserVar[ALECTO_V1_WIND_VAR-1], ALECTO_V1_WIND_VAR -1, CMD_VARIABLE_EVENT);
        userPluginSendEvent(customEvent);
      }
      RawSignal.Number=0;
      return 1L;
    }
    if ((nibble3 & 0x7) == 7)
    {
      winddirection = ((bitstream >> 15) & 0x1ff) / 45;
      userPluginPrintSensorData("AlectoV1", 0, channel, rc, "WD", int2str(winddirection));
      UserVar[ALECTO_V1_WINDDIRECTION_VAR-1] = (float)winddirection;
      if (userPluginSendMode != 0)
      {
        customEvent=float2event(UserVar[ALECTO_V1_WINDDIRECTION_VAR-1], ALECTO_V1_WINDDIRECTION_VAR -1, CMD_VARIABLE_EVENT);
        userPluginSendEvent(customEvent);
      }

      windgust = ((bitstream >> 24) & 0xff);
      C1 = windgust / 10;
      C2 = windgust - (C1 * 10);
      TempString[0]=0;
      strcat(TempString,int2str(C1));
      strcat(TempString,".");
      strcat(TempString,int2str(C2));
      userPluginPrintSensorData("AlectoV1", 0, channel, rc, "WG", TempString);
      UserVar[ALECTO_V1_WINDGUST_VAR-1] = (float)windgust * 0.72;
      if (userPluginSendMode != 0)
      {
        customEvent=float2event(UserVar[ALECTO_V1_WINDGUST_VAR-1], ALECTO_V1_WINDGUST_VAR -1, CMD_VARIABLE_EVENT);
        userPluginSendEvent(customEvent);
      }
      RawSignal.Number=0;
      return 1L;
    }
  }
  return 0L; 
}
#endif

/*********************************************************************************************\
 * Decodes signals from Alecto Weatherstation outdoor unit, type 2 (176/160 pulses, 88/80 bits, 868 MHz).
 * Requires an 868 MHz receiver!! (Can be connected to Nodo IR port with some modifications in the main Nodo code)
 * Code is in 'alpha' stage, use at own risc, side effects to main Nodo code are unknown.
 * Tested using Alecto DKW2012 and an Aurel RX-AM8SF receiver.
 * Other sensor units and RF receivers may work, but no guarantees what-so-ever!
 * Version 0.0.8
 * 08-10-2012
 * Work to be done:
 *     Improve code
 * DKW2012 Message Format: (11 Bytes, 88 bits):
 * AAAAAAAA AAAABBBB BBBB__CC CCCCCCCC DDDDDDDD EEEEEEEE FFFFFFFF GGGGGGGG GGGGGGGG HHHHHHHH IIIIIIII
 *                         Temperature Humidity Windspd_ Windgust Rain____ ________ Winddir  Checksum
 * A = start/unknown, first 8 bits are always 11111111
 * B = Rolling code
 * C = Temperature (10 bit value with -400 base)
 * D = Humidity
 * E = windspeed (* 0.3 m/s, correction for webapp = 3600/1000 * 0.3 * 100 = 108))
 * F = windgust (* 0.3 m/s, correction for webapp = 3600/1000 * 0.3 * 100 = 108))
 * G = Rain ( * 0.3 mm)
 * H = winddirection (0 = north, 4 = east, 8 = south 12 = west)
 * I = Checksum, calculation is still under investigation
 *
 * WS3000 system has no winddirection, message format is 8 bit shorter
 * Message Format: (10 Bytes, 80 bits):
 * AAAAAAAA AAAABBBB BBBB__CC CCCCCCCC DDDDDDDD EEEEEEEE FFFFFFFF GGGGGGGG GGGGGGGG HHHHHHHH
 *                         Temperature Humidity Windspd_ Windgust Rain____ ________ Checksum
 * 
 * DCF Time Message Format:
 * AAAAAAAA BBBBCCCC DDDDDDDD EFFFFFFF GGGGGGGG HHHHHHHH IIIIIIII JJJJJJJJ KKKKKKKK LLLLLLLL MMMMMMMM
 * 	    11                 Hours   Minutes  Seconds  Year     Month    Day      ?        Checksum
 * B = 11 = DCF
 * C = ?
 * D = ?
 * E = ?
 * F = Hours BCD format (7 bits only for this byte, MSB could be '1')
 * G = Minutes BCD format
 * H = Seconds BCD format
 * I = Year BCD format (only two digits!)
 * J = Month BCD format
 * K = Day BCD format
 * L = ?
 * M = Checksum
 \*********************************************************************************************/
#ifdef ALECTO_V2
boolean Alecto_V2_Decode(void)
{
  unsigned long bitstream1=0;
  unsigned long bitstream2=0;
  unsigned long bitstream3=0;
  byte msgtype=0;
  byte rc=0;
  long temperature=0;
  long humidity=0;
  unsigned long rain=0;
  byte windspeed=0;
  byte windgust=0;
  unsigned long winddirection=0;
  unsigned long customEvent=0;
  byte checksum=0;
  byte checksumcalc=0;
  int C1=0;
  int C2=0;

  if ((RawSignal.Number != 160) && (RawSignal.Number != 176)) return 0L;

  for(byte x= 17; x<= 79; x=x+2) if(RawSignal.Pulses[x] < 0x300) bitstream1 = (bitstream1 << 1) | 0x1; 
  else bitstream1 = (bitstream1 << 1);
  for(byte x= 81; x<=143; x=x+2) if(RawSignal.Pulses[x] < 0x300) bitstream2 = (bitstream2 << 1) | 0x1; 
  else bitstream2 = (bitstream2 << 1);
  for(byte x=145; x<=175; x=x+2) if(RawSignal.Pulses[x] < 0x300) bitstream3 = (bitstream3 << 1) | 0x1; 
  else bitstream3 = (bitstream3 << 1);

  byte byte1 = (bitstream1 >> 24) & 0xff;
  byte byte2 = (bitstream1 >> 16) & 0xff;
  byte byte3 = (bitstream1 >>  8) & 0xff;
  byte byte4 = (bitstream1 >>  0) & 0xff;
  byte byte5 = (bitstream2 >> 24) & 0xff;
  byte byte6 = (bitstream2 >> 16) & 0xff;
  byte byte7 = (bitstream2 >>  8) & 0xff;
  byte byte8 = (bitstream2 >>  0) & 0xff;
  byte byte9 = (bitstream3 >> 8) & 0xff;

  if (RawSignal.Number == 176)
    checksum = (bitstream3 >> 0) & 0xff;
  else
    checksum = (bitstream3 >> 8) & 0xff;

  byte data[9];
  data[0] = byte1;
  data[1] = byte2;
  data[2] = byte3;
  data[3] = byte4;
  data[4] = byte5;
  data[5] = byte6;
  data[6] = byte7;
  data[7] = byte8;
  data[8] = byte9;
  if (RawSignal.Number == 176)
    checksumcalc = userPluginCRC8(data, 9);
  else
    checksumcalc = userPluginCRC8(data, 8);

  msgtype = (bitstream1 >> 28) & 0xf;
  rc = (bitstream1 >> 20) & 0xff;

  if(Settings.Debug==VALUE_ON) userPluginPrintChecksum("AlectoV2", checksum, checksumcalc);

  if (checksum != checksumcalc) return 0L;
  if (!userPluginCheckID("AlectoV2", rc)) return 0L;

  if (msgtype == 11) AlectoDCFDecode(byte6, byte7, byte8, byte3, byte4, byte5);
  if (msgtype != 10) return 0L;

  temperature = ((bitstream1 >> 8) & 0x3ff) - 400;
  UserVar[ALECTO_V2_TEMPERATURE_VAR-1] = (float)temperature / 10;

  TempString[0]=0;
  if (temperature < 0) {
    temperature = -temperature;
    strcat(TempString,"-");
  } 
  C1 = temperature / 10;
  C2 = temperature - (C1 * 10);
  strcat(TempString,int2str(C1));
  strcat(TempString,".");
  strcat(TempString,int2str(C2));
  userPluginPrintSensorData("AlectoV2", 0, 0, rc, "T", TempString);

  humidity = bitstream1 & 0xff;
  userPluginPrintSensorData("AlectoV2", 0, 0, rc, "H", int2str(humidity));
  UserVar[ALECTO_V2_HUMIDITY_VAR-1] = (float)humidity;

  userPluginRain = (bitstream2 & 0xffff);
  rain = userPluginRain - userPluginRainBase;
  userPluginPrintSensorData("AlectoV2", 0, 0, rc, "R", int2str(rain));
  UserVar[ALECTO_V2_RAIN_VAR-1] = (float)rain * 0.30;

  windspeed = ((bitstream2 >> 24) & 0xff);
  userPluginPrintSensorData("AlectoV2", 0, 0, rc, "WS", int2str(windspeed));
  UserVar[ALECTO_V2_WIND_VAR-1] = (float)windspeed * 1.08;

  windgust = ((bitstream2 >> 16) & 0xff);
  userPluginPrintSensorData("AlectoV2", 0, 0, rc, "WG", int2str(windgust));
  UserVar[ALECTO_V2_WINDGUST_VAR-1] = (float)windgust * 1.08;

  if (RawSignal.Number == 176)
  {
    winddirection = ((bitstream3 >> 8) & 0xf);
    userPluginPrintSensorData("AlectoV2", 0, 0, rc, "WD", int2str(winddirection));
    UserVar[ALECTO_V2_WINDDIRECTION_VAR-1] = (float)winddirection;
  }

  if (userPluginSendMode != 0)
  {
    customEvent=float2event(UserVar[ALECTO_V2_TEMPERATURE_VAR-1], ALECTO_V2_TEMPERATURE_VAR -1, CMD_VARIABLE_EVENT);
    userPluginSendEvent(customEvent);

    customEvent=float2event(UserVar[ALECTO_V2_HUMIDITY_VAR-1], ALECTO_V2_HUMIDITY_VAR -1, CMD_VARIABLE_EVENT);
    userPluginSendEvent(customEvent);

    customEvent=float2event(UserVar[ALECTO_V2_RAIN_VAR-1], ALECTO_V2_RAIN_VAR -1, CMD_VARIABLE_EVENT);
    userPluginSendEvent(customEvent);

    customEvent=float2event(UserVar[ALECTO_V2_WIND_VAR-1], ALECTO_V2_WIND_VAR -1, CMD_VARIABLE_EVENT);
    userPluginSendEvent(customEvent);

    customEvent=float2event(UserVar[ALECTO_V2_WINDGUST_VAR-1], ALECTO_V2_WINDGUST_VAR -1, CMD_VARIABLE_EVENT);
    userPluginSendEvent(customEvent);

    if (RawSignal.Number == 176)
    {
      customEvent=float2event(UserVar[ALECTO_V2_WINDDIRECTION_VAR-1], ALECTO_V2_WINDDIRECTION_VAR -1, CMD_VARIABLE_EVENT);
      userPluginSendEvent(customEvent);
    }
  }

  RawSignal.Number=0;
  return 1L; 

}

/*********************************************************************************************\ 
 * Decodes Alecto DCF time data
 \*********************************************************************************************/
void AlectoDCFDecode(byte year, byte month, byte day, byte hours, byte minutes, byte seconds)
{
  hours = hours & 0x7f;
  TempString[0]=0;
  strcat(TempString,"AlectoV2 DCF Time: ");
  strcat(TempString,int2str((day >> 4) & 0xf));
  strcat(TempString,int2str(day & 0xf));
  strcat(TempString,"-");
  strcat(TempString,int2str((month >> 4) & 0xf));
  strcat(TempString,int2str(month & 0xf));
  strcat(TempString,"-");
  strcat(TempString,int2str((year >> 4) & 0xf));
  strcat(TempString,int2str(year & 0xf));
  strcat(TempString," ");
  strcat(TempString,int2str((hours >> 4) & 0xf));
  strcat(TempString,int2str(hours & 0xf));
  strcat(TempString,":");
  strcat(TempString,int2str((minutes >> 4) & 0xf));
  strcat(TempString,int2str(minutes & 0xf));
  strcat(TempString,":");
  strcat(TempString,int2str((seconds >> 4) & 0xf));
  strcat(TempString,int2str(seconds & 0xf));
  userPluginPrintTerminal(TempString);
}
#endif

/*********************************************************************************************\
 * Decodes signals from Alecto Weatherstation outdoor unit, type 3 (94/126 pulses, 47/63 bits, 433 MHz).
 * Code is in beta stage, use at own risc, side effects to main Nodo code are unknown.
 * Other sensor units and RF receivers may work, but no guarantees what-so-ever!
 * Version 0.0.3
 * 08-10-2012
 * Work to be done:
 *     Improve code
 * WS1100 Message Format: (7 bits preamble, 5 Bytes, 40 bits):
 * AAAAAAA AAAABBBB BBBB__CC CCCCCCCC DDDDDDDD EEEEEEEE
 *                        Temperature Humidity Checksum
 * A = start/unknown, first 8 bits are always 11111111
 * B = Rolling code
 * C = Temperature (10 bit value with -400 base)
 * D = Checksum
 * E = Humidity
 *
 * WS1200 Message Format: (7 bits preamble, 7 Bytes, 56 bits):
 * AAAAAAA AAAABBBB BBBB__CC CCCCCCCC DDDDDDDD DDDDDDDD EEEEEEEE FFFFFFFF 
 *                        Temperature Rain LSB Rain MSB ???????? Checksum
 * A = start/unknown, first 8 bits are always 11111111
 * B = Rolling code
 * C = Temperature (10 bit value with -400 base)
 * D = Rain ( * 0.3 mm)
 * E = ?
 * F = Checksum
 \*********************************************************************************************/
#ifdef ALECTO_V3
boolean Alecto_V3_Decode(void)
{
  unsigned long bitstream1=0;
  unsigned long bitstream2=0;
  byte rc=0;
  long temperature=0;
  long humidity=0;
  unsigned long rain=0;
  byte rain1 =0;
  byte rain2 =0;
  unsigned long customEvent=0;
  byte checksum=0;
  byte checksumcalc=0;
  int C1=0;
  int C2=0;

  if ((RawSignal.Number != 94) && (RawSignal.Number != 126)) return 0L;

  // get first 32 relevant bits, skip first 7 bits (temp solution)
  for(byte x=15; x<=77; x=x+2)
  {
    if(RawSignal.Pulses[x] < 0x300)
    {
      bitstream1 = (bitstream1 << 1) | 0x1;
    } 
    else
    {
      bitstream1 = (bitstream1 << 1);
    }
  }

  // get second 32 relevant bits (temp solution)
  for(byte x=79; x<=141; x=x+2)
  {
    if(RawSignal.Pulses[x] < 0x300)
    {
      bitstream2 = (bitstream2 << 1) | 0x1;
    } 
    else
    {
      bitstream2 = (bitstream2 << 1);
    }
  }

  byte byte1 = (bitstream1 >> 24) & 0xff;
  byte byte2 = (bitstream1 >> 16) & 0xff;
  byte byte3 = (bitstream1 >>  8) & 0xff;
  byte byte4 = (bitstream1 >>  0) & 0xff;
  byte byte5 = (bitstream2 >> 24) & 0xff;
  byte byte6 = (bitstream2 >> 16) & 0xff;

  byte data[6];
  data[0] = byte1;
  data[1] = byte2;
  data[2] = byte3;
  data[3] = byte4;
  data[4] = byte5;
  data[5] = byte6;

  if (RawSignal.Number == 126)
  {
    checksum = (bitstream2 >> 8) & 0xff;
    checksumcalc = userPluginCRC8(data, 6);
  }
  else
  {
    checksum = (bitstream2 >> 24) & 0xff;
    checksumcalc = userPluginCRC8(data, 4);
  }

  rc = (bitstream1 >> 20) & 0xff;

  if(Settings.Debug==VALUE_ON) userPluginPrintChecksum("AlectoV3", checksum, checksumcalc);

  if (checksum != checksumcalc) return 0L;
  if (!userPluginCheckID("AlectoV3", rc)) return 0L;

  temperature = ((bitstream1 >> 8) & 0x3ff) - 400;
  UserVar[ALECTO_V3_TEMPERATURE_VAR-1] = (float)temperature / 10;

  TempString[0]=0;
  if (temperature < 0) {
    temperature = -temperature;
    strcat(TempString,"-");
  } 
  C1 = temperature / 10;
  C2 = temperature - (C1 * 10);
  strcat(TempString,int2str(C1));
  strcat(TempString,".");
  strcat(TempString,int2str(C2));
  userPluginPrintSensorData("AlectoV3", 0, 0, rc, "T", TempString);

  if (RawSignal.Number == 126)
  {
    rain1 = ((bitstream1 >> 0) & 0xff);
    rain2 = ((bitstream2 >> 24) & 0xff);
    userPluginRain = (rain2 * 256) + rain1;
    rain = userPluginRain - userPluginRainBase;
    userPluginPrintSensorData("AlectoV3", 0, 0, rc, "R", int2str(rain));
    UserVar[ALECTO_V3_RAIN_VAR-1] = (float)rain * 0.30;
  }
  else
  {
    humidity = bitstream1 & 0xff;
    userPluginPrintSensorData("AlectoV3", 0, 0, rc, "H", int2str(humidity));
    UserVar[ALECTO_V3_HUMIDITY_VAR-1] = (float)humidity;
  }

  if (userPluginSendMode != 0)
  {
    customEvent=float2event(UserVar[ALECTO_V3_TEMPERATURE_VAR-1], ALECTO_V3_TEMPERATURE_VAR -1, CMD_VARIABLE_EVENT);
    userPluginSendEvent(customEvent);

    if (RawSignal.Number == 126)
      customEvent=float2event(UserVar[ALECTO_V3_RAIN_VAR-1], ALECTO_V3_RAIN_VAR -1, CMD_VARIABLE_EVENT);
    else
      customEvent=float2event(UserVar[ALECTO_V3_HUMIDITY_VAR-1], ALECTO_V3_HUMIDITY_VAR -1, CMD_VARIABLE_EVENT);
    userPluginSendEvent(customEvent);
  }

  RawSignal.Number=0;
  return 1L; 

}
#endif

/*********************************************************************************************\
 * Custom printroutine for sensor data
 \*********************************************************************************************/
void userPluginPrintSensorData(char* protocol, int id, int channel, int rc, char* type, char* data)
{
  char datatemp[10];
  strcpy(datatemp,data);

  // Send result to terminal as a string '<protocol>,<sensortype>,<channel>,<rolling ID code><datatype>,<value>
  sprintf(TempString,"%s", protocol);
  strcat(TempString,",");
  strcat(TempString,int2str(id));
  strcat(TempString,",");
  strcat(TempString,int2str(channel));
  strcat(TempString,",");
  strcat(TempString,int2str(rc));
  strcat(TempString,",");
  strcat(TempString,type);
  strcat(TempString,",");
  strcat(TempString, datatemp);

  userPluginPrintTerminal(TempString);

}

/*********************************************************************************************\
 * print limited rawsignal
 \*********************************************************************************************/
void UserPluginPrintRawSignal(byte limit)
{
  TempString[0]=0;
  strcat(TempString,"RF Rawsignal:");
  for(byte x=1;x<=limit;x++)
  {
    if (x>1) strcat(TempString,",");
    strcat(TempString,int2str(RawSignal.Pulses[x]));
  }
  userPluginPrintTerminal(TempString);
}

/*********************************************************************************************\
 * Calculates CRC-8 checksum
 * reference http://lucsmall.com/2012/04/29/weather-station-hacking-part-2/
 *           http://lucsmall.com/2012/04/30/weather-station-hacking-part-3/
 *           https://github.com/lucsmall/WH2-Weather-Sensor-Library-for-Arduino/blob/master/WeatherSensorWH2.cpp
 \*********************************************************************************************/
uint8_t userPluginCRC8( uint8_t *addr, uint8_t len)
{
  uint8_t crc = 0;
  // Indicated changes are from reference CRC-8 function in OneWire library
  while (len--) {
    uint8_t inbyte = *addr++;
    for (uint8_t i = 8; i; i--) {
      uint8_t mix = (crc ^ inbyte) & 0x80; // changed from & 0x01
      crc <<= 1; // changed from right shift
      if (mix) crc ^= 0x31;// changed from 0x8C;
      inbyte <<= 1; // changed from right shift
    }
  }
  return crc;
}

/*********************************************************************************************\
 * Check for valid sensor ID
 \*********************************************************************************************/
boolean userPluginCheckID(char* protocol, byte checkID)
{
  for (byte x=0; x<5; x++) if (userPluginValidID[x] == checkID) return true;

  TempString[0]=0;
  strcat(TempString, "RF ");
  strcat(TempString, protocol);
  strcat(TempString, " Unknown sensor ID:");
  strcat(TempString,int2str(checkID));
  userPluginPrintTerminal(TempString);

  return false;
}

/*********************************************************************************************\
 * print checksum
 \*********************************************************************************************/
void userPluginPrintChecksum(char* protocol, byte checksum, byte checksumcalc)
{
  TempString[0]=0;
  strcat(TempString, "RF ");
  strcat(TempString, protocol);
  strcat(TempString, " Checksum:");
  strcat(TempString,int2str(checksum));
  strcat(TempString, "-");
  strcat(TempString,int2str(checksumcalc));
  userPluginPrintTerminal(TempString);
}

#endif
