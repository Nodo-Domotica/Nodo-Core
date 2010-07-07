/**************************************************************************\

    This file is part of Nodo Due, © Copyright Paul Tonkes, Kenneth Rovers

    Nodo Due is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Nodo Due is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Nodo Due.  If not, see <http://www.gnu.org/licenses/>.

\**************************************************************************/

// BaseCode
// Hierbij word de huiscode opgeteld voor het genereren van de nieuwe 26-bits KAKU code.
// Bij ontvangst of decoderen wordt de basecode er juist afgehaald.
// Voor het achterhalen van de basecode van een afstandsbediening: uncomment de print 
// regel bij Rawsignal2NewKAKU
#define BaseCode 0


/*

 Hier bevinden zich de de routines voor verwerken van de KAKU specifieke signalen:
 
  Conventionele KAKU signalen:
  * KAKU_2_RawSignal()        Zet 32-bit event van type CMD_KAKU om naar een pulsenreeks in de RawBuffer t.b.v. verzending via RF.
  * RawSignal_2_KAKU()        Zet een pulsenreeks die zich in de RawBuffer bevindt om naar een 32-bit CMD_KAKU event

  KAKU signalen volgens de nieuwe codering:
  * RawSignal_2_NewKAKU()     Zet een pulsenreeks die zich in de RawBuffer bevindt om naar een 32-bit CMD_KAKU_NEW event
  * NewKAKU_2_RawSignal()     Zet 32-bit event van type CMD_KAKU_NEW om naar een pulsenreeks in de RawBuffer t.b.v. verzending via RF.

*/


/*********************************************************************************************\
* KAKU
* Encoding volgens Princeton PT2262 / MOSDESIGN M3EB / Domia Lite spec
* Pulse (T) is 350us PWDM
* 0 = T,3T,T,3T, 1 = T,3T,3T,T, short 0 = T,3T,T,T
*
* KAKU ondersteund:
*   on/off       ---- 000x Off/On
*   all on/off   ---- 001x AllOff/AllOn // is group (unit code bestaat uit short 0s)
*   bright dim   ---- 010x Dim/Bright   // niet ondersteund
*   dim absolute xxxx 0110 Dim16        // niet ondersteund
\*********************************************************************************************/
#define KAKU_CodeLength    12  // aantal data bits
#define KAKU_T            350  // us
#define KAKU_nP             1  // pulse
#define KAKU_nS             1  // short pulse
#define KAKU_nL             3  // long pulse

/*********************************************************************************************\
* Deze routine berekent de RAW pulsen uit een CMD_KAKU plaatst deze in de buffer RawSignal
\*********************************************************************************************/
void KAKU_2_RawSignal(unsigned long Code)
{
  byte Home, Unit, Level, Command;
  boolean Group;
  int i;
  byte n;
  boolean b;

  // CMD_KAKU = NNxxHU0C, NN=Nodo Unit, xx=CMD_KAKU, H=Home, U=Unit, C=Commando
  Home    = (Code >> 12) & 0xF;
  Unit    = (Code >>  8) & 0xF;
  Command = (Code      ) & 0x1;
  Group   = (Code & CMD_ALLOFF) == CMD_ALLOFF;
  Code = Home | Unit << 4 | (0x600 | (Command << 11));

  RawSignal[0] = 4+(KAKU_CodeLength*4);
  RawSignal[1] = 0; //KAKU_T;  // no start sync
  RawSignal[2] = 0; //10*KAKU_T;
  for (i=0; i<KAKU_CodeLength; i++) {
    encodePWDM(2*i+1, KAKU_T, KAKU_nP, KAKU_nS, KAKU_nL, 0);
    n = KAKU_nL;
    b = (Code >> i) & 1;
    if (Group && i>=4 && i<8) { b = 0; n = KAKU_nS; } // short 0
    encodePWDM(2*i+2, KAKU_T, KAKU_nP, KAKU_nS, n, b);
  }
  RawSignal[3+(KAKU_CodeLength*4)] = KAKU_T;
  RawSignal[4+(KAKU_CodeLength*4)] = 32*KAKU_T; // stop sync pulse
}

/*********************************************************************************************\
* Deze routine berekent de uit een RawSignal een CMD_KAKU
* Geeft een false retour als geen geldig KAKU commando uit het signaal te destilleren
\*********************************************************************************************/
unsigned long RawSignal_2_KAKU(void)
{
  byte Home, Unit, Level, Command;
  int i;
  byte x;
  unsigned long y=0;

  // conventionele KAKU bestaat altijd uit 12 data bits plus stop. Ongelijk, dan geen KAKU
  if (RawSignal[0] != 4+(KAKU_CodeLength*4)) { return false; }

  // omzetten naar bitstream als timing correct is
  Command = CMD_OFF;
  for (i=0; i<KAKU_CodeLength; i++) {
    x = decodePWDM(2*i+1, KAKU_T, KAKU_nP, KAKU_nS, KAKU_nL) << 1 | decodePWDM(2*i+2, KAKU_T, KAKU_nP, KAKU_nS, KAKU_nL);
    if      (x>=3) { return false; }
    else if (x==0) { y = (y >> 1); }
    else if (x==1) { y = (y >> 1 | (1 << (KAKU_CodeLength-1))); }
    else if (x==2) { y = (y >> 1); Command = CMD_ALLOFF; }
  }
  if ((y & 0x600) != 0x600) { return false; }
  
  Home =     (y      ) & 0x0F;
  Unit =     (y >>  4) & 0x0F;
  Command |= (y >> 11) & 0x01;
  return command2event(CMD_KAKU, (Home << 4 | Unit), Command);
}


/*********************************************************************************************\
* NewKAKU
* Encoding volgens Arduino Home Easy pagina
* Pulse (T) is 275us PDM
* 0 = T,T,T,4T, 1 = T,4T,T,T, dim = T,T,T,T op bit 27
*
* NewKAKU ondersteund:
*   on/off       ---- 000x Off/On
*   all on/off   ---- 001x AllOff/AllOn
*   bright dim   ---- 010x Dim/Bright   // niet ondersteund
*   dim absolute xxxx 0110 Dim16        // dim op bit 27 + 4 extra bits voor dim level
\*********************************************************************************************/
#define NewKAKU_CodeLength        32         // aantal data bits, +4 voor dimmen
#define NewKAKU_T                175 //275   // us
#define NewKAKU_nP                 2 //1     // pulse
#define NewKAKU_nS                 1         // short pulse
#define NewKAKU_nL                 7 //4     // long pulse

/*********************************************************************************************\
* Deze routine berekent de RAW pulsen uit een CMD_NEWKAKU plaatst deze in de buffer RawSignal
\*********************************************************************************************/
void NewKAKU_2_RawSignal(unsigned long Code)
{
  unsigned long Home;
  byte Unit, Level, Command;
  boolean Dim;
  int i;
  byte l;
  boolean b0, b1;

  // CMD_NewKAKU = NNxxHULC, NN=Nodo unit, xx=CMD_NewKAKU, H=Home, U=Unit, L=Level, C=Commando
  Home    = ((Code >> 12) & 0xF) + BaseCode; // 26 bit, nu basecode + 4 bit home code
  Unit    =  (Code >>  8) & 0xF;
  Level   =  (Code >>  4) & 0xF;
  Command =  (Code      ) & 0x3;
  Dim     =  (Code & 0xE) == CMD_DIMLEVEL;

  if (Dim) { Command = 0; l = 4+((NewKAKU_CodeLength+4)*4); } else { l = 4+(NewKAKU_CodeLength*4); }
  Code = Home << 6 | Command << 4 | Unit;

  RawSignal[0] = l;
  RawSignal[1] = NewKAKU_T;
  RawSignal[2] = 2*NewKAKU_nL*NewKAKU_T;
  
  for (i=NewKAKU_CodeLength-1; i>=0; i--) {
    b0 = ( (Code >> i)) & 1;
    b1 = (~(Code >> i)) & 1; //!b0;
    if ((i == 4) && (Dim)) { b0 = 0; b1 = 0; }       // dim
    encodePDM(2*(NewKAKU_CodeLength-1-i)+1, NewKAKU_T, NewKAKU_nP, NewKAKU_nS, NewKAKU_nL, b0);
    encodePDM(2*(NewKAKU_CodeLength-1-i)+2, NewKAKU_T, NewKAKU_nP, NewKAKU_nS, NewKAKU_nL, b1);
  }
  if (Dim) {
    for (i=3; i>=0; i--) {
      encodePDM(2*(NewKAKU_CodeLength+4-1-i)+1, NewKAKU_T, NewKAKU_nP, NewKAKU_nS, NewKAKU_nL, ( (Level >> i)) & 1);
      encodePDM(2*(NewKAKU_CodeLength+4-1-i)+2, NewKAKU_T, NewKAKU_nP, NewKAKU_nS, NewKAKU_nL, (~(Level >> i)) & 1);
    }
  }

  RawSignal[l-1] = NewKAKU_T;
  RawSignal[l  ] = 10*NewKAKU_nL*NewKAKU_T;
}


/*********************************************************************************************\
* Deze routine berekent de uit een RawSignal een CMD_NEWKAKU
* Geeft een false retour als geen geldig KAKU commando uit het signaal te destilleren
\*********************************************************************************************/
unsigned long RawSignal_2_NewKAKU(void)
{
  byte Home, Unit, Level, Command;
  boolean Dim;
  int i;
  byte x;
  unsigned long y=0;

  // nieuwe KAKU bestaat altijd uit start bit + 32 bits + evt 4 dim bits. Ongelijk, dan geen NewKAKU
  if ((RawSignal[0] != 4+(NewKAKU_CodeLength*4)) && (RawSignal[0] != 4+(NewKAKU_CodeLength*4)+4)) { return false; }

  // omzetten naar bitstream als timing correct is
  Level = 0;
  Dim = false;
  for (i=0; i<NewKAKU_CodeLength; i++) {
    x = decodePDM(2*i+1, NewKAKU_T, NewKAKU_nP, NewKAKU_nS, NewKAKU_nL) << 1 | decodePDM(2*i+2, NewKAKU_T, NewKAKU_nP, NewKAKU_nS, NewKAKU_nL);
    if      (x>=3) { return false; }
    else if (x==1) { y = y << 1; }
    else if (x==2) { y = y << 1 | 1; }
    else if (x==0 && i==27) { y = y << 1; Dim = true; }
  }
  if (Dim) {
    for (i=NewKAKU_CodeLength; i<(NewKAKU_CodeLength+4); i++) {
      x = decodePDM(2*i+1, NewKAKU_T, NewKAKU_nP, NewKAKU_nS, NewKAKU_nL) << 1 | decodePDM(2*i+2, NewKAKU_T, NewKAKU_nP, NewKAKU_nS, NewKAKU_nL);
      if      (x>=3) { return false; }
      else if (x==1) { Level = Level << 1; }
      else if (x==2) { Level = Level << 1 | 1; }
      else if (x==0) { return false; }
    }
  }

// Serial.print("BaseCode: ");
// Serial.println(y >> 6, DEC);
  Home =    ((y >> 6) - BaseCode) & 0xF;
  Unit =    ( y     ) & 0x0F;
  if (Dim) { Command = CMD_DIMLEVEL; } else { Command = (y >> 4) & 0x03; }
  return command2event(CMD_KAKU_NEW, (Home << 4 | Unit), (Level << 4 | Command));
}
