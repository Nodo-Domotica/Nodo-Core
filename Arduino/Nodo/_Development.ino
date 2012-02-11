/**************************************************************************************************************************\

Aanpassingen:
- Alle invoerregels in een Terminalsessie worden nu ook weergegeven. 
- Aanpassing commando "FileWrite <filename>". Schrijft alle via TelNet of Serial binnenkomende regels naar bestand <filename> totdat FileWrite weerals tekst voorbij komt of er 60sec. voorbij is
- bij ophalen file een <br> / CRLF teveel in de bodytekst. Verwijderd.
- Alle invoerregels in een Terminalsessie worden nu ook in de logfile op SDCard vastgelegd.

Known Errors / ToDo:

- Sendbusy en Waitbusy testen of mmi en oppikken commando nog goed werken. Queue testen
- Found in r306: Status WiredAnalog groter dan toegestane poort
- Status naar file op SDCard
- Geeft FileList een bestand te weinig aan???
- In de output naar terminal wordt een komma gebruikt als scheiding tussen de delen. Niet consequent, zou puntkomma moeten zijn.
- capture/upload mogelijkheid om snel een bestand aan te kunnen maken op sd.


Release V3.0.0: Functionele aanpassingen ten opzichte van de 1.2.1 release
- EventListshow, eventListWrite, eventListErase hebben nu als parameter-1 de regel waar het betrekking op heeft (regel, ALL, 0=All)
- Toevoeging commando "IPSettings".
- Commaando toegevoegd "WiredCalibrate <poort> <High|Low> <ijkwaarde>"
- Nodo ontvangt events via http. Voorbeeld: http://192.168.1.110/?event=sound&passwoord=Nodo&id=00000000
- Ethernet intergratie. Events van EventGhost (PC, Android) ontvangen en verzenden over IP;
- Toevoeging commando "URL <line>", hiermee kan de URL van de server worden ingesteld waar de events (via HTTP-Poort 80) naar toegezonden moeten worden. (max. 40 tekens)
- Nieuw commando "OutputIP <HTTP|EventGhost|Off> , <SaveIP Save|On|Off>"
- Bij opstarten de melding "Booting..." omdat wachten op IP adres van de router de eerste keer even tijd in beslag kan nemen.
- Indien SDCard geplaatst, dan logging naar Log.txt.
- UserPlugin maakt mogelijk om gebruiker zelf code aan de Nodo code toe te voegen.
- 8 digitale wired poorten i.p.v. 4
- 8 analoge wired poorten i.p.v. 4
- Aanpassing "WiredSmittTrigger": invoer analoge decimale waarde. 
- Aanpassing "WiredTreshold": invoer analoge decimale waarde.
- Eventlist uitgebreid van 120 posities naar 256
- queue voor opvangen events tijdens delay van 15 uitgebreid naar 32 events.
- Welkomsttekst uitgebreid met de IP-settings
- Welkomsttekst uitgebreid met melding logging naar SDCard.
- Toevoeging commando "Password"
- Toevoeging commando "Reboot"
- Ccommando "Status" geeft in als resultaat in de tag "output=Status"
- commando "Port" toegevoed voor instellen van de IP-poort.
- Commando "Display" Vervallen.
- Commando hoeft niet meer te worden afgesloten met een puntkomma. Puntkomma wordt alleen gebruikt om meerdere commandos per regel te scheiden.
- Toevoeging commando "VariableSave <variabele>", slaat variabelen op zodat deze na een herstart weer worden geladen. 0=alle, Opslaan gebeurt NIET meer automatisch.
- Logging naar SDCard naar bestand log.dat. 
- Commando "RawSignalGet" en "RawSignlPut" vervallen;
- Toevoeging commando "RawSignalSave <key>". Slaat pulsenreeks van het eerstvolgende ontvangen signaal op op SDCard onder opgegeven nummer
- Toevoeging commando "RawSignalSend <key>". Verzend een eerder onder <key> opgeslagen pulsenreeks. Als <key> = 0, dan wordt huidige inhoud verzonden
- "SendVarUserEvent" renamed naar "VariableSendUserEvent"
- nieuw commando: "VariableUserEvent" genereert een userevent op basis van de inhoud van twee variabelen.
- Commando "TransmitSettings" vervallen. Vervangen door "OutputIR", "OutputRF"
- Commando "Simulate" vervallen. Kan worden opgelost met de nieuwe commandos "Output...".
- Aanpassing weergave van datum/tijd. De dag wordt na NA de datum weergegeven ipv VOOR (ivm kunnen sorteren logfile).
- Errors worden nu weergeven met een tekstuele toelichting wat de fout veroorzaakte: "Error=<tekst>". 
- "Timestamp=" wordt nu iets anders weergegeven als "Date=yyyy-mm-dd, Time=hh:mm".
- Variabelen worden na wijzigen niet meer automatisch opgeslagen in het EEPROM geheugen. Opslaan kan nu met commando "VariableSave"
- Toevoeging commando "Host <hostadres>". Vul in als "Host www.mijnhost.nl/pad/mijnscript.php"
- Tag 'Direction' vervangen door Input, Output, Internal
- Commando "SendEvent <poort>" toegevoegd. Vervangt oude SendSignal. Stuurt laatst ontvangen event door. Par1 bevat de poort( EventGhost, IR, RF, HTTP, All)
- Onbekende hex-events worden mogelijk door andere waarde weergegeven a.g.v. filtering aan Nodo gelijke events. ??? wenselijk/noodzakelijk?
- Een EventlistWrite commando met bijhehorende event en actie moeten zich binnen 1 regel bevinden die wordt afgesloten met een \n
- Verzenden naar Serial vindt pas plaats als er door ontvangst van een teken gecontroleerd is dat seriele verbinding nodig is;
- Commando "VariableSetWiredAnalog" vervallen. Past niet meer bij 10-bit berwerking en calibratie/ijking
- Commando "WiredRange" vervallen. Overbodig geworden n.a.v. calibratie/ijking funktionaliteit.
- Event aangepast "WiredAnalog". Geeft nu gecalibreerde waarde weer metdecimalen achter de komma
- Verzenden van IR staat default op Off na een reset.
- Sound: als Par1 groter dan 8, dan tijdsduur=Par2*100 milliseconde, toonhoogte=Par2*100 Hz
- Aanpassing weergave van EventList zodat deze direct weer gebruikt kan worden om weg te schrijven
- Commando's/Events worden niet meer tussen "(" en ")" haken weergegeven.
- Commando "EventGhostServer" toegevoegd. Hiermee kun je het IP adres van een EG-server opgeven waar events naar toe moeten worden verstuurd. 'Auto,On' slaat laatste client op als een server.
- Commando "Debug" toegevoegd. Geeft extra informatie over de verwerking van events door de Nodo. Was vroeger de parameter "Trace" van het vervallen commando "Display"
- Commando "VariableUserEvent" vervallen. Niet meer compatibel met analoge waarden in variabelen,
- Commando "VariableSendUserEvent" vervallen. Niet meer compatibel met analoge waarden in variabelen,
- VariableSave nu ook per individuele variabele mogelijk. 0=alle;
- Alle commando's die werken met gebruikersvariabelen zijn geschikt gemaakt voor bereik van +100..-100
- Toevoeging commando "WiredAnalogVariable <wiredpoort>,<variabele>". Kent de meetwaarde van een WiredIn toe aan een variabele
- (New)KAKU signaal geeft een unit-0. (NIET KUNNEN TESTEN!)
- LET OP: MAC adres even naar je egen vaste IP adres wijzigen
- Nieuw commando "EventlistFile <filename>". Slaat de eventlist op in een bestand op SDCard. Naam max. acht posities ZONDER EXTENTIE.
- Nieuw commando "FileShow <filename>". Geeft de inhoud van een file weer.
- Nieuw commando "FileErase <filename>". Verwijderd een file van SDCard.
- Nieuw commando "FileExecute <filename>". Voert alle regels in een file uit.
- Nieuw commando "FileWrite <filename>". Schrijft alle via TelNet of Serial binnenkomende regels naar bestand <filename> totdat FileWrite weerals tekst voorbij komt of er 60sec. voorbij is
- HTTP-request dat de Nodo ontvangt kan worden gebruikt om een file terug te laten sturen. ?password=<password>&id=<id>&file=<filename>
- Commando "NodoIP <ip_address>" toegevoegd. Hiermee kan handmatig aan de Nodo een IP adres worden toegekend. Als adres 0.0.0.0, dan wordt IP adres verkregen via DHCP.
- Commando "Divert" verwijderd. wordt mogelijk vervangen door "Send <unit>,<poort>" TIJDELIJK T.B.V. TEST OP BRUIKBAARHEID, NOG IN ONTWIKKELING.???


Onder de motorkap:
- Verwerken van seriele gegevens volledig herschreven
- omnummeren van tabel met events,commando en waarden om plaats te maken voor uitbreiding commandoset. LET OP: niet meer compatibel met de Uno 1.2.1 en lagere versies!

\**************************************************************************************************************************/


