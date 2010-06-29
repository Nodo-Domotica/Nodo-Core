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

// verzenden van RF codes:
#define RF_REPEAT_DELAY                 0   // aantal milliseconden wachttijd tussen herhalingen bij verzenden RF (zit nu in sync pulsen)
#define RF_FINISHED_DELAY               0   // tijd in milliseconden dat gewacht moet worden na verzenden van één complete code van -n- herhalingen 

// Parameters voor check op vrije 433 ether alvorens verzenden signalen:

// ontvangen van RF-codes:
#define RF_TIMEOUT                   3000   // na deze tijd in uSec. wordt een signaal als beëindigd beschouwd
#define RF_MIN_RAW_PULSES              32   // =16 bits. Minimaal aantal ontvangen bits*2 alvorens cpu tijd wordt besteed aan decodering, etc. Zet zo hoog mogelijk om CPU-tijd te sparen en minder 'onzin' te ontvangen.
#define RF_FREE_TIMEOUT             10000   // tijd in milliseconden waarna wachten op vrije RF-ether wordt afgebroken 
#define RF_FREE_FRAME              250000   // aantal microseconden dat de band vrij van signalen moet zijn voordat verzonden mag worden
#define RF_MIN_PULSE                  100   // pulsen korter dan deze tijd uSec. worden als stoorpulsen beschouwd.

// verzenden van IR codes
#define IR_REPEAT_DELAY                 0   // aantal milliseconden wachttijd tussen herhalingen bij verzenden IR (zit nu in sync pulsen)
#define IR_FINISHED_DELAY               0   // tijd in milliseconden dat gewacht moet worden na verzenden van één complete code van -n- herhalingen 

// ontvangen van IR codes
#define IR_TIMEOUT                   5000   // na deze tijd (uSec.) geen IR siglaal meer ontvangen te hebben, wordt aangenomen dat verzenden van één code gereed is.
#define IR_MIN_PULSE                  100   // pulsen korter dan deze tijd uSec. worden als stoorpulsen beschouwd.
#define IR_MIN_RAW_PULSES              16   // =8 bits. Minimaal aantal ontvangen bits*2 alvorens cpu tijd wordt besteed aan decodering, etc. Zet zo hoog mogelijk om CPU-tijd te sparen en minder 'onzin' te ontvangen.
#define IR_ANALYSE_SHARPNESS            2   // 2=50%, 3=33% 4=25% 5=20% let op: >0 !!! scherpte bij analyse. Hoe lager deze waarde, hoe meer variate in signaal mag zitten

// creatie van IR signalen
#define IR_PULSE_0                     500   // PWM: Tijdsduur van de puls bij verzenden van een '0' in uSec.
#define IR_PULSE_1                    1500   // PWM: Tijdsduur van de puls bij verzenden van een '1' in uSec.
#define IR_SPACE                       500   // PWM: Tijdsduur van de space tussen de bitspuls bij verzenden van een '1' in uSec.    


unsigned long RF_ReadyTimer;

//*******************************************************************************************************************************************

unsigned long AnalyzeSignal(void)
  {
  int x,y,z,SPT, SST, P, S;
  boolean monster[16];
  unsigned long Code=0L;

  // Als het signaal een volle buffer beslaat is het zeer waarschijnlijk ruis.
  if (RawSignal[0]==RAW_BUFFER_SIZE) { return 0; }

  // check of het een KAKU signaal is volgens de conventionele KAKU codering.
  Code=RawSignal_2_KAKU();
  if(Code!=0L)
    {
    E.Type=CMD_KAKU;
    return Code;
    }
  
  // check of het een KAKU signaal is volgens de nieuwe KAKU codering.
  Code=RawSignal_2_NewKAKU();
  if(Code!=0L)
    {
    E.Type=CMD_KAKU_NEW;
    return Code;
    }
  
  // check of het een X10 signaal is
  Code=RawSignal_2_X10();
  if(Code!=0L)
    {
    E.Type=CMD_X10;
    return Code;
    }
 
  Code=0L;

  // Geen KAKU. Bepaal modulatie soort a.d.h.v. stappen A,B,C
  
  // A: zoek de kortste PULSE en kortste SPACE
  SPT=IR_TIMEOUT;
  SST=IR_TIMEOUT;
  x=1;
  while(x<RawSignal[0]-1)
    {
    if(RawSignal[x]<SPT)SPT=RawSignal[x]; // Zoek naar de korte HIGH puls in de tabel. oneven=High
    x++;
    if(RawSignal[x]<SST)SST=RawSignal[x]; // Zoek naar de korte LOW puls in de tabel. even=Low
    x++;
    }

  // zoek de 'langste' korte puls en de 'langste' korte space 
  P=SPT/IR_ANALYSE_SHARPNESS;
  S=SST/IR_ANALYSE_SHARPNESS;
  y=true;
  while(y)
    {
    y=false;
    x=1;
    while(x<RawSignal[0]-1)
      {
      if(RawSignal[x]>SPT && RawSignal[x]<=(SPT+P))
        {
        SPT=RawSignal[x];
        y=true;
        }
      x++;
      if(RawSignal[x]>SST && RawSignal[x]<=(SST+S))
        {
        SST=RawSignal[x];
        y=true;
        }
      x++;   
      }
    }
  // de maximale lengten van een pulse en een space zijn nu bekend. Iedere puls of space die langer is, is een '1'-bit.  
  
  // B: tel het aantal lange SPACES en PULSES.
  P=0; //aantal lange PULSE tijden
  S=0; //aantal lange SPACE tijden

  for(x=1;x<RawSignal[0];x++) // x=5, sla startbit over. soms twee startbits
    {
    if(RawSignal[x]>SPT)P++;// tel aantal lange PULSE tijden
    x++;
    if(RawSignal[x]>SST)S++;// tel aantal lange SPACE tijden
    };

  
  // C: Bepaal a.d.h.v. aantal PULSEN en SPACES het type modulatie
  if(P>=1 && S<=1)x=1;// Pulselentgth modulation data in PULSE: / SIRC / RECS 80 (High). Deawoo, JVC, Panasonic, RECS-80
  if(P<=1 && S>=1)x=2;// Pulselentgth modulation data in SPACE: / SIRCS / RECS 80 (Low). Sony, 
  if(P==1 && S>=1 && RawSignal[1]<=SPT) x=3; // díe ene lange puls geen startbit is maar zich ergens anders bevindt, dan ook bi-phase;
  if(S==1 && P>=1 && RawSignal[2]<=SST) x=3; // díe ene lange space niet direct na de startbit maar zich ergens anders bevindt, dan ook bi-phase;
  if(P>1  && S>1 )x=3;// BI-Phase / manchester modulation data in PULSE & SPACE: / RC-5 / RC-6), Phillips

  switch(x)
    {
    case 1:
      x=3; // start bij 3 want 0=aantal bits in array, 1=puls startbit, 2=space startbit, 3=eerste pulse, die moeten we hebben 
      z=0; // bit in de Code die geset moet worden 

      do{
        if(z>31)Code=Code>>1;
        if(RawSignal[x]>SPT)
          {
          if(z<=31)// de eerste 32 bits vullen in de 32-bit variabele
              Code|=(long)(1L<<z); //LSB in signaal wordt  als eerste verzonden
          else // daarna de resterende doorschuiven
            Code|=0x80000000L;
          }
        z++;
        x+=2;
        }while(x<RawSignal[0]);
      break;

    case 2:
      x=4; // start bij 4 want 0=aantal bits in array, 1=puls startbit, 2=space startbit, 3=pulse dus overslaan want data zit in de space. 
      z=0; // bit in de Code die geset moet worden 
      do{
        if(z>31)Code=Code>>1;
        if(RawSignal[x]>SST)
          {
          if(z<=31)// de eerste 32 bits vullen in de 32-bit variabele
              Code|=(long)(1L<<z); //LSB in signaal wordt als eerste verzonden
          else // daarna de resterende doorschuiven
            {
            Code|=0x80000000L;
            }
          }
        z++;
        x+=2;
        }while(x<RawSignal[0]);
      break;
      
    case 3:
      // haal uit de transities hoog-laag en laag-hoog het signaal
      x=1; //eerste PULSE-SPACE paar,      
      monster[0]=0;   
      monster[1]=0;   
      monster[2]=0;   
      monster[3]=0;   
      monster[4]=0;   
      monster[5]=0;
      int monstercounter=0; 

      do{ 
        if(RawSignal[x++]<=SPT) //start bij x=1, dus dit is de PULSE
          monstercounter++;
        else
          monstercounter+=2;

        if(RawSignal[x++]<=SST) // x is nu één opgehoogd, dus nu wordt de Space vergeleken
            monster[monstercounter++]=1;
        else
            {monster[monstercounter++]=1;monster[monstercounter++]=1;}
            
        // nu zijn er [monstercounter] monsters gevuld. Uit één SPACE/PULSE paar zijn dit er altijd 2,3 of 4
        // het tweede monster bevat nu de ontvangen bit
       
        do
          {
          Code=Code<<1;
          Code|=(long)(monster[1]); //LSB in signaal wordt  als eerste verzonden

          // schuif de inhoud van de array twee monsters naar voren
          for(y=0;y<=monstercounter;y++)monster[y]=monster[y+2];
          monstercounter-=2;
          }while(monstercounter>=1);//zolang er nog voldoende gevulde monsters zijn...

        // gevulde monsters zijn op, bemonster nu volgende SPACE-PULSE paar
        }while(x<=RawSignal[0]);
      break;
    }
  E.Type=CMD_TYPE_UNKNOWN;
  return Code;
  }
 
 /**********************************************************************************************\
 * Opwekken draaggolf van 38Khz voor verzenden IR. 
 * Deze code is gesloopt uit de library FrequencyTimer2.h omdat deze library niet meer door de compiler versie 0015 kwam. 
 * Tevens volledig uitgekleed.  
 \*********************************************************************************************/
static uint8_t enabled = 0;

static void IR38Khz_set()  
  {
  uint8_t pre, top;
  unsigned long period=208; // IR_TransmitCarrier=26 want pulsen van de IR-led op een draaggolf van 38Khz. (1000000/38000=26uSec.) Vervolgens period=IR_TransmitCarrier*clockCyclesPerMicrosecond())/2;  // period =208 bij 38Khz
  pre=1;
  top=period-1;
  TCCR2B=0;
  TCCR2A=0;
  TCNT2=0;
  ASSR&=~_BV(AS2);    // use clock, not T2 pin
  OCR2A=top;
  TCCR2A=(_BV(WGM21)|(enabled?_BV(COM2A0):0));
  TCCR2B=pre;
  }  


 
 /**********************************************************************************************\
 * Deze functie wacht totdat de 433 band vrij is of er een timeout heeft plaats gevonden 
 * De volgende methode wordt toegepast:
 * - Aanname: Alle pulsen korte dan RF_MIN_PULSE zijn stoorpulsen en geen onderdeel van
 *            geldig signaal.
 * - Uitgangspunt: Nodo's mogen niet tegelijk gaan zenden.
 * Deze routine werkt goed voor de Aurel ontvangers, maar niet voor de ontvangers die ruis doorgeven bij geen signaal
 \*********************************************************************************************/

  #define RF_MIN_PULSE                  350   // pulsen korter dan deze tijd microseconden worden als onbelangrijke stoorpulsen beschouwd.
  #define RF_FREE_FRAME                 500   // aantal milliseconden dat geluisterd moet worden of de 433 band vrij is
  #define RF_FREE_WAIT                  500   // wachttijd in milliseconden alvorens de volgende poging wordt gedaan
  #define RF_FREE_TRY                    10   // aantal pogingen dat wordt gekeken of de ether vrij is.

  boolean WaitForFreeRF(void)
    {
    unsigned long TimeOutTimer;  // meet of de time-out waarde gepasseerd is in milliseconden
    byte Try=0;
    int Busy;
    
    if(Simulate)return true;
    delay(S.Unit*25);// korte pauze om alle Nodo's ten opzichte van elkaar net iets asynchroon te laten meten.
    
    while(true) 
      {
      Busy=0;
      TimeOutTimer=millis()+RF_FREE_FRAME;  // tijdsbestek waarin geluisterd wordt of de ether vrij is
      while(millis()<TimeOutTimer)Busy+=(pulseIn(RF_ReceiveDataPin,HIGH,RF_FREE_FRAME)>RF_MIN_PULSE);

      if(Busy>5)// een enkele stoorpuls is acceptabel, daarom groter dan één.
        {
        if(Try++<RF_FREE_TRY) // de ether was bezet maar er zijn nog pogingen te gaan
          delay(RF_FREE_WAIT*S.Unit);
        else
          {// ether was bezet, maar de time-out tijd is verstreken
          Serial.print(Text(Text_04));
          PrintTerm();
          return false;
          }
        }
      else // de ether is vrij
        return true;
      }
    }

// /**********************************************************************************************\
// * Deze functie wacht totdat de 433 band vrij is of er een timeout heeft plaats gevonden 
// * Dit is de versie uit Build 66. 
// \*********************************************************************************************/
//  void WaitForFreeRF(void)
//    {
//    unsigned long PulseLength;  // meet of tijdsbestek waarin de ether vrij moet zijn in milliseconden
//    unsigned long TimeOutTimer;  // meet of de time-out waarde gepasseerd is in milliseconden
//    
//    if(Simulate)return;
//    TimeOutTimer=millis()+RF_FREE_TIMEOUT;  // Timeout: na deze tijd terugkeren, ongeacht of de ether vrij was of niet
//
//    do 
//      {
//      // wacht op een pulse. Tevens Unit code hierin verwerkt zodat niet alle Nodo's gelijktijdig gaan zenden
//      // units met een hoger adres moeten langer op hun beurt wachten. 
//      PulseLength=pulseIn(RF_ReceiveDataPin,HIGH,RF_FREE_FRAME + (unsigned long)(S.Unit * RF_FREE_FRAME>>2));
//      }while(PulseLength!=0 && PulseLength>RF_MIN_PULSE && millis()<TimeOutTimer);
//      // Uit de while-loop als:
//      // - Er absolute stilte in de ether was gedurende RF_FREE_FRAME milliseconden
//      // - Er een puls was maar die was kleiner dan RF_MIN_PULSE
//      // - De ether was vol maar de tijd RF_FREE_TIMEOUT is verstreken.      
//    }


 /**********************************************************************************************\
 * Wacht totdat de pin verandert naar status state. Geeft de tijd in uSec. terug. 
 * Als geen verandering, dan wordt na timeout teruggekeerd met de waarde 0L
 \*********************************************************************************************/
unsigned long WaitForChangeState(uint8_t pin, uint8_t state, unsigned long timeout)
	{
        uint8_t bit=digitalPinToBitMask(pin);
        uint8_t port=digitalPinToPort(pin);
	uint8_t stateMask=(state?bit:0);
	unsigned long width = 0; // keep initialization out of time critical area
	unsigned long maxloops = microsecondsToClockCycles(timeout) / 16;
	
	// wait for the pulse to stop. One loop takes 16 clock-cycles
	while((*portInputRegister(port)&bit)==stateMask)
		if (width++==maxloops)
			return 0L;//timeout opgetreden
	return clockCyclesToMicroseconds(width*19+16); 
	}


 /*********************************************************************************************\
 * Deze routine zendt een RAW code via RF. 
 * De inhoud van de buffer RawSignal moet de pulstijden bevatten. 
 * RawSignal[0] het aantal pulsen*2
 \*********************************************************************************************/

void RawCodeSend_RF(byte repeats)
  {
  int x;
    
  if(Simulate)return;
  digitalWrite(RF_ReceivePowerPin,LOW);   // Spanning naar de RF ontvanger uit om interferentie met de zender te voorkomen.
  digitalWrite(RF_TransmitPowerPin,HIGH); // zet de 433Mhz zender aan   
  
  for(byte y=0; y<repeats; y++) // herhaal verzenden IR code
    {
    x=1;
    delay(RF_REPEAT_DELAY); // pauze tussen herhalingen.
    while(x<=RawSignal[0])
      {
      digitalWrite(RF_TransmitDataPin,HIGH); // 1
      delayMicroseconds(RawSignal[x++]); 
      digitalWrite(RF_TransmitDataPin,LOW);// 0
      delayMicroseconds(RawSignal[x++]); 
      }
    }
  digitalWrite(RF_TransmitPowerPin,LOW); // zet de 433Mhz zender weer uit
  digitalWrite(RF_ReceivePowerPin,HIGH); // Spanning naar de RF ontvanger weer aan. 
  }


 /*********************************************************************************************\
 * Deze routine zendt een 32-bits code via IR. 
 * De inhoud van de buffer RawSignal moet de pulstijden bevatten. 
 * RawSignal[0] het aantal pulsen*2
 * Pulsen worden verzonden op en draaggolf van 38Khz.
 \*********************************************************************************************/

void RawCodeSend_IR(byte repeats)
  {
  int x;
  if(Simulate)return;
  
  for(byte y=0; y<repeats; y++) // herhaal verzenden IR code
    {
    x=1;
    while(x<=RawSignal[0])
      {
      TCCR2A|=_BV(COM2A0);// zet IR-modulatie AAN
      delayMicroseconds(RawSignal[x++]); 
      TCCR2A&=~_BV(COM2A0);// zet IR-modulatie UIT
      delayMicroseconds(RawSignal[x++]); 
      }
    delay(IR_REPEAT_DELAY); // pauze tussen herhalingen
    }
  delay(IR_FINISHED_DELAY); // pauze na verzenden code
  }

 /*********************************************************************************************\
 * Deze routine berekend de RAW pulsen van een 32-bit Nodo-code en plaatst deze in de buffer RawSignal
 * RawSignal.Bits het aantal pulsen*2+startbit*2 ==> 66
 \*********************************************************************************************/
void RawCodeCreate(unsigned long Code)
  {
  byte BitCounter,y;
  
  y=1;
  
  // begin met een startbit. 
  RawSignal[y++]=IR_PULSE_1*2; 
  RawSignal[y++]=IR_SPACE; 

  // de rest van de bits 
  for(BitCounter=0; BitCounter<=31; BitCounter++)
    {
    if(Code>>BitCounter&1)
      RawSignal[y++]=IR_PULSE_1; 
    else
      RawSignal[y++]=IR_PULSE_0;   
    RawSignal[y++]=IR_SPACE;   
    }
  RawSignal[0]=66; //  1 startbit bestaande uit een pulse/space + 32-bits is 64 pulse/space = totaal 66
  }

 
 /**********************************************************************************************\
 * Haal de pulsen van de TSOP1738 en plaats de tijden in een buffer.
 * in rust is de uitgang van de TSOP hoog.
 * op het momen dat hier aangekomen wordt is de startbit actief, dus de ingang laag.
 * Revision 01, 17-12-2009, P.K.Tonkes@gmail.com
 \*********************************************************************************************/

boolean IRFetchSignal(void)
  {
  int IR_RawCodeLength=3; //first two sync pulses are skipped by detection;
  unsigned long PulseLength;

  do{// lees de pulsen in microseconden en plaats deze in een tijdelijke buffer
    PulseLength=WaitForChangeState(IR_ReceiveDataPin, LOW,IR_TIMEOUT); // meet hoe lang signaal LOW (= PULSE van IR signaal)
    if(PulseLength<IR_MIN_PULSE)return false;
    RawSignal[IR_RawCodeLength++]=PulseLength;
    PulseLength=WaitForChangeState(IR_ReceiveDataPin, HIGH,IR_TIMEOUT); // meet hoe lang signaal HIGH (= SPACE van IR signaal)
    RawSignal[IR_RawCodeLength++]=PulseLength;
    }while(IR_RawCodeLength<RAW_BUFFER_SIZE && PulseLength!=0);// Zolang nog niet alle bits ontvangen en er niet vroegtijdig een timeout plaats vindt

  if(IR_RawCodeLength>=IR_MIN_RAW_PULSES)
    {
    RawSignal[0]=IR_RawCodeLength-1;
    RawSignal[1]=0;
    RawSignal[2]=0;
    return true;
    }
  RawSignal[0]=0;
  return false;
  }

 /**********************************************************************************************\
 * Haal de pulsen van de RF433 ontvanger en plaats de tijden in een buffer.
 * in rust is de uitgang van de ontvanger laag
 * op het momen dat hier aangekomen wordt is de startbit actief, dus de ingang hoog.
 * Revision 01, 02-02-2010, P.K.Tonkes@gmail.com
 \*********************************************************************************************/
boolean RFFetchSignal(void)
  {
  int RF_RawCodeLength=3; //first two sync pulses are skipped by detection
  unsigned long PulseLength;

  do{// lees de pulsen in microseconden en plaats deze in een tijdelijke buffer
    PulseLength=WaitForChangeState(RF_ReceiveDataPin, HIGH,RF_TIMEOUT); // meet hoe lang signaal HIGH (= PULSE van RF signaal)
    if(PulseLength<RF_MIN_PULSE)return false; // gelijk weer terug als het alleen maar een korte stoorpuls was.

    RawSignal[RF_RawCodeLength++]=PulseLength;
    PulseLength=WaitForChangeState(RF_ReceiveDataPin, LOW,RF_TIMEOUT); // meet hoe lang signaal LOW (= SPACE van RF signaal)
    RawSignal[RF_RawCodeLength++]=PulseLength;
    }while(RF_RawCodeLength<RAW_BUFFER_SIZE && PulseLength!=0);// Zolang nog niet alle bits ontvangen en er niet vroegtijdig een timeout plaats vindt

  if(RF_RawCodeLength>=RF_MIN_RAW_PULSES)
    {
    RawSignal[0]=RF_RawCodeLength-1;// -1 omdat de teller RF_RawCodeLength++ altijd met één teveel uit de loop komt.
    RawSignal[1]=0;
    RawSignal[2]=0;
    return true;
    }
  RawSignal[0]=0;
  return false;
  }
