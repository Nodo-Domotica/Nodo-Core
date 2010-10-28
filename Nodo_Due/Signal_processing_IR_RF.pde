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

// ontvangen van RF-codes:
#define RF_TIMEOUT                   5000   // na deze tijd in uSec. wordt een signaal als beëindigd beschouwd.
#define RF_MIN_PULSE                  100   // pulsen korter dan deze tijd uSec. worden als stoorpulsen beschouwd.
#define RF_MIN_RAW_PULSES              32   // =16 bits. Minimaal aantal ontvangen bits*2 alvorens cpu tijd wordt besteed aan decodering, etc. Zet zo hoog mogelijk om CPU-tijd te sparen en minder 'onzin' te ontvangen.

// ontvangen van IR codes
//#define IR_TIMEOUT                  10000   // na deze tijd (uSec.) geen IR siglaal meer ontvangen te hebben, wordt aangenomen dat verzenden van één code gereed is.
#define IR_MIN_PULSE                  100   // pulsen korter dan deze tijd uSec. worden als stoorpulsen beschouwd.
#define IR_MIN_RAW_PULSES              16   // =8 bits. Minimaal aantal ontvangen bits*2 alvorens cpu tijd wordt besteed aan decodering, etc. Zet zo hoog mogelijk om CPU-tijd te sparen en minder 'onzin' te ontvangen.

// vezenden van RF signalen
#define RF_REPEATS 7

// verzenden van IR signalen
#define IR_REPEATS 5

// creatie van NODO signalen
#define NODO_PULSE_0                    500   // PWM: Tijdsduur van de puls bij verzenden van een '0' in uSec.
#define NODO_PULSE_1                   1500   // PWM: Tijdsduur van de puls bij verzenden van een '1' in uSec. (3x NODO_PULSE_0)
#define NODO_SPACE                      500   // PWM: Tijdsduur van de space tussen de bitspuls bij verzenden van een '1' in uSec.    

unsigned long AnalyzeRawSignal(void)
  {
  unsigned long Code=0L;
  
  if(RawSignal[0]==RAW_BUFFER_SIZE)return 0L;     // Als het signaal een volle buffer beslaat is het zeer waarschijnlijk ruis.

  Code=RawSignal_2_KAKU();    
  if(Code)return Code;   // check of het een KAKU signaal is volgens de conventionele KAKU codering.

  Code=RawSignal_2_NewKAKU();
  if(Code)return Code;   // check of het een KAKU signaal is volgens de nieuwe KAKU codering.

  Code=RawSignal_2_32bit();
  return Code;   // Geen Nodo, KAKU of NewKAKU code. Genereer uit het onbekende signaal een (vrijwel) unieke 32-bit waarde uit.  
  }

 /**********************************************************************************************\
 * Deze functie genereert uit een willekeurig gevulde RawSignal afkomstig van de meeste 
 * afstandsbedieningen een (vrijwel) unieke bit code.
 * Zowel breedte van de pulsen als de afstand tussen de pulsen worden in de berekening
 * meegenomen zodat deze functie geschikt is voor PWM, PDM en Bi-Pase modulatie.
 * LET OP: Het betreft een unieke hash-waarde zonder betekenis van waarde.
 \*********************************************************************************************/
unsigned long RawSignal_2_32bit(void)
  {
  int x,y,z;
  int Counter_pulse=0,Counter_space=0;
  int MinPulse=0xffff;
  int MinSpace=0xffff;
  unsigned long CodeP=0L;
  unsigned long CodeS=0L;
  
  // zoek de kortste tijd (PULSE en SPACE)
  x=5; // 0=aantal, 1=startpuls, 2=space na startpuls, 3=1e puls
  while(x<=RawSignal[0]-4)
    {
    if(RawSignal[x]<MinPulse)MinPulse=RawSignal[x]; // Zoek naar de kortste pulstijd.
    x++;
    if(RawSignal[x]<MinSpace)MinSpace=RawSignal[x]; // Zoek naar de kortste spacetijd.
    x++;
    }
  MinPulse+=(MinPulse*S.AnalyseSharpness)/100;
  MinSpace+=(MinSpace*S.AnalyseSharpness)/100;
     
  x=3; // 0=aantal, 1=startpuls, 2=space na startpuls, 3=1e pulslengte
  z=0; // bit in de Code die geset moet worden 
  do{
    if(z>31)
      {
      CodeP=CodeP>>1;
      CodeS=CodeS>>1;
      }
      
    if(RawSignal[x]>MinPulse)
      {
      if(z<=31)// de eerste 32 bits vullen in de 32-bit variabele
          CodeP|=(long)(1L<<z); //LSB in signaal wordt  als eerste verzonden
      else // daarna de resterende doorschuiven
        CodeP|=0x80000000L;
      Counter_pulse++;
      }
    x++;

    if(RawSignal[x]>MinSpace)
      {
      if(z<=31)// de eerste 32 bits vullen in de 32-bit variabele
          CodeS|=(long)(1L<<z); //LSB in signaal wordt  als eerste verzonden
      else // daarna de resterende doorschuiven
        CodeS|=0x80000000L;
      Counter_space++;
      }
    x++;
    z++;
    }while(x<RawSignal[0]);
 
 // geef code terug,maar maak zet de msb nibble (=home) op een niet bestaand Home 0x0F zodat deze niet abusievelijk als commando worden verwerkt.
 if(Counter_pulse>=1 && Counter_space<=1)return CodeP; // data zat in de pulsbreedte
 if(Counter_pulse<=1 && Counter_space>=1)return CodeS; // data zat in de pulse afstand
 return (CodeS^CodeP); // data zat in beide = bi-phase, maak er een leuke mix van.
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
 *
 \*********************************************************************************************/

// Parameters voor check op vrije 433 ether alvorens verzenden signalen:

void WaitForFreeRF(void)
  {
  int x=S.WaitForFreeRF_Time?S.WaitForFreeRF_Time*1000:S.Unit*1000;
  unsigned long FreeTimer;  // meet of de time-out waarde gepasseerd is in milliseconden

  if(Simulate)return;
  
  FreeTimer=millis()+x; // reset de timer.  
  while(FreeTimer>millis())
    {
    if((*portInputRegister(RFport)&RFbit)==RFbit)// Kijk if er iets op de RF poort binnenkomt. (Pin=HOOG als signaal in de ether). 
      {
      if(RFFetchSignal())// Als het een duidelijk signaal was
        FreeTimer=millis()+x; // reset de timer weer.
      }
    digitalWrite(MonitorLedPin,(millis()>>7)&0x01);
    }
  }


 /**********************************************************************************************\
 * Wacht totdat de pin verandert naar status state. Geeft de tijd in uSec. terug. 
 * Als geen verandering, dan wordt na timeout teruggekeerd met de waarde 0L
 \*********************************************************************************************/
unsigned long WaitForChangeState(uint8_t pin, uint8_t state, unsigned long timeout)
	{
        uint8_t bit = digitalPinToBitMask(pin);
        uint8_t port = digitalPinToPort(pin);
	uint8_t stateMask = (state ? bit : 0);
	unsigned long numloops = 0; // keep initialization out of time critical area
	unsigned long maxloops = microsecondsToClockCycles(timeout) / 19;
	
	// wait for the pulse to stop. One loop takes 19 clock-cycles
	while((*portInputRegister(port) & bit) == stateMask)
		if (numloops++ == maxloops)
			return 0;//timeout opgetreden
	return clockCyclesToMicroseconds(numloops * 19 + 16); 
	}


 /*********************************************************************************************\
 * Deze routine zendt een RAW code via RF. 
 * De inhoud van de buffer RawSignal moet de pulstijden bevatten. 
 * RawSignal[0] het aantal pulsen*2
 \*********************************************************************************************/

void RawSendRF(void)
  {
  int x;
    
  if(Simulate)return;
  if(S.WaitForFreeRF==WAITFREERF_ALL)WaitForFreeRF();

  digitalWrite(RF_ReceivePowerPin,LOW);   // Spanning naar de RF ontvanger uit om interferentie met de zender te voorkomen.
  digitalWrite(RF_TransmitPowerPin,HIGH); // zet de 433Mhz zender aan   
  
  for(byte y=0; y<RF_REPEATS; y++) // herhaal verzenden RF code
    {
    x=1;
    while(x<=RawSignal[0])
      {
      digitalWrite(RF_TransmitDataPin,HIGH); // 1
      delayMicroseconds(RawSignal[x++]); 
      digitalWrite(RF_TransmitDataPin,LOW); // 0
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

void RawSendIR(void)
  {
  int x;

  if(Simulate)return;
  
  for(byte y=0; y<IR_REPEATS; y++) // herhaal verzenden IR code
    {
    x=1;
    while(x<=RawSignal[0])
      {
      TCCR2A|=_BV(COM2A0); // zet IR-modulatie AAN
      delayMicroseconds(RawSignal[x++]); 
      TCCR2A&=~_BV(COM2A0); // zet IR-modulatie UIT
      delayMicroseconds(RawSignal[x++]); 
      }
    }
  }

 /*********************************************************************************************\
 * Deze routine berekend de RAW pulsen van een 32-bit Nodo-code en plaatst deze in de buffer RawSignal
 * RawSignal.Bits het aantal pulsen*2+startbit*2 ==> 66
 * 
 \*********************************************************************************************/
void Nodo_2_RawSignal(unsigned long Code)
  {
  byte BitCounter,y;

  y=1;
  
  // begin met een startbit. 
  RawSignal[y++]=NODO_PULSE_1*2; 
  RawSignal[y++]=NODO_SPACE; 

  // de rest van de bits 
  for(BitCounter=0; BitCounter<=31; BitCounter++)
    {
    if(Code>>BitCounter&1)
      RawSignal[y++]=NODO_PULSE_1; 
    else
      RawSignal[y++]=NODO_PULSE_0;   
    RawSignal[y++]=NODO_SPACE;   
    }
  RawSignal[y-1]=NODO_SPACE*32;// laatste SPACE is tevens wachttijd tussen pulstreinen
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
  int IR_RawCodeLength=1;
  unsigned long PulseLength;

  do{// lees de pulsen in microseconden en plaats deze in een tijdelijke buffer
    PulseLength=WaitForChangeState(IR_ReceiveDataPin, LOW, S.AnalyseTimeOut); // meet hoe lang signaal LOW (= PULSE van IR signaal)
    if(PulseLength<IR_MIN_PULSE)return false;
    RawSignal[IR_RawCodeLength++]=PulseLength;
    PulseLength=WaitForChangeState(IR_ReceiveDataPin, HIGH, S.AnalyseTimeOut); // meet hoe lang signaal HIGH (= SPACE van IR signaal)
    RawSignal[IR_RawCodeLength++]=PulseLength;
    }while(IR_RawCodeLength<RAW_BUFFER_SIZE && PulseLength!=0);// Zolang nog niet alle bits ontvangen en er niet vroegtijdig een timeout plaats vindt

  if(IR_RawCodeLength>=IR_MIN_RAW_PULSES)
    {
    RawSignal[0]=IR_RawCodeLength-1;
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
  int RF_RawCodeLength=1;
  unsigned long PulseLength;

  do{// lees de pulsen in microseconden en plaats deze in een tijdelijke buffer
    PulseLength=WaitForChangeState(RF_ReceiveDataPin, HIGH, RF_TIMEOUT); // meet hoe lang signaal HIGH (= PULSE van RF signaal)
    if(PulseLength<RF_MIN_PULSE)return false; // gelijk weer terug als het alleen maar een korte stoorpuls was.

    RawSignal[RF_RawCodeLength++]=PulseLength;
    PulseLength=WaitForChangeState(RF_ReceiveDataPin, LOW, RF_TIMEOUT); // meet hoe lang signaal LOW (= SPACE van RF signaal)
    RawSignal[RF_RawCodeLength++]=PulseLength;
    }while(RF_RawCodeLength<RAW_BUFFER_SIZE && PulseLength!=0);// Zolang nog niet alle bits ontvangen en er niet vroegtijdig een timeout plaats vindt

  if(RF_RawCodeLength>=RF_MIN_RAW_PULSES)
    {
    RawSignal[0]=RF_RawCodeLength-1;// -1 omdat de teller RF_RawCodeLength++ altijd met één teveel uit de loop komt.
    return true;
    }
  RawSignal[0]=0;
  return false;
  }

