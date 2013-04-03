/********************* Aanpassingen t.o.v. versie 3.0.1  **********************************\
Nog te doen:

- CRC een XOR met BUILD???
- SendTo: Na een retry kans op twee maal uitvoeren commando's aan slave zijde???
- NewNodo blijft soms nog onterecht worden verzonden?
- Hoe een variabele van een Nodo oversturen naar een variabele in een andere Nodo?
- Opnemen en weer verzenden van een rawsignal testen.
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
- Issue 642: Alarmset instellen met status "off"
- Issue 641: alarmset settings verdwijnen

R513
- Escape bij loop: Binnen vijf seconden na een reboot kan een commando naar de Nodo worden gestuurd om te resetten. Uitvoer vindt plaats voor uitvoer van het boot event.
- SendTo commando aanpassing. Onder de motorkap aangepast EN SendTo blijft actief totdat SendTo Off wordt ingegeven
- Status alleen nog maar uitvraagbaar via Terminal of Serial.

R517
- SendTo uitgebreid met parameter All.
- Issue 653: Break
- Issue 675: Eventlist bevat ghost entries
- Issue 474: Slave reboot en reset
- Issue 674: NodoList
- Issue 577: SendTo naar unit met Output RF,Off
- Issue 672: Sendto geeft erg vaak timeout
- UserPlugin verwijderd. Hiervoor zijn de devices in de plaats gekomen.
- Commando [Unit] gerenamed naar [UnitSet]. Dit is consequenter.

R518
- Issue 675: Eventlist bevat ghost entries
- Issue 687: SendTo is van de wijs te brengen!

R519
LET OP: Tevens WebApp aanpassing, echter deze build toont in de WebApp geen nog Eventlist die direct weer te programmeren is. (Actie: Martin)
- Issue 692: Toevoegen van DEVICE_PERIODIC call.
- Status All via HTTP geeft leeg resultaat
- Toevoeging entries voor nieuwe devices 19 & 20 (Code nog niet beschikbaar: Actie Martinus)
- Aanpassingen t.b.v. WebApp (resultaten terug in Bodytext)
- DEVICE_ONCE_A_SECOND funktie toegevoegd aan devices.
- Issue 688: File uitvoeren werkt soms niet
- weergave alarmen werkt niet altijd correct.
- EventlistShow weergave aangepast. Ëventlist <regel> is nu EventlistWrite <regel> geworden.
- Tweede parameter in commando [UnitSet] is het Nodo Home adres [1..7]. Nodos met verschillende Home adressen kunnen geen Nodo-events uitwisselen.

R522
- Bug gevonden in SendTo voor I2C
- LET OP: Commando [HomeSet] is nog niet operationeel
- FileExecute leverde aan einde bestand een Abort message.
- Automatische detectie ethernetkaart er weer uitgehaald. (MOSI/MISO detectie niet betrouwbaar)

R524
- Unitnummer van Mega naar WebApp niet correct als event afkomstig van Small
- [Lock] commando weer beschikbaar
- [TimerRandom] opgegven tijd nu evenals andere timers in seconden in plaats van minuten
- Kleine bug uit command regel parser verwijderd. Deze bug leverde soms onterecht een par2=0 op als er teveel spaties in commando regel zitten.
- [HomeSet] commando operationeel gemaakt.
- [WaitEvent <unit>,<command>] weer hersteld n.a.v. 96-bit migratie
- [SendTo] werkt nu ook op small en vanuit de eventlist. LET OP: geen handshaking bij gebruik vanuit eventlist. Snelle variant. Events worden direct op de bestemming afgevuurd zonder terugkoppeling.
- [SendTo] Zodra vanaf de master het eerste event wordt verstuurd, zullen alle andere units worden geblokkeerd voor vezrenden via IR/RF (??? nog testen in landschap)

R525
- [SendTo <unit>, All] werkt nu op op de Small en vanuit de eventlist.

R526
- Commando [Stop] toegevoegd. Stopt de verwerking van een eventlist voor alle iteraties.
- [SendTo] Zolang de All optie actief is, wachten alle andere Nodos totdat de band weer vrij is.

R527
- Nieuw commando [VariablePulseCount <variabele>] voor vullen van een variabele met het aantal pulsen. [Status PulseCount] vervallen.
- Nieuw commando [VariablePulseTime <variabele>] voor vullen van een variabele met tijdsduur laatste puls in mSec. [Status PulseTime] vervallen.
- Nieuw commando [VariableLastEvent <BestemmingVariabele>, <BronVariable>]. Kan i.c.m. een wildcard worden gebruikt om de waarde van een event op te slaan in een variabele. Kan waarde halen uit [WiredAnalog], [Variable], [VariableSet].

R528
- Issue 669: override MAC address
- PortInput en PortOutput geven incorrecte MMI
- Nieuw commando [RawsignalReceive <on|off>]. Ontvangst van Rawsignals aan/uitschakelen. Default aan.
- BodyText naar HTTP-Sessie verloopt niet meer door tussenkomst van file op SDCard. (Verbeteren performance en voorkomen slijtage SDCard bij intensief gebruik).
- Home adres van de Nodo verwerkt in MAC adres.


\*****************************************************************************************/
