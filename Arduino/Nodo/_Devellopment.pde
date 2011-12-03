/**************************************************************************************************************************\

Builds:

Ideeën / Issues:
- rawsignal get en put herschreven voor nieuwe input verwerking;
- vreemde weergave: Date=2011-11-09, Time=23:23, System=System, Unit=2, Event=(Error Unknown command,)
- Nodo due compatibel maken n.a.v. omnummeren CMD_...
- onduidelijk of transmitsettings nog een Par2 voor instellen van de herhalen: documnteren!
- temperatuur uitlezen LM335

Opgeloste issues:
- Issue xxx:
- Sendbusy en Waitbusy testen of mmi en oppikken commando nog goed werken


Release V3.0.0: Functionele aanpassingen ten opzichte van de 1.2.1 release
- Ethernet intergratie. Events van EventGhost (PC, Android, IPad) ontvangen en verzenden over IP;
- Indien SDCard geplaatst, dan logging naar Log.txt.
- UserPlugin maakt mogelijk om gebruiker zelf code aan de Nodo code toe te voegen.
- Aantal timers verhoogd van 15 naar 32
- Aantal gebruikersvariabelen verhoogd van 15 naar 32
- 8 digitale wired poorten i.p.v. 4
- 8 analoge wired poorten i.p.v. 4
- Eventlist uitgebreid van 120 posities naar 256
- queue voor opvangen events tijdens delay van 15 uitgebreid naar 32 events.
- Toevoeging commando "Port" voor instellen TCP/IP poort
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
- Toevoeging commando "RawsignalSave". Slaat pulsenreeks van het eerstvolgende ntvangen signaal op op SDCard onder opgegeven nummer;
- "SendVarUserEvent" renamed naar "VariableSendUserEvent"
- nieuw commando: "VariableUserEvent" genereert een userevent op basis van de inhoud van twee variabelen.
- Commando "TransmitSettings" vervallen. Vervangen door "TransmitIR", "TransmitRF"
- Commando "Simulate" vervallen. Kan worden opgelost met de nieuwe commandos "TransmitIR" en "TransmitRF".
- Aanpassing weergave van datum/tijd. De dag wordt na NA de datum weergegeven ipv VOOR (ivm kunnen sorteren logfile).
- Errors worden nu weergeven met een tekstuele toelichting wat de fout veroorzaakte.
- "Timestamp=" wordt nu iets anders weergegeven als "Date=yyyy-mm-dd, Time=hh:mm".
- Variabelen worden na wijzigen niet meer automatisch opgeslagen in het EEPROM geheugen. Opslaan kan nu met commando "VariableSave"
- Tag Direction vervangen door Input, Output, Internal
- Een EventlistWrite commando met bijhehorende event en actie moeten zich binnen 1 regel bevinden die wordt afgesloten met een \n
- Verzenden naar Serial vindt pas plaats als er door ontvangst van een teken gecontroleerd is dat seriele verbinding nodig is;


Onder de motorkap:
- Verwerken van seriele gegevens volledig herschreven
- omnummeren van tabel met events,commando en waarden om plaats te maken voor uitbreiding commandoset. LET OP: niet meer compatibel met de Uno 1.2.1 en lagere versies!

\**************************************************************************************************************************/


