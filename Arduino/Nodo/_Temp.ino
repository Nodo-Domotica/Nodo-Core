/*

sendto 1;sound;sound;sound;sound;sound;sound;sound;sound;sound;
#define NODO_MAC 0x54, 0xa5, 0x8d, 0x17, 0xaf, 0x41 // Productie MAC Paul ???

Serial.print("*** debug: =");Serial.println(); //??? Debug
Serial.println("*** debug: ");//???

Verzenden blok data:

Methode=1: Par1 en Par2 delen in een regulier event gebruiken voor overdracht
=============================================================================
* in delen van 16-bit Par1 en Pr2 bit van een 32-bit reguliere eventcode.
* opvangen in de queue.
* CMD_BLOCK niet weergeven voor gebruiker.
+ eenvoudig implementeerbaar. Code al aanwezig
- veel overhead in transport
? Handshaking

0: CMD_BLOCK,Par1=toepassing,0
1: data-1
2: data-2
3: data-3
n: data-n 
x: CMD_BLOCK, sluiting, checksum.

Methode=2: Oude Sendto methode (afwijkens signaal)
=============================================================================














*/


