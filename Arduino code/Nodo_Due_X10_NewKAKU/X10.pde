/**************************************************************************\

    This file is part of Nodo Due, © Copyright Kenneth Rovers

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


/*

 Hier bevinden zich de de routines voor verwerken van de X10 specifieke signalen:
  * X10_2_RawSignal()     Zet 32-bit event van type CMD_XTEN om naar een pulsenreeks in de RawBuffer t.b.v. verzending via RF.
  * RawSignal_2_X10()     Zet een pulsenreeks die zich in de RawBuffer bevindt om naar een 32-bit CMD_X10 event.

*/


/*********************************************************************************************\
* X10
* Encoding volgens NEC IR protocol / X10 spec
* Pulse (T) is 220us PDM
* 0 = 3T,2T, 1 = 3T,7T
*
* X10 ondersteund:
*   on/off       ---- 000x Off/On
*   all on/off   ---- 001x AllOff/AllOn
*   bright dim   ---- 010x Dim/Bright
*   dim absolute xxxx 0110 Dim16        // niet ondersteund door X10 RF?
\*********************************************************************************************/

#define X10_CodeLength    32  // aantal data bits
#define X10_T            220  // us
#define X10_nP             3  // pulse
#define X10_nS             2  // short pulse
#define X10_nL             7  // long pulse

/*********************************************************************************************\
* Deze routine berekent de RAW pulsen uit een CMD_X10 plaatst deze in de buffer RawSignal
\*********************************************************************************************/
void X10_2_RawSignal(unsigned long Code)
{
  byte Home, Unit, Level, Command;
  boolean On,Off;
  byte X10Address, X10Data;
  int i;

  // CMD_X10 = NNFFxxHULC, NN=Nodo unit, FF=command escape code, xx=command, H=Home, U=Unit, L=Level, C=Commando
  Home    = (Code >> 12) & 0xF;
  Unit    = (Code >>  8) & 0xF;
  Level   = (Code >>  4) & 0xF;
  Command = (Code      ) & 0x6;
  On      = (Code      ) & 0x1;
  Off     = (~Code     ) & 0x1;

  if ((Home & 4) == 0) { X10Address = (Home ^ 0x6) << 4; } else { X10Address = (Home ^ 0xC) << 4; }
  if (Command == CMD_OFF) { // On/Off
      X10Address |= (Unit & 8) >> 1;
      X10Data =
            (Unit & 1) << 4
          | (Unit & 2) << 2
          | (Unit & 4) << 4
          | (Off     ) << 5;
  } else if (Command == CMD_ALLOFF) { // AllOn/AllOff
    X10Data = (8 | On) << 4;
  } else if (Command == CMD_DIM) { // Bright/Dim
    X10Data = ((8 | Off) << 4) | 8;
  } else {
    X10Data = 0;
  }
  Code = ((unsigned long) X10Address) << 24 | ((unsigned long) (~X10Address) & 0xFF) << 16 | ((unsigned long) X10Data) << 8 | ((unsigned long) (~X10Data)) & 0xFF;
  RawSignal[0]=4+(X10_CodeLength*2);
  RawSignal[1]=40*X10_T; // start sync
  RawSignal[2]=20*X10_T;
  for (i=0; i<X10_CodeLength; i++) {
    encodePDM(i+1, X10_T, X10_nP, X10_nS, X10_nL, (Code >> (X10_CodeLength-1-i)) & 1);
  }
  RawSignal[2+(X10_CodeLength*2)+1]=3*X10_T;  // stop sync
  RawSignal[2+(X10_CodeLength*2)+2]=160*X10_T;
}


/*********************************************************************************************\
* Deze routine berekent de uit een RawSignal een CMD_XTEN
* Geeft een false retour als geen geldig X10 commando uit het signaal te destilleren
\*********************************************************************************************/
unsigned long RawSignal_2_X10(void)
{
  byte Home, Unit, Level, Command;
  byte X10Address, X10Data;
  int i;
  byte x;
  unsigned long y=0;
  
  // conventionele X10 bestaat altijd uit 32 bits plus stop bit. Ongelijk, dan geen X10
  if (RawSignal[0] != 2+(X10_CodeLength*2)+2) { return false; }

  // omzetten naar bitstream als timing correct is
  for (i=0; i<X10_CodeLength; i++) {
    x = decodePDM(i+1, X10_T, X10_nP, X10_nS, X10_nL);
    if      (x>=2) { return false; }
    else if (x==0) { y = (y << 1); }
    else if (x==1) { y = (y << 1 | 1); }
  }
  
  X10Address = y >> 24;
  if (X10Address != (~y >> 16 & 0xFF)) { return false; }
  X10Data = y >> 8;
  if (X10Data != (~y & 0xFF)) { return false; }
  if ((X10Address & 64) == 0) { Home = (X10Address >> 4) ^ 0xC; } else { Home = (X10Address >> 4) ^ 0x6; }
  if ((X10Data & 128) == 0) { // On/Off
    Unit = 
        (X10Address &  4) << 1
      | (X10Data    & 64) >> 4
      | (X10Data    &  8) >> 2
      | (X10Data    & 16) >> 4;
    Command = (~X10Data & 32) >> 5;
  } else if ((X10Data & 8) == 0) { // AllOn/AllOff
    Unit = 0;
    Command = CMD_ALLOFF | ((X10Data & 16) >> 4);
  } else { // Bright/Dim
    Unit = 0;
    Command = CMD_DIM | ((~X10Data & 16) >> 4);
  }
  
  return command2event(CMD_X10, (Home << 4 | Unit), Command);
}

