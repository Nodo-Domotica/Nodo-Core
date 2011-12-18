
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
 ******************************************************************************************************************************
 *
 * Voor toelichting op de licentievoorwaarden zie    : http://www.gnu.org/licenses
 * Uitgebreide documentatie is te vinden op          : http://www.nodo-domotica.nl
 * bugs kunnen worden gelogd op                      : https://code.google.com/p/arduino-nodo/
 * Compiler voor deze programmacode te downloaden op : http://arduino.cc
 * Voor vragen of suggesties, mail naar              : p.k.tonkes@gmail.com
 * Compiler                                          : Arduino Compiler versie 1.0
 * Arduino board                                     : Arduino Mega 1280 of 2560 @16Mhz.
 * Hardware                                          : Hardware en Arduino penbezetting volgens schema ???
 *
 \****************************************************************************************************************************/

#define VERSION        005        // Nodo Version nummer:
                                  // Major.Minor.Patch
                                  // Major: Grote veranderingen aan concept, besturing, werking.
                                  // Minor: Uitbreiding/aanpassing van commando's, functionaliteit en MMI aanpassingen
                                  // Patch: Herstel van bugs zonder (ingrijpende) functionele veranderingen.

#include "pins_arduino.h"
#include <EEPROM.h>
#include <Wire.h>
#include <avr/pgmspace.h>
#include <SPI.h>
#include <EthernetNodo.h>
#include <stdint.h>
#include <SD.h>

/**************************************************************************************************************************\
*  Nodo Event            = TTTTUUUUCCCCCCCC1111111122222222       -> T=Type, U=Unit, C=Command, 1=Par-1, 2=Par-2
\**************************************************************************************************************************/

// strings met vaste tekst naar PROGMEM om hiermee RAM-geheugen te sparen.
prog_char PROGMEM Text_01[] = "Nodo Domotica controller (c) Copyright 2011 P.K.Tonkes.";
prog_char PROGMEM Text_02[] = "Licensed under GNU General Public License.";
prog_char PROGMEM Text_03[] = "Line=";
prog_char PROGMEM Text_04[] = "SunMonThuWedThuFriSat";
prog_char PROGMEM Text_05[] = "0123456789abcdef";
prog_char PROGMEM Text_06[] = "Error=";
prog_char PROGMEM Text_07[] = "Waiting for signal...";
prog_char PROGMEM Text_08[] = "Queue=Out, ";
prog_char PROGMEM Text_09[] = "Queue=In, ";
prog_char PROGMEM Text_10[] = "EventClientIP"; // nog nodig ???
prog_char PROGMEM Text_11[] = "Output=";
prog_char PROGMEM Text_12[] = "Input=";
prog_char PROGMEM Text_13[] = "Ok.";
prog_char PROGMEM Text_14[] = "Event=";
prog_char PROGMEM Text_15[] = "Booting...";
prog_char PROGMEM Text_16[] = "Action=";
prog_char PROGMEM Text_17[] = "payload";
prog_char PROGMEM Text_18[] = "accept";
prog_char PROGMEM Text_19[] = "close";
prog_char PROGMEM Text_20[] = "quintessence";
prog_char PROGMEM Text_21[] = "payload withoutRelease";
prog_char PROGMEM Text_22[] = "********************************************************************************";
prog_char PROGMEM Text_23[] = "Log.txt";
prog_char PROGMEM Text_24[] = "Logging on SDCard enabled.";
prog_char PROGMEM Text_25[] = "System=";
prog_char PROGMEM Text_26[] = "Event received from: ";
prog_char PROGMEM Text_27[] = "Raw/Key"; // Directory op de SDCard voor opslag RawSignal
prog_char PROGMEM Text_28[] = "Raw/Hex"; // Directory op de SDCard voor opslag RawSignal
prog_char PROGMEM Text_29[] = "RawSignal="; //??? is deze nog nodig
prog_char PROGMEM Text_30[] = "No access!";
 
// Commando's:
prog_char PROGMEM Cmd_000[]="";    
prog_char PROGMEM Cmd_001[]="ReceiveSettings";
prog_char PROGMEM Cmd_002[]="BreakOnVarEqu";
prog_char PROGMEM Cmd_003[]="BreakOnVarLess";
prog_char PROGMEM Cmd_004[]="BreakOnVarMore";
prog_char PROGMEM Cmd_005[]="BreakOnVarNEqu";
prog_char PROGMEM Cmd_006[]="ClockSetDate";
prog_char PROGMEM Cmd_007[]="ClockSetYear";
prog_char PROGMEM Cmd_008[]="ClockSetTime";
prog_char PROGMEM Cmd_009[]="ClockSetDOW";
prog_char PROGMEM Cmd_010[]="Delay";
prog_char PROGMEM Cmd_011[]="Divert";
prog_char PROGMEM Cmd_012[]="EventlistErase";
prog_char PROGMEM Cmd_013[]="EventlistShow";
prog_char PROGMEM Cmd_014[]="EventlistWrite";
prog_char PROGMEM Cmd_015[]="VariableSave";
prog_char PROGMEM Cmd_016[]="RawSignalSave";
prog_char PROGMEM Cmd_017[]="RawSignalSend";
prog_char PROGMEM Cmd_018[]="Reset";
prog_char PROGMEM Cmd_019[]="SendKAKU";
prog_char PROGMEM Cmd_020[]="SendNewKAKU";
prog_char PROGMEM Cmd_021[]="";
prog_char PROGMEM Cmd_022[]="Terminal";
prog_char PROGMEM Cmd_023[]="SimulateDay";
prog_char PROGMEM Cmd_024[]="Sound";
prog_char PROGMEM Cmd_025[]="Status";
prog_char PROGMEM Cmd_026[]="UserPlugin";
prog_char PROGMEM Cmd_027[]="TimerRandom";
prog_char PROGMEM Cmd_028[]="TimerSetSec";
prog_char PROGMEM Cmd_029[]="TimerSetMin";
prog_char PROGMEM Cmd_030[]="";//???
prog_char PROGMEM Cmd_031[]="Unit";
prog_char PROGMEM Cmd_032[]="WaitBusy";
prog_char PROGMEM Cmd_033[]="VariableDec";
prog_char PROGMEM Cmd_034[]="VariableInc";
prog_char PROGMEM Cmd_035[]="VariableSet";
prog_char PROGMEM Cmd_036[]="VariableVariable";
prog_char PROGMEM Cmd_037[]="";
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
prog_char PROGMEM Cmd_048[]="VariableSendUserEvent";
prog_char PROGMEM Cmd_049[]="WiredRange";
prog_char PROGMEM Cmd_050[]="VariableUserEvent";
prog_char PROGMEM Cmd_051[]="WiredAnalogCalibrate";
prog_char PROGMEM Cmd_052[]="Reboot";
prog_char PROGMEM Cmd_053[]="TerminalPort";
prog_char PROGMEM Cmd_054[]="WiredAnalogSend";
prog_char PROGMEM Cmd_055[]="EventPort";
prog_char PROGMEM Cmd_056[]="AnalyseSettings";
prog_char PROGMEM Cmd_057[]="Password";
prog_char PROGMEM Cmd_058[]="OutputIR";
prog_char PROGMEM Cmd_059[]="OutputRF";
prog_char PROGMEM Cmd_060[]="";
prog_char PROGMEM Cmd_061[]="LogShow";
prog_char PROGMEM Cmd_062[]="LogErase";
prog_char PROGMEM Cmd_063[]="";
prog_char PROGMEM Cmd_064[]="";
prog_char PROGMEM Cmd_065[]="";
prog_char PROGMEM Cmd_066[]="";
prog_char PROGMEM Cmd_067[]="";
prog_char PROGMEM Cmd_068[]="";
prog_char PROGMEM Cmd_069[]="";
prog_char PROGMEM Cmd_070[]="";
prog_char PROGMEM Cmd_071[]="";
prog_char PROGMEM Cmd_072[]="";
prog_char PROGMEM Cmd_073[]="";
prog_char PROGMEM Cmd_074[]="";
prog_char PROGMEM Cmd_075[]="";
prog_char PROGMEM Cmd_076[]="";
prog_char PROGMEM Cmd_077[]="";
prog_char PROGMEM Cmd_078[]="";
prog_char PROGMEM Cmd_079[]="";
prog_char PROGMEM Cmd_080[]="";
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
prog_char PROGMEM Cmd_099[]="";

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
prog_char PROGMEM Cmd_118[]="Error";
prog_char PROGMEM Cmd_119[]="Boot";
prog_char PROGMEM Cmd_120[]="DaylightSaving";
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
prog_char PROGMEM Cmd_152[]="Command";
prog_char PROGMEM Cmd_153[]="Parameter";
prog_char PROGMEM Cmd_154[]="IR";
prog_char PROGMEM Cmd_155[]="EventGhost";
prog_char PROGMEM Cmd_156[]="RF";
prog_char PROGMEM Cmd_157[]="Serial";
prog_char PROGMEM Cmd_158[]="Wired";
prog_char PROGMEM Cmd_159[]="EventList";
prog_char PROGMEM Cmd_160[]="System";
prog_char PROGMEM Cmd_161[]="Timers";
prog_char PROGMEM Cmd_162[]="Variables";
prog_char PROGMEM Cmd_163[]="Clock";
prog_char PROGMEM Cmd_164[]="Terminal";
prog_char PROGMEM Cmd_165[]="Tag";
prog_char PROGMEM Cmd_166[]="Time";
prog_char PROGMEM Cmd_167[]="";
prog_char PROGMEM Cmd_168[]="Input";
prog_char PROGMEM Cmd_169[]="Output";
prog_char PROGMEM Cmd_170[]="Internal";
prog_char PROGMEM Cmd_171[]="Busy";
prog_char PROGMEM Cmd_172[]="Source";
prog_char PROGMEM Cmd_173[]="RF2IR";
prog_char PROGMEM Cmd_174[]="IR2RF";
prog_char PROGMEM Cmd_175[]="All";
prog_char PROGMEM Cmd_176[]="Output_RAW";//??? nog in gebruik?
prog_char PROGMEM Cmd_177[]="Nesting";
prog_char PROGMEM Cmd_178[]="Queue";
prog_char PROGMEM Cmd_179[]="Trace";
prog_char PROGMEM Cmd_180[]="";
prog_char PROGMEM Cmd_181[]="ServerIP";
prog_char PROGMEM Cmd_182[]="High";
prog_char PROGMEM Cmd_183[]="Low";
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
prog_char PROGMEM Cmd_200[]="";
prog_char PROGMEM Cmd_201[]="Unknown command.";
prog_char PROGMEM Cmd_202[]="Invalid parameter in command.";
prog_char PROGMEM Cmd_203[]="Unable to access SDCard.";
prog_char PROGMEM Cmd_204[]="Queue overflow.";
prog_char PROGMEM Cmd_205[]="Eventlist nesting error.";
prog_char PROGMEM Cmd_206[]="Writing to eventlist failed.";
prog_char PROGMEM Cmd_207[]="IP connection problem.";
prog_char PROGMEM Cmd_208[]="Incorrect password.";
prog_char PROGMEM Cmd_209[]="Command not supported in this Nodo version.";
prog_char PROGMEM Cmd_210[]="Unauthorized terminal access.";

// commando:
#define FIRST_COMMAND                    0 // Eerste COMMANDO uit de commando tabel
#define CMD_NULL                         0 // lege dummy.
#define CMD_RECEIVE_SETTINGS             1
#define CMD_BREAK_ON_VAR_EQU             2
#define CMD_BREAK_ON_VAR_LESS            3
#define CMD_BREAK_ON_VAR_MORE            4
#define CMD_BREAK_ON_VAR_NEQU            5
#define CMD_CLOCK_DATE                   6
#define CMD_CLOCK_YEAR                   7
#define CMD_CLOCK_TIME                   8
#define CMD_CLOCK_DOW                    9
#define CMD_DELAY                       10
#define CMD_DIVERT                      11
#define CMD_EVENTLIST_ERASE             12
#define CMD_EVENTLIST_SHOW              13
#define CMD_EVENTLIST_WRITE             14
#define CMD_VARIABLE_SAVE               15
#define CMD_RAWSIGNAL_SAVE              16
#define CMD_RAWSIGNAL_SEND              17
#define CMD_RESET                       18
#define CMD_SEND_KAKU                   19
#define CMD_SEND_KAKU_NEW               20
#define CMD_res21                       21
#define CMD_TERMINAL                    22
#define CMD_SIMULATE_DAY                23
#define CMD_SOUND                       24
#define CMD_STATUS                      25
#define CMD_USERPLUGIN                  26
#define CMD_TIMER_RANDOM                27
#define CMD_TIMER_SET_SEC               28
#define CMD_TIMER_SET_MIN               29
#define CMD_RES30                       30
#define CMD_UNIT                        31
#define CMD_WAITBUSY                    32
#define CMD_VARIABLE_DEC                33
#define CMD_VARIABLE_INC                34
#define CMD_VARIABLE_SET                35
#define CMD_VARIABLE_VARIABLE           36
#define CMD_Res                         37
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
#define CMD_VARIABLE_SEND_USEREVENT     48
#define CMD_WIRED_RANGE                 49
#define CMD_VARIABLE_USEREVENT          50
#define CMD_WIRED_ANALOG_CALIBRATE      51
#define CMD_REBOOT                      52
#define CMD_MONITOR_PORT                53
#define CMD_WIRED_ANALOG_SEND           54
#define CMD_EVENT_PORT                  55
#define CMD_ANALYSE_SETTINGS            56
#define CMD_PASSWORD                    57
#define CMD_TRANSMIT_IR                 58
#define CMD_TRANSMIT_RF                 59
#define CMD_RES60                       60
#define CMD_LOGFILE_SHOW                61
#define CMD_LOGFILE_ERASE               62
#define CMD_RES063                      63
#define CMD_RES064                      64
#define CMD_RES065                      65
#define CMD_RES066                      66
#define CMD_RES067                      67
#define CMD_RES068                      68
#define CMD_RES069                      69
#define CMD_RES070                      70
#define CMD_RES071                      71
#define CMD_RES072                      72
#define CMD_RES073                      73
#define CMD_RES074                      74
#define CMD_RES075                      75
#define CMD_RES076                      76
#define CMD_RES077                      77
#define CMD_RES078                      78
#define CMD_RES079                      79
#define CMD_RES080                      80
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
#define CMD_RES099                      99
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
#define CMD_ERROR                      118
#define CMD_BOOT_EVENT                 119
#define CMD_DLS_EVENT                  120
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
#define VALUE_COMMAND                  152
#define VALUE_PARAMETER                153
#define VALUE_SOURCE_IR                154
//#define VALUE_SOURCE_IR_RF             155
#define VALUE_SOURCE_EVENTGHOST        155
#define VALUE_SOURCE_RF                156
#define VALUE_SOURCE_SERIAL            157
#define VALUE_SOURCE_WIRED             158
#define VALUE_SOURCE_EVENTLIST         159
#define VALUE_SOURCE_SYSTEM            160
#define VALUE_SOURCE_TIMER             161
#define VALUE_SOURCE_VARIABLE          162
#define VALUE_SOURCE_CLOCK             163
#define VALUE_SOURCE_TERMINAL          164
#define VALUE_TAG                      165 // ??? kan weg?
#define VALUE_TIMESTAMP                166
#define VALUE_RES167                   167
#define VALUE_DIRECTION_INPUT          168
#define VALUE_DIRECTION_OUTPUT         169
#define VALUE_DIRECTION_INTERNAL       170
#define VALUE_BUSY                     171
#define VALUE_SOURCE                   172
#define VALUE_RF_2_IR                  173 //??? kan weg?
#define VALUE_IR_2_RF                  174 // ??? kan weg ?
#define VALUE_ALL                      175 // Deze waarde MOET groter dan 16 zijn.
#define VALUE_DIRECTION_OUTPUT_RAW     176
#define VALUE_NESTING                  177
#define VALUE_SOURCE_QUEUE             178
#define VALUE_TRACE                    179
#define VALUE_RES180                   180
#define VALUE_SOURCE_SERVER_IP         181
#define VALUE_HIGH                     182
#define VALUE_LOW                      183
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
#define VALUE_RES200                   200
#define ERROR_01                       201
#define ERROR_02                       202
#define ERROR_03                       203
#define ERROR_04                       204
#define ERROR_05                       205
#define ERROR_06                       206
#define ERROR_07                       207
#define ERROR_08                       208
#define ERROR_09                       209
#define ERROR_10                       210
#define LAST_VALUE                     210 // laatste VALUE uit de commando tabel
#define COMMAND_MAX                    211 // aantal commando's (dus geteld vanaf 0)


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
  Cmd_210          
  };          

// Tabel met zonsopgang en -ondergang momenten. afgeleid van KNMI gegevens midden Nederland.
PROGMEM prog_uint16_t Sunrise[]={528,525,516,503,487,467,446,424,401,378,355,333,313,295,279,268,261,259,263,271,283,297,312,329,345,367,377,394,411,428,446,464,481,498,512,522,528,527};
PROGMEM prog_uint16_t Sunset[]={999,1010,1026,1044,1062,1081,1099,1117,1135,1152,1169,1186,1203,1219,1235,1248,1258,1263,1264,1259,1249,1235,1218,1198,1177,1154,1131,1107,1084,1062,1041,1023,1008,996,990,989,993,1004};

// omschakeling zomertijd / wintertijd voor komende 10 jaar. één int bevat de omschakeldatum van maart en oktober.
#define DLSBase 2010 // jaar van eerste element uit de array
PROGMEM prog_uint16_t DLSDate[]={2831,2730,2528,3127,3026,2925,2730,2629,2528,3127};

// Declaratie aansluitingen I/O-pennen op de Arduino ATMega2560 voorzien van een Wiznet EthernetShield met SDCard

// Future use                       6,7// Future use
#define WiredAnalogInputPin_1       8  // Analoge inputs A8 t/m A15 worden gebruikt voor WiredIn 1 tot en met 8
// Future use                  A0..A5  // Future use.
// Gereserveerd                     0  // USB-RX
// Gereserveerd                     1  // USB-TX
// Gereseveerd                      2  // IRQ,Ethernet
#define IR_ReceiveDataPin           3  // Op deze input komt het IR signaal binnen van de TSOP. Bij HIGH bij geen signaal.
#define EthernetShield_CS_SDCard    4  // Chipselect van de SDCard. Niet gebruiken voor andere doeleinden
#define MonitorLedPin               5  // bij iedere ontvangst of verzending licht deze led kort op.
#define LED_RGB_R                   5  // RGB-Led, aansluiting rood
#define LED_RGB_G                   6  // RGB-Led, aansluiting groen
#define LED_RGB_B                   7  // RGB-Led, aansluiting blauw
#define BuzzerPin                   8  // luidspreker aansluiting
#define IR_TransmitDataPin          9  // Aan deze PWM pin zit een zender IR-Led. (gebufferd via transistor i.v.m. hogere stroom die nodig is voor IR-led)
#define Ethernetshield_CS_W5100     10 // D10..D13  // gereserveerd voor Ethernet & SDCard
#define RF_TransmitPowerPin         14 // +5 volt / Vcc spanning naar de zender.
#define RF_TransmitDataPin          15 // data naar de zender
#define RF_ReceivePowerPin          16 // Spanning naar de ontvanger via deze pin.
#define RF_ReceiveDataPin           17 // Op deze input komt het 433Mhz-RF signaal binnen. LOW bij geen signaal.
#define Serial_TX                   18 // Future use: TX serial communication
#define Serial_RX                   19 // Future use: RX serial communication
// gereserveerd                     20 // I2C-SDA
// gereserveerd                     21 // I2C-SLC
#define WiredDigitalOutputPin_1     22 // 8 digitale outputs D22 t/m D29 worden gebruikt voor WiredIn 1 tot en met 8
// Future use                 D30..D49 // Future use
// Gereserveerd               D50..D53 // Ethernet / SPI
#define EthernetShield_CS_SDCardH   53 // Gereserveerd voor correct funktioneren van de SDCard: Hardware CS/SPI ChipSelect


#define SERVER_IP_MAX                8 // maximaal aantal servers in de tebel met IP adressen 
#define UNIT                       0x1 // Unit nummer van de Nodo. Bij gebruik van meerdere nodo's deze uniek toewijzen [1..F]
#define EVENTLIST_MAX              256 // aantal events dat de lijst bevat in het EEPROM geheugen van de ATMega328. Iedere regel in de eventlist heeft 8 bytes nodig. eerste adres is 0
#define USER_VARIABLES_MAX          32 // aantal beschikbare gebruikersvariabelen voor de user.
#define RAW_BUFFER_SIZE            256 // Maximaal aantal te ontvangen 128 bits.
#define UNIT_MAX                    15 // Hoogst mogelijke unit nummer van een Nodo
#define MACRO_EXECUTION_DEPTH       10 // maximale nesting van macro's.
#define WIRED_PORTS                  8 // aantal WiredIn/WiredOut poorten
#define BAUD                     19200 // Baudrate voor seriële communicatie.
#define SERIAL_TERMINATOR_1       0x0A // Met dit teken wordt een regel afgesloten. 0x0A is een linefeed <LF>, default voor EventGhost
#define SERIAL_TERMINATOR_2       0x00 // Met dit teken wordt een regel afgesloten. 0x0D is een Carriage Return <CR>, 0x00 = niet in gebruik.
#define INPUT_BUFFER_SIZE          128 // Buffer waar de karakters van de seriele /IP poort in worden opgeslagen.
#define TIMER_MAX                   16 // aantal beschikbare timers voor de user, gerekend vanaf 
#define Loop_INTERVAL_1            250 // tijdsinterval in ms. voor achtergrondtaken snelle verwerking
#define Loop_INTERVAL_2           5000 // tijdsinterval in ms. voor achtergrondtaken langzame verwerking
#define EVENT_QUEUE_MAX             32 // maximaal aantal plaatsen in de queue
#define ENDSIGNAL_TIME            1500 // Dit is de tijd in milliseconden waarna wordt aangenomen dat het ontvangen één reeks signalen beëindigd is
#define SIGNAL_TIMEOUT_RF         5000 // na deze tijd in uSec. wordt één RF signaal als beëindigd beschouwd.
#define SIGNAL_TIMEOUT_IR        10000 // na deze tijd in uSec. wordt één IR signaal als beëindigd beschouwd.
#define TX_REPEATS                   5 // aantal herhalingen van een code binnen één RF of IR reeks
#define MIN_PULSE_LENGTH           100 // pulsen korter dan deze tijd uSec. worden als stoorpulsen beschouwd.
#define MIN_RAW_PULSES              16 // =8 bits. Minimaal aantal ontvangen bits*2 alvorens cpu tijd wordt besteed aan decodering, etc. Zet zo hoog mogelijk om CPU-tijd te sparen en minder 'onzin' te ontvangen.
#define SHARP_TIME                 500 // tijd in milliseconden dat de nodo gefocust moet blijven luisteren naar één dezelfde poort na binnenkomst van een signaal
#define SIGNAL_TYPE_UNKNOWN          0 // Type ontvangen of te verzenden signaal in de eventcode
#define SIGNAL_TYPE_NODO             1 // Type ontvangen of te verzenden signaal in de eventcode
#define SIGNAL_TYPE_KAKU             2 // Type ontvangen of te verzenden signaal in de eventcode
#define SIGNAL_TYPE_NEWKAKU          3 // Type ontvangen of te verzenden signaal in de eventcode
#define NODO_TYPE_EVENT              1
#define NODO_TYPE_COMMAND            2

//****************************************************************************************************************************************

struct Settings
  {
  int     Version;        
  int     WiredInputThreshold[WIRED_PORTS], WiredInputSmittTrigger[WIRED_PORTS];
  int     WiredInput_Calibration_IH[WIRED_PORTS], WiredInput_Calibration_IL[WIRED_PORTS];
  int     WiredInput_Calibration_OH[WIRED_PORTS], WiredInput_Calibration_OL[WIRED_PORTS];
  boolean WiredInputPullUp[WIRED_PORTS];
  byte    AnalyseSharpness;
  int     AnalyseTimeOut;
  byte    UserVar[USER_VARIABLES_MAX];
  byte    Unit;
  byte    TransmitIR;
  byte    TransmitRF;
  byte    TransmitRepeatIR;
  byte    TransmitRepeatRF;
  byte    WaitFreeRF_Window;
  byte    WaitFreeRF_Delay;
  byte    SendBusy;
  byte    WaitBusy;
  boolean Trace;                                            // Weergeven van extra gegevens t.b.v. beter inzicht verloop van de verwerking
  boolean DaylightSaving;                                   // Vlag die aangeeft of het zomertijd of wintertijd is
  int     DaylightSavingSet;                                // Vlag voor correct automatisch kunnen overschakelen van zomertijd naar wintertijd of vice-versa
  char    Password[25];                                     // String met wachtwoord.
  byte    Server_IP[SERVER_IP_MAX][4];                      // lijst met IP adressen waar de events naar toe moeten worden gestuurd.
  int     Event_Port;                                       // IP-Poort waar EventGhost events op binnenkomen / uitgaan.
  int     Terminal_Port;                                    // IP-Poort waar de Telnes sessie over verbinding maakt.
  byte    Terminal_Enabled;                                 // vlag geeft aan of Telnet sessies toegestaan zijn.
  byte    Terminal_Prompt;                                  // vlag geeft aan of er een prompt getoond moet worden tijdens de telnet sessie
  }S;

char InputBuffer[INPUT_BUFFER_SIZE];                        // Buffer voor input van IP en Serial regel.
unsigned long UserTimer[TIMER_MAX];                         // Timers voor de gebruiker.
unsigned long StaySharpTimer=millis();                      // timer die start bij ontvangn van een signaal. Dwingt om enige tijd te luisteren naar dezelfde poort.
unsigned long LoopIntervalTimer_1=millis();                 // Timer voor periodieke verwerking. millis() maakt dat de intervallen van 1 en 2 niet op zelfde moment vallen => 1 en 2 nu asynchroon.
unsigned long LoopIntervalTimer_2=0L;                       // Timer voor periodieke verwerking.
unsigned long QueueEvent[EVENT_QUEUE_MAX];                  // queue voor tijdelijk onthouden van events die tijdens een delay functie voorbij komen.
byte QueuePort[EVENT_QUEUE_MAX];                            // tabel behorend bij de queue. Geeft herkomst van het event in de queue aan.
byte QueuePos;                                              // teller die wijst naar een plaats in de queue.
byte Ethernet_MAC_Address[]={0xDE,0xAD,0xBE,0xEF,0xFE,0xED};// MAC adres van de Nodo.
EthernetServer EventServer(1024);                           // Server class voor ontvangen van Events van een EventGhost applicatie
EthernetClient EventClient=false;                           // Client class voor ontvangen van Events van een EventGhost applicatie
EthernetServer TerminalServer(23);                          // Server class voor Terminal sessie.
EthernetClient TerminalClient=false;                        // Client class voor Terminal sessie.
boolean WiredInputStatus[WIRED_PORTS];                      // Status van de WiredIn worden hierin opgeslagen.
boolean WiredOutputStatus[WIRED_PORTS];                     // Wired variabelen.
int BusyNodo;                                               // in deze variabele de status van het event 'Busy' van de betreffende units 1 t/m 15. bit-1 = unit-1.
byte UserVarPrevious[USER_VARIABLES_MAX];                   // Vorige versie van de UserVariablles: om wisselingen te kunnen vaststellen.
byte DaylightPrevious;                                      // t.b.v. voorkomen herhaald genereren van events binnen de lopende minuut waar dit event zich voordoet.
byte WiredCounter=0, VariableCounter;                       // tellers.
byte EventlistDepth=0;                                      // teller die bijhoudt hoe vaak er binnen een macro weer een macro wordt uitgevoerd. Voorkomt tevens vastlopers a.g.v. loops die door een gebruiker zijn gemaakt met macro's.
byte Hold=false;
unsigned long Content=0L,ContentPrevious;                   // Ontvangen event van RF, IR, ... Tevens buffer voor het vorige ontvangen Event
unsigned long Checksum=0L;                                  // Als gelijk aan Event dan tweemaal dezelfde code ontvangen: checksum funktie.
unsigned long SupressRepeatTimer;
unsigned long HoldTimer;
unsigned long EventTimeCodePrevious;                        // t.b.v. voorkomen herhaald ontvangen van dezelfde code binnen ingestelde tijd.
void(*Reset)(void)=0;                                       // reset functie op adres 0.
uint8_t RFbit,RFport,IRbit,IRport;                          // t.b.v. verwerking IR/FR signalen.
uint8_t MD5HashCode[16];                                    // tabel voor berekenen van MD5 hash codes t.b.v. uitwisselen wachtwoord EventGhost.
boolean SDCardPresent = false;                              // Vlag die aangeeft of er een SDCard is gevonden die kan worden beschreven.
boolean EthernetEnabled = false;                            // Vlag die aangeeft of er een Ethernetverbinding is.
byte UserVar[USER_VARIABLES_MAX];
char TempString[INPUT_BUFFER_SIZE];                         // Globale, tijdelijke string voor algemeen gebruik in diverste functies. ??? Nodig?
byte IP[4];                                                 // tijdelijke plek voor IP adres.
boolean TerminalConnected=false;                            // Vlag geeft aan of er een verbinding is met een Terminal.
boolean SerialConnected=true;                               // Vlag geeft aan of er een verbinding USB-poort.

// RealTimeclock DS1307
struct RealTimeClock {byte Hour,Minutes,Seconds,Date,Month,Day,Daylight; int Year,DaylightSaving;}Time; // Hier wordt datum & tijd in opgeslagen afkomstig van de RTC.

struct RawsignalStruct
  {
  unsigned int Pulses[RAW_BUFFER_SIZE+2];                   // Tabel met de gemeten pulsen in microseconden. eerste waarde [0] wordt NIET gebruikt. (legacy redenen).
  byte Source;                                              // Bron waar het signaal op is binnengekomen.
  int Number;                                               // aantal bits, maal twee omdat iedere bit een pulse en een space heeft.
  byte Key;                                                 // sleutel waaronder de pulsenreeks op SDCard opgeslgen moet worden.
  byte Type;                                                // Type signaal dan ontvangen is.
  }RawSignal;
  

void setup() 
  {    
  byte x;

  Serial.begin(BAUD);  // Initialiseer de seriële poort
  Serial.println(ProgmemString(Text_15));
  SerialHold(true);// XOFF verzenden zodat PC even wacht met versturen van data via Serial (Xon/Xoff-handshaking)
  LoadSettings();      // laad alle settings zoals deze in de EEPROM zijn opgeslagen
  if(S.Version!=VERSION)ResetFactory(); // Als versienummer in EEPROM niet correct is, dan een ResetFactory.
  
  // Initialiseer in/output poorten.
  pinMode(IR_ReceiveDataPin,INPUT);
  pinMode(RF_ReceiveDataPin,INPUT);
  pinMode(RF_TransmitDataPin,OUTPUT);
  pinMode(RF_TransmitPowerPin,OUTPUT);
  pinMode(RF_ReceivePowerPin,OUTPUT);
  pinMode(IR_TransmitDataPin,OUTPUT);
  pinMode(MonitorLedPin,OUTPUT);
  pinMode(BuzzerPin, OUTPUT);
  pinMode(EthernetShield_CS_SDCardH, OUTPUT); // CS/SPI: nodig voor correct funktioneren van de SDCard!
  digitalWrite(IR_ReceiveDataPin,HIGH);  // schakel pull-up weerstand in om te voorkomen dat er rommel binnenkomt als pin niet aangesloten.
  digitalWrite(RF_ReceiveDataPin,HIGH);  // schakel pull-up weerstand in om te voorkomen dat er rommel binnenkomt als pin niet aangesloten.
  digitalWrite(RF_ReceivePowerPin,HIGH); // Spanning naar de RF ontvanger aann
  RFbit=digitalPinToBitMask(RF_ReceiveDataPin);
  RFport=digitalPinToPort(RF_ReceiveDataPin);  
  IRbit=digitalPinToBitMask(IR_ReceiveDataPin);
  IRport=digitalPinToPort(IR_ReceiveDataPin);

  // initialiseer de Wired ingangen.
  for(x=0;x<WIRED_PORTS;x++)
    {
    digitalWrite(A0+WiredAnalogInputPin_1+x,S.WiredInputPullUp[x]?HIGH:LOW);// Zet de pull-up weerstand van 20K voor analoge ingangen. Analog-0 is gekoppeld aan Digital-14
    pinMode(WiredDigitalOutputPin_1+x,OUTPUT); // definieer Arduino pin's voor Wired-Out
    }

  IR_PWM_INIT();// initialiseer 38Khz PWM voor verzenden van IR signalen.
  Wire.begin();        // zet I2C communicatie gereed voor uitlezen van de realtime clock.

  // SDCard initialiseren:
  // Maak directories aan op de SDCard als ze nog iet bestaan) en check of het logbestand kan worden geopend
  // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer SDCard chip
  digitalWrite(Ethernetshield_CS_W5100, HIGH);
  digitalWrite(EthernetShield_CS_SDCard,LOW);
  if(SD.begin(EthernetShield_CS_SDCard))
    {
    SD.mkdir(ProgmemString(Text_27)); // maak drectory aan waar de Rawsignal HEX bestanden in worden opgeslagen
    SD.mkdir(ProgmemString(Text_28)); // maak drectory aan waar de Rawsignal KEY bestanden in worden opgeslagen
    
    File dataFile = SD.open(ProgmemString(Text_23), FILE_WRITE);
    if (dataFile) 
      {
      dataFile.close();
      SDCardPresent=true; // Als logfile kon worden geopend, dan is alles voor elkaar kan de vlag SDCardPresent op true worden gezet.
      }    
    }
  // SDCard en de W5100 kunnen niet gelijktijdig werken. Selecteer W510 chip
  digitalWrite(Ethernetshield_CS_W5100, LOW);
  digitalWrite(EthernetShield_CS_SDCard,HIGH);
  
      
  //Zorg ervoor dat er niet direct na een boot een CMD_CLOCK_DAYLIGHT event optreedt
  ClockRead();
  SetDaylight();
  DaylightPrevious=Time.Daylight;

  // Zet statussen WIRED_IN op hoog, anders wordt direct wij het opstarten meerdere malen een event gegenereerd omdat de pull-up weerstand analoge de waarden op hoog zet
  for(x=0;x<WIRED_PORTS;x++){WiredInputStatus[x]=true;}

  // Initialiseer ethernet device  ???
  if(Ethernet.begin(Ethernet_MAC_Address)!=0)
    {
    // MAC en IP adres van de Nodo
    EthernetServer EventServer(S.Event_Port);           // Server class voor ontvangst van Events
    EventServer.begin();                                // Start Server voor ontvangst van Events
    EthernetEnabled=true;
    }
    
  PrintWelcome(); // geef de welkomsttekst weer
  SerialConnected=false;
  ProcessEvent(command2event(CMD_BOOT_EVENT,0,0),VALUE_DIRECTION_INTERNAL,CMD_BOOT_EVENT,0,0);  // Voer het 'Boot' event uit.
  }

void loop() 
  {
  int x,y,z;
  int InByte;
  int Pos=0;  
  
  SerialHold(false); // er mogen weer tekens binnen komen van SERIAL

  // hoofdloop: scannen naar signalen
  // dit is een tijdkritische loop die wacht tot binnengekomen event op IR, RF, SERIAL, CLOCK, DAYLIGHT, TIMER
  // als er geen signalen binnenkomen duurt deze hoofdloop +/- 35uSec. snel genoeg om geen signalen te missen.
  while(true)
    {
    if(Hold)
      {
      digitalWrite(MonitorLedPin,(millis()>>7)&0x01);
      // als in de hold-modus met reden het Delay commando EN de tijd is om, dan geneste call naar loop() verlaten.
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

    // Ethernet alleen als verkrijgen van IP adres correct is verlopen
    if(EthernetEnabled)
      {
      // IP (Event) : *************** kijk of er een Event klaar staat op het ethernet shield **********************    
      if(EventGhostReceive(InputBuffer))
        ExecuteLine(InputBuffer, VALUE_SOURCE_EVENTGHOST);

        
      // IP (Terminal) : *************** kijk of er verzoek tot verbinding vanuit een terminal is **********************    
      if(TerminalServer.available())
        {
        if(!TerminalConnected)
          {
          // we hebben een nieuwe Terminal client
          TerminalConnected=true;
          TerminalClient=TerminalServer.available();
          x=S.Terminal_Enabled;
          S.Terminal_Enabled=VALUE_ON;
          PrintWelcome();
          S.Terminal_Enabled=x;
          }
  
        if(TerminalClient.connected() && TerminalClient.available()) // er staat data van de terminal klaar
          {
          if(TerminalReceive(InputBuffer))
            {
            TerminalClient.print("\n\r");
            if(S.Terminal_Enabled==VALUE_ON)
              ExecuteLine(InputBuffer, VALUE_SOURCE_TERMINAL);    
            else
              {
              TerminalClient.println(cmd2str(ERROR_10));
              RaiseError(ERROR_10); 
              }
//???            TerminalClient.print(">");            
            }
          }
        }
      }
      
    // SERIAL: *************** kijk of er data klaar staat op de seriële poort **********************
    if(Serial.available())
      {
      do
        {
        InByte=Serial.read();
        if(isprint(InByte) && Pos<INPUT_BUFFER_SIZE) // alleen de printbare tekens zijn zinvol.
          {
          StaySharpTimer=millis()+SHARP_TIME;      
          InputBuffer[Pos++]=InByte;
          }
        else if(InByte=='\n') 
          {
          SerialHold(true);
          InputBuffer[Pos]=0; // serieel ontvangen regel is compleet
          SerialConnected=true;
          ExecuteLine(InputBuffer, VALUE_SOURCE_SERIAL);
          Pos=0;  
          InputBuffer[0]=0; // serieel ontvangen regel is compleet
          SerialHold(false);
          StaySharpTimer=millis()+SHARP_TIME;      
          }
        }while(Serial.available() || StaySharpTimer>millis());
      }

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
               RawSignal.Source=VALUE_SOURCE_IR;
               CheckRawSignalKey(&Content); // check of er een RawSignal key op de SDCard aanwezig is en vul met Nodo Event. Call by reference!

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
               RawSignal.Source=VALUE_SOURCE_RF;
               CheckRawSignalKey(&Content); // check of er een RawSignal key op de SDCard aanwezig is en vul met Nodo Event. Call by reference!

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
      if(CheckEventlist(Content,VALUE_SOURCE_CLOCK) && EventTimeCodePrevious!=Content)
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
        if(UserVar[x]!=UserVarPrevious[x]) // de eerste gewijzigde variabele
          {
          UserVarPrevious[x]=UserVar[x];
          Content=command2event(CMD_VARIABLE_EVENT,x+1,UserVar[x]);
          ProcessEvent(Content,VALUE_DIRECTION_INTERNAL,VALUE_SOURCE_VARIABLE,0,0);      // verwerk binnengekomen event.
          }
        }
        
      // WIRED: *************** kijk of statussen gewijzigd zijn op WIRED **********************

      // als de huidige waarde groter dan threshold EN de vorige keer was dat nog niet zo DAN verstuur code
      z=false; // vlag om te kijken of er een wijziging is die verzonden moet worden.
      y=analogRead(WiredAnalogInputPin_1+WiredCounter);
     
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

      if(WiredCounter<WIRED_PORTS)
        WiredCounter++;
      else
        WiredCounter=0;
      }// korte interval
    }// // while 
  }
