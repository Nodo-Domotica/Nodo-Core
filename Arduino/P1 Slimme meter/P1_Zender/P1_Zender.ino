void setup(void)
  {
  Serial.begin(9600,SERIAL_7E1);
  pinMode(13, OUTPUT);  
  digitalWrite(A0, HIGH);
  }

unsigned long DelayTimer=0L;
  
void loop(void)
  {
  if(analogRead(A0)>512)
    {
    if(DelayTimer<millis())
      {
      digitalWrite(13, HIGH);  
  
      Serial.println(F("/XMX5XMXABCE000062748"));
      Serial.println(F(""));
      Serial.println(F("0-0:96.1.1(31333633393939362020202020202020)"));
      Serial.println(F("1-0:1.8.1(00557.348*kWh)"));
      Serial.println(F("1-0:1.8.2(00620.166*kWh)"));
      Serial.println(F("1-0:2.8.1(00000.000*kWh)"));
      Serial.println(F("1-0:2.8.2(00000.000*kWh)"));
      Serial.println(F("0-0:96.14.0(0002)"));
      Serial.println(F("1-0:1.7.0(0000.55*kW)"));
      Serial.println(F("1-0:2.7.0(0000.00*kW)"));
      Serial.println(F("0-0:17.0.0(999*A)"));
      Serial.println(F("0-0:96.3.10(1)"));
      Serial.println(F("0-0:96.13.1()"));
      Serial.println(F("0-0:96.13.0()"));
      Serial.println(F("0-1:96.1.0(3238303131303038333134313136363133)"));
      Serial.println(F("0-1:24.1.0(03)"));
      Serial.println(F("0-1:24.3.0(131002110000)(08)(60)(1)(0-1:24.2.0)(m3)"));
      Serial.println(F("(00055.209)"));
      Serial.println(F("0-1:24.4.0(1)"));
      Serial.println(F("!"));
  
      digitalWrite(13, LOW);  
      DelayTimer=millis()+10000;    
      }
    }
  else
    {
    DelayTimer=0L;
    }
  }
