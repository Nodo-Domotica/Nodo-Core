//#######################################################################################################
//#################################### Device-255: MyDevice   ############################################
//#######################################################################################################


/*********************************************************************************************\
 * Funktionele beschrijving: Dit is een leeg device dat als voorbeeld is opgenomen. Aan de hand
 *                           van dit voorbeeld kan een gebruiker zelf een egen device ontwikkelen.
 *                           Als je device hebt ontwikkeld die voor anderen ook van nut kan zijn,
 *                           meld deze dan aan bij het Nodo-team om deze te delen met andere gebruikers
 *                           en om een Device-ID aan te vragen.
 * 
 * <Geef hier een beschrijving van de funktionele werking van het device en hoe deze door de gebruiker
 * moet worden aangeroepen.>
 *
 * Auteur             : <naam en EMail adres van de ontwikkelaar>
 * Support            : <website>
 * Datum              : <datum>
 * Versie             : <versie>
 * Nodo productnummer : <Nodo productnummer. Toegekend door Nodo team>
 * Compatibiliteit    : Vanaf Nodo build nummer <build nummer>
 * Syntax             : "MijnDevice <Par1>, <Par2>"
 *
 ***********************************************************************************************
 * Technische beschrijving:
 *
 * Compiled size      : <grootte> bytes voor een Mega en <grootte> voor een Small.
 * Externe funkties   : <geef hier aan welke funkties worden gebruikt. 
 *
 * <Geef hier een technische toelichting op de werking van het device en eventueel gebruikte protocol>
 * 
 * Tips en aandachtspunten voor programmeren van een device:
 * 
 * -  Geheugen is beperkt. Programmeer compact en benut iedere byte RAM en PROGMEM. De Arduino heeft niet de luxe van een PC!
 *    Het is verantwoordelijkheid van de programmeur om te bewaken. Vrij geheugen is opvraagbaar met [Status FreeMem].
 * -  Bouw geen lange wachtlussen in de code. Dit kan leiden tot timings-promlemen waaronder missen van events.
 *    Deze funktie bevindt zich ik een tijdkritische loop. Detecteer zo snel mogelijk of het ontvangen signaal
 *    ook bij dit protocol hoort. 
 * -  De array RawSignal.Pulses[] bevat alle Mark en Space tijden in microseconden. Te beginnen vanaf element [1].
 *    Deze array is reeds gevuld bij aankomst. Element [0] bevat een vermenigvuldigingsfactor voor omrekenen naar
 *    echte microseconden.
 * -  RawSignal.Pulses bevat het aantal posities die een mark/space bevatten. Let op dat de waarde RAW_BUFFER_SIZE
 *    nooit overschreden wordt. Anders gegarandeerd vastlopers! Positie [1] bevat de 'mark' van de startbit. 
 * -  De struct NodoEventStruct bevat alle informatie die nodig is voor verwerking en weergave van het event
 *    dat is ontvangen of moet worden weergegeven. 
 *    of er voldoende geheugen over blijft voor stabiele werking van de Nodo.
 * -  Om uitwisselbaar te blijven met andere Nodo versies en Nodo gebruikers, geen aanpassingen aan de Code maken.
 *    Let op dat je bij gebruik van functies uit de Nodo code je je eigen code gevoelig makt voor onderhoud
 *    bij uitbrengen van nieuwe releases.
 * -  Maak geen gebruik van interrupt driven routines, dit verstoort (mogelijk) de werking van de I2C, Serial en ethernet
 *    communicatie.
 * -  Maak slecht in uitzonderlijke gevallen gebruik van Globals en pas geen waarden van globals uit de Nodo code aan.
 * -  Besteed uitgebreid aandacht aan de documentatie van het protocol. Indien mogelijk verwijzen naar originele
 *    specificatie.
 *
 * Voorbeelden van devices:
 * - Digitale temperatuur sensoren (Zoals Dallas DS18B20)
 * - Digitale vochtigheidssensoren (Zoals DTH-11)
 * - Vergroten van aantal digitale wired met een multiplexer. Tot 8-ingangen per Wired poort met bv. een 74151.
 * - Acht verschillende analoge ingangen meten met eén WiredIn met een LTC1380.
 * - WiredOut uitgangen uitbreiden tot 8, 16, 32, 64 verschillende digitale uitgangen met behulp van 74HCT595
 * - I2C devices aansturen via de SLC en SDA lijnen van de arduino.
 * - etcetera.
 *
 * De Wired poorten en de additionele IO poorten op de Mega in uw eigen code gebruiken aan de hand van de naam zoals deze zijn gedefinieerd
 * in de code:
 *
 * PIN_WIRED_OUT_n staat voor WiredOut poort, waarbij n overeen komt met het nummer van de WiredOut poort te beginnen met 1.
 * PIN_WIRED_IN_n staat voor WiredOut poort, waarbij n overeen komt met het nummer van de WiredOut poort te beginnen met 1.
 * PIN_IO_n staat voor additionele IO-poort, waarbij n overeen komt met het nummer van deze poort, te beginnen met 1 (Alleen beschikbaar op de Mega).
 * 
 * Voor de verwijzing naar de Arduino pinnummers: zie schema of declaraties in tabblad "Base"
 * ANDERE POORTEN NIET GEBRUIKEN OMDAT DEZE (IN DE TOEKOMST) EEN SPECIFIEKE FUNKTIE HEBBEN.
 * Let bij het ontwerp van de Hardware en de software op dat geen gebruik wordt gemaakt van de Arduino pinnen die al een voorgedefinieerde
 * Nodo funktie hebben, De WiredIn en WiredOut kunnen wel vrij worden gebruikt met die kanttekening dat Nodo commando's de lijnen eveneens 
 * Besturen wat kan leiden tot beschadiging van hardware. Let hier in het bijzonder op als een Arduino pin wordt gedefinieerd als een output.
 * Op de Nodo Mega zijn nog vier extra communicatielijnen die gebruikt kunnen worden voor User input/output: PIN_IO_1 t/m PIN_IO_4 (Arduino pin 38 t/m 41)
 * Besef dat niet alle pennen van de Arduino gebruikt kunnen worden daar vele Arduino pinnen al een voorgedefinieerde
 * Nodo funktie hebben, De WiredIn en WiredOut kunnen wel vrij worden gebruikt met die kanttekening dat Nodo commando's de lijnen eveneens 
 * Besturen wat kan leiden tot beschadiging van hardware. Let hier in het bijzonder op als een Arduino pin wordt gedefinieerd als een output.
 * Op de Nodo Mega zijn nog vier extra communicatielijnen die gebruikt kunnen worden voor User input/output: PIN_IO_1 t/m PIN_IO_4 (Arduino pin 38 t/m 41)
 *
 * Ben je klaar met het schrijven van je code, kleed dan de code zoveel mogelijk uit om geheugenruimte te
 * besparen en stroomlijn op performance.
 \*********************************************************************************************/
 

// Ieder device heeft een uniek ID. Deze ID's worden onderhouden door het Nodo team. Als je een device hebt geprogrammeerd
// die van waarde kan zijn voor andere gebruikers, meldt deze dan aan bij het Nodo team zodat deze kan worden meegenomen
// in de Nodo-releases. Device 255 is een "knutsel" device voor de gebruiker.

// Een device heeft naast een uniek ID ook een eigen MMI string die de gebruiker kan invoeren via Telnet, Serial, HTTP 
// of een script. Geef hier de naam op. De afhandeling is niet hoofdletter gevoelig.
#define DEVICE_NAME_255 "MyDevice"
boolean Device_255(byte function, struct NodoEventStruct *event, char *string)
  {
  boolean success=false;

  // Deze device code wordt vanuit meerdere plaatsen in de Nodo code aangeroepen, steeds met een doel. Dit doel bevindt zich
  // in de variabele [function]. De volgende doelen zijn gedefinieerd:
  //
  // DEVICE_RAWSIGNAL_IN  => Afhandeling van een via RF/IR ontvangen event
  // DEVICE_COMMAND       => Commando voor afhandelen/uitsturen van een event.
  // DEVICE_MMI_IN        => Invoer van de gebruiker/script omzetten naar een event. (Alleen voor mega)
  // DEVICE_MMI_OUT       => Omzetten van een event naar een voor de gebruiker leesbare tekst (Alleen voor Mega)
  // DEVIDE_ONCE_A_SECOND => ongeveer iedere seconde.
  // DEVICE_INIT          => Eenmalig, direct na opstarten van de Nodo

  #ifdef DEVICE_255_CORE
  switch(function)
    {    
    case DEVICE_ONCE_A_SECOND:
      {
      // Dit deel van de code wordt (ongeveer!) eenmaal per seconde aangeroepen. Let op dat deze code zich binnen een 
      // tijdkritisch deel van de hoofdloop van de Nodo bevindt! Gebruik dus alleen om snel een waarde te (re)setten
      // of om de status van een poort/variabele te checken. Zolang de verwerking zich hier plaats vindt zal de
      // Nodo géén IR of RF events kunnen ontvangen.
      // Heb je geen taken die periodiek uitgevoerd moten worden, dan mag je deze case weglaten om geheugen te besparen.
      Serial.println(F("*** debug: MyDevice: DEVICE_ONCE_A_SECOND")); //??? Debug
      success=true;
      break;
      }

    case DEVICE_RAWSIGNAL_IN:
      {
      // Code op deze plaats wordt uitgevoerd zodra er een event via RF of IR is binnengekomen
      // De RawSignal buffer is gevuld met pulsen. de struct [event] moet hier worden samengesteld.      
      // Als decoderen is gelukt, dan de variabele [success] vullen met een true. De Nodo zal het 
      // event vervolgens als een regulier event afhandelen.
      // Heb je geen signalen te verwerken, dan mag je deze case weglaten om geheugen te besparen.
      Serial.println(F("*** debug: MyDevice: DEVICE_RAWSIGNAL_IN")); //??? Debug
      success=false; // in dit voorbeeld even op false.
      break;
      }
      
    case DEVICE_COMMAND:
      {
      // Als er vanuit de gebruiker, script of eventlist dit device een event moet uitsturen, dan is op het
      // moment dat de code hier wordt aangeroepen, de struct [event] gevuld en gereed voor verwerking.
      // Als voor verlaten de struct [event] is gevuld met een ander event, dan wordt deze uitgevoerd als een nieuw
      // event. Dit kan bijvoorbeeld worden benut als een variabele wordt uitgelezen en de waarde verder verwerkt
      // moet worden.
      Serial.println(F("*** debug: MyDevice: DEVICE_COMMAND =Hello World!=")); //??? Debug
      success=true;
      break;
      }
      
    case DEVICE_INIT:
      {
      // Code hier wordt eenmalig aangeroepen na een reboot van de Nodo.
      // Heb je initialisatie taken, dan mag je deze case weglaten om geheugen te besparen.
      Serial.println(F("*** debug: MyDevice: DEVICE_INIT")); //??? Debug
      break;
      }
    #endif // CORE
    
    #ifdef DEVICE_255_MMI // alleen relevant voor een Nodo Mega want de Small heeft geen MMI!
    case DEVICE_MMI_IN:
      {
      // Zodra er via een script, HTTP, Telnet of Serial een commando wordt ingevoerd, wordt dit deel van de code langs gelopen.
      // Op deze plek kan de invoer [string] worden geparsed en omgezet naar een struct [event]. Als parsen van de invoerstring [string]
      // is gelukt en de struct is gevuld, dan de variabele [success] vullen met true zodat de Nodo zorg kan dragen voor verdere verwerking van het event.
      Serial.print(F("*** debug: MyDevice: DEVICE_MMI_IN, string="));Serial.println(string); //??? Debug

      char *TempStr=(char*)malloc(26);
      string[25]=0;

      // Met DEVICE_MMI_IN wordt de invoer van de gebruiker (string) omgezet naar een event zodat de Nodo deze intern kan verwerken.
      // Hier aangekomen bevat string het volledige commando. Test als eerste of het opgegeven commando overeen komt met DEVICE_NAME
      // Dit is het eerste argument in het commando. 
      if(GetArgv(string,TempStr,1))
        {
        // Als het door de gebruiker ingegeven ommando/event overeenkomt met de naam van dit device...
        if(strcasecmp(TempStr,DEVICE_NAME_255)==0)
          {
          // Vervolgens tweede parameter gebruiken
          if(GetArgv(string,TempStr,2)) 
            {
            // plaats hier je code die de eerste parameter die zich in [TempStr] bevindt verder uitparsed
            // De byte Par1 en unsigned long Par2 die zich in de struct [event] bevindt kunnen worden gebruikt.
              
            if(GetArgv(string,TempStr,3))
              {
              // indien gewenst kan een tweede parameter worden gebruikt (=derde argument)
              // Plaats wederom de code paar uitparsen van de parameter hier.
              // heb je het laatste parameter geparsen, dan de variabele [success] vullen 
              // met een true zodat verdere verwerking van het event plaats kan vinden.

              // Een device kan worden verwerkt als een commando of een event. Geef dit aan want 
              // op moment van invoer moet de Nodo t.b.v. latere verwerking weten hoe de zaken afgehandeld moeten worden
              event->Type = NODO_TYPE_DEVICE_COMMAND;
              
              // Als success wordt gevuld met true, dan wordt het commando/event
              // geaccepteerd als geldig.
              success=true;
              }
            }
          }
        }
      free(TempStr);
      break;
      }

    case DEVICE_MMI_OUT:
      {
      // De code die zich hier bevindt zorgt er voor dan een event met het unieke ID in de struct [event] weer wordt
      // omgezet naar een leesbaar event voor de gebruiker. het resultaat moet worden geplaatst in de string [string]
      // let op dat het totale commando niet meer dan 25 posities in beslag neemt.
      // Dit deel van de code wordt alleen uitgevoerd door een Nodo Mega, omdat alleen deze over een MMI beschikt.
      Serial.println(F("*** debug: MyDevice: DEVICE_MMI_OUT")); //??? Debug

      strcpy(string,DEVICE_NAME_255);            // Commando 
      strcat(string," ");
      strcat(string,int2str(event->Par1));      // Parameter-1 (8-bit)
      strcat(string,",");
      strcat(string,int2str(event->Par2));      // Parameter-2 (32-bit)

      break;
      }
    #endif //MMI
    }
    
  return success;
  }

