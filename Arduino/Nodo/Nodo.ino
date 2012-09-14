
/****************************************************************************************************************************\ 
* Arduino project "Nodo" © Copyright 2012 Paul Tonkes 
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
******************************************************************************************************************************
*
* Voor toelichting op de licentievoorwaarden zie    : http://www.gnu.org/licenses
* Uitgebreide documentatie is te vinden op          : http://www.nodo-domotica.nl
* bugs kunnen worden gelogd op                      : https://code.google.com/p/arduino-nodo/
* Compiler voor deze programmacode te downloaden op : http://arduino.cc
* Voor vragen of suggesties, mail naar              : p.k.tonkes@gmail.com
* Compiler                                          : Arduino Compiler met minimaal versie 1.0.1
*
/****************************** Door gebruiker in te stellen: ***************************************************************/

#define NODO_MEGA          0
#define USER_PLUGIN        0                                     // Plugin: 0 = niet compileren, 1 = wel compileren
#define USER_PLUGIN_NAME   "UserPlugin"                          // Commando naam waarmee de plugin kan worden aangeroepen
#define UNIT_NODO_MINI     15                                    // Default unitnummer van een Nodo-Mini na een reset van de Nodo.
#define UNIT_NODO_MEGA     1                                     // Default unitnummer van een Nodo-Mega na een reset van de Nodo.

// De code kan worden gecompileerd als een Nodo-Mini voor de Arduino met een ATMega328 processor
// of voor een Nodo-Mega voor een Arduino met een ATMega1280 of ATMega2560
// Voor de Nodo-Mega variant bij onderstaande zeven regels de // remarks verwijderen.

#define NODO_MEGA          1
#define TRACE              1
#include <SD.h>
#include <EthernetNodo.h>
#include <SPI.h>
#define ETHERNET           1                                     // EthernetShield: 0 = afwezig, 1 = aanwezig
#define NODO_MAC           0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF    // Default Nodo MACadres


// Onderstaand de formules die gebruikt worden voor omrekening van pulsen naar analoge waarden.
// Zet de formules zo op dat uitsluitend met gehele getallen gerekend wordt.
// Het resulaat van de formule wordt gedeeld door 100 weergegeven. Dus de waarde 1 representeert de
// analoge waarde 0.01
// Het bereik van het resultaat uit de formule moet zich tussen van -1000 tot 1000 bevinden en wordt dan weergegeven als resp. -100.0 tot 100.0
// Deling door nul is ongeldig, maar zal niet tot een error leiden.
//
// a          = variabele met resultaat van de berekening.
// PulseTime  = tijd tussen twee pulsen uitgedrukt in milliseconden.
// PulseCount = Aantal pulsen tussen twee metingen.

#define FORMULA_1            a = 3600000/PulseTime;            /* 1000 pulsen in een uur = 1KWh */
#define FORMULA_2            a = PulseCount; PulseCount=0;
#define FORMULA_3            a = PulseCount / 10; PulseCount=0;
#define FORMULA_4            a = 0;
#define FORMULA_5            a = 0;
#define FORMULA_6            a = 0;
#define FORMULA_7            a = 0;
#define FORMULA_8            a = 0;
#define FORMULA_9            a = 0;
#define FORMULA_10           a = 0;

/****************************************************************************************************************************/
//#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) 

#define SETTINGS_VERSION     10
#define NODO_BUILD          430
#include <EEPROM.h>
#include <Wire.h>

// strings met vaste tekst naar PROGMEM om hiermee RAM-geheugen te sparen.
prog_char PROGMEM Text_01[] = "Nodo Domotica controller (c) Copyright 2012 P.K.Tonkes.";
prog_char PROGMEM Text_02[] = "Licensed under GNU General Public License.";
prog_char PROGMEM Text_03[] = "Enter your password: ";
prog_char PROGMEM Text_04[] = "SunMonTueWedThuFriSat";
prog_char PROGMEM Text_05[] = "0123456789abcdef";
prog_char PROGMEM Text_22[] = "!******************************************************************************!";
 
#if NODO_MEGA
prog_char PROGMEM Text_06[] = "Waiting for busy Nodo: ";
prog_char PROGMEM Text_07[] = "Waiting for signal...";
prog_char PROGMEM Text_08[] = "ATTENTION: Beta version. Nodo in trace mode! Logging to TRACE.DAT on SDCard.";
prog_char PROGMEM Text_09[] = "(Last 10KByte)";
prog_char PROGMEM Text_10[] = "Nodo"; // Default wachtwoord na een reset
prog_char PROGMEM Text_13[] = "RawSignal saved.";
prog_char PROGMEM Text_14[] = "Event=";
prog_char PROGMEM Text_17[] = "payload";
prog_char PROGMEM Text_18[] = "accept";
prog_char PROGMEM Text_19[] = "close";
prog_char PROGMEM Text_20[] = "quintessence";
prog_char PROGMEM Text_21[] = "payload withoutRelease";
prog_char PROGMEM Text_23[] = "log.dat";
prog_char PROGMEM Text_24[] = "Queue: Capturing events...";
prog_char PROGMEM Text_15[] = "Queue.dat";
prog_char PROGMEM Text_26[] = "Queue: Processing events...";
prog_char PROGMEM Text_27[] = "raw/raw"; // Directory op de SDCard voor opslag van sleutels naar .hex files
prog_char PROGMEM Text_28[] = "raw/key"; // Directory op de SDCard voor opslag RawSignal
prog_char PROGMEM Text_29[] = "Queue: Finished.";
prog_char PROGMEM Text_30[] = "Terminal connection closed.";


// Commando's:
prog_char PROGMEM Cmd_000[]=""; // dummy. Niet gebruiken
prog_char PROGMEM Cmd_001[]="BreakOnDaylight";
prog_char PROGMEM Cmd_002[]="BreakOnVarEqu";
prog_char PROGMEM Cmd_003[]="BreakOnVarLess";
prog_char PROGMEM Cmd_004[]="BreakOnVarMore";
prog_char PROGMEM Cmd_005[]="BreakOnVarNEqu";
prog_char PROGMEM Cmd_006[]="BreakOnLater";
prog_char PROGMEM Cmd_007[]="BreakOnEarlier";
prog_char PROGMEM Cmd_008[]="ClockSetDate";
prog_char PROGMEM Cmd_009[]="ClockSetYear";
prog_char PROGMEM Cmd_010[]="ClockSetTime";
prog_char PROGMEM Cmd_011[]="ClockSetDOW";
prog_char PROGMEM Cmd_012[]="EventlistErase";
prog_char PROGMEM Cmd_013[]="EventlistShow";
prog_char PROGMEM Cmd_014[]="EventlistWrite";
prog_char PROGMEM Cmd_015[]="VariableSave";
prog_char PROGMEM Cmd_016[]="RawSignalSave";
prog_char PROGMEM Cmd_017[]="RawSignalSend";
prog_char PROGMEM Cmd_018[]="Reset";
prog_char PROGMEM Cmd_019[]="SendKAKU";
prog_char PROGMEM Cmd_020[]="SendNewKAKU";
prog_char PROGMEM Cmd_021[]="Delay";
prog_char PROGMEM Cmd_022[]="SendTo";
prog_char PROGMEM Cmd_023[]="SimulateDay";
prog_char PROGMEM Cmd_024[]="Sound";
prog_char PROGMEM Cmd_025[]="Debug";
prog_char PROGMEM Cmd_026[]=USER_PLUGIN_NAME;
prog_char PROGMEM Cmd_027[]="TimerRandom";
prog_char PROGMEM Cmd_028[]="TimerSetSec";
prog_char PROGMEM Cmd_029[]="TimerSetMin";
prog_char PROGMEM Cmd_030[]="ID";
prog_char PROGMEM Cmd_031[]="Unit";
prog_char PROGMEM Cmd_032[]="WaitBusy";
prog_char PROGMEM Cmd_033[]="VariableDec";
prog_char PROGMEM Cmd_034[]="VariableInc";
prog_char PROGMEM Cmd_035[]="VariableSet";
prog_char PROGMEM Cmd_036[]="VariableVariable";
prog_char PROGMEM Cmd_037[]="SendEvent";
prog_char PROGMEM Cmd_038[]="WaitFreeRF";
prog_char PROGMEM Cmd_039[]="WiredAnalog";
prog_char PROGMEM Cmd_040[]="WiredOut";
prog_char PROGMEM Cmd_041[]="WiredPullup";
prog_char PROGMEM Cmd_042[]="WiredSmittTrigger";
prog_char PROGMEM Cmd_043[]="WiredThreshold";
prog_char PROGMEM Cmd_044[]="SendUserEvent";
prog_char PROGMEM Cmd_045[]="RawSignalCopy";
prog_char PROGMEM Cmd_046[]="WildCard";
prog_char PROGMEM Cmd_047[]="SendBusy";
prog_char PROGMEM Cmd_048[]="ClientIP";
prog_char PROGMEM Cmd_049[]="Password";
prog_char PROGMEM Cmd_050[]="EventlistFile";
prog_char PROGMEM Cmd_051[]="WiredCalibrateHigh";
prog_char PROGMEM Cmd_052[]="WiredCalibrateLow";
prog_char PROGMEM Cmd_053[]="Lock";
prog_char PROGMEM Cmd_054[]="Status"; 
prog_char PROGMEM Cmd_055[]="";
prog_char PROGMEM Cmd_056[]="AnalyseSettings";
prog_char PROGMEM Cmd_057[]="OutputIP";
prog_char PROGMEM Cmd_058[]="OutputIR";
prog_char PROGMEM Cmd_059[]="OutputRF";
prog_char PROGMEM Cmd_060[]="ReceiveSettings";
prog_char PROGMEM Cmd_061[]="HTTPHost";
prog_char PROGMEM Cmd_062[]="FileErase";
prog_char PROGMEM Cmd_063[]="FileShow";
prog_char PROGMEM Cmd_064[]="FileExecute";
prog_char PROGMEM Cmd_065[]="FileWrite";
prog_char PROGMEM Cmd_066[]="FileList";
prog_char PROGMEM Cmd_067[]="FileLog";
prog_char PROGMEM Cmd_068[]="FileGetHTTP";
prog_char PROGMEM Cmd_069[]="NodoIP";
prog_char PROGMEM Cmd_070[]="Gateway";
prog_char PROGMEM Cmd_071[]="Subnet";
prog_char PROGMEM Cmd_072[]="DnsServer";
prog_char PROGMEM Cmd_073[]="PortServer";
prog_char PROGMEM Cmd_074[]="PortClient";
prog_char PROGMEM Cmd_075[]="EventGhostServer";
prog_char PROGMEM Cmd_076[]="VariablePulse";
prog_char PROGMEM Cmd_077[]="";
prog_char PROGMEM Cmd_078[]="VariableWiredAnalog";
prog_char PROGMEM Cmd_079[]="Reboot";
prog_char PROGMEM Cmd_080[]="Echo";
prog_char PROGMEM Cmd_081[]="";
prog_char PROGMEM Cmd_082[]="";
prog_char PROGMEM Cmd_083[]="";
prog_char PROGMEM Cmd_084[]="";
prog_char PROGMEM Cmd_085[]="";
prog_char PROGMEM Cmd_086[]="";
prog_char PROGMEM Cmd_087[]="";
prog_char PROGMEM Cmd_088[]="";
prog_char PROGMEM Cmd_089[]="";
prog_char PROGMEM Cmd_090[]="";
prog_char PROGMEM Cmd_091[]="";
prog_char PROGMEM Cmd_092[]="";
prog_char PROGMEM Cmd_093[]="";
prog_char PROGMEM Cmd_094[]="";
prog_char PROGMEM Cmd_095[]="";
prog_char PROGMEM Cmd_096[]="";
prog_char PROGMEM Cmd_097[]="";
prog_char PROGMEM Cmd_098[]=""; 
prog_char PROGMEM Cmd_099[]=""; // TransmitQueue: Niet voor gebruiker bestemd.

// events:
#define RANGE_EVENT 100 // alle codes groter of gelijk aan deze waarde zijn events.
prog_char PROGMEM Cmd_100[]="UserEvent"; // deze moet altijd op 100 blijven anders opnieuw leren aan universele afstandsbediening!
prog_char PROGMEM Cmd_101[]="ClockDaylight";
prog_char PROGMEM Cmd_102[]="ClockAll";
prog_char PROGMEM Cmd_103[]="ClockSun";
prog_char PROGMEM Cmd_104[]="ClockMon";
prog_char PROGMEM Cmd_105[]="ClockTue";
prog_char PROGMEM Cmd_106[]="ClockWed";
prog_char PROGMEM Cmd_107[]="ClockThu";
prog_char PROGMEM Cmd_108[]="ClockFri";
prog_char PROGMEM Cmd_109[]="ClockSat";
prog_char PROGMEM Cmd_110[]="RawSignal";
prog_char PROGMEM Cmd_111[]="KAKU";
prog_char PROGMEM Cmd_112[]="NewKAKU";
prog_char PROGMEM Cmd_113[]="Timer";
prog_char PROGMEM Cmd_114[]="WiredIn";
prog_char PROGMEM Cmd_115[]="Variable";
prog_char PROGMEM Cmd_116[]="Busy";
prog_char PROGMEM Cmd_117[]="";
prog_char PROGMEM Cmd_118[]="Message";
prog_char PROGMEM Cmd_119[]="Boot";
prog_char PROGMEM Cmd_120[]="";
prog_char PROGMEM Cmd_121[]="";
prog_char PROGMEM Cmd_122[]="";
prog_char PROGMEM Cmd_123[]="";
prog_char PROGMEM Cmd_124[]="";
prog_char PROGMEM Cmd_125[]="";
prog_char PROGMEM Cmd_126[]="";
prog_char PROGMEM Cmd_127[]="";
prog_char PROGMEM Cmd_128[]="";
prog_char PROGMEM Cmd_129[]="";
prog_char PROGMEM Cmd_130[]="";
prog_char PROGMEM Cmd_131[]="";
prog_char PROGMEM Cmd_132[]="";
prog_char PROGMEM Cmd_133[]="";
prog_char PROGMEM Cmd_134[]="";
prog_char PROGMEM Cmd_135[]="";
prog_char PROGMEM Cmd_136[]="";
prog_char PROGMEM Cmd_137[]="";
prog_char PROGMEM Cmd_138[]="";
prog_char PROGMEM Cmd_139[]="";
prog_char PROGMEM Cmd_140[]="";
prog_char PROGMEM Cmd_141[]="";
prog_char PROGMEM Cmd_142[]="";
prog_char PROGMEM Cmd_143[]="";
prog_char PROGMEM Cmd_144[]="";
prog_char PROGMEM Cmd_145[]="";
prog_char PROGMEM Cmd_146[]="";
prog_char PROGMEM Cmd_147[]="";
prog_char PROGMEM Cmd_148[]="";
prog_char PROGMEM Cmd_149[]="";

// Waarden:
prog_char PROGMEM Cmd_150[]="Off";
prog_char PROGMEM Cmd_151[]="On";
prog_char PROGMEM Cmd_152[]="";
prog_char PROGMEM Cmd_153[]="";
prog_char PROGMEM Cmd_154[]="IR";
prog_char PROGMEM Cmd_155[]="HTTP";
prog_char PROGMEM Cmd_156[]="RF";
prog_char PROGMEM Cmd_157[]="Serial";
prog_char PROGMEM Cmd_158[]="Wired";
prog_char PROGMEM Cmd_159[]="EventList";
prog_char PROGMEM Cmd_160[]="System";
prog_char PROGMEM Cmd_161[]="Timers";
prog_char PROGMEM Cmd_162[]="Variables";
prog_char PROGMEM Cmd_163[]="Clock";
prog_char PROGMEM Cmd_164[]="Terminal";
prog_char PROGMEM Cmd_165[]="EventGhost";
prog_char PROGMEM Cmd_166[]="Status";
prog_char PROGMEM Cmd_167[]="File";
prog_char PROGMEM Cmd_168[]="Input";
prog_char PROGMEM Cmd_169[]="Output";
prog_char PROGMEM Cmd_170[]="Internal";
prog_char PROGMEM Cmd_171[]="Busy";
prog_char PROGMEM Cmd_172[]="Source";
prog_char PROGMEM Cmd_173[]="";
prog_char PROGMEM Cmd_174[]="";
prog_char PROGMEM Cmd_175[]="All";
prog_char PROGMEM Cmd_176[]="DaylightSaving";
prog_char PROGMEM Cmd_177[]="EventlistCount";
prog_char PROGMEM Cmd_178[]="Queue";
prog_char PROGMEM Cmd_179[]="Auto";
prog_char PROGMEM Cmd_180[]="Time";
prog_char PROGMEM Cmd_181[]="Count";
prog_char PROGMEM Cmd_182[]="";
prog_char PROGMEM Cmd_183[]="";
prog_char PROGMEM Cmd_184[]="";
prog_char PROGMEM Cmd_185[]="";
prog_char PROGMEM Cmd_186[]="";
prog_char PROGMEM Cmd_187[]="";
prog_char PROGMEM Cmd_188[]="";
prog_char PROGMEM Cmd_189[]="";
prog_char PROGMEM Cmd_190[]="";
prog_char PROGMEM Cmd_191[]="";
prog_char PROGMEM Cmd_192[]="";
prog_char PROGMEM Cmd_193[]="";
prog_char PROGMEM Cmd_194[]="";
prog_char PROGMEM Cmd_195[]="";
prog_char PROGMEM Cmd_196[]="";
prog_char PROGMEM Cmd_197[]="";
prog_char PROGMEM Cmd_198[]="";
prog_char PROGMEM Cmd_199[]="";
prog_char PROGMEM Cmd_200[]="Ok.";
prog_char PROGMEM Cmd_201[]="Error: Unknown command.";
prog_char PROGMEM Cmd_202[]="Error: Invalid parameter in command.";
prog_char PROGMEM Cmd_203[]="Error: Unable to open file on SDCard.";
prog_char PROGMEM Cmd_204[]="";
prog_char PROGMEM Cmd_205[]="Error: Eventlist nesting error.";
prog_char PROGMEM Cmd_206[]="Error: Reading/writing eventlist failed.";
prog_char PROGMEM Cmd_207[]="Error: Unable to establish connection.";
prog_char PROGMEM Cmd_208[]="Error: Incorrect password.";
prog_char PROGMEM Cmd_209[]="Error: Wireless access locked.";
prog_char PROGMEM Cmd_210[]="Error: Access not allowed.";
prog_char PROGMEM Cmd_211[]="Error: Sending/receiving EventGhost failed.";
prog_char PROGMEM Cmd_212[]="Error: SendTo failed.";
prog_char PROGMEM Cmd_213[]="Error: Timeout on busy Nodo.";
prog_char PROGMEM Cmd_214[]="Waiting for busy Nodo(s)...";
prog_char PROGMEM Cmd_215[]="";

// tabel die refereert aan de commando strings
PROGMEM const char *CommandText_tabel[]={
  Cmd_000,Cmd_001,Cmd_002,Cmd_003,Cmd_004,Cmd_005,Cmd_006,Cmd_007,Cmd_008,Cmd_009,
  Cmd_010,Cmd_011,Cmd_012,Cmd_013,Cmd_014,Cmd_015,Cmd_016,Cmd_017,Cmd_018,Cmd_019,
  Cmd_020,Cmd_021,Cmd_022,Cmd_023,Cmd_024,Cmd_025,Cmd_026,Cmd_027,Cmd_028,Cmd_029,
  Cmd_030,Cmd_031,Cmd_032,Cmd_033,Cmd_034,Cmd_035,Cmd_036,Cmd_037,Cmd_038,Cmd_039,
  Cmd_040,Cmd_041,Cmd_042,Cmd_043,Cmd_044,Cmd_045,Cmd_046,Cmd_047,Cmd_048,Cmd_049,
  Cmd_050,Cmd_051,Cmd_052,Cmd_053,Cmd_054,Cmd_055,Cmd_056,Cmd_057,Cmd_058,Cmd_059,
  Cmd_060,Cmd_061,Cmd_062,Cmd_063,Cmd_064,Cmd_065,Cmd_066,Cmd_067,Cmd_068,Cmd_069,
  Cmd_070,Cmd_071,Cmd_072,Cmd_073,Cmd_074,Cmd_075,Cmd_076,Cmd_077,Cmd_078,Cmd_079,          
  Cmd_080,Cmd_081,Cmd_082,Cmd_083,Cmd_084,Cmd_085,Cmd_086,Cmd_087,Cmd_088,Cmd_089,          
  Cmd_090,Cmd_091,Cmd_092,Cmd_093,Cmd_094,Cmd_095,Cmd_096,Cmd_097,Cmd_098,Cmd_099,          
  Cmd_100,Cmd_101,Cmd_102,Cmd_103,Cmd_104,Cmd_105,Cmd_106,Cmd_107,Cmd_108,Cmd_109,
  Cmd_110,Cmd_111,Cmd_112,Cmd_113,Cmd_114,Cmd_115,Cmd_116,Cmd_117,Cmd_118,Cmd_119,
  Cmd_120,Cmd_121,Cmd_122,Cmd_123,Cmd_124,Cmd_125,Cmd_126,Cmd_127,Cmd_128,Cmd_129,
  Cmd_130,Cmd_131,Cmd_132,Cmd_133,Cmd_134,Cmd_135,Cmd_136,Cmd_137,Cmd_138,Cmd_139,
  Cmd_140,Cmd_141,Cmd_142,Cmd_143,Cmd_144,Cmd_145,Cmd_146,Cmd_147,Cmd_148,Cmd_149,
  Cmd_150,Cmd_151,Cmd_152,Cmd_153,Cmd_154,Cmd_155,Cmd_156,Cmd_157,Cmd_158,Cmd_159,
  Cmd_160,Cmd_161,Cmd_162,Cmd_163,Cmd_164,Cmd_165,Cmd_166,Cmd_167,Cmd_168,Cmd_169,
  Cmd_170,Cmd_171,Cmd_172,Cmd_173,Cmd_174,Cmd_175,Cmd_176,Cmd_177,Cmd_178,Cmd_179,          
  Cmd_180,Cmd_181,Cmd_182,Cmd_183,Cmd_184,Cmd_185,Cmd_186,Cmd_187,Cmd_188,Cmd_189,          
  Cmd_190,Cmd_191,Cmd_192,Cmd_193,Cmd_194,Cmd_195,Cmd_196,Cmd_197,Cmd_198,Cmd_199,          
  Cmd_200,Cmd_201,Cmd_202,Cmd_203,Cmd_204,Cmd_205,Cmd_206,Cmd_207,Cmd_208,Cmd_209,          
  Cmd_210,Cmd_211,Cmd_212,Cmd_213,Cmd_214,Cmd_215           
  };          

#endif


// commando:
#define FIRST_COMMAND                    0 // Eerste COMMANDO uit de commando tabel
#define CMD_BREAK_ON_DAYLIGHT            1
#define CMD_BREAK_ON_VAR_EQU             2
#define CMD_BREAK_ON_VAR_LESS            3
#define CMD_BREAK_ON_VAR_MORE            4
#define CMD_BREAK_ON_VAR_NEQU            5
#define CMD_BREAK_ON_TIME_LATER          6
#define CMD_BREAK_ON_TIME_EARLIER        7
#define CMD_CLOCK_DATE                   8
#define CMD_CLOCK_YEAR                   9
#define CMD_CLOCK_TIME                  10
#define CMD_CLOCK_DOW                   11
#define CMD_EVENTLIST_ERASE             12
#define CMD_EVENTLIST_SHOW              13
#define CMD_EVENTLIST_WRITE             14
#define CMD_VARIABLE_SAVE               15
#define CMD_RAWSIGNAL_SAVE              16
#define CMD_RAWSIGNAL_SEND              17
#define CMD_RESET                       18
#define CMD_SEND_KAKU                   19
#define CMD_SEND_KAKU_NEW               20
#define CMD_DELAY                       21
#define CMD_SEND                        22
#define CMD_SIMULATE_DAY                23
#define CMD_SOUND                       24
#define CMD_DEBUG                       25
#define CMD_USERPLUGIN                  26
#define CMD_TIMER_RANDOM                27
#define CMD_TIMER_SET_SEC               28
#define CMD_TIMER_SET_MIN               29
#define CMD_ID                          30
#define CMD_UNIT                        31
#define CMD_WAITBUSY                    32
#define CMD_VARIABLE_DEC                33
#define CMD_VARIABLE_INC                34
#define CMD_VARIABLE_SET                35
#define CMD_VARIABLE_VARIABLE           36
#define CMD_SEND_EVENT                  37
#define CMD_WAITFREERF                  38
#define CMD_WIRED_ANALOG                39
#define CMD_WIRED_OUT                   40
#define CMD_WIRED_PULLUP                41
#define CMD_WIRED_SMITTTRIGGER          42
#define CMD_WIRED_THRESHOLD             43
#define CMD_SEND_USEREVENT              44
#define CMD_RAWSIGNAL_COPY              45
#define CMD_COMMAND_WILDCARD            46
#define CMD_SENDBUSY                    47
#define CMD_CLIENT_IP                   48
#define CMD_PASSWORD                    49
#define CMD_EVENTLIST_FILE              50
#define CMD_WIRED_ANALOG_CALIBRATE_HIGH 51
#define CMD_WIRED_ANALOG_CALIBRATE_LOW  52
#define CMD_LOCK                        53
#define CMD_STATUS                      54
#define CMD_RES55                       55
#define CMD_ANALYSE_SETTINGS            56
#define CMD_TRANSMIT_IP                 57
#define CMD_TRANSMIT_IR                 58
#define CMD_TRANSMIT_RF                 59
#define CMD_RECEIVE_SETTINGS            60
#define CMD_HTTP_REQUEST                61
#define CMD_FILE_ERASE                  62
#define CMD_FILE_SHOW                   63
#define CMD_FILE_EXECUTE                64
#define CMD_FILE_WRITE                  65
#define CMD_FILE_LIST                   66
#define CMD_FILE_LOG                    67
#define CMD_FILE_GET_HTTP               68
#define CMD_NODO_IP                     69
#define CMD_GATEWAY                     70
#define CMD_SUBNET                      71
#define CMD_DNS_SERVER                  72
#define CMD_PORT_SERVER                 73
#define CMD_PORT_CLIENT                 74
#define CMD_EVENTGHOST_SERVER           75
#define CMD_PULSE_VARIABLE              76
#define CMD_RES77                       77
#define CMD_VARIABLE_WIREDANALOG        78
#define CMD_REBOOT                      79
#define CMD_ECHO                        80
#define CMD_RES081                      81
#define CMD_RES082                      82
#define CMD_RES083                      83
#define CMD_RES084                      84
#define CMD_RES085                      85
#define CMD_RES086                      86
#define CMD_RES087                      87
#define CMD_RES088                      88
#define CMD_RES089                      89
#define CMD_RES090                      90
#define CMD_RES091                      91
#define CMD_RES092                      92
#define CMD_RES093                      93
#define CMD_RES094                      94
#define CMD_RES095                      95
#define CMD_RES096                      96
#define CMD_RES097                      97
#define CMD_RES098                      98
#define CMD_TRANSMIT_QUEUE              99 //
#define LAST_COMMAND                    99 // Laatste COMMANDO uit de commando tabel

// events:
#define FIRST_EVENT                    100 // eerste EVENT uit de commando tabel
#define CMD_USEREVENT                  100 // deze moet altijd op 100 blijven anders opnieuw leren aan universele afstandsbediening!
#define CMD_CLOCK_EVENT_DAYLIGHT       101
#define CMD_CLOCK_EVENT_ALL            102
#define CMD_CLOCK_EVENT_SUN            103
#define CMD_CLOCK_EVENT_MON            104
#define CMD_CLOCK_EVENT_TUE            105
#define CMD_CLOCK_EVENT_WED            106
#define CMD_CLOCK_EVENT_THU            107
#define CMD_CLOCK_EVENT_FRI            108
#define CMD_CLOCK_EVENT_SAT            109
#define CMD_RAWSIGNAL                  110
#define CMD_KAKU                       111
#define CMD_KAKU_NEW                   112
#define CMD_TIMER_EVENT                113
#define CMD_WIRED_IN_EVENT             114
#define CMD_VARIABLE_EVENT             115
#define CMD_BUSY                       116
#define CMD_RES117                     117
#define CMD_MESSAGE                    118
#define CMD_BOOT_EVENT                 119
#define CMD_RES_EVENT_120              120
#define CMD_RES_EVENT_121              121
#define CMD_RES_EVENT_122              122
#define CMD_RES_EVENT_123              123
#define CMD_RES_EVENT_124              124
#define CMD_RES_EVENT_125              125
#define CMD_RES_EVENT_126              126
#define CMD_RES_EVENT_127              127
#define CMD_RES_EVENT_128              128
#define CMD_RES_EVENT_129              129
#define CMD_RES_EVENT_130              130
#define CMD_RES_EVENT_131              131
#define CMD_RES_EVENT_132              132
#define CMD_RES_EVENT_133              133
#define CMD_RES_EVENT_134              134
#define CMD_RES_EVENT_135              135
#define CMD_RES_EVENT_136              136
#define CMD_RES_EVENT_137              137
#define CMD_RES_EVENT_138              138
#define CMD_RES_EVENT_139              139
#define CMD_RES_EVENT_140              140
#define CMD_RES_EVENT_141              141
#define CMD_RES_EVENT_142              142
#define CMD_RES_EVENT_143              143
#define CMD_RES_EVENT_144              144
#define CMD_RES_EVENT_145              145
#define CMD_RES_EVENT_146              146
#define CMD_RES_EVENT_147              147
#define CMD_RES_EVENT_148              148
#define CMD_RES_EVENT_149              149
#define LAST_EVENT                     149 // Laatste EVENT uit de commando tabel
                                        
// Waarden:
#define FIRST_VALUE                    150  // eerste VALUE uit de commando tabel
#define VALUE_OFF                      150 
#define VALUE_ON                       151 // Deze waarde MOET groter dan 16 zijn.
#define VALUE_RES152                   152
#define VALUE_RES153                   153
#define VALUE_SOURCE_IR                154
#define VALUE_SOURCE_HTTP              155
#define VALUE_SOURCE_RF                156
#define VALUE_SOURCE_SERIAL            157
#define VALUE_SOURCE_WIRED             158
#define VALUE_SOURCE_EVENTLIST         159
#define VALUE_SOURCE_SYSTEM            160
#define VALUE_SOURCE_TIMER             161
#define VALUE_SOURCE_VARIABLE          162
#define VALUE_SOURCE_CLOCK             163
#define VALUE_SOURCE_TELNET            164
#define VALUE_SOURCE_EVENTGHOST        165
#define VALUE_SOURCE_STATUS            166
#define VALUE_SOURCE_FILE              167
#define VALUE_DIRECTION_INPUT          168
#define VALUE_DIRECTION_OUTPUT         169
#define VALUE_DIRECTION_INTERNAL       170
#define VALUE_BUSY                     171
#define VALUE_SOURCE                   172
#define VALUE_RES173                   173
#define VALUE_RES174                   174
#define VALUE_ALL                      175 // Deze waarde MOET groter dan 16 zijn.
#define VALUE_DLS                      176
#define VALUE_EVENTLIST_COUNT          177
#define VALUE_SOURCE_QUEUE             178
#define VALUE_AUTO                     179
#define VALUE_TIME                     180
#define VALUE_COUNT                    181
#define VALUE_RES182                   182
#define VALUE_RES183                   183
#define VALUE_RES184                   184
#define VALUE_RES185                   185
#define VALUE_RES186                   186
#define VALUE_RES187                   187
#define VALUE_RES188                   188
#define VALUE_RES189                   189
#define VALUE_RES190                   190
#define VALUE_RES191                   191
#define VALUE_RES192                   192
#define VALUE_RES193                   193
#define VALUE_RES194                   194
#define VALUE_RES195                   195
#define VALUE_RES196                   196
#define VALUE_RES197                   197
#define VALUE_RES198                   198
#define VALUE_RES199                   199
#define MESSAGE_00                     200
#define MESSAGE_01                     201
#define MESSAGE_02                     202
#define MESSAGE_03                     203
#define MESSAGE_04                     204
#define MESSAGE_05                     205
#define MESSAGE_06                     206
#define MESSAGE_07                     207
#define MESSAGE_08                     208
#define MESSAGE_09                     209
#define MESSAGE_10                     210
#define MESSAGE_11                     211
#define MESSAGE_12                     212
#define MESSAGE_13                     213
#define MESSAGE_14                     214
#define MESSAGE_15                     215
#define LAST_VALUE                     215 // laatste VALUE uit de commando tabel
#define COMMAND_MAX                    215 // hoogste commando


// Tabel met zonsopgang en -ondergang momenten. afgeleid van KNMI gegevens midden Nederland.
PROGMEM prog_uint16_t Sunrise[]={528,525,516,503,487,467,446,424,401,378,355,333,313,295,279,268,261,259,263,271,283,297,312,329,345,367,377,394,411,428,446,464,481,498,512,522,528,527};
PROGMEM prog_uint16_t Sunset[]={999,1010,1026,1044,1062,1081,1099,1117,1135,1152,1169,1186,1203,1219,1235,1248,1258,1263,1264,1259,1249,1235,1218,1198,1177,1154,1131,1107,1084,1062,1041,1023,1008,996,990,989,993,1004};

// omschakeling zomertijd / wintertijd voor komende 10 jaar. één int bevat de omschakeldatum van maart en oktober.
#define DLSBase 2010 // jaar van eerste element uit de array
PROGMEM prog_uint16_t DLSDate[]={2831,2730,2528,3127,3026,2925,2730,2629,2528,3127};

#define BLOK_REPEAT_TIME          1000  // Tijd waarbinnen hetzelfde event niet nogmaals via RF of IR mag binnenkomen. Onderdrukt ongewenste herhalingen van signaal
#define NODO_PULSE_0               500  // PWM: Tijdsduur van de puls bij verzenden van een '0' in uSec.
#define NODO_PULSE_MID            1000  // PWM: Pulsen langer zijn '1'
#define NODO_PULSE_1              1500  // PWM: Tijdsduur van de puls bij verzenden van een '1' in uSec. (3x NODO_PULSE_0)
#define NODO_SPACE                 500  // PWM: Tijdsduur van de space tussen de bitspuls bij verzenden van een '1' in uSec.   
#define RED                          1  // Led = Rood
#define GREEN                        2  // Led = Groen
#define BLUE                         3  // Led = Blauw
#define BAUD                     19200 // Baudrate voor seriële communicatie.
#define UNIT_MAX                    15 // Hoogst mogelijke unit nummer van een Nodo
#define SERIAL_TERMINATOR_1       0x0A // Met dit teken wordt een regel afgesloten. 0x0A is een linefeed <LF>, default voor EventGhost
#define SERIAL_TERMINATOR_2       0x00 // Met dit teken wordt een regel afgesloten. 0x0D is een Carriage Return <CR>, 0x00 = niet in gebruik.
#define Loop_INTERVAL_1              5 // tijdsinterval in ms. voor achtergrondtaken snelle verwerking
#define Loop_INTERVAL_2            100 // tijdsinterval in ms. voor achtergrondtaken langzame verwerking
#define Loop_INTERVAL_3           1000 // tijdsinterval in ms. voor achtergrondtaken langzame verwerking
#define ENDSIGNAL_TIME            1500 // Dit is de tijd in milliseconden waarna wordt aangenomen dat het ontvangen één reeks signalen beëindigd is
#define SIGNAL_TIMEOUT_RF         5000 // na deze tijd in uSec. wordt één RF signaal als beëindigd beschouwd.
#define SIGNAL_TIMEOUT_IR        10000 // na deze tijd in uSec. wordt één IR signaal als beëindigd beschouwd.
#define TX_REPEATS                   5 // aantal herhalingen van een code binnen één RF of IR reeks
#define MIN_PULSE_LENGTH           100 // pulsen korter dan deze tijd uSec. worden als stoorpulsen beschouwd.
#define MIN_RAW_PULSES              16 // =8 bits. Minimaal aantal ontvangen bits*2 alvorens cpu tijd wordt besteed aan decodering, etc. Zet zo hoog mogelijk om CPU-tijd te sparen en minder 'onzin' te ontvangen.
#define SHARP_TIME                 500 // tijd in milliseconden dat de nodo gefocust moet blijven luisteren naar één dezelfde poort na binnenkomst van een signaal
#define RECEIVER_STABLE            750 // Tijd die de RF ontvanger nodig heeft om na inschakelen voedspanning signalen op te kunnen vangen.
#define SIGNAL_TYPE_UNKNOWN          0 // Type ontvangen of te verzenden signaal in de eventcode
#define SIGNAL_TYPE_NODO             1 // Type ontvangen of te verzenden signaal in de eventcode
#define SIGNAL_TYPE_KAKU             2 // Type ontvangen of te verzenden signaal in de eventcode
#define SIGNAL_TYPE_NEWKAKU          3 // Type ontvangen of te verzenden signaal in de eventcode
#define NODO_TYPE_EVENT              1
#define NODO_TYPE_COMMAND            2

// Hardware in gebruik
#define HW_BOARD_UNO    0
#define HW_BOARD_MEGA   1
#define HW_ETHERNET     2
#define HW_SDCARD       3
#define HW_SERIAL       4
#define HW_CLOCK        5
#define HW_RF_RX        6
#define HW_IR_RX        7

#if NODO_MEGA // Definities voor de Nodo-Mega variant.
#define MACRO_EXECUTION_DEPTH       10 // maximale nesting van macro's.
#define INPUT_BUFFER_SIZE          128  // Buffer waar de karakters van de seriele/IP poort in worden opgeslagen.
#define TIMER_MAX                   15  // aantal beschikbare timers voor de user, gerekend vanaf 1
#define USER_VARIABLES_MAX          15  // aantal beschikbare gebruikersvariabelen voor de user.
#define EVENT_QUEUE_MAX             32  // maximaal aantal plaatsen in de queue
#define PULSE_IRQ                    5  // IRQ verbonden aan de IR_RX_DATA pen 18 van de Mega
#define PIN_WIRED_IN_1               8  // NIET VERANDEREN. Analoge inputs A8 t/m A15 worden gebruikt voor WiredIn 1 tot en met 8
#define PIN_LED_RGB_R               47  // RGB-Led, aansluiting rood
#define PIN_LED_RGB_G               48  // RGB-Led, aansluiting groen
#define PIN_LED_RGB_B               49  // RGB-Led, aansluiting blauw
#define PIN_SPEAKER                 42  // luidspreker aansluiting
#define PIN_IR_TX_DATA              17  // NIET VERANDEREN. Aan deze pin zit een zender IR-Led. (gebufferd via transistor i.v.m. hogere stroom die nodig is voor IR-led)
#define PIN_IR_RX_DATA              18  // Op deze input komt het IR signaal binnen van de TSOP. Bij HIGH bij geen signaal.
#define PIN_RF_TX_VCC               15  // +5 volt / Vcc spanning naar de zender.
#define PIN_RF_TX_DATA              14  // data naar de zender
#define PIN_RF_RX_VCC               16  // Spanning naar de ontvanger via deze pin.
#define PIN_RF_RX_DATA              19  // Op deze input komt het 433Mhz-RF signaal binnen. LOW bij geen signaal.
#define PIN_WIRED_OUT_1             30  // 8 digitale outputs D30 t/m D37 worden gebruikt voor WiredIn 1 tot en met 8
#define EthernetShield_SCK          52  // NIET VERANDEREN. Ethernet shield: SCK-lijn van de ethernet kaart
#define EthernetShield_CS_SDCardH   53  // NIET VERANDEREN. Ethernet shield: Gereserveerd voor correct funktioneren van de SDCard: Hardware CS/SPI ChipSelect
#define EthernetShield_CS_SDCard     4  // NIET VERANDEREN. Ethernet shield: Chipselect van de SDCard. Niet gebruiken voor andere doeleinden
#define Ethernetshield_CS_W5100     10  // NIET VERANDEREN. Ethernet shield: D10..D13  // gereserveerd voor Ethernet & SDCard
#define Ethernet_shield_CS_SDCard    4  // NIET VERANDEREN. Ethernet shield: Chipselect van de SDCard. Niet gebruiken voor andere doeleinden
#define Ethernet_shield_CS_W5100    10  // NIET VERANDEREN. Ethernet shield: D10..D13  // gereserveerd voor Ethernet & SDCard
#define PIN_CLOCK_SDA               20  // I2C communicatie lijn voor de realtime clock.
#define PIN_CLOCK_SLC               21  // I2C communicatie lijn voor de realtime clock.
#define RAW_BUFFER_SIZE            256  // Maximaal aantal te ontvangen 128 bits.
#define TERMINAL_PORT               23
#define EVENTLIST_MAX              250 // aantal events dat de lijst bevat in het EEPROM geheugen. Iedere regel in de eventlist heeft 8 bytes nodig. eerste adres is 0
#define WIRED_PORTS                  8 // aantal WiredIn/WiredOut poorten
#define PASSWORD_MAX_RETRY           5 // aantal keren dat een gebruiker een foutief wachtwoord mag ingeven alvorens tijdslot in werking treedt
#define PASSWORD_TIMEOUT           300 // aantal seconden dat het terminal venster is geblokkeerd na foutive wachtwoord
#define TERMINAL_TIMEOUT           300 // Aantal seconden dat, na de laatst ontvangen regel, de terminalverbinding open mag staan.
#define COOKIE_REFRESH_TIME         60 // Tijd tussen automatisch verzenden van een nieuw Cookie als de beveiligde HTTP modus is inschakeld.

#else // als het voor de Nodo-Mini variant is
#define MACRO_EXECUTION_DEPTH        4 // maximale nesting van macro's.
#define TIMER_MAX                    8 // aantal beschikbare timers voor de user, gerekend vanaf 1
#define USER_VARIABLES_MAX           8 // aantal beschikbare gebruikersvariabelen voor de user.
#define EVENT_QUEUE_MAX             16 // maximaal aantal plaatsen in de queue
#define PULSE_IRQ                    3 // IRQ verbonden aan de IR_RX_DATA pen 3 van de ATMega328 (Uno/Nano/Duemillanove)
#define RAW_BUFFER_SIZE            160 // Maximaal aantal te ontvangen 128 bits.
#define EVENTLIST_MAX              100 // aantal events dat de lijst bevat in het EEPROM geheugen. Iedere regel in de eventlist heeft 8 bytes nodig. eerste adres is 0
#define WIRED_PORTS                  4 // aantal WiredIn/WiredOut poorten
#define PIN_LED_RGB_R               13 // RGB-Led, aansluiting rood
#define PIN_LED_RGB_B               13 // RGB-Led, aansluiting blauw, maar voor de Nodo mini is dit de eveneens de rode led.
#define PIN_WIRED_IN_1               0 // Eerste WIRED input pin. Wired-IN loopt van A0 tot en met 3
#define PIN_SPEAKER                  6 // luidspreker aansluiting
#define PIN_IR_TX_DATA              11 // NIET VERANDEREN. Aan deze pin zit een zender IR-Led. (gebufferd via transistor i.v.m. hogere stroom die nodig is voor IR-led)
#define PIN_IR_RX_DATA               3 // Op deze input komt het IR signaal binnen van de TSOP. Bij HIGH bij geen signaal.
#define PIN_RF_TX_VCC                4 // +5 volt / Vcc spanning naar de zender.
#define PIN_RF_TX_DATA               5 // data naar de zender
#define PIN_RF_RX_DATA               2 // Op deze input komt het 433Mhz-RF signaal binnen. LOW bij geen signaal.
#define PIN_RF_RX_VCC               12 // Spanning naar de ontvanger via deze pin.
#define PIN_WIRED_OUT_1              7 // 7 digitale outputs D07 t/m D10 worden gebruikt voor WiredIn 1 tot en met 4

#endif

//****************************************************************************************************************************************

struct SettingsStruct
  {
  int     Version;        
  int     WiredInputThreshold[WIRED_PORTS], WiredInputSmittTrigger[WIRED_PORTS];
  int     WiredInput_Calibration_IH[WIRED_PORTS], WiredInput_Calibration_IL[WIRED_PORTS];
  int     WiredInput_Calibration_OH[WIRED_PORTS], WiredInput_Calibration_OL[WIRED_PORTS];
  byte    WiredInputPullUp[WIRED_PORTS];
  byte    AnalyseSharpness;
  unsigned int AnalyseTimeOut;
  int     UserVar[USER_VARIABLES_MAX];
  byte    Unit;
  byte    TransmitIR;
  byte    TransmitRepeatIR;
  byte    TransmitRF;
  byte    TransmitRepeatRF;
  byte    TransmitIP;                                       // Definitie van het gebruik van de IP-poort: Off, EventGhost of HTTP
  byte    WaitFreeRF;
  byte    SendBusy;
  byte    WaitBusyAll;                                      // maximale tijd dat gewacht moet worden op Nodos die bezig zijn met verwerking
  boolean DaylightSaving;                                   // Vlag die aangeeft of het zomertijd of wintertijd is
  int     DaylightSavingSet;                                // Vlag voor correct automatisch kunnen overschakelen van zomertijd naar wintertijd of vice-versa
  int     Lock;                                             // bevat de pincode waarmee IR/RF ontvangst is geblokkeerd. Bit nummer hoogste bit wordt gebruiktvoor in/uitschakelen.
  byte    Debug;                                            // Weergeven van extra gegevens t.b.v. beter inzicht verloop van de verwerking
  
#if NODO_MEGA
  char    Password[25];                                     // String met wachtwoord.
  char    ID[10];                                           // code waar de Nodo uniek mee geïdentificeerd kan worden in een netwerk
  char    HTTPRequest[80];                                  // HTTP request;
  byte    EventGhostServer_IP[4];                           // IP adres van waar EventGhost Events naar verstuurd moeten worden.
  byte    AutoSaveEventGhostIP;                             // Automatisch IP adres opslaan na ontvangst van een EG event of niet.
  byte    Nodo_IP[4];                                       // IP adres van van de Nodo. als 0.0.0.0 ingevuld, dan IP toekenning o.b.v. DHCP
  byte    Client_IP[4];                                     // IP adres van van de Client die verbinding wil maken met de Nodo, 
  byte    Subnet[4];                                        // Submask
  byte    Gateway[4];                                       // Gateway
  byte    DnsServer[4];                                     // DNS Server IP adres
  int     PortServer;                                       // Poort van de inkomende IP communnicatie
  int     PortClient;                                       // Poort van de uitgaande IP communnicatie
  byte    HTTP_Pin;                                         // Als deze VALUE_ON bevat worden events tussen WebApp en Nodo alleen uitgewisseld bij juiste key in HTTP reques.
  byte    EchoSerial;
  byte    EchoTelnet;
#endif
  }Settings;

struct NodoBusyStruct //@2
  {
  int Status;                                               // in deze variabele de status van het event 'Busy' van de betreffende units 1 t/m 15. bit-1 = unit-1.
  boolean Sent;                                             // Vlag die bijhoudt of het Busy On event is verzonden.
  int ResetTimer;                                           // Timer voor resetten van de status.
  }Busy;

struct QueueStruct
  {
  byte Port[EVENT_QUEUE_MAX];                               // tabel behorend bij de queue. Geeft herkomst van het event in de queue aan.
  unsigned long Event[EVENT_QUEUE_MAX];                     // queue voor tijdelijk onthouden van events die tijdens een delay functie voorbij komen.
  byte Position;                                                 // teller die wijst naar een plaats in de queue.
  }Queue;
  
unsigned long PulseCount;                                   // Pulsenteller van de IR puls. Iedere hoog naar laag transitie wordt deze teller met één verhoogd
unsigned long PulseTime;                                    // Tijdsduur tussen twee pulsen teller in milliseconden: millis()-vorige meting.
unsigned long PulseTimePrevious;                            // Tijdsduur tussen twee pulsen teller in milliseconden: vorige meting
unsigned long UserTimer[TIMER_MAX];                         // Timers voor de gebruiker.
boolean WiredInputStatus[WIRED_PORTS];                      // Status van de WiredIn worden hierin opgeslagen.
boolean WiredOutputStatus[WIRED_PORTS];                     // Wired variabelen.
byte DaylightPrevious;                                      // t.b.v. voorkomen herhaald genereren van events binnen de lopende minuut waar dit event zich voordoet.
byte ExecutionDepth=0;                                      // teller die bijhoudt hoe vaak er binnen een macro weer een macro wordt uitgevoerd. Voorkomt tevens vastlopers a.g.v. loops die door een gebruiker zijn gemaakt met macro's.
void(*Reset)(void)=0;                                       // reset functie op adres 0.
uint8_t RFbit,RFport,IRbit,IRport;                          // t.b.v. verwerking IR/FR signalen.
int UserVar[USER_VARIABLES_MAX];                            // Gebruikers variabelen
unsigned long HW_Config=0;                                  // Hardware configuratie zoals gedetecteerd door de Nodo. 

#if NODO_MEGA
uint8_t MD5HashCode[16];                                    // tabel voor berekenen van MD5 hash codes t.b.v. uitwisselen wachtwoord EventGhost.
int CookieTimer;                                            // Seconden teller die bijhoudt wanneer er weer een nieuw Cookie naar de WebApp verzonden moet worden.
int TerminalConnected=0;                                    // Vlag geeft aan of en hoe lang nog (seconden) er verbinding is met een Terminal.
boolean TemporyEventGhostError=false;                       // Vlag om tijdelijk evetghost verzending stil te leggen na een communicatie probleem
int TerminalLocked=1;                                       // 0 als als gebruiker van een telnet terminalsessie juiste wachtwoord heeft ingetoetst
char TempLogFile[13];                                       // Naam van de Logfile waar (naast de standaard logging) de verwerking in gelogd moet worden.
char HTTPCookie[10];                                        // Cookie voor uitwisselen van encrypted events via HTTP
int FileWriteMode=0;                                        // Het aantal seconden dat deze timer ingesteld staat zal er geen verwerking plaats vinden van TerminalInvoer. Iedere seconde --.
char InputBuffer_Serial[INPUT_BUFFER_SIZE+2];               // Buffer voor input Seriele data
char InputBuffer_Terminal[INPUT_BUFFER_SIZE+2];             // Buffer voor input terminal verbinding Telnes sessie

// ethernet classes voor IP communicatie EventGhost, Telnet terminal en HTTP.
byte Ethernet_MAC_Address[]={NODO_MAC};// MAC adres van de Nodo.
EthernetServer IPServer(80);                                // Server class voor HTTP sessie.
EthernetServer TerminalServer(23);                          // Server class voor Terminal sessie.
EthernetClient TerminalClient;                              // Client class voor Terminal sessie.
byte ClientIPAddress[4];                                    // IP adres van de EventGhost client die verbinding probeert te maken c.q. verbonden is.
#endif

// RealTimeclock DS1307
struct RealTimeClock {byte Hour,Minutes,Seconds,Date,Month,Day,Daylight; int Year,DaylightSaving;}Time; // Hier wordt datum & tijd in opgeslagen afkomstig van de RTC.

struct RawsignalStruct
  {
  unsigned int Pulses[RAW_BUFFER_SIZE+2];                   // Tabel met de gemeten pulsen in microseconden. eerste waarde [0] wordt NIET gebruikt. (legacy redenen).
  byte Source;                                              // Bron waar het signaal op is binnengekomen.
  int Number;                                               // aantal bits, maal twee omdat iedere bit een pulse en een space heeft.
  int Key;                                                 // sleutel waaronder de pulsenreeks op SDCard opgeslgen moet worden.
  byte Type;                                                // Type signaal dan ontvangen is.
  }RawSignal;
  
void setup() 
  {    
  byte x;

  // Initialiseer in/output poorten.
  pinMode(PIN_IR_RX_DATA, INPUT);
  pinMode(PIN_RF_RX_DATA, INPUT);
  pinMode(PIN_RF_TX_DATA, OUTPUT);
  pinMode(PIN_RF_TX_VCC,  OUTPUT);
  pinMode(PIN_RF_RX_VCC,  OUTPUT);
  pinMode(PIN_IR_TX_DATA, OUTPUT);
  pinMode(PIN_LED_RGB_R,  OUTPUT);
  pinMode(PIN_SPEAKER,    OUTPUT);
  pinMode(PIN_LED_RGB_B,  OUTPUT);
  digitalWrite(PIN_IR_TX_DATA,LOW);   // Zet de IR zenders initiëel uit! Anders mogelijk overbelasting !
  digitalWrite(PIN_IR_RX_DATA,HIGH);  // schakel pull-up weerstand in om te voorkomen dat er rommel binnenkomt als pin niet aangesloten.
  digitalWrite(PIN_RF_RX_DATA,HIGH);  // schakel pull-up weerstand in om te voorkomen dat er rommel binnenkomt als pin niet aangesloten.
  digitalWrite(PIN_RF_RX_VCC,HIGH);   // Spanning naar de RF ontvanger aan

#if NODO_MEGA
  pinMode(PIN_LED_RGB_G,  OUTPUT);
  pinMode(EthernetShield_CS_SDCardH, OUTPUT); // CS/SPI: nodig voor correct funktioneren van de SDCard!
#endif

  RFbit=digitalPinToBitMask(PIN_RF_RX_DATA);
  RFport=digitalPinToPort(PIN_RF_RX_DATA);  
  IRbit=digitalPinToBitMask(PIN_IR_RX_DATA);
  IRport=digitalPinToPort(PIN_IR_RX_DATA);

  Led(BLUE);

  bitWrite(HW_Config,HW_BOARD_MEGA,NODO_MEGA);
  Wire.begin();        // zet I2C communicatie gereed voor uitlezen van de realtime clock.
  Serial.begin(BAUD);  // Initialiseer de seriële poort

#if NODO_MEGA
  Serial.println("Booting...");
  SerialHold(true);// XOFF verzenden zodat PC even wacht met versturen van data via Serial (Xon/Xoff-handshaking)
#endif

  // initialiseer de Busy Nodo gegevens
  Busy.Status=0;
  Busy.Sent=false;
  Busy.ResetTimer=0;

  LoadSettings();      // laad alle settings zoals deze in de EEPROM zijn opgeslagen
  if(Settings.Version!=SETTINGS_VERSION)ResetFactory(); // Als versienummer in EEPROM niet correct is, dan een ResetFactory.

  // initialiseer de Wired ingangen.
  for(x=0;x<WIRED_PORTS;x++)
    {
    if(Settings.WiredInputPullUp[x]==VALUE_ON)
      pinMode(A0+PIN_WIRED_IN_1+x,INPUT_PULLUP);
    else
      pinMode(A0+PIN_WIRED_IN_1+x,INPUT);

    pinMode(PIN_WIRED_OUT_1+x,OUTPUT); // definieer Arduino pin's voor Wired-Out
    }


  //Zorg ervoor dat er niet direct na een boot een CMD_CLOCK_DAYLIGHT event optreedt
  ClockRead();
  SetDaylight();
  DaylightPrevious=Time.Daylight;
  randomSeed(Time.Seconds);

  // Zet statussen WIRED_IN op hoog, anders wordt direct wij het opstarten meerdere malen een event gegenereerd omdat de pull-up weerstand analoge de waarden op hoog zet
  for(x=0;x<WIRED_PORTS;x++){WiredInputStatus[x]=true;}

#if NODO_MEGA
  // SDCard initialiseren:
  // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer SDCard chip
  SelectSD(true);
  if(SD.begin(EthernetShield_CS_SDCard))
    {
    SD.mkdir(ProgmemString(Text_27)); // maak drectory aan waar de Rawsignal HEX bestanden in worden opgeslagen
    SD.mkdir(ProgmemString(Text_28)); // maak drectory aan waar de Rawsignal KEY bestanden in worden opgeslagen
    SD.remove(ProgmemString(Text_15)); // eventueel queue wissen. 
    bitWrite(HW_Config,HW_SDCARD,1);
    }

  #if TRACE
  Trace(0,0,0);
  #endif
   
  // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer W5100 chip
  SelectSD(false);
  bitWrite(HW_Config,HW_ETHERNET,ETHERNET);//??? nog slim detecteren

  // Initialiseer ethernet device
  if(bitRead(HW_Config,HW_ETHERNET))
    {
    IPServer = EthernetServer(Settings.PortServer);
    TerminalServer = EthernetServer(23);
  
    if((Settings.Nodo_IP[0] + Settings.Nodo_IP[1] + Settings.Nodo_IP[2] + Settings.Nodo_IP[3])==0)// Als door de user IP adres is ingesteld op 0.0.0.0 dan IP adres ophalen via DHCP
      {
      if(Ethernet.begin(Ethernet_MAC_Address)==0) // maak verbinding en verzoek IP via DHCP
        {
        Serial.println(F("Error: Failed to configure Ethernet using DHCP"));
        bitWrite(HW_Config,HW_ETHERNET,0);
        }
      }
    else
      Ethernet.begin(Ethernet_MAC_Address, Settings.Nodo_IP, Settings.DnsServer, Settings.Gateway, Settings.Subnet);
  
    bitWrite(HW_Config,HW_ETHERNET,((Ethernet.localIP()[0]+Ethernet.localIP()[1]+Ethernet.localIP()[2]+Ethernet.localIP()[3])!=0)); // Als er een IP adres is, dan Ethernet inschakelen
    IPServer.begin(); // Start Server voor ontvangst van Events
    TerminalServer.begin(); // Start server voor Terminalsessies via TelNet

    // Als ethernet enbled en beveiligde modus, dan direct een Cookie sturen, ander worden eerste events niet opgepikt door de WebApp
    if(Settings.HTTP_Pin==VALUE_ON)
      SendHTTPCookie(); // Verzend een nieuw cookie
    }

  RawSignal.Key=-1; // Als deze variable ongelijk aan -1 dan wordt er een Rawsignal opgeslagen.  
  bitWrite(HW_Config,HW_SERIAL,1); // zonder deze vlag vindt er geen output naar de serial poort plaats. Tijdelijk even inschakelen.

#endif

  PrintWelcome(); // geef de welkomsttekst weer
  #if USER_PLUGIN
    UserPlugin_Init();
  #endif
  TransmitCode(command2event(Settings.Unit, CMD_BOOT_EVENT,Settings.Unit,0),VALUE_ALL);  
  ProcessEvent(command2event(Settings.Unit, CMD_BOOT_EVENT,Settings.Unit,0),VALUE_DIRECTION_INTERNAL,VALUE_SOURCE_SYSTEM,0,0);  // Voer het 'Boot' event uit.
  bitWrite(HW_Config,HW_SERIAL,0); // Serial weer uitschakelen.
  }


void loop() 
  {
  int x,y,z;
  byte SerialInByte;
  byte TerminalInByte;
  int SerialInByteCounter=0;
  int TerminalInbyteCounter=0;
  byte Slice_1,Slice_2;    

  unsigned long LoopIntervalTimer_1=millis();                 // Timer voor periodieke verwerking. millis() maakt dat de intervallen van 1 en 2 niet op zelfde moment vallen => 1 en 2 nu asynchroon.
  unsigned long LoopIntervalTimer_2=0L;                       // Timer voor periodieke verwerking.
  unsigned long LoopIntervalTimer_3=0L;                       // Timer voor periodieke verwerking.
  unsigned long Content=0L;                                   // Ontvangen event van RF, IR, ... Tevens buffer voor het vorige ontvangen Event
  unsigned long EventTimeCodePrevious; 
  unsigned long StaySharpTimer;                               // Timer die er voor zorgt dat bij communicatie via een kanaal de focus hier ook enige tijd op blijft

#if NODO_MEGA
  SerialHold(false); // er mogen weer tekens binnen komen van SERIAL
  InputBuffer_Serial[0]=0; // serieel buffer string leeg maken
  TempLogFile[0]=0; // geen extra logging
#endif

  // hoofdloop: scannen naar signalen
  // dit is een tijdkritische loop die wacht tot binnengekomen event op IR, RF, SERIAL, CLOCK, DAYLIGHT, TIMER, etc
  // Er is bewist niet gekozen voor opvangen van signalen via IRQ's omdat er dan bij ontvangst van seriele gegevens
  // tekens verloren gaan. Deze hoofdloop duurt normaal ongeveer 27 microseconden en bij kijken naar Ethernet 
  // maximaal 95 uSec. Snel genoeg om een startbit van een binnenkomend RF of IR niet te hoeven missen omdat die
  // van de orde groote zijn tussen 500..3000 microseconden.

  while(true)
    {
    // Check voor IR of RF events
    if(GetEvent_IRRF(&Content,&x)) 
      {
      ProcessEvent(Content,VALUE_DIRECTION_INPUT,x,0,0); // verwerk binnengekomen event.
      }
      
    // 1: niet tijdkritische processen die periodiek uitgevoerd moeten worden
    if(LoopIntervalTimer_1<millis())// korte interval
      {
      LoopIntervalTimer_1=millis()+Loop_INTERVAL_1; // reset de timer  

      switch(Slice_1++)
        {
        case 0:  // binnen Slice_1
          {                        
          Led(GREEN);
          } 

#if NODO_MEGA
        case 1: // binnen Slice_1
          {        
          if(bitRead(HW_Config,HW_ETHERNET))
            {
            // IP Event: *************** kijk of er een Event van IP  **********************    
            if(IPServer.available())// deze call duurt +/- 90uSec.  
              ExecuteIP();
            }
          break;
          }
    
        case 2: // binnen Slice_1 
          {
          // IP Telnet verbinding : *************** kijk of er verzoek tot verbinding vanuit een terminal is **********************    
          if(bitRead(HW_Config,HW_ETHERNET))
            {
            if(TerminalServer.available())
              {          
              if(TerminalConnected==0)
                {
                // we hebben een nieuwe Terminal client
                TerminalClient=TerminalServer.available();
                TerminalConnected=TERMINAL_TIMEOUT;
                InputBuffer_Terminal[0]=0;
                TerminalInbyteCounter=0;
                TerminalClient.flush(); // schoon beginnen.

                if(TerminalLocked==0)
                  TerminalLocked=1;
                  
                if(TerminalLocked<=PASSWORD_MAX_RETRY)
                   TerminalClient.print(ProgmemString(Text_03));
                else
                  {
                  TerminalClient.print(cmd2str(MESSAGE_10));
                  RaiseMessage(MESSAGE_10);
                  }
                }
  
              while(TerminalClient.available()) 
                {
                TerminalInByte=TerminalClient.read();
                
                if(isprint(TerminalInByte))
                  {
                  if(TerminalInbyteCounter<INPUT_BUFFER_SIZE)
                    {
                    if(Settings.EchoTelnet==VALUE_ON)
                      TerminalClient.write(TerminalInByte);// Echo ontvangen teken                  
                    InputBuffer_Terminal[TerminalInbyteCounter++]=TerminalInByte;
                    }
//                  else
//                     TerminalClient.write('#');// geen ruimte meer.
                  }
                  
                if(TerminalInByte==0x03 || TerminalInByte==0x18)
                  {
                  // TerminalSessie timeout, dan de verbinding netjes afsluiten
                  InputBuffer_Terminal[0]=0;
                  TerminalClient.println(ProgmemString(Text_30));
                  delay(1000); // geef de client even de gelegenheid de tekst te ontvangen
                  TerminalClient.stop();
                  TerminalConnected=0;
                  break;
                  }
                  
                if(TerminalInByte==0x0a || TerminalInByte==0x0d)
                  {
                  if(Settings.EchoTelnet==VALUE_ON)
                    TerminalClient.println("");// Echo de nieuwe regel.
                  TerminalConnected=TERMINAL_TIMEOUT;
                  InputBuffer_Terminal[TerminalInbyteCounter]=0;
                  if(TerminalInbyteCounter==0)break; // als de string leeg is, dan niets verwerken.
                  TerminalInbyteCounter=0;
  
                  if(TerminalLocked==0) // als op niet op slot
                    {
                    TerminalClient.getRemoteIP(ClientIPAddress);  
                    ExecuteLine(InputBuffer_Terminal, VALUE_SOURCE_TELNET);
//                    TerminalClient.write('>');
                    }
                  else
                    {
                    if(TerminalLocked<=PASSWORD_MAX_RETRY)// teller is wachtloop bij herhaaldelijke pogingen foutief wachtwoord. Bij >3 pogingen niet meer toegestaan
                      {
                      if(strcmp(InputBuffer_Terminal,Settings.Password)==0)// als wachtwoord goed is, dan slot er af
                        {
                        TerminalLocked=0;
                        y=bitRead(HW_Config,HW_SERIAL);
                        bitWrite(HW_Config,HW_SERIAL,1);
                        PrintWelcome();
                        bitWrite(HW_Config,HW_SERIAL,y);
                        }
                      else// als foutief wachtwoord, dan teller 
                        {
                        TerminalLocked++;
                        TerminalClient.println("?");
                        if(TerminalLocked>PASSWORD_MAX_RETRY)
                          {
                          TerminalLocked=PASSWORD_TIMEOUT; // blokkeer tijd terminal
                          TerminalClient.print(cmd2str(MESSAGE_10));
                          RaiseMessage(MESSAGE_10);
                          }
                        else
                          TerminalClient.print(ProgmemString(Text_03));
                        }
                      }
                    else
                      {
                      TerminalClient.println(cmd2str(MESSAGE_10));
                      }
                    }
                  TerminalClient.write('>');                    
                  }
                }
              }
            }
          break;
          }
          
        case 3: // binnen Slice_1
          {
          // SERIAL: *************** kijk of er data klaar staat op de seriële poort **********************
          if(Serial.available())
            {
            bitWrite(HW_Config,HW_SERIAL,1);// Input op seriele poort ontvangen. Vanaf nu ook output naar Seriele poort want er is klaarblijkelijk een actieve verbinding
            StaySharpTimer=millis()+SHARP_TIME;      

            while(StaySharpTimer>millis()) // blijf even paraat voor luisteren naar deze poort en voorkom dat andere input deze communicatie onderbreekt
              {          
              while(Serial.available())
                {                        
                SerialInByte=Serial.read();                
                if(Settings.EchoSerial==VALUE_ON)
                  Serial.write(SerialInByte);// echo ontvangen teken

                StaySharpTimer=millis()+SHARP_TIME;      
                
                if(isprint(SerialInByte))
                  {
                  if(SerialInByteCounter<INPUT_BUFFER_SIZE) // alleen tekens aan de string toevoegen als deze nog in de buffer past.
                    InputBuffer_Serial[SerialInByteCounter++]=SerialInByte;
//                  else
//                    Serial.write('#');
                  }
                  
                if(SerialInByte=='\n')
                  {
                  SerialHold(true);
                  InputBuffer_Serial[SerialInByteCounter]=0; // serieel ontvangen regel is compleet
                  ExecuteLine(InputBuffer_Serial, VALUE_SOURCE_SERIAL);
                  Serial.write('>');
                  SerialInByteCounter=0;  
                  InputBuffer_Serial[0]=0; // serieel ontvangen regel is verwerkt. String leegmaken
                  SerialHold(false);
                  StaySharpTimer=millis()+SHARP_TIME;      
                  }
                }
              }
            }
          break;
          }
#endif

        default:  // binnen Slice_1
          Slice_1=0;
          break;
        }
      }// switch(Slice_1)

    // 2: niet tijdkritische processen die periodiek uitgevoerd moeten worden
    if(LoopIntervalTimer_2<millis())// lange interval
      {
      LoopIntervalTimer_2=millis()+Loop_INTERVAL_2; // reset de timer  
      
      switch(Slice_2++)
        {
        case 0:
          {
          // CLOCK: **************** Lees periodiek de realtime klok uit en check op events  ***********************
          Content=ClockRead(); // Lees de Real Time Clock waarden in de struct Time
          if(bitRead(HW_Config,HW_CLOCK))//check of de klok we aanwzig is
            {
            if(CheckEventlist(Content,VALUE_SOURCE_CLOCK) && EventTimeCodePrevious!=Content)
              {
              EventTimeCodePrevious=Content; 
              ProcessEvent(Content,VALUE_DIRECTION_INTERNAL,VALUE_SOURCE_CLOCK,0,0);      // verwerk binnengekomen event.
              }
            else
              Content=0L;
            }
          break;
          }

        case 1:
          {    
          // DAYLIGHT: **************** Check zonsopkomst & zonsondergang  ***********************
          if(bitRead(HW_Config,HW_CLOCK))
            {
            SetDaylight();
            if(Time.Daylight!=DaylightPrevious)// er heeft een zonsondergang of zonsopkomst event voorgedaan
              {
              Content=command2event(Settings.Unit, CMD_CLOCK_EVENT_DAYLIGHT,Time.Daylight,0L);
              DaylightPrevious=Time.Daylight;
              ProcessEvent(Content,VALUE_DIRECTION_INTERNAL,VALUE_SOURCE_CLOCK,0,0);      // verwerk binnengekomen event.
              }
            }
          break;
          }
          
        case 2:
          {
          // WIRED: *************** kijk of statussen gewijzigd zijn op WIRED **********************  
          // als de huidige waarde groter dan threshold EN de vorige keer was dat nog niet zo DAN verstuur code
          for(x=0;x<WIRED_PORTS;x++)
            {
            // lees analoge waarde. Dit is een 10-bit waarde, unsigned 0..1023
            // vervolgens met map() omrekenen naar gekalibreerde waarde        
            y=map(analogRead(PIN_WIRED_IN_1+x),Settings.WiredInput_Calibration_IL[x],Settings.WiredInput_Calibration_IH[x],Settings.WiredInput_Calibration_OL[x],Settings.WiredInput_Calibration_OH[x]);        
                 
            if(!WiredInputStatus[x] && y>(Settings.WiredInputThreshold[x]+Settings.WiredInputSmittTrigger[x]))
              {
              WiredInputStatus[x]=true;
              Content=command2event(Settings.Unit, CMD_WIRED_IN_EVENT,x+1,WiredInputStatus[x]?VALUE_ON:VALUE_OFF);
              ProcessEvent(Content,VALUE_DIRECTION_INPUT,VALUE_SOURCE_WIRED,0,0);      // verwerk binnengekomen event.
              }
      
            if(WiredInputStatus[x] && y<(Settings.WiredInputThreshold[x]-Settings.WiredInputSmittTrigger[x]))
              {
              WiredInputStatus[x]=false;
              Content=command2event(Settings.Unit, CMD_WIRED_IN_EVENT,x+1,WiredInputStatus[x]?VALUE_ON:VALUE_OFF);
              ProcessEvent(Content,VALUE_DIRECTION_INPUT,VALUE_SOURCE_WIRED,0,0);      // verwerk binnengekomen event.
              }
            }
          break;
          }

        case 3:
          break;

        case 4:
          {
          // TIMER: **************** Genereer event als één van de Timers voor de gebruiker afgelopen is ***********************    
          for(x=0;x<TIMER_MAX;x++)
            {
            if(UserTimer[x]!=0L)// als de timer actief is
              {
              if(UserTimer[x]<millis()) // als de timer is afgelopen.
                {
                UserTimer[x]=0;// zet de timer op inactief.
                Content=command2event(Settings.Unit, CMD_TIMER_EVENT,x+1,0);
                ProcessEvent(Content,VALUE_DIRECTION_INTERNAL,VALUE_SOURCE_TIMER,0,0);      // verwerk binnengekomen event.
                }
              }
            }
          break;
          }
        default:
          Slice_2=0;
        }
      }

    // 3: niet tijdkritische processen die periodiek uitgevoerd moeten worden
    if(LoopIntervalTimer_3<millis())// lange interval: Iedere seconde.
      {
      LoopIntervalTimer_3=millis()+Loop_INTERVAL_3; // reset de timer  

      // loop periodiek langs de userplugin
      #if USER_PLUGIN
        UserPlugin_Periodically();
      #endif

#if NODO_MEGA
      // Terminal onderhoudstaken
      // tel seconden terug nadat de gebruiker driemaal foutief wachtwoord ingegeven
      if(TerminalLocked>PASSWORD_MAX_RETRY)
        if(--TerminalLocked==PASSWORD_MAX_RETRY)TerminalLocked=1;

      if(TerminalConnected)// Sessie duurt nog TerminalConnected seconden. Als 0 dan is er geen verbinding meer
        {
        TerminalConnected--;
        if(!TerminalClient.connected())
          TerminalConnected=0;
        if(!TerminalConnected)
          {
          TerminalClient.flush();
          TerminalClient.stop();
          }        
        }
      
      // Timer voor blokkeren verwerking. teller verlagen
      if(FileWriteMode>0)
        {
        FileWriteMode--;
        if(FileWriteMode==0)
          TempLogFile[0]=0;
        }
        
      // Timer voor verzenden van Cookie naar de WebApp
      if(Settings.HTTP_Pin==VALUE_ON)
        {
        if(CookieTimer>0)
          CookieTimer--;
        else
          {
          CookieTimer=COOKIE_REFRESH_TIME;
          SendHTTPCookie(); // Verzend een nieuw cookie
          }
        }
#endif

      // De Nodo houdt bij of Andere Nodos bezig zijn. Periodiek wordt de status gereset
      // om te voorkomen dat de Nodo lange tijd onnodig vast komt te zitten.
      // Als deze teller nul is, dan wordt de status van de busy Nodos gereset.
      if(Busy.ResetTimer>0)
        Busy.ResetTimer--;
      else
        Busy.Status=0;      

      }
    }// while 
  }


