
/**********************************************************************************************\
 * verzendt een event en geeft dit tevens weer op SERIAL
 * Als UseRawSignal=true, dan wordt er geen signaal opgebouwd, maar de actuele content van de
 * RawSignal buffer gebruikt.
 \*********************************************************************************************/
boolean SendEvent(struct NodoEventStruct *ES, boolean UseRawSignal, boolean Display)
  {    
  // PrintNodoEvent("DEBUG: SendEvent():", ES);//???

  ES->Direction=VALUE_DIRECTION_OUTPUT;
  byte x,Port=ES->Port;
  boolean broadcast=((ES->Flags&TRANSMISSION_BROADCAST) != 0);


  #if HARDWARE_STATUS_LED || HARDWARE_STATUS_LED_RGB
  if(broadcast)
    Led(BLUE);
  #endif
  
  PluginCall(PLUGIN_EVENT_OUT, ES,0);                                           // loop de plugins langs voor eventuele afhandeling van dit event.

  #if HARDWARE_RAWSIGNAL || HARDWARE_RF433 || HARDWARE_INFRARED
  if(UseRawSignal)
    {

    #if HARDWARE_INFRARED
    if(Port==VALUE_SOURCE_IR || Port==VALUE_ALL)
      {
      ES->Port=VALUE_SOURCE_IR;
      #if NODO_MEGA
      if(Display)PrintEvent(ES,VALUE_ALL);
      #endif
      RawSendIR();
      }
    #endif

    #if HARDWARE_RF433
    if(Port==VALUE_SOURCE_RF || Port==VALUE_ALL)
      {
      ES->Port=VALUE_SOURCE_RF;
      #if NODO_MEGA
      if(Display)PrintEvent(ES,VALUE_ALL);
      #endif
      RawSendRF();
      }
    #endif
    }
  #endif
  

  while(millis()<HoldTransmission);                                             // Respecteer een minimale tijd tussen verzenden van events. Wachten alvorens event te verzenden.  
  HoldTransmission=DELAY_BETWEEN_TRANSMISSIONS+millis();        
                                             
  #if HARDWARE_INFRARED
  if(NodoOnline(0,VALUE_SOURCE_IR,0) || Port==VALUE_SOURCE_IR || broadcast)
    {
    Nodo_2_RawSignal(ES);
    ES->Port=VALUE_SOURCE_IR;
    #if NODO_MEGA
    if(Display)PrintEvent(ES,VALUE_ALL);
    #endif
    RawSendIR();
    }
  #endif



  #if HARDWARE_RF433
  if(NodoOnline(0,VALUE_SOURCE_RF,0) || Port==VALUE_SOURCE_RF || broadcast)
    {
    Nodo_2_RawSignal(ES);
    ES->Port=VALUE_SOURCE_RF;
    #if NODO_MEGA
    if(Display)PrintEvent(ES,VALUE_ALL);
    #endif
    RawSendRF();
    }
  #endif


  #if HARDWARE_NRF24L01
  if(NodoOnline(0,VALUE_SOURCE_NRF24L01,0) || broadcast)
    {
    ES->Port=VALUE_SOURCE_NRF24L01;
    #if NODO_MEGA
    if(Display)PrintEvent(ES,VALUE_ALL);
    #endif
// Trace("SendEvent(): NRF24L01 Start",0);//???
    if(x=Port_NRF24L01_EventSend(ES))
      RaiseMessage(MESSAGE_NODO_NOT_FOUND,x);
// Trace("SendEvent(): NRF24L01 Finish",0);//???
    }
  #endif
  

  #if HARDWARE_I2C
  if(NodoOnline(0,VALUE_SOURCE_I2C,0) || broadcast)
    {
    ES->Port=VALUE_SOURCE_I2C;
    #if NODO_MEGA
    if(Display)PrintEvent(ES,VALUE_ALL);
    #endif
    Port_I2C_EventSend(ES);
//Trace("SendEvent(): I2C",0);//???
    }
  #endif
  

  #if HARDWARE_ETHERNET
  // Verstuur signaal als HTTP-event. Enkele soorten events moeten altijd naar de WebApp. ??? Zal aangepast worden zodra we de WebSocket hebben.
  if(HW_Status(HW_ETHERNET) && (Port==VALUE_SOURCE_HTTP || Port==VALUE_ALL) && Settings.PortOutput!=0 && ES->Type!=NODO_TYPE_SYSTEM)
    {
    ES->Port=VALUE_SOURCE_HTTP;
    if(Display)PrintEvent(ES,VALUE_ALL);
    SendHTTPEvent(ES);
    }
  #endif 
  }
  

