#define PULSE_DEBOUNCE_TIME           10 // pulsen kleiner dan deze waarde in milliseconden worden niet geteld. Bedoeld om verstoringen a.g.v. ruis of dender te voorkomen
#define PULSE_TRANSITION         FALLING // FALLING of RISING: Geeft aan op welke flank de PulseCounter start start met tellen. Default FALLING
#define I2C_START_ADDRESS              1 // Alle Nodo's op de I2C bus hebben een uniek adres dat start vanaf dit nummer. Er zijn max. 32 Nodo's. Let op overlap met andere devices. RTC zit op adres 104.
#define NODO_30_COMPATIBLE          true // t.b.v. compatibiliteit met vorige Nodo versie: NewKAKU HEX waarden en UserEvents
#define BAUD                       19200 // Baudrate voor seriële communicatie.
#define WAIT_FREE_RX               false // true: wacht default op verzenden van een event tot de IR/RF lijn onbezet is. Wordt overruled door commando [WaitFreeRX]
#define WAIT_FREE_RX_WINDOW          500 // minimale wachttijd wanneer wordt gewacht op een vrije RF of IR band. Is deze waarde te klein, dan kunnen er restanten van signalen binnenkomen als RawSignal. Te groot maakt de Nodo sloom.
#define WAITFREE_TIMEOUT           30000 // tijd in ms. waarna het wachten wordt afgebroken als er geen ruimte in de vrije ether komt
#define PASSWORD_MAX_RETRY             5 // aantal keren dat een gebruiker een foutief wachtwoord mag ingeven alvorens tijdslot in werking treedt
#define PASSWORD_TIMEOUT             300 // aantal seconden dat het terminal venster is geblokkeerd na foutive wachtwoord
#define TERMINAL_TIMEOUT             600 // Aantal seconden dat, na de laatst ontvangen regel, de terminalverbinding open mag staan.
#define DELAY_BETWEEN_TRANSMISSIONS  500 // Minimale tijd tussen verzenden van twee events. Geeft ontvangende apparaten (en Nodo's) verwerkingstijd.
#define NODO_TX_TO_RX_SWITCH_TIME    500 // Tijd die andere Nodo's nodig hebben om na zenden weer gereed voor ontvangst te staan. (Opstarttijd 433RX modules)
#define SIGNAL_ANALYZE_SHARPNESS      50 // Scherpte c.q. foutmarge die gehanteerd wordt bij decoderen van RF/IR signaal.
#define MIN_RAW_PULSES                16 // =8 bits. Minimaal aantal ontvangen bits*2 alvorens cpu tijd wordt besteed aan decodering, etc. Zet zo hoog mogelijk om CPU-tijd te sparen en minder 'onzin' te ontvangen.

#include <SPI.h>
#include <Arduino.h>

#define stringify(x) #x
#define CONFIGFILE2(a, b) stringify(a/Config/b)
#define CONFIGFILE(a, b) CONFIGFILE2(a, b)
#include CONFIGFILE(SKETCH_PATH,CONFIG_FILE)

#define NODO_VERSION         36  // Ophogen bij gewijzigde settings struct of nummering events/commando's. 
#define NODO_BUILD          551  //??? ophogen bij iedere build
#include <EEPROM.h>
#include <Wire.h>

#define CMD_DUMMY                       0
#define EVENT_USEREVENT                 1
#define CMD_ALARM_SET                   2
#define CMD_ANALYSE_SETTINGS            3
#define CMD_BREAK_ON_DAYLIGHT           4
#define CMD_BREAK_ON_TIME_EARLIER       5
#define CMD_BREAK_ON_TIME_LATER         6
#define CMD_BREAK_ON_VAR_EQU            7
#define CMD_BREAK_ON_VAR_LESS           8
#define CMD_BREAK_ON_VAR_LESS_VAR       9
#define CMD_BREAK_ON_VAR_MORE           10
#define CMD_BREAK_ON_VAR_MORE_VAR       11
#define CMD_BREAK_ON_VAR_NEQU           12
#define CMD_CLIENT_IP                   13
#define CMD_CLOCK_DATE                  14
#define CMD_CLOCK_TIME                  15
#define CMD_CLOCK_SYNC                  16
#define CMD_DEBUG                       17
#define CMD_DELAY                       18
#define CMD_ECHO      19
#define CMD_DNS_SERVER                  20
#define CMD_EVENTLIST_ERASE             21
#define CMD_EVENTLIST_FILE              22
#define CMD_EVENTLIST_SHOW              23
#define CMD_EVENTLIST_WRITE             24
#define CMD_FILE_ERASE                  25
#define CMD_FILE_EXECUTE                26
#define CMD_FILE_GET_HTTP               27
#define CMD_FILE_LIST                   28
#define CMD_FILE_LOG                    29
#define CMD_FILE_SHOW                   30
#define CMD_FILE_WRITE                  31
#define CMD_GATEWAY                     32
#define CMD_HOME_SET                    33
#define CMD_HTTP_REQUEST                34
#define CMD_ID                          35
#define CMD_IF                          36
#define CMD_LOCK                        37
#define CMD_LOG                         38
#define CMD_NODO_IP                     39
#define CMD_OUTPUT                      40
#define CMD_PASSWORD                    41
#define CMD_PORT_SERVER                 42
#define CMD_PORT_CLIENT                 43
#define CMD_RAWSIGNAL_ERASE             44
#define CMD_RAWSIGNAL_LIST              45
#define CMD_RAWSIGNAL_RECEIVE           46
#define CMD_RAWSIGNAL_SAVE              47
#define CMD_RAWSIGNAL_SEND              48
#define CMD_REBOOT       49
#define CMD_RECEIVE_SETTINGS            50
#define CMD_RESET                       51
#define CMD_SEND_EVENT                  52
#define CMD_SENDTO                      53
#define CMD_SEND_USEREVENT              54
#define CMD_SETTINGS_SAVE               55
#define CMD_SOUND                       56
#define CMD_STATUS                      57
#define CMD_STOP                        58
#define CMD_SUBNET                      59
#define CMD_TEMP                        60
#define CMD_TIMER_RANDOM                61
#define CMD_TIMER_SET                   62
#define CMD_TIMER_SET_VARIABLE          63
#define CMD_UNIT_SET                    64
#define CMD_VARIABLE_DEC                65
#define CMD_VARIABLE_RECEIVE    66
#define CMD_VARIABLE_INC                67
#define CMD_VARIABLE_PULSE_COUNT        68
#define CMD_VARIABLE_PULSE_TIME         69
#define CMD_VARIABLE_SEND              70
#define CMD_VARIABLE_SET                71
#define CMD_VARIABLE_VARIABLE           72
#define CMD_VARIABLE_SET_WIRED_ANALOG   73
#define CMD_WAIT_EVENT                  74
#define CMD_WAITFREERF                  75
#define CMD_WIRED_ANALOG                76
#define CMD_WIRED_OUT                   77
#define CMD_WIRED_PULLUP                78
#define CMD_WIRED_SMITTTRIGGER          79
#define CMD_WIRED_THRESHOLD             80
#define EVENT_ALARM                     81
#define EVENT_BOOT                      82
#define EVENT_CLOCK_DAYLIGHT            83
#define EVENT_MESSAGE                   84
#define EVENT_NEWNODO                   85
#define EVENT_RAWSIGNAL                 86
#define EVENT_TIME                      87
#define EVENT_TIMER                     88
#define EVENT_VARIABLE                  89
#define EVENT_WILDCARD                  90
#define EVENT_WIRED_IN                  91
#define VALUE_ALL                       92
#define VALUE_BUILD                     93
#define VALUE_DLS                       94
#define VALUE_DEVICE 95
#define VALUE_RECEIVED_EVENT            96
#define VALUE_EVENTLIST          97
#define VALUE_EVENTLIST_COUNT           98
#define VALUE_SOURCE_EVENTLIST               99
#define VALUE_SOURCE_FILE               100
#define VALUE_FREEMEM                   101
#define VALUE_SOURCE_HTTP               102
#define VALUE_HWCONFIG                  103
#define VALUE_SOURCE_I2C                104
#define VALUE_DIRECTION_INPUT           105
#define VALUE_SOURCE_IR                 106
#define VALUE_OFF                       107
#define VALUE_ON                        108
#define VALUE_DIRECTION_OUTPUT          109
#define VALUE_RECEIVED_PAR1             110
#define VALUE_RECEIVED_PAR2             111
#define VALUE_SOURCE_RF                 112
#define VALUE_SOURCE_SERIAL             113
#define VALUE_SOURCE_TELNET             114
#define VALUE_SOURCE_WIRED             115
#define VALUE_SOURCE_THISUNIT                 116
#define VALUE_UNIT                      117
#define MESSAGE_00                      118
#define MESSAGE_01                      119
#define MESSAGE_02                      120
#define MESSAGE_03                      121
#define MESSAGE_04                      122
#define MESSAGE_05                      123
#define MESSAGE_06                      124
#define MESSAGE_07                      125
#define MESSAGE_08                      126
#define MESSAGE_09                      127
#define MESSAGE_10                      128
#define MESSAGE_11                      129
#define MESSAGE_12                      130
#define MESSAGE_13                      131
#define MESSAGE_14                      132
#define MESSAGE_15                      133
#define MESSAGE_16                      134
#define MESSAGE_17                      135
#define MESSAGE_18                      136


#define COMMAND_MAX                     136 // hoogste commando

#if NODO_MEGA
prog_char PROGMEM Cmd_0[]="-";
prog_char PROGMEM Cmd_1[]="UserEvent"; // Vast op positie 1 !
prog_char PROGMEM Cmd_2[]="AlarmSet";
prog_char PROGMEM Cmd_3[]="AnalyseSettings";
prog_char PROGMEM Cmd_4[]="BreakOnDaylight";
prog_char PROGMEM Cmd_5[]="BreakOnEarlier";
prog_char PROGMEM Cmd_6[]="BreakOnLater";
prog_char PROGMEM Cmd_7[]="BreakOnVarEqu";
prog_char PROGMEM Cmd_8[]="BreakOnVarLess";
prog_char PROGMEM Cmd_9[]="BreakOnVarLessVar";
prog_char PROGMEM Cmd_10[]="BreakOnVarMore";
prog_char PROGMEM Cmd_11[]="BreakOnVarMoreVar";
prog_char PROGMEM Cmd_12[]="BreakOnVarNEqu";
prog_char PROGMEM Cmd_13[]="ClientIP";
prog_char PROGMEM Cmd_14[]="ClockSetDate";
prog_char PROGMEM Cmd_15[]="ClockSetTime";
prog_char PROGMEM Cmd_16[]="ClockSync";
prog_char PROGMEM Cmd_17[]="Debug";
prog_char PROGMEM Cmd_18[]="Delay";
prog_char PROGMEM Cmd_19[]="Echo";
prog_char PROGMEM Cmd_20[]="DnsServer";
prog_char PROGMEM Cmd_21[]="EventlistErase";
prog_char PROGMEM Cmd_22[]="EventlistFile";
prog_char PROGMEM Cmd_23[]="EventlistShow";
prog_char PROGMEM Cmd_24[]="EventlistWrite";
prog_char PROGMEM Cmd_25[]="FileErase";
prog_char PROGMEM Cmd_26[]="FileExecute";
prog_char PROGMEM Cmd_27[]="FileGetHTTP";
prog_char PROGMEM Cmd_28[]="FileList";
prog_char PROGMEM Cmd_29[]="FileLog";
prog_char PROGMEM Cmd_30[]="FileShow";
prog_char PROGMEM Cmd_31[]="FileWrite";
prog_char PROGMEM Cmd_32[]="Gateway";
prog_char PROGMEM Cmd_33[]="HomeSet";
prog_char PROGMEM Cmd_34[]="HTTPHost";
prog_char PROGMEM Cmd_35[]="ID";
prog_char PROGMEM Cmd_36[]="if";
prog_char PROGMEM Cmd_37[]="Lock";
prog_char PROGMEM Cmd_38[]="Log";
prog_char PROGMEM Cmd_39[]="IP";
prog_char PROGMEM Cmd_40[]="Output";
prog_char PROGMEM Cmd_41[]="Password";
prog_char PROGMEM Cmd_42[]="PortInput";
prog_char PROGMEM Cmd_43[]="PortOutput";
prog_char PROGMEM Cmd_44[]="RawSignalErase";
prog_char PROGMEM Cmd_45[]="RawSignalList";
prog_char PROGMEM Cmd_46[]="RawSignalReceive";
prog_char PROGMEM Cmd_47[]="RawSignalSave";
prog_char PROGMEM Cmd_48[]="RawSignalSend";
prog_char PROGMEM Cmd_49[]="Reboot";
prog_char PROGMEM Cmd_50[]="ReceiveSettings";
prog_char PROGMEM Cmd_51[]="Reset";
prog_char PROGMEM Cmd_52[]="SendEvent";
prog_char PROGMEM Cmd_53[]="SendTo";
prog_char PROGMEM Cmd_54[]="SendUserEvent";
prog_char PROGMEM Cmd_55[]="SettingsSave";
prog_char PROGMEM Cmd_56[]="Sound";
prog_char PROGMEM Cmd_57[]="Status";
prog_char PROGMEM Cmd_58[]="Stop";
prog_char PROGMEM Cmd_59[]="Subnet";
prog_char PROGMEM Cmd_60[]="Temp";
prog_char PROGMEM Cmd_61[]="TimerRandom";
prog_char PROGMEM Cmd_62[]="TimerSet";
prog_char PROGMEM Cmd_63[]="TimerSetVariable";
prog_char PROGMEM Cmd_64[]="UnitSet";
prog_char PROGMEM Cmd_65[]="VariableDec";
prog_char PROGMEM Cmd_66[]="VariableReceive";
prog_char PROGMEM Cmd_67[]="VariableInc";
prog_char PROGMEM Cmd_68[]="VariablePulseCount";
prog_char PROGMEM Cmd_69[]="VariablePulseTime";
prog_char PROGMEM Cmd_70[]="VariableSend";
prog_char PROGMEM Cmd_71[]="VariableSet";
prog_char PROGMEM Cmd_72[]="VariableSetVariable";
prog_char PROGMEM Cmd_73[]="VariableWiredAnalog";
prog_char PROGMEM Cmd_74[]="WaitEvent";
prog_char PROGMEM Cmd_75[]="WaitFreeRX";
prog_char PROGMEM Cmd_76[]="WiredAnalog";
prog_char PROGMEM Cmd_77[]="WiredOut";
prog_char PROGMEM Cmd_78[]="WiredPullup";
prog_char PROGMEM Cmd_79[]="WiredSmittTrigger";
prog_char PROGMEM Cmd_80[]="WiredThreshold";
prog_char PROGMEM Cmd_81[]="Alarm";
prog_char PROGMEM Cmd_82[]="Boot";
prog_char PROGMEM Cmd_83[]="ClockDaylight";
prog_char PROGMEM Cmd_84[]="Message";
prog_char PROGMEM Cmd_85[]="NewNodo";
prog_char PROGMEM Cmd_86[]="RawSignal";
prog_char PROGMEM Cmd_87[]="Time";
prog_char PROGMEM Cmd_88[]="Timer";
prog_char PROGMEM Cmd_89[]="Variable";
prog_char PROGMEM Cmd_90[]="WildCard";
prog_char PROGMEM Cmd_91[]="WiredIn";
prog_char PROGMEM Cmd_92[]="All";
prog_char PROGMEM Cmd_93[]="Build";
prog_char PROGMEM Cmd_94[]="DaylightSaving";
prog_char PROGMEM Cmd_95[]="Device";
prog_char PROGMEM Cmd_96[]="Event";
prog_char PROGMEM Cmd_97[]="EventList";
prog_char PROGMEM Cmd_98[]="EventlistCount";
prog_char PROGMEM Cmd_99[]="Eventlist";
prog_char PROGMEM Cmd_100[]="File";
prog_char PROGMEM Cmd_101[]="FreeMem";
prog_char PROGMEM Cmd_102[]="HTTP";
prog_char PROGMEM Cmd_103[]="HWConfig";
prog_char PROGMEM Cmd_104[]="I2C";
prog_char PROGMEM Cmd_105[]="Input";
prog_char PROGMEM Cmd_106[]="IR";
prog_char PROGMEM Cmd_107[]="Off";
prog_char PROGMEM Cmd_108[]="On";
prog_char PROGMEM Cmd_109[]="Output";
prog_char PROGMEM Cmd_110[]="Par1";
prog_char PROGMEM Cmd_111[]="Par2";
prog_char PROGMEM Cmd_112[]="RF";
prog_char PROGMEM Cmd_113[]="Serial";
prog_char PROGMEM Cmd_114[]="Terminal";
prog_char PROGMEM Cmd_115[]="Wired";
prog_char PROGMEM Cmd_116[]="ThisUnit";
prog_char PROGMEM Cmd_117[]="Unit";
prog_char PROGMEM Cmd_118[]="Ok.";
prog_char PROGMEM Cmd_119[]="Unknown command.";
prog_char PROGMEM Cmd_120[]="Invalid parameter in command.";
prog_char PROGMEM Cmd_121[]="Unable to open file.";
prog_char PROGMEM Cmd_122[]="Error during queing events.";
prog_char PROGMEM Cmd_123[]="Eventlist nesting error.";
prog_char PROGMEM Cmd_124[]="Reading/writing eventlist failed.";
prog_char PROGMEM Cmd_125[]="Unable to establish TCP/IP connection.";
prog_char PROGMEM Cmd_126[]="Incorrect password.";
prog_char PROGMEM Cmd_127[]="Execution stopped.";
prog_char PROGMEM Cmd_128[]="Access denied.";
prog_char PROGMEM Cmd_129[]="SendTo not completed.";
prog_char PROGMEM Cmd_130[]="Unit not online or within range.";
prog_char PROGMEM Cmd_131[]="Data lost during SendTo.";
prog_char PROGMEM Cmd_132[]="SDCard error.";
prog_char PROGMEM Cmd_133[]="Break.";
prog_char PROGMEM Cmd_134[]="RawSignal saved.";
prog_char PROGMEM Cmd_135[]="Unknown device.";
prog_char PROGMEM Cmd_136[]="Device returned an error.";


// tabel die refereert aan de commando strings
PROGMEM const char *CommandText_tabel[]={
Cmd_0,Cmd_1,Cmd_2,Cmd_3,Cmd_4,Cmd_5,Cmd_6,Cmd_7,Cmd_8,Cmd_9,
Cmd_10,Cmd_11,Cmd_12,Cmd_13,Cmd_14,Cmd_15,Cmd_16,Cmd_17,Cmd_18,Cmd_19,
Cmd_20,Cmd_21,Cmd_22,Cmd_23,Cmd_24,Cmd_25,Cmd_26,Cmd_27,Cmd_28,Cmd_29,
Cmd_30,Cmd_31,Cmd_32,Cmd_33,Cmd_34,Cmd_35,Cmd_36,Cmd_37,Cmd_38,Cmd_39,
Cmd_40,Cmd_41,Cmd_42,Cmd_43,Cmd_44,Cmd_45,Cmd_46,Cmd_47,Cmd_48,Cmd_49,
Cmd_50,Cmd_51,Cmd_52,Cmd_53,Cmd_54,Cmd_55,Cmd_56,Cmd_57,Cmd_58,Cmd_59,
Cmd_60,Cmd_61,Cmd_62,Cmd_63,Cmd_64,Cmd_65,Cmd_66,Cmd_67,Cmd_68,Cmd_69,
Cmd_70,Cmd_71,Cmd_72,Cmd_73,Cmd_74,Cmd_75,Cmd_76,Cmd_77,Cmd_78,Cmd_79,
Cmd_80,Cmd_81,Cmd_82,Cmd_83,Cmd_84,Cmd_85,Cmd_86,Cmd_87,Cmd_88,Cmd_89,
Cmd_90,Cmd_91,Cmd_92,Cmd_93,Cmd_94,Cmd_95,Cmd_96,Cmd_97,Cmd_98,Cmd_99,
Cmd_100,Cmd_101,Cmd_102,Cmd_103,Cmd_104,Cmd_105,Cmd_106,Cmd_107,Cmd_108,Cmd_109,
Cmd_110,Cmd_111,Cmd_112,Cmd_113,Cmd_114,Cmd_115,Cmd_116,Cmd_117,Cmd_118,Cmd_119,
Cmd_120,Cmd_121,Cmd_122,Cmd_123,Cmd_124,Cmd_125,Cmd_126,Cmd_127,Cmd_128,Cmd_129,
Cmd_130,Cmd_131,Cmd_132,Cmd_133,Cmd_134,Cmd_135,Cmd_136};

// Tabel met zonsopgang en -ondergang momenten. afgeleid van KNMI gegevens midden Nederland.
PROGMEM prog_uint16_t Sunrise[]={528,525,516,503,487,467,446,424,401,378,355,333,313,295,279,268,261,259,263,271,283,297,312,329,345,367,377,394,411,428,446,464,481,498,512,522,528,527};
PROGMEM prog_uint16_t Sunset[]={999,1010,1026,1044,1062,1081,1099,1117,1135,1152,1169,1186,1203,1219,1235,1248,1258,1263,1264,1259,1249,1235,1218,1198,1177,1154,1131,1107,1084,1062,1041,1023,1008,996,990,989,993,1004};

// strings met vaste tekst naar PROGMEM om hiermee RAM-geheugen te sparen.
prog_char PROGMEM Text_01[] = "Nodo Domotica controller (c) Copyright 2013 P.K.Tonkes.";
prog_char PROGMEM Text_02[] = "Licensed under GNU General Public License.";
prog_char PROGMEM Text_03[] = "Enter your password: ";
prog_char PROGMEM Text_04[] = "SunMonTueWedThuFriSat";
prog_char PROGMEM Text_05[] = "0123456789abcdef";
prog_char PROGMEM Text_07[] = "Waiting...";
prog_char PROGMEM Text_08[] = "SendTo: Busy Nodo or transmission error. Retry...";
prog_char PROGMEM Text_09[] = "(Last 100 KByte)";
prog_char PROGMEM Text_10[] = "RF/IR claimed by unit %d. Waiting...";
prog_char PROGMEM Text_13[] = "RawSignal saved.";
prog_char PROGMEM Text_14[] = "Event=";
prog_char PROGMEM Text_22[] = "!******************************************************************************!";
prog_char PROGMEM Text_23[] = "LOG.DAT";
prog_char PROGMEM Text_28[] = "RAW"; // Directory op de SDCard voor opslag RawSignal
prog_char PROGMEM Text_30[] = "Terminal connection closed.";

#endif

// omschakeling zomertijd / wintertijd voor komende 10 jaar. één int bevat de omschakeldatum van maart en oktober.
#define DLSBase 2010 // jaar van eerste element uit de array
PROGMEM prog_uint16_t DLSDate[]={2831,2730,2528,3127,3026,2925,2730,2629,2528,3127};

#define DEVICE_MMI_IN                1
#define DEVICE_MMI_OUT               2
#define DEVICE_RAWSIGNAL_IN          3
#define DEVICE_COMMAND               4 
#define DEVICE_INIT                  5
#define DEVICE_ONCE_A_SECOND         6

#define RED                            1 // Led = Rood
#define GREEN                          2 // Led = Groen
#define BLUE                           3 // Led = Blauw
#define UNIT_MAX                      31 // Hoogst mogelijke unit nummer van een Nodo
#define HOME_MAX                       7 // Hoogst mogelijke unit nummer van een Nodo
#define SERIAL_TERMINATOR_1         0x0A // Met dit teken wordt een regel afgesloten. 0x0A is een linefeed <LF>
#define SERIAL_TERMINATOR_2         0x00 // Met dit teken wordt een regel afgesloten. 0x0D is een Carriage Return <CR>, 0x00 = niet in gebruik.
#define Loop_INTERVAL_1               50 // tijdsinterval in ms. voor achtergrondtaken snelle verwerking
#define Loop_INTERVAL_2              250 // tijdsinterval in ms. voor achtergrondtaken langzame verwerking
#define Loop_INTERVAL_3             1000 // tijdsinterval in ms. voor achtergrondtaken langzame verwerking


// Hardware in gebruik: Bits worden geset in de variabele HW_Config, uit te lezen met [Status HWConfig]
#define HW_BIC_0        0
#define HW_BIC_1        1
#define HW_BIC_2        2
#define HW_BIC_3        3
#define HW_BOARD_UNO    4
#define HW_BOARD_MEGA   5
#define HW_ETHERNET     6
#define HW_SDCARD       7
#define HW_SERIAL       8
#define HW_CLOCK        9
#define HW_RF_RX       10
#define HW_IR_RX       11
#define HW_PULSE       12
#define HW_PLUGIN      13
#define HW_I2C         14
#define HW_WEBAPP      15

// Definitie van de speciale hardware uitvoeringen van de Nodo.
#define BIC_DEFAULT                  0  // Standaard Nodo zonder specifike hardware aansturing
#define BIC_HWMESH_NES_V1X           1  // Nodo Ethernet Shield V1.x met Aurel tranceiver. Vereist speciale pulse op PIN_BSF_0 voor omschakelen tussen Rx en Tx.

#define DEVICE_MAX                  32 // Maximaal aantal devices 
#define MACRO_EXECUTION_DEPTH       10 // maximale nesting van macro's.

#if NODO_MEGA // Definities voor de Nodo-Mega variant.
#define EVENT_QUEUE_MAX             16 // maximaal aantal plaatsen in de queue.
#define INPUT_BUFFER_SIZE          128  // Buffer waar de karakters van de seriele/IP poort in worden opgeslagen.
#define TIMER_MAX                   15  // aantal beschikbare timers voor de user, gerekend vanaf 1
#define ALARM_MAX                    8 // aantal alarmen voor de user
#define USER_VARIABLES_MAX          15  // aantal beschikbare gebruikersvariabelen voor de user.
#define PULSE_IRQ                    5  // IRQ verbonden aan de IR_RX_DATA pen 18 van de Mega
#define PIN_BIC_0                   26 // Board Identification Code: bit-0
#define PIN_BIC_1                   27 // Board Identification Code: bit-1
#define PIN_BIC_2                   28 // Board Identification Code: bit-2
#define PIN_BIC_3                   29 // Board Identification Code: bit-3
#define PIN_BSF_0                   22 // Board Specific Function lijn-0
#define PIN_BSF_1                   23 // Board Specific Function lijn-1
#define PIN_BSF_2                   24 // Board Specific Function lijn-2
#define PIN_BSF_3                   25 // Board Specific Function lijn-3
#define PIN_IO_1                    38 // Extra IO-lijn 1 voor gebruikers.
#define PIN_IO_2                    39 // Extra IO-lijn 2 voor gebruikers.
#define PIN_IO_3                    40 // Extra IO-lijn 3 voor gebruikers.
#define PIN_IO_4                    41 // Extra IO-lijn 4 voor gebruikers.
#define PIN_WIRED_IN_1               8 // NIET VERANDEREN. Analoge inputs A8 t/m A15 worden gebruikt voor WiredIn 1 tot en met 8
#define PIN_WIRED_IN_2               9 // NIET VERANDEREN. Analoge inputs A8 t/m A15 worden gebruikt voor WiredIn 1 tot en met 8
#define PIN_WIRED_IN_3              10 // NIET VERANDEREN. Analoge inputs A8 t/m A15 worden gebruikt voor WiredIn 1 tot en met 8
#define PIN_WIRED_IN_4              11 // NIET VERANDEREN. Analoge inputs A8 t/m A15 worden gebruikt voor WiredIn 1 tot en met 8
#define PIN_WIRED_IN_5              12 // NIET VERANDEREN. Analoge inputs A8 t/m A15 worden gebruikt voor WiredIn 1 tot en met 8
#define PIN_WIRED_IN_6              13 // NIET VERANDEREN. Analoge inputs A8 t/m A15 worden gebruikt voor WiredIn 1 tot en met 8
#define PIN_WIRED_IN_7              14 // NIET VERANDEREN. Analoge inputs A8 t/m A15 worden gebruikt voor WiredIn 1 tot en met 8
#define PIN_WIRED_IN_8              15 // NIET VERANDEREN. Analoge inputs A8 t/m A15 worden gebruikt voor WiredIn 1 tot en met 8
#define PIN_LED_RGB_R               47 // RGB-Led, aansluiting rood
#define PIN_LED_RGB_G               48 // RGB-Led, aansluiting groen
#define PIN_LED_RGB_B               49 // RGB-Led, aansluiting blauw
#define PIN_SPEAKER                 42 // luidspreker aansluiting
#define PIN_IR_TX_DATA              17 // NIET VERANDEREN. Aan deze pin zit een zender IR-Led. (gebufferd via transistor i.v.m. hogere stroom die nodig is voor IR-led)
#define PIN_IR_RX_DATA              18 // Op deze input komt het IR signaal binnen van de TSOP. Bij HIGH bij geen signaal.
#define PIN_RF_TX_VCC               15 // +5 volt / Vcc spanning naar de zender.
#define PIN_RF_TX_DATA              14 // data naar de zender
#define PIN_RF_RX_VCC               16 // Spanning naar de ontvanger via deze pin.
#define PIN_RF_RX_DATA              19 // Op deze input komt het 433Mhz-RF signaal binnen. LOW bij geen signaal.
#define PIN_WIRED_OUT_1             30 // 8 digitale outputs D30 t/m D37 worden gebruikt voor WiredIn 1 tot en met 8
#define PIN_WIRED_OUT_2             31 // 8 digitale outputs D30 t/m D37 worden gebruikt voor WiredIn 1 tot en met 8
#define PIN_WIRED_OUT_3             32 // 8 digitale outputs D30 t/m D37 worden gebruikt voor WiredIn 1 tot en met 8
#define PIN_WIRED_OUT_4             33 // 8 digitale outputs D30 t/m D37 worden gebruikt voor WiredIn 1 tot en met 8
#define PIN_WIRED_OUT_5             34 // 8 digitale outputs D30 t/m D37 worden gebruikt voor WiredIn 1 tot en met 8
#define PIN_WIRED_OUT_6             35 // 8 digitale outputs D30 t/m D37 worden gebruikt voor WiredIn 1 tot en met 8
#define PIN_WIRED_OUT_7             36 // 8 digitale outputs D30 t/m D37 worden gebruikt voor WiredIn 1 tot en met 8
#define PIN_WIRED_OUT_8             37 // 8 digitale outputs D30 t/m D37 worden gebruikt voor WiredIn 1 tot en met 8
#define EthernetShield_SCK          52 // NIET VERANDEREN. Ethernet shield: SCK-lijn van de ethernet kaart
#define EthernetShield_CS_SDCardH   53 // NIET VERANDEREN. Ethernet shield: Gereserveerd voor correct funktioneren van de SDCard: Hardware CS/SPI ChipSelect
#define EthernetShield_CS_SDCard     4 // NIET VERANDEREN. Ethernet shield: Chipselect van de SDCard. Niet gebruiken voor andere doeleinden
#define EthernetShield_CS_W5100     10 // NIET VERANDEREN. Ethernet shield: D10..D13  // gereserveerd voor Ethernet & SDCard
#define PIN_CLOCK_SDA               20 // I2C communicatie lijn voor de realtime clock.
#define PIN_CLOCK_SLC               21 // I2C communicatie lijn voor de realtime clock.
#define TERMINAL_PORT               23 // TelNet poort. Standaard 23
#define EEPROM_SIZE               4096 // Groote van het EEPROM geheugen.
#define WIRED_PORTS                  8 // aantal WiredIn/WiredOut poorten
#define COOKIE_REFRESH_TIME         60 // Tijd in sec. tussen automatisch verzenden van een nieuw Cookie als de beveiligde HTTP modus is inschakeld.
#define SERIAL_STAY_SHARP_TIME      50 // Tijd in mSec. dat na ontvangen van een tken uitsluitend naar Serial als input wordt geluisterd. 

#else // als het voor de Nodo-Small variant is
#define EVENT_QUEUE_MAX              8 // maximaal aantal plaatsen in de queue
#define TIMER_MAX                    8 // aantal beschikbare timers voor de user, gerekend vanaf 1
#define USER_VARIABLES_MAX           8 // aantal beschikbare gebruikersvariabelen voor de user.
#define PULSE_IRQ                    1 // IRQ-1 verbonden aan de IR_RX_DATA pen 3 van de ATMega328 (Uno/Nano/Duemillanove)
#define EEPROM_SIZE               1024 // Groote van het EEPROM geheugen.
#define WIRED_PORTS                  4 // aantal WiredIn/WiredOut poorten
#define PIN_LED_RGB_R               13 // RGB-Led, aansluiting rood
#define PIN_LED_RGB_B               13 // RGB-Led, aansluiting blauw, maar voor de Nodo Small is dit de eveneens de rode led.
#define PIN_WIRED_IN_1               0 // Eerste WIRED input pin. Wired-IN loopt van A0 tot en met 3
#define PIN_WIRED_IN_2               1 // Eerste WIRED input pin. Wired-IN loopt van A0 tot en met 3
#define PIN_WIRED_IN_3               2 // Eerste WIRED input pin. Wired-IN loopt van A0 tot en met 3
#define PIN_WIRED_IN_4               3 // Eerste WIRED input pin. Wired-IN loopt van A0 tot en met 3
#define PIN_SPEAKER                  6 // luidspreker aansluiting
#define PIN_IR_TX_DATA              11 // NIET VERANDEREN. Aan deze pin zit een zender IR-Led. (gebufferd via transistor i.v.m. hogere stroom die nodig is voor IR-led)
#define PIN_IR_RX_DATA               3 // Op deze input komt het IR signaal binnen van de TSOP. Bij HIGH bij geen signaal.
#define PIN_RF_TX_VCC                4 // +5 volt / Vcc spanning naar de zender.
#define PIN_RF_TX_DATA               5 // data naar de zender
#define PIN_RF_RX_DATA               2 // Op deze input komt het 433Mhz-RF signaal binnen. LOW bij geen signaal.
#define PIN_RF_RX_VCC               12 // Spanning naar de ontvanger via deze pin.
#define PIN_WIRED_OUT_1              7 // 4 digitale outputs D07 t/m D10 worden gebruikt voor WiredIn 1 tot en met 4
#define PIN_WIRED_OUT_2              8 // 4 digitale outputs D07 t/m D10 worden gebruikt voor WiredIn 1 tot en met 4
#define PIN_WIRED_OUT_3              9 // (pwm) 4 digitale outputs D07 t/m D10 worden gebruikt voor WiredIn 1 tot en met 4
#define PIN_WIRED_OUT_4             10 // (pwm) 4 digitale outputs D07 t/m D10 worden gebruikt voor WiredIn 1 tot en met 4
#endif
//****************************************************************************************************************************************

struct SettingsStruct
  {
  int     Version;        
  byte    Unit; // Max 5 bits in bebruik
  byte    Home; // Max 3 bits in gebruik, wordt voordat er verzonden wordt samen met Unit tot één byte gemaakt.
  boolean NewNodo;
  int     WiredInputThreshold[WIRED_PORTS], WiredInputSmittTrigger[WIRED_PORTS];
  byte    WiredInputPullUp[WIRED_PORTS];
  byte    TransmitIR;
  byte    TransmitRF;
  byte    WaitFree;
  byte    Debug;                                            // Weergeven van extra gegevens t.b.v. beter inzicht verloop van de verwerking
  byte    RawSignalReceive;
  unsigned long Lock;                                       // bevat de pincode waarmee IR/RF ontvangst is geblokkeerd. Bit nummer hoogste bit wordt gebruiktvoor in/uitschakelen.
  
  #if NODO_MEGA
  unsigned long Alarm[ALARM_MAX];                           // Instelbaar alarm
  byte    TransmitIP;                                       // Definitie van het gebruik van HTTP-communicatie via de IP-poort: [Off] of [On]
  char    Password[26];                                     // String met wachtwoord.
  char    ID[10];                                           // ID waar de Nodo uniek mee geïdentificeerd kan worden in een netwerk
  char    HTTPRequest[80];                                  // HTTP request;
  char    Temp[26];                                         // Tijdelijke variabele voor de gebruiker;
  byte    Nodo_IP[4];                                       // IP adres van van de Nodo. als 0.0.0.0 ingevuld, dan IP toekenning o.b.v. DHCP
  byte    Client_IP[4];                                     // IP adres van van de Client die verbinding wil maken met de Nodo, 
  byte    Subnet[4];                                        // Submask
  byte    Gateway[4];                                       // Gateway
  byte    DnsServer[4];                                     // DNS Server IP adres
  unsigned int  OutputPort;                                 // Poort van de inkomende IP communnicatie
  unsigned int  PortClient;                                 // Poort van de uitgaande IP communnicatie
  byte    EchoSerial;
  byte    EchoTelnet;
  byte    Log;
  byte    RawSignalSave;
  #endif
  }Settings;


 // Niet alle gegevens uit een event zijn relevant. Om ruimte in EEPROM te besparen worden uitsluitend
 // de alleen noodzakelijke gegevens in EEPROM opgeslagen. Hiervoor een struct vullen die later als
 // één blok weggeschreven kan worden.

 struct EventlistStruct
   {
   byte EventType;
   byte EventCommand;
   byte EventPar1;
   unsigned long EventPar2;
   
   byte ActionType;
   byte ActionCommand;
   byte ActionPar1;
   unsigned long ActionPar2;
   };


// Een Nodo signaal bestaat uit een 32-bit Event en een 32-bit met meta-data t.b.v. het transport van Nodo naar Nodo.
// In het transport deel bevinden zich de volgende transmissievlaggen:
#define TRANSMISSION_QUEUE          1  // Master => Slave : Event maakt deel uit van een reeks die aan de slave zijde in de queue geplaatst moeten worden alvorens te verwerken.
#define TRANSMISSION_SENDTO         2  // Master => Slave : Event maakt deel uit van een SendTo reeks die in de queue geplaatst moet worden
#define TRANSMISSION_NEXT           4  // Master => Slave : Na dit event volgt direct nog een event.
#define TRANSMISSION_LOCK           8  // Master => Slave : Verzoek om de ether te blokkeren voor exclusieve communicatie tussen master en een slave Nodo.
#define TRANSMISSION_CONFIRM       16  // Master => Slave : Verzoek aan master om bevestiging te sturen na ontvangst.
#define TRANSMISSION_VIEW_ONLY     32  // Master => Slave : Uitsluitend het event weergeven, niet uitvoeren

#define NODO_TYPE_EVENT             1
#define NODO_TYPE_COMMAND           2
#define NODO_TYPE_SYSTEM            3
#define NODO_TYPE_DEVICE_EVENT      4
#define NODO_TYPE_DEVICE_COMMAND    5

// De Nodo kent naast gebruikers commando's en events eveneens Nodo interne events
#define SYSTEM_COMMAND_CONFIRMED  1

  
struct NodoEventStruct
  {
  // Event deel
  byte Type;
  byte Command;
  byte Par1;
  unsigned long Par2;

  // Transmissie deel
  byte SourceUnit;
  byte DestinationUnit;
  byte Flags;
  byte Port;
  byte Direction;
  };

volatile unsigned long PulseCount=0L;                       // Pulsenteller van de IR puls. Iedere hoog naar laag transitie wordt deze teller met één verhoogd
volatile unsigned long PulseTime=0L;                        // Tijdsduur tussen twee pulsen teller in milliseconden: millis()-vorige meting.
boolean RebootNodo=false;                                   // Als deze vlag staat, dan reboot de Nodo (cold-start)
unsigned long HoldTransmission=0L;                          // wachten op dit tijdstip in millis() alvorens event te verzenden.
byte Transmission_SelectedUnit=0;                           // 
byte  Transmission_SendToUnit=0;                            // Unitnummer waar de events naar toe gestuurd worden. 0=alle.
byte  Transmission_SendToAll=0;                             // Vlag die aangeeft of de SendTo permanent staat ingesteld of eenmalig (VALUE_ALL)
boolean Transmission_ThisUnitIsMaster=false;
boolean Transmission_NodoOnly=false;                        // Als deze vlag staat, dan worden er uitsluitend Nodo-eigen signalen ontvangen.  
byte QueuePosition=0;
unsigned long UserTimer[TIMER_MAX];                         // Timers voor de gebruiker.
boolean WiredInputStatus[WIRED_PORTS];                      // Status van de WiredIn worden hierin opgeslagen.
boolean WiredOutputStatus[WIRED_PORTS];                     // Wired variabelen.
byte DaylightPrevious;                                      // t.b.v. voorkomen herhaald genereren van events binnen de lopende minuut waar dit event zich voordoet.
byte ExecutionDepth=0;                                      // teller die bijhoudt hoe vaak er binnen een macro weer een macro wordt uitgevoerd. Voorkomt tevens vastlopers a.g.v. loops die door een gebruiker zijn gemaakt met macro's.
int ExecutionLine=0;                                        // Regel in de eventlist die in uitvoer is.
void(*Reboot)(void)=0;                                      // reset functie op adres 0.
uint8_t RFbit,RFport,IRbit,IRport;                          // t.b.v. verwerking IR/FR signalen.
float UserVar[USER_VARIABLES_MAX];                          // Gebruikers variabelen
unsigned long HW_Config=0;                                  // Hardware configuratie zoals gedetecteerd door de Nodo. 
struct NodoEventStruct LastReceived;                        // Laatst ontvangen event
byte RequestForConfirm=0;                                   // Als ongelijk nul, dan heeft deze Nodo een verzoek ontvangen om een systemevent 'Confirm' te verzenden. Waarde wordt in Par1 meegezonden.
int EventlistMax=0;                                         // beschikbaar aantal regels in de eventlist. Wordt tijdens setup berekend.


// Van alle devices die worden mee gecompileerd, worden in een tabel de adressen opgeslagen zodat
// hier naar toe gesprongen kan worden
void DeviceInit(void);
boolean (*Device_ptr[DEVICE_MAX])(byte, struct NodoEventStruct*, char*);
byte Device_id[DEVICE_MAX];
boolean ExecuteCommand(NodoEventStruct *EventToExecute);//protoype definieren.

#if NODO_MEGA
byte AlarmPrevious[ALARM_MAX];                              // Bevat laatste afgelopen alarm. Ter voorkoming dat alarmen herhaald aflopen.
byte BIC=0;                                                 // Board Identification Code: identificeert de hardware uitvoering van de Nodo
uint8_t MD5HashCode[16];                                    // tabel voor berekenen van MD5 hash codes.
int CookieTimer;                                            // Seconden teller die bijhoudt wanneer er weer een nieuw Cookie naar de WebApp verzonden moet worden.
int TerminalConnected=0;                                    // Vlag geeft aan of en hoe lang nog (seconden) er verbinding is met een Terminal.
int TerminalLocked=1;                                       // 0 als als gebruiker van een telnet terminalsessie juiste wachtwoord heeft ingetoetst
char TempLogFile[13];                                       // Naam van de Logfile waar (naast de standaard logging) de verwerking in gelogd moet worden.
char HTTPCookie[10];                                        // Cookie voor uitwisselen van encrypted events via HTTP
int FileWriteMode=0;                                        // Het aantal seconden dat deze timer ingesteld staat zal er geen verwerking plaats vinden van TerminalInvoer. Iedere seconde --.
char InputBuffer_Serial[INPUT_BUFFER_SIZE+2];               // Buffer voor input Seriele data
char InputBuffer_Terminal[INPUT_BUFFER_SIZE+2];             // Buffer voor input terminal verbinding Telnet sessie
boolean ClockSyncHTTP=false;

// ethernet classes voor IP communicatie Telnet terminal en HTTP.
EthernetServer HTTPServer(80);                              // Server class voor HTTP sessie. Poort wordt later goed gezet.
EthernetClient HTTPClient;                                  // Client calls voor HTTP sessie.
EthernetServer TerminalServer(TERMINAL_PORT);               // Server class voor Terminal sessie.
EthernetClient TerminalClient;                              // Client class voor Terminal sessie.

byte ClientIPAddress[4];                                    // IP adres van de client die verbinding probeert te maken c.q. verbonden is.
byte HTTPClientIP[4];                                       // IP adres van de Host
#endif


// RealTimeclock DS1307
struct RealTimeClock {byte Hour,Minutes,Seconds,Date,Month,Day,Daylight,DaylightSaving,DaylightSavingSetMonth,DaylightSavingSetDate; int Year;}Time; // Hier wordt datum & tijd in opgeslagen afkomstig van de RTC.


#define RAW_BUFFER_SIZE            256                      // Maximaal aantal te ontvangen 128 bits is voldoende voor capture meeste signalen.
struct RawSignalStruct                                      // Variabelen geplaatst in struct zodat deze later eenvoudig kunnen worden weggeschreven naar SDCard
  {
  byte Source;                                              // Bron waar het signaal op is binnengekomen.
  int  Number;                                              // aantal bits, maal twee omdat iedere bit een mark en een space heeft.
  byte Repeats;                                             // Aantal maal dat de pulsreeks verzonden moet worden bij een zendactie.
  byte Delay;                                               // Pauze in ms. na verzenden van één enkele pulsenreeks
  byte Multiply;                                            // Pulses[] * Multiply is de echte tijd van een puls in microseconden
  byte reserved_1;                                          // future use: voorkomt dat bij uitbreiding alle RawSignals niet meer van SDCard te lezen zijn
  byte reserved_2;                                          // future use: voorkomt dat bij uitbreiding alle RawSignals niet meer van SDCard te lezen zijn
  byte reserved_3;                                          // future use: voorkomt dat bij uitbreiding alle RawSignals niet meer van SDCard te lezen zijn
  byte reserved_4;                                          // future use: voorkomt dat bij uitbreiding alle RawSignals niet meer van SDCard te lezen zijn
  byte Pulses[RAW_BUFFER_SIZE+2];                           // Tabel met de gemeten pulsen in microseconden gedeeld door RawSignal.Multiply. Dit scheelt helft aan RAM geheugen.
                                                            // Om legacy redenen zit de eerste puls in element 1. Element 0 wordt dus niet gebruikt.
  }RawSignal={0,0,0,0,0,0,0,0,0};


void setup() 
  {    
  int x;
  struct NodoEventStruct TempEvent;

  Serial.begin(BAUD);  // Initialiseer de seriële poort

  // bereken aantal beschikbare eventlistregels in het eeprom geheugen
  x=(EEPROM_SIZE-sizeof(struct SettingsStruct))/sizeof(struct EventlistStruct);
  EventlistMax=x>255?255:x;

  #if NODO_MEGA
  // initialiseer BIC-lijnen en lees de BIC uit/
  for(x=0;x<=3;x++)
    {
    pinMode(PIN_BIC_0+x,INPUT);
    pinMode(PIN_BIC_0+x,INPUT_PULLUP);
    HW_Config|=digitalRead(PIN_BIC_0+x)<<x;
    }  

  // Zet de alarmen op nog niet afgegaan.
  for(x=0;x<ALARM_MAX;x++)
    AlarmPrevious[x]=0xff; // Deze waarde kan niet bestaan en voldoet dus.
  #endif


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
  digitalWrite(PIN_RF_RX_VCC,HIGH);   // Spanning naar de RF ontvanger aan.
  digitalWrite(PIN_IR_RX_DATA,INPUT_PULLUP);  // schakel pull-up weerstand in om te voorkomen dat er rommel binnenkomt als pin niet aangesloten.
  digitalWrite(PIN_RF_RX_DATA,INPUT_PULLUP);  // schakel pull-up weerstand in om te voorkomen dat er rommel binnenkomt als pin niet aangesloten.
    
  #if NODO_MEGA
  pinMode(PIN_LED_RGB_G,  OUTPUT);
  pinMode(EthernetShield_CS_SDCardH, OUTPUT); // CS/SPI: nodig voor correct funktioneren van de SDCard!

  // Hardware specifieke initialisatie.
  switch(HW_Config&0xf)
    {    
    case BIC_DEFAULT:// Standaard Nodo zonder specifike hardware aansturing
      break;                 

    case BIC_HWMESH_NES_V1X: // Nodo Ethernet Shield V1.x met Aurel tranceiver. Vereist speciale pulse op PIN_BSF_0 voor omschakelen tussen Rx en Tx.
      pinMode(PIN_BSF_0,OUTPUT);
      digitalWrite(PIN_BSF_0,HIGH);
      break;
    }
  #endif

  RFbit=digitalPinToBitMask(PIN_RF_RX_DATA);
  RFport=digitalPinToPort(PIN_RF_RX_DATA);  
  IRbit=digitalPinToBitMask(PIN_IR_RX_DATA);
  IRport=digitalPinToPort(PIN_IR_RX_DATA);

  Led(BLUE);

  ClearEvent(&LastReceived);

  #if NODO_MEGA
  bitWrite(HW_Config,HW_BOARD_MEGA,1);
  Serial.println(F("Booting..."));
  SerialHold(true);// XOFF verzenden zodat PC even wacht met versturen van data via Serial (Xon/Xoff-handshaking)
  #endif

  LoadSettings();      // laad alle settings zoals deze in de EEPROM zijn opgeslagen
  if(Settings.Version!=NODO_VERSION)ResetFactory(); // Als versienummer in EEPROM niet correct is, dan een ResetFactory.

  // initialiseer de Wired ingangen.
  for(x=0;x<WIRED_PORTS;x++)
    {
    if(Settings.WiredInputPullUp[x]==VALUE_ON)
      pinMode(A0+PIN_WIRED_IN_1+x,INPUT_PULLUP);
    else
      pinMode(A0+PIN_WIRED_IN_1+x,INPUT);

    pinMode(PIN_WIRED_OUT_1+x,OUTPUT); // definieer Arduino pin's voor Wired-Out
    }
  for(x=0;x<WIRED_PORTS;x++){WiredInputStatus[x]=true;}


  // Start de I2C poort. 
  Wire.begin(Settings.Unit + I2C_START_ADDRESS);

  //Lees de tijd uit de RTC en zorg ervoor dat er niet direct na een boot een CMD_CLOCK_DAYLIGHT event optreedt
  ClockRead();

  #if NODO_MEGA
  SetDaylight();
  DaylightPrevious=Time.Daylight;

  // SDCard detecteren en evt. gereed maken voor gebruik in de Nodo
  SDCardInit();

  // Start Ethernet kaart en start de HTTP-Server en de Telnet-server
  #if ETHERNET
  // Detecteren of fysieke laag is aangesloten wordt niet ondersteund door de Ethernet Library van Arduino.
  bitWrite(HW_Config,HW_ETHERNET,1); 
  
  if(bitRead(HW_Config,HW_ETHERNET))
    {
    Serial.println(F("Initialising ethernet connection..."));
    if(EthernetInit())
      {
      // als het verkrijgen van een ethernet adres gelukt is en de servers draaien, zet dan de vlag dat ethernet present is
      // Als ethernet enabled en beveiligde modus, dan direct een Cookie sturen, ander worden eerste events niet opgepikt door de WebApp
      if(Settings.Password[0]!=0 && Settings.TransmitIP==VALUE_ON && bitRead(HW_Config,HW_ETHERNET))
        SendHTTPCookie(); // Verzend een nieuw cookie
      }
    else
      {
      // niet gelukt om ethernet verbinding op gang te krijgen
      bitWrite(HW_Config,HW_ETHERNET,0);
      RaiseMessage(MESSAGE_07);
      }
    }
  #endif

  #endif

  Wire.onReceive(ReceiveI2C);   // verwijs naar ontvangstroutine

  // Alle devices moeten aan te roepen zijn vanuit de Devicenummers zoals die in de events worden opgegeven
  // initialiseer de lijst met pointers naar de device funkties.
  DeviceInit();

  bitWrite(HW_Config,HW_I2C,true); // Zet I2C aan zodat het boot event op de I2C-bus wordt verzonden. Hiermee worden bij de andere Nodos de I2C geactiveerd.
    
  // Zend boot event naar alle Nodo's en vraag om een bevestiging. Tevens heeft de gebruiker de gelegenheid om direct na een boot
  // een eventlisterase/reset te verzenden als een small in de problemen.
  // Alle Nodo's die online zenden zullen een Confirm event verzenden.
  ClearEvent(&TempEvent);
  TempEvent.Direction = VALUE_DIRECTION_INPUT;
  TempEvent.Port      = VALUE_ALL;
  TempEvent.Flags     = TRANSMISSION_CONFIRM;
  TempEvent.Type      = NODO_TYPE_EVENT;
  TempEvent.Command   = EVENT_BOOT;
  TempEvent.Par1      = Settings.Unit;
  SendEvent(&TempEvent,false,true,Settings.WaitFree==VALUE_ON);  

  bitWrite(HW_Config,HW_I2C,false); // Zet I2C weer uit. Wordt weer geactiveerd als er een I2C event op de bus verschijnt.

  // Wacht even kort op reacties van andere Nodo's 
  Wait(3, false,0 , false);  

  bitWrite(HW_Config,HW_SERIAL,1); // Serial inschakelen.
  PrintWelcome(); // geef de welkomsttekst weer

  QueueProcess();
  
  // Voer boot-event uit.
  ClearEvent(&TempEvent);
  TempEvent.Port      = VALUE_SOURCE_THISUNIT;
  TempEvent.Direction = VALUE_DIRECTION_INPUT;
  TempEvent.Flags     = TRANSMISSION_CONFIRM;
  TempEvent.Type      = NODO_TYPE_EVENT;
  TempEvent.Command   = EVENT_BOOT;
  TempEvent.Par1      = Settings.Unit;
  TempEvent.Flags     = 0;
  ProcessEvent2(&TempEvent);  // Voer het 'Boot' event uit.

  // Als er nog geen settings aangepast, dan een NewNodo-event sturen.
  if(Settings.NewNodo)
    {
    TempEvent.Port     = VALUE_ALL;
    TempEvent.Type     = NODO_TYPE_EVENT;
    TempEvent.Command  = EVENT_NEWNODO;
    TempEvent.Par1     = Settings.Unit;
    SendEvent(&TempEvent,false,true,false); 
    }
  bitWrite(HW_Config,HW_SERIAL,Serial.available()?1:0); // Serial weer uitschakelen.
  }

void loop() 
  {
  int x,y,z;
  byte SerialInByte;
  byte TerminalInByte;
  int SerialInByteCounter=0;
  int TerminalInbyteCounter=0;
  byte Slice_1=0,Slice_2=0;    
  struct NodoEventStruct ReceivedEvent;
  byte PreviousMinutes=0;                                     // Sla laatst gecheckte minuut op zodat niet vaker dan nodig (eenmaal per minuut de eventlist wordt doorlopen
  
  unsigned long LoopIntervalTimer_1=millis();                 // Timer voor periodieke verwerking. millis() maakt dat de intervallen van 1 en 2 niet op zelfde moment vallen => 1 en 2 nu asynchroon.
  unsigned long LoopIntervalTimer_2=0L;                       // Timer voor periodieke verwerking.
  unsigned long LoopIntervalTimer_3=0L;                       // Timer voor periodieke verwerking.
  unsigned long StaySharpTimer;                               // Timer die er voor zorgt dat bij communicatie via een kanaal de focus hier ook enige tijd op blijft
  unsigned long PreviousTimeEvent=0L; 

  #if NODO_MEGA
  SerialHold(false); // er mogen weer tekens binnen komen van SERIAL
  InputBuffer_Serial[0]=0; // serieel buffer string leeg maken
  TempLogFile[0]=0; // geen extra logging
  #endif

  // hoofdloop: scannen naar signalen
  // dit is een tijdkritische loop die wacht tot binnengekomen event op IR, RF, I2C, SERIAL, CLOCK, DAYLIGHT, TIMER, etc
  // Er is bewust NIET gekozen voor opvangen van signalen via IRQ's omdat er dan communicatie problemen ontstaan.

  while(true)
    {
    // Check voor IR, I2C of RF events
    if(ScanEvent(&ReceivedEvent)) 
      ProcessEvent1(&ReceivedEvent); // verwerk binnengekomen event.
      
    // 1: niet tijdkritische processen die periodiek uitgevoerd moeten worden
    if(LoopIntervalTimer_1<millis())// korte interval
      {
      LoopIntervalTimer_1=millis()+Loop_INTERVAL_1; // reset de timer  

      switch(Slice_1++)
        {
        case 0:  // binnen Slice_1
          {
          Led(GREEN);
          break;
          }
        
        #if NODO_MEGA
        case 1: // binnen Slice_1
          {
          if(bitRead(HW_Config,HW_ETHERNET))
            // IP Event: *************** kijk of er een Event van IP  **********************    
            if(HTTPServer.available())
              ExecuteIP();

          break;
          }
    
        case 2: // binnen Slice_1 
          {
          // IP Telnet verbinding : *************** kijk of er verzoek tot verbinding vanuit een terminal is **********************    
          if(bitRead(HW_Config,HW_ETHERNET))
            {
            if(TerminalServer.available())
              {          
              if(TerminalConnected==0)// indien een nieuwe connectie
                {
                // we hebben een nieuwe Terminal client
                TerminalClient=TerminalServer.available();
                TerminalConnected=TERMINAL_TIMEOUT;
                InputBuffer_Terminal[0]=0;
                TerminalInbyteCounter=0;
                TerminalClient.flush();// eventuele rommel weggooien.

                if(Settings.Password[0]!=0)
                  {
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
                else
                  {
                  TerminalLocked=0;
                  y=bitRead(HW_Config,HW_SERIAL);
                  bitWrite(HW_Config,HW_SERIAL,0);
                  PrintWelcome();
                  bitWrite(HW_Config,HW_SERIAL,y);
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
                  else
                     TerminalClient.write('?');// geen ruimte meer.
                  }

                else if(TerminalInByte==0x0a || TerminalInByte==0x0d)
                  {
                  if(Settings.EchoTelnet==VALUE_ON)
                    TerminalClient.println("");// Echo de nieuwe regel.
                  TerminalConnected=TERMINAL_TIMEOUT;
                  InputBuffer_Terminal[TerminalInbyteCounter]=0;
                  TerminalInbyteCounter=0;
                
                  if(TerminalLocked==0) // als op niet op slot
                    {
                    TerminalClient.getRemoteIP(ClientIPAddress);
                    ExecutionDepth=0;
                    RaiseMessage(ExecuteLine(InputBuffer_Terminal,VALUE_SOURCE_TELNET));
                    TerminalClient.write('>');// prompt
                    }
                  else
                    {
                    if(TerminalLocked<=PASSWORD_MAX_RETRY)// teller is wachtloop bij herhaaldelijke pogingen foutief wachtwoord. Bij >3 pogingen niet meer toegestaan
                      {
                      if(strcmp(InputBuffer_Terminal,Settings.Password)==0)// als wachtwoord goed is, dan slot er af
                        {
                        TerminalLocked=0;
                        y=bitRead(HW_Config,HW_SERIAL);
                        bitWrite(HW_Config,HW_SERIAL,0);
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
                  }
                else 
                  {// bij een niet printbaar teken wordt de verbinding direct verbroken. Uit veiligheidsoverweging om te voorkomen dat bulk rommel naar de Nodo gestuurd wordt.
                  // TerminalSessie timeout, dan de verbinding netjes afsluiten
                  InputBuffer_Terminal[0]=0;
//                  TerminalClient.println(ProgmemString(Text_30));
//                  delay(100); // geef de client even de gelegenheid de tekst te ontvangen
                  TerminalClient.flush();// eventuele rommel weggooien.
//                  TerminalClient.stop();
//                  TerminalConnected=0;
                  break;
                  }//??? test
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
            StaySharpTimer=millis()+SERIAL_STAY_SHARP_TIME;

            while(StaySharpTimer>millis()) // blijf even paraat voor luisteren naar deze poort en voorkom dat andere input deze communicatie onderbreekt
              {          
              while(Serial.available())
                {                        
                SerialInByte=Serial.read();                
                if(Settings.EchoSerial==VALUE_ON)
                  Serial.write(SerialInByte);// echo ontvangen teken
                StaySharpTimer=millis()+SERIAL_STAY_SHARP_TIME;      
                
                if(isprint(SerialInByte))
                  {
                  if(SerialInByteCounter<INPUT_BUFFER_SIZE) // alleen tekens aan de string toevoegen als deze nog in de buffer past.
                    InputBuffer_Serial[SerialInByteCounter++]=SerialInByte;
                  }
                  
                if(SerialInByte=='\n')
                  {
                  SerialHold(true);
                  InputBuffer_Serial[SerialInByteCounter]=0; // serieel ontvangen regel is compleet
                  ExecutionDepth=0;
                  RaiseMessage(ExecuteLine(InputBuffer_Serial,VALUE_SOURCE_SERIAL));
                  Serial.write('>'); // Prompt
                  SerialInByteCounter=0;  
                  InputBuffer_Serial[0]=0; // serieel ontvangen regel is verwerkt. String leegmaken
                  SerialHold(false);
                  StaySharpTimer=millis()+SERIAL_STAY_SHARP_TIME;      
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
          // CLOCK: **************** Check op time events  ***********************          
          if(bitRead(HW_Config,HW_CLOCK) && Time.Minutes!=PreviousMinutes)//Als klok aanwezig en er is een minuut verstreken
            {
            PreviousMinutes=Time.Minutes;
            ClearEvent(&ReceivedEvent);
            ReceivedEvent.Type             = NODO_TYPE_EVENT;
            ReceivedEvent.Command          = EVENT_TIME;
            ReceivedEvent.Par2             = Time.Minutes%10 | (unsigned long)(Time.Minutes/10)<<4 | (unsigned long)(Time.Hour%10)<<8 | (unsigned long)(Time.Hour/10)<<12 | (unsigned long)Time.Day<<16;
            ReceivedEvent.Direction        = VALUE_DIRECTION_INPUT;
            ReceivedEvent.Port             = VALUE_SOURCE_THISUNIT;
            
            if(CheckEventlist(&ReceivedEvent))
              {
              if(PreviousTimeEvent!=ReceivedEvent.Par2)
                {
                ProcessEvent1(&ReceivedEvent); // verwerk binnengekomen event.
                PreviousTimeEvent=ReceivedEvent.Par2; 
                }
              }
            else
              PreviousTimeEvent=0L;
            }
          break;
          }

        case 1:
          {
          #if NODO_MEGA    
          // DAYLIGHT: **************** Check zonsopkomst & zonsondergang  ***********************
          if(bitRead(HW_Config,HW_CLOCK))
            {
            SetDaylight();
            if(Time.Daylight!=DaylightPrevious)// er heeft een zonsondergang of zonsopkomst event voorgedaan
              {
              ClearEvent(&ReceivedEvent);
              ReceivedEvent.Type             = NODO_TYPE_EVENT;
              ReceivedEvent.Command          = EVENT_CLOCK_DAYLIGHT;
              ReceivedEvent.Par1             = Time.Daylight;
              ReceivedEvent.Direction        = VALUE_DIRECTION_INPUT;
              ReceivedEvent.Port             = VALUE_SOURCE_THISUNIT;
              ProcessEvent1(&ReceivedEvent); // verwerk binnengekomen event.
              DaylightPrevious=Time.Daylight;
              }
            }
          #endif
          break;
          }
          
        case 2:
          {
          // WIRED: *************** kijk of statussen gewijzigd zijn op WIRED **********************  
          // als de huidige waarde groter dan threshold EN de vorige keer was dat nog niet zo DAN event genereren
          for(x=0;x<WIRED_PORTS;x++)
            {
            // lees analoge waarde. Dit is een 10-bit waarde, unsigned 0..1023
            y=analogRead(PIN_WIRED_IN_1+x);
            z=false; // verandering
            
            if(!WiredInputStatus[x] && y>(Settings.WiredInputThreshold[x]+Settings.WiredInputSmittTrigger[x]))
              {
              WiredInputStatus[x]=true;
              z=true;
              }
      
            if(WiredInputStatus[x] && y<(Settings.WiredInputThreshold[x]-Settings.WiredInputSmittTrigger[x]))
              {
              WiredInputStatus[x]=false;
              z=true;
              }
            if(z)
              {
              ClearEvent(&ReceivedEvent);
              ReceivedEvent.Type             = NODO_TYPE_EVENT;
              ReceivedEvent.Command          = EVENT_WIRED_IN;
              ReceivedEvent.Par1             = x+1;
              ReceivedEvent.Par2             = WiredInputStatus[x]?VALUE_ON:VALUE_OFF;
              ReceivedEvent.Direction        = VALUE_DIRECTION_INPUT;
              ReceivedEvent.Port             = VALUE_SOURCE_WIRED;
              ProcessEvent1(&ReceivedEvent); // verwerk binnengekomen event.
              }
            }
          break;
          }

        case 3:
          {
          // TIMER: **************** Genereer event als één van de Timers voor de gebruiker afgelopen is ***********************    
          for(x=0;x<TIMER_MAX;x++)
            {
            if(UserTimer[x]!=0L)// als de timer actief is
              {
              if(UserTimer[x]<millis()) // als de timer is afgelopen.
                {
                UserTimer[x]=0L;// zet de timer op inactief.

                ClearEvent(&ReceivedEvent);
                ReceivedEvent.Type             = NODO_TYPE_EVENT;
                ReceivedEvent.Command          = EVENT_TIMER;
                ReceivedEvent.Par1             = x+1;
                ReceivedEvent.Direction        = VALUE_DIRECTION_INPUT;
                ReceivedEvent.Port             = VALUE_SOURCE_THISUNIT;
                ProcessEvent1(&ReceivedEvent); // verwerk binnengekomen event.
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

      // CLOCK: **************** Lees periodiek de realtime klok uit en check op events  ***********************
      ClockRead(); // Lees de Real Time Clock waarden in de struct Time

      // Loopt voor de devices de periodieke aanroep langs.
      for(x=0;x<DEVICE_MAX; x++)
        if(Device_ptr[x]!=0)
          Device_ptr[x](DEVICE_ONCE_A_SECOND,0,0);

      // rebooten van de Nodo is buiten de processing routines geplaatst om zo te voorkomen dat er een reboot van de Arduino
      // plaats vindt terwijl er nog open bestanden of communicatie is. Dit kan mogelijk leiden tot problemen.
      // Op deze plek zijn files gesloten en kan een reboot veilig plaats vinden.
      if(RebootNodo)
        Reboot();

      #if NODO_MEGA
      // ALARM: **************** Genereer event als één van de alarmen afgelopen is ***********************    
      if(bitRead(HW_Config,HW_CLOCK))//check of de klok aanwzig is
        if(ScanAlarm(&ReceivedEvent)) 
          ProcessEvent1(&ReceivedEvent); // verwerk binnengekomen event.

      // Terminal onderhoudstaken
      // tel seconden terug nadat de gebruiker gedefinieerd maal foutief wachtwoord ingegeven
      if(TerminalLocked>PASSWORD_MAX_RETRY)
        if(--TerminalLocked==PASSWORD_MAX_RETRY)TerminalLocked=1;

      if(TerminalConnected)// Sessie duurt nog TerminalConnected seconden. Als 0 dan is er geen verbinding meer
        {
        TerminalConnected--;
        if(!TerminalClient.connected())
          TerminalConnected=0;
        if(!TerminalConnected)
          {
          delay(10);
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
      if(Settings.Password[0]!=0 && Settings.TransmitIP==VALUE_ON)
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
      }
    }// while 
  }


