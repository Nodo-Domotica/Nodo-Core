/********************* Aanpassingen t.o.v. versie 3.0.1  **********************************\
Nog te doen:

- Commando [Select]: Hiermee kan worden opgegeven dat events naar een specifieke Nodo moeten worden verzonden. Een niet geselecteerde Nodo kan ook niet zenden.
- Commando SendEvent herstellen. ??
- CRC een XOR met BUILD???
- UserPlugins werken nog niet
- [AlarmSet] toont na invoer een onjuist tijdstip.
- Portinput geeft parameter teveel weer
- Naast unit ook Home realiseren.
- als ethernet define op nul staat worden er toch pogingen gedaan om cookies te verzenden.
*****************************************************************************************

- Reactietijd van HTTP event naar RF output versneld.
- Melding ophalen IP adres via DHCP bij booten
- Issue 632: R487 mini variabelen worden niet meer doorgestuurd via rf
- Issue 633: Verwerken Queue vindt soms laat plaats
- Issue 635: Onnodig vaak verzoek DHCP voor IP adres
- Issue 636: AlarmSet
- Issue 639: FileExecute voert script niet altijd uit
- Issue 640: twee alarmsets, slechts 1 gaat af
- Issue 641: alarmset settings verdwijnen
- Issue 642: Alarmset instellen met status "off"
- Issue 643: in arduino-nodo: WaitFreeRF als user define
- Bug in verzenden IR signaal opgelost (aan einde van het signaal werden onterecht vervolgpulsen verzonden)

@@@@ Next:
- I2C communicatie tussen Nodo's.
- Aantal regels in Eventlist: Small=75, Mega=250
- EventlistErase wist nu alleen de bezette regels in de eventlist
- Issue 648: Voorkomen van I2C busconflicten bij uitlezen clock
- SendTo voor RF versneld.
- Legacy ondersteuning: Oude 32-bit userevents kunnen nog worden ontvangen.
- SendTo kiest automatisch kanaal I2C, RF of IR.
- SendTo doet Retry bij een checksum error;
- commando [Status EventlistCount] geeft nu ook aantal vrije plaatsen in de eventlist
- Limiet +100/-100 voor variabelen vervallen. LET OP: Vergelijken met [BreakOnVarEqu] vindt plaats met integer deel voor de komma.
- ClockAll, ClockSun...ClockSat zijn vervallen. Hiervoor in de plaats gekomen het event [Time hh:mm,day] 
- AlarmSet werkt nu ook met tijdweergave hh:mm en wildcards in de tijd.
- [SendBusy], [WaitBusy] en [Busy] zijn komen te vervallen.
- Commando delay vereenvoudigd. Er wordt altijd gequeued, mits de events ook voor deze unit bestemd zijn
- Eventlist default leeg.
- Veranderen van unit nummer zonder wissen eventlist.
- 32 unit nummers beschikbaar.
- Bij error tijdens uitvoer van een script wordt de verwerking gestopt. Tenzij de continue_on_error mode: [FileExecute <script>, ON]
- [Status All] wordt alleen nog ondersteund vanaf Terminal en HTTP. Niet meer via een [SendTo]
- Als een HTTP-request via de browser wordt gedaan dan wordt het resultaat aan de gebruiker getoond op de brouwser.
- [TimerSetSec] en [TimerSetMin] vervallen. Nu [TimerSet <seconden>] geworden.
- Variabelen worden niet meer als setting opgeslagen 


=== LET OP: Deze versie vereist een aanpassing aan de WebApp ===

- ClockSync nu zonder parameters. Nodo regelt synchronisatie met WebApp en Nodo's die online zijn.
- Toegevoegde commando's: [RawSignalList] en [RawSignalErase]. [RawSignalSave] is nu een setting. Default worden alleRawSignals die binnenkomen opgeslagen op SDCard.
- Device voor Dalles sensor aangepast aan nieuwe eventt-format. [Device <WiredOutPort>, <VariabeleNummer>]. Default voor DS18B20
- [Status FreeMem] toegevoegd. Geeft vrije werkgeheugen in bytes.
- [RawSignalErase] zonder parameters wist alle Rawsignals van SDCard.
- [FileErase] zonder parameters wist alle scriptfiles van SDCard.

\*****************************************************************************************/
