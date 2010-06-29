#define BUILD 70

 /*****************************************************************************************************\
  Compiler            : 0018 Alpha  

  Compiled size       : 25510

  Hardware            : - Arduino ATMeg328 processor @16Mhz.
                        - Hardware en Arduino penbezetting volgens schema Nodo Due Rev.001

  Done                : Build 65:
                        - Issue #63: return waarde van cmd2str() aangepast. (Niet zeker of dit de oorzaak is, maar wel waarschijnlijk.)
                        - Issue #70: WaitRF: Herschreven, nu minder gevoelig voor korte stoorpulsen.
                        - Issue #69: Commando Home toegevoegd. Bereik van [0..15]
                        - Issue #69: Unit nummer nu bereik van [0..15]
                        - Issue #69: Codes voor andere units worden nu als normale tekst afgedrukt.
                        - Issue #69: In startup melding wordt nu ook Home weergegeven.
                        - Issue #69: Na aanpassen Unit- of Home-adres wordt de Eventlist gereset naar de factory default.
                        - Issue #55: Commando "Alarm" vervangen door "Sound"
                        - Nesting error opgelost. 
                        - Commando SAVE vervallen. Alle settings en variabelen worden nu altijd automatisch opgeslagen.
                        - TRACE laat nu zien welk nieuw event ontstaat uit een actie van de eventlist.
                        - EVENTLIST: toegevoegd bij weergave van een regel uit de eventlist.
                        - Voorafgaand aan de melding "Break!" wordt nu "SYSTEM:" weergegeven.
                        - Alle foutmeldingen worden nu voorafgegaan door `SYSTEM´
                        - Specifieke melding bij programmeren als macrolist vol is.
                        - StatusEvent 1e parameter werd weergegeven als een CommandoCode, nu als tekst
                        
                        Build 66:
                        - Issue #75: ResetFactory, EventlistErase, Unit en Home mogen uitsluitend nog via SERIAL worden verzonden.
                        - Issue #76: Issues worden nu uitsluitend uitgevoerd door unit waar naar geforward wordt
                        - OtherUnit toegevoegd als type event bij event Wildcard.
                        - Bug: Verzenden van IR werd niet vermeld als een OUTPUT melding
                        - als Commando van RF afkomstig, dan WaitForFreeRF en een pausetijd die afhankelijk is van het unit adres.
                          dit om te voorkomen dat responde wordt gegeven terwijl de zendende nodo nog niet klaar is
                        - In de notatie Unit-x uit de haakjes '( ....)' gehaald. Maakt geen deel uit van het commando
                        - Diverse technische aanpassingen in verwerking commando's (t.b.v. Multi-nodo)

                        Build 67:
                        - Reset factory heeft nu geen gevulde parameters nodig. Commando alleen uitvoerbaar via SERIAL is veilig genoeg.
                        - commando 'Reset' zet nu ook alle variabelen op 0.
                       
                        Build 69:
                       - Aanpassing functie WaitForFreeRF(). Andere wijze van detecteren vrije band t.b.v. Multi Nodo omgevingen.
                       - Home en unit code bereik aangepast. Nulwaarde is niet meer geldig ! (vooruitlopend op wildcard unit)
                       - Eventlist wissen nu 100x sneller.
                       - Commando 'UserCmd' toegevoegd.
                       - Geen 'leeg' event of lege actie meer mogelijk in de eventlist.
                       - Issue #92: E.TypePrevious aanpassing.
                        
                        Build 70:
                       - AllOn/AllOff, Bright/Dim, Dim<level> en X10 commando's toegevoegd.
                       - Nieuwe KAKU group, NewKAKU dim en X10 code toegevoegd.
                       - Uitschakelen RF bij IR ontvangst.
                       - BaseCode naar settings
                        
  01-06-2010: Lokale bestanden en versienummering met SVN ondergebracht in GoogleCode.
  
  
  Todo                : - Issue #52: "EventlistWrite ;0xF000001F; BreakOnVarNEqu 1,0;". doet het niet. 
                          "EventlistWrite; 0xF000001F; BreakOnVarNEqu" 1,0; wel !
                          Doet zich alleen voor als er tussen de Off en de puntkomma een spatie staat. Relatie met issue #52?
                        

 ********************************************************************************************************
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
 * Voor discussie: Zie Logitech Harmony forum        : http://www.harmony-forum.nl 
 * Uitgebreide documentatie is te vinden op          : http://members.chello.nl/p.tonkes8/index.html
 * bugs kunnen worden gelogd op                      : https://code.google.com/p/arduino-nodo/
 * Compiler voor deze programmacode te downloaden op : http://arduino.cc
 * Voor vragen of suggesties, mail naar              : p.k.tonkes@gmail.com
 *
 ********************************************************************************************************/


// Settings die eenmalig door de gebruiker aangepast en mee gecompileerd kunnen worden:

#define BAUD                   19200 // Baudrate voor seriële communicatie.
#define SERIAL_TERMINATOR_1     0x0A // Met dit teken wordt een regel afgesloten. 0x0A is een linefeed <LF>, default voor EventGhost
#define SERIAL_TERMINATOR_2     0x00 // Met dit teken wordt een regel afgesloten. 0x0D is een Carriage Return <CR>, 0x00 = niet in gebruik.
#define RF_ENDSIGNAL_TIME       1000 // Dit is de tijd in milliseconden waarna wordt aangenomen dat het ontvangen één RF signaal beëindigd is
#define IR_ENDSIGNAL_TIME       1000 // Dit is de tijd in milliseconden waarna wordt aangenomen dat het ontvangen één IR signaal beëindigd is


//****************************************************************************************************************************************

#include "pins_arduino.h"
#include "ctype.h"
#include <EEPROM.h>
#include <Wire.h>
#include <avr/pgmspace.h>

// ********alle strings naar PROGMEM om hiermee veel RAM-geheugen te sparen ***********************************************
prog_char PROGMEM Text_01[] = "NODO-2 (Beta) Build ";
prog_char PROGMEM Text_02[] = "SYSTEM: Macrolist full!";
prog_char PROGMEM Text_03[] = ", Home ";
prog_char PROGMEM Text_04[] = "SYSTEM: 433Mhz busy! ";
prog_char PROGMEM Text_06[] = "SYSTEM: Error in command! ";
prog_char PROGMEM Text_07[] = " ,Simulated @";
prog_char PROGMEM Text_09[] = "SYSTEM: Break!";
prog_char PROGMEM Text_10[] = "INPUT: ";
prog_char PROGMEM Text_11[] = "OUTPUT: ";
prog_char PROGMEM Text_12[] = "SYSTEM: ";
prog_char PROGMEM Text_13[] = "EXECUTE: ";
prog_char PROGMEM Text_14[] = ", Unit ";
prog_char PROGMEM Text_15[] = "EVENTLIST: ";
prog_char PROGMEM Text_18[] = "SYSTEM: Serial timeout error!";
prog_char PROGMEM Text_21[] = "************************************************************";
prog_char PROGMEM Text_26[] = "SYSTEM: Waiting for RF/IR event...";
prog_char PROGMEM Text_30[] = ", Rawsignal=(";
prog_char PROGMEM Text_50[] = "SYSTEM: Nesting error in Eventlist!";
prog_char PROGMEM Text_99[] = "*** under construction ***";

// 0 tot 6 zijn KAKU, NewKAKU, X10 commandos
// Vanaf nul omdat hiervoor maar 4 bits (bit 0-3) beschikbaar zijn (bit 4-7 zijn dan het dim niveau)
#define CMD_OFF 0
#define CMD_ON 1
#define CMD_ALLOFF 2
#define CMD_ALLON 3
#define CMD_DIM 4
#define CMD_BRIGHT 5
#define CMD_DIMLEVEL 6
// Vanaf hier de rest van de lijst +5
#define CMD_TYPE_EVENT 7
#define CMD_TYPE_UNKNOWN 8
#define CMD_PORT_IR 9
#define CMD_PORT_RF 10
#define CMD_SOURCE_CLOCK 11
#define CMD_SOURCE_TIMER 12
#define CMD_PORT_SERIAL 13
#define CMD_PORT_WIRED 14
#define CMD_SOURCE_VARIABLE 15
#define CMD_SOURCE_MACRO 16
#define CMD_SOURCE_SYSTEM 17
#define CMD_TYPE_COMMAND 18
#define CMD_TYPE_OTHERUNIT 19
#define CMD_FORWARD 20
#define CMD_EVENTLIST_SHOW 21
#define CMD_EVENTLIST_ERASE 22
#define CMD_EVENTLIST_WRITE 23
#define CMD_RAWSIGNAL_GET 24
#define CMD_RAWSIGNAL_PUT 25
#define CMD_RESET 26
#define CMD_RESET_FACTORY 27
#define CMD_SIMULATE 28
#define CMD_STATUS 29
#define CMD_STATUS_LIST 30
#define CMD_STATUS_EVENT 31
#define CMD_TRACE 32
#define CMD_UNIT 33
#define CMD_HOME 34
#define CMD_BASECODE 35
#define CMD_BUILD 36
#define CMD_SOUND 37
#define CMD_USER 38
#define CMD_BREAK_ON_VAR_EQU 39
#define CMD_BREAK_ON_VAR_LESS 40
#define CMD_BREAK_ON_VAR_MORE 41
#define CMD_BREAK_ON_VAR_NEQU 42
#define CMD_CLOCK_DATE 43
#define CMD_CLOCK_YEAR 44
#define CMD_CLOCK_TIME 45
#define CMD_CLOCK_DLS 46
#define CMD_DELAY 47
#define CMD_RECEIVE_REPEATS_IR 48
#define CMD_RECEIVE_REPEATS_RF 49
#define CMD_SEND_IR 50
#define CMD_SEND_RAW_IR 51
#define CMD_SEND_RF 52
#define CMD_SEND_RAW_RF 53
#define CMD_SIMULATE_DAY 54
#define CMD_TIMER_SET 55
#define CMD_TIMER_RANDOM 56
#define CMD_TIMER_RESET 57
#define CMD_VARIABLE_SET 58
#define CMD_VARIABLE_CLEAR 59
#define CMD_VARIABLE_DAYLIGHT 60
#define CMD_VARIABLE_DEC 61
#define CMD_VARIABLE_INC 62
#define CMD_VARIABLE_VARIABLE 63
#define CMD_VARIABLE_WIRED_ANALOG 64
#define CMD_WIRED_ANALOG 65
#define CMD_WIRED_OUT 66
#define CMD_WIRED_PULLUP 67
#define CMD_WIRED_SMITTTRIGGER 68
#define CMD_WIRED_THRESHOLD 69
#define CMD_CLOCK_EVENT_DAYLIGHT 70
#define CMD_EVENT_STATUS 71
#define CMD_TIMER_EVENT 72
#define CMD_VARIABLE_EVENT 73
#define CMD_WIRED_IN_EVENT 74
#define CMD_CLOCK_EVENT_ALL 75
#define CMD_CLOCK_EVENT_MON 76
#define CMD_CLOCK_EVENT_TUE 77
#define CMD_CLOCK_EVENT_WED 78
#define CMD_CLOCK_EVENT_THU 79
#define CMD_CLOCK_EVENT_FRI 80
#define CMD_CLOCK_EVENT_SAT 81
#define CMD_CLOCK_EVENT_SUN 82
#define CMD_KAKU 83
#define CMD_KAKU_NEW 84
#define CMD_X10 85
#define CMD_BOOT_EVENT 86
#define CMD_WILDCARD_EVENT 87

prog_char PROGMEM Cmd_0[]="Off";
prog_char PROGMEM Cmd_1[]="On";
prog_char PROGMEM Cmd_2[]="AllOff";
prog_char PROGMEM Cmd_3[]="AllOn";
prog_char PROGMEM Cmd_4[]="Dim";
prog_char PROGMEM Cmd_5[]="Bright";
prog_char PROGMEM Cmd_6[]="Dim";
prog_char PROGMEM Cmd_7[]="EventUnknown";
prog_char PROGMEM Cmd_8[]="Event";
prog_char PROGMEM Cmd_9[]="IR";
prog_char PROGMEM Cmd_10[]="RF";
prog_char PROGMEM Cmd_11[]="Clock";
prog_char PROGMEM Cmd_12[]="Timers";
prog_char PROGMEM Cmd_13[]="Serial";
prog_char PROGMEM Cmd_14[]="Wired";
prog_char PROGMEM Cmd_15[]="Variables";
prog_char PROGMEM Cmd_16[]="EventList";
prog_char PROGMEM Cmd_17[]="System";
prog_char PROGMEM Cmd_18[]="Command";
prog_char PROGMEM Cmd_19[]="OtherUnit";
prog_char PROGMEM Cmd_20[]="Forward";
prog_char PROGMEM Cmd_21[]="EventlistShow";
prog_char PROGMEM Cmd_22[]="EventlistErase";
prog_char PROGMEM Cmd_23[]="EventlistWrite";
prog_char PROGMEM Cmd_24[]="RawsignalGet";
prog_char PROGMEM Cmd_25[]="RawsignalPut";
prog_char PROGMEM Cmd_26[]="Reset";
prog_char PROGMEM Cmd_27[]="ResetFactory";
prog_char PROGMEM Cmd_28[]="Simulate";
prog_char PROGMEM Cmd_29[]="Status";
prog_char PROGMEM Cmd_30[]="StatusList";
prog_char PROGMEM Cmd_31[]="StatusEvent";
prog_char PROGMEM Cmd_32[]="Trace";
prog_char PROGMEM Cmd_33[]="Unit";
prog_char PROGMEM Cmd_34[]="Home";
prog_char PROGMEM Cmd_35[]="BaseCode";
prog_char PROGMEM Cmd_36[]="Build";
prog_char PROGMEM Cmd_37[]="Sound";
prog_char PROGMEM Cmd_38[]="UserCmd";
prog_char PROGMEM Cmd_39[]="BreakOnVarEqu";
prog_char PROGMEM Cmd_40[]="BreakOnVarLess";
prog_char PROGMEM Cmd_41[]="BreakOnVarMore";
prog_char PROGMEM Cmd_42[]="BreakOnVarNEqu";
prog_char PROGMEM Cmd_43[]="ClockDate";
prog_char PROGMEM Cmd_44[]="ClockYear";
prog_char PROGMEM Cmd_45[]="ClockTime";
prog_char PROGMEM Cmd_46[]="ClockDLS";
prog_char PROGMEM Cmd_47[]="Delay";
prog_char PROGMEM Cmd_48[]="ReceiveRepeatsIR";
prog_char PROGMEM Cmd_49[]="ReceiveRepeatsRF";
prog_char PROGMEM Cmd_50[]="SendIR";
prog_char PROGMEM Cmd_51[]="SendRawIR";
prog_char PROGMEM Cmd_52[]="SendRF";
prog_char PROGMEM Cmd_53[]="SendRawRF";
prog_char PROGMEM Cmd_54[]="SimulateDay";
prog_char PROGMEM Cmd_55[]="TimerSet";
prog_char PROGMEM Cmd_56[]="TimerRandom";
prog_char PROGMEM Cmd_57[]="TimerReset";
prog_char PROGMEM Cmd_58[]="VariableSet";
prog_char PROGMEM Cmd_59[]="VariableClear";
prog_char PROGMEM Cmd_60[]="VariableDaylight";
prog_char PROGMEM Cmd_61[]="VariableDec";
prog_char PROGMEM Cmd_62[]="VariableInc";
prog_char PROGMEM Cmd_63[]="VariableVariable";
prog_char PROGMEM Cmd_64[]="VariableWiredAnalog";
prog_char PROGMEM Cmd_65[]="WiredAnalog";
prog_char PROGMEM Cmd_66[]="WiredOut";
prog_char PROGMEM Cmd_67[]="WiredPullup";
prog_char PROGMEM Cmd_68[]="WiredSmittTrigger";
prog_char PROGMEM Cmd_69[]="WiredThreshold";
prog_char PROGMEM Cmd_70[]="ClockDaylight";
prog_char PROGMEM Cmd_71[]="EventStatus";
prog_char PROGMEM Cmd_72[]="Timer";
prog_char PROGMEM Cmd_73[]="Variable";
prog_char PROGMEM Cmd_74[]="WiredIn";
prog_char PROGMEM Cmd_75[]="ClockAll";
prog_char PROGMEM Cmd_76[]="ClockMon";
prog_char PROGMEM Cmd_77[]="ClockTue";
prog_char PROGMEM Cmd_78[]="ClockWed";
prog_char PROGMEM Cmd_79[]="ClockThu";
prog_char PROGMEM Cmd_80[]="ClockFri";
prog_char PROGMEM Cmd_81[]="ClockSat";
prog_char PROGMEM Cmd_82[]="ClockSun";
prog_char PROGMEM Cmd_83[]="KAKU";
prog_char PROGMEM Cmd_84[]="NewKAKU";
prog_char PROGMEM Cmd_85[]="X10";
prog_char PROGMEM Cmd_86[]="Boot";
prog_char PROGMEM Cmd_87[]="Wildcard";

#define RANGE_EVENT 70 // alle codes groter of gelijk aan deze waarde zijn een event.
#define COMMAND_MAX 88 // aantal commando's

// tabel die refereert aan de commando strings
PROGMEM const char *CommandText_tabel[]={Cmd_0 ,
    Cmd_1 ,Cmd_2 ,Cmd_3 ,Cmd_4 ,Cmd_5 ,Cmd_6 ,Cmd_7 ,Cmd_8 ,Cmd_9 ,Cmd_10,
    Cmd_11,Cmd_12,Cmd_13,Cmd_14,Cmd_15,Cmd_16,Cmd_17,Cmd_18,Cmd_19,Cmd_20,
    Cmd_21,Cmd_22,Cmd_23,Cmd_24,Cmd_25,Cmd_26,Cmd_27,Cmd_28,Cmd_29,Cmd_30,
    Cmd_31,Cmd_32,Cmd_33,Cmd_34,Cmd_35,Cmd_36,Cmd_37,Cmd_38,Cmd_39,Cmd_40,
    Cmd_41,Cmd_42,Cmd_43,Cmd_44,Cmd_45,Cmd_46,Cmd_47,Cmd_48,Cmd_49,Cmd_50,
    Cmd_51,Cmd_52,Cmd_53,Cmd_54,Cmd_55,Cmd_56,Cmd_57,Cmd_58,Cmd_59,Cmd_60,
    Cmd_61,Cmd_62,Cmd_63,Cmd_64,Cmd_65,Cmd_66,Cmd_67,Cmd_68,Cmd_69,Cmd_70,
    Cmd_71,Cmd_72,Cmd_73,Cmd_74,Cmd_75,Cmd_76,Cmd_77,Cmd_78,Cmd_79,Cmd_80,
    Cmd_81,Cmd_82,Cmd_83,Cmd_84,Cmd_85,Cmd_86,Cmd_87};
          
PROGMEM prog_uint16_t Sunrise[]={         
    528,525,516,503,487,467,446,424,401,378,355,333,313,295,279,268,261,259,263,271,283,297,312,329,
    345,367,377,394,411,428,446,464,481,498,512,522,528,527};
      
PROGMEM prog_uint16_t Sunset[]={          
    999,1010,1026,1044,1062,1081,1099,1117,1135,1152,1169,1186,1203,1219,1235,1248,1258,1263,1264,1259,
    1249,1235,1218,1198,1177,1154,1131,1107,1084,1062,1041,1023,1008,996,990,989,993,1004};

// Declaratie aansluitingen op de Arduino
#define IR_ReceiveDataPin          3  // Op deze input komt het IR signaal binnen van de TSOP. Bij HIGH bij geen signaal.
#define IR_TransmitDataPin        11  // Aan deze pin zit een zender IR-Led. (gebufferd via transistor i.v.m. hogere stroom die nodig is voor IR-led)
#define RF_TransmitPowerPin        4  // +5 volt / Vcc spanning naar de zender.
#define RF_TransmitDataPin         5  // data naar de zender
#define RF_ReceiveDataPin          2  // Op deze input komt het 433Mhz-RF signaal binnen. LOW bij geen signaal.
#define RF_ReceivePowerPin        12  // Spanning naar de ontvanger via deze pin.
#define MonitorLedPin             13  // bij iedere ontvangst of verzending licht deze led kort op.
#define BuzzerPin                  6  // luidspreker aansluiting
#define WiredAnalogInputPin_1      0  // vier analoge inputs van 0 tot en met 3
#define WiredDigitalOutputPin_1    7  // vier digitale outputs van 7 tot en met 10
// D0 en D1 kunnen niet worden gebruikt. In gebruik door de FTDI-chip voor seriele USB-communiatie (TX/RX).
// A4 en A5 worden gebruikt voor I2C communicatie met o.a. de real-time clock

#define UNIT                       0x1 // Unit nummer van de Nodo. Bij gebruik van meerdere nodo's deze uniek toewijzen [1..F]
#define HOME                       0x1 // Home adres van de Nodo. Bij gebruik van meerdere nodo's deze hetzelfde houden [1..F]
#define BASECODE                   0x0 // Base code voor 26-bit code. Geeft de mogenlijkheid een bestaande zender te emuleren. Hierbij worden de nodo-home & unit codes en de home codes van het NewKAKU commando bij opgeteld.
#define Eventlist_OFFSET            64 // Eerste deel van het EEPROM geheugen is voor de settings. Reserveer __ bytes. Deze niet te gebruiken voor de Eventlist.
#define Eventlist_MAX              120 // aantal events dat de lijst bevat in het EEPROM geheugen van de ATMega328. Iedere event heeft 8 bytes nodig. eerste adres is 0
#define USER_TIMER_MAX              15 // aantal beschikbare timers voor de user.
#define USER_VARIABLES_MAX          15 // aantal beschikbare gebbruikersvariabelen voor de user.
#define RAW_BUFFER_SIZE            200 // Maximaal aantal te ontvangen bits*2. 
#define DIRECTION_IN                 1
#define DIRECTION_OUT                2
#define DIRECTION_INTERNAL           3
#define DIRECTION_EXECUTE            4

struct RealTimeClock {int Hour,Minutes,Seconds,Date,Month,Day,Daylight,Year;}Time;
unsigned long UserTimer[USER_TIMER_MAX];
byte TimerCounter=0;
byte UserVarPrevious[USER_VARIABLES_MAX];
boolean WiredInputStatus[4],WiredOutputStatus[4];   // Wired variabelen
unsigned int RawSignal[RAW_BUFFER_SIZE];            // Tabel met de gemeten pulsen in microseconden. eerste waarde is het aantal bits*2
unsigned long EventTimeCodePrevious;                // t.b.v. voorkomen herhaald ontvangen van dezelfde code binnen ingestelde tijd
byte DaylightPrevious;                              // t.b.v. voorkomen herhaald genereren van events binnen de lopende minuut waar dit event zich voordoet
byte depth;                                         // teller die bijhoudt hoe vaak er binnen een macro weer een macro wordt uitgevoerd. Voorkomt tevens vastlopers a.g.v. loops die door een gebruiker zijn gemaakt met macro's
byte Forward;
boolean Simulate, Trace;

void(*Reset)(void)=0; //declare reset function @ address 0

struct Settings
  {
  byte Build;
  boolean RFRepeatChecksum, RFRepeatSuppress;
  boolean IRRepeatChecksum, IRRepeatSuppress;
  boolean DaylightSaving;
  byte WiredInputThreshold[4], WiredInputSmittTrigger[4], WiredInputPullUp[4];
  byte UserVar[USER_VARIABLES_MAX];
  byte Unit;
  byte Home;
  unsigned long BaseCode;
  }S;

struct EventStruct
  {
  unsigned long Content,ContentPrevious;
  byte Port,PortPrevious;
  byte Type,TypePrevious;
  byte Direction;
  }E;

uint8_t RFbit,RFport,IRbit,IRport;
  
// *****************************************************************************************************************************

void setup() 
  {    
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
  SerialHold(true);
  IR38Khz_set();       // Initialiseet de 38Khz draaggolf voor de IR-zender.
  LoadSettings();      // laad alle settings zoals deze in de EEPROM zijn opgeslagen
  if(S.Build!=BUILD)ResetFactory();
  Forward=S.Unit;
  
  // initialiseer de Wired in- en uitgangen
  for(byte x=0;x<=3;x++)
    {
    pinMode(WiredDigitalOutputPin_1+x,OUTPUT); // definieer Arduino pin's voor Wired-Out
    digitalWrite(14+WiredAnalogInputPin_1+x,S.WiredInputPullUp[x]?HIGH:LOW);// Zet de pull-up weerstand van 20K voor analoge ingangen. Analog-0 is gekoppeld aan Digital-14
    }
    
  //Zorg ervoor dat er niet direct na een boot een CMD_CLOCK_DAYLIGHT event optreedt
  ClockRead();
  CheckDaylight();
  DaylightPrevious=Time.Daylight;
  // Zet statussen WIRED_IN op hoog, anders wordt direct wij het opstarten vier maal een event gegenereerd omdat de pull-up weerstand analoge de waarden op FF zet
  for(byte x=0;x<4;x++){WiredInputStatus[x]=true;}

  PrintTerm();
  Serial.print(Text(Text_21));
  PrintTerm();
  Serial.print(Text(Text_01));
  PrintCode(S.Build);
  Serial.print(Text(Text_03));
  PrintCode(S.Home);
  Serial.print(Text(Text_14));
  PrintCode(S.Unit);
  PrintTerm();
  Serial.print(Text(Text_21));
  PrintTerm();
 
  ExecuteEventlist(command2event(CMD_BOOT_EVENT,0,0),CMD_SOURCE_SYSTEM,0,0,0,0);  
  }

void loop() 
  {
  byte x;

  EventScan();// wacht op een event 
  digitalWrite(MonitorLedPin,HIGH); // LED aan om te bevestigen dat er iets is ontvangen
    
  E.Type=EventType(E.Content);
  PrintEvent(DIRECTION_IN,E.Port,E.Type,E.Content);
  PrintTerm();

  switch(E.Type)
    {
    case CMD_TYPE_COMMAND: // Als commando bedoeld is voor deze unit
      if(Forward!=S.Unit)// Als commando moet worden geforward naar andere unit
        {// doorsturen naar andere Nodo...
        ExecuteCommand(command2event(CMD_SEND_RF,2,0),0,0,(E.Content&0x00ffffff)|((unsigned long)(Forward))<<24|((unsigned long)(S.Home))<<28,0,0);
        Forward=S.Unit;
        }      
      else    
        {// command zelf uivoeren.
        if(E.Port==CMD_PORT_RF)WaitForFreeRF();// als van RF afkomstig, dan een wachten op vrije ether en pausetijd die afhankelijk is van het unit adres.
        ExecuteCommand(E.Content,E.Port,E.Type,E.ContentPrevious,E.PortPrevious,E.TypePrevious); // zelf uitvoeren
        }
      break;
      
    case CMD_TYPE_OTHERUNIT: // bestemd voor een andere Nodo
      break; // verder niets mee doen
      
    default: // het is geen commando maar een ander type event.
      ExecuteEventlist(E.Content,E.Port,E.Type,E.ContentPrevious,E.PortPrevious,E.TypePrevious);
    }
    
  // maak gereed voor volgende event
  E.ContentPrevious=E.Content;
  E.PortPrevious=E.Port; 
  E.TypePrevious=E.Type;

  E.Content=0L;
  E.Port=0;
  E.Type=0;
  E.Direction=0;
  depth=0;
  
  digitalWrite(MonitorLedPin,LOW); // LED uit
  }


