//#######################################################################################################
//#################################### Plugin-23: PWM Led dimmer ########################################
//#######################################################################################################

/*********************************************************************************************\
 * Funktionele beschrijving: Dit device stuurt een PWM signaal uit waarmee een RGB-led kan worden aangestuurd.
 *                           Als de Arduino uitgang wordt gebruikt om een MOSFET of een andere schakeling
 *                           aan te sturen, dan kunnen hiermee led-strips worden aangestuurd.
 *
 *                           
 * Auteur             : P.K.Tonkes
 * Support            : P.K.Tonkes@gmail.com
 * Datum              : 26-05-2013
 * Versie             : 22-12-2013, Versie 1,1, P.K.Tonkes: Event en Send commando in 1 plugin ondergebracht.
 *                      26-05-2013, Versie 1.0, P.K.Tonkes: Beta versie 
 * Nodo productnummer : Plugin-23 PWM Led-dimmer (SWACDE-23-V11)
 * Compatibiliteit    : Vanaf Nodo build nummer 645
 * Syntax             : RGBLed <Red>,<Green>,<Blue>,<FadeTimeInMinutes> ==> Stuurt de RGB-led aan
 *                      RGBLedSend <NodoUnit>,<Red>,<Green>,<Blue>,<FadeTimeInMinutes> ==> Verzendt event en stuurt
 *                      de RGB-led van een andere Nodo aan
 * 
 * LET OP: De PLUGIN_CORE niet gebruiken als er een IR-Zender is aangesloten !
 *         Een van de RGB-kleuren maakt gebruik van de IR_TX_DATA pen. Het continue
 *         aansturen van de IR-Leds zou de Leds/transistor kunnen overbelasten.   
 * 
 ***********************************************************************************************
 * Technische beschrijving:
 *
 * De dimfunktie werkt op basis van Pulsbreedte modulatie (PWM) Dit is een standaard methode voor 
 * dimmen van LED's, mmaar wrdt ook gebruikt voor regelen van snelheid van motoren. 
 * Omdat de PWM funkties niet op alle pinnen van de Arduino beschikbaar zijn,
 * zijn deze in dit device hard toegewezen is de pinkeuze geen gebruikers optie.
 *
 * Nodo Mega:  Rood  = Arduino pen PWM-5
 *             Groen = Arduino pen PWM-6
 *             Blauw = Arduino pen PWM-7
 *
 * Nodo Small: Rood  = Arduino pen D9
 *             Groen = Arduino pen D10
 *             Blauw = Arduino pen D11 (LET OP: Op een standaard Nodo is dit de IR zend-led. Deze verwijderen!)
 *             
 \*********************************************************************************************/
 
#define PLUGIN_ID 23
#define PLUGIN_NAME_23_COMMAND "RGBLedSend"
#define PLUGIN_NAME_23_EVENT   "RGBLed"

#if NODO_MEGA
#define PWM_R     5
#define PWM_G     6
#define PWM_B     7
#else
#define PWM_R     9
#define PWM_G    10
#define PWM_B    11
#endif

#ifdef PLUGIN_023_CORE
byte InputLevelR=0, InputLevelG=0, InputLevelB=0;
byte OutputLevelR=0, OutputLevelG=0, OutputLevelB=0;
int CalledCounter=0, FadeTimeCounter=0;

boolean FadeLed(void)
  {
  if(InputLevelR>OutputLevelR)OutputLevelR++;
  if(InputLevelG>OutputLevelG)OutputLevelG++;
  if(InputLevelB>OutputLevelB)OutputLevelB++;

  if(InputLevelR<OutputLevelR)OutputLevelR--;
  if(InputLevelG<OutputLevelG)OutputLevelG--;
  if(InputLevelB<OutputLevelB)OutputLevelB--;

  analogWrite(PWM_R,OutputLevelR);
  analogWrite(PWM_G,OutputLevelG);
  analogWrite(PWM_B,OutputLevelB);

  if(InputLevelR==OutputLevelR && InputLevelG==OutputLevelG && InputLevelB==OutputLevelB)
    return false;// klaar met faden naar nieuwe waarde
  else
    return true;
  }
  
void Plugin_023_FLC(void)
  {  
  if(FadeTimeCounter==CalledCounter++)
    {
    CalledCounter=0;
    if(!FadeLed())
      FastLoopCall_ptr==0;
    }
  }
  
#endif  

boolean Plugin_023(byte function, struct NodoEventStruct *event, char *string)
  {
  boolean success=false;
  byte x;
  struct NodoEventStruct TempEvent;  
  static byte fade=0;
  
  switch(function)
    { 
    #ifdef PLUGIN_023_CORE
    case PLUGIN_EVENT_IN:
      {
      x=event->Par1 & 0x1f; // Unit nummer

      if(x==0 || x==Settings.Unit)
        {
        fade=event->Par1>>5;
        InputLevelR = (event->Par2>>16) & 0xff;
        InputLevelG = (event->Par2>>8 ) & 0xff;
        InputLevelB = (event->Par2)&0xff;
        
        if(fade==0)
          {
          while(FadeLed())
            delay(5);
          }
        else
          {
          // De waarde fade bevat het aantal minuten dat de omschakeling maximaal mag duren.
          // Per minuut 60000/Loop_INTERVAL_1 keer een call naar Plugin_023_FLC
          // In een minuut max. 255 dimniveaus ==> (60000/(Loop_INTERVAL_1 * 255)) calls nodig voor 1 dimniveau verschil. 
          FadeTimeCounter=fade*60000/(Loop_INTERVAL_1*255);
          CalledCounter=0;
          FastLoopCall_ptr=&Plugin_023_FLC;
          }
        
        success=true;
        }
      break;
      }

    case PLUGIN_COMMAND:
      {
      ClearEvent(&TempEvent);
      TempEvent.DestinationUnit=event->Par1&0x1F; 
      TempEvent.Port      = VALUE_ALL;
      TempEvent.Type      = NODO_TYPE_PLUGIN_EVENT;
      TempEvent.Command   = PLUGIN_ID;
      TempEvent.Par1      = event->Par1;
      TempEvent.Par2      = event->Par2;
      SendEvent(&TempEvent, false ,true, Settings.WaitFree==VALUE_ON);
  
      success=true;
      break;
      }
    #endif // CORE
    
    #if NODO_MEGA
    case PLUGIN_MMI_IN:
      {

      char *TempStr=(char*)malloc(INPUT_COMMAND_SIZE);

      if(GetArgv(string,TempStr,1))
        {
        event->Command = 23;                                                    // Plugin nummer  
        event->Type = 0;
        
        // Event:   RGBLed <R>,<G>,<B>,<Fade> 
        if(strcasecmp(TempStr,PLUGIN_NAME_23_EVENT)==0)
          {
          event->Type = NODO_TYPE_PLUGIN_EVENT;
          event->Par1=0;
          if(event->Type)
            {
            if(GetArgv(string,TempStr,2))                                       // R 
              {
              event->Par2=str2int(TempStr)<<16;
              if(GetArgv(string,TempStr,3))                                     // G
                {
                event->Par2|=str2int(TempStr)<<8;
                if(GetArgv(string,TempStr,4))                                   // B
                  {
                  event->Par2|=str2int(TempStr);
                  success=true;
                  }
                if(GetArgv(string,TempStr,5))                                   // Fade
                  event->Par1=(str2int(TempStr)<<5);                            // Fade zit in de hoogste drie bits (0..7)
                }
              }
            }
          }
          
        // Command: RGBLedSend <NodoUnit>,<R>,<G>,<B>,<Fade> 
        if(strcasecmp(TempStr,PLUGIN_NAME_23_COMMAND)==0)
          {
          event->Type = NODO_TYPE_PLUGIN_COMMAND;
          if(event->Type)
            {
            if(GetArgv(string,TempStr,2))                                       // Unit
              {
              event->Par1=str2int(TempStr)&0x1F;                                // Unit zit in de eerste vijf bits (0..31)                                       
              if(GetArgv(string,TempStr,3))                                     // R 
                {
                event->Par2=str2int(TempStr)<<16;
                if(GetArgv(string,TempStr,4))                                   // G
                  {
                  event->Par2|=str2int(TempStr)<<8;
                  if(GetArgv(string,TempStr,5))                                 // B
                    {
                    event->Par2|=str2int(TempStr);
                    success=true;
                    }
                  if(GetArgv(string,TempStr,6))                                 // Fade
                    event->Par1|=(str2int(TempStr)<<5);                         // Fade zit in de hoogste drie bits (0..7)
                  }
                }
              }
            }
          }
        }
      free(TempStr);
      break;
      }

    case PLUGIN_MMI_OUT:
      {
      if(event->Type==NODO_TYPE_PLUGIN_COMMAND)
        {
        strcpy(string,PLUGIN_NAME_23_COMMAND);
        strcat(string," ");
        strcat(string,int2str((event->Par1 & 0x1f)));                           // Unit
        strcat(string,",");
        }
      else
        {
        strcpy(string,PLUGIN_NAME_23_EVENT);
        strcat(string," ");
        }
        
      strcat(string,int2str((event->Par2 >>16)&0xff));                          // Parameter-1 = R (5-bit)
      strcat(string,",");
      strcat(string,int2str((event->Par2 >> 8)&0xff));                          // Parameter-2 = G (8-bit)
      strcat(string,",");
      strcat(string,int2str((event->Par2     )&0xff));                          // Parameter-3 = B (8-bit)
      strcat(string,",");
      strcat(string,int2str((event->Par1     )>>5));                            // Parameter-4 = Fade (3-bit)

      break;
      }
    #endif //MMI
    }      
  return success;
  }

  