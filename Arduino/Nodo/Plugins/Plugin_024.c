//#######################################################################################################
//#################################### Plugin-42: P1 Slimme meter            ############################
//#######################################################################################################

/*********************************************************************************************\
 * Deze plugin leest waarden uit van een P1 slimme meter. Hiermee is het mogelijk om je
 * electra en/of gasverbruik te meten en de waarden te gebruiken als Nodo variabelen. Deze
 * variabelen kunnen desgewenst worden weergegeven in de WebApp.   
 *
 * Auteur             : Paul Tonkes, p.k.tonkes@gmail.com
 * Support            : www.nodo-domotica.nl
 * Datum              : Okt.2013
 * Versie             : 0.5 Nog ernstig in beta
 * Nodo productnummer : ???
 * Compatibiliteit    : Vanaf Nodo build nummer 596
 * Syntax             : "P1Read <Par1:Variabele>, <Par2:Waarde>"
 *
 ***********************************************************************************************
 * Funktionele beschrijving:
 * 
 * Slimme meters zenden data via seriele communicatie. Deze data wordt door de meter verzonden zodra
 * de trigger lijn van de meter hoog (+5 volt) wordt. De data bestuit regels met daarin per regel een
 * z.n. "OBIS" code die aangeeft om wat voor een soort gegeven het gaat en de inhoudelijke waarde.
 * Deze plugin vangt de data van de Slimme Meter op en haalt de waarden op om ze vervolgens in de 
 * opgegeven variabele te plaatsen. Zodra de variabele is gevuld kan deze naar eigen behoefte worden
 * gebruikt voor verdere verwerking.
 *     
 *********************************************************************************************** 
 * Hardware: Sluit de P1 slimme meter als volgt aan:
 *            
 *            P1:                                       Nodo:
 *            ========                                  ==========
 *            GND                     <-------------->  GND
 *            Trigger (Hoog = zenden) <---------------  WiredOut-1 
 *            Data (Laag=rust)        ---(inverter)-->  RX0  (Hoog = rust) 
 *
 * LET OP: Omdat deze plugin gebruik maakt van de Seriele communicatie is er geen mogelijkheid 
 *         meer om via seriele communicatie commando's naar de Nodo te sturen of events te bekijken.
 *         Voor een Nodo-small is dit geen probleem daar deze geen MMI heeft. Voor een Mega blijft 
 *         een TelNet verbinding wel gewoon mogelijk.
 *         van de Nodo. In geval van debugging kan, als de seriele settings op uw terminal programma
 *         overeenkomen met die van de Slimme Meter en deze plugin, wel worden gebruikt om de output
 *         van de Slimme meter te bekijken. In alle overige situaties een ventuele USB/FTDI aansluiting
 *         loskoppelen.        
 *   
 ***********************************************************************************************
 * Configuratie:
 *  
 *       - P1 meters van verschillende fabrikanten hanteren wel eens andere communicatie snelheden 
 *         en -settings. Kijk hiervoor naar de specificaties van uw meter en pas in deze plugin de 
 *         waarden P24_SERIAL_BAUD en P24_SERIAL_CONFIG aan. 
 *
 *       - Er zijn twee mogelijkheden om de data van de Slimme Meter op te vangen:
 *        
 *         1. OBIS-code: Dit is een standaard codering die wordt gehanteerd door alle netbeheerders
 *            en dus ook (vrijwel) altijd wordt ondersteund door alle Slimme Meters. Door een OBIS-
 *            code op te nemen in de opvangtabel, kan een bijbehorende waarde worden opgevangen.   
 *            Wilt je andere waarden uitlezen nan nu voorgedefinieerd in de opvangtabel, zoek dan 
 *            de OBIS code op van de waarde die u wilt uitlezen en pas de tabel aan.
 *            
 *          2. Regelnummer: In sommige gevallen is de waarde die je wilt gebruiken niet aan een
 *             OBIS-code gekoppeld (Vaak geval bij een gasmeter waarde). In deze gevallen kun je
 *             als je de dump van je meter kent, opgeven uit welk regelnummer je de data wilt halen.
 *             Geen dan in de opvangtabel niet de OBIS-Code, maar het regelnummer vooraf gegaan 
 *             met een # teken. Zodra dit regelnummer voorbij komt, wordt de waarde er uitgehaald.      
 *
 *        - Let op dat deze plugin alleen numurieke waarden teruggeeft die zich bevinden tussen haakjes.
 *          Niet nummerieke tekens tussen de haakjes, zoals 'Kwh' of 'm3' aanduidingen worden genegeerd.
 *          De variabelen worden gevuld met twee cijfers achter de komma, mits de meter deze ook zo 
 *          aanlevert.    
 *         waarden zal uitlezen en doorgeven aan een variabele.
 *  
 ***********************************************************************************************
 * Gebruik:  P1Read <Variable>, <Value 1..>
 * 
 *         Als de variabele is gevuld, kan deze op gebruikelijke wijze worden gebruikt in de Nodo
 *         of worden verzonden via RF, IR, I2C of HTTP naar bijvoorbeeld de WebApp.   
 *
 * LET OP: De Slimme meter zend een datablok. Gedurende verzenden van dit datablok, het  
 *         analyseren van het signaal en eventuele her-transmissies zal de Nodo geen events
 *         ontvangen of genereren. Het verdient daarom de aanbeveling om voor deze plugin 
 *         geen Nodo te gebruiken die tijdkritische taken moet verrichten of waar een 
 *         trefzekere ontvangst van verwacht wordt.    
 *         
 *         Als onverhoopt de data niet correct wordt opevangen, dan worden enkele nieuwe 
 *         pogingen ondernomen. Als na deze pogingen de opgevraagde OBIS code niet wordt
 *         gedetecteerd, dan zal de plugin terugkeren met een foutmelding. Let hierbij op
 *         dat vervolgregels in de eventlist, als gevolg van deze foutmelding dan NIET worden
 *         uitgevoerd. Setten van timers bij voorkeur dus VOOR aanroep van deze plugin en niet
 *         er na. Dit is geen bug, maar 'build-as-designed'.         
 ***********************************************************************************************
 * Technische specificatie:
 *
 * Compiled size      : ??? bytes voor een Mega en ??? voor een Small.
 * Externe funkties   : float2ul(), 
 * Specificatie P1    : Dutch Smart Meter Requirements v4.0.4 Final P1 ZT.doc Date: 03-04-2012 
 \*********************************************************************************************/

#define P24_SERIAL_BAUD   9600
#define P24_SERIAL_CONFIG SERIAL_7E1


#define CATCH_TABLE_MAX 7 // Aantal op te vangen mogelijkheden

char CatchTable[CATCH_TABLE_MAX][10]={   "1.8.1",     // Waarde 1, Electra: Totaal verbruik tarief 1 (nacht)
                                         "1.8.2",     // Waarde 2, Electra: Totaal verbruik tarief 2 (dag)
                                         "2.8.1",     // Waarde 3, Electra: Totaal geleverd tarief 1 (nacht)
                                         "2.8.2",     // Waarde 4, Electra: Totaal geleverd tarief 2 (dag)
                                         "1.7.0",     // Waarde 5, Electra: Huidig verbruik
                                         "2.7.0",     // Waarde 6, Electra: Huidige teruglevering
                                         "#18"  };    // Waarde 7, Gas: Huidige verbruik op regel 18

#define PLUGIN_ID 24
#define PLUGIN_NAME "P1Read"
#define P24_BUFFERSIZE 100
boolean ParseString(char* Input, char* Result, char* Start, char* StopChrs, char* ValidChrs);
char* P24_Str;
unsigned long EvenWachtenPizza=0;
boolean P24_Initialized=false;

void P24Init(void)
  {
  Serial.println(F("Plugin_024: Attention, Serial/USB/FTDI input captured by plugin!"));
  delay(1000);
  Serial.begin(P24_SERIAL_BAUD,P24_SERIAL_CONFIG);

  P24_Initialized=true;
  }


boolean Plugin_024(byte function, struct NodoEventStruct *event, char *string)
  {
  boolean success=false;
  boolean Fetched=false;
  int  ReceivedCharCounter;
  byte ReceivedChar,var;
  byte Try=0,Line;
  
  float FetchedData=0;
  
  switch(function)
    {
    #ifdef PLUGIN_024_CORE
                
    case PLUGIN_INIT:
      {
      P24_Str=(char*)malloc(P24_BUFFERSIZE);
      break;
      }
      
    case PLUGIN_COMMAND:
      {
      if(!P24_Initialized)P24Init();
      var=event->Par1; // variabele die moet worden gevuld.

      do
        {
        Line=0;
        // De Slimme meter begint data te zenden zodra de trigger van laag naar hoog gaat. 
        // gedurende de transmissie blijft de trigger lijn hoog.
        digitalWrite(PIN_WIRED_OUT_1, HIGH);
        
        // wacht beperkte tijd op binnenkomende data. Het datablok zal in normale situaties
        // zeker binnen 1.5 seconde binnengekomen moeten zijn.
        EvenWachtenPizza=millis()+1500;
        while(EvenWachtenPizza>millis())
          {
          while(Serial.available())
            {
            ReceivedChar=Serial.read();
            if(ReceivedCharCounter<P24_BUFFERSIZE)
              P24_Str[ReceivedCharCounter++]=ReceivedChar;
              
            if(ReceivedChar=='\n')
              {
              P24_Str[ReceivedCharCounter]=0;
              ReceivedCharCounter=0;
              Line++;   
              
              if(CatchTable[event->Par2-1][0]=='#')
                {
                if(Line==strtol(CatchTable[event->Par2-1]+1,NULL,0))
                  {
                  if(ParseString(P24_Str,P24_Str,"(" , ")" , "1234567890."))
                    {
                    FetchedData=atof(P24_Str);
                    Fetched=true;
                    }
                  }
                }

              else if(ParseString(P24_Str,P24_Str,CatchTable[event->Par2-1] , ")" , "1234567890."))
                {
                FetchedData=atof(P24_Str);
                Fetched=true;
                }
              P24_Str[0]=0;
              }
            }
          }
  
        // Breng de trigger lijn weer omlaag
        digitalWrite(PIN_WIRED_OUT_1, LOW);
  
        if(!Fetched)
          {
          // Er is iets mis gegaan, de OBIS code is niet voorbij gekomen.
          // Dan een nieuwe poging.
          delay(2000);
          }
        }while(!Fetched && ++Try<5);            
        
      if(Fetched)
        {  
        ClearEvent(event);                             // Ga uit van een default schone event. Oude eventgegevens wissen.        
        event->Type         = NODO_TYPE_COMMAND;
        event->Command      = CMD_VARIABLE_SET;        // Commando "VariableSet"
        event->Par1         = var;                     // Variabele die gevuld moet worden.
        event->Par2         = float2ul(FetchedData);   // uitgelezen waarde 
        success=true;
        }
        
      else
        success=false;
        
      break;
      }
    #endif // CORE
      
    #if NODO_MEGA
    case PLUGIN_MMI_IN:
      {
      if(GetArgv(string,P24_Str,1))
        {
        if(strcasecmp(P24_Str,PLUGIN_NAME)==0)
          {
          // Par1 en Par2 hoeven niet te worden geparsed omdat deze default al door de MMI invoer van de Nodo 
          // worden gevuld indien het integer waarden zijn. Toetsen op bereiken moet nog wel plaats vinden.
          // Par1 -> Variabele die gevuld moet worden
          // Par2 -> OBIS code verwijzing <1..CATCH_TABLE_MAX>
          
          if(event->Par1>=1 && event->Par1<=USER_VARIABLES_MAX && event->Par2>=1 && event->Par2<=CATCH_TABLE_MAX)            
            {
            success=true;
            event->Type    = NODO_TYPE_PLUGIN_COMMAND;
            event->Command = PLUGIN_ID;
            }
          }
        }
      break;
      }

    case PLUGIN_MMI_OUT:
      {
      strcpy(string,PLUGIN_NAME);            // Eerste argument=het commando deel
      strcat(string," ");
      strcat(string,int2str(event->Par1));
      strcat(string,",");
      strcat(string,int2str(event->Par2));
      break;
      }
    #endif //MMI
    }      
  return success;
  }

#ifdef PLUGIN_024_CORE

 /*********************************************************************************************\
  * Deze funktie zoekt in de string [Input] naar de tekst [Start]. Zodra deze is gevonden
  * worden alle geldige tekens die zijn opgegeven in de string [Valid] geplaatst in
  * de string [Result] totdat de een teken zoals opgegeven in [Stop] wordt gevonden.
  * Als [Stop] nul is of verwijst naar een lege string worden alle resterende tekens in
  * result geplaatst. Als [Valid] nul is, worden alle tekens in [Result] geplaatst.
  *
  * Let op dat Result voldoende ruimte bevat om alle tekens in weg te schrijven, anders kans
  * op een vastloper!
  \*********************************************************************************************/

boolean ParseString(char* Input, char* Result, char* Start, char* StopChrs, char* ValidChrs)
  {
  int StartPos=0,StopPos=0;
  int x,y,z;

  byte StartLen=strlen(Start);
  byte InputLen=strlen(Input);
  byte StopLen=strlen(StopChrs);
  
  // in enkele gevallen is parsen zinloos:
  if(InputLen<=StartLen)return false;
  
  // Zoek if de startstring voorkomt in de Input
  for(x=0; x<(InputLen-StartLen); x++)
    {
    z=0;
    for(y=0;y<StartLen;y++)
      if(Start[y]==Input[x+y])
        z++;
      else
        z=0;
    if(z==StartLen)
      StartPos=x+y;    
    }

  
  if(StartPos)
    {    
    // Zoek if de Stopstring voorkomt in de Input  
    if(StopChrs[0]==0 || StopChrs==0)
      StopPos=InputLen;

    else
      for(x=StartPos; x<InputLen && !StopPos; x++)
        for(y=0;y<StopLen;y++)
          if(StopChrs[y]==Input[x+y])
            StopPos=x;
    }

  z=0;
  for(x=StartPos;x<StopPos;x++)
    {
    if(ValidChrs)
      {
      for(y=0;y<strlen(ValidChrs);y++)
        {
        if(Input[x]==ValidChrs[y])
          Result[z++]=Input[x];
        }
      }
    else
      Result[z++]=Input[x];
    }
  Result[z]=0;
    
  return (z>0);
  }
  
#endif //CORE
