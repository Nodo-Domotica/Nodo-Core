/****************************************************************************************************************************\
* In dit tabblad staan voorzieningen die nodig zijn om de devices te integreren in de Nodo code op een zodanige manier dat
* uitsluitend DIE devices worden meegecompileerd die ook daadwerklijk door de gebruiker zijn opgegeven. Niet opgegeven
* devices worden dus niet meegecompileerd om geheugen te besparen. Dit tabblad bevat geen code die door de gebruiker 
* aangepast hoeft te worden.
*
* Devics worden uniek genummerd. Deze nummers worden uitgegeven door het Nodo team. Device_255 is een template voor 
* een device dat kan worden gebruikt als je zelf een device wilt toevoegen aan de Nodo. 
\*************************************************************************************************************************/

#define DEVICEFILE2(a, b) stringify(a/Devices/b)
#define DEVICEFILE(a, b) DEVICEFILE2(a, b)

#ifdef DEVICE_001
#include DEVICEFILE(SKETCH_PATH,Device_001.c)
#endif

#ifdef DEVICE_002
#include DEVICEFILE(SKETCH_PATH,Device_002.c)
#endif

#ifdef DEVICE_003
#include DEVICEFILE(SKETCH_PATH,Device_003.c)
#endif

#ifdef DEVICE_004
#include DEVICEFILE(SKETCH_PATH,Device_004.c)
#endif

#ifdef DEVICE_005
#include DEVICEFILE(SKETCH_PATH,Device_005.c)
#endif

#ifdef DEVICE_006
#include DEVICEFILE(SKETCH_PATH,Device_006.c)
#endif

#ifdef DEVICE_007
#include DEVICEFILE(SKETCH_PATH,Device_007.c)
#endif

#ifdef DEVICE_008
#include DEVICEFILE(SKETCH_PATH,Device_008.c)
#endif

#ifdef DEVICE_009
#include DEVICEFILE(SKETCH_PATH,Device_009.c)
#endif

#ifdef DEVICE_010
#include DEVICEFILE(SKETCH_PATH,Device_010.c)
#endif

#ifdef DEVICE_011
#include DEVICEFILE(SKETCH_PATH,Device_011.c)
#endif

#ifdef DEVICE_012
#include DEVICEFILE(SKETCH_PATH,Device_012.c)
#endif

#ifdef DEVICE_013
#include DEVICEFILE(SKETCH_PATH,Device_013.c)
#endif

#ifdef DEVICE_014
#include DEVICEFILE(SKETCH_PATH,Device_014.c)
#endif

#ifdef DEVICE_015
#include DEVICEFILE(SKETCH_PATH,Device_015.c)
#endif

#ifdef DEVICE_016
#include DEVICEFILE(SKETCH_PATH,Device_016.c)
#endif

#ifdef DEVICE_017
#include DEVICEFILE(SKETCH_PATH,Device_017.c)
#endif

#ifdef DEVICE_018
#include DEVICEFILE(SKETCH_PATH,Device_018.c)
#endif

#ifdef DEVICE_019
#include DEVICEFILE(SKETCH_PATH,Device_019.c)
#endif

#ifdef DEVICE_020
#include DEVICEFILE(SKETCH_PATH,Device_020.c)
#endif

#ifdef DEVICE_021
#include DEVICEFILE(SKETCH_PATH,Device_021.c)
#endif

#ifdef DEVICE_022
#include DEVICEFILE(SKETCH_PATH,Device_022.c)
#endif

#ifdef DEVICE_023
#include DEVICEFILE(SKETCH_PATH,Device_023.c)
#endif

#ifdef DEVICE_024
#include DEVICEFILE(SKETCH_PATH,Device_024.c)
#endif

#ifdef DEVICE_025
#include DEVICEFILE(SKETCH_PATH,Device_025.c)
#endif

#ifdef DEVICE_026
#include DEVICEFILE(SKETCH_PATH,Device_026.c)
#endif

#ifdef DEVICE_027
#include DEVICEFILE(SKETCH_PATH,Device_027.c)
#endif

#ifdef DEVICE_028
#include DEVICEFILE(SKETCH_PATH,Device_028.c)
#endif

#ifdef DEVICE_029
#include DEVICEFILE(SKETCH_PATH,Device_029.c)
#endif

#ifdef DEVICE_030
#include DEVICEFILE(SKETCH_PATH,Device_030.c)
#endif

#ifdef DEVICE_031
#include DEVICEFILE(SKETCH_PATH,Device_031.c)
#endif

#ifdef DEVICE_032
#include DEVICEFILE(SKETCH_PATH,Device_032.c)
#endif

#ifdef DEVICE_033
#include DEVICEFILE(SKETCH_PATH,Device_033.c)
#endif

#ifdef DEVICE_034
#include DEVICEFILE(SKETCH_PATH,Device_034.c)
#endif

#ifdef DEVICE_035
#include DEVICEFILE(SKETCH_PATH,Device_035.c)
#endif

#ifdef DEVICE_036
#include DEVICEFILE(SKETCH_PATH,Device_036.c)
#endif

#ifdef DEVICE_037
#include DEVICEFILE(SKETCH_PATH,Device_037.c)
#endif

#ifdef DEVICE_038
#include DEVICEFILE(SKETCH_PATH,Device_038.c)
#endif

#ifdef DEVICE_039
#include DEVICEFILE(SKETCH_PATH,Device_039.c)
#endif

#ifdef DEVICE_040
#include DEVICEFILE(SKETCH_PATH,Device_040.c)
#endif

#ifdef DEVICE_041
#include DEVICEFILE(SKETCH_PATH,Device_041.c)
#endif

#ifdef DEVICE_042
#include DEVICEFILE(SKETCH_PATH,Device_042.c)
#endif

#ifdef DEVICE_043
#include DEVICEFILE(SKETCH_PATH,Device_043.c)
#endif

#ifdef DEVICE_044
#include DEVICEFILE(SKETCH_PATH,Device_044.c)
#endif

#ifdef DEVICE_045
#include DEVICEFILE(SKETCH_PATH,Device_045.c)
#endif

#ifdef DEVICE_046
#include DEVICEFILE(SKETCH_PATH,Device_046.c)
#endif

#ifdef DEVICE_047
#include DEVICEFILE(SKETCH_PATH,Device_047.c)
#endif

#ifdef DEVICE_048
#include DEVICEFILE(SKETCH_PATH,Device_048.c)
#endif

#ifdef DEVICE_049
#include DEVICEFILE(SKETCH_PATH,Device_049.c)
#endif

#ifdef DEVICE_050
#include DEVICEFILE(SKETCH_PATH,Device_050.c)
#endif

#ifdef DEVICE_051
#include DEVICEFILE(SKETCH_PATH,Device_051.c)
#endif

#ifdef DEVICE_052
#include DEVICEFILE(SKETCH_PATH,Device_052.c)
#endif

#ifdef DEVICE_053
#include DEVICEFILE(SKETCH_PATH,Device_053.c)
#endif

#ifdef DEVICE_054
#include DEVICEFILE(SKETCH_PATH,Device_054.c)
#endif

#ifdef DEVICE_055
#include DEVICEFILE(SKETCH_PATH,Device_055.c)
#endif

#ifdef DEVICE_056
#include DEVICEFILE(SKETCH_PATH,Device_056.c)
#endif

#ifdef DEVICE_057
#include DEVICEFILE(SKETCH_PATH,Device_057.c)
#endif

#ifdef DEVICE_058
#include DEVICEFILE(SKETCH_PATH,Device_058.c)
#endif

#ifdef DEVICE_059
#include DEVICEFILE(SKETCH_PATH,Device_059.c)
#endif

#ifdef DEVICE_060
#include DEVICEFILE(SKETCH_PATH,Device_060.c)
#endif

#ifdef DEVICE_061
#include DEVICEFILE(SKETCH_PATH,Device_061.c)
#endif

#ifdef DEVICE_062
#include DEVICEFILE(SKETCH_PATH,Device_062.c)
#endif

#ifdef DEVICE_063
#include DEVICEFILE(SKETCH_PATH,Device_063.c)
#endif

#ifdef DEVICE_064
#include DEVICEFILE(SKETCH_PATH,Device_064.c)
#endif

#ifdef DEVICE_065
#include DEVICEFILE(SKETCH_PATH,Device_065.c)
#endif

#ifdef DEVICE_066
#include DEVICEFILE(SKETCH_PATH,Device_066.c)
#endif

#ifdef DEVICE_067
#include DEVICEFILE(SKETCH_PATH,Device_067.c)
#endif

#ifdef DEVICE_068
#include DEVICEFILE(SKETCH_PATH,Device_068.c)
#endif

#ifdef DEVICE_069
#include DEVICEFILE(SKETCH_PATH,Device_069.c)
#endif

#ifdef DEVICE_070
#include DEVICEFILE(SKETCH_PATH,Device_070.c)
#endif

#ifdef DEVICE_071
#include DEVICEFILE(SKETCH_PATH,Device_071.c)
#endif

#ifdef DEVICE_072
#include DEVICEFILE(SKETCH_PATH,Device_072.c)
#endif

#ifdef DEVICE_073
#include DEVICEFILE(SKETCH_PATH,Device_073.c)
#endif

#ifdef DEVICE_074
#include DEVICEFILE(SKETCH_PATH,Device_074.c)
#endif

#ifdef DEVICE_075
#include DEVICEFILE(SKETCH_PATH,Device_075.c)
#endif

#ifdef DEVICE_076
#include DEVICEFILE(SKETCH_PATH,Device_076.c)
#endif

#ifdef DEVICE_077
#include DEVICEFILE(SKETCH_PATH,Device_077.c)
#endif

#ifdef DEVICE_078
#include DEVICEFILE(SKETCH_PATH,Device_078.c)
#endif

#ifdef DEVICE_079
#include DEVICEFILE(SKETCH_PATH,Device_079.c)
#endif

#ifdef DEVICE_080
#include DEVICEFILE(SKETCH_PATH,Device_080.c)
#endif

#ifdef DEVICE_081
#include DEVICEFILE(SKETCH_PATH,Device_081.c)
#endif

#ifdef DEVICE_082
#include DEVICEFILE(SKETCH_PATH,Device_082.c)
#endif

#ifdef DEVICE_083
#include DEVICEFILE(SKETCH_PATH,Device_083.c)
#endif

#ifdef DEVICE_084
#include DEVICEFILE(SKETCH_PATH,Device_084.c)
#endif

#ifdef DEVICE_085
#include DEVICEFILE(SKETCH_PATH,Device_085.c)
#endif

#ifdef DEVICE_086
#include DEVICEFILE(SKETCH_PATH,Device_086.c)
#endif

#ifdef DEVICE_087
#include DEVICEFILE(SKETCH_PATH,Device_087.c)
#endif

#ifdef DEVICE_088
#include DEVICEFILE(SKETCH_PATH,Device_088.c)
#endif

#ifdef DEVICE_089
#include DEVICEFILE(SKETCH_PATH,Device_089.c)
#endif

#ifdef DEVICE_090
#include DEVICEFILE(SKETCH_PATH,Device_090.c)
#endif

#ifdef DEVICE_091
#include DEVICEFILE(SKETCH_PATH,Device_091.c)
#endif

#ifdef DEVICE_092
#include DEVICEFILE(SKETCH_PATH,Device_092.c)
#endif

#ifdef DEVICE_093
#include DEVICEFILE(SKETCH_PATH,Device_093.c)
#endif

#ifdef DEVICE_094
#include DEVICEFILE(SKETCH_PATH,Device_094.c)
#endif

#ifdef DEVICE_095
#include DEVICEFILE(SKETCH_PATH,Device_095.c)
#endif

#ifdef DEVICE_096
#include DEVICEFILE(SKETCH_PATH,Device_096.c)
#endif

#ifdef DEVICE_097
#include DEVICEFILE(SKETCH_PATH,Device_097.c)
#endif

#ifdef DEVICE_098
#include DEVICEFILE(SKETCH_PATH,Device_098.c)
#endif

#ifdef DEVICE_099
#include DEVICEFILE(SKETCH_PATH,Device_099.c)
#endif

#ifdef DEVICE_100
#include DEVICEFILE(SKETCH_PATH,Device_100.c)
#endif

#ifdef DEVICE_255
#include DEVICEFILE(SKETCH_PATH,Device_255.c)
#endif

void DeviceInit(void)
  {
  byte x;

  // Wis te pointertabel voor de devices.
  for(x=0;x<DEVICE_MAX;x++)
    {
    Device_ptr[x]=0;
    Device_id[x]=0;
    }
    
  x=0;
        
  
  
  #ifdef DEVICE_001
  Device_id[x]=1;Device_ptr[x++]=&Device_001;
  #endif
  
  #ifdef DEVICE_002
  Device_id[x]=2;Device_ptr[x++]=&Device_002;
  #endif
  
  #ifdef DEVICE_003
  Device_id[x]=3;Device_ptr[x++]=&Device_003;
  #endif
  
  #ifdef DEVICE_004
  Device_id[x]=4;Device_ptr[x++]=&Device_004;
  #endif
  
  #ifdef DEVICE_005
  Device_id[x]=5;Device_ptr[x++]=&Device_005;
  #endif
  
  #ifdef DEVICE_006
  Device_id[x]=6;Device_ptr[x++]=&Device_006;
  #endif
  
  #ifdef DEVICE_007
  Device_id[x]=7;Device_ptr[x++]=&Device_007;
  #endif
  
  #ifdef DEVICE_008
  Device_id[x]=8;Device_ptr[x++]=&Device_008;
  #endif
  
  #ifdef DEVICE_009
  Device_id[x]=9;Device_ptr[x++]=&Device_009;
  #endif
  
  #ifdef DEVICE_010
  Device_id[x]=10;Device_ptr[x++]=&Device_010;
  #endif
  
  #ifdef DEVICE_011
  Device_id[x]=11;Device_ptr[x++]=&Device_011;
  #endif
  
  #ifdef DEVICE_012
  Device_id[x]=12;Device_ptr[x++]=&Device_012;
  #endif
  
  #ifdef DEVICE_013
  Device_id[x]=13;Device_ptr[x++]=&Device_013;
  #endif
  
  #ifdef DEVICE_014
  Device_id[x]=14;Device_ptr[x++]=&Device_014;
  #endif
  
  #ifdef DEVICE_015
  Device_id[x]=15;Device_ptr[x++]=&Device_015;
  #endif
  
  #ifdef DEVICE_016
  Device_id[x]=16;Device_ptr[x++]=&Device_016;
  #endif
  
  #ifdef DEVICE_017
  Device_id[x]=17;Device_ptr[x++]=&Device_017;
  #endif
  
  #ifdef DEVICE_018
  Device_id[x]=18;Device_ptr[x++]=&Device_018;
  #endif
  
  #ifdef DEVICE_019
  Device_id[x]=19;Device_ptr[x++]=&Device_019;
  #endif
  
  #ifdef DEVICE_020
  Device_id[x]=20;Device_ptr[x++]=&Device_020;
  #endif
  
  #ifdef DEVICE_021
  Device_id[x]=21;Device_ptr[x++]=&Device_021;
  #endif
  
  #ifdef DEVICE_022
  Device_id[x]=22;Device_ptr[x++]=&Device_022;
  #endif
  
  #ifdef DEVICE_023
  Device_id[x]=23;Device_ptr[x++]=&Device_023;
  #endif
  
  #ifdef DEVICE_024
  Device_id[x]=24;Device_ptr[x++]=&Device_024;
  #endif
  
  #ifdef DEVICE_025
  Device_id[x]=25;Device_ptr[x++]=&Device_025;
  #endif
  
  #ifdef DEVICE_026
  Device_id[x]=26;Device_ptr[x++]=&Device_026;
  #endif
  
  #ifdef DEVICE_027
  Device_id[x]=27;Device_ptr[x++]=&Device_027;
  #endif
  
  #ifdef DEVICE_028
  Device_id[x]=28;Device_ptr[x++]=&Device_028;
  #endif
  
  #ifdef DEVICE_029
  Device_id[x]=29;Device_ptr[x++]=&Device_029;
  #endif
  
  #ifdef DEVICE_030
  Device_id[x]=30;Device_ptr[x++]=&Device_030;
  #endif
  
  #ifdef DEVICE_031
  Device_id[x]=31;Device_ptr[x++]=&Device_031;
  #endif
  
  #ifdef DEVICE_032
  Device_id[x]=32;Device_ptr[x++]=&Device_032;
  #endif
  
  #ifdef DEVICE_033
  Device_id[x]=33;Device_ptr[x++]=&Device_033;
  #endif
  
  #ifdef DEVICE_034
  Device_id[x]=34;Device_ptr[x++]=&Device_034;
  #endif
  
  #ifdef DEVICE_035
  Device_id[x]=35;Device_ptr[x++]=&Device_035;
  #endif
  
  #ifdef DEVICE_036
  Device_id[x]=36;Device_ptr[x++]=&Device_036;
  #endif
  
  #ifdef DEVICE_037
  Device_id[x]=37;Device_ptr[x++]=&Device_037;
  #endif
  
  #ifdef DEVICE_038
  Device_id[x]=38;Device_ptr[x++]=&Device_038;
  #endif
  
  #ifdef DEVICE_039
  Device_id[x]=39;Device_ptr[x++]=&Device_039;
  #endif
  
  #ifdef DEVICE_040
  Device_id[x]=40;Device_ptr[x++]=&Device_040;
  #endif
  
  #ifdef DEVICE_041
  Device_id[x]=41;Device_ptr[x++]=&Device_041;
  #endif
  
  #ifdef DEVICE_042
  Device_id[x]=42;Device_ptr[x++]=&Device_042;
  #endif
  
  #ifdef DEVICE_043
  Device_id[x]=43;Device_ptr[x++]=&Device_043;
  #endif
  
  #ifdef DEVICE_044
  Device_id[x]=44;Device_ptr[x++]=&Device_044;
  #endif
  
  #ifdef DEVICE_045
  Device_id[x]=45;Device_ptr[x++]=&Device_045;
  #endif
  
  #ifdef DEVICE_046
  Device_id[x]=46;Device_ptr[x++]=&Device_046;
  #endif
  
  #ifdef DEVICE_047
  Device_id[x]=47;Device_ptr[x++]=&Device_047;
  #endif
  
  #ifdef DEVICE_048
  Device_id[x]=48;Device_ptr[x++]=&Device_048;
  #endif
  
  #ifdef DEVICE_049
  Device_id[x]=49;Device_ptr[x++]=&Device_049;
  #endif
  
  #ifdef DEVICE_050
  Device_id[x]=50;Device_ptr[x++]=&Device_050;
  #endif
  
  #ifdef DEVICE_051
  Device_id[x]=51;Device_ptr[x++]=&Device_051;
  #endif
  
  #ifdef DEVICE_052
  Device_id[x]=52;Device_ptr[x++]=&Device_052;
  #endif
  
  #ifdef DEVICE_053
  Device_id[x]=53;Device_ptr[x++]=&Device_053;
  #endif
  
  #ifdef DEVICE_054
  Device_id[x]=54;Device_ptr[x++]=&Device_054;
  #endif
  
  #ifdef DEVICE_055
  Device_id[x]=55;Device_ptr[x++]=&Device_055;
  #endif
  
  #ifdef DEVICE_056
  Device_id[x]=56;Device_ptr[x++]=&Device_056;
  #endif
  
  #ifdef DEVICE_057
  Device_id[x]=57;Device_ptr[x++]=&Device_057;
  #endif
  
  #ifdef DEVICE_058
  Device_id[x]=58;Device_ptr[x++]=&Device_058;
  #endif
  
  #ifdef DEVICE_059
  Device_id[x]=59;Device_ptr[x++]=&Device_059;
  #endif
  
  #ifdef DEVICE_060
  Device_id[x]=60;Device_ptr[x++]=&Device_060;
  #endif
  
  #ifdef DEVICE_061
  Device_id[x]=61;Device_ptr[x++]=&Device_061;
  #endif
  
  #ifdef DEVICE_062
  Device_id[x]=62;Device_ptr[x++]=&Device_062;
  #endif
  
  #ifdef DEVICE_063
  Device_id[x]=63;Device_ptr[x++]=&Device_063;
  #endif
  
  #ifdef DEVICE_064
  Device_id[x]=64;Device_ptr[x++]=&Device_064;
  #endif
  
  #ifdef DEVICE_065
  Device_id[x]=65;Device_ptr[x++]=&Device_065;
  #endif
  
  #ifdef DEVICE_066
  Device_id[x]=66;Device_ptr[x++]=&Device_066;
  #endif
  
  #ifdef DEVICE_067
  Device_id[x]=67;Device_ptr[x++]=&Device_067;
  #endif
  
  #ifdef DEVICE_068
  Device_id[x]=68;Device_ptr[x++]=&Device_068;
  #endif
  
  #ifdef DEVICE_069
  Device_id[x]=69;Device_ptr[x++]=&Device_069;
  #endif
  
  #ifdef DEVICE_070
  Device_id[x]=70;Device_ptr[x++]=&Device_070;
  #endif
  
  #ifdef DEVICE_071
  Device_id[x]=71;Device_ptr[x++]=&Device_071;
  #endif
  
  #ifdef DEVICE_072
  Device_id[x]=72;Device_ptr[x++]=&Device_072;
  #endif
  
  #ifdef DEVICE_073
  Device_id[x]=73;Device_ptr[x++]=&Device_073;
  #endif
  
  #ifdef DEVICE_074
  Device_id[x]=74;Device_ptr[x++]=&Device_074;
  #endif
  
  #ifdef DEVICE_075
  Device_id[x]=75;Device_ptr[x++]=&Device_075;
  #endif
  
  #ifdef DEVICE_076
  Device_id[x]=76;Device_ptr[x++]=&Device_076;
  #endif
  
  #ifdef DEVICE_077
  Device_id[x]=77;Device_ptr[x++]=&Device_077;
  #endif
  
  #ifdef DEVICE_078
  Device_id[x]=78;Device_ptr[x++]=&Device_078;
  #endif
  
  #ifdef DEVICE_079
  Device_id[x]=79;Device_ptr[x++]=&Device_079;
  #endif
  
  #ifdef DEVICE_080
  Device_id[x]=80;Device_ptr[x++]=&Device_080;
  #endif
  
  #ifdef DEVICE_081
  Device_id[x]=81;Device_ptr[x++]=&Device_081;
  #endif
  
  #ifdef DEVICE_082
  Device_id[x]=82;Device_ptr[x++]=&Device_082;
  #endif
  
  #ifdef DEVICE_083
  Device_id[x]=83;Device_ptr[x++]=&Device_083;
  #endif
  
  #ifdef DEVICE_084
  Device_id[x]=84;Device_ptr[x++]=&Device_084;
  #endif
  
  #ifdef DEVICE_085
  Device_id[x]=85;Device_ptr[x++]=&Device_085;
  #endif
  
  #ifdef DEVICE_086
  Device_id[x]=86;Device_ptr[x++]=&Device_086;
  #endif
  
  #ifdef DEVICE_087
  Device_id[x]=87;Device_ptr[x++]=&Device_087;
  #endif
  
  #ifdef DEVICE_088
  Device_id[x]=88;Device_ptr[x++]=&Device_088;
  #endif
  
  #ifdef DEVICE_089
  Device_id[x]=89;Device_ptr[x++]=&Device_089;
  #endif
  
  #ifdef DEVICE_090
  Device_id[x]=90;Device_ptr[x++]=&Device_090;
  #endif
  
  #ifdef DEVICE_091
  Device_id[x]=91;Device_ptr[x++]=&Device_091;
  #endif
  
  #ifdef DEVICE_092
  Device_id[x]=92;Device_ptr[x++]=&Device_092;
  #endif
  
  #ifdef DEVICE_093
  Device_id[x]=93;Device_ptr[x++]=&Device_093;
  #endif
  
  #ifdef DEVICE_094
  Device_id[x]=94;Device_ptr[x++]=&Device_094;
  #endif
  
  #ifdef DEVICE_095
  Device_id[x]=95;Device_ptr[x++]=&Device_095;
  #endif
  
  #ifdef DEVICE_096
  Device_id[x]=96;Device_ptr[x++]=&Device_096;
  #endif
  
  #ifdef DEVICE_097
  Device_id[x]=97;Device_ptr[x++]=&Device_097;
  #endif
  
  #ifdef DEVICE_098
  Device_id[x]=98;Device_ptr[x++]=&Device_098;
  #endif
  
  #ifdef DEVICE_099
  Device_id[x]=99;Device_ptr[x++]=&Device_099;
  #endif
  
  #ifdef DEVICE_100
  Device_id[x]=100;Device_ptr[x++]=&Device_100;
  #endif
  
  #ifdef DEVICE_255
  Device_id[x]=255;Device_ptr[x++]=&Device_255;
  #endif

  // Initialiseer alle devices door aanroep met verwerkingsparameter DEVICE_INIT
  for(byte x=0;Device_ptr[x]!=0 && x<DEVICE_MAX; x++)
    Device_ptr[x](DEVICE_INIT,0,0);
  }

 /*********************************************************************************************\
 * Met deze functie worden de devices aangeroepen. In Event->Command zit het nummer van het device dat moet
 * worden aangeroepen. Deze functie doorzoekt de ID en pointertabel en roept van hieruit het
 * juiste device aan. Als resultaat komt er een foutcode of 0 bij succes.
 * Als Event->Command=0 wordt opgegeven worden alle devices met de betreffende funktie call aangeroepen.
 * Als er een verzoek wordt gedaan om alle devices het Rawsignal te onderzoeken, dan wordt teruggekeerd
 * met een true als het eerste device een true geretourneerd heeft.
 \*********************************************************************************************/
byte DeviceCall(byte Function, struct NodoEventStruct *Event, char *str)
  {
  byte error=MESSAGE_DEVICE_UNKNOWN;

  for(byte x=0; x<DEVICE_MAX; x++)
    {
    // Zoek het device in de tabel en voer de device code uit.
    if(Device_ptr[x]!=0 && (Event->Command==0 || Device_id[x]==Event->Command) )    // Als device bestaat of alle devices moeten worden langsgelopen
      {
      error=0;
      if(!Device_ptr[x](Function,Event,str))
        error=MESSAGE_DEVICE_ERROR;
      else if(Function==DEVICE_RAWSIGNAL_IN)      // Als gechecked moet worden op een bruikbaar rawsignal, dan bij de eerste hit terugkeren
        return true;
      }
    }
  return error;
  }



