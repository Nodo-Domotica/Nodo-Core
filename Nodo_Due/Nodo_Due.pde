

 /****************************************************************************************************************************\
 * Arduino project "Nodo Due" © Copyright 2010 Paul Tonkes
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You received a copy of the GNU General Public License
 * along with this program in tab '_COPYING'.
 *
 * voor toelichting op de licentievoorwaarden zie    : http://www.gnu.org/licenses
 * Voor discussie: Zie Logitech Harmony forum        : http://groups.google.com/group/arduinodo/topics
 * Uitgebreide documentatie is te vinden op          : http://www.nodo-domotica.nl
 * bugs kunnen worden gelogd op                      : https://code.google.com/p/arduino-nodo/
 * Compiler voor deze programmacode te downloaden op : http://arduino.cc
 * Voor vragen of suggesties, mail naar              : p.k.tonkes@gmail.com
 * Compiler                                          : - Arduino Compiler 0022
 * Hardware                                          : - Arduino UNO, Duemilanove of Nano met een ATMeg328 processor @16Mhz.
 *                                                     - Hardware en Arduino penbezetting volgens schema Nodo Due Rev.003
 \****************************************************************************************************************************/

#define VERSION        120        // Nodo Version nummer:
                                  // Major.Minor.Patch
                                  // Major: Grote veranderingen aan concept, besturing, werking.
                                  // Minor: Uitbreiding/aanpassing van commando's, functionaliteit en MMI aanpassingen
                                  // Patch: Herstel van bugs zonder (ingrijpende) functionele veranderingen.


#include "pins_arduino.h"
#include <EEPROM.h>
#include <Wire.h>
#include <avr/pgmspace.h>

/**************************************************************************************************************************\
*  Nodo Event            = TTTTUUUUCCCCCCCC1111111122222222       -> T=Type, U=Unit, 1=Par-1, 2=Par-2
\**************************************************************************************************************************/

// strings met vaste tekst naar PROGMEM om hiermee RAM-geheugen te sparen.
prog_char PROGMEM Text_01[] = "Nodo-Due Domotica controller (c) Copyright 2011 P.K.Tonkes.";
prog_char PROGMEM Text_02[] = "Licensed under GNU General Public License.";
prog_char PROGMEM Text_03[] = "Line=";
prog_char PROGMEM Text_04[] = "SunMonThuWedThuFriSat";
prog_char PROGMEM Text_06[] = "Unknown command: ";
prog_char PROGMEM Text_07[] = "RawSignal=";
prog_char PROGMEM Text_08[] = "Queue=Out, ";
prog_char PROGMEM Text_09[] = "Queue=In, ";
prog_char PROGMEM Text_10[] = "TimeStamp=";
prog_char PROGMEM Text_11[] = "Direction=";
prog_char PROGMEM Text_12[] = "Source=";
prog_char PROGMEM Text_13[] = "ThisUnit=";
prog_char PROGMEM Text_14[] = "Event=";
prog_char PROGMEM Text_15[] = "Version=";
prog_char PROGMEM Text_16[] = "Action=";

#define RANGE_VALUE 30 // alle codes kleiner of gelijk aan deze waarde zijn vaste Nodo waarden.
#define RANGE_EVENT 81 // alle codes groter of gelijk aan deze waarde zijn een event.
#define COMMAND_MAX 102 // aantal commando's (dus geteld vanaf 0)

#define VALUE_OFF 0
#define VALUE_COMMAND 1
#define VALUE_PARAMETER 2
#define VALUE_SOURCE_IR 3
#define VALUE_SOURCE_IR_RF 4
#define VALUE_SOURCE_RF 5
#define VALUE_SOURCE_SERIAL 6
#define VALUE_SOURCE_WIRED 7
#define VALUE_SOURCE_EVENTLIST 8
#define VALUE_SOURCE_SYSTEM 9
#define VALUE_SOURCE_TIMER 10
#define VALUE_SOURCE_VARIABLE 11
#define VALUE_SOURCE_CLOCK 12
#define VALUE_TRACE 13
#define VALUE_TAG 14
#define VALUE_TIMESTAMP 15
#define VALUE_DIRECTION 16
#define VALUE_DIRECTION_INPUT 17
#define VALUE_DIRECTION_OUTPUT 18
#define VALUE_DIRECTION_INTERNAL 19
#define VALUE_BUSY 20
#define VALUE_SOURCE 21
#define VALUE_RF_2_IR 22
#define VALUE_IR_2_RF 23
#define VALUE_ALL 24 // Deze waarde MOET groter dan 16 zijn.
#define VALUE_DIRECTION_OUTPUT_RAW 25
#define VALUE_NESTING 26
#define VALUE_SOURCE_QUEUE 27
#define VALUE_ON 28 // Deze waarde MOET groter dan 16 zijn.
#define VALUE_RES6 29
#define VALUE_RES3 30
#define CMD_ANALYSE_SETTINGS 31
#define CMD_BREAK_ON_VAR_EQU 32
#define CMD_BREAK_ON_VAR_LESS 33
#define CMD_BREAK_ON_VAR_MORE 34
#define CMD_BREAK_ON_VAR_NEQU 35
#define CMD_CLOCK_DATE 36
#define CMD_CLOCK_YEAR 37
#define CMD_CLOCK_TIME 38
#define CMD_CLOCK_DOW 39
#define CMD_DELAY 40
#define CMD_DIVERT 41
#define CMD_EVENTLIST_ERASE 42
#define CMD_EVENTLIST_SHOW 43
#define CMD_EVENTLIST_WRITE 44
#define CMD_TRANSMIT_SETTINGS 45
#define CMD_RAWSIGNAL_GET 46
#define CMD_RAWSIGNAL_PUT 47
#define CMD_RESET 48
#define CMD_SEND_KAKU 49
#define CMD_SEND_KAKU_NEW 50
#define CMD_SEND_SIGNAL 51
#define CMD_SIMULATE 52
#define CMD_SIMULATE_DAY 53
#define CMD_SOUND 54
#define CMD_STATUS 55
#define CMD_RES1 56
#define CMD_TIMER_RANDOM 57
#define CMD_TIMER_SET_SEC 58
#define CMD_TIMER_SET_MIN 59
#define CMD_DISPLAY 60
#define CMD_UNIT 61
#define CMD_WAITBUSY 62
#define CMD_VARIABLE_DEC 63
#define CMD_VARIABLE_INC 64
#define CMD_VARIABLE_SET 65
#define CMD_VARIABLE_VARIABLE 66
#define CMD_VARIABLE_WIRED_ANALOG 67
#define CMD_WAITFREERF 68
#define CMD_WIRED_ANALOG 69
#define CMD_WIRED_OUT 70
#define CMD_WIRED_PULLUP 71
#define CMD_WIRED_SMITTTRIGGER 72
#define CMD_WIRED_THRESHOLD 73
#define CMD_SEND_USEREVENT 74
#define CMD_COPYSIGNAL 75
#define CMD_COMMAND_WILDCARD 76
#define CMD_SENDBUSY 77
#define CMD_SEND_VAR_USEREVENT 78
#define CMD_WIRED_RANGE 79
#define CMD_COMMAND_RES4 80
#define CMD_BOOT_EVENT 81
#define CMD_CLOCK_EVENT_DAYLIGHT 82
#define CMD_CLOCK_EVENT_ALL 83
#define CMD_CLOCK_EVENT_SUN 84
#define CMD_CLOCK_EVENT_MON 85
#define CMD_CLOCK_EVENT_TUE 86
#define CMD_CLOCK_EVENT_WED 87
#define CMD_CLOCK_EVENT_THU 88
#define CMD_CLOCK_EVENT_FRI 89
#define CMD_CLOCK_EVENT_SAT 90
#define CMD_RES_91 91
#define CMD_KAKU 92
#define CMD_KAKU_NEW 93
#define CMD_TIMER_EVENT 94
#define CMD_WIRED_IN_EVENT 95
#define CMD_VARIABLE_EVENT 96
#define CMD_BUSY 97
#define CMD_res 98
#define CMD_ERROR 99
#define CMD_USER_EVENT 100// deze moet altijd op 100 blijven anders opnieuw leren aan universele afstandsbediening!
#define CMD_DLS_EVENT 101

prog_char PROGMEM Cmd_0[]="Off";
prog_char PROGMEM Cmd_1[]="Command";
prog_char PROGMEM Cmd_2[]="Parameter";
prog_char PROGMEM Cmd_3[]="IR";
prog_char PROGMEM Cmd_4[]="IR&RF";
prog_char PROGMEM Cmd_5[]="RF";
prog_char PROGMEM Cmd_6[]="Serial";
prog_char PROGMEM Cmd_7[]="Wired";
prog_char PROGMEM Cmd_8[]="EventList";
prog_char PROGMEM Cmd_9[]="System";
prog_char PROGMEM Cmd_10[]="Timers";
prog_char PROGMEM Cmd_11[]="Variables";
prog_char PROGMEM Cmd_12[]="Clock";
prog_char PROGMEM Cmd_13[]="Trace";
prog_char PROGMEM Cmd_14[]="Tag";
prog_char PROGMEM Cmd_15[]="Timestamp";
prog_char PROGMEM Cmd_16[]="Direction";
prog_char PROGMEM Cmd_17[]="Input";
prog_char PROGMEM Cmd_18[]="Output";
prog_char PROGMEM Cmd_19[]="Internal";
prog_char PROGMEM Cmd_20[]="Busy";
prog_char PROGMEM Cmd_21[]="Source";
prog_char PROGMEM Cmd_22[]="RF2IR";
prog_char PROGMEM Cmd_23[]="IR2RF";
prog_char PROGMEM Cmd_24[]="All";
prog_char PROGMEM Cmd_25[]="Output_RAW";
prog_char PROGMEM Cmd_26[]="Nesting";
prog_char PROGMEM Cmd_27[]="Queue";
prog_char PROGMEM Cmd_28[]="On";
prog_char PROGMEM Cmd_29[]=""; // reserve
prog_char PROGMEM Cmd_30[]=""; // reserve
prog_char PROGMEM Cmd_31[]="ReceiveSettings";
prog_char PROGMEM Cmd_32[]="BreakOnVarEqu";
prog_char PROGMEM Cmd_33[]="BreakOnVarLess";
prog_char PROGMEM Cmd_34[]="BreakOnVarMore";
prog_char PROGMEM Cmd_35[]="BreakOnVarNEqu";
prog_char PROGMEM Cmd_36[]="ClockSetDate";
prog_char PROGMEM Cmd_37[]="ClockSetYear";
prog_char PROGMEM Cmd_38[]="ClockSetTime";
prog_char PROGMEM Cmd_39[]="ClockSetDOW";
prog_char PROGMEM Cmd_40[]="Delay";
prog_char PROGMEM Cmd_41[]="Divert";
prog_char PROGMEM Cmd_42[]="EventlistErase";
prog_char PROGMEM Cmd_43[]="EventlistShow";
prog_char PROGMEM Cmd_44[]="EventlistWrite";
prog_char PROGMEM Cmd_45[]="TransmitSettings";
prog_char PROGMEM Cmd_46[]="RawsignalGet";
prog_char PROGMEM Cmd_47[]="RawsignalPut";
prog_char PROGMEM Cmd_48[]="Reset";
prog_char PROGMEM Cmd_49[]="SendKAKU";
prog_char PROGMEM Cmd_50[]="SendNewKAKU";
prog_char PROGMEM Cmd_51[]="SendSignal";
prog_char PROGMEM Cmd_52[]="Simulate";
prog_char PROGMEM Cmd_53[]="SimulateDay";
prog_char PROGMEM Cmd_54[]="Sound";
prog_char PROGMEM Cmd_55[]="Status";
prog_char PROGMEM Cmd_56[]="";
prog_char PROGMEM Cmd_57[]="TimerRandom";
prog_char PROGMEM Cmd_58[]="TimerSetSec";
prog_char PROGMEM Cmd_59[]="TimerSetMin";
prog_char PROGMEM Cmd_60[]="Display";
prog_char PROGMEM Cmd_61[]="Unit";
prog_char PROGMEM Cmd_62[]="WaitBusy";
prog_char PROGMEM Cmd_63[]="VariableDec";
prog_char PROGMEM Cmd_64[]="VariableInc";
prog_char PROGMEM Cmd_65[]="VariableSet";
prog_char PROGMEM Cmd_66[]="VariableVariable";
prog_char PROGMEM Cmd_67[]="VariableWiredAnalog";
prog_char PROGMEM Cmd_68[]="WaitFreeRF";
prog_char PROGMEM Cmd_69[]="WiredAnalog";
prog_char PROGMEM Cmd_70[]="WiredOut";
prog_char PROGMEM Cmd_71[]="WiredPullup";
prog_char PROGMEM Cmd_72[]="WiredSmittTrigger";
prog_char PROGMEM Cmd_73[]="WiredThreshold";
prog_char PROGMEM Cmd_74[]="SendUserEvent";
prog_char PROGMEM Cmd_75[]="RawSignalCopy";
prog_char PROGMEM Cmd_76[]="WildCard";
prog_char PROGMEM Cmd_77[]="SendBusy";
prog_char PROGMEM Cmd_78[]="SendVarUserEvent";
prog_char PROGMEM Cmd_79[]="WiredRange";
prog_char PROGMEM Cmd_80[]=""; // reserve
prog_char PROGMEM Cmd_81[]="Boot";
prog_char PROGMEM Cmd_82[]="ClockDaylight";
prog_char PROGMEM Cmd_83[]="ClockAll";
prog_char PROGMEM Cmd_84[]="ClockSun";
prog_char PROGMEM Cmd_85[]="ClockMon";
prog_char PROGMEM Cmd_86[]="ClockTue";
prog_char PROGMEM Cmd_87[]="ClockWed";
prog_char PROGMEM Cmd_88[]="ClockThu";
prog_char PROGMEM Cmd_89[]="ClockFri";
prog_char PROGMEM Cmd_90[]="ClockSat";
prog_char PROGMEM Cmd_91[]=""; // reserve
prog_char PROGMEM Cmd_92[]="KAKU";
prog_char PROGMEM Cmd_93[]="NewKAKU";
prog_char PROGMEM Cmd_94[]="Timer";
prog_char PROGMEM Cmd_95[]="WiredIn";
prog_char PROGMEM Cmd_96[]="Variable";
prog_char PROGMEM Cmd_97[]="Busy";
prog_char PROGMEM Cmd_98[]=""; // reserve
prog_char PROGMEM Cmd_99[]="Error"; // deze moet altijd op  blijven
prog_char PROGMEM Cmd_100[]="UserEvent"; // deze moet altijd op 100 blijven anders opnieuw leren aan universele afstandsbediening!
prog_char PROGMEM Cmd_101[]="DaylightSaving";

// tabel die refereert aan de commando strings
PROGMEM const char *CommandText_tabel[]={
  Cmd_0 ,Cmd_1 ,Cmd_2 ,Cmd_3 ,Cmd_4 ,Cmd_5 ,Cmd_6 ,Cmd_7 ,Cmd_8 ,Cmd_9 ,
  Cmd_10,Cmd_11,Cmd_12,Cmd_13,Cmd_14,Cmd_15,Cmd_16,Cmd_17,Cmd_18,Cmd_19,
  Cmd_20,Cmd_21,Cmd_22,Cmd_23,Cmd_24,Cmd_25,Cmd_26,Cmd_27,Cmd_28,Cmd_29,
  Cmd_30,Cmd_31,Cmd_32,Cmd_33,Cmd_34,Cmd_35,Cmd_36,Cmd_37,Cmd_38,Cmd_39,
  Cmd_40,Cmd_41,Cmd_42,Cmd_43,Cmd_44,Cmd_45,Cmd_46,Cmd_47,Cmd_48,Cmd_49,
  Cmd_50,Cmd_51,Cmd_52,Cmd_53,Cmd_54,Cmd_55,Cmd_56,Cmd_57,Cmd_58,Cmd_59,
  Cmd_60,Cmd_61,Cmd_62,Cmd_63,Cmd_64,Cmd_65,Cmd_66,Cmd_67,Cmd_68,Cmd_69,
  Cmd_70,Cmd_71,Cmd_72,Cmd_73,Cmd_74,Cmd_75,Cmd_76,Cmd_77,Cmd_78,Cmd_79,          
  Cmd_80,Cmd_81,Cmd_82,Cmd_83,Cmd_84,Cmd_85,Cmd_86,Cmd_87,Cmd_88,Cmd_89,          
  Cmd_90,Cmd_91,Cmd_92,Cmd_93,Cmd_94,Cmd_95,Cmd_96,Cmd_97,Cmd_98,Cmd_99,          
  Cmd_100,Cmd_101};          

PROGMEM prog_uint16_t Sunrise[]={         
  528,525,516,503,487,467,446,424,401,378,355,333,313,295,279,268,261,259,263,271,283,297,312,329,
  345,367,377,394,411,428,446,464,481,498,512,522,528,527};
      
PROGMEM prog_uint16_t Sunset[]={          
  999,1010,1026,1044,1062,1081,1099,1117,1135,1152,1169,1186,1203,1219,1235,1248,1258,1263,1264,1259,
  1249,1235,1218,1198,1177,1154,1131,1107,1084,1062,1041,1023,1008,996,990,989,993,1004};

// omschakeling zomertijd / wintertijd voor komende 10 jaar. één int bevat de omschakeldata in maart en oktober.
PROGMEM prog_uint16_t DLSDate[]={2831,2730,2528,3127,3026,2925,2730,2629,2528,3127};
#define DLSBase 2010 // jaar van eerste element uit de array


// Declaratie aansluitingen I/O-pennen op de Arduino
// D0 en D1 kunnen niet worden gebruikt. In gebruik door de FTDI-chip voor seriele USB-communiatie (TX/RX).
// A4 en A5 worden gebruikt voor I2C communicatie voor o.a. de real-time clock
#define IR_ReceiveDataPin           3  // Op deze input komt het IR signaal binnen van de TSOP. Bij HIGH bij geen signaal.
#define IR_TransmitDataPin         11  // Aan deze pin zit een zender IR-Led. (gebufferd via transistor i.v.m. hogere stroom die nodig is voor IR-led)
#define RF_TransmitPowerPin         4  // +5 volt / Vcc spanning naar de zender.
#define RF_TransmitDataPin          5  // data naar de zender
#define RF_ReceiveDataPin           2  // Op deze input komt het 433Mhz-RF signaal binnen. LOW bij geen signaal.
#define RF_ReceivePowerPin         12  // Spanning naar de ontvanger via deze pin.
#define MonitorLedPin              13  // bij iedere ontvangst of verzending licht deze led kort op.
#define BuzzerPin                   6  // luidspreker aansluiting
#define WiredAnalogInputPin_1       0  // vier analoge inputs van 0 tot en met 3
#define WiredDigitalOutputPin_1     7  // vier digitale outputs van 7 tot en met 10

#define UNIT                       0x1 // Unit nummer van de Nodo. Bij gebruik van meerdere nodo's deze uniek toewijzen [1..F]
#define Eventlist_OFFSET            64 // Eerste deel van het EEPROM geheugen is voor de settings. Reserveer __ bytes. Deze niet te gebruiken voor de Eventlist.
#define Eventlist_MAX              120 // aantal events dat de lijst bevat in het EEPROM geheugen van de ATMega328. Iedere event heeft 8 bytes nodig. eerste adres is 0
#define USER_VARIABLES_MAX          15 // aantal beschikbare gebruikersvariabelen voor de user.
#define RAW_BUFFER_SIZE            200 // Maximaal aantal te ontvangen bits*2
#define UNIT_MAX                    15 
#define MACRO_EXECUTION_DEPTH       10 // maximale nesting van macro's.

#define SIGNAL_TYPE_UNKNOWN          0
#define SIGNAL_TYPE_NODO             1
#define SIGNAL_TYPE_KAKU             2
#define SIGNAL_TYPE_NEWKAKU          3

#define NODO_TYPE_EVENT              1
#define NODO_TYPE_COMMAND            2

#define BAUD                     19200 // Baudrate voor seriële communicatie.
#define SERIAL_TERMINATOR_1       0x0A // Met dit teken wordt een regel afgesloten. 0x0A is een linefeed <LF>, default voor EventGhost
#define SERIAL_TERMINATOR_2       0x00 // Met dit teken wordt een regel afgesloten. 0x0D is een Carriage Return <CR>, 0x00 = niet in gebruik.

#define EVENT_PART_COMMAND           1
#define EVENT_PART_TYPE              2
#define EVENT_PART_UNIT              3
#define EVENT_PART_PAR1              4
#define EVENT_PART_PAR2              5

#define DISPLAY_TIMESTAMP            1
#define DISPLAY_UNIT                 2
#define DISPLAY_DIRECTION            4
#define DISPLAY_SOURCE               8
#define DISPLAY_TRACE               16
#define DISPLAY_TAG                 32
#define DISPLAY_SERIAL              64

#define DISPLAY_RESET               DISPLAY_UNIT + DISPLAY_SOURCE + DISPLAY_DIRECTION + DISPLAY_TAG

// settings voor verzenden en ontvangen van IR/RF 
#define ENDSIGNAL_TIME          1500 // Dit is de tijd in milliseconden waarna wordt aangenomen dat het ontvangen één reeks signalen beëindigd is
#define SIGNAL_TIMEOUT_RF       5000 // na deze tijd in uSec. wordt één RF signaal als beëindigd beschouwd.
#define SIGNAL_TIMEOUT_IR      10000 // na deze tijd in uSec. wordt één IR signaal als beëindigd beschouwd.
#define TX_REPEATS                 5 // aantal herhalingen van een code binnen één RF of IR reeks
#define MIN_PULSE_LENGTH         100 // pulsen korter dan deze tijd uSec. worden als stoorpulsen beschouwd.
#define MIN_RAW_PULSES            16 // =8 bits. Minimaal aantal ontvangen bits*2 alvorens cpu tijd wordt besteed aan decodering, etc. Zet zo hoog mogelijk om CPU-tijd te sparen en minder 'onzin' te ontvangen.
#define SHARP_TIME               500 // tijd in milliseconden dat de nodo gefocust moet blijven luisteren naar één dezelfde poort na binnenkomst van een signaal

//****************************************************************************************************************************************

struct Settings
  {
  int     Version;
  byte    WiredInputThreshold[4], WiredInputSmittTrigger[4], WiredInputPullUp[4],WiredInputRange[4];
  byte    AnalyseSharpness;
  int     AnalyseTimeOut;
  byte    UserVar[USER_VARIABLES_MAX];
  byte    Unit;
  byte    Display;
  byte    TransmitPort;
  byte    TransmitRepeat;
  byte    WaitFreeRF_Window;
  byte    WaitFreeRF_Delay;
  boolean SendBusy;
  boolean WaitBusy;
  boolean DaylightSaving;
  int     DaylightSavingSet;
  }S;


// Timers voor de gebruiker
#define TIMER_MAX              15      // aantal beschikbare timers voor de user, gerekend vanaf 0 t/m 14
unsigned long UserTimer[TIMER_MAX];

// timers voor verwerking op intervals
#define Loop_INTERVAL_1          250  // tijdsinterval in ms. voor achtergrondtaken.
#define Loop_INTERVAL_2         5000  // tijdsinterval in ms. voor achtergrondtaken.
unsigned long StaySharpTimer=millis();
unsigned long LoopIntervalTimer_1=millis();// millis() maakt dat de intervallen van 1 en 2 niet op zelfde moment vallen => 1 en 2 nu asynchroon
unsigned long LoopIntervalTimer_2=0L;

// definiëer een kleine queue voor events die voorbij komen tijdens een delay
#define EVENT_QUEUE_MAX 15
unsigned long QueueEvent[EVENT_QUEUE_MAX];
byte QueuePort[EVENT_QUEUE_MAX];
byte QueuePos;

// Overige globals
boolean Simulate,RawsignalGet;
boolean WiredInputStatus[4],WiredOutputStatus[4];   // Wired variabelen
unsigned int RawSignal[RAW_BUFFER_SIZE+2];          // Tabel met de gemeten pulsen in microseconden. eerste waarde is het aantal bits*2
int BusyNodo;                                       // in deze variabele de status van het event 'Busy' van de betreffende units 1 t/m 15. bit-1 = unit-1
byte UserVarPrevious[USER_VARIABLES_MAX];
byte DaylightPrevious;                              // t.b.v. voorkomen herhaald genereren van events binnen de lopende minuut waar dit event zich voordoet
byte WiredCounter=0, VariableCounter;
byte EventlistDepth=0;                              // teller die bijhoudt hoe vaak er binnen een macro weer een macro wordt uitgevoerd. Voorkomt tevens vastlopers a.g.v. loops die door een gebruiker zijn gemaakt met macro's
byte Hold=false;
unsigned long Content=0L,ContentPrevious;
unsigned long Checksum=0L;
unsigned long SupressRepeatTimer;
unsigned long HoldTimer;
unsigned long EventTimeCodePrevious;                // t.b.v. voorkomen herhaald ontvangen van dezelfde code binnen ingestelde tijd
void(*Reset)(void)=0;                               //reset functie op adres 0
uint8_t RFbit,RFport,IRbit,IRport;
struct RealTimeClock {byte Hour,Minutes,Seconds,Date,Month,Day,Daylight; int Year,DaylightSaving;}Time;

void setup() 
  {    
  byte x;
    
  pinMode(IR_ReceiveDataPin,INPUT);
  pinMode(RF_ReceiveDataPin,INPUT);
  pinMode(RF_TransmitDataPin,OUTPUT);
  pinMode(RF_TransmitPowerPin,OUTPUT);
  pinMode(RF_ReceivePowerPin,OUTPUT);
  pinMode(IR_TransmitDataPin,OUTPUT);
  pinMode(MonitorLedPin,OUTPUT);
  pinMode(BuzzerPin, OUTPUT);
  
  digitalWrite(IR_ReceiveDataPin,HIGH);  // schakel pull-up weerstand in om te voorkomen dat er rommel binnenkomt als pin niet aangesloten
  digitalWrite(RF_ReceiveDataPin,HIGH);  // schakel pull-up weerstand in om te voorkomen dat er rommel binnenkomt als pin niet aangesloten
  digitalWrite(RF_ReceivePowerPin,HIGH); // Spanning naar de RF ontvanger aan.

  RFbit=digitalPinToBitMask(RF_ReceiveDataPin);
  RFport=digitalPinToPort(RF_ReceiveDataPin);  
  IRbit=digitalPinToBitMask(IR_ReceiveDataPin);
  IRport=digitalPinToPort(IR_ReceiveDataPin);

  Wire.begin();        // zet I2C communicatie gereed voor uitlezen van de realtime clock.
  Serial.begin(BAUD);  // Initialiseer de seriële poort
  IR38Khz_set();       // Initialiseet de 38Khz draaggolf voor de IR-zender.
  LoadSettings();      // laad alle settings zoals deze in de EEPROM zijn opgeslagen
  
  if(S.Version!=VERSION)ResetFactory(); // Als versienummer in EEPROM niet correct is, dan een ResetFactory.
  
  // initialiseer de Wired in- en uitgangen
  for(x=0;x<=3;x++)
    {
    pinMode(WiredDigitalOutputPin_1+x,OUTPUT); // definieer Arduino pin's voor Wired-Out
    digitalWrite(14+WiredAnalogInputPin_1+x,S.WiredInputPullUp[x]?HIGH:LOW);// Zet de pull-up weerstand van 20K voor analoge ingangen. Analog-0 is gekoppeld aan Digital-14
    }
    
  //Zorg ervoor dat er niet direct na een boot een CMD_CLOCK_DAYLIGHT event optreedt
  ClockRead();
  SetDaylight();
  DaylightPrevious=Time.Daylight;

  // Zet statussen WIRED_IN op hoog, anders wordt direct wij het opstarten vier maal een event gegenereerd omdat de pull-up weerstand analoge de waarden op FF zet
  for(x=0;x<4;x++){WiredInputStatus[x]=true;}

  PrintWelcome(); 
  ProcessEvent(command2event(CMD_BOOT_EVENT,0,0),VALUE_DIRECTION_INTERNAL,VALUE_SOURCE_SYSTEM,0,0);  // Voer het 'Boot' event uit.
  SerialHold(false);    // Zend een X-Off zodat de nodo geen seriele tekens ontvangt die nog niet verwerkt kunnen worden
  }

void loop() 
  {
  int x,y,z;
  
  SerialHold(false); // er mogen weer tekens binnen komen van SERIAL

  // hoofdloop: scannen naar signalen
  // dit is een tijdkritische loop die wacht tot binnengekomen event op IR, RF, SERIAL, CLOCK, DAYLIGHT, TIMER
  // als er geen signalen binnenkomen duurt deze hoofdloop +/- 35uSec. snel genoeg om geen signalen te missen.
  while(true)
    {
    if(Hold)
      {
      digitalWrite(MonitorLedPin,(millis()>>7)&0x01);
      // als in de hold-modus met reden het Delay commando EN de tijd is om, dan geneste aanroop loop() verlaten.
      if(Hold==CMD_DELAY && HoldTimer<millis())
        {
        Hold=false;
        return;
        }
        
      // als in de hold-modus met reden Busy commando EN de er zijn geen Nodo's meer met status Busy, dan geneste aanroop loop() verlaten.
      if(Hold==CMD_BUSY && (!BusyNodo || HoldTimer<millis()))
        {
        Hold=false;
        return;
        }
      }
    else
      digitalWrite(MonitorLedPin,LOW);           // LED weer uit


    // SERIAL: *************** kijk of er data klaar staat op de seriële poort **********************
    do
      {
      if(Serial.available()>0)
        {
        if(Content=Receive_Serial())
          {
          Nodo_2_RawSignal(Content);// bouw een RawSignal op zodat deze later eventueel kan worden verzonden met SendSignal
          ProcessEvent(Content,VALUE_DIRECTION_INPUT,VALUE_SOURCE_SERIAL,0,0);      // verwerk binnengekomen event.
          }
        StaySharpTimer=millis()+SHARP_TIME;
        SerialHold(false);
        }
      }while(millis()<StaySharpTimer);

    // IR: *************** kijk of er data staat op IR en genereer een event als er een code ontvangen is **********************
    do
      {
      if((*portInputRegister(IRport)&IRbit)==0)// Kijk if er iets op de IR poort binnenkomt. (Pin=LAAG als signaal in de ether). 
        {
        if(FetchSignal(IR_ReceiveDataPin,LOW,S.AnalyseTimeOut))// Als het een duidelijk IR signaal was
          {
          Content=AnalyzeRawSignal(); // Bereken uit de tabel met de pulstijden de 32-bit code. 
          if(Content)// als AnalyzeRawSignal een event heeft opgeleverd
            {
            StaySharpTimer=millis()+SHARP_TIME;
            if(Content==Checksum && (millis()>SupressRepeatTimer || Content!=ContentPrevious))// tweede maal ontvangen als checksum
               {
               SupressRepeatTimer=millis()+ENDSIGNAL_TIME; // zodat herhalingen niet opnieuw opgepikt worden
               ProcessEvent(Content,VALUE_DIRECTION_INPUT,VALUE_SOURCE_IR,0,0); // verwerk binnengekomen event.
               ContentPrevious=Content;
               }
            Checksum=Content;
            }
          }
        }
      }while(StaySharpTimer>millis());
  
  
    // RF: *************** kijk of er data start op RF en genereer een event als er een code ontvangen is **********************
    do// met StaySharp wordt focus gezet op luisteren naar RF, doordat andere input niet wordt opgepikt
      {
      if((*portInputRegister(RFport)&RFbit)==RFbit)// Kijk if er iets op de RF poort binnenkomt. (Pin=HOOG als signaal in de ether). 
        {
        if(FetchSignal(RF_ReceiveDataPin,HIGH,SIGNAL_TIMEOUT_RF))// Als het een duidelijk RF signaal was
          {
          Content=AnalyzeRawSignal(); // Bereken uit de tabel met de pulstijden de 32-bit code. 
          if(Content)// als AnalyzeRawSignal een event heeft opgeleverd
            {
            StaySharpTimer=millis()+SHARP_TIME;
            if(Content==Checksum && (millis()>SupressRepeatTimer || Content!=ContentPrevious))// tweede maal ontvangen als checksum
               {
               SupressRepeatTimer=millis()+ENDSIGNAL_TIME; // zodat herhalingen niet opnieuw opgepikt worden
               ProcessEvent(Content,VALUE_DIRECTION_INPUT,VALUE_SOURCE_RF,0,0); // verwerk binnengekomen event.
               ContentPrevious=Content;
               }
            Checksum=Content;
            }
          }
        }
      }while(millis()<StaySharpTimer);
     
    // 2: niet tijdkritische processen die periodiek uitgevoerd moeten worden
    if(LoopIntervalTimer_2<millis()) // lange interval
      {
      LoopIntervalTimer_2=millis()+Loop_INTERVAL_2; // reset de timer

      // CLOCK: **************** Lees periodiek de realtime klok uit en check op events  ***********************
      Content=ClockRead(); // Lees de Real Time Clock waarden in de struct Time
      if(CheckEventlist(Content) && EventTimeCodePrevious!=Content)
        {
        EventTimeCodePrevious=Content; 
        ProcessEvent(Content,VALUE_DIRECTION_INTERNAL,VALUE_SOURCE_CLOCK,0,0);      // verwerk binnengekomen event.
        }
      else
        Content=0L;
              
      // DAYLIGHT: **************** Check zonsopkomst & zonsondergang  ***********************
      SetDaylight();
      if(Time.Daylight!=DaylightPrevious)// er heeft een zonsondergang of zonsopkomst event voorgedaan
        {
        Content=command2event(CMD_CLOCK_EVENT_DAYLIGHT,Time.Daylight,0L);
        DaylightPrevious=Time.Daylight;
        ProcessEvent(Content,VALUE_DIRECTION_INTERNAL,VALUE_SOURCE_CLOCK,0,0);      // verwerk binnengekomen event.
        }
      }// lange interval

    // 1: niet tijdkritische processen die periodiek uitgevoerd moeten worden
    if(LoopIntervalTimer_1<millis())// korte interval
      {
      LoopIntervalTimer_1=millis()+Loop_INTERVAL_1; // reset de timer

      // TIMER: **************** Genereer event als één van de Timers voor de gebruiker afgelopen is ***********************    
      for(x=0;x<TIMER_MAX;x++)
        {
        if(UserTimer[x]!=0L)// als de timer actief is
          {
          if(UserTimer[x]<millis()) // als de timer is afgelopen.
            {
            UserTimer[x]=0;// zet de timer op inactief.
            Content=command2event(CMD_TIMER_EVENT,x+1,0);
            ProcessEvent(Content,VALUE_DIRECTION_INTERNAL,VALUE_SOURCE_TIMER,0,0);      // verwerk binnengekomen event.
            }
          }
        }

      // VARIABLE: *************** Behandel gewijzigde variabelen als en binnengekomen event ******************************
      for(x=0;x<USER_VARIABLES_MAX;x++)
        {
        if(S.UserVar[x]!=UserVarPrevious[x]) // de eerste gewijzigde variabele
          {
          UserVarPrevious[x]=S.UserVar[x];
          Content=command2event(CMD_VARIABLE_EVENT,x+1,S.UserVar[x]);
          ProcessEvent(Content,VALUE_DIRECTION_INTERNAL,VALUE_SOURCE_VARIABLE,0,0);      // verwerk binnengekomen event.
          }
        }
        
      // WIRED: *************** kijk of statussen gewijzigd zijn op WIRED **********************
      if(WiredCounter<3)
        WiredCounter++;
      else
        WiredCounter=0;

      // als de huidige waarde groter dan threshold EN de vorige keer was dat nog niet zo DAN verstuur code
      z=false; // vlag om te kijken of er een wijziging is die verzonden moet worden.
      y=WiredAnalog(WiredCounter);
     
      if(y>S.WiredInputThreshold[WiredCounter]+S.WiredInputSmittTrigger[WiredCounter] && !WiredInputStatus[WiredCounter])
        {
        WiredInputStatus[WiredCounter]=true;
        z=true;
        }

      if(y<S.WiredInputThreshold[WiredCounter]-S.WiredInputSmittTrigger[WiredCounter] && WiredInputStatus[WiredCounter])
        {
        WiredInputStatus[WiredCounter]=false;
        z=true;
        }

      if(z)// er is een verandering van status op de ingang. 
        {    
        Content=command2event(CMD_WIRED_IN_EVENT,WiredCounter+1,WiredInputStatus[WiredCounter]?VALUE_ON:VALUE_OFF);
        ProcessEvent(Content,VALUE_DIRECTION_INPUT,VALUE_SOURCE_WIRED,0,0);      // verwerk binnengekomen event.
        }
      }// korte interval
    }// // while 
  }
