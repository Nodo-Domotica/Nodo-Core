/********************* Aanpassingen t.o.v. versie 3.0.1  **********************************\
Nog te doen:

- Testen [VariableEvent]
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
- Nieuw commando [VariableEvent <BestemmingVariabele>, <BronVariable>]. Kan i.c.m. een wildcard worden gebruikt om de waarde van een event op te slaan in een variabele. Kan waarde halen uit een [WiredAnalog] of [Variable] event.
- Issue 669: override MAC address
- PortInput en PortOutput geven incorrecte MMI
- Nieuw commando [RawsignalReceive <on|off>]. Ontvangst van Rawsignals aan/uitschakelen. Default aan.
- BodyText naar HTTP-Sessie verloopt niet meer door tussenkomst van file op SDCard. (Verbeteren performance en voorkomen slijtage SDCard bij intensief gebruik).
- Home adres van de Nodo verwerkt in MAC adres.

R528 (Ter beschikking gesteld aan gebruikers als Beta 3.4.9)
- RawSignal aanpassingen: Ander format op SDCard. Tevens 0x voor hexwaarden bij RawSignalList. Oude files voor RawSignal zijn NIET compatibel!
- Technisch: Toevoeging elementen Delay en Multiply aan de struct RawSignal
- Technisch: Workaround voor delayMicroSeconds(0); bug in Arduino compiler 1.0.1 en later.
- Technisch: Reductie I/O-bewerkingen bij opslaan van files op SDCard ontvangen via HTTP. Verbetering performance.
- Commando [ClockSimulate] vervallen.
- Commando [ClockSetDate] nu volgens formaat: DD-MM-YYYY. Commando [ClockSetYear] komen te vervallen.
- Commando [ClockSetDOW] komen te vervallen. Dag van de week wordt nu automatisch bereken.
- Commando [ClockSetTime] nu volgens formaat: HH:MM. (Seconden worden op 00 gezet bij instellen van de klok.
- Commando [ClockSync] zet klokken van alle bekende Nodos gelijk. Eerst wordt tijd opgehaald van de WebApp, daarna slaves gelijk gezet.
- Issue 704: Defaults
- Issue 705: Status HomeSet
- Issue 706: Ontvangen NewKaku display onjuist
- Toevoeging DEVICE_18: RawSignalAnalyze
- Toevoeging DEVICE_19: Innovations ID-12 RFID Tag reader
- Toevoeging DEVICE_21: DFRobot LCD I2C/TWI 1602 Display

==========================================================================================
R530
- Issue 660: ClockAll met voorloopnul geeft bizar resultaat
- Issue 708: BreakOnEarlier en Later: syntax error
- Issue 710: ClockSetTime par2
- Issue 711: SimulateDay wordt geaccepteerd
- Issue 712: BreakOnLater werkt niet
- Issue 713: FileExecute vanuit de Eventlist werkt niet altijd
- Issue 715: in arduino-nodo: PortInput invoer niet correct verwerkt
- Aanpassing: Bij uitvoer eventlist werd nesting diepte niet altijd correct weergegeven.
- Verwijderd: Commando [Test] verwijderd. Deze was in gebruik voor testdoeleinden tijdens ontwikkeling
- Toegevoegd: Commando [TimerSetVariable <timer>, <variable>]. Vult een timer met de inhoud van een variabele.
- Toegevoegd: Device-22 HC-SR04 Distance sensor (SWACDE-22-V10) == Integratie in Nodo-code nog testen ==
\*****************************************************************************************/
