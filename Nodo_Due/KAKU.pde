  /**************************************************************************\
    This file is part of Nodo Due, (c) Copyright Paul Tonkes, 
    Thanks to: Kenneth Rover

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

 Hier bevinden zich de de routines voor verwerken van de KAKU specifieke signalen:
 
  Conventionele KAKU signalen:
  * KAKU_2_RawSignal()        Zet 32-bit event van type CMD_KAKU om naar een pulsenreeks in de RawBuffer t.b.v. verzending via RF.
  * RawSignal_2_KAKU()        Zet een pulsenreeks die zich in de RawBuffer bevindt om naar een 32-bit CMD_KAKU event

  KAKU signalen volgens de nieuwe codering:
  * RawSignal_2_NewKAKU()     Zet een pulsenreeks die zich in de RawBuffer bevindt om naar een 32-bit CMD_KAKU_NEW event
  * NewKAKU_2_RawSignal()     Zet 32-bit event van type CMD_KAKU_NEW om naar een pulsenreeks in de RawBuffer t.b.v. verzending via RF.

*/


/*********************************************************************************************\
* Deze routine berekent de RAW pulsen uit een CMD_KAKU plaatst deze in de buffer RawSignal
*
* KAKU
* Encoding volgens Princeton PT2262 / MOSDESIGN M3EB / Domia Lite spec.
* Pulse (T) is 350us PWDM
* 0 = T,3T,T,3T, 1 = T,3T,3T,T, short 0 = T,3T,T,T
*
* KAKU ondersteund:
*   on/off       ---- 000x Off/On
*   all on/off   ---- 001x AllOff/AllOn // is group (unit code bestaat uit short 0s)
\*********************************************************************************************/
#define KAKU_ALLOFF 2
#define KAKU_ALLON 3
#define KAKU_CodeLength    12  // aantal data bits
#define KAKU_T            350  // us

void KAKU_2_RawSignal(unsigned long Code)
  {
  byte Home, Unit, Level, Command;
  boolean Group;
  int i;
  boolean b;

  // CMD_KAKU = NNxxHU0C, NN=Nodo Home + Unit, xx=CMD_KAKU, H=Home, U=Unit, C=Commando
  Home    = (Code >> 12) & 0xF;
  Unit    = (Code >>  8) & 0xF;
  Command = Code&0xff!=0;
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
  RawSignal[(KAKU_CodeLength*4)+2] = KAKU_T*32; // pauze tussen de pulsreeksen
  }

/*********************************************************************************************\
* Deze routine berekent de uit een RawSignal een CMD_KAKU
* Geeft een false retour als geen geldig KAKU commando uit het signaal te destilleren
\*********************************************************************************************/
unsigned long RawSignal_2_KAKU(void)
  {
  byte Home, Unit, Level, Command=VALUE_OFF;
  int i,j;
  boolean Group=false;
  unsigned long bitstream=0;

  // conventionele KAKU bestaat altijd uit 12 data bits plus stop. Ongelijk, dan geen KAKU
  if (RawSignal[0]!=(KAKU_CodeLength*4)+2)return false;

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

  return SetEventType(command2event(CMD_KAKU, (Home << 4 | Unit), Command),SIGNAL_TYPE_KAKU); // hoogte nible wissen en weer vullen met type NewKAKU
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
*   dim absolute xxxx 0110 Dim16        // dim op bit 27 + 4 extra bits voor dim level
*
*  Nodo Event            = TTTTUUUUCCCCCCCC1111111122222222       -> T=Type, U=Unit, 1=Par-1, 2=Par-2
*  Nodo commando NewKAKU = TTTTUUUUPPPPPPPPAAAAAAAALLLLCCCC       -> T=Type, U=Unit, P=CMD_NEWKAKU_SEND, A=User adres, L=dimlevel, c=commando
*  NewKAKU (org.)        = AAAAAAAAAAAAAAAAAAAAAAAAAACCUUUU(LLLL) -> A=KAKU_adres, C=commando, U=KAKU-Unit, L=extra dimlevel bits (optioneel)
*  NewKAKU van Nodo      = 000000000000000000AAAAAAAACC0001(LLLL) -> H=Nodo_home, A=User adres, D=Dim, C=commando, L=extra dimlevel bits (optioneel)
*  Bit                   = 01234567890123456789012345678901 2345  -> Bit-0 gaat als eerste door de ether.
*                                    1111111111222222222233 3333 
*
\*********************************************************************************************/

#define NewKAKU_RawSignalLength      132
#define NewKAKUdim_RawSignalLength   148
#define NewKAKU_1T                   275        // us
#define NewKAKU_mT                   500        // us, midden tussen 1T en 4T 
#define NewKAKU_4T                  1100        // us
#define NewKAKU_8T                  2200        // us, Tijd van de space na de startbit

/*********************************************************************************************\
* Deze routine berekent de RAW pulsen uit een CMD_NEWKAKU plaatst deze in de buffer RawSignal
\*********************************************************************************************/
void NewKAKU_2_RawSignal(unsigned long CodeNodo)
  {
  unsigned long bitstream=0L;
  byte Bit, Level, i=1;
  byte x; /// aantal posities voor pulsen/spaces in RawSignal
  byte y; 

  // bouw het KAKU adres op
  bitstream=1|(CodeNodo>>2)&0x3FC0;  // Door gebruiker gekozen adres uit de Nodo_code toevoegen aan adres deel van de KAKU code. Tevens compatibiliteit met Versie 1.1 door 1e bit te zetten (Home adres 1)

  // zet commando bit en level
  Level=CodeNodo&0xff;
  if(Level==VALUE_ON || Level==VALUE_OFF)
    {
    bitstream|=(Level==VALUE_ON)<<4; // bit-5 is het on/off commando in KAKU signaal
    Level=false;
    x=130;// verzend startbit + 32-bits = 130
    }
  else
    {
    Level--;// Dim level 0..15
    x=146;// verzend startbit + 32-bits = 130 + 4dimbits = 146
    }
 
  // bitstream bevat nu de KAKU-bits die verzonden moeten worden.

  for(i=3;i<=x;i++)RawSignal[i]=NewKAKU_1T;  // De meeste tijden in signaal zijn T. Vul alle pulstijden met deze waarde. Later worden de 4T waarden op hun plek gezet
  
  i=1;
  RawSignal[i++]=NewKAKU_1T; //pulse van de startbit
  RawSignal[i++]=NewKAKU_8T; //space na de startbit
  
  y=31; // bit uit de bitstream
  while(i<x)
    {
    if((bitstream>>(y--))&1)
      RawSignal[i+1]=NewKAKU_4T;     // Bit=1; // T,4T,T,T
    else
      RawSignal[i+3]=NewKAKU_4T;     // Bit=0; // T,T,T,4T

    if(x==146)  // als het een dim opdracht betreft
      {
      if(i==111) // Plaats van de Commnado-bit uit KAKU 
        RawSignal[i+3]=NewKAKU_1T;  // moet een T,T,T,T zijn bij een dim commando.
      if(i==127)  // als alle pulsen van de 32-bits weggeschreven zijn
        {
        bitstream=(unsigned long)Level; //  nog vier extra dim-bits om te verzenden
        y=3;
        }
      }
    i+=4;
    }
  RawSignal[i++]=NewKAKU_1T; //pulse van de stopbit
  RawSignal[i]=NewKAKU_1T*32; //space van de stopbit tevens pause tussen signalen
  RawSignal[0]=i; // aantal bits*2 die zich in het opgebouwde RawSignal bevinden
  }


/*********************************************************************************************\
* Deze routine berekent de uit een RawSignal een CMD_NEWKAKU
* Geeft een false retour als geen geldig KAKU commando uit het signaal te destilleren
\*********************************************************************************************/
unsigned long RawSignal_2_NewKAKU(void)
  {
  unsigned long bitstream=0L;
  boolean Bit;
  int Level=0,i;
  
  // nieuwe KAKU bestaat altijd uit start bit + 32 bits + evt 4 dim bits. Ongelijk, dan geen NewKAKU
  if (RawSignal[0]!=NewKAKU_RawSignalLength && (RawSignal[0]!=NewKAKUdim_RawSignalLength))return 0L;

  // RawSignal[0] bevat aantal pulsen * 2  => negeren
  // RawSignal[1] bevat startbit met tijdsduur van 1T => negeren
  // RawSignal[2] bevat lange space na startbit met tijdsduur van 8T => negeren
  i=3; // RawSignal[3] is de eerste van een T,xT,T,xT combinatie
  
  do 
    {
    if     (RawSignal[i]<NewKAKU_mT && RawSignal[i+1]<NewKAKU_mT && RawSignal[i+2]<NewKAKU_mT && RawSignal[i+3]>NewKAKU_mT)Bit=0; // T,T,T,4T
    else if(RawSignal[i]<NewKAKU_mT && RawSignal[i+1]>NewKAKU_mT && RawSignal[i+2]<NewKAKU_mT && RawSignal[i+3]<NewKAKU_mT)Bit=1; // T,4T,T,T
    else if(RawSignal[i]<NewKAKU_mT && RawSignal[i+1]<NewKAKU_mT && RawSignal[i+2]<NewKAKU_mT && RawSignal[i+3]<NewKAKU_mT)       // T,T,T,T Deze hoort te zitten op i=111 want: 27e NewKAKU bit maal 4 plus 2 posities voor startbit
      {
      if(RawSignal[0]!=NewKAKUdim_RawSignalLength) // als de dim-bits er niet zijn
        return false;
      }
    else
      return false; // andere mogelijkheden zijn niet geldig in NewKAKU signaal.  
      
    if(i<130) // alle bits die tot de 32-bit pulstrein behoren 32bits * 4posities per bit + pulse/space voor startbit
      bitstream=(bitstream<<1) | Bit;
    else // de resterende vier bits die tot het dimlevel behoren 
      Level=(Level<<1) | Bit;
 
    i+=4;// volgende pulsenquartet
    }while(i<RawSignal[0]-2); //-2 omdat de space/pulse van de stopbit geen deel meer van signaal uit maakt.

  if(bitstream>0x0ffff)
    // het is van een NewKAKU zender afkomstig. Geef de hex-waarde terug.
    return SetEventType(bitstream,SIGNAL_TYPE_NEWKAKU); // hoogte nible wissen en weer vullen met type NewKAKU
  else
    {
    // het is van een andere Nodo afkomstig. Maak er een Nodo commando van.
    if(i>140)
      return command2event(CMD_KAKU_NEW, (bitstream>>6)&0xff,Level+1);
    else
      {
      i=((bitstream>>4)&0x01)?VALUE_ON:VALUE_OFF;
      return command2event(CMD_KAKU_NEW, (bitstream>>6)&0xff,i);
      }
    }
  return bitstream;
  }

