/****************************************************************************************************************************\
* In dit tabblad staan voorzieningen die nodig zijn om de devices te integreren in de Nodo code op een zodanige manier dat
* uitsluitend DIE devices worden meegecompileerd die ook daadwerklijk door de gebruiker zijn opgegeven. Niet opgegeven
* devices worden dus niet meegecompileerd om geheugen te besparen. Dit tabblad bevat geen code die door de gebruiker 
* aangepast hoeft te worden.
*
* Devics worden uniek genummerd. Deze nummers worden uitgegeven door het Nodo team. Device_255 is een template voor 
* een device dat kan worden gebruikt als je zelf een device wilt toevoegen aan de Nodo. 
\*************************************************************************************************************************/

#ifdef DEVICE_001_FILE
#include DEVICE_001_FILE
#endif

#ifdef DEVICE_002_FILE
#include DEVICE_002_FILE
#endif

#ifdef DEVICE_003_FILE
#include DEVICE_003_FILE
#endif

#ifdef DEVICE_004_FILE
#include DEVICE_004_FILE
#endif

#ifdef DEVICE_005_FILE
#include DEVICE_005_FILE
#endif

#ifdef DEVICE_006_FILE
#include DEVICE_006_FILE
#endif

#ifdef DEVICE_007_FILE
#include DEVICE_007_FILE
#endif

#ifdef DEVICE_008_FILE
#include DEVICE_008_FILE
#endif

#ifdef DEVICE_009_FILE
#include DEVICE_009_FILE
#endif

#ifdef DEVICE_010_FILE
#include DEVICE_010_FILE
#endif

#ifdef DEVICE_011_FILE
#include DEVICE_011_FILE
#endif

#ifdef DEVICE_012_FILE
#include DEVICE_012_FILE
#endif

#ifdef DEVICE_013_FILE
#include DEVICE_013_FILE
#endif

#ifdef DEVICE_014_FILE
#include DEVICE_014_FILE
#endif

#ifdef DEVICE_015_FILE
#include DEVICE_015_FILE
#endif

#ifdef DEVICE_016_FILE
#include DEVICE_016_FILE
#endif

#ifdef DEVICE_017_FILE
#include DEVICE_017_FILE
#endif

#ifdef DEVICE_018_FILE
#include DEVICE_018_FILE
#endif

#ifdef DEVICE_019_FILE
#include DEVICE_019_FILE
#endif

#ifdef DEVICE_020_FILE
#include DEVICE_020_FILE
#endif

#ifdef DEVICE_021_FILE
#include DEVICE_021_FILE
#endif

#ifdef DEVICE_022_FILE
#include DEVICE_022_FILE
#endif

#ifdef DEVICE_023_FILE
#include DEVICE_023_FILE
#endif

#ifdef DEVICE_024_FILE
#include DEVICE_024_FILE
#endif

#ifdef DEVICE_025_FILE
#include DEVICE_025_FILE
#endif

#ifdef DEVICE_255_FILE
#include DEVICE_255_FILE
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
      
  #ifdef DEVICE_001_FILE
  Device_id[x]=1;Device_ptr[x++]=&Device_001;
  #endif

  #ifdef DEVICE_002_FILE
  Device_id[x]=2;Device_ptr[x++]=&Device_002;
  #endif

  #ifdef DEVICE_003_FILE
  Device_id[x]=3;Device_ptr[x++]=&Device_003;
  #endif

  #ifdef DEVICE_004_FILE
  Device_id[x]=4;Device_ptr[x++]=&Device_004;
  #endif

  #ifdef DEVICE_005_FILE
  Device_id[x]=5;Device_ptr[x++]=&Device_005;
  #endif

  #ifdef DEVICE_006_FILE
  Device_id[x]=6;Device_ptr[x++]=&Device_006;
  #endif

  #ifdef DEVICE_007_FILE
  Device_id[x]=7;Device_ptr[x++]=&Device_007;
  #endif

  #ifdef DEVICE_008_FILE
  Device_id[x]=8;Device_ptr[x++]=&Device_008;
  #endif

  #ifdef DEVICE_009_FILE
  Device_id[x]=9;Device_ptr[x++]=&Device_009;
  #endif

  #ifdef DEVICE_010_FILE
  Device_id[x]=10;Device_ptr[x++]=&Device_010;
  #endif

  #ifdef DEVICE_011_FILE
  Device_id[x]=11;Device_ptr[x++]=&Device_011;
  #endif

  #ifdef DEVICE_012_FILE
  Device_id[x]=12;Device_ptr[x++]=&Device_012;
  #endif

  #ifdef DEVICE_013_FILE
  Device_id[x]=13;Device_ptr[x++]=&Device_013;
  #endif

  #ifdef DEVICE_014_FILE
  Device_id[x]=14;Device_ptr[x++]=&Device_014;
  #endif

  #ifdef DEVICE_015_FILE
  Device_id[x]=15;Device_ptr[x++]=&Device_015;
  #endif

  #ifdef DEVICE_016_FILE
  Device_id[x]=16;Device_ptr[x++]=&Device_016;
  #endif

  #ifdef DEVICE_017_FILE
  Device_id[x]=17;Device_ptr[x++]=&Device_017;
  #endif

  #ifdef DEVICE_018_FILE
  Device_id[x]=18;Device_ptr[x++]=&Device_018;
  #endif

  #ifdef DEVICE_019_FILE
  Device_id[x]=19;Device_ptr[x++]=&Device_019;
  #endif

  #ifdef DEVICE_020_FILE
  Device_id[x]=20;Device_ptr[x++]=&Device_020;
  #endif

  #ifdef DEVICE_021_FILE
  Device_id[x]=21;Device_ptr[x++]=&Device_021;
  #endif

  #ifdef DEVICE_022_FILE
  Device_id[x]=22;Device_ptr[x++]=&Device_022;
  #endif

  #ifdef DEVICE_023_FILE
  Device_id[x]=23;Device_ptr[x++]=&Device_023;
  #endif

  #ifdef DEVICE_024_FILE
  Device_id[x]=24;Device_ptr[x++]=&Device_024;
  #endif

  #ifdef DEVICE_025_FILE
  Device_id[x]=25;Device_ptr[x++]=&Device_025;
  #endif
  
  #ifdef DEVICE_255_FILE
  Device_id[x]=255;Device_ptr[x++]=&Device_255;
  #endif    

  // Initialiseer alle devices door aanroep met verwerkingsparameter DEVICE_INIT
  for(byte x=0;Device_ptr[x]!=0 && x<DEVICE_MAX; x++)
    Device_ptr[x](DEVICE_INIT,0,0);
  }






