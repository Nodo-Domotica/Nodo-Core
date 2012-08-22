/*



Event   = Alarm <nummer>
Command = AlarmSet <nummer>, <hour>, <minute>
                    0,1..4   00..23  00..59
                    3        5       6


FileWrite Test

! *****************************************************
! Testje om te kijken of het SendTo commando goed werkt
! Master = Unit 1
! Slave  = Unit 5
! Als alles goed is gegaan zal het verzenden van een UserEvent 100 op 
! de slave een SlowWhoop signaal geven.
! *****************************************************

SendTo 5;WaitFreeRF 0,0 ! Anders wordt deze test wel heel stroperig uitgevoerd

! verzend nu een testje waarbij eveneens de Eventlist wordt beschreven
sendto 5; sound ! geef eerste teken van leven op de slave
sendto 5; sendkaku A3,off
sendto 5; eventlisterase
sendto 5; eventlistwrite; userevent 100; sound 2
sendto 5; eventlistwrite; userevent 100; sound 3
sendto 5; eventlistwrite; userevent 100; senduserevent 123
sendto 5; sendkaku A3,on
sendto 5; sound
SendUserEvent 100

FileWrite
 





#define NODO_MAC 0x54, 0xa5, 0x8d, 0x17, 0xaf, 0x41 // Productie MAC Paul ???

Serial.print("*** debug: =");Serial.println(); //??? Debug
Serial.println("*** debug: ");//???



*/

