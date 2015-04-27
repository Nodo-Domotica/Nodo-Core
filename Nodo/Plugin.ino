
/****************************************************************************************************************************\
* In dit tabblad staan voorzieningen die nodig zijn om de plugins te integreren in de Nodo code op een zodanige manier dat
* uitsluitend DIE plugins worden meegecompileerd die ook daadwerklijk door de gebruiker zijn opgegeven. Niet opgegeven
* plugins worden dus niet meegecompileerd om geheugen te besparen. Dit tabblad bevat geen code die door de gebruiker 
* aangepast hoeft te worden.
*
* Devics worden uniek genummerd. Deze nummers worden uitgegeven door het Nodo team. Plugin_255 is een template voor 
* een plugin dat kan worden gebruikt als je zelf een plugin wilt toevoegen aan de Nodo. 
\*************************************************************************************************************************/

// Workaround voor plugins die nog niet zijn aangepast voor de 3.8 release
#if PLUGIN_37_COMPATIBILITY
float UserVar[USER_VARIABLES_MAX];
#endif
#define WIRED_PORTS HARDWARE_WIRED_OUT_PORTS



#define PLUGINFILE2(a, b) stringify(a/Plugins/b)
#define PLUGINFILE(a, b) PLUGINFILE2(a, b)

#if defined(PLUGIN_001) || defined(PLUGIN_001_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_001.c)
#endif

#if defined(PLUGIN_002) || defined(PLUGIN_002_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_002.c)
#endif

#if defined(PLUGIN_003) || defined(PLUGIN_003_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_003.c)
#endif

#if defined(PLUGIN_004) || defined(PLUGIN_004_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_004.c)
#endif

#if defined(PLUGIN_005) || defined(PLUGIN_005_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_005.c)
#endif

#if defined(PLUGIN_006) || defined(PLUGIN_006_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_006.c)
#endif

#if defined(PLUGIN_007) || defined(PLUGIN_007_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_007.c)
#endif

#if defined(PLUGIN_008) || defined(PLUGIN_008_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_008.c)
#endif

#if defined(PLUGIN_009) || defined(PLUGIN_009_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_009.c)
#endif

#if defined(PLUGIN_010) || defined(PLUGIN_010_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_010.c)
#endif

#if defined(PLUGIN_011) || defined(PLUGIN_011_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_011.c)
#endif

#if defined(PLUGIN_012) || defined(PLUGIN_012_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_012.c)
#endif

#if defined(PLUGIN_013) || defined(PLUGIN_013_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_013.c)
#endif

#if defined(PLUGIN_014) || defined(PLUGIN_014_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_014.c)
#endif

#if defined(PLUGIN_015) || defined(PLUGIN_015_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_015.c)
#endif

#if defined(PLUGIN_016) || defined(PLUGIN_016_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_016.c)
#endif

#if defined(PLUGIN_017) || defined(PLUGIN_017_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_017.c)
#endif

#if defined(PLUGIN_018) || defined(PLUGIN_018_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_018.c)
#endif

#if defined(PLUGIN_019) || defined(PLUGIN_019_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_019.c)
#endif

#if defined(PLUGIN_020) || defined(PLUGIN_020_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_020.c)
#endif

#if defined(PLUGIN_021) || defined(PLUGIN_021_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_021.c)
#endif

#if defined(PLUGIN_022) || defined(PLUGIN_022_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_022.c)
#endif

#if defined(PLUGIN_023) || defined(PLUGIN_023_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_023.c)
#endif

#if defined(PLUGIN_024) || defined(PLUGIN_024_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_024.c)
#endif

#if defined(PLUGIN_025) || defined(PLUGIN_025_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_025.c)
#endif

#if defined(PLUGIN_026) || defined(PLUGIN_026_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_026.c)
#endif

#if defined(PLUGIN_027) || defined(PLUGIN_027_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_027.c)
#endif

#if defined(PLUGIN_028) || defined(PLUGIN_028_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_028.c)
#endif

#if defined(PLUGIN_029) || defined(PLUGIN_029_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_029.c)
#endif

#if defined(PLUGIN_030) || defined(PLUGIN_030_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_030.c)
#endif

#if defined(PLUGIN_031) || defined(PLUGIN_031_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_031.c)
#endif

#if defined(PLUGIN_032) || defined(PLUGIN_032_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_032.c)
#endif

#if defined(PLUGIN_033) || defined(PLUGIN_033_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_033.c)
#endif

#if defined(PLUGIN_034) || defined(PLUGIN_034_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_034.c)
#endif

#if defined(PLUGIN_035) || defined(PLUGIN_035_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_035.c)
#endif

#if defined(PLUGIN_036) || defined(PLUGIN_036_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_036.c)
#endif

#if defined(PLUGIN_037) || defined(PLUGIN_037_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_037.c)
#endif

#if defined(PLUGIN_038) || defined(PLUGIN_038_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_038.c)
#endif

#if defined(PLUGIN_039) || defined(PLUGIN_039_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_039.c)
#endif

#if defined(PLUGIN_040) || defined(PLUGIN_040_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_040.c)
#endif

#if defined(PLUGIN_041) || defined(PLUGIN_041_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_041.c)
#endif

#if defined(PLUGIN_042) || defined(PLUGIN_042_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_042.c)
#endif

#if defined(PLUGIN_043) || defined(PLUGIN_043_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_043.c)
#endif

#if defined(PLUGIN_044) || defined(PLUGIN_044_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_044.c)
#endif

#if defined(PLUGIN_045) || defined(PLUGIN_045_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_045.c)
#endif

#if defined(PLUGIN_046) || defined(PLUGIN_046_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_046.c)
#endif

#if defined(PLUGIN_047) || defined(PLUGIN_047_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_047.c)
#endif

#if defined(PLUGIN_048) || defined(PLUGIN_048_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_048.c)
#endif

#if defined(PLUGIN_049) || defined(PLUGIN_049_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_049.c)
#endif

#if defined(PLUGIN_050) || defined(PLUGIN_050_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_050.c)
#endif

#if defined(PLUGIN_051) || defined(PLUGIN_051_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_051.c)
#endif

#if defined(PLUGIN_052) || defined(PLUGIN_052_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_052.c)
#endif

#if defined(PLUGIN_053) || defined(PLUGIN_053_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_053.c)
#endif

#if defined(PLUGIN_054) || defined(PLUGIN_054_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_054.c)
#endif

#if defined(PLUGIN_055) || defined(PLUGIN_055_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_055.c)
#endif

#if defined(PLUGIN_056) || defined(PLUGIN_056_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_056.c)
#endif

#if defined(PLUGIN_057) || defined(PLUGIN_057_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_057.c)
#endif

#if defined(PLUGIN_058) || defined(PLUGIN_058_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_058.c)
#endif

#if defined(PLUGIN_059) || defined(PLUGIN_059_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_059.c)
#endif

#if defined(PLUGIN_060) || defined(PLUGIN_060_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_060.c)
#endif

#if defined(PLUGIN_061) || defined(PLUGIN_061_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_061.c)
#endif

#if defined(PLUGIN_062) || defined(PLUGIN_062_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_062.c)
#endif

#if defined(PLUGIN_063) || defined(PLUGIN_063_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_063.c)
#endif

#if defined(PLUGIN_064) || defined(PLUGIN_064_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_064.c)
#endif

#if defined(PLUGIN_065) || defined(PLUGIN_065_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_065.c)
#endif

#if defined(PLUGIN_066) || defined(PLUGIN_066_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_066.c)
#endif

#if defined(PLUGIN_067) || defined(PLUGIN_067_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_067.c)
#endif

#if defined(PLUGIN_068) || defined(PLUGIN_068_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_068.c)
#endif

#if defined(PLUGIN_069) || defined(PLUGIN_069_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_069.c)
#endif

#if defined(PLUGIN_070) || defined(PLUGIN_070_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_070.c)
#endif

#if defined(PLUGIN_071) || defined(PLUGIN_071_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_071.c)
#endif

#if defined(PLUGIN_072) || defined(PLUGIN_072_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_072.c)
#endif

#if defined(PLUGIN_073) || defined(PLUGIN_073_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_073.c)
#endif

#if defined(PLUGIN_074) || defined(PLUGIN_074_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_074.c)
#endif

#if defined(PLUGIN_075) || defined(PLUGIN_075_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_075.c)
#endif

#if defined(PLUGIN_076) || defined(PLUGIN_076_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_076.c)
#endif

#if defined(PLUGIN_077) || defined(PLUGIN_077_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_077.c)
#endif

#if defined(PLUGIN_078) || defined(PLUGIN_078_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_078.c)
#endif

#if defined(PLUGIN_079) || defined(PLUGIN_079_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_079.c)
#endif

#if defined(PLUGIN_080) || defined(PLUGIN_080_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_080.c)
#endif

#if defined(PLUGIN_081) || defined(PLUGIN_081_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_081.c)
#endif

#if defined(PLUGIN_082) || defined(PLUGIN_082_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_082.c)
#endif

#if defined(PLUGIN_083) || defined(PLUGIN_083_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_083.c)
#endif

#if defined(PLUGIN_084) || defined(PLUGIN_084_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_084.c)
#endif

#if defined(PLUGIN_085) || defined(PLUGIN_085_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_085.c)
#endif

#if defined(PLUGIN_086) || defined(PLUGIN_086_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_086.c)
#endif

#if defined(PLUGIN_087) || defined(PLUGIN_087_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_087.c)
#endif

#if defined(PLUGIN_088) || defined(PLUGIN_088_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_088.c)
#endif

#if defined(PLUGIN_089) || defined(PLUGIN_089_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_089.c)
#endif

#if defined(PLUGIN_090) || defined(PLUGIN_090_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_090.c)
#endif

#if defined(PLUGIN_091) || defined(PLUGIN_091_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_091.c)
#endif

#if defined(PLUGIN_092) || defined(PLUGIN_092_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_092.c)
#endif

#if defined(PLUGIN_093) || defined(PLUGIN_093_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_093.c)
#endif

#if defined(PLUGIN_094) || defined(PLUGIN_094_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_094.c)
#endif

#if defined(PLUGIN_095) || defined(PLUGIN_095_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_095.c)
#endif

#if defined(PLUGIN_096) || defined(PLUGIN_096_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_096.c)
#endif

#if defined(PLUGIN_097) || defined(PLUGIN_097_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_097.c)
#endif

#if defined(PLUGIN_098) || defined(PLUGIN_098_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_098.c)
#endif

#if defined(PLUGIN_099) || defined(PLUGIN_099_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_099.c)
#endif

#if defined(PLUGIN_100) || defined(PLUGIN_100_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_100.c)
#endif

#if defined(PLUGIN_250) || defined(PLUGIN_250_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_250.c)
#endif

#if defined(PLUGIN_251) || defined(PLUGIN_251_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_251.c)
#endif

#if defined(PLUGIN_252) || defined(PLUGIN_252_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_252.c)
#endif

#if defined(PLUGIN_253) || defined(PLUGIN_253_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_253.c)
#endif

#if defined(PLUGIN_254) || defined(PLUGIN_254_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_254.c)
#endif

#if defined(PLUGIN_255) || defined(PLUGIN_255_CORE)
#include PLUGINFILE(SKETCH_PATH,Plugin_255.c)
#endif

void PluginInit(void)
  {
  byte x;

  // Wis te pointertabel voor de plugins.
  for(x=0;x<PLUGIN_MAX;x++)
    {
    Plugin_ptr[x]=0;
    Plugin_id[x]=0;
    }
    
  x=0;
        
  #if defined(PLUGIN_001) || defined(PLUGIN_001_CORE)
  Plugin_id[x]=1;Plugin_ptr[x++]=&Plugin_001;
  #endif
  
  #if defined(PLUGIN_002) || defined(PLUGIN_002_CORE)
  Plugin_id[x]=2;Plugin_ptr[x++]=&Plugin_002;
  #endif
  
  #if defined(PLUGIN_003) || defined(PLUGIN_003_CORE)
  Plugin_id[x]=3;Plugin_ptr[x++]=&Plugin_003;
  #endif
  
  #if defined(PLUGIN_004) || defined(PLUGIN_004_CORE)
  Plugin_id[x]=4;Plugin_ptr[x++]=&Plugin_004;
  #endif
  
  #if defined(PLUGIN_005) || defined(PLUGIN_005_CORE)
  Plugin_id[x]=5;Plugin_ptr[x++]=&Plugin_005;
  #endif
  
  #if defined(PLUGIN_006) || defined(PLUGIN_006_CORE)
  Plugin_id[x]=6;Plugin_ptr[x++]=&Plugin_006;
  #endif
  
  #if defined(PLUGIN_007) || defined(PLUGIN_007_CORE)
  Plugin_id[x]=7;Plugin_ptr[x++]=&Plugin_007;
  #endif
  
  #if defined(PLUGIN_008) || defined(PLUGIN_008_CORE)
  Plugin_id[x]=8;Plugin_ptr[x++]=&Plugin_008;
  #endif
  
  #if defined(PLUGIN_009) || defined(PLUGIN_009_CORE)
  Plugin_id[x]=9;Plugin_ptr[x++]=&Plugin_009;
  #endif
  
  #if defined(PLUGIN_010) || defined(PLUGIN_010_CORE)
  Plugin_id[x]=10;Plugin_ptr[x++]=&Plugin_010;
  #endif
  
  #if defined(PLUGIN_011) || defined(PLUGIN_011_CORE)
  Plugin_id[x]=11;Plugin_ptr[x++]=&Plugin_011;
  #endif
  
  #if defined(PLUGIN_012) || defined(PLUGIN_012_CORE)
  Plugin_id[x]=12;Plugin_ptr[x++]=&Plugin_012;
  #endif
  
  #if defined(PLUGIN_013) || defined(PLUGIN_013_CORE)
  Plugin_id[x]=13;Plugin_ptr[x++]=&Plugin_013;
  #endif
  
  #if defined(PLUGIN_014) || defined(PLUGIN_014_CORE)
  Plugin_id[x]=14;Plugin_ptr[x++]=&Plugin_014;
  #endif
  
  #if defined(PLUGIN_015) || defined(PLUGIN_015_CORE)
  Plugin_id[x]=15;Plugin_ptr[x++]=&Plugin_015;
  #endif
  
  #if defined(PLUGIN_016) || defined(PLUGIN_016_CORE)
  Plugin_id[x]=16;Plugin_ptr[x++]=&Plugin_016;
  #endif
  
  #if defined(PLUGIN_017) || defined(PLUGIN_017_CORE)
  Plugin_id[x]=17;Plugin_ptr[x++]=&Plugin_017;
  #endif
  
  #if defined(PLUGIN_018) || defined(PLUGIN_018_CORE)
  Plugin_id[x]=18;Plugin_ptr[x++]=&Plugin_018;
  #endif
  
  #if defined(PLUGIN_019) || defined(PLUGIN_019_CORE)
  Plugin_id[x]=19;Plugin_ptr[x++]=&Plugin_019;
  #endif
  
  #if defined(PLUGIN_020) || defined(PLUGIN_020_CORE)
  Plugin_id[x]=20;Plugin_ptr[x++]=&Plugin_020;
  #endif
  
  #if defined(PLUGIN_021) || defined(PLUGIN_021_CORE)
  Plugin_id[x]=21;Plugin_ptr[x++]=&Plugin_021;
  #endif
  
  #if defined(PLUGIN_022) || defined(PLUGIN_022_CORE)
  Plugin_id[x]=22;Plugin_ptr[x++]=&Plugin_022;
  #endif
  
  #if defined(PLUGIN_023) || defined(PLUGIN_023_CORE)
  Plugin_id[x]=23;Plugin_ptr[x++]=&Plugin_023;
  #endif
  
  #if defined(PLUGIN_024) || defined(PLUGIN_024_CORE)
  Plugin_id[x]=24;Plugin_ptr[x++]=&Plugin_024;
  #endif
  
  #if defined(PLUGIN_025) || defined(PLUGIN_025_CORE)
  Plugin_id[x]=25;Plugin_ptr[x++]=&Plugin_025;
  #endif
  
  #if defined(PLUGIN_026) || defined(PLUGIN_026_CORE)
  Plugin_id[x]=26;Plugin_ptr[x++]=&Plugin_026;
  #endif
  
  #if defined(PLUGIN_027) || defined(PLUGIN_027_CORE)
  Plugin_id[x]=27;Plugin_ptr[x++]=&Plugin_027;
  #endif
  
  #if defined(PLUGIN_028) || defined(PLUGIN_028_CORE)
  Plugin_id[x]=28;Plugin_ptr[x++]=&Plugin_028;
  #endif
  
  #if defined(PLUGIN_029) || defined(PLUGIN_029_CORE)
  Plugin_id[x]=29;Plugin_ptr[x++]=&Plugin_029;
  #endif
  
  #if defined(PLUGIN_030) || defined(PLUGIN_030_CORE)
  Plugin_id[x]=30;Plugin_ptr[x++]=&Plugin_030;
  #endif
  
  #if defined(PLUGIN_031) || defined(PLUGIN_031_CORE)
  Plugin_id[x]=31;Plugin_ptr[x++]=&Plugin_031;
  #endif
  
  #if defined(PLUGIN_032) || defined(PLUGIN_032_CORE)
  Plugin_id[x]=32;Plugin_ptr[x++]=&Plugin_032;
  #endif
  
  #if defined(PLUGIN_033) || defined(PLUGIN_033_CORE)
  Plugin_id[x]=33;Plugin_ptr[x++]=&Plugin_033;
  #endif
  
  #if defined(PLUGIN_034) || defined(PLUGIN_034_CORE)
  Plugin_id[x]=34;Plugin_ptr[x++]=&Plugin_034;
  #endif
  
  #if defined(PLUGIN_035) || defined(PLUGIN_035_CORE)
  Plugin_id[x]=35;Plugin_ptr[x++]=&Plugin_035;
  #endif
  
  #if defined(PLUGIN_036) || defined(PLUGIN_036_CORE)
  Plugin_id[x]=36;Plugin_ptr[x++]=&Plugin_036;
  #endif
  
  #if defined(PLUGIN_037) || defined(PLUGIN_037_CORE)
  Plugin_id[x]=37;Plugin_ptr[x++]=&Plugin_037;
  #endif
  
  #if defined(PLUGIN_038) || defined(PLUGIN_038_CORE)
  Plugin_id[x]=38;Plugin_ptr[x++]=&Plugin_038;
  #endif
  
  #if defined(PLUGIN_039) || defined(PLUGIN_039_CORE)
  Plugin_id[x]=39;Plugin_ptr[x++]=&Plugin_039;
  #endif
  
  #if defined(PLUGIN_040) || defined(PLUGIN_040_CORE)
  Plugin_id[x]=40;Plugin_ptr[x++]=&Plugin_040;
  #endif
  
  #if defined(PLUGIN_041) || defined(PLUGIN_041_CORE)
  Plugin_id[x]=41;Plugin_ptr[x++]=&Plugin_041;
  #endif
  
  #if defined(PLUGIN_042) || defined(PLUGIN_042_CORE)
  Plugin_id[x]=42;Plugin_ptr[x++]=&Plugin_042;
  #endif
  
  #if defined(PLUGIN_043) || defined(PLUGIN_043_CORE)
  Plugin_id[x]=43;Plugin_ptr[x++]=&Plugin_043;
  #endif
  
  #if defined(PLUGIN_044) || defined(PLUGIN_044_CORE)
  Plugin_id[x]=44;Plugin_ptr[x++]=&Plugin_044;
  #endif
  
  #if defined(PLUGIN_045) || defined(PLUGIN_045_CORE)
  Plugin_id[x]=45;Plugin_ptr[x++]=&Plugin_045;
  #endif
  
  #if defined(PLUGIN_046) || defined(PLUGIN_046_CORE)
  Plugin_id[x]=46;Plugin_ptr[x++]=&Plugin_046;
  #endif
  
  #if defined(PLUGIN_047) || defined(PLUGIN_047_CORE)
  Plugin_id[x]=47;Plugin_ptr[x++]=&Plugin_047;
  #endif
  
  #if defined(PLUGIN_048) || defined(PLUGIN_048_CORE)
  Plugin_id[x]=48;Plugin_ptr[x++]=&Plugin_048;
  #endif
  
  #if defined(PLUGIN_049) || defined(PLUGIN_049_CORE)
  Plugin_id[x]=49;Plugin_ptr[x++]=&Plugin_049;
  #endif
  
  #if defined(PLUGIN_050) || defined(PLUGIN_050_CORE)
  Plugin_id[x]=50;Plugin_ptr[x++]=&Plugin_050;
  #endif
  
  #if defined(PLUGIN_051) || defined(PLUGIN_051_CORE)
  Plugin_id[x]=51;Plugin_ptr[x++]=&Plugin_051;
  #endif
  
  #if defined(PLUGIN_052) || defined(PLUGIN_052_CORE)
  Plugin_id[x]=52;Plugin_ptr[x++]=&Plugin_052;
  #endif
  
  #if defined(PLUGIN_053) || defined(PLUGIN_053_CORE)
  Plugin_id[x]=53;Plugin_ptr[x++]=&Plugin_053;
  #endif
  
  #if defined(PLUGIN_054) || defined(PLUGIN_054_CORE)
  Plugin_id[x]=54;Plugin_ptr[x++]=&Plugin_054;
  #endif
  
  #if defined(PLUGIN_055) || defined(PLUGIN_055_CORE)
  Plugin_id[x]=55;Plugin_ptr[x++]=&Plugin_055;
  #endif
  
  #if defined(PLUGIN_056) || defined(PLUGIN_056_CORE)
  Plugin_id[x]=56;Plugin_ptr[x++]=&Plugin_056;
  #endif
  
  #if defined(PLUGIN_057) || defined(PLUGIN_057_CORE)
  Plugin_id[x]=57;Plugin_ptr[x++]=&Plugin_057;
  #endif
  
  #if defined(PLUGIN_058) || defined(PLUGIN_058_CORE)
  Plugin_id[x]=58;Plugin_ptr[x++]=&Plugin_058;
  #endif
  
  #if defined(PLUGIN_059) || defined(PLUGIN_059_CORE)
  Plugin_id[x]=59;Plugin_ptr[x++]=&Plugin_059;
  #endif
  
  #if defined(PLUGIN_060) || defined(PLUGIN_060_CORE)
  Plugin_id[x]=60;Plugin_ptr[x++]=&Plugin_060;
  #endif
  
  #if defined(PLUGIN_061) || defined(PLUGIN_061_CORE)
  Plugin_id[x]=61;Plugin_ptr[x++]=&Plugin_061;
  #endif
  
  #if defined(PLUGIN_062) || defined(PLUGIN_062_CORE)
  Plugin_id[x]=62;Plugin_ptr[x++]=&Plugin_062;
  #endif
  
  #if defined(PLUGIN_063) || defined(PLUGIN_063_CORE)
  Plugin_id[x]=63;Plugin_ptr[x++]=&Plugin_063;
  #endif
  
  #if defined(PLUGIN_064) || defined(PLUGIN_064_CORE)
  Plugin_id[x]=64;Plugin_ptr[x++]=&Plugin_064;
  #endif
  
  #if defined(PLUGIN_065) || defined(PLUGIN_065_CORE)
  Plugin_id[x]=65;Plugin_ptr[x++]=&Plugin_065;
  #endif
  
  #if defined(PLUGIN_066) || defined(PLUGIN_066_CORE)
  Plugin_id[x]=66;Plugin_ptr[x++]=&Plugin_066;
  #endif
  
  #if defined(PLUGIN_067) || defined(PLUGIN_067_CORE)
  Plugin_id[x]=67;Plugin_ptr[x++]=&Plugin_067;
  #endif
  
  #if defined(PLUGIN_068) || defined(PLUGIN_068_CORE)
  Plugin_id[x]=68;Plugin_ptr[x++]=&Plugin_068;
  #endif
  
  #if defined(PLUGIN_069) || defined(PLUGIN_069_CORE)
  Plugin_id[x]=69;Plugin_ptr[x++]=&Plugin_069;
  #endif
  
  #if defined(PLUGIN_070) || defined(PLUGIN_070_CORE)
  Plugin_id[x]=70;Plugin_ptr[x++]=&Plugin_070;
  #endif
  
  #if defined(PLUGIN_071) || defined(PLUGIN_071_CORE)
  Plugin_id[x]=71;Plugin_ptr[x++]=&Plugin_071;
  #endif
  
  #if defined(PLUGIN_072) || defined(PLUGIN_072_CORE)
  Plugin_id[x]=72;Plugin_ptr[x++]=&Plugin_072;
  #endif
  
  #if defined(PLUGIN_073) || defined(PLUGIN_073_CORE)
  Plugin_id[x]=73;Plugin_ptr[x++]=&Plugin_073;
  #endif
  
  #if defined(PLUGIN_074) || defined(PLUGIN_074_CORE)
  Plugin_id[x]=74;Plugin_ptr[x++]=&Plugin_074;
  #endif
  
  #if defined(PLUGIN_075) || defined(PLUGIN_075_CORE)
  Plugin_id[x]=75;Plugin_ptr[x++]=&Plugin_075;
  #endif
  
  #if defined(PLUGIN_076) || defined(PLUGIN_076_CORE)
  Plugin_id[x]=76;Plugin_ptr[x++]=&Plugin_076;
  #endif
  
  #if defined(PLUGIN_077) || defined(PLUGIN_077_CORE)
  Plugin_id[x]=77;Plugin_ptr[x++]=&Plugin_077;
  #endif
  
  #if defined(PLUGIN_078) || defined(PLUGIN_078_CORE)
  Plugin_id[x]=78;Plugin_ptr[x++]=&Plugin_078;
  #endif
  
  #if defined(PLUGIN_079) || defined(PLUGIN_079_CORE)
  Plugin_id[x]=79;Plugin_ptr[x++]=&Plugin_079;
  #endif
  
  #if defined(PLUGIN_080) || defined(PLUGIN_080_CORE)
  Plugin_id[x]=80;Plugin_ptr[x++]=&Plugin_080;
  #endif
  
  #if defined(PLUGIN_081) || defined(PLUGIN_081_CORE)
  Plugin_id[x]=81;Plugin_ptr[x++]=&Plugin_081;
  #endif
  
  #if defined(PLUGIN_082) || defined(PLUGIN_082_CORE)
  Plugin_id[x]=82;Plugin_ptr[x++]=&Plugin_082;
  #endif
  
  #if defined(PLUGIN_083) || defined(PLUGIN_083_CORE)
  Plugin_id[x]=83;Plugin_ptr[x++]=&Plugin_083;
  #endif
  
  #if defined(PLUGIN_084) || defined(PLUGIN_084_CORE)
  Plugin_id[x]=84;Plugin_ptr[x++]=&Plugin_084;
  #endif
  
  #if defined(PLUGIN_085) || defined(PLUGIN_085_CORE)
  Plugin_id[x]=85;Plugin_ptr[x++]=&Plugin_085;
  #endif
  
  #if defined(PLUGIN_086) || defined(PLUGIN_086_CORE)
  Plugin_id[x]=86;Plugin_ptr[x++]=&Plugin_086;
  #endif
  
  #if defined(PLUGIN_087) || defined(PLUGIN_087_CORE)
  Plugin_id[x]=87;Plugin_ptr[x++]=&Plugin_087;
  #endif
  
  #if defined(PLUGIN_088) || defined(PLUGIN_088_CORE)
  Plugin_id[x]=88;Plugin_ptr[x++]=&Plugin_088;
  #endif
  
  #if defined(PLUGIN_089) || defined(PLUGIN_089_CORE)
  Plugin_id[x]=89;Plugin_ptr[x++]=&Plugin_089;
  #endif
  
  #if defined(PLUGIN_090) || defined(PLUGIN_090_CORE)
  Plugin_id[x]=90;Plugin_ptr[x++]=&Plugin_090;
  #endif
  
  #if defined(PLUGIN_091) || defined(PLUGIN_091_CORE)
  Plugin_id[x]=91;Plugin_ptr[x++]=&Plugin_091;
  #endif
  
  #if defined(PLUGIN_092) || defined(PLUGIN_092_CORE)
  Plugin_id[x]=92;Plugin_ptr[x++]=&Plugin_092;
  #endif
  
  #if defined(PLUGIN_093) || defined(PLUGIN_093_CORE)
  Plugin_id[x]=93;Plugin_ptr[x++]=&Plugin_093;
  #endif
  
  #if defined(PLUGIN_094) || defined(PLUGIN_094_CORE)
  Plugin_id[x]=94;Plugin_ptr[x++]=&Plugin_094;
  #endif
  
  #if defined(PLUGIN_095) || defined(PLUGIN_095_CORE)
  Plugin_id[x]=95;Plugin_ptr[x++]=&Plugin_095;
  #endif
  
  #if defined(PLUGIN_096) || defined(PLUGIN_096_CORE)
  Plugin_id[x]=96;Plugin_ptr[x++]=&Plugin_096;
  #endif
  
  #if defined(PLUGIN_097) || defined(PLUGIN_097_CORE)
  Plugin_id[x]=97;Plugin_ptr[x++]=&Plugin_097;
  #endif
  
  #if defined(PLUGIN_098) || defined(PLUGIN_098_CORE)
  Plugin_id[x]=98;Plugin_ptr[x++]=&Plugin_098;
  #endif
  
  #if defined(PLUGIN_099) || defined(PLUGIN_099_CORE)
  Plugin_id[x]=99;Plugin_ptr[x++]=&Plugin_099;
  #endif
  
  #if defined(PLUGIN_100) || defined(PLUGIN_100_CORE)
  Plugin_id[x]=100;Plugin_ptr[x++]=&Plugin_100;
  #endif
  
  #if defined(PLUGIN_250) || defined(PLUGIN_250_CORE)
  Plugin_id[x]=250;Plugin_ptr[x++]=&Plugin_250;
  #endif

  #if defined(PLUGIN_251) || defined(PLUGIN_251_CORE)
  Plugin_id[x]=251;Plugin_ptr[x++]=&Plugin_251;
  #endif

  #if defined(PLUGIN_252) || defined(PLUGIN_252_CORE)
  Plugin_id[x]=252;Plugin_ptr[x++]=&Plugin_252;
  #endif

  #if defined(PLUGIN_253) || defined(PLUGIN_253_CORE)
  Plugin_id[x]=253;Plugin_ptr[x++]=&Plugin_253;
  #endif

  #if defined(PLUGIN_254) || defined(PLUGIN_254_CORE)
  Plugin_id[x]=254;Plugin_ptr[x++]=&Plugin_254;
  #endif

  #if defined(PLUGIN_255) || defined(PLUGIN_255_CORE)
  Plugin_id[x]=255;Plugin_ptr[x++]=&Plugin_255;
  #endif

  // Initialiseer alle plugins door aanroep met verwerkingsparameter PLUGIN_INIT
  PluginCall(PLUGIN_INIT,0,0);
  }

 /*********************************************************************************************\
 * Met deze functie worden de plugins aangeroepen. In Event->Command zit het nummer van het plugin dat moet
 * worden aangeroepen. Deze functie doorzoekt de ID en pointertabel en roept van hieruit het
 * juiste plugin aan. Als resultaat komt er true bij succes.
 * Als er een verzoek wordt gedaan om alle plugins het Rawsignal te onderzoeken, dan wordt teruggekeerd
 * met een true als het eerste plugin een true geretourneerd heeft.
 \*********************************************************************************************/

byte PluginCall(byte Function, struct NodoEventStruct *Event, char *str)
  {
  int x;

  #if PLUGIN_37_COMPATIBILITY
  if(Function==PLUGIN_INIT)
    for(byte x=0;x<USER_VARIABLES_MAX;x++)
      UserVar[x]=999999.0;

  if(Function==PLUGIN_ONCE_A_SECOND)
    for(byte x=0;x<USER_VARIABLES_MAX;x++)
      if(UserVar[x]!=999999.0)
        UserVariableSet(x+1,UserVar[x],false);

  #endif


  switch(Function)
    {
    // Alle plugins langslopen, geen messages genereren.
    case PLUGIN_ONCE_A_SECOND:
    case PLUGIN_EVENT_IN:
    case PLUGIN_EVENT_OUT:
    case PLUGIN_INIT:
      for(x=0; x<PLUGIN_MAX; x++)
        if(Plugin_id[x]!=0)
          Plugin_ptr[x](Function,Event,str);
      return true;
      break;
    
    // Alle plugins langslopen. Na de eerste hit direct terugkeren met returnwaarde true, geen messages genereren.
    case PLUGIN_MMI_IN:
    
    #if HARDWARE_RAWSIGNAL || HARDWARE_RF433 || HARDWARE_INFRARED
    case PLUGIN_RAWSIGNAL_IN:
    #endif
    
    #if HARDWARE_SERIAL_1
    case PLUGIN_SERIAL_IN:
    #endif
    
    case PLUGIN_ETHERNET_IN:
      for(x=0; x<PLUGIN_MAX; x++)
        if(Plugin_id[x]!=0)
          if(Plugin_ptr[x](Function,Event,str))
            return true;
      break;

    // alleen specifieke plugin aanroepen zoals opgegeven in Event->Command. Bij terugkeer false, geef messsage en return met false;
    case PLUGIN_COMMAND:
      for(x=0; x<PLUGIN_MAX; x++)
        {
        if(Plugin_id[x]==Event->Command)
          {
          if(!Plugin_ptr[x](Function,Event,str))
            {
            RaiseMessage(MESSAGE_PLUGIN_ERROR,Plugin_id[x]);
            return false;
            }
          else
            {
            if(Event->Command && Event->Type!=NODO_TYPE_PLUGIN_COMMAND)
              {
              Event->Direction    = VALUE_DIRECTION_INPUT;
              Event->Port         = VALUE_SOURCE_PLUGIN;
              return ProcessEvent(Event);
              }
            return true;
            }
          }
        }
      RaiseMessage(MESSAGE_PLUGIN_UNKNOWN,Event->Command);
      return false;
      break;

    // alleen plugin aanroepen zoals opgegeven in Event->Command. Keer terug zonder message
    case PLUGIN_MMI_OUT:
      for(x=0; x<PLUGIN_MAX; x++)
        if(Plugin_id[x]==Event->Command)
          {
          Plugin_ptr[x](Function,Event,str);
          return true; // Plugin gevonden
          }

      break;
    }// case
  return false;
  }
