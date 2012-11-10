


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

