
//#######################################################################################################
//#################################### Plugin-18: Signal Analyzer   #####################################
//#######################################################################################################

/*********************************************************************************************\
 * Dit device geeft een pulsenreeks weer die op RF of IR is binnengekomen. Het is bedoeld
 * als hulpmiddel om signalen te analyseren in geval je niet beschikt over een scope of
 * een logic analyzer. Signaal wordt alleen weergegeven op Serial/USB. Manchster laat alle
 * Mark & Spaces zien. 
 *
 * Auteur             : Paul Tonkes
 * Support            : www.nodo-domotica.nl
 * Datum              : 10-03-2013
 * Versie             : 0.1
 * Nodo productnummer : <Nodo productnummer. Toegekend door Nodo team>
 * Compatibiliteit    : R513
 * Syntax             : n.v.t
 *
 ***********************************************************************************************
 * Technische beschrijving:
 *
 * Compiled size      : 1200 bytes voor een Mega.
 * Externe funkties   : <geef hier aan welke funkties worden gebruikt. 
 * 
 \*********************************************************************************************/
 
#define PLUGIN_ID 18
#define PLUGIN_NAME "RawSignalAnalyze"

boolean Plugin_018(byte function, struct NodoEventStruct *event, char *string)
  {
  boolean success=false;
  
  switch(function)
    {
    case PLUGIN_RAWSIGNAL_IN:
      {
      if(RawSignal.Number<8)return false;
      
      int x;
      unsigned int y,z;
    
      // zoek naar de langste kortst puls en de kortste lange puls
      unsigned int MarkShort=50000;
      unsigned int MarkLong=0;
      for(x=5;x<RawSignal.Number;x+=2)
        {
        y=RawSignal.Pulses[x]*RawSignal.Multiply;
        if(y<MarkShort)
          MarkShort=y;
        if(y>MarkLong)
          MarkLong=y;
        }
      z=true;
      while(z)
        {
        z=false;
        for(x=5;x<RawSignal.Number;x+=2)
          {
          y=RawSignal.Pulses[x]*RawSignal.Multiply;
          if(y>MarkShort && y<(MarkShort+MarkShort/2))
            {
            MarkShort=y;
            z=true;
            }
          if(y<MarkLong && y>(MarkLong-MarkLong/2))
            {
            MarkLong=y;
            z=true;
            }
          }
        }
      unsigned int MarkMid=((MarkLong-MarkShort)/2)+MarkShort;
  
      // zoek naar de langste kortst puls en de kortste lange puls
      unsigned int SpaceShort=50000;
      unsigned int SpaceLong=0;
      for(x=4;x<RawSignal.Number;x+=2)
        {
        y=RawSignal.Pulses[x]*RawSignal.Multiply;
        if(y<SpaceShort)
          SpaceShort=y;
        if(y>SpaceLong)
          SpaceLong=y;
        }
      z=true;
      while(z)
        {
        z=false;
        for(x=4;x<RawSignal.Number;x+=2)
          {
          y=RawSignal.Pulses[x]*RawSignal.Multiply;
          if(y>SpaceShort && y<(SpaceShort+SpaceShort/2))
            {
            SpaceShort=y;
            z=true;
            }
          if(y<SpaceLong && y>(SpaceLong-SpaceLong/2))
            {
            SpaceLong=y;
            z=true;
            }
          }
        }
      int SpaceMid=((SpaceLong-SpaceShort)/2)+SpaceShort;
    
      // Bepaal soort signaal
      y=0;
      if(MarkLong  > (2*MarkShort  ))y=1; // PWM
      if(SpaceLong > (2*SpaceShort ))y+=2;// PDM

      Serial.print(F( "Bits="));

      if(y==0)Serial.println(F("?"));
      if(y==1)
        {
        for(x=1;x<RawSignal.Number;x+=2)
          {
          y=RawSignal.Pulses[x]*RawSignal.Multiply;
          if(y>MarkMid)
            Serial.write('1');
          else
            Serial.write('0');
          }
        Serial.print(F(", Type=PWM"));
        }
      if(y==2)
        {
        for(x=2;x<RawSignal.Number;x+=2)
          {
          y=RawSignal.Pulses[x]*RawSignal.Multiply;
          if(y>SpaceMid)
            Serial.write('1');
          else
            Serial.write('0');
          }
        Serial.print(F(", Type=PDM"));
        }
      if(y==3)
        {
        for(x=1;x<RawSignal.Number;x+=2)
          {
          y=RawSignal.Pulses[x]*RawSignal.Multiply;
          if(y>MarkMid)
            Serial.write('1');
          else
            Serial.write('0');
          
          y=RawSignal.Pulses[x+1]*RawSignal.Multiply;
          if(y>SpaceMid)
            Serial.write('1');
          else
            Serial.write('0');
          }
        Serial.print(F( ", Type=Manchester"));
        }

      Serial.print(F(", Pulses="));
      Serial.print(RawSignal.Number/2);

      Serial.print(F(", Pulses(uSec)="));      
      for(x=1;x<RawSignal.Number;x++)
        {
        Serial.print(RawSignal.Pulses[x]*RawSignal.Multiply); 
        Serial.write(',');       
        }
      Serial.println();
      
//      int dev=250;  
//      for(x=1;x<=RawSignal.Number;x+=2)
//        {
//        for(y=1+int(RawSignal.Pulses[x])*RawSignal.Multiply/dev; y;y--)
//          Serial.write('M');  // Mark  
//        for(y=1+int(RawSignal.Pulses[x+1])*RawSignal.Multiply/dev; y;y--)
//          Serial.write('_');  // Space  
//        }    
//      Serial.println();

      break;
      }      
    }      
  return success;
  }

