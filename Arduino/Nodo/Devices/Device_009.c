
//#######################################################################################################
//##################################### Device-09 AlectoV2  #############################################
//#######################################################################################################

/*********************************************************************************************\
 * Dit protocol zorgt voor ontvangst van Alecto weerstation buitensensoren
 * 
 * Auteur             : Nodo-team (Martinus van den Broek) www.nodo-domotica.nl
 *                      Support ACH2010 en code optimalisatie door forumlid: Arendst
 * Support            : www.nodo-domotica.nl
 * Datum              : Mrt.2013
 * Versie             : 1.0
 * Nodo productnummer : n.v.t. meegeleverd met Nodo code.
 * Compatibiliteit    : Vanaf Nodo build nummer 508
 * Syntax             : "AlectoV2 <Par1:Sensor ID>, <Par2:Basis Variabele>"
 *********************************************************************************************
 * Technische informatie:
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
 * WS3000 and ACH2010 systems have no winddirection, message format is 8 bit shorter
 * Message Format: (10 Bytes, 80 bits):
 * AAAAAAAA AAAABBBB BBBB__CC CCCCCCCC DDDDDDDD EEEEEEEE FFFFFFFF GGGGGGGG GGGGGGGG HHHHHHHH
 *                         Temperature Humidity Windspd_ Windgust Rain____ ________ Checksum
 * 
 * DCF Time Message Format: (NOT DECODED!, we already have time sync through webapp)
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

#ifdef DEVICE_009
#define DEVICE_ID 9
#define DEVICE_NAME "AlectoV2"

#define DKW2012_PULSECOUNT 176
#define ACH2010_MIN_PULSECOUNT 160 // reduce this value (144?) in case of bad reception
#define ACH2010_MAX_PULSECOUNT 160

boolean Device_009(byte function, struct NodoEventStruct *event, char *string)
{
  boolean success=false;

  switch(function)
  {
#ifdef DEVICE_CORE_009
  case DEVICE_RAWSIGNAL_IN:
    {
      if (!(((RawSignal.Number >= ACH2010_MIN_PULSECOUNT) && (RawSignal.Number <= ACH2010_MAX_PULSECOUNT)) || (RawSignal.Number == DKW2012_PULSECOUNT))) return false;

      const byte PTMF=50;
      byte c=0;
      byte rfbit;
      byte data[9]; 
      byte msgtype=0;
      byte rc=0;
      unsigned int rain=0;
      byte checksum=0;
      byte checksumcalc=0;
      byte basevar;
      byte maxidx = 8;

      if(RawSignal.Number > ACH2010_MAX_PULSECOUNT) maxidx = 9;  
      // Get message back to front as the header is almost never received complete for ACH2010
      byte idx = maxidx;
      for(byte x=RawSignal.Number; x>0; x=x-2)
        {
          if(RawSignal.Pulses[x-1]*PTMF < 0x300) rfbit = 0x80; else rfbit = 0;  
          data[idx] = (data[idx] >> 1) | rfbit;
          c++;
          if (c == 8) 
          {
            if (idx == 0) break;
            c = 0;
            idx--;
          }   
        }

      checksum = data[maxidx];
      checksumcalc = ProtocolAlectoCRC8(data, maxidx);
  
      msgtype = (data[0] >> 4) & 0xf;
      rc = (data[0] << 4) | (data[1] >> 4);

      if (checksum != checksumcalc) return false;
  
      basevar = ProtocolAlectoCheckID(rc);

      event->Par1=rc;
      event->Par2=basevar;
      event->SourceUnit    = 0;                     // Komt niet van een Nodo unit af, dus unit op nul zetten
      event->Port          = VALUE_SOURCE_RF;

      if (basevar == 0) return true;
      if ((msgtype != 10) && (msgtype != 5)) return true;
  
      UserVar[basevar -1] = (float)(((data[1] & 0x3) * 256 + data[2]) - 400) / 10;
      UserVar[basevar +1 -1] = (float)data[3];

      rain = (data[6] * 256) + data[7];
      // check if rain unit has been reset!
      if (rain < ProtocolAlectoRainBase) ProtocolAlectoRainBase=rain;
      if (ProtocolAlectoRainBase > 0)
      {
        UserVar[basevar +2 -1] += ((float)rain - ProtocolAlectoRainBase) * 0.30;
      }
      ProtocolAlectoRainBase = rain;

      UserVar[basevar +3 -1] = (float)data[4] * 1.08;
      UserVar[basevar +4 -1] = (float)data[5] * 1.08;
      if (RawSignal.Number == DKW2012_PULSECOUNT) UserVar[basevar +5 -1] = (float)(data[8] & 0xf);

      success = true;
      break;
    }
  case DEVICE_COMMAND:
    {
    if ((event->Par2 > 0) && (ProtocolAlectoCheckID(event->Par1) == 0))
      {
      for (byte x=0; x<5; x++)
        {
        if (ProtocolAlectoValidID[x] == 0)
          {
            ProtocolAlectoValidID[x] = event->Par1;
            ProtocolAlectoVar[x] = event->Par2;
            break;
          }
        }
      }
    break;
    }
#endif // DEVICE_CORE_009

#if NODO_MEGA
  case DEVICE_MMI_IN:
    {
    char *TempStr=(char*)malloc(26);
    string[25]=0;

    if(GetArgv(string,TempStr,1))
      {
      if(strcasecmp(TempStr,DEVICE_NAME)==0)
        {
        if(event->Par1>0 && event->Par1<255 && event->Par2>0 && event->Par2<=USER_VARIABLES_MAX)
          success=true;
        }
      }
      free(TempStr);
      break;
    }

  case DEVICE_MMI_OUT:
    {
    strcpy(string,DEVICE_NAME);            // Eerste argument=het commando deel
    strcat(string," ");
    strcat(string,int2str(event->Par1));
    strcat(string,",");
    strcat(string,int2str(event->Par2));
    break;
    }
#endif //NODO_MEGA
  }      
  return success;
}

byte ProtocolAlectoValidID[5];
byte ProtocolAlectoVar[5];
unsigned int ProtocolAlectoRainBase=0;

/*********************************************************************************************\
 * Calculates CRC-8 checksum
 * reference http://lucsmall.com/2012/04/29/weather-station-hacking-part-2/
 *           http://lucsmall.com/2012/04/30/weather-station-hacking-part-3/
 *           https://github.com/lucsmall/WH2-Weather-Sensor-Library-for-Arduino/blob/master/WeatherSensorWH2.cpp
 \*********************************************************************************************/
uint8_t ProtocolAlectoCRC8( uint8_t *addr, uint8_t len)
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
byte ProtocolAlectoCheckID(byte checkID)
{
  for (byte x=0; x<5; x++) if (ProtocolAlectoValidID[x] == checkID) return ProtocolAlectoVar[x];
  return 0;
}

#endif //DEVICE_09


