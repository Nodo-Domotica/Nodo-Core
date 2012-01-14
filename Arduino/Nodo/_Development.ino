/**************************************************************************************************************************\


Known Errors / ToDo:

- Found in r310: Date=2012-01-14, Time=20:31, Input=Wired, Unit=1, Event=(WiredIn 9,On) event ?? na verzenden met WiredAnalogSend.
- Check / aanpassing werking WiredSmittTrigger
- Check / aanpassing werking WiredTreshold
- Commaando toegevoegd "WiredAnalogSend". Leest analoge waarde van poort uit en verstuurt event met analoge waarde, getransponeert volgens ijkwaarden.
- Found in r310: Na een RaiseError() na invoer niet bestaand command loopt de Arduino vast. 
- Found in r306: als ethernet niet aangesloten dan start de nodo (soms) niet op.
- Found in r306: Nodo due compatibel maken n.a.v. omnummeren CMD_...
- Sendbusy en Waitbusy testen of mmi en oppikken commando nog goed werken. Queue testen
- Wired poorten testen incl. Trigger, Threshold.
- Found in r306: Na volledig volschrijven van de eventlist blijft Nodo hangen in continue uitvoeren van het reboot event.
- Found in r306: Na een RaiseError vanuit een TelNet sessie werkt invoer commando eerst daarop volgende keer niet. Eerste commando wordt overgeslagen.
- Gevoelige commando's verhuizen van ExecuteCommand naar ExecuteLine
- Commando "OutputHTTP". Hiermee kan worden ingesteld of ieder event ook word verstuurd als een HTTP-request. Het url adres van de server kan worden worden ingesteld met commando "URL <web address>"
- Commando "OutputPachube"

Minor:
- Found in r306: Status WiredAnalog 22 is mogelijk

Ideeën:

Aanpassingen vanaf r310:
- Commaando toegevoegd "WiredAnalogCalibrate <poort> <High|Low> <ijkwaarde>"
- Toevoeging commando "URL <line>", hiermee kan de URL van de server worden ingesteld waar de events (via HTTP-Poort 80) naar toegezonden moeten worden. (max. 40 tekens)

Aanpassingen vanaf r306:
- Toevoeging commando "HTTPRequest <line>"
- Opgelost: status weergaven van OutputEG wordt niet goed wergegeven
- Verwijderen commando "IPNodo" IP adres. Gaat nu via DHCP.
- Bug: Found in r306: Events naar IP adressen worden niet correct verzonden als tegelijkertijd een TelNet sessie actief is.
- Funktioneel: Echo tekens bij TelNet sessie verwijderd. Dit regelen de meeste Terminal programma's
- Technisch: Telnet, Serial en EventGhost hebben ieder een eigen inputbuffer
- Technisch: Verzenden van een EventGhost Event naar IP nu voorzien van retry bij fout in verzending.
- Nieuw commando "SendEG <On|Off> , <AutoSaveIP On|Off>"

Aanpassingen vanaf r305:
- Bug: Terugsturen events naar bekende IP adressen werkt niet goed. (self-learning)
- Bug: EventlistWrite werkt niet
- Bug: Hex-events worden niet geaccepteerd.
- Funktioneel: prompt teken weggehaald uit terminal venster. lasig altijd weer te geven en voegt niets toe.
- Funktioneel: Terminal On setting niet bewaard na reboot.
- Technisch: Invoer commando via telnet werkt niet lekker;


Release V3.0.0: Functionele aanpassingen ten opzichte van de 1.2.1 release
- Commaando toegevoegd "WiredAnalogCalibrate <poort> <High|Low> <ijkwaarde>"
- Ethernet intergratie. Events van EventGhost (PC, Android) ontvangen en verzenden over IP;
- Toevoeging commando "URL <line>", hiermee kan de URL van de server worden ingesteld waar de events (via HTTP-Poort 80) naar toegezonden moeten worden. (max. 40 tekens)
- Nieuw commando "OutputEG <On|Off> , <SaveIP Save|On|Off>"
- Bij opstarten de melding "Booting..." omdat wachten op IP adres van de router de eerste keer even tijd in beslag kan nemen.
- Indien SDCard geplaatst, dan logging naar Log.txt.
- UserPlugin maakt mogelijk om gebruiker zelf code aan de Nodo code toe te voegen.
- Aantal timers verhoogd van 15 naar 32
- Aantal gebruikersvariabelen verhoogd van 15 naar 32
- 8 digitale wired poorten i.p.v. 4
- 8 analoge wired poorten i.p.v. 4
- Eventlist uitgebreid van 120 posities naar 256
- queue voor opvangen events tijdens delay van 15 uitgebreid naar 32 events.
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
- Commando "WiredRange" vervallen. Overbodig geworden n.a.v. calibratie/ijking funktionaliteit.
- Event aangepast "WiredAnalog". Geeft nu gecalibreerde waarde weer metdecimalen achter de komma
- Verzenden van IR staat default op Off na een reset.
Onder de motorkap:
- Verwerken van seriele gegevens volledig herschreven
- omnummeren van tabel met events,commando en waarden om plaats te maken voor uitbreiding commandoset. LET OP: niet meer compatibel met de Uno 1.2.1 en lagere versies!

\**************************************************************************************************************************/


