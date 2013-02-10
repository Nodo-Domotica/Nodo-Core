/********************* Aanpassingen t.o.v. versie 3.0.1  **********************************\
Nog te doen:
- aanpassen unitnmmer, reset en reboot via SendTo leidt tot timeout error
- Status all uitvraag op small: Past niet in queue, geen alternatief zonder queue
- Commando [Select]: Hiermee kan worden opgegeven dat events naar een specifieke Nodo moeten worden verzonden. Een niet geselecteerde Nodo kan ook niet zenden.
- RawSignals geheel aanpassen
- Commando SendEvent herstellen. ??
- ClockSync: Par2 kan de poort worden opgegeven: IR, RF, I2C. Zonder opgave van poort wordt RF gebruikt.
- testen: werkt ClockSync ook voor unit-0 dus alle units?
- CRC een XOR met BUILD???
- UserEvents compatibel oude Nodo versies 32-bit?
- UserPlugin

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

@@@@ Next:
- I2C communicatie tussen Nodo's.
- Issue 648: Voorkomen van I2C busconflicten bij uitlezen clock
- SendTo voor RF versneld.
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
\*****************************************************************************************/
