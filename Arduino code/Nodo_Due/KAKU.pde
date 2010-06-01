/**************************************************************************\

    This file is part of Nodo Due, © Copyright Paul Tonkes

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
  * OLDKAKU_2_RawSignal()     Zet 32-bit event van type CMD_KAKU om naar een pulsenreeks in de RawBuffer t.b.v. verzending via RF.
  * RawSignal_2_OLDKAKU()     Zet een pulsenreeks die zich in de RawBuffer bevindt om naar een 32-bit CMD_KAKU event

  KAKU signalen volgens de nieuwe codering:
  * RawSignal_2_NEWKAKU()     Zet een pulsenreeks die zich in de RawBuffer bevindt om naar een 32-bit CMD_AKAKU event
  * NEWKAKU_2_RawSignal()     Zet 32-bit event van type CMD_KAKU om naar een pulsenreeks in de RawBuffer t.b.v. verzending via RF.


*/

#define KAKU_OldCodeLength        25     // aantal bits van het oude KAKU signaal met Home, Adres, Status.
#define KAKU_NewCodeLength        66     // aantal bits van KAKU zenders volgens nieuwe automatische adressering.
#define KAKU_ShortPulse          500     // Verzenden KAKU signaal: De lengte van een korte puls (500uSec). Een lange is tweemaal deze waarde
#define KAKU_LongPulse          1000     // Verzenden KAKU signaal: De lengte van een korte puls (500uSec). Een lange is tweemaal deze waarde
#define KAKU_0                   700     // Ontvangst KAKU signaal: Alle RawSignal tijden korter dan deze waarde (750uSec.) zijn een 0.


 /*********************************************************************************************\
 * Deze routine berekent de uit een RawSignal een CMD_KAKU
 * Geeft een false retour als geen geldig nieuw KAKU commando uit het signaal te destilleren
 \*********************************************************************************************/
unsigned long RawSignal_2_NEWKAKU(void)
  {
  // dummy, nog uitwerken


  unsigned long Code=0L;
  byte x;

  // Data zit in de SPACE, Eerst ontvangen bit is MSB. totaal 66 bits waarval MSB is startbit (1) en LSB is stopbit (0)
  // oneven bit is de inverse van de even bits (checksum?) 
  if(RawSignal[0]==KAKU_NewCodeLength*2)
    {
    PrintTerm();
    x=4; //1=start_pulse, 2=start_space, 3=bit_1_pulse, 4=bit_1_space--> hier begint de data!
    while(x<=RawSignal[0]-2)// -2 omdat de pulse/space van de stopbis geen onderdeel uitmaakt van de code
      {
      //      if(RawSignal[x]>0x200)
      //        Serial.print("1");
      //      else
      //        Serial.print("0");
      Code=(Code<<1)| ((RawSignal[x])>0x200);
      x+=2;
      }
    return Code;
    PrintTerm();
    }
  return false;
  }


 /*********************************************************************************************\
 * Deze routine berekent de RAW pulsen uit een CMD_AKAKU plaatst deze in de buffer RawSignal
 \*********************************************************************************************/
void NEWKAKU_2_RawSignal(unsigned long Code)
  {
  // dummy, nog uitwerken
  }


 /*********************************************************************************************\
 * Deze routine berekent de RAW pulsen uit een CMD_KAKU plaatst deze in de buffer RawSignal
 \*********************************************************************************************/
void OLDKAKU_2_RawSignal(unsigned long Code)
  {
  byte Home,Address,Command;
  int BitCounter,y=0;

  // CMD_KAKU = UUFF5AHA0C, UU=Unit,FF=commando escape code, 5A=commando, H=Home, A=adres, C=Commando
  Home=(Code>>12)&0xf;
  Address=(Code>>8)&0xf;
  Command=Code&1;
  
  Code=0x28UL// gebruik code hier om de bitstream in op te slaan
     |((unsigned long)Command)<<1
     |((unsigned long)Home&1)<<23
     |((unsigned long)Home&2)<<20
     |((unsigned long)Home&4)<<17
     |((unsigned long)Home&8)<<14
     |((unsigned long)Address&1)<<15
     |((unsigned long)Address&2)<<12
     |((unsigned long)Address&4)<<9
     |((unsigned long)Address&8)<<6;

  RawSignal[y++]=KAKU_OldCodeLength*2;
  for(BitCounter=KAKU_OldCodeLength; BitCounter>=0; BitCounter--)
    {
    if((Code>>BitCounter-1)&1)
      {
      RawSignal[y++]=KAKU_LongPulse; // mark bij data=1
      RawSignal[y++]=KAKU_ShortPulse;   // space
      }
    else
      {
      RawSignal[y++]=KAKU_ShortPulse;   // mark bij data=0
      RawSignal[y++]=KAKU_LongPulse;   // space
      }
    }
  }


 /*********************************************************************************************\
 * Deze routine berekent de uit een RawSignal een CMD_KAKU
 * Geeft een false retour als geen geldig KAKU commando uit het signaal te destilleren
 \*********************************************************************************************/
unsigned long RawSignal_2_OLDKAKU(void)
  {
  // conventionele KAKU bestaat altijd uit 25 bits. Ongelijk, dan geen KAKU
  if(RawSignal[0]!=KAKU_OldCodeLength*2)return false;

  // haal uit het signaal een checksum en toets of er aan wordt voldaan
  if(((   (RawSignal[35])>KAKU_0)<<1 
       | ((RawSignal[37])>KAKU_0)<<2 
       | ((RawSignal[39])>KAKU_0)<<3 
       | ((RawSignal[41])>KAKU_0)<<4 
       | ((RawSignal[43])>KAKU_0)<<5 
       | ((RawSignal[45])>KAKU_0)<<6)
                                       !=0x28)return false;

  // haal data uit het signaal.
  return command2event(CMD_KAKU,
          // Home
          ((RawSignal[ 3])>KAKU_0)<<4 
        | ((RawSignal[ 7])>KAKU_0)<<5  
        | ((RawSignal[11])>KAKU_0)<<6 
        | ((RawSignal[15])>KAKU_0)<<7
          // Address      
        | ((RawSignal[19])>KAKU_0)    
        | ((RawSignal[23])>KAKU_0)<<1 
        | ((RawSignal[27])>KAKU_0)<<2 
        | ((RawSignal[31])>KAKU_0)<<3,
          // Command
          ((RawSignal[47])>KAKU_0));
   }

/**********************************************************************************************\
 * Converteert een string volgens formaat "<Home><address>" naar een absoluut KAKU adres [0..255]
 \*********************************************************************************************/
byte KAKUString2address(char* KAKU_String)
  {
  byte x=0,y=false; // teller die wijst naar het het te behandelen teken
  byte c;   // teken uit de string die behandeld wordt
  byte Home=0,Address=0;
 
  while((c=KAKU_String[x++])!=0)
    {
    if(c>='0' && c<='9'){Address=Address*10;Address=Address+c-'0';}// KAKU adres 1 is intern 0
    if(c>='a' && c<='p'){Home=c-'a';y=true;} // KAKU home A is intern 0
    }

  if(y)// oude KAKU notatie [A1..P16] 
    return (Home<<4) | (Address-1);
  else // absoluut adres [0..255]
    return Address;      
  }

