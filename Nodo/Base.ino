byte dummy=0;
#include <SPI.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <Ethernet.h>
#include <Wire.h>
#include <avr/pgmspace.h>

#define NODO_BUILD                       820                                    // ??? Ophogen bij iedere Build / versiebeheer.
#define PLUGIN_37_COMPATIBILITY        false                                    // Compatibiliteit met plugins die de variabelen tabel UserVar[] nog gebruiken.
#define NODO_VERSION_MINOR                15                                    // Ophogen bij gewijzigde settings struct of nummering events/commando's. 
#define NODO_VERSION_MAJOR                 3                                    // Ophogen bij DataBlock en NodoEventStruct wijzigingen.
#define HOME_NODO                          1                                    // Home adres van Nodo's die tot een groep behoren (1..7). Heeft je buurman ook een Nodo, kies hier dan een ander Home adres

#define MIN_RAW_PULSES                    16                                    // =8 bits. Minimaal aantal ontvangen bits*2 alvorens cpu tijd wordt besteed aan decodering, etc. Zet zo hoog mogelijk om CPU-tijd te sparen en minder 'onzin' te ontvangen.
#define DELAY_BETWEEN_TRANSMISSIONS      100                                    // Minimale tijd tussen verzenden van twee events. Geeft ontvangende apparaten (en Nodo's) verwerkingstijd.
#define SIGNAL_REPEAT_TIME               500                                    // Tijd in mSec. waarbinnen hetzelfde event niet nogmaals via RF/IR mag binnenkomen. Onderdrukt ongewenste herhalingen van signaal
#define RAWSIGNAL_SAMPLE_DEFAULT          25                                    // Sample grootte / Resolutie in uSec waarmee ontvangen Rawsignalen pulsen worden opgeslagen
#define RAWSIGNAL_TX_REPEATS               8                                    // Aantal keer dat een frame met pulsen herhaald wordt verzonden (RawSignalSend)
#define RAWSIGNAL_TX_DELAY                20                                    // Tijd in mSec. tussen herhalingen frames bij zenden. (RawSignalSend)
#define RAWSIGNAL_TOLERANCE              100                                    // Tolerantie die gehanteerd wordt bij decoderen van RF/IR signaal. T.b.v. uitrekenen HEX-code.
#define MIN_PULSE_LENGTH                  50                                    // Pulsen korter dan deze tijd uSec. worden als stoorpulsen beschouwd.
#define NODO_TX_TO_RX_SWITCH_TIME       1000                                    // Tijd die andere Nodo's nodig hebben om na zenden weer gereed voor ontvangst te staan. (Opstarttijd sommige 433RX modules is relatief lang)
#define TRANSMITTER_STABLE_TIME            5                                    // Tijd die de RF zender nodig heeft om na inschakelen van de voedspanning een stabiele draaggolf te hebben.
#define SIGNAL_TIMEOUT                     5                                    // na deze tijd in mSec. wordt een signaal als beeindigd beschouwd.

#define PULSE_DEBOUNCE_TIME               10                                    // pulsen kleiner dan deze waarde in milliseconden worden niet geteld. Bedoeld om verstoringen a.g.v. ruis of dender te voorkomen
#define PULSE_TRANSITION             FALLING                                    // FALLING of RISING: Geeft aan op welke flank de PulseCounter start start met tellen. Default FALLING
#define PASSWORD_MAX_RETRY                 5                                    // aantal keren dat een gebruiker een foutief wachtwoord mag ingeven alvorens tijdslot in werking treedt
#define PASSWORD_TIMEOUT                 300                                    // aantal seconden dat het terminal venster is geblokkeerd na foutive wachtwoord
#define TERMINAL_TIMEOUT                 600                                    // Aantal seconden dat, na de laatst ontvangen regel, de terminalverbinding open mag staan.
#define BROADCAST_TIME                   600                                    // Aantal seconden tussen verzending van het broadcast systeemevent.

#define PLUGIN_MMI_IN                      1
#define PLUGIN_MMI_OUT                     2
#define PLUGIN_RAWSIGNAL_IN                3
#define PLUGIN_COMMAND                     4       
#define PLUGIN_INIT                        5
#define PLUGIN_ONCE_A_SECOND               6
#define PLUGIN_EVENT_IN                    7
#define PLUGIN_EVENT_OUT                   8      
#define PLUGIN_SERIAL_IN                   9
#define PLUGIN_ETHERNET_IN                11

#define RED                                1                                    // Led = Rood
#define GREEN                              2                                    // Led = Groen
#define BLUE                               3                                    // Led = Blauw
#define UNIT_MAX                          32                                    // Hoogst mogelijke unit nummer van een Nodo
#define SERIAL_TERMINATOR_1             0x0A                                    // Met dit teken wordt een regel afgesloten. 0x0A is een linefeed <LF>
#define SERIAL_TERMINATOR_2             0x00                                    // Met dit teken wordt een regel afgesloten. 0x0D is een Carriage Return <CR>, 0x00 = niet in gebruik.
#define SCAN_HIGH_TIME                    50                                    // tijdsinterval in ms. voor achtergrondtaken snelle verwerking
#define SCAN_LOW_TIME                   1000                                    // tijdsinterval in ms. voor achtergrondtaken langzame verwerking
#define PLUGIN_MAX                        24                                    // Maximaal aantal devices 
#define MACRO_EXECUTION_DEPTH             10                                    // maximale nesting van macro's.
#define XON                             0x11                                    // Seriale communicatie XON/XOFF handshaking
#define XOFF                            0x13                                    // Seriale communicatie XON/XOFF handshaking
#define USER_VARIABLES_MAX_NR            255                                    // Hoogste te gebruiken variabelenummer.
#define INPUT_LINE_SIZE                  128                                    // Buffer waar de karakters van de seriele/IP poort in worden opgeslagen.
#define INPUT_COMMAND_SIZE                80                                    // Maximaal aantal tekens waar een commando uit kan bestaan.
#define FOCUS_TIME                       500                                    // Tijd in mSec. dat na ontvangen van een teken uitsluitend naar Serial als input wordt geluisterd. 


// Default alle hardware voorzieningen op false. In de gekozen HW-xxxx.h bestend
// worden de settings later correct ingesteld.
#define HARDWARE_WIRED_IN_PORTS            false
#define HARDWARE_WIRED_OUT_PORTS           false
#define HARDWARE_STATUS_LED_RGB            false
#define HARDWARE_STATUS_LED                false
#define HARDWARE_SPEAKER                   false
#define HARDWARE_PULSE                     false
#define HARDWARE_BATTERY                   false
#define HARDWARE_RAWSIGNAL                 false
#define HARDWARE_INFRARED                  false
#define HARDWARE_RF433                     false
#define HARDWARE_SERIAL_1                  false
#define HARDWARE_SERIAL_2_SW               false
#define HARDWARE_SERIAL_2_HW               false
#define HARDWARE_CLOCK                     false
#define HARDWARE_I2C                       false
#define HARDWARE_SDCARD                    false
#define HARDWARE_ETHERNET                  false
#define HARDWARE_SPI_SOFTWARE              false
#define HARDWARE_SPI_HARDWARE              false
#define HARDWARE_NRF24L01                  false
#define HARDWARE_BUTTONS                   false

#define HW_WIRED_IN_PORTS                      0
#define HW_WIRED_OUT_PORTS                     1
#define HW_STATUS_LED_RGB                      2
#define HW_STATUS_LED                          3
#define HW_SPEAKER                             4
#define HW_PULSE                               5
#define HW_BATTERY                             6
#define HW_RAWSIGNAL                           7
#define HW_INFRARED                            8
#define HW_RF433                               9
#define HW_SERIAL_1                           10
#define HW_SERIAL_2_SW                        11
#define HW_SERIAL_2_HW                        12
#define HW_CLOCK                              13
#define HW_I2C                                14
#define HW_SDCARD                             15
#define HW_ETHERNET                           16
#define HW_SPI_SOFTWARE                       17
#define HW_SPI_HARDWARE                       18
#define HW_NRF24L01                           19
#define HW_BUTTONS                            20
#define HW_NODO_MEGA                          30
#define HW_NODO_SMALL                         31


// t.b.v. includen Config_xx.c files
#define  stringify(x) #x
#define  CONFIGFILE2(a, b) stringify(a/Config/b)
#define  CONFIGFILE(a, b) CONFIGFILE2(a, b)
#include CONFIGFILE(SKETCH_PATH,CONFIG_FILE)

#define  stringify(x) #x
#define  HARDWAREFILE2(a, b) stringify(a/Hardware/HW-b.h)
#define  HARDWAREFILE(a, b) HARDWAREFILE2(a, b)
#include HARDWAREFILE(SKETCH_PATH,HARDWARE_CONFIG)

// Tweede maal includen zodat HARDWARE_xxxx overruled regels worden geset.
#define  stringify(x) #x
#define  CONFIGFILE2(a, b) stringify(a/Config/b)
#define  CONFIGFILE(a, b) CONFIGFILE2(a, b)
#include CONFIGFILE(SKETCH_PATH,CONFIG_FILE)

#if HARDWARE_ETHERNET
#define TERMINAL_PORT               23                                          // TelNet poort. Standaard 23
#define COOKIE_REFRESH_TIME         60                                          // Tijd in sec. tussen automatisch verzenden van een nieuw Cookie als de beveiligde HTTP modus is inschakeld.
#define  ETHERNET_LIB <Ethernet.h>
#define  ETHERNETLIB(b) ETHERNET_LIB
#include ETHERNETLIB(ETHERNET_LIB)
EthernetServer IPServer(80);                                                    // Server class voor HTTP sessie. Poort wordt later goed gezet.
EthernetClient IPClient;                                                        // Client calls voor HTTP sessie.
EthernetServer TerminalServer(TERMINAL_PORT);                                   // Server class voor Terminal sessie.
EthernetClient TerminalClient;                                                  // Client class voor Terminal sessie.
//???byte ClientIPAddress[4];                                                        // IP adres van de client die verbinding probeert te maken c.q. verbonden is.
//???byte IPClientIP[4];                                                             // IP adres van de Host
#endif


#if NODO_MEGA // Definities voor de Nodo-Mega variant.
#define USER_VARIABLES_MAX          32                                          // aantal beschikbare gebruikersvariabelen voor de user.
#define EVENT_QUEUE_MAX             16                                          // maximaal aantal plaatsen in de queue.
#define TIMER_MAX                   15                                          // aantal beschikbare timers voor de user, gerekend vanaf 1
#define ALARM_MAX                    8                                          // aantal alarmen voor de user

#else // als het voor de Nodo-Small variant is
#define USER_VARIABLES_MAX          16                                          // aantal beschikbare gebruikersvariabelen voor de user.
#define EVENT_QUEUE_MAX              8                                          // maximaal aantal plaatsen in de queue
#define TIMER_MAX                    8                                          // aantal beschikbare timers voor de user, gerekend vanaf 1
#endif // NODO_MEGA


// Onderstaande commando codes mogen worden gebruikt door andere devices dan een Nodo.
// Uit compatibility overwegingen zullen deze commando codes niet worden aangepast bij 
// vervolgreleases. 

#define CMD_DUMMY                       0
#define EVENT_BOOT                      1
#define CMD_SOUND                       2
#define EVENT_USEREVENT                 3
#define EVENT_VARIABLE                  4
#define RESERVED_3                      5
#define RESERVED 4                      6
#define RESERVED_5                      7
#define RESERVED_6                      8
#define RESERVED_09                     9
#define RESERVED_10                     10
#define RESERVED_11                     11
#define RESERVED_12                     12
#define RESERVED_13                     13
#define RESERVED_14                     14
#define RESERVED_15                     15
#define COMMAND_MAX_FIXED               15

// Onderstaande commando codes kunnen bij vervolgreleases worden aangepast. Deze codes dus niet buiten de Nodo
// code gebruiken.

#define EVENT_ALARM                     16
#define CMD_ALARM_SET                   17
#define VALUE_ALL                       18                                      
#define CMD_ANALYSE_SETTINGS            19
#define CMD_BREAK_ON_DAYLIGHT           20
#define CMD_BREAK_ON_TIME_EARLIER       21
#define CMD_BREAK_ON_TIME_LATER         22
#define CMD_BREAK_ON_VAR_EQU            23
#define CMD_BREAK_ON_VAR_LESS           24
#define CMD_BREAK_ON_VAR_LESS_VAR       25
#define CMD_BREAK_ON_VAR_MORE           26
#define CMD_BREAK_ON_VAR_MORE_VAR       27
#define CMD_BREAK_ON_VAR_NEQU           28
#define CMD_RES_29                      29                                      // ??? reserve
#define CMD_RES_30                      30
#define VALUE_SOURCE_CLOCK              31
#define EVENT_CLOCK_DAYLIGHT            32
#define CMD_CLOCK_DATE                  33
#define CMD_CLOCK_TIME                  34
#define CMD_CLOCK_SYNC                  35
#define VALUE_DLS                       36
#define CMD_DEBUG                       37
#define CMD_DELAY                       38
#define VALUE_SOURCE_PLUGIN             39
#define CMD_DNS_SERVER                  40
#define CMD_ECHO                        41
#define VALUE_RECEIVED_EVENT            42
#define VALUE_EVENTLIST                 43
#define VALUE_SOURCE_EVENTLIST          44
#define VALUE_EVENTLIST_COUNT           45
#define CMD_EVENTLIST_ERASE             46
#define CMD_EVENTLIST_FILE              47
#define CMD_EVENTLIST_SHOW              48
#define CMD_EVENTLIST_WRITE             49
#define VALUE_SOURCE_FILE               50
#define CMD_FILE_ERASE                  51
#define CMD_FILE_EXECUTE                52
#define CMD_FILE_GET_HTTP               53
#define CMD_FILE_LIST                   54
#define CMD_RES_55                      55                                      //??? reserve
#define CMD_FILE_SHOW                   56
#define CMD_FILE_WRITE                  57
#define VALUE_FREEMEM                   58
#define CMD_GATEWAY                     59
#define VALUE_HWCONFIG                  60 
#define VALUE_HWSTATUS                  61
#define VALUE_SOURCE_HTTP               62
#define CMD_HTTP_REQUEST                63
#define VALUE_SOURCE_I2C                64
#define CMD_ID                          65
#define CMD_IF                          66
#define VALUE_DIRECTION_INPUT           67
#define CMD_NODO_IP                     68
#define VALUE_SOURCE_IR                 69
#define EVENT_WILDCARD                  70
#define CMD_LOG                         71
#define EVENT_MESSAGE                   72
#define VALUE_SOURCE_NRF24L01           73
#define VALUE_OFF                       74
#define VALUE_ON                        75
#define CMD_OUTPUT                      76                                      // ??? vervalt na implementatie WebSocket. 
#define VALUE_DIRECTION_OUTPUT          77
#define VALUE_RECEIVED_PAR1             78
#define VALUE_RECEIVED_PAR2             79
#define CMD_PASSWORD                    80
#define CMD_PORT_INPUT                  81
#define CMD_PORT_OUTPUT                 82
#define EVENT_RAWSIGNAL                 83
#define CMD_RAWSIGNAL_ERASE             84
#define CMD_RAWSIGNAL_LIST              85
#define CMD_RAWSIGNAL_RECEIVE           86
#define CMD_RAWSIGNAL_SAVE              87
#define CMD_RAWSIGNAL_SEND              88
#define CMD_REBOOT                      89
#define CMD_RECEIVE_SETTINGS            90
#define CMD_RESET                       91
#define VALUE_SOURCE_RF                 92
#define CMD_SEND_EVENT                  93
#define CMD_SENDTO                      94
#define CMD_SEND_USEREVENT              95
#define VALUE_SOURCE_SERIAL             96
#define CMD_SETTINGS_SAVE               97
#define CMD_STATUS                      98
#define CMD_VARIABLE_SAVE               99
#define CMD_SUBNET                      100
#define VALUE_SOURCE_SYSTEM             101
#define CMD_TEMP                        102
#define VALUE_SOURCE_TELNET             103
#define EVENT_TIME                      104
#define EVENT_TIMER                     105
#define CMD_TIMER_RANDOM                106
#define CMD_TIMER_SET                   107
#define CMD_TIMER_SET_VARIABLE          108
#define CMD_WIRED_OUT_VARIABLE          109
#define CMD_VARIABLE_PAYLOAD            110
#define CMD_VARIABLE_DEC                111
#define CMD_VARIABLE_INC                112
#define CMD_VARIABLE_PULSE_COUNT        113
#define CMD_VARIABLE_PULSE_TIME         114
#define CMD_VARIABLE_SET                115
#define CMD_VARIABLE_SEND               116
#define CMD_VARIABLE_VARIABLE           117
#define CMD_VARIABLE_SET_WIRED_ANALOG   118
#define CMD_VARIABLE_LOG                119
#define CMD_VARIABLE_GLOBAL             120
#define CMD_VARIABLE_TOGGLE             121
#define VALUE_SOURCE_WIRED              122
#define VALUE_WIRED_ANALOG              123
#define EVENT_WIRED_IN                  124
#define CMD_WIRED_OUT                   125
#define CMD_WIRED_PULLUP                126
#define CMD_WIRED_SMITTTRIGGER          127
#define CMD_WIRED_THRESHOLD             128
#define CMD_FILE_WRITE_LINE             129
#define CMD_RAWSIGNAL_SAMPLE            130
#define CMD_RAWSIGNAL_SHOW              131
#define CMD_RAWSIGNAL_REPEATS           132
#define CMD_RAWSIGNAL_DELAY             133
#define CMD_RAWSIGNAL_PULSES            134
#define CMD_RAWSIGNAL_CLEANUP           135
#define CMD_ALIAS_WRITE                 136
#define CMD_ALIAS_ERASE                 137
#define CMD_ALIAS_SHOW                  138
#define CMD_ALIAS_LIST                  139
#define CMD_POWERSAVE                   140
#define VALUE_BUILD                     141
#define CMD_STOP                        142
#define CMD_SLEEP                       143   
#define VALUE_FAST                      144
#define CMD_WAIT_FREE_NODO              145
#define CMD_BUSY                        146                                
#define VALUE_TOGGLE                    147
#define VALUE_UNIT                      148                                     
#define CMD_UNIT_SET                    149
#define COMMAND_MAX                     149                                     // hoogste commando

#define MESSAGE_OK                      0
#define MESSAGE_UNKNOWN_COMMAND         1
#define MESSAGE_INVALID_PARAMETER       2
#define MESSAGE_UNABLE_OPEN_FILE        3
#define MESSAGE_NESTING_ERROR           4
#define MESSAGE_EVENTLIST_FAILED        5
#define MESSAGE_TCPIP_FAILED            6
#define MESSAGE_EXECUTION_STOPPED       7
#define MESSAGE_ACCESS_DENIED           8
#define MESSAGE_SENDTO_ERROR            9
#define MESSAGE_SDCARD_ERROR            10
#define MESSAGE_BREAK                   11
#define MESSAGE_RAWSIGNAL_SAVED         12 
#define MESSAGE_PLUGIN_UNKNOWN          13
#define MESSAGE_PLUGIN_ERROR            14
#define MESSAGE_VERSION_ERROR           15
#define MESSAGE_BUSY_TIMEOUT            16
#define MESSAGE_NODO_NOT_FOUND          17
#define MESSAGE_VARIABLE_ERROR          18
#define MESSAGE_HARDWARE_ERROR          19
#define MESSAGE_FUTURE_20               20 //??? FUTURE
#define MESSAGE_MAX                     20                                      // laatste bericht tekst

// ??? const char phi_prompt_lcd_ch0[] PROGMEM ={ 4,14,
// ??? prog_char PROGMEM Cmd_1[] ="Boot";

#if NODO_MEGA
const char Cmd_0[]   PROGMEM ="-";
const char Cmd_1[]   PROGMEM ="Boot";
const char Cmd_2[]   PROGMEM ="Sound";
const char Cmd_3[]   PROGMEM ="UserEvent";
const char Cmd_4[]   PROGMEM ="Variable";
const char Cmd_5[]   PROGMEM ="";
const char Cmd_6[]   PROGMEM ="";
const char Cmd_7[]   PROGMEM ="";
const char Cmd_8[]   PROGMEM ="";
const char Cmd_9[]   PROGMEM ="";
const char Cmd_10[]  PROGMEM ="";
const char Cmd_11[]  PROGMEM ="";
const char Cmd_12[]  PROGMEM ="";
const char Cmd_13[]  PROGMEM ="";
const char Cmd_14[]  PROGMEM ="";
const char Cmd_15[]  PROGMEM ="";
const char Cmd_16[]  PROGMEM ="Alarm";
const char Cmd_17[]  PROGMEM ="AlarmSet";
const char Cmd_18[]  PROGMEM ="All";
const char Cmd_19[]  PROGMEM ="AnalyseSettings";
const char Cmd_20[]  PROGMEM ="BreakOnDaylight";
const char Cmd_21[]  PROGMEM ="BreakOnEarlier";
const char Cmd_22[]  PROGMEM ="BreakOnLater";
const char Cmd_23[]  PROGMEM ="BreakOnVarEqu";
const char Cmd_24[]  PROGMEM ="BreakOnVarLess";
const char Cmd_25[]  PROGMEM ="BreakOnVarLessVar";
const char Cmd_26[]  PROGMEM ="BreakOnVarMore";
const char Cmd_27[]  PROGMEM ="BreakOnVarMoreVar";
const char Cmd_28[]  PROGMEM ="BreakOnVarNEqu";
const char Cmd_29[]  PROGMEM ="";                                                  // ??? reserve
const char Cmd_30[]  PROGMEM ="ClientIP";
const char Cmd_31[]  PROGMEM ="Clock";
const char Cmd_32[]  PROGMEM ="ClockDaylight";
const char Cmd_33[]  PROGMEM ="ClockSetDate";
const char Cmd_34[]  PROGMEM ="ClockSetTime";
const char Cmd_35[]  PROGMEM ="ClockSync";
const char Cmd_36[]  PROGMEM ="DaylightSaving";
const char Cmd_37[]  PROGMEM ="Debug";
const char Cmd_38[]  PROGMEM ="Delay";
const char Cmd_39[]  PROGMEM ="Plugin";
const char Cmd_40[]  PROGMEM ="DnsServer";
const char Cmd_41[]  PROGMEM ="Echo";
const char Cmd_42[]  PROGMEM ="Event";
const char Cmd_43[]  PROGMEM ="EventList";                                         // VALUE_EVENTLIST
const char Cmd_44[]  PROGMEM ="Eventlist";                                         // VALUE_SOURCE_EVENTLIST
const char Cmd_45[]  PROGMEM ="EventlistCount";
const char Cmd_46[]  PROGMEM ="EventlistErase";
const char Cmd_47[]  PROGMEM ="EventlistFile";
const char Cmd_48[]  PROGMEM ="EventlistShow";
const char Cmd_49[]  PROGMEM ="EventlistWrite";
const char Cmd_50[]  PROGMEM ="File";
const char Cmd_51[]  PROGMEM ="FileErase";
const char Cmd_52[]  PROGMEM ="FileExecute";
const char Cmd_53[]  PROGMEM ="FileGetHTTP";
const char Cmd_54[]  PROGMEM ="FileList";
const char Cmd_55[]  PROGMEM ="";                                                  //??? reserve
const char Cmd_56[]  PROGMEM ="FileShow";
const char Cmd_57[]  PROGMEM ="FileWrite";
const char Cmd_58[]  PROGMEM ="FreeMem";
const char Cmd_59[]  PROGMEM ="Gateway";
const char Cmd_60[]  PROGMEM ="HWConfig";
const char Cmd_61[]  PROGMEM ="HWStatus";
const char Cmd_62[]  PROGMEM ="HTTP";
const char Cmd_63[]  PROGMEM ="HTTPHost";
const char Cmd_64[]  PROGMEM ="I2C";
const char Cmd_65[]  PROGMEM ="ID";
const char Cmd_66[]  PROGMEM ="if";
const char Cmd_67[]  PROGMEM ="Input";
const char Cmd_68[]  PROGMEM ="IP";
const char Cmd_69[]  PROGMEM ="IR";
const char Cmd_70[]  PROGMEM ="WildCard";                                          
const char Cmd_71[]  PROGMEM ="Log";
const char Cmd_72[]  PROGMEM ="Message";
const char Cmd_73[]  PROGMEM ="RF24";
const char Cmd_74[]  PROGMEM ="Off";
const char Cmd_75[]  PROGMEM ="On";
const char Cmd_76[]  PROGMEM ="Output";                                                  // ??? Vervalt na implementatie WebSocket
const char Cmd_77[]  PROGMEM ="Output";
const char Cmd_78[]  PROGMEM ="Par1";
const char Cmd_79[]  PROGMEM ="Par2";
const char Cmd_80[]  PROGMEM ="Password";
const char Cmd_81[]  PROGMEM ="PortInput";
const char Cmd_82[]  PROGMEM ="PortOutput";
const char Cmd_83[]  PROGMEM ="RawSignal";
const char Cmd_84[]  PROGMEM ="RawSignalErase";
const char Cmd_85[]  PROGMEM ="RawSignalList";
const char Cmd_86[]  PROGMEM ="RawSignalReceive";
const char Cmd_87[]  PROGMEM ="RawSignalWrite";
const char Cmd_88[]  PROGMEM ="RawSignalSend";
const char Cmd_89[]  PROGMEM ="Reboot";
const char Cmd_90[]  PROGMEM ="ReceiveSettings";
const char Cmd_91[]  PROGMEM ="Reset";
const char Cmd_92[]  PROGMEM ="RF";
const char Cmd_93[]  PROGMEM ="EventSend";
const char Cmd_94[]  PROGMEM ="SendTo";
const char Cmd_95[]  PROGMEM ="UserEventSend";
const char Cmd_96[]  PROGMEM ="Serial";
const char Cmd_97[]  PROGMEM ="SettingsSave";
const char Cmd_98[]  PROGMEM ="Status";
const char Cmd_99[]  PROGMEM ="VariableSave";
const char Cmd_100[] PROGMEM ="Subnet";
const char Cmd_101[] PROGMEM ="System";
const char Cmd_102[] PROGMEM ="Temp";
const char Cmd_103[] PROGMEM ="Terminal";
const char Cmd_104[] PROGMEM ="Time";
const char Cmd_105[] PROGMEM ="Timer";
const char Cmd_106[] PROGMEM ="TimerSetRandom";
const char Cmd_107[] PROGMEM ="TimerSet";
const char Cmd_108[] PROGMEM ="TimerSetVariable";
const char Cmd_109[] PROGMEM ="WiredOutVariable";
const char Cmd_110[] PROGMEM ="VariablePayload";
const char Cmd_111[] PROGMEM ="VariableDec";
const char Cmd_112[] PROGMEM ="VariableInc";
const char Cmd_113[] PROGMEM ="VariablePulseCount";
const char Cmd_114[] PROGMEM ="VariablePulseTime";
const char Cmd_115[] PROGMEM ="VariableSet";
const char Cmd_116[] PROGMEM ="VariableSend";
const char Cmd_117[] PROGMEM ="VariableSetVariable";
const char Cmd_118[] PROGMEM ="VariableWiredAnalog";
const char Cmd_119[] PROGMEM ="VariableLog";
const char Cmd_120[] PROGMEM ="VariableGlobal";
const char Cmd_121[] PROGMEM ="VariableToggle";
const char Cmd_122[] PROGMEM ="Wired";
const char Cmd_123[] PROGMEM ="WiredAnalog";
const char Cmd_124[] PROGMEM ="WiredIn";
const char Cmd_125[] PROGMEM ="WiredOut";
const char Cmd_126[] PROGMEM ="WiredPullup";
const char Cmd_127[] PROGMEM ="WiredSmittTrigger";
const char Cmd_128[] PROGMEM ="WiredThreshold";
const char Cmd_129[] PROGMEM ="FileWriteLine";
const char Cmd_130[] PROGMEM ="RawSignalSample";
const char Cmd_131[] PROGMEM ="RawSignalShow";
const char Cmd_132[] PROGMEM ="RawSignalRepeats";
const char Cmd_133[] PROGMEM ="RawSignalDelay";
const char Cmd_134[] PROGMEM ="RawSignalPulses";
const char Cmd_135[] PROGMEM ="RawSignalClean";
const char Cmd_136[] PROGMEM ="AliasWrite";
const char Cmd_137[] PROGMEM ="AliasErase";
const char Cmd_138[] PROGMEM ="AliasShow";
const char Cmd_139[] PROGMEM ="AliasList";
const char Cmd_140[] PROGMEM ="PowerSave";
const char Cmd_141[] PROGMEM ="Build";                                             
const char Cmd_142[] PROGMEM ="Stop";
const char Cmd_143[] PROGMEM ="Sleep";
const char Cmd_144[] PROGMEM ="Fast";
const char Cmd_145[] PROGMEM ="WaitBusyNodo"; 
const char Cmd_146[] PROGMEM ="Busy";
const char Cmd_147[] PROGMEM ="Toggle"; 
const char Cmd_148[] PROGMEM ="Unit"; 
const char Cmd_149[] PROGMEM ="UnitSet";
                     
                     
// tabel die refereert aan de commando strings
// PROGMEM const char *CommandText_tabel[]={ ???


const char* const CommandText_tabel[] PROGMEM = {
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
Cmd_130,Cmd_131,Cmd_132,Cmd_133,Cmd_134,Cmd_135,Cmd_136,Cmd_137,Cmd_138,Cmd_139,
Cmd_140,Cmd_141,Cmd_142,Cmd_143,Cmd_144,Cmd_145,Cmd_146,Cmd_147,Cmd_148,Cmd_149};

// Message max. 40 pos.      "1234567890123456789012345678901234567890"
const char Msg_0[] PROGMEM = "Ok.";
const char Msg_1[] PROGMEM = "Unknown command.";
const char Msg_2[] PROGMEM = "Invalid parameter in command.";
const char Msg_3[] PROGMEM = "Unable to open.";
const char Msg_4[] PROGMEM = "Nesting error.";
const char Msg_5[] PROGMEM = "Reading/writing eventlist.";
const char Msg_6[] PROGMEM = "Unable to establish TCP/IP connection.";
const char Msg_7[] PROGMEM = "Execution stopped.";
const char Msg_8[] PROGMEM = "Access denied.";
const char Msg_9[] PROGMEM = "Communication error.";
const char Msg_10[]PROGMEM = "SDCard error.";
const char Msg_11[]PROGMEM = "Break.";
const char Msg_12[]PROGMEM = "RawSignal saved.";
const char Msg_13[]PROGMEM = "Unknown plugin.";
const char Msg_14[]PROGMEM = "Plugin returned an error.";
const char Msg_15[]PROGMEM = "Incompatibel Nodo event.";
const char Msg_16[]PROGMEM = "Timeout on busy Nodo.";
const char Msg_17[]PROGMEM = "Nodo not found.";
const char Msg_18[]PROGMEM = "Variable error.";
const char Msg_19[]PROGMEM = "Hardware configuration error(s).";
const char Msg_20[] = ""; //??? FUTURE

// tabel die refereert aan de message strings
const char* const MessageText_tabel[] PROGMEM ={Msg_0,Msg_1,Msg_2,Msg_3,Msg_4,Msg_5,Msg_6,Msg_7,Msg_8,Msg_9,Msg_10,Msg_11,Msg_12,Msg_13,Msg_14,Msg_15,Msg_16,Msg_17,Msg_18,Msg_19,Msg_20};

// strings met vaste tekst naar PROGMEM om hiermee RAM-geheugen te sparen.
const char Text_03[] PROGMEM = "Enter your password: ";
const char Text_04[] PROGMEM = "SunMonTueWedThuFriSatWrkWnd";
const char Text_05[] PROGMEM = "0123456789abcdef";
const char Text_06[] PROGMEM = "; Payload=";
const char Text_07[] PROGMEM = "Waiting...";
const char Text_08[] PROGMEM = "RAWSIGN";                                        // Directory op de SDCard voor opslag RawSignal
const char Text_09[] PROGMEM = "(Last 100 KByte)";
const char Text_10[] PROGMEM = "Busy Nodo: ";
const char Text_11[] PROGMEM = "ALIAS_I";                                        // Directory op de SDCard voor opslag Input: Aliassen van gebruiker -> Nodo Keyword.
const char Text_12[] PROGMEM = "ALIAS_O";                                        // Directory op de SDCard voor opslag Output: Nodo Keywords -> Alias van gebruiker.
const char Text_13[] PROGMEM = "! Variable=%d, Value=";
const char Text_14[] PROGMEM = "Event=";
const char Text_16[] PROGMEM = "! Date=%02d-%02d-%d, Time=%02d:%02d, Variable=%d, Value=";
const char Text_17[] PROGMEM = "Date=%02d-%02d-%d (%s); Time=%02d:%02d";
const char Text_22[] PROGMEM = "!******************************************************************************!";
const char Text_23[] PROGMEM = "LOG";
const char Text_30[] PROGMEM = "Terminal connection closed.";
#endif // MEGA


#if HARDWARE_CLOCK
// Tabel met zonsopgang en -ondergang momenten. afgeleid van KNMI gegevens midden Nederland.
const int Sunrise[] PROGMEM ={528,525,516,503,487,467,446,424,401,378,355,333,313,295,279,268,261,259,263,271,283,297,312,329,345,367,377,394,411,428,446,464,481,498,512,522,528,527};
const int Sunset[] PROGMEM ={999,1010,1026,1044,1062,1081,1099,1117,1135,1152,1169,1186,1203,1219,1235,1248,1258,1263,1264,1259,1249,1235,1218,1198,1177,1154,1131,1107,1084,1062,1041,1023,1008,996,990,989,993,1004};

// omschakeling zomertijd / wintertijd voor komende 10 jaar. Een int bevat de omschakeldatum van maart en oktober.
#define DLSBase 2010 // jaar van eerste element uit de array
const int DLSDate[] PROGMEM ={2831,2730,2528,3127,3026,2925,2730,2629,2528,3127};

struct RealTimeClock {byte Hour,Minutes,Seconds,Date,Month,Day,Daylight,DaylightSaving,DaylightSavingSetMonth,DaylightSavingSetDate; int Year;}Time; // Hier wordt datum & tijd in opgeslagen afkomstig van de RTC.
#endif // HARDWARE_CLOCK


#if HARDWARE_SERIAL_1 && NODO_MEGA
char InputBuffer_Serial[INPUT_LINE_SIZE];                                       // Buffer voor input Seriele data
#endif


// In het transport deel van een Nodo event kunnen zich de volgende vlaggen bevinden:
#define TRANSMISSION_BUSY                                1
#define TRANSMISSION_QUEUE                               2                      // Event maakt deel uit van een reeks die in de queue geplaatst moet worden
#define TRANSMISSION_QUEUE_NEXT                          4                      // Event maakt deel uit van een reeks die in de queue geplaatst moet worden
#define TRANSMISSION_CONFIRM                             8                      // Verzoek aan master om bevestiging te sturen na ontvangst.
#define TRANSMISSION_VIEW                               16                      // Uitsluitend het event weergeven, niet uitvoeren
#define TRANSMISSION_VIEW_EVENTLIST                     32                      // Uitsluitend het event weergeven, niet uitvoeren
#define TRANSMISSION_BROADCAST                          64                      // Als deze vlag staat dan wordt event naar alle Nodos gestuurd als een bekendmaking van deze Nodo.
#define TRANSMISSION_SENDTO                            128                      // Geeft aan dat een event deel uit maakt van een SendTo verzending. Hierrmee wordt voorkomen dat een 'verdwaald' event wordt uitgevoerd.
#define NODO_TYPE_EVENT                                  1
#define NODO_TYPE_COMMAND                                2
#define NODO_TYPE_SYSTEM                                 3               
#define NODO_TYPE_PLUGIN_EVENT                           4
#define NODO_TYPE_PLUGIN_COMMAND                         5
#define NODO_TYPE_RAWSIGNAL                              6

// De Nodo kent naast gebruikers commando's en events eveneens Nodo interne events
#define SYSTEM_COMMAND_CONFIRMED                         1
#define SYSTEM_COMMAND_QUEUE_SENDTO                      2                      // Dit is aankondiging reeks, dus niet het user comando "SendTo".
#define SYSTEM_COMMAND_QUEUE_EVENTLIST_SHOW              3                      // Dit is aankondiging reeks, dus niet het user comando "EventlistShow".
#define SYSTEM_COMMAND_QUEUE_EVENTLIST_WRITE             4                      // Dit is aankondiging reeks, dus niet het user comando "EventlistShow".
#define SYSTEM_COMMAND_BROADCAST                         5                      // Wordt periodiek verzonden om NodoOnline actueel te houden.
#define SYSTEM_COMMAND_BUSY                              6                      // Hiermee wordt aangegeven dat de Nodo busy is (event drager voor de vlag TRANSMISSION_BUSY)

//****************************************************************************************************************************************

#if HARDWARE_RAWSIGNAL || HARDWARE_RF433 || HARDWARE_INFRARED

#if NODO_MEGA
#define RAW_BUFFER_SIZE            512                                          // Maximaal aantal te ontvangen bits = waarde/2
#else
#define RAW_BUFFER_SIZE            256                                          // Maximaal aantal te ontvangen bits = waarde/2
#endif

struct RawSignalStruct                                                          // Variabelen geplaatst in struct zodat deze later eenvoudig kunnen worden weggeschreven naar SDCard
  {
  byte Source;                                                                  // Bron waar het signaal op is binnengekomen.
  int  Number;                                                                  // aantal bits, maal twee omdat iedere bit een mark en een space heeft.
  byte Repeats;                                                                 // Aantal maal dat de pulsreeks verzonden moet worden bij een zendactie.
  byte Delay;                                                                   // Pauze in ms. na verzenden van een enkele pulsenreeks
  byte Multiply;                                                                // Pulses[] * Multiply is de echte tijd van een puls in microseconden
  boolean RepeatChecksum;                                                       // Als deze vlag staat moet er eentweede signaal als checksum binnenkomen om een geldig event te zijn. 
  unsigned long Time;                                                           // Tijdstempel wanneer signaal is binnengekomen (millis())
  byte Pulses[RAW_BUFFER_SIZE+2];                                               // Tabel met de gemeten pulsen in microseconden gedeeld door RawSignal.Multiply. Dit scheelt helft aan RAM geheugen.
                                                                                // Om legacy redenen zit de eerste puls in element 1. Element 0 wordt dus niet gebruikt.
  }RawSignal={0,0,0,0,0,0,0L};
#endif // HARDWARE_RAWSIGNAL

struct SettingsStruct
  {
  byte    Unit;
  byte    WaitBusyNodo;
  byte    RawSignalReceive;
  byte    RawSignalSample;
                                                                                                          
  #if HARDWARE_WIRED_IN_PORTS
  int     WiredInputThreshold[HARDWARE_WIRED_IN_PORTS];
  int     WiredInputSmittTrigger[HARDWARE_WIRED_IN_PORTS];
  byte    WiredInputPullUp[HARDWARE_WIRED_IN_PORTS];
  #endif
  
  #if NODO_MEGA
  unsigned long Alarm[ALARM_MAX];                                               // Instelbaar alarm
  byte    Debug;                                                                // Weergeven van extra gegevens t.b.v. beter inzicht verloop van de verwerking
  byte    TransmitHTTP;                                                         // Definitie van het gebruik van HTTP-communicatie via de IP-poort: [Off | On | All]
  char    Password[26];                                                         // String met wachtwoord.
  char    ID[10];                                                               // ID waar de Nodo uniek mee geÃ¯dentificeerd kan worden in een netwerk
  char    HTTPRequest[80];                                                      // HTTP request;
  char    Temp[26];                                                             // Tijdelijke variabele voor de gebruiker;
  byte    Nodo_IP[4];                                                           // IP adres van van de Nodo. als 0.0.0.0 ingevuld, dan IP toekenning o.b.v. DHCP
  byte    Subnet[4];                                                            // Submask
  byte    Gateway[4];                                                           // Gateway
  byte    DnsServer[4];                                                         // DNS Server IP adres
  unsigned int  PortInput;                                                      // Poort van de inkomende IP communnicatie
  unsigned int  PortOutput;                                                     // Poort van de uitgaande IP communnicatie
  byte    EchoSerial;
  byte    EchoTelnet;
  byte    Log;
  byte    future1;
  byte    future2;
  byte    RawSignalChecksum;
  byte    Alias;
  byte    Res2;
  byte    Res3;
  byte    Res4;
  #endif // NODO_MEGA
  int     Version;                                                              // Onjuiste versie in EEPROM zorgt voor een [Reset]
  }Settings;


 // Niet alle gegevens uit een event zijn relevant. Om ruimte in EEPROM te besparen worden uitsluitend
 // de alleen noodzakelijke gegevens in EEPROM opgeslagen. Hiervoor een struct vullen die later als
 // een geheel blok weggeschreven kan worden.

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

  
struct NodoEventStruct
  {
  // Event deel
  uint8_t Type;
  uint8_t Command;
  uint8_t Par1;
  unsigned long Par2;
  uint16_t Payload;
    
  // Transmissie deel
  uint8_t SourceUnit;
  uint8_t DestinationUnit;
  uint8_t Flags;
  uint8_t Port;
  uint8_t Direction;
  };

struct TransmissionStruct
  {
  uint8_t Type;
  uint8_t Command;
  uint8_t Par1;
  unsigned long Par2;
  uint16_t Payload;
  uint8_t SourceUnit;
  uint8_t DestinationUnit;
  uint8_t Flags;

  uint8_t Checksum;
  };


void PluginInit(void);
boolean (*Plugin_ptr[PLUGIN_MAX])(byte, struct NodoEventStruct*, char*);        // Van alle devices die worden mee gecompileerd, worden in een tabel de adressen opgeslagen zodat hier naartoe gesprongen kan worden.
void (*FastLoopCall_ptr)(void);
byte Plugin_id[PLUGIN_MAX];
unsigned long HW_config=0,HW_status=0;                                          // Hardware configuratie zoals gedetecteerd door de Nodo. 
volatile unsigned long PulseCount=0L;                                           // Pulsenteller van de IR puls. Iedere hoog naar laag transitie wordt deze teller met Ã©Ã©n verhoogd
volatile unsigned long PulseTime=0L;                                            // Tijdsduur tussen twee pulsen teller in milliseconden: millis()-vorige meting.
unsigned long BusyNodo=0;                                                       // In deze variabele wordt per bitpositie aangegeven of een Nodo unitnummer busy is.
boolean Transmission_NodoOnly=false;                                            // Als deze vlag staat, dan worden er uitsluitend Nodo-eigen signalen ontvangen.  
byte QueuePosition=0;
unsigned long UserTimer[TIMER_MAX];                                             // Timers voor de gebruiker.
byte DaylightPrevious;                                                          // t.b.v. voorkomen herhaald genereren van events binnen de lopende minuut waar dit event zich voordoet.
byte ExecutionDepth=0;                                                          // teller die bijhoudt hoe vaak er binnen een macro weer een macro wordt uitgevoerd. Voorkomt tevens vastlopers a.g.v. loops die door een gebruiker zijn gemaakt met macro's.
int ExecutionLine=0;                                                            // Regel in de eventlist die in uitvoer is. Wordt gebruikt voor weergave regelnummer bij uitvoer eventlist.
void(*Reboot)(void)=0;                                                          // reset functie op adres 0.
uint8_t RFbit,RFport,IRbit,IRport;                                              // t.b.v. verwerking IR/FR signalen.
struct NodoEventStruct LastReceived;                                            // Laatst ontvangen event
byte RequestForConfirm=false;                                                   // Als true dan heeft deze Nodo een verzoek ontvangen om een systemevent 'Confirm' te verzenden. Waarde wordt in Par1 meegezonden.
int EventlistMax=0;                                                             // beschikbaar aantal regels in de eventlist. Wordt tijdens setup berekend.
int TimerBroadcast=BROADCAST_TIME;                                              // timer (in sec.) voor periodiek verzenden van een broadcast.
float TempFloat,TempFloat2;                                                     // globale floats die gebruikt mogen worden als een tijdelijke variabele
float UserVarValue[USER_VARIABLES_MAX];                                         // inhoudelijke waarde van de gebruikersvariabele.
byte UserVarKey[USER_VARIABLES_MAX];                                            // Nummer/sleutel van de gebruikersvariabelen
boolean UserVarGlobal[USER_VARIABLES_MAX];                                      // Tabel waarin wordt bijgehouden welke variabelen globaal zijn.
uint16_t UserVarPayload[USER_VARIABLES_MAX];                                    // Extra payload informatie behorende bij een variabele.


#if HARDWARE_WIRED_IN_PORTS
boolean WiredInputStatus[HARDWARE_WIRED_IN_PORTS];                              // Status van de WiredIn worden hierin opgeslagen.
#endif

#if HARDWARE_WIRED_OUT_PORTS
byte WiredOutputStatus[HARDWARE_WIRED_OUT_PORTS];                               // Wired statussen bewaren zodat een verandering wordt gedetecteerd.
#endif

#if NODO_MEGA
byte  Transmission_SendToUnit=0;                                                // Unitnummer waar de events naar toe gestuurd worden. 0=alle.
byte  Transmission_SendToAll=0;                                                 // Waarde die aangeeft of het SendTo permanent staat ingeschakeld. 0=uit, in andere gevallen het unitnummer.
boolean Transmission_SendtoFast=false;                                          // Vlag die aangeeft of de SendTo via de snelle modus (zonder handshaking) plaats moet vinden.
byte AlarmPrevious[ALARM_MAX];                                                  // Bevat laatste afgelopen alarm. Ter voorkoming dat alarmen herhaald aflopen.
boolean WebApp=false;                                                           // ??? Deze vlag moet later onder Port_WebSocket worden ondergebracht
uint8_t MD5HashCode[16];                                                        // tabel voor berekenen van MD5 hash codes.
int TimerCookie=5;                                                              // Seconden teller die bijhoudt wanneer er weer een nieuw Cookie naar de WebApp verzonden moet worden.
int TerminalConnected=0;                                                        // Vlag geeft aan of en hoe lang nog (seconden) er verbinding is met een Terminal.
int TerminalLocked=1;                                                           // 0 als als gebruiker van een telnet terminalsessie juiste wachtwoord heeft ingetoetst
char TempLogFile[13];                                                           // Naam van de Logfile waar (naast de standaard logging) de verwerking in gelogd moet worden.
char HTTPCookie[10];                                                            // Cookie voor uitwisselen van encrypted events via HTTP
int FileWriteMode=0;                                                            // Het aantal seconden dat deze timer ingesteld staat zal er geen verwerking plaats vinden van TerminalInvoer. Iedere seconde --.
char InputBuffer_Terminal[INPUT_LINE_SIZE];                                     // Buffer voor input terminal verbinding Telnet sessie
char StrTmp[40];                                                                // Kleine string voor gebruik binnen(!) funkties.                                  
boolean ClockSyncHTTP=false;
#endif // NODO_MEGA


void setup() 
  {    
  int x;
  struct NodoEventStruct TempEvent;

  #if HARDWARE_SERIAL_1
  Serial.begin(BAUD);                                                           // Initialiseer de seriele poort
  HW_SetStatus(HW_SERIAL_1,true,true);
  #endif


  #if NODO_MEGA
  HW_SetStatus(HW_NODO_MEGA,true,true);
  #else
  HW_SetStatus(HW_NODO_SMALL,true,true);
  #endif

  x=(EEPROM_SIZE-sizeof(struct SettingsStruct))/sizeof(struct EventlistStruct); // bereken aantal beschikbare eventlistregels in het eeprom geheugen
  EventlistMax=x>255?255:x;

 
  #if NODO_MEGA
  for(x=0;x<ALARM_MAX;x++)                                                      // Zet alle alarmen op nog niet afgegaan.
    AlarmPrevious[x]=0xff;                                                      // Deze waarde kan niet bestaan en voldoet dus.
  #endif //NODO-MEGA


  for(x=0;x<TIMER_MAX;x++)                                                      // Alle timers op nul zetten.
    UserTimer[x]=0L;

  for(x=0;x<UNIT_MAX;x++)                                                       // Maak de tabel met Nodo's leeg.
    NodoOnline(x,0,true);

  #if HARDWARE_SPEAKER
    pinMode(PIN_SPEAKER,    OUTPUT);
    HW_SetStatus(HW_SPEAKER,true,true);
  #endif


  #if HARDWARE_PULSE
  pinMode(PIN_PULSE, INPUT);
  digitalWrite(PIN_PULSE,INPUT_PULLUP);                                    // schakel pull-up weerstand in om te voorkomen dat er rommel binnenkomt als pin niet aangesloten.
  HW_SetStatus(HW_PULSE,true,true);
  #endif // HARDWARE_PULSE


  #if HARDWARE_INFRARED
  pinMode(PIN_IR_RX_DATA, INPUT);
  pinMode(PIN_IR_TX_DATA, OUTPUT);
  digitalWrite(PIN_IR_TX_DATA,LOW);                                             // Zet de IR zenders initiÃ«el uit! Anders mogelijk overbelasting !
  digitalWrite(PIN_IR_RX_DATA,INPUT_PULLUP);                                    // schakel pull-up weerstand in om te voorkomen dat er rommel binnenkomt als pin niet aangesloten.
  IRbit=digitalPinToBitMask(PIN_IR_RX_DATA);
  IRport=digitalPinToPort(PIN_IR_RX_DATA);
  HW_SetStatus(HW_INFRARED,true,true);
  #endif


  #if HARDWARE_RF433
  pinMode(PIN_RF_RX_DATA, INPUT);
  pinMode(PIN_RF_TX_DATA, OUTPUT);
  pinMode(PIN_RF_TX_ENABLE,  OUTPUT);
  digitalWrite(PIN_RF_RX_DATA,INPUT_PULLUP);                                    // schakel pull-up weerstand in om te voorkomen dat er rommel binnenkomt als pin niet aangesloten.
  RFbit=digitalPinToBitMask(PIN_RF_RX_DATA);
  RFport=digitalPinToPort(PIN_RF_RX_DATA);  
  HW_SetStatus(HW_RF433,true,true);
  #if PIN_RF_RX_ENABLE                                                          // Ondersteuning voor 1e generatie hardware
  pinMode(PIN_RF_RX_ENABLE,  OUTPUT);
  digitalWrite(PIN_RF_RX_ENABLE,HIGH);                                          // Spanning naar de RF ontvanger aan.
  #endif
  #endif


  #if HARDWARE_STATUS_LED
  pinMode(PIN_LED, OUTPUT);
  Led(RED);
  HW_SetStatus(HW_STATUS_LED,true,true);
  #endif
  
    
  #if HARDWARE_STATUS_LED_RGB
  pinMode(PIN_LED_R,  OUTPUT);
  pinMode(PIN_LED_G,  OUTPUT);
  pinMode(PIN_LED_B,  OUTPUT);
  Led(BLUE);
  HW_SetStatus(HW_STATUS_LED_RGB,true,true);
  #endif
  
  
  #if HARDWARE_AUREL_PULSE
  pinMode(PIN_AUREL_PULSE,OUTPUT);
  digitalWrite(PIN_AUREL_PULSE,HIGH);
  #endif

  ClearEvent(&LastReceived);

  UserVariableInit();                                                           // Initialiseer de user variabelen
  LoadSettings();                                                               // laad alle settings zoals deze in de EEPROM zijn opgeslagen

  if(Settings.Version!=NODO_VERSION_MINOR)ResetFactory();                       // De Nodo resetten als Versie van de settings zoals geladen vanuit EEPROM niet correct is.
  
  #if HARDWARE_WIRED_IN_PORTS  !=0
  for(x=0;x<HARDWARE_WIRED_IN_PORTS;x++)                                        // initialiseer de Wired ingangen.
    {
    if(Settings.WiredInputPullUp[x]==VALUE_ON)
      pinMode(A0+PIN_WIRED_IN_1+x,INPUT_PULLUP);
    else
      pinMode(A0+PIN_WIRED_IN_1+x,INPUT);

    WiredInputStatus[x]=true;                                                   // Status van de wired poort setten zodat er niet onterecht bij start al events worden gegenereerd.
    }
  HW_SetStatus(HW_WIRED_IN_PORTS,true,true);
  #endif
  

  #if HARDWARE_WIRED_OUT_PORTS !=0
  for(x=0;x<HARDWARE_WIRED_OUT_PORTS;x++)                                       // initialiseer de WiredOut uitgangen.
    {
    pinMode(PIN_WIRED_OUT_1+x,OUTPUT);                                          // definieer Arduino pin's voor Wired-Out
    WiredOutputStatus[x]=0;
    }
  HW_SetStatus(HW_WIRED_OUT_PORTS,true,true);
  #endif

   
  #if HARDWARE_SDCARD
  pinMode(EthernetShield_CS_SDCardH, OUTPUT);                                   // CS/SPI: nodig voor correct funktioneren van de SDCard!
  HW_SetStatus(HW_SDCARD,SDCardInit(),true);
  if(HW_Status(HW_SDCARD))
    FileExecute("","config","dat",true,VALUE_ALL);                              // Voer bestand config uit als deze bestaat. die goeie oude MD-DOS tijd ;-)
  #endif
   
  
  #if HARDWARE_I2C
  HW_SetStatus(HW_I2C,Port_I2C_Init(),true);
  #endif


  #if HARDWARE_CLOCK && HARDWARE_I2C
  Wire.begin();
  HW_SetStatus(HW_CLOCK,ClockRead(),true);                                      //Lees de tijd uit de RTC en zorg ervoor dat er niet direct na een boot een CMD_CLOCK_DAYLIGHT event optreedt
  #endif
  
  
  #if HARDWARE_CLOCK && NODO_MEGA
  if(HW_Status(HW_CLOCK))
    {
    SetDaylight();
    DaylightPrevious=0xff;                                                      // vul de waarde met een niet bestaande daylight status zodat er na een boot altijd een event wordt gegenereerd.
    }
  #endif 


  #if HARDWARE_SPI_HARDWARE
  SPI_begin();                                                                  // Initialiseer poorten voor SPI-communicatie.
  HW_SetStatus(HW_SPI_HARDWARE,true,true);
  #endif


  #if HARDWARE_SPI_SOFTWARE
  SPI_begin();                                                                  // Initialiseer poorten voor SPI-communicatie.
  HW_SetStatus(HW_SPI_SOFTWARE,true,true);
  #endif

  
  #if HARDWARE_NRF24L01
  HW_SetStatus(HW_NRF24L01,Port_NRF24L01_Init(),true);
  #endif


  #if HARDWARE_ETHERNET
  // Detecteren of fysieke laag is aangesloten wordt niet ondersteund door de Ethernet Library van Arduino :-(

  #if HARDWARE_SERIAL_1
  Serial.print(F("Ethernet connection: "));
  #endif // HARDWARE_SERIAL_1

  if(EthernetInit())                                                            // Start Ethernet kaart en start de HTTP-Server en de Telnet-server
    {
    HW_SetStatus(HW_ETHERNET,true,true);
    if(Settings.Password[0]!=0 && Settings.TransmitHTTP!=VALUE_OFF)
      {
      #if HARDWARE_SERIAL_1
      Serial.print(F("Host connection: "));Serial.print(Settings.HTTPRequest);
      if(SendHTTPCookie())                                                     // Verzend een nieuw cookie
        {
        WebApp=true;
        Serial.println(F(" Ok."));
        }        
      #endif // HARDWARE_SERIAL_1

      #if HARDWARE_SERIAL_1
      else
        Serial.println(F(" Failed!"));
      #endif // HARDWARE_SERIAL_1
      }
    }
  else
    {
    HW_SetStatus(HW_ETHERNET,false,true);
    }
  #endif

  PluginInit();                                                                 // vul pointers naar de plugin nummers

  #if HARDWARE_SERIAL_1
  PrintWelcome();                                                               // geef de welkomsttekst weer
  #endif

  ClearEvent(&TempEvent);
  TempEvent.Par1      = Settings.Unit;
  TempEvent.Command   = EVENT_BOOT;

  TempEvent.Direction = VALUE_DIRECTION_INPUT;
  TempEvent.Port      = VALUE_SOURCE_SYSTEM;
  TempEvent.Type      = NODO_TYPE_EVENT;
  ProcessEvent(&TempEvent);                                                     // Voer het boot event uit.

  TempEvent.Direction = VALUE_DIRECTION_OUTPUT;
  TempEvent.Port      = VALUE_ALL;
  TempEvent.Type      = NODO_TYPE_EVENT;
  TempEvent.Flags     = TRANSMISSION_BROADCAST;
  SendEvent(&TempEvent,false,true);                                             // Zend 'boot' event naar alle Nodo's.  

  QueueProcess();

  if(HW_config!=HW_status)
    RaiseMessage(MESSAGE_HARDWARE_ERROR,0);

  #if HARDWARE_SDCARD && HARDWARE_SERIAL_1

  if(HW_SDCARD)
    FileExecute("", "autoexec", "dat",true,VALUE_ALL);                          // Voer bestand AutoExec uit als deze bestaat. die goeie oude MD-DOS tijd ;-)
  #endif

  #if HARDWARE_SERIAL_1 && NODO_MEGA
  Serial.println(F("\nReady. Nodo is waiting for serial input...\n"));
  HW_SetStatus(HW_SERIAL_1,Serial.available()?1:0,false);
  #endif
  }

void loop() 
  {
  int x,y,z;
  byte SerialInByte;
  byte TerminalInByte;
  int SerialInByteCounter=0;
  int TerminalInbyteCounter=0;
  byte Slice_1=0;    
  struct NodoEventStruct ReceivedEvent;
  byte PreviousMinutes=0;                                                       // Sla laatst gecheckte minuut op zodat niet vaker dan nodig (eenmaal per minuut de eventlist wordt doorlopen
  
  unsigned long LoopIntervalTimer=0L;                                           // Timer voor periodieke verwerking.
  unsigned long FocusTimer;                                                     // Timer die er voor zorgt dat bij communicatie via een kanaal de focus hier ook enige tijd op blijft
  unsigned long PreviousTimeEvent=0L; 

  #if HARDWARE_SERIAL_1 && NODO_MEGA
  InputBuffer_Serial[0]=0;                                                      // serieel buffer string leeg maken
  #endif

  #if HARDWARE_SDCARD
  TempLogFile[0]=0;                                                             // geen extra logging
  #endif

  // hoofdloop: scannen naar signalen
  // dit is een tijdkritische loop die wacht tot binnengekomen event op de beschikbare poorten en SERIAL, CLOCK, DAYLIGHT, TIMER, etc

  while(true)
    {
    if(ScanEvent(&ReceivedEvent))
      { 
      ProcessingStatus(true);
      ProcessEvent(&ReceivedEvent);                                             // verwerk binnengekomen event.
      Slice_1=0;
      ProcessingStatus(false);
      }

    // SERIAL: *************** kijk of er data klaar staat op de seriele poort **********************
    #if HARDWARE_SERIAL_1 && NODO_MEGA
    if(Serial.available())
      {
      PluginCall(PLUGIN_SERIAL_IN,0,0);

      HW_SetStatus(HW_SERIAL_1,true,false);
      FocusTimer=millis()+FOCUS_TIME;

      while(FocusTimer>millis())                                                // blijf even paraat voor luisteren naar deze poort en voorkom dat andere input deze communicatie onderbreekt.
        {          
        if(Serial.available())
          {                        
          SerialInByte=Serial.read();                

          if(Settings.EchoSerial==VALUE_ON)
            Serial.write(SerialInByte);                                         // echo ontvangen teken
          
          if(SerialInByte==XON)
            Serial.write(XON);                                                  // om te voorkomen dat beide devices om wat voor reden dan ook op elkaar wachten
          
          if(isprint(SerialInByte))
            if(SerialInByteCounter<(INPUT_LINE_SIZE-1))
              InputBuffer_Serial[SerialInByteCounter++]=SerialInByte;
              
          if(SerialInByte=='\n')
            {
            ProcessingStatus(true);
            InputBuffer_Serial[SerialInByteCounter]=0;                          // serieel ontvangen regel is compleet
            RaiseMessage(ExecuteLine(InputBuffer_Serial,VALUE_SOURCE_SERIAL),0);
            Serial.write('>');                                                  // Prompt
            SerialInByteCounter=0;  
            InputBuffer_Serial[0]=0;                                            // serieel ontvangen regel is verwerkt. String leegmaken
            ProcessingStatus(false);
            }
          FocusTimer=millis()+FOCUS_TIME;                                             
          }
        }
      }// if(Serial.available())
    #endif
      


    // Een plugin mag tijdelijk een claim doen op de snelle aanroep vanuit de hoofdloop. 
    // Als de waarde FastLoopCall_ptr is gevuld met het adres van een funktie dan wordt de betreffende funktie eenmaal
    // per FAST_LOOP mSec aangeroepen.
    if(FastLoopCall_ptr)
      FastLoopCall_ptr();

    switch(Slice_1++)
      {        
      case 0:
        {
        // IP Event: *************** kijk of er een Event van IP komt **********************    
        #if HARDWARE_ETHERNET
        if(HW_Status(HW_ETHERNET))
          if(IPServer.available())
            if(!PluginCall(PLUGIN_ETHERNET_IN,0,0))
              ExecuteIP();

        #endif
        break;
        }
 
      case 1:
        {
        #if HARDWARE_ETHERNET
        // IP Telnet verbinding : *************** kijk of er verzoek tot verbinding vanuit een terminal is **********************    
        if(HW_Status(HW_ETHERNET))
          {
          while(TerminalServer.available())
            {          
            if(TerminalConnected==0)                                            // indien een nieuwe connectie
              {
              TerminalClient=TerminalServer.available();
              TerminalConnected=TERMINAL_TIMEOUT;
              InputBuffer_Terminal[0]=0;
              TerminalInbyteCounter=0;
              TerminalClient.flush();                                           // eventuele rommel weggooien.

              if(Settings.Password[0]!=0)
                {
                if(TerminalLocked==0)
                  TerminalLocked=1;
                  
                if(TerminalLocked<=PASSWORD_MAX_RETRY)
                   TerminalClient.print(ProgmemString(Text_03));
                else
                  {
                  TerminalClient.print(cmd2str(MESSAGE_ACCESS_DENIED));
                  RaiseMessage(MESSAGE_ACCESS_DENIED,0);
                  }
                }
              else
                {
                TerminalLocked=0;
                y=HW_Status(HW_SERIAL_1);
                HW_SetStatus(HW_SERIAL_1,false,false);
                PrintWelcome();
                HW_SetStatus(HW_SERIAL_1,y,false);
                }                
              }
            
            while(TerminalClient.available()) 
              {
              TerminalInByte=TerminalClient.read();
              
              if(isprint(TerminalInByte))
                {
                if(TerminalInbyteCounter<(INPUT_LINE_SIZE-1))
                  {
                  if(Settings.EchoTelnet==VALUE_ON)
                    {
                    if(TerminalLocked && isprint(TerminalInByte))
                      TerminalClient.write('*');                                // echo asterisk zodat wachtwoord niet zichtbaar is a.g.v. echo.
                    else
                      TerminalClient.write(TerminalInByte);                     // Echo ontvangen teken                  
                    }
                  InputBuffer_Terminal[TerminalInbyteCounter++]=TerminalInByte;
                  }
                else
                   TerminalClient.write('?');                                   // geen ruimte meer.
                }

              else if(TerminalInByte==0x0a /*LF*/ || TerminalInByte==0x0d /*CR*/)
                {                    
                if(Settings.EchoTelnet==VALUE_ON)
                  TerminalClient.println("");                                   // Echo de nieuwe regel.
                TerminalConnected=TERMINAL_TIMEOUT;
                InputBuffer_Terminal[TerminalInbyteCounter]=0;
                TerminalInbyteCounter=0;
              
                if(TerminalLocked==0)                                           // als niet op slot
                  {
//???                  TerminalClient.getRemoteIP(ClientIPAddress);
                  ExecutionDepth=0;
                  ProcessingStatus(true);
                  RaiseMessage(ExecuteLine(InputBuffer_Terminal,VALUE_SOURCE_TELNET),0);
                  ProcessingStatus(false);
                  TerminalClient.write('>');                                    // prompt
                  }
                else
                  {
                  if(TerminalLocked<=PASSWORD_MAX_RETRY)                        // teller is wachtloop bij herhaaldelijke pogingen foutief wachtwoord. Bij >3 pogingen niet meer toegestaan
                    {
                    if(strcmp(InputBuffer_Terminal,Settings.Password)==0)       // als wachtwoord goed is, dan slot er af
                      {
                      TerminalLocked=0;
                      y=HW_Status(HW_SERIAL_1);
                      HW_SetStatus(HW_SERIAL_1,false,false);
                      PrintWelcome();
                      HW_SetStatus(HW_SERIAL_1,y,false);
                      }
                    else                                                        // als foutief wachtwoord, dan teller 
                      {
                      TerminalLocked++;
                      TerminalClient.println("?");
                      if(TerminalLocked>PASSWORD_MAX_RETRY)
                        {
                        TerminalLocked=PASSWORD_TIMEOUT;                        // blokkeer tijd terminal
                        TerminalClient.print(cmd2str(MESSAGE_ACCESS_DENIED));
                        RaiseMessage(MESSAGE_ACCESS_DENIED,0);
                        }
                      else
                        TerminalClient.print(ProgmemString(Text_03));
                      }
                    }
                  else
                    {
                    TerminalClient.println(cmd2str(MESSAGE_ACCESS_DENIED));
                    }
                  }
                }
              else 
                {
                // bij een niet printbaar teken 
                break;
                }
              }
            }
          }
        #endif
        break;
        }
      
      case 2:
        {
        #if HARDWARE_WIRED_IN_PORTS
        // WIRED: *************** kijk of statussen gewijzigd zijn op WIRED **********************  
        // als de huidige waarde groter dan threshold EN de vorige keer was dat nog niet zo DAN event genereren
        for(x=0;x<HARDWARE_WIRED_IN_PORTS;x++)
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
            ProcessEvent(&ReceivedEvent); // verwerk binnengekomen event.
            }
          }
        #endif
        break;
        }
        
      default:
        ProcessingStatus(false);
        Slice_1=0;
        break;
      }// switch


    // Niet tijdkritische processen die periodiek uitgevoerd moeten worden
    if(LoopIntervalTimer<millis())// lange interval: Iedere seconde.
      {
      LoopIntervalTimer=millis()+SCAN_LOW_TIME; // reset de timer  

      // PLUGIN: **************** Loop iedere seconde de plugins langs voor onderhoudstaken ***********************
      PluginCall(PLUGIN_ONCE_A_SECOND,&ReceivedEvent,0);


      // BROADCAST: **************** Stuur periodiek een broadcast event naar alle Nodos zodat ze weten dat deze Nodo online is ***********************
      if(TimerBroadcast>0)
        TimerBroadcast--;
      else
        {
        TimerBroadcast=BROADCAST_TIME;
        ClearEvent(&ReceivedEvent);
        ReceivedEvent.Type             = NODO_TYPE_SYSTEM;
        ReceivedEvent.Command          = SYSTEM_COMMAND_BROADCAST;
        ReceivedEvent.Direction        = VALUE_DIRECTION_OUTPUT;
        ReceivedEvent.Port             = VALUE_ALL;
        ReceivedEvent.Flags            = TRANSMISSION_BROADCAST;
        SendEvent(&ReceivedEvent,false,true);                                       // Zend broadcast-event naar alle Nodo's.  
        }


      // TIMER: **************** Genereer event als 1 van de Timers voor de gebruiker afgelopen is ***********************    
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
            ReceivedEvent.Port             = VALUE_SOURCE_CLOCK;
            ProcessEvent(&ReceivedEvent); // verwerk binnengekomen event.
            }
          }
        }

      #if HARDWARE_CLOCK && HARDWARE_I2C
      if(HW_Status(HW_CLOCK))                                                   // Als de klok aanwezig
        {
        ClockRead(); // Lees de Real Time Clock waarden in de struct Time       // Dan klok uitlezen
        if(Time.Minutes!=PreviousMinutes)                                       // eenmaal per minuut
          {
          PreviousMinutes=Time.Minutes;

          // CLOCK: **************** Check op Time-events  ***********************
          ClearEvent(&ReceivedEvent);
          ReceivedEvent.SourceUnit       = 0;
          ReceivedEvent.Type             = NODO_TYPE_EVENT;
          ReceivedEvent.Command          = EVENT_TIME;
          ReceivedEvent.Par2             = Time.Minutes%10 | (unsigned long)(Time.Minutes/10)<<4 | (unsigned long)(Time.Hour%10)<<8 | (unsigned long)(Time.Hour/10)<<12 | (unsigned long)Time.Day<<16;
          ReceivedEvent.Direction        = VALUE_DIRECTION_INPUT;
          ReceivedEvent.Port             = VALUE_SOURCE_CLOCK;
          
          if(CheckEventlist(&ReceivedEvent))
            {
            if(PreviousTimeEvent!=ReceivedEvent.Par2)
              {
              ProcessEvent(&ReceivedEvent); // verwerk binnengekomen event.
              PreviousTimeEvent=ReceivedEvent.Par2; 
              }
            }
          else
            PreviousTimeEvent=0L;
          }

        // CLOCK: **************** Check op Daylight-events  ***********************
        #if NODO_MEGA
        SetDaylight();
        if(Time.Daylight!=DaylightPrevious)                                     // er heeft een zonsondergang of zonsopkomst event voorgedaan
          {
          ClearEvent(&ReceivedEvent);
          ReceivedEvent.Type             = NODO_TYPE_EVENT;
          ReceivedEvent.Command          = EVENT_CLOCK_DAYLIGHT;
          ReceivedEvent.Par1             = Time.Daylight;
          ReceivedEvent.Direction        = VALUE_DIRECTION_INPUT;
          ReceivedEvent.Port             = VALUE_SOURCE_CLOCK;
          ProcessEvent(&ReceivedEvent);                                         // verwerk binnengekomen event.
          DaylightPrevious=Time.Daylight;
          }

        // CLOCK: **************** Check op Alarm-events  ***********************
        if(ScanAlarm(&ReceivedEvent)) 
          ProcessEvent(&ReceivedEvent);                                         // verwerk binnengekomen event.
        #endif  // NODO_MEGA
        }
      #endif 
    

      // Terminal onderhoudstaken
      // tel seconden terug nadat de gebruiker gedefinieerd maal foutief wachtwoord ingegeven
      #if HARDWARE_ETHERNET
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
        
      // Timer voor verzenden van Cookie naar de WebApp/Server
      if(WebApp)
        {
        if(TimerCookie>0)
          TimerCookie--;
        else
          {
          TimerCookie=COOKIE_REFRESH_TIME;
          SendHTTPCookie(); // Verzend een nieuw cookie
          }
        }
      #endif // HARDWARE_ETHERNET


      // Timer voor blokkeren verwerking. teller verlagen
      #if HARDWARE_SDCARD
      if(FileWriteMode>0)
        {
        FileWriteMode--;
        if(FileWriteMode==0)
          TempLogFile[0]=0;
        }
      #endif //Mega
      }
    }// while 
  }


