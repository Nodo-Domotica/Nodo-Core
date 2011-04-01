/**************************************************************************************************************************\

Release V1.2.0

Opgeloste issues:
- Issue 164:	HEX-code ontvangen IR en verwerken in Eventlist
- Issue 165:	KAKU Groep commando regressie bug.
- Issue 166:	WaitFreeRF gaat niet goed bij 'Series' en ontvangst van een SendKaku via IR.
- Issue 168:	ClockAll event in de erventlist geeft een match bij UserEvent
- Issue 170:	datum instellen met maand 0 wordt geaccepteerd
- Issue 175:	VariableVariable geeft foutmelding in Par2
- Issue 176:	Delay verstoort werking
- Issue 177:	revisie186 - delay probleem
- Issue 178:	spontante Variables events.
- Issue 172:	wildcard gebruik in UserEvent
- Issue 173:	GetLM335 => Opgelost met WiredRange.
- Issue 174:	Gebruik van variabele in UserEvent
- Issue 208: Aanpassing divert commando: Bij een Divert 0 wordt het te diverten commando ook zelf uitgevoerd.

Overige functionele aanpassingen:
- Home adres is komen te vervallen;
- SendNewKAKU: Dimniveau aanpassing. Opties voor Par2 zijn [On,Off,1..16] Dim niveaus dus ZONDER het woord 'dim'. Hoogste dimniveau is nu ook bereikbaar.
- Aanpassing pause bij herhaald sound 'ding-dong' en de 'Whoop' is nu een 'slow-whoop';
- '(WildCard All, All); (Sound 0, 0)' niet meer default in de eventlist;
- Aanpassing commando 'Delay': Kan worden uitgezet en voorbijkomende event worden in queue gezet voor latere verwerking;
- 'Home' adres is vervallen. Nu alleen Unit nummer toe te wijzen.
- Nesting error wordt nu als een error-event verzonden.
- dubbele regeleinden aan einde regel in EventList verwijderd;
- OUTPUT melding verschijnt bij uitvoer van een wired out.
- WaitfreeRF is gewijzigd. Par1=delay alvorens te zenden, Par2=luistertijd naar vrije ether. 
  Default 0,0. Bij wijzigenunit > 1 dan delay afhankelijk van unitnummer.
  All en series vervallen.
- Als unitnummer wordt gewijzigd dan wordt tevens de WaitFreeRF geactiveerd.
- Bij trace wordt geen lijn met sterretjes meer weergegeven.
- Versnellen verwerking RF signalen:
  * default 7x pulsreeks verzenden KAKU teruggebracht naar 5x.
  * Space stopbit timings aangepast.
  * delay in zendroutine na zenden pulsreeks verwijderd. 
  Hierdoor:
  * verzenden KAKU     : 4 schakelingen per seconde.
  * verzenden NewKAKU  : 2,5 schakelingen per seconde.
  * verzenden Nodo     : 2,7 comando's per seconde.
- Aantal herhalingen bij verzenden van een code instelbaar met TransmitSettings (Par2)
- Commando Sendstatus vervallen. Status commando aangepast;
- SERIES parameter t.b.v. WaitFreeRF vervallen.
- MMI aanpassing + 'Display' commando toegevoegd en commando 'Trace' vervallen. Trace in te stellen met Display commando
- Datum tijd notatie aangepast naar standaardnotatie: EEE YYYY-MM-DD HH:MM
- Als de 'Simulate' aan staat wordt dit weergegeven op Serial.
- 'TimerSet' commando gerenamed naar 'TimerSetMin':
- Timers allemaal op 0 zetten levert nu geen event meer op.
- 'variableClear' commando vervallen. wissen kan nu met 'variableSet', waarbij variabele=0 een wildcard is voor alle variabelen.
- 'TimerSetMin' en 'TimerSetSec': 0=wildcard voor alle timers.
- 'TimerReset': commando vervallen;
- 'TimerSetSec': nieuw commando. Als oude 'TimerSet', maar zet timers, maar dan in resolutie van seconden.
- 'Status' aanpassing: 'TimerSet' parameter is nu 'TimerSetMin' i.v.m. aanpassing commando 'TimerSet'.
- Queue binnenkomst en verwerking wordt nu anders weergegeven
- Queue wordt nu FIFO afgehandeld i.p.v. LIFO
- Bug opgelost: Als bij een eventlistwrite een actie wordt aangegeven die niet correct is verscheen er niet (altijd) een foutmelding. Nu wel.
- Aanpassing divert commando: Geen verzending door ether als de divert gelijk is aan huidige unitnummer. Niet nodig want alle units zijn uniek
- Status: Par1 extra optie 'Boot' om de welkomsttekst weer te geven.
- Weergave regels 'EventlistShow' aangepast aan nieuwe MMI
- "SYSTEM: Rawsignal=" melding veranderd in "RawSignal="
-  Timestamp" is "TimeStamp" geworden. tevens weergave van deze tag in de welkomsttekst.
- Geen weergave van het unitnummer als het signaal een HEX, KAKU of NewKAKU is.
- Commando 'SendBusy' toegevoegd;
- Commando 'WaitBusy' toegevoegd;
- snellere verwerking van een gevulde queue


Onder de motorkap:
- Timers nu in een int i.p.v. unsigned long en aanpassing aflopen timers => geheugenbesparing
- EventPart functie laten vervallen en vervangen door directe shift/and op events => geheugenbesparing
- EventType functie vervallen. Event type wordt nu onderdeel van het Event op de plaats waar het Home adres van de Nodo stond.
- Trace settings anders opgelost.
- intern: tegelijk afgelopen timers worden nu sneller achter elkaar afgehandeld.
- UserEvent draagt bij verzenden het unitnummer i.p.v. nul. Pas aan de ontvangstzijde wordt deze toebedeeld aan alle units.

\**************************************************************************************************************************/


