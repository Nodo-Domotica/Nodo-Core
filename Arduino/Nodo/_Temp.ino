
/*


                          
x=(v&0x1FF)*5; // Minuten na 0:00 met resolutie van 5 min.
Serial.print("*** debug: uren         =");Serial.println(x/60) ; //??? Debug
Serial.print("*** debug: minuten      =");Serial.println(x-(x/60)*60); //??? Debug
Serial.print("*** debug: Alarm nummer =");Serial.println((v>>14)  & 0x03); //??? Debug
Serial.print("*** debug: dag          =");Serial.println((v>> 9)  & 0x0F); //??? Debug
Serial.print("*** debug: Enabled      =");Serial.println((v>>13)  & 1   ); //??? Debug 


Date=MM-DD-YYYY, Time=HH:MM:SS, Day=xxx, Dls=n

Event   = Alarm <nummer>
Command = AlarmSet <nummer>, <hour>, <minute>
                    0,1..4   00..23  00..59
                    3        5       6


Serial.print("*** debug: =");Serial.println(); //??? Debug
Serial.println("*** debug: ");//???


*/
