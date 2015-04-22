
/**********************************************************************************************\
 * verzendt een event en geeft dit tevens weer op SERIAL
 * Als UseRawSignal=true, dan wordt er geen signaal opgebouwd, maar de actuele content van de
 * RawSignal buffer gebruikt.
 \*********************************************************************************************/
boolean SendEvent(struct NodoEventStruct *ES, boolean UseRawSignal, boolean Display)
  {    
  ES->Direction=VALUE_DIRECTION_OUTPUT;
  // PrintNodoEvent("DEBUG: SendEvent():", ES);//???
  
  byte Port         = ES->Port;
  byte Source       = ES->SourceUnit;                                           // bewaar oorspronkelijke source voor de WebApp.Andere poorten: dan deze unit is source
  ES->SourceUnit    = Settings.Unit;                                        // anderz zal in geval van een EventSend een event worden doorgestuurd via andere poorten
                                                                                // waarna ontvangende Nodo's een verkeerde vulling van de NodoOnline tabel krijgen.
                                                                                
  boolean Nodo      = ES->Type==NODO_TYPE_EVENT || ES->Type==NODO_TYPE_COMMAND || ES->Type==NODO_TYPE_SYSTEM; 
  boolean Broadcast = (ES->Flags&TRANSMISSION_BROADCAST)!=0;

  #if HARDWARE_STATUS_LED_RGB
  if(Broadcast)
    Led(BLUE);
  #endif
  
  PluginCall(PLUGIN_EVENT_OUT, ES,0);                                           // loop de plugins langs voor eventuele afhandeling van dit event.

  while(millis()<HoldTransmission);                                             // Respecteer een minimale tijd tussen verzenden van events. Wachten alvorens event te verzenden.  
  HoldTransmission=DELAY_BETWEEN_TRANSMISSIONS+millis();        


  #if HARDWARE_RAWSIGNAL
  if(!UseRawSignal)                                                             // Als het geen RawSignal betreft, dan een pulsenreeks opbouwen
    Nodo_2_RawSignal(ES);
  #endif
  
                                             
  #if HARDWARE_INFRARED
  if(Broadcast || Port==VALUE_SOURCE_IR || (Port==VALUE_ALL && (!Nodo || NodoOnline(0,VALUE_SOURCE_IR,0))))
    {
    #if NODO_MEGA && HARDWARE_SERIAL_1
    ES->Port=VALUE_SOURCE_IR;
    if(Display)PrintEvent(ES,VALUE_ALL);
    #endif
    RawSendIR();
    }
  #endif


  #if HARDWARE_RF433
  if(Broadcast || Port==VALUE_SOURCE_RF || (Port==VALUE_ALL && (!Nodo || NodoOnline(0,VALUE_SOURCE_RF,0))))
    {
    #if NODO_MEGA && HARDWARE_SERIAL_1
    ES->Port=VALUE_SOURCE_RF;
    if(Display)PrintEvent(ES,VALUE_ALL);
    #endif
    RawSendRF();
    }
 #endif


  #if HARDWARE_NRF24L01
  if(Port==VALUE_SOURCE_NRF24L01 || (Port==VALUE_ALL && (Broadcast || NodoOnline(0,VALUE_SOURCE_NRF24L01,0))))
    {
    #if NODO_MEGA
    ES->Port=VALUE_SOURCE_NRF24L01;
    if(Display)PrintEvent(ES,VALUE_ALL);
    #endif
    // Trace("SendEvent(): NRF24L01 Start.",0);//???
    if(byte x=Port_NRF24L01_EventSend(ES))
      RaiseMessage(MESSAGE_NODO_NOT_FOUND,x);
    // Trace("SendEvent(): NRF24L01 Finish.",0);//???
    }
  #endif
  

  #if HARDWARE_I2C
  if(Port==VALUE_SOURCE_I2C || (Port==VALUE_ALL && (Broadcast || NodoOnline(0,VALUE_SOURCE_I2C,0))))
    {
    ES->Port=VALUE_SOURCE_I2C;
    #if NODO_MEGA
    if(Display)PrintEvent(ES,VALUE_ALL);
    #endif
    //Trace("SendEvent(): I2C Start.",0);//???
    Port_I2C_EventSend(ES);
    //Trace("SendEvent(): I2C Finish.",0);//???
    }
  #endif
  

  #if HARDWARE_ETHERNET
  if((Port==VALUE_SOURCE_HTTP || Port==VALUE_ALL || ES->Type==NODO_TYPE_PLUGIN_EVENT) && ES->Type!=NODO_TYPE_SYSTEM )
    {
    ES->Port=VALUE_SOURCE_HTTP;
    ES->SourceUnit=Source;                                                      // oorspronkelijke bron weergeven
    if(Display)PrintEvent(ES,VALUE_ALL);
    //Trace("SendEvent(): HTTP Start.",0);//???
    SendHTTPEvent(ES);
    //Trace("SendEvent(): HTTP End.",0);//???
    }
  #endif 
  }
  
  
  