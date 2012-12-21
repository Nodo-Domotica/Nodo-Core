
/*
NODO GOES I2C ???

I2C poort event uitwisseling.

* Alle Nodo's kunnen master zijn. Dus iedere Nodo kan een event op de bus zetten
* conflicten: hoe detecteren en mee omgaan
* Wat over de lijn?
* Unitnummer verwerkt in het adres
* Aantal units uitbreiden naar 32, dan kan dit ook nog in de 32-bit versie worden opgenomen. type terugbrengen naar 3 bits.
* Bij commando's betreft het unitnummer de bestemming, bij een event de bron.

I2C=data uitwiseling: 8 byte (64-bit) blok (exc. 7bits adres)
Type
Unit nummer
Command
CRC
Par1
Par2
Par3
Par4

NODO GOES 64-BIT????

            ---6---------5---------4---------3---------2---------1----------
            3210987654321098765432109876543210987654321098765432109876543210
IST:                                        TTTTUUUUCCCCCCCC1111111122222222

T = type  (4-bit) 
U = unit  (4-bit)
C = Cmd   (8-bit)
1 = Par-1 (8-bit)
2 = Par-2 (8-bit)

            ---6---------5---------4---------3---------2---------1----------
            3210987654321098765432109876543210987654321098765432109876543210
SOLL:       

== HEADER ==
U = Unit     (8-bit) 
C = Cmd      (8-bit) Command of Event
B = Bytes    (8-bit) Aantal unsigned long data blokken signaalgroote
V = Checksum (8-bit)

== DATA ==
1 = Par-1    (8-bit)
2 = Par-2    (8-bit)
3 = Par-3    (8-bit)
3 = Par-4    (8-bit)

- hoe omgaan in de eventlist?
- backwards compatibel met vorige 32-bit nodo's
- Vaste bloklengte of variabele bloklengte. Minimale blokgrootte 4 bytes = 1 datablok. Maximale datablokken?
- Geen herhalingen, eenmaal verzenden.
- Vastleggen in de Rasignal buffer of niet? Header opvangen met Rawsignal, de rest anders?
- 64-bit alleen voor Mega Nodo's?
- Eventlist laten vervallen in EEPROM en volledig runnen vanaf SDCard?



Serial.print("*** debug: =");Serial.println(); //??? Debug
Serial.println("*** debug: ");//???


*/
