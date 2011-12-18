/**************************************************************************************************************************\


Known Errors / ToDo:
- Found in r306: als ethernet niet aangesloten dan start de nodo (soms) niet op.
- Found in r306: ijk/kalibreer procedure werkt nog niet goed
- Found in r306: Status WiredAnalog 22 is mogelijk
- Found in r306: Nodo due compatibel maken n.a.v. omnummeren CMD_...
- Sendbusy en Waitbusy testen of mmi en oppikken commando nog goed werken. Queue testen
- Wired poorten testen incl. Trigger, Threshold.
- Found in r306: Na volledig volschrijven van de eventlist blijft Nodo hangen in continue uitvoeren van het reboot event.
- Found in r306: Tijdens type van een regel in Telnet kan het typen worden onderbroken door binnenkomend event. (weergave verminkt)
- Found in r306: Na een RaiseError vanuit een TelNet sessie werkt invoer commando eerst daarop volgende keer niet. Eerste commando wordt overgeslagen.

Ideeën:
- versturen eventlist naar andere Nodo (over IR/RF)
- Moet een self-learned IP adres in de settings worden opgeslagen of niet??? Nu niet het geval.

Aanpassingen r307:
- Found in r306: Events naar IP adressen worden niet correct verzonden als tegelijkertijd een TelNet sessie actief is.
- Verzenden van een EventGhost Event naar IP nu voorzien van retry bij fout in verzending.

Aanpassingen r306:
- Found in r306: Terminal On setting niet bewaard na reboot.
- Found in r306: Terugsturen events naar bekende IP adressen werkt niet goed. (self-learning)
- Found in r306: Invoer commando via telnet werkt niet lekker;
- Found in r306: EventlistWrite werkt niet
- Found in r306: prompt teken weggehaald uit terminal venster. lasig altijd weer te geven en voegt niets toe.
- Found in r306: Hex-events worden niet geaccepteerd.


Release V3.0.0: Functionele aanpassingen ten opzichte van de 1.2.1 release
- Ethernet intergratie. Events van EventGhost (PC, Android) ontvangen en verzenden over IP;
- Bij opstarten de melding "Booting..." omdat wachten op IP adres van de router de eerste keer even tijd in beslag kan nemen.
- Indien SDCard geplaatst, dan logging naar Log.txt.
- UserPlugin maakt mogelijk om gebruiker zelf code aan de Nodo code toe te voegen.
- Aantal timers verhoogd van 15 naar 32
- Aantal gebruikersvariabelen verhoogd van 15 naar 32
- 8 digitale wired poorten i.p.v. 4
- 8 analoge wired poorten i.p.v. 4
- Eventlist uitgebreid van 120 posities naar 256
- queue voor opvangen events tijdens delay van 15 uitgebreid naar 32 events.
- Toevoeging commando "IP" IP adres
- Welkomsttekst uitgebreid met de IP-settings
- Welkomsttekst uitgebreid met melding logging naar SDCard.
- Toevoeging commando "Password"
- Toevoeging commando "Reboot"
- Ccommando "Status" geeft in als resultaat in de tag "output=Status"
- Toevoeging commando "Terminal <[On|Off]>,<prompt [On|Off]>". tevens "Status" commando uitgebreid met setting "Terminal"
- Commando "Display" Vervallen.
- Commando hoeft niet meer te worden afgesloten met een puntkomma. Puntkomma wordt alleen gebruikt om meerdere commandos per regel te scheiden.
- Toevoeging commando "VariableSave", slaat alle variabelen op zodat deze na een herstart weer worden geladen. Opslaan gebeurt NIET meer automatisch.
- Toevoeging commando "LogShow": laat de inhoud van de log op SDCard zien
- Toevoeging commando "LogErase": wist de logfile
- Commando "RawSignalGet" en "RawSignlPut" vervallen;
- Toevoeging commando "RawSignalSave <key>". Slaat pulsenreeks van het eerstvolgende ontvangen signaal op op SDCard onder opgegeven nummer
- Toevoeging commando "RawSignalSend <key>". Verzend een eerder onder <key> opgeslagen pulsenreeks. Als <key> = 0, dan wordt huidige inhoud verzonden
- "SendVarUserEvent" renamed naar "VariableSendUserEvent"
- nieuw commando: "VariableUserEvent" genereert een userevent op basis van de inhoud van twee variabelen.
- Commando "TransmitSettings" vervallen. Vervangen door "TransmitIR", "TransmitRF"
- Commando "Simulate" vervallen. Kan worden opgelost met de nieuwe commandos "TransmitIR" en "TransmitRF".
- Aanpassing weergave van datum/tijd. De dag wordt na NA de datum weergegeven ipv VOOR (ivm kunnen sorteren logfile).
- Errors worden nu weergeven met een tekstuele toelichting wat de fout veroorzaakte: "Error=<tekst>". 
- "Timestamp=" wordt nu iets anders weergegeven als "Date=yyyy-mm-dd, Time=hh:mm".
- Variabelen worden na wijzigen niet meer automatisch opgeslagen in het EEPROM geheugen. Opslaan kan nu met commando "VariableSave"
- Tag Direction vervangen door Input, Output, Internal
- Een EventlistWrite commando met bijhehorende event en actie moeten zich binnen 1 regel bevinden die wordt afgesloten met een \n
- Verzenden naar Serial vindt pas plaats als er door ontvangst van een teken gecontroleerd is dat seriele verbinding nodig is;
- Commando "VariableSetWiredAnalog" vervallen. Past niet meer bij 10-bit berwerking en calibratie/ijking
- Commaando toegevoegd "WiredAnalogSend"
- Commaando toegevoegd "WiredAnalogCalibrate <poort> <High|Low> <ijkwaarde>"
- Commando "WiredRange" vervallen. Overbodig geworden n.a.v. calibratie/ijking funktionaliteit.
- Event aangepast "WiredAnalog". Geeft nu gecalibreerde waarde weer metdecimalen achter de komma

Onder de motorkap:
- Verwerken van seriele gegevens volledig herschreven
- omnummeren van tabel met events,commando en waarden om plaats te maken voor uitbreiding commandoset. LET OP: niet meer compatibel met de Uno 1.2.1 en lagere versies!

\**************************************************************************************************************************/

//void PrintRawSignal(void)
//  {
//  Serial.print("*** RawSignal=");//???
//  for(int x=1;x<=RawSignal.Number;x++)
//    {
//    Serial.print(RawSignal.Pulses[x],DEC);//??? Debug  
//    Serial.print(",");//???
//    }
//  Serial.println();//???    
//  Serial.print("*** RawSignal.Number=");Serial.println(RawSignal.Number,DEC);//??? Debug
//  }
  
  // De WiredAnalog heeft een afwijkende opbouw van Par1 en Par2. Dit omdat er in één parameter van 8-bits niet voldoende ruimte is om
  // een 10-bits analoge waarde vast te houden. Om deze reden worden Par1 en Par2 samengevoegd tot één 16-bit waarde die vervolgens
  // de volgende opbouw heeft: WWWWxsnnnnnnnnnn, waarbij W de Wired poort is en n de analoge waarde en s de sign-bit voor negatieve getallen


int calibrated2int(byte Par1, byte Par2)
  {
  int Value;
  
//Serial.print("3*** Par1=");Serial.println(Par1,BIN);//??? Debug
//Serial.print("3*** Par2=");Serial.println(Par2,BIN);//??? Debug  
  
  Value=(Par1 | Par2<<8) & 0x3ff; // 10-bit waarde
  if(Par2&0x4) // 11e bit in de 16-bits combinatie van Par2 Par1 is het sign-teken.
    Value=-(Value);

Serial.print("3*** Value=");Serial.println(Value,DEC);//??? Debug

  return Value;
  }



int analog2port(byte Par1, byte Par2)
  {
  //Serial.print("2*** Par1=");Serial.println(Par1,BIN);//??? Debug
  //Serial.print("2*** Par2=");Serial.println(Par2,BIN);//??? Debug
  //Serial.print("2*** Port=");Serial.println(((Par2>>4)&0xf)+1,DEC);//??? Debug
  return ((Par2>>4)&0xf);    
  }

void int2calibrated(byte *Par1, byte *Par2, int Port, int Value)
  {
  byte P1,P2;

  // mapping en calibratie nog inbouwen  
  //  Serial.print("4*** Port=");Serial.println(Port,DEC);//??? Debug
  //  Serial.print("4*** Value=");Serial.println(Value,DEC);//??? Debug
  
  P1=(Value   )&0xff; // 1e acht bits
  P2=(Value>>8)&0x03; // laatste twee bits 
  
//  Serial.print("4a*** Par1=");Serial.println(P1,BIN);//??? Debug
//  Serial.print("4a*** Par2=");Serial.println(P2,BIN);//??? Debug

  if(Value<0)
    P2=P2 | 0x04; // set 11e bit om aan te geven dat de waarde negaief is

//  Serial.print("4b*** Par1=");Serial.println(P1,BIN);//??? Debug
//  Serial.print("4b*** Par2=");Serial.println(P2,BIN);//??? Debug
    
  P2=P2 | (Port<<4); // poort op bit 13..16 plaatsen. 
  
//  Serial.print("4c*** Par1=");Serial.println(P1,BIN);//??? Debug
//  Serial.print("4c*** Par2=");Serial.println(P2,BIN);//??? Debug
  
  *Par1=P1;
  *Par2=P2;
  }

char* calibrated2str(byte Par1, byte Par2)
  {
  static char str[15];
  int x;
  
 // nog uitwerken: aantal decimalen achter de komma afhankelijk maken van grootte van de waarde

  x=(Par1 | Par2<<8) & 0x3ff; // 10-bit waarde

  str[0]=0;
  if(Par2&0x4) // 11e bit in de 16-bits combinatie van Par2 Par1 is het sign-teken.
    strcat(str,"-");
    
  strcat(str,int2str(x/10));
  strcat(str,".");
  x%=10;
  strcat(str,int2str(abs(x)));              
  strcat(str,")");

  return str;
  }
  
