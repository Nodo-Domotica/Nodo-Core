  /**************************************************************************\
    This file is part of Nodo Due, (c) Copyright Paul Tonkes

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

/**********************************************************************************************\
 * Geef een geluidssignaal met toonhoogte van 'frequentie' in Herz en tijdsduur 'duration' in milliseconden.
 * LET OP: toonhoogte is slechts een grove indicatie. Deze routine is bedoeld als signaalfunctie
 * en is niet bruikbaar voor toepassingen waar de toonhoogte zuiver/exact moet zijn.
 * Definieer de constante:
 * #define BuzzerPin <LuidsprekerAansluiting>
 * Neem in setup() de volgende regel op:
 * pinMode(BuzzerPin, OUTPUT);
 * Routine wordt verlaten na beeindiging van de pieptoon.
 * Revision 01, 13-02-2009, P.K.Tonkes@gmail.com
 \*********************************************************************************************/

void Beep(int frequency, int duration)//Herz,millisec 
  {
  long halfperiod=500000L/frequency;
  long loops=(long)duration*frequency/(long)1000;
  for(loops;loops>0;loops--) 
    {
    digitalWrite(BuzzerPin, HIGH);
    delayMicroseconds(halfperiod);
    digitalWrite(BuzzerPin, LOW);
    delayMicroseconds(halfperiod);
    }
  }
 
 /**********************************************************************************************\
 * Geeft een belsignaal.
 * Revision 01, 09-03-2009, P.K.Tonkes@gmail.com
 \*********************************************************************************************/
void Alarm(int Variant,int Duration)
  {
   byte x,y;
   if(Duration==0)Duration=1;

   switch (Variant)
    { 
    case 1:// four beeps
      for(y=1;y<=(Duration>1?Duration:1);y++)
        {
        Beep(3000,30);
        delay(100);
        Beep(3000,30);
        delay(100);
        Beep(3000,30);
        delay(100);
        Beep(3000,30);
        delay(1000);
        }    
      break;

    case 2: // whoop up
      for(y=1;y<=(Duration>1?Duration:1);y++)
        {
        for(x=1;x<=50;x++)
            Beep(250*x/4,20);
        }          
      break;

    case 3: // whoop down
      for(y=1;y<=(Duration>1?Duration:1);y++)
        {
        for(x=50;x>0;x--)
            Beep(250*x/4,20);
        }          
      break;

    case 4:// S.O.S.
      for(y=1;y<=(Duration>1?Duration:1);y++)
        {
          Beep(1200,50);
          delay(100);
          Beep(1200,50);
          delay(100);
          Beep(1200,50);
          delay(200);
          Beep(1200,300);
          delay(100);
          Beep(1200,300);
          delay(100);
          Beep(1200,300);
          delay(200);
          Beep(1200,50);
          delay(100);
          Beep(1200,50);
          delay(100);
          Beep(1200,50);
          if(Duration>1)delay(500);
        }
      break;
          
     case 5:// ding-dong
       for(x=0;x<Duration;x++)
         {
         if(x>0)delay(2000);
         Beep(1500,500);
         Beep(1200,500);
         }    
       break;

    case 6: // phone ring
      for(x=0;x<(15*Duration);x++)
        {
          Beep(1000,40);
          Beep(750,40);
        }
        break;

    case 7: // boot
        Beep(1500,100);
        Beep(1000,100);
        break;
                       
    default:// beep
       Beep(2000,20*(Duration>1?Duration:1));
       break;
    }
  }



