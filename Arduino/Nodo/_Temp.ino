
// Event, EventPar1, EventPar2, EventUnit, EventPort

// CatchEvent Unit, Event, par1, Par2, scriptfile  : 

// if xxx=yyy; if aaa!=bbb; sound





/*

Date=MM-DD-YYYY, Time=HH:MM:SS, Day=xxx, Dls=n

Event   = Alarm <nummer>
Command = AlarmSet <nummer>, <hour>, <minute>
                    0,1..4   00..23  00..59
                    3        5       6

#define NODO_MAC 0x54, 0xa5, 0x8d, 0x17, 0xaf, 0x41 // Productie MAC Paul ???

Serial.print("*** debug: =");Serial.println(); //??? Debug
Serial.println("*** debug: ");//???


*/



//
//boolean VariableSet(byte Variable, float Value, boolean Event)
//  {    
//  Trace(0,0,0);
//  if(Variable >0 && Variable<=USER_VARIABLES_MAX)
//    {
//    if(Value>=(USER_VARIABLES_RANGE_MIN) && Value <=USER_VARIABLES_RANGE_MAX)
//      {
//      Serial.print("*** debug: Variable=");Serial.print(Variable); //??? Debug
//      Serial.print(", Value=");Serial.print(Value); //??? Debug
//      Serial.print(", UserVar[Variable-1]=");Serial.println(UserVar[Variable-1]); //??? Debug
//
//      UserVar[Variable-1]=(int)(Value*100);
//
//      if(Event)
//        ProcessEvent2(NodoFloat2event(UserVar[Variable-1], Variable, CMD_VARIABLE_EVENT), VALUE_DIRECTION_INTERNAL, VALUE_SOURCE_VARIABLE, 0, 0);      // verwerk binnengekomen event.
//
//      return true;
//      }
//  return false;
//  }

