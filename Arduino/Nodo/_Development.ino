
/**************************************************************************************************************************\

Aanpassingen t.o.v. laatste build:
- [HWConfig] toegevoegd als optie voor commando [Status]
- HTTP-Poort 6636 ipv 8080
- Interne verwerking van variabelen en analoge waarden nu onder de motorkap geregeld et variabeletype float.
- [AnalyseSettings] niet meer met commando instelbaar, alleen compileerbaar
- DayLight events niet meer beschikbaar voor een Small-Nodo, alleen nog op de Mega-Nodo.
- Voor Nodo-Small geldt: Gebruikers variabelen niet meer naar EEPROM te schrijven. EEPROM is alleen nog bedoeld voor settings. Commando [VariableSave] hiermee vervallen.

Aanpassingen t.o.v. Final Beta Build 436:
- DayLight events niet meer beschikbaar voor een Small-Nodo, alleen nog op de Mega-Nodo.
- Voor Nodo-Small geldt: Gebruikers variabelen niet meer naar EEPROM te schrijven. EEPROM is alleen nog bedoeld voor settings. Commando [VariableSave] hiermee vervallen.
- [AnalyseSettings] niet meer met commando instelbaar, alleen compileerbaar
- HTTP-Poort 6636 ipv 8080
- [HWConfig] toegevoegd als optie voor commando [Status]
- Welkomsttekst wordt niet meer op Serial weergegeven als Telnet verbinding tot stand komt.
- queue op de Mega teruggeplaatst naar RAM, dit i.v.m. erg hoge mutatiegraad van de SDCard en mogelijk defect van SDCard.
- Chat teken aangepast naar #
- Div. aanpassingen TelNet en HTTP communicatie
- Issue 557: OutputIP heeft geen invloed op verzenden naar WebApp
- Issue 558: Te lange Filename wordt zonder waarschuwing geweigerd
- Issue 559: Queueing error
- Issue 504: Nodo Busy status in WebApp
- Issue 564: echo voor TelNet default op Off.
- Issue 563: Password verkeerd ingesteld in build 446
- Alleen DNS-Resolving van de HTTPHost tijdens opstarten.
- Default geen wachtwoord ingesteld op de Nodo.
- Toevoeging aan Welkomsttekst: "Host=www.nodo-domotica.nl, HostIP=141.138.136.245"
- Boot event wordt na opstarten niet meer getoont, tenzij er input op serial is ontvangen.
- Issue 549: Output commando's bundelen ==>> LET OP: Hiervoor aanpassing van de WebApp nodig.
- Issue 548: Onbekend UserEvent laat Small stoppen met scannen.
- Issue 541: WaitFreeRF nodig? Default "on"?
- Issue 542: Versie van een Small niet via RF te achterhalen
- Issue 534: Meer defensieve verwerking van de RTC data via de I2C bus
- Aanpassing formules voor pulsen.
- PullUp voor IR_RX_DATA lijn 
- Issue 543: Small: Tellen van pulsen werkt niet correct
- Issue 538: Logging naar LOG.DAT is voor veel genruikers onnodig omdat de WebApp hier ook in voorziet
- Issue 539: EventGhost APOP protocol verwijderen uit de Nodo
- Issue 537: in arduino-nodo: Headers niet consequent bij verzenden/ontvangen
- Issue 535: Status via een HTTP-request leidt niet tot een event.
- Issue 505: Overvloedige error message.
- Issue 443: Break on tijd.
- Issue 486: Extra entry point voor UserPlugin

Known Errors / ToDo:
- dubbel prompt teken.
- SendBusy op de Master zet de slaves tijdelijk op onbereikbaar. Hierdoor werkt SendTo niet
- In geval van onbereikbare Nodo-Small: Hoe resetten?
- Status All uitvraag bij een Small levert: [Input=File, Unit=15, Event=Debug] Geen geldig event
- Vreemde weergave events: Internal=System


\**************************************************************************************************************************/

