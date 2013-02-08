/********************* Aanpassingen t.o.v. versie 3.0.1  **********************************\
@@@@@ Todo:
- eventlist nummering bij printen event niet correct (geleende variabele uit struct)
- aanpassen unitnmmer leidt tot timeout error
- Werkt event NewNodo nog wel?
- Status all uitvraag op small:
  * past niet in queue, geen alternatief zonder queue
  * geeft vreemde resultate, worden opgevangen instructies ook door master uitgevoerd? (Display only flag toevoegen)
  * 
  
- dubbele error als er tijdens de sendto een fout is opgetreden.
- RawSignals nog in Par-2 (32-bit) opnemen
- Commando SendEvent herstellen.
- hoe worden rawsignal/hex events in de struct opgeslagen? nog uitwerken.
- ClockSync: Par2 kan de poort worden opgegeven: IR, RF, I2C. Zonder opgave van poort wordt RF gebruikt.
- bij verzenden moet unitnummer van de bestemming worden weergeven
- testen: werkt ClockSync ook voor unit-0 dus alle units?
- testen: RawSignal opslaan, ontvangen en verzenden. Dit i.v.m. verwijderen RawSignal.Type
- in flags: show event only versus verwerken event.
- CRC een XOR met BUILD
- herhalingen rf onderdrukken

*****************************************************************************************

- Commando's die via een [SendTo] worden ontvangen op een mega worden nu ook weergegeven
- Reactietijd an HTTP event naar RF output versneld.
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
- Issue 648: Voorkomen van I2C busconflicten bij uitlezen clock
- ClockAll, ClockSun...ClockSat zijn vervallen. Hiervoor in de plaats gekomen het event [Time] 
- AlarmSet werkt nu ook met tijdweergave hh:mm en wildcards in de tijd.
- [SendBusy], [WaitBusy] en [Busy] zijn komen te vervallen.
- in code instelbare minimale tijd tussen verzenden events.
- Commando delay vereenvoudigd. Er wordt altijd gequeued, mits de events ook voor deze unit bestemd zijn
- Commando [Select] toegevoegd: Hiermee kan worden opgegeven dat events naar een specifieke Nodo moeten worden verzonden. Een niet geselecteerde Nodo kan ook niet zenden.
- Eventlist default leeg.
- Veranderen van unit nummer zonder wissen eventlist.
- [SendTo] Hoeft niet voor iedere regel. Eenmalig inschakelen en eenmalig uitschakelen. Werkt nu ook op een Small.


\*****************************************************************************************/
