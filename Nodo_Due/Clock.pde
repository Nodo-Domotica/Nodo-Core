 
/**********************************************************************************************\
 * 
 * Deze functie vult de globale variabele Time.DayLight met de status van zonsopkomst & -opgang
 \*********************************************************************************************/
#define offset 60
void SetDaylight()
  {
  // Tabel Sunset & Sunrise: om de 10 dagen de tijden van zonsopkomst en zonsondergang in minuten na middernacht. 
  // Geldig voor in Nederland (gemiddelde voor midden Nederland op 52.00 graden NB en 5.00 graden OL) 
  // Eerste dag is 01 januari, tweede is 10, januari, derde is 20 januari, etc.
  // tussenliggende dagen worden berekend aan de hand van lineaire interpolatie tussen de tabelwaarden. 
  // Afwijking t.o.v. KNMI-tabel is hiermee beperkt tot +/-1 min.
  
  int DOY,index,now,up,down;
  int u0,u1,d0,d1;
  
  DOY=((Time.Month-1)*304)/10+Time.Date;// schrikkeljaar berekening niet nodig, levert slechts naukeurigheidsafwijking van één minuut.
  index=(DOY/10);
  now=Time.Hour*60+Time.Minutes;

  //zomertijd correctie 
  if(Time.DaylightSaving)
    {
    if(now>=60)now-=60;
    else now=now+1440-60;
    }
    
  u0=pgm_read_word_near(Sunrise+index);
  u1=pgm_read_word_near(Sunrise+index+1);
  d0=pgm_read_word_near(Sunset+index);
  d1=pgm_read_word_near(Sunset+index+1);

  up  =u0+((u1-u0)*(DOY%10))/10;// Zon op in minuten na middernacht
  down=d0+((d1-d0)*(DOY%10))/10;// Zon onder in minuten na middernacht

  Time.Daylight=0;                        // astronomische start van de dag (in de wintertijd om 0:00 uur)
  if(now>=(up-offset))   Time.Daylight=1; // <offset> minuten voor zonsopkomst 
  if(now>=up)            Time.Daylight=2; // zonsopkomst
  if(now>=(down-offset)) Time.Daylight=3; // <offset> minuten voor zonsondergang
  if(now>=down)          Time.Daylight=4; // zonsondergang
  }

#define DS1307_SEC       0
#define DS1307_MIN       1
#define DS1307_HR        2
#define DS1307_DOW       3
#define DS1307_DATE      4
#define DS1307_MTH       5
#define DS1307_YR        6
#define DS1307_BASE_YR   2000

#define DS1307_CTRL_ID   B1101000  //DS1307
#define DS1307_CLOCKHALT B10000000
#define DS1307_LO_BCD    B00001111
#define DS1307_HI_BCD    B11110000
#define DS1307_HI_SEC    B01110000
#define DS1307_HI_MIN    B01110000
#define DS1307_HI_HR     B00110000
#define DS1307_LO_DOW    B00000111
#define DS1307_HI_DATE   B00110000
#define DS1307_HI_MTH    B00110000
#define DS1307_HI_YR     B11110000

int rtc[7];

/**********************************************************************************************\
 * Leest de realtime clock en plaatst actuele waarden in de struct Time. 
 * Revision 01, 09-01-2010, P.K.Tonkes@gmail.com
 \*********************************************************************************************/

void SimulateDay(byte days) 
  {
  unsigned long SimulatedClockEvent, Event, Action;
  
  Time.Seconds=0;
  Time.Minutes=0;
  Time.Hour=0;
  DaylightPrevious=4;// vullen met 4, dan wordt in de zomertijd 4 niet tweemaal per etmaal weergegeven
  
  PrintLine();
  for(int d=1;d<=days;d++)
    {
    for(int m=0;m<=1439;m++)  // loop alle minuten van één etmaal door
      {
      // Simuleer alle menuten van een etmaal
      if(Time.Minutes==60){Time.Minutes=0;Time.Hour++;}  // roll-over naar volgende uur
      if(Time.Hour==24)   {Time.Hour=0;Time.Day++;}      // roll-over naar volgende etmaal
      if(Time.Day==8)     {Time.Day=1;}                  // roll-over naar volgende week

      // Kijk of er op het gesimuleerde tijdstip een hit is in de EventList
      SimulatedClockEvent=command2event(CMD_CLOCK_EVENT_ALL+Time.Day,Time.Hour,Time.Minutes);
      if(CheckEventlist(SimulatedClockEvent)) // kijk of er een hit is in de EventList
        ProcessEvent(SimulatedClockEvent,VALUE_DIRECTION_INTERNAL,VALUE_SOURCE_CLOCK,0,0);
        
      // Kijk of er op het gesimuleerde tijdstip een zonsondergang of zonsopkomst wisseling heeft voorgedaan
      SetDaylight(); // Zet in de struct ook de Time.DayLight status behorend bij de tijd
      if(Time.Daylight!=DaylightPrevious)// er heeft een zonsondergang of zonsopkomst wisseling voorgedaan
        {
        SimulatedClockEvent=command2event(CMD_CLOCK_EVENT_DAYLIGHT,Time.Daylight,0L);
        DaylightPrevious=Time.Daylight;
        ProcessEvent(SimulatedClockEvent,VALUE_DIRECTION_INTERNAL,VALUE_SOURCE_CLOCK,0,0);
        }
      Time.Minutes++;
      }
    }
  PrintLine();
  ClockRead();// klok weer op de juiste tijd zetten.
  SetDaylight();// daglicht status weer terug op de juiste stand zetten
  DaylightPrevious=Time.Daylight;
  }

// update the data on the RTC from the bcd formatted data in the buffer
void DS1307_save(void)
  {
  Wire.beginTransmission(DS1307_CTRL_ID);
  Wire.send(0x00); // reset register pointer
  for(byte i=0; i<7; i++)Wire.send(rtc[i]);
  Wire.endTransmission();
  }

// Aquire data from the RTC chip in BCD format, refresh the buffer
void DS1307_read(void)
  {
  Wire.beginTransmission(DS1307_CTRL_ID);  // reset the register pointer to zero
  Wire.send(0x00);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_CTRL_ID, 7);  // request the 7 bytes of data    (secs, min, hr, dow, date. mth, yr)
  for(byte i=0; i<7; i++)rtc[i]=Wire.receive();// store data in raw bcd format
  }

    
/**********************************************************************************************\
 * Zet de RTC op tijd.
 \*********************************************************************************************/
void ClockSet(void) 
  {
  rtc[DS1307_SEC]  =DS1307_CLOCKHALT;  // Stop the clock. Set the ClockHalt bit high to stop the rtc. This bit is part of the seconds byte
  DS1307_save();
  rtc[DS1307_MIN]  =((Time.Minutes/10)<<4)+(Time.Minutes%10);
  rtc[DS1307_HR]   =((Time.Hour/10)<<4)+(Time.Hour%10); // schrijf de wintertijd weg.
  rtc[DS1307_DOW]  =Time.Day;
  rtc[DS1307_DATE] =((Time.Date/10)<<4)+(Time.Date%10);
  rtc[DS1307_MTH]  =((Time.Month/10)<<4)+(Time.Month%10);
  rtc[DS1307_YR]   =(((Time.Year-DS1307_BASE_YR)/10)<<4)+(Time.Year%10); 
  rtc[DS1307_SEC]  =((Time.Seconds/10)<<4)+(Time.Seconds%10); // and start the clock again...
  DS1307_save();
  }

/**********************************************************************************************\
 * Leest de realtime clock en plaatst actuele waarden in de struct Time. 
 * Eveneens wordt de Event code terug gegeven
 \*********************************************************************************************/
unsigned long ClockRead(void)
  {
  DS1307_read();// lees de RTC chip uit
  Time.Seconds =(10*((rtc[DS1307_SEC] & DS1307_HI_SEC)>>4))+(rtc[DS1307_SEC] & DS1307_LO_BCD);
  Time.Minutes =(10*((rtc[DS1307_MIN] & DS1307_HI_MIN)>>4))+(rtc[DS1307_MIN] & DS1307_LO_BCD);
  Time.Date    =(10*((rtc[DS1307_DATE] & DS1307_HI_DATE)>>4))+(rtc[DS1307_DATE] & DS1307_LO_BCD);
  Time.Month   =(10*((rtc[DS1307_MTH] & DS1307_HI_MTH)>>4))+(rtc[DS1307_MTH] & DS1307_LO_BCD);
  Time.Year    =(10*((rtc[DS1307_YR] & DS1307_HI_YR)>>4))+(rtc[DS1307_YR] & DS1307_LO_BCD)+DS1307_BASE_YR;
  Time.Hour    =(10*((rtc[DS1307_HR] & DS1307_HI_HR)>>4))+(rtc[DS1307_HR] & DS1307_LO_BCD);
  Time.Day     =rtc[DS1307_DOW] & DS1307_LO_DOW;

  long x=(long)pgm_read_word_near(DLSDate+Time.Year-DLSBase);
  long y=(long)((long)(Time.Date*100L)+(long)(Time.Month*10000L)+(long)Time.Hour);
  Time.DaylightSaving=(y>=((x/100L)*100L+30002L) && y<((x%100L)*100L+100003L));  
  
  
  // Automatische zomer-/wintertijd schakeling
  x=Time.Month*100+Time.Date;  
  if(Time.DaylightSaving!=S.DaylightSaving  && x!=S.DaylightSavingSet)
    {  
    if(S.DaylightSaving)// als het zomertijd is en wintertijd wordt EN verzetdatum ongelijk aan nu
      Time.Hour=Time.Hour==0?23:Time.Hour-1;// ...dan de klok een uur terug.
    else // als het wintertijd is en zomertijd wordt
      Time.Hour=Time.Hour<23?Time.Hour+1:0; //... dan klok uur vooruit.
    S.DaylightSavingSet=x;
    S.DaylightSaving=Time.DaylightSaving;
    SaveSettings();
    ClockSet();// verzet de RTC klok
    TransmitCode(command2event(CMD_DLS_EVENT,S.DaylightSaving,0),SIGNAL_TYPE_NODO);
    }
      
  return ((unsigned long)(SIGNAL_TYPE_NODO))<<28 |
         ((unsigned long)(S.Unit))<<24 | 
         ((unsigned long)(CMD_CLOCK_EVENT_ALL+Time.Day))<<16 | 
         ((unsigned long)(Time.Hour))<<8 | 
         ((unsigned long)(Time.Minutes));
  }


