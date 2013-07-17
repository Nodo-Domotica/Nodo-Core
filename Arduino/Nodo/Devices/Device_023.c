
//#######################################################################################################
//#################################### Device-23: PWM Led dimmer ########################################
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
 * Versie             : 1.0 (Beta)
 * Nodo productnummer : Device-23 PWM Led-dimmer (SWACDE-23-V10)
 * Compatibiliteit    : Vanaf Nodo build nummer 530
 * Syntax             : PWM <R>,<G>,<B>, <Fade: On | Off>
 *                      De opgegeven waarden R,G en B kunnen een waarde hebben van 0..255
 *                      Laatste parameter <fade> is optioneel en kan [On] of [Off] zijn.
 ***********************************************************************************************
 * Technische beschrijving:
 *
 * Compiled size      : 1225 bytes
 * Externe funkties   : str2cmd(), GetArgv(), cmd2str(), str2int(), int2str().
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
 
#define DEVICE_ID 23
#define DEVICE_NAME_23 "LED"

#if NODO_MEGA
#define PWM_R     5
#define PWM_G     6
#define PWM_B     7
#else
#define PWM_R     9
#define PWM_G    10
#define PWM_B    11
#endif

boolean Device_023(byte function, struct NodoEventStruct *event, char *string)
  {
  boolean success=false;
  static byte InputLevelR=0, InputLevelG=0, InputLevelB=0;
  static byte OutputLevelR=0, OutputLevelG=0, OutputLevelB=0;
  static byte fade=VALUE_OFF;
  
  switch(function)
    { 
    #ifdef DEVICE_023_CORE
    case DEVICE_ONCE_A_SECOND:
      {
      if(fade==VALUE_ON)
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
          fade=VALUE_OFF;
        }
      success=true;
      break;
      }
      
    case DEVICE_COMMAND:
      {
      InputLevelR=(event->Par2 >>16)&0xff;
      InputLevelG=(event->Par2 >> 8)&0xff;
      InputLevelB=(event->Par2     )&0xff;  
      fade=event->Par1;
      
      if(fade!=VALUE_ON)
        {
        OutputLevelR=InputLevelR;
        OutputLevelG=InputLevelG;
        OutputLevelB=InputLevelB;

        analogWrite(PWM_R,OutputLevelR);
        analogWrite(PWM_G,OutputLevelG);
        analogWrite(PWM_B,OutputLevelB);
        }
      success=true;
      break;
      }
    #endif // CORE
    
    #if NODO_MEGA
    case DEVICE_MMI_IN:
      {
      char *TempStr=(char*)malloc(26);
      string[25]=0;

      if(GetArgv(string,TempStr,1))
        {
        if(strcasecmp(TempStr,DEVICE_NAME_23)==0)
          {
          if(GetArgv(string,TempStr,2)) 
            {
            event->Par2=str2int(TempStr)<<16;
            if(GetArgv(string,TempStr,3))
              {
              event->Par2|=str2int(TempStr)<<8;
              if(GetArgv(string,TempStr,4))
                {
                event->Par2|=str2int(TempStr);
                success=true;
                event->Type = NODO_TYPE_DEVICE_COMMAND;
                }
              if(GetArgv(string,TempStr,5))
                event->Par1=str2cmd(TempStr);
              else
                event->Par1=VALUE_OFF;
              }
            }
          }
        }
      free(TempStr);
      break;
      }

    case DEVICE_MMI_OUT:
      {
      strcpy(string,DEVICE_NAME_23);                        // Commando 
      strcat(string," ");
      strcat(string,int2str((event->Par2 >>16)&0xff));      // Parameter-1 = R (8-bit)
      strcat(string,",");
      strcat(string,int2str((event->Par2 >> 8)&0xff));      // Parameter-2 = G (8-bit)
      strcat(string,",");
      strcat(string,int2str((event->Par2     )&0xff));      // Parameter-3 = B (8-bit)
      strcat(string,",");
      strcat(string,cmd2str(event->Par1));                  // Parameter-4 = Fading <On | Off> (8-bit)
      break;
      }
    #endif //MMI
    }      
  return success;
  }

