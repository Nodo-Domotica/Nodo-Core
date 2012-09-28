
/**************************************************************************************************************************\

Aanpassingen t.o.v. laatste build:
- Issue 535: Status via een HTTP-request leidt niet tot een event.
- Issue 505: Overvloedige error message.
- Issue 443: Break on tijd.
- Issue 486: Extra entry point voor UserPlugin

Aanpassingen t.o.v. Final Beta Build 436:
- Issue 535: Status via een HTTP-request leidt niet tot een event.
- Issue 505: Overvloedige error message.
- Issue 443: Break on tijd.
- Issue 486: Extra entry point voor UserPlugin

Known Errors / ToDo:
- SendBusy op de Master zet de slaves tijdelijk op onbereikbaar. Hierdoor werkt SendTo niet
- In geval van onbereikbare Nodo-Small: Hoe resetten?
- Status All uitvraag bij een Small levert: [Input=File, Unit=15, Event=Debug] Geen geldig event
- Vreemde weergave events: Internal=System


\**************************************************************************************************************************/

