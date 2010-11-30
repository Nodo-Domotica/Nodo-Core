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

#define KAKU_ALLOFF 2
#define KAKU_ALLON 3
#define KAKU_DIMLEVEL 6

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
* Encoding volgens Princeton PT2262 / MOSDESIGN M3EB / Domia Lite spec.
* Pulse (T) is 350us PWDM
* 0 = T,3T,T,3T, 1 = T,3T,3T,T, short 0 = T,3T,T,T
*
* KAKU ondersteund:
*   on/off       ---- 000x Off/On
*   all on/off   ---- 001x AllOff/AllOn // is group (unit code bestaat uit short 0s)
\*********************************************************************************************/
#define KAKU_CodeLength    12  // aantal data bits
#define KAKU_T            350  // us


/*********************************************************************************************\
* Deze routine berekent de RAW pulsen uit een CMD_KAKU plaatst deze in de buffer RawSignal
\*********************************************************************************************/

void KAKU_2_RawSignal(unsigned long Code)
  {
  byte Home, Unit, Level, Command;
  boolean Group;
  int i;
  boolean b;

  // CMD_KAKU = NNxxHU0C, NN=Nodo Home + Unit, xx=CMD_KAKU, H=Home, U=Unit, C=Commando
  Home    = (Code >> 12) & 0xF; // home in bit 
  Unit    = (Code >>  8) & 0xF;
  Command = (Code      ) & 0x1;
  Group   = (Code & KAKU_ALLOFF) == KAKU_ALLOFF;
  Code = Home | Unit << 4 | (0x600 | (Command << 11));

  RawSignal[0]=KAKU_CodeLength*4+2;

  for (i=0; i<KAKU_CodeLength; i++) // loop de 12-bits langs en vertaal naar pulse/space signalen.  
    {
    RawSignal[4*i+1]=KAKU_T;
    RawSignal[4*i+2]=KAKU_T*3;

    if (Group && i>=4 && i<8) 
      {
      RawSignal[4*i+3]=KAKU_T;
      RawSignal[4*i+4]=KAKU_T;
      } // short 0
    else
      {
      if((Code>>i)&1)// 1
        {
        RawSignal[4*i+3]=KAKU_T*3;
        RawSignal[4*i+4]=KAKU_T;
        }
      else //0
        {
        RawSignal[4*i+3]=KAKU_T;
        RawSignal[4*i+4]=KAKU_T*3;          
        }          
      }
    }
  RawSignal[(KAKU_CodeLength*4)+1] = KAKU_T;
  RawSignal[(KAKU_CodeLength*4)+2] = KAKU_T*32; // stop sync pulse
  }

/*********************************************************************************************\
* Deze routine berekent de uit een RawSignal een CMD_KAKU
* Geeft een false retour als geen geldig KAKU commando uit het signaal te destilleren
\*********************************************************************************************/
unsigned long RawSignal_2_KAKU(void)
  {
  byte Home, Unit, Level, Command;
  int i,j;
  unsigned long bitstream=0;

  // conventionele KAKU bestaat altijd uit 12 data bits plus stop. Ongelijk, dan geen KAKU
  if (RawSignal[0]!=(KAKU_CodeLength*4)+2)return false;

//???  Command = KAKU_ALLOFF;
  Command = 0;

  for (i=0; i<KAKU_CodeLength; i++)
    {
    j=KAKU_T*2;
        
    if      (RawSignal[4*i+1]<j && RawSignal[4*i+2]>j && RawSignal[4*i+3]<j && RawSignal[4*i+4]>j) {bitstream=(bitstream >> 1);} // 0
    else if (RawSignal[4*i+1]<j && RawSignal[4*i+2]>j && RawSignal[4*i+3]>j && RawSignal[4*i+4]<j) {bitstream=(bitstream >> 1 | (1 << (KAKU_CodeLength-1))); }// 1
    else if (RawSignal[4*i+1]<j && RawSignal[4*i+2]>j && RawSignal[4*i+3]<j && RawSignal[4*i+4]<j) {bitstream=(bitstream >> 1); Command= KAKU_ALLOFF;} // Short 0, Groep commando. Zet bit-2 van Par2.
    else {return false;} // foutief signaal
    }
 
  if ((bitstream&0x600)!=0x600)return false; // twee vaste bits van KAKU gebruiken als checksum
  
  Home =     (bitstream      ) & 0x0F;
  Unit =     (bitstream >>  4) & 0x0F;
  Command |= (bitstream >> 11) & 0x01;

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
*
*  Nodo Event            = HHHHUUUUCCCCCCCC1111111122222222       -> H=Nodo_home, U=Unit, 1=Par-1, 2=Par-2
*  Nodo commando NewKAKU = HHHHUUUUPPPPPPPPAAAAAAAALLLLCCCC       -> H=Nodo_home, U=Unit, P=CMD_NEWKAKU_SEND, A=User adres, L=dimlevel, c=commando
*  NewKAKU (org.)        = AAAAAAAAAAAAAAAAAAAAAAAAAACCUUUU(LLLL) -> A=KAKU_adres, D=Dim, C=commando, U=KAKU-Unit, L=extra dimlevel bits (optioneel)
*  NewKAKU van Nodo      = 000000000000000000AAAAAAAACCHHHH(LLLL) -> H=Nodo_home, A=User adres, C=commando, L=extra dimlevel bits (optioneel)
*
*
\*********************************************************************************************/
#define NewKAKU_CodeLength        32         // aantal data bits, +4 voor dimmen
#define NewKAKU_T                175         // us
#define NewKAKU_nP                 2         // pulse
#define NewKAKU_nS                 1         // short pulse
#define NewKAKU_nL                 7         // long pulse

/*********************************************************************************************\
* Deze routine berekent de RAW pulsen uit een CMD_NEWKAKU plaatst deze in de buffer RawSignal
\*********************************************************************************************/
void NewKAKU_2_RawSignal(unsigned long CodeNodo)
  {
  unsigned long CodeKAKU; // NewKAKU adres
  byte Level, Command; // NewKAKU Dim-level en Commando  
  byte Unit;
  boolean Dim;
  int i;
  byte j;
  boolean b0, b1;

  // CodeNodo bevat het NewKAKU commando volgens Nodo formaat
  // CodeKAKU bevat het KAKU signaal zoals verzonden moet worden via RF
  
  Level    =  (CodeNodo >>  4) & 0xF; // dim_level uit de Nodo_code halen.
  Command  =  (CodeNodo      ) & 0x3; // schakel commando uit de Nodo_code halen.
  Dim      =  (CodeNodo & 0xE) == KAKU_DIMLEVEL; // Dim vlag zetten als gebruiker gekozen heeft voor een dimlevel.
 
  if (Dim) 
    {
    Command=0;
    j=2+((NewKAKU_CodeLength+4)*4)+2;
    }
  else 
    {
    j=2+(NewKAKU_CodeLength*4)+2;
    }

  CodeKAKU =    (CodeNodo>>28)& 0xF      // Nodo_home uit de Nodo_code toevoegen aan adres deel van de KAKU code
              | (CodeNodo>> 2)& 0x3FC0   // Door gebruiker gekozen adres uit de Nodo_code toevoegen aan adres deel van de KAKU code
              |  Command << 4;           // Commando toevoegen aan KAKU code

  // CodeKAKU bevat nu de KAKU-bits die verzonden moeten worden.

  // KAKU code omzetten naar RawSignal
  RawSignal[0] = j;
  RawSignal[1] = NewKAKU_nP*NewKAKU_T;
  RawSignal[2] = 2*NewKAKU_nL*NewKAKU_T;
  for (i=NewKAKU_CodeLength-1; i>=0; i--)
    {
    b0 = ( (CodeKAKU >> i)) & 1;
    b1 = (~(CodeKAKU >> i)) & 1; //!b0;
    if ((i == 4) && (Dim)){b0=0;b1=0;}// dim
    NewKAKUencodePDM(2*(NewKAKU_CodeLength-1-i)+1, b0);
    NewKAKUencodePDM(2*(NewKAKU_CodeLength-1-i)+2, b1);
    }
  if(Dim)
    {
    for (i=3; i>=0; i--)
      {
      NewKAKUencodePDM(2*(NewKAKU_CodeLength+4-1-i)+1, ( (Level >> i)) & 1);
      NewKAKUencodePDM(2*(NewKAKU_CodeLength+4-1-i)+2, (~(Level >> i)) & 1);
      }
    }
  RawSignal[j-1] =    NewKAKU_nP*NewKAKU_T;
  RawSignal[j  ] =    NewKAKU_nL*NewKAKU_T;
  }


/*********************************************************************************************\
* Deze routine berekent de uit een RawSignal een CMD_NEWKAKU
* Geeft een false retour als geen geldig KAKU commando uit het signaal te destilleren
\*********************************************************************************************/
unsigned long RawSignal_2_NewKAKU(void)
{
  byte nodo_home, user_address, Level, Command;
  boolean Dim;
  int i;
  byte x;
  unsigned long y=0;

  // nieuwe KAKU bestaat altijd uit start bit + 32 bits + evt 4 dim bits. Ongelijk, dan geen NewKAKU
  if ((RawSignal[0] != 2+(NewKAKU_CodeLength*4)+2) && (RawSignal[0] != 2+((NewKAKU_CodeLength+4)*4)+2))return 0L;

  // omzetten naar bitstream als timing correct is
  Level=0;
  Dim=false;
  
  for(i=0; i<NewKAKU_CodeLength; i++) 
    {
    x = NewKAKUdecodePDM(2*i+1) << 1 | NewKAKUdecodePDM(2*i+2);
    if      (x>=3)return false;
    else if (x==1)y=y<<1;
    else if (x==2)y=y<<1|1;
    else if (x==0 && i==27) {y = y << 1; Dim = true;}
    }
    
  if(Dim) 
    {
    for (i=NewKAKU_CodeLength; i<(NewKAKU_CodeLength+4); i++)
      {
      x = NewKAKUdecodePDM(2*i+1) << 1 | NewKAKUdecodePDM(2*i+2);
      if      (x>=3)return false;
      else if (x==1)Level=Level<<1;
      else if (x==2)Level=Level<<1|1;
      else if (x==0)return false;
      }
    }
  // y bevat nu de KAKU-bits die ontvangen zijn.

  nodo_home  = y&0xF;
  if(nodo_home==S.Home)
    {// KAKU code was verzonden door een Nodo met het zelfde home adres.
    user_address = (y>>6) & 0xff;
    if(Dim)
      Command=KAKU_DIMLEVEL;
    else
      Command=(y>>4)&0x03;    
    return command2event(CMD_KAKU_NEW, user_address, (Level << 4 | Command));
    }
  else// De KAKU code is niet verzonden door een Nodo met hetzelfde home
    return y;
  }


void NewKAKUencodePDM(byte i, boolean y) 
  { // pulse distance modulation / space coded
  byte n;

  if (y==0)
    n=NewKAKU_nS;
  else
    n=NewKAKU_nL;

  RawSignal[2*i+1] = NewKAKU_T * NewKAKU_nP;
  RawSignal[2*i+2] = NewKAKU_T * n;    
  }


byte NewKAKUdecodePDM(byte i)  // pulse distance modulation
  {
  byte y = (NewKAKUdecodePulse(2*i, NewKAKU_nP, 0) << 1 | NewKAKUdecodePulse(2*i+1, NewKAKU_nS, NewKAKU_nL));
  if (y==0)return 0;
  if (y==1)return 1;
  return 0xFF;
  }


byte NewKAKUdecodePulse(byte i, byte n0, byte n1)
  {
  unsigned int t; //, t0, t1;
  t = RawSignal[i+1];
  if ((t > ((n0-1)*NewKAKU_T)) && (t < ((n0+1)*NewKAKU_T)))return 0;
  if ((t > ((n1-1)*NewKAKU_T)) && (t < ((n1+1)*NewKAKU_T)))return 1;
  return 0xFF;
  }

