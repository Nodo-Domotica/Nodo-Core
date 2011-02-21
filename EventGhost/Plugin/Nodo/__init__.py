# Bugs:
# - 
#
# Idee:
# - Logging naar file
# - borgen dat de Boot altijd plaatsvindt.
# - korte wachttijd na opstarten plugin om Nodo de boor uit te kunnen laten voeren




# This is a part of the EventGhost and the Nodo software.
# Copyright (C) 2011 Paul Tonkes <p.k.tonkes@gmail.com>
# Thanks to Frank van der Strigt <fvdstrigt@hotmail.com>

r"""<rst>
Plugin voor de **Nodo Due** V1.2.x zelfbouw domotica controller.

Kijk op voor meer informatie op de `website`__.

.. image:: Nodo.jpg
   :align: center

**Let op:** Gebruik altijd de plugin-versie die hoort bij de juiste versie van de Nodo!

__ http://members.chello.nl/p.tonkes8/index.html
"""

import eg
import binascii

eg.RegisterPlugin(
    name="Nodo",
    description=__doc__,
    url="http://members.chello.nl/p.tonkes8/index.html",
    author="Paul Tonkes",
    version="Plugin version 0.00 beta",
    canMultiLoad = True,
    createMacrosOnAdd = True,
)

#===============================================================================

CommandsToRemember = ("WiredOut", "WiredIn", "WiredAnalog", "KAKU", "NewKAKU", "Variable", "UserEvent", "VariableSet", "Variable")

NodoCommandList =  (   
      ('Configuratie', None, None, None,0),
          ('Display','Commando: Veranderd de gegevens weergave van SERIAL\nLET OP: kan de werking van de plugin stoppen.\nLees voor meer opties de command-reference!','[Trace,Reset,...]','[ON,OFF]',0),
          ('Reset','Commando: Reset de Nodo. Alle instellingen worden teruggezet naar default waarden.',None,None,0),
          ('Unit','Commando: Verander het Unit adres van de Nodo','Unitnummer[1-15]',None,0),

      ('EventList', None, None,None,0),
          ('EventlistErase','Commando: Wis de inhoud van de Eventlist',None,None,0),
          ('EventlistShow','Commando: Laat de EventList zien',None,None,0),
          ('EventlistWrite', 'Commando: Schrijf een regel in de Eventlist om een macro samen te stellen.', None,None,0),

      ('KlikAanKlikUit', None, None,None,0),                                      
          ('SendKAKU','Commando: Schakelt een KlikAanKlikUit ontvanger volgens de conventionele codering', 'Adres[A1..P16]','Command[On,Off]',1),
          ('KAKU','Event: Klik-Aan-JKlik-Uit event','Adres [A1..P16]','Status [ON,OFF]',1),
          ('SendNewKAKU','Commando: Schakelt of dimt een KlikAanKlikUit ontvanger volgens de automatische codering','Adres[0-255]','Command[On,Off,1..16]',1),
          ('NewKAKU','Event: Klik-Aan-Klik-Uit event volgens nieuwe, automatische codering','Adres [0..255]','Commando [ON,OFF,1..16]',1),

      ('UserEvent', None, None,None,0),                                      
          ('UserEvent','Event: Vrij te definieren en gebruiken. Nulwaarde heeft een wildcard functie.', 'Waarde[0..255]','Waarde[0..255]',1),
          ('SendUserEvent','Commando: Verzend een UserEvent.', 'Waarde[0..255]','Waarde[0..255]',1),
          ('SendVarUserEvent','Commando: Verzend een Event samengesteld uit twee variabelen.','Variabele [1..15]','Variabele [1..15]',1),

      ('Timers', None, None,None,0),                                      
          ('TimerRandom','Commando: Stel de timer in met een willekeurige tijd','Timer nummer. [1..15]','Maximaal aantal minuten. [1...255]',1),
          ('TimerSetMin','Commando: Stel een timer in. Timer 0 = alle timers.','Timer [0..15]','Minuten [0..255]',1),
          ('TimerSetSec','Commando: Stel een timer in. Timer 0 = alle timers.','Timer [0..15]','Seconden [0..255]',1),
          ('Timer','Event: Timer Event','Timer nummer. [1..15]',None,1),

      ('Clock', None, None,None,0),                                      
          ('ClockSync','Commando: Zet de klok van de Nodo gelijk aan die van de computer',None,None,1),
          ('ClockSetDate','Commando: Zet handmatig de datum an de Nodo','Dag [1..31]','Maand [1..12]',1),
          ('ClockSetDOW','Commando: Zet handmatig de dag van de week','Weekdag [1..7] 1=zondag',None,1),
          ('ClockSetTime','Commando: Zet handmatig de tijd van de Nodo','Uren [0..23]','Minuten [0..59]',1),
          ('ClockSetYear','Commando: Stel handmatig het huidige jaar in','Eeuw [19..21]','Jaar [00..99]',1),
          ('ClockSun','Event: Levert match in eventlist op zondag voor opgegeven uren:minuten','Uren [0..23]','Minuten [0..59]',1),
          ('ClockMon','Event: Levert match in eventlist op maandag voor opgegeven uren:minuten','Uren [0..23]','Minuten [0..59]',1),
          ('ClockTue','Event: Levert match in eventlist op dinsdag voor opgegeven uren:minuten','Uren [0..23]','Minuten [0..59]',1),
          ('ClockWed','Event: Levert match in eventlist op woensdag voor opgegeven uren:minuten','Uren [0..23]','Minuten [0..59]',1),
          ('ClockThu','Event: Levert match in eventlist op donderdag voor opgegeven uren:minuten','Uren [0..23]','Minuten [0..59]',1),
          ('ClockFri','Event: Levert match in eventlist op vrijdag voor opgegeven uren:minuten','Uren [0..23]','Minuten [0..59]',1),
          ('ClockSat','Event: Levert match in eventlist op zaterdag voor opgegeven uren:minuten','Uren [0..23]','Minuten [0..59]',1),
          ('ClockAll','Event: Levert match in wildcard op voor opgegeven uren:minuten','Uren [0..23]','Minuten [0..59]',1),

      ('Variabelen', None, None,None,0),                                      
          ('BreakOnVarEqu','Eventlist commando: Breekt uitvoer van een macro af als variabele gelijk is aan waarde.','Variabele [1..15]','Waarde [0..255]',1),
          ('BreakOnVarLess','Eventlist commando: Breekt uitvoer van een macro af als variabele minder is dan waarde','Variabele [1..15]','Waarde [0..255]',1),
          ('BreakOnVarMore','Eventlist commando: Breekt uitvoer van een macro af als variabele meer is dan waarde','Variabele [1..15]','Waarde [0..255]',1),
          ('BreakOnVarNEqu','Eventlist commando: Breekt uitvoer van een macro af als variabele ongelijk is aan waarde','Variabele [1..15]','Waarde [0..255]',1),
          ('Variable','Event: treedt op als de inhoud van een variabele wijzigt.','Variabele [1..15]','Waarde [0..255]',1),
          ('VariableDec','Commando: Tel een waarde op bij een variabele','Variabele [1..15]','Waarde [1..255]',1),
          ('VariableInc','Commando: Trek een waarde af van een variabele','Variabele [1..15]','Waarde [1..255]',1),
          ('VariableSet','Commando: Zet een variabele. Variabele 0 = alle variabelen.','Variabele [0..15]','Waarde [0..255]',1),
          ('VariableVariable','Commando: Vul een variabele met de waarde van een andere variabele','Variabele [1..15]','Variabele [1..15]',1),
          ('SendVarUserEvent','Commando: Verzend een Event samengesteld uit twee variabelen.','Variabele [1..15]','Variabele [1..15]',1),

      ('Signaalverwerking IR & RF', None, None,None,0),                                      
          ('ReceiveSettings','Commando: geavanceerde instellingen voor analyse van ontvangen signalen','Timeout [1..255] milliseconde','Scherpte [1..255]',1),
          ('RawsignalCopy','Commando: Stuurt exact een signaal door tussen RF en IR','[IR2RF, RF2IR]','Seconden [1..255]',1),
          ('RawsignalGet','Commando: Haal de RAW codes op van het eerstvolgende signaal dat wordt ontvangen',None,None,1),
          ('RawsignalPut','Commando: Verzend een RAW signaal','Pulsetijd, Spacetijd, ...,...,0',None,1),
          ('SendSignal','Commando: Verzend de inhoud van de RAW buffer (nogmaals)',None,None,1),
          ('TransmitSettings','Commando: Geef aan naar welke poorten verzonden moet worden.','Poort [IR, RF, IR&RF]',None,1),
          ('WaitFreeRF','Commando: Wacht voordat een RF signaal wordt verzonden op een vrije ether','[Off,Series,All]','Tijd [1..255] x 0.1 sec.',1),

      ('Wired', None, None,None,0),                                      
          ('VariableWiredAnalog','Commando: Vult een variabele met de waarde van een WIRED-IN','Variabele [1..15]','Poort [1..4]',1),
          ('WiredAnalog','Commando: Lees de analoge waarde van een WIRED-IN poort','Poort [1..4]',None,1),
          ('WiredIn','Commando: Lees de status van een WIRED-IN poort','Poort [1..4]',None,1),
          ('WiredOut','Commando: Zet de status van een WIRED_OUT poort','Poort [1..4]','Status [On,Off] ',1),
          ('WiredPullup','Commando: Stel de pull-up weerstand in van een WIRED-IN poort','Poort [1..4]','Pull-up [Off,On]',1),
          ('WiredRange','Commando: Stel voor een WIRED-IN poort het analoge bereik in waarbinnen analoge waarden worden gemeten','Poort [1..4]','Bereik [0..4]',1),
          ('WiredSmittTrigger','Commando: Stel de SmittTrigger in van een WIRED-IN poort','Poort [1..4]','Smitt-trigger [0..255]',1),
          ('WiredThreshold','Commando: Stel de threshold in van een WIRED-IN poort','Poort [1..4]','Threshold [0..255]',1),

      ('Debugging', None, None,None,0),                                      
          ('Simulate','Commando: Simulate voorkomt dat er via RF wordt verzonden. Handig voor testdoeleinden.','[ON,OFF]',None,0),
          ('SimulateDay','Commando: Simuleer de tijd ','[1,7]',None,0),

      ('Overig', None, None,None,0),                                      
          ('Boot','Event: Event reedt op na een reset',None,None,1),
          ('Delay','Commando: Wacht','Tijd in seconden [Off, 1..255]','Queue events [On,Off]',1),
          ('Confirm','Commando: Geeft na ontvangst een bevestiging dat het singaal door de Nodo is ontvangen','Confirm [On,Off]',None,1),
          ('RawCommand', 'Commando: Stuur een commando naar de Nodo.', 'Meerdere commandos van elkaar scheiden met een puntkomma.\n\nCommand Par1,Par2',None,0),
          ('Sound','Commando: Geef geluidsignaal','Signaal. [0..7]','Herhalingen. [1..255]',1),
          ('Status','Commando: Haal de status op van de Nodo','Nodo commando (optioneel)','Parameter-1 behorend bij Nodo commando (optioneel)\n[1..4] voor Wired...\n[1..15] voor TimerSet,VariableSet\n',1),

      (None, None, None,None,0),              
)

#===============================================================================

class NodoCommandNoDivert(eg.ActionClass):
    # class voor Nodo Command's zonder parameters van de gebruiker
    def __call__(self, Divert=0):
        eg.plugins.NodoSerial.plugin.Send(self.cmd  + ';')

    def GetLabel(self, ):
        return self.cmd + ';'

#===============================================================================
                        
class NodoCommand(eg.ActionClass):
    # class voor Nodo Command's zonder parameters van de gebruiker die wel ge-divert kan worden
    class text:
        Diverting = ""

    def __call__(self, Divert=0):
        if Divert!=0:
            NodoCommand = "Divert " + str(Divert) + ";"
        else:
            NodoCommand=""
            
        NodoCommand+=self.cmd
        eg.plugins.NodoSerial.plugin.Send(NodoCommand  + ';')

    def GetLabel(self, Divert=0):
        if Divert!=0:
            return "Divert " + str(Divert) + "; " + self.cmd + ';'
        else:
            return self.cmd + ';'

    def Configure(self, Divert=0):
        panel = eg.ConfigPanel(self)

        if self.text.Diverting=="0":
            DivertCtrl = panel.SpinIntCtrl(Divert, 0, 15)        
            DivertSettingsBox = panel.BoxedGroup(
                "Divert",
                ("Nodo unit: ", DivertCtrl)
            )
            eg.EqualizeWidths(DivertSettingsBox.GetColumnItems(0))
            panel.sizer.Add(DivertSettingsBox)

            while panel.Affirmed():
                panel.SetResult(DivertCtrl.GetValue())
                        
#===============================================================================

class NodoCommandPar1(eg.ActionClass):
    # class voor Nodo Commando's met Parameter-1

    class text:
        DescPar1=""
        Diverting = ""

    def __call__(self, Par1="0", Divert=0):
        if Divert!=0:
            NodoCommand = "Divert " + str(Divert) + ";"
        else:
            NodoCommand=""

        NodoCommand+=self.cmd
        
        if len(Par1)>0:
            NodoCommand += ' ' + Par1
        else:
            NodoCommand += " 0"
        NodoCommand+=';'
        eg.plugins.NodoSerial.plugin.Send(NodoCommand)

    def GetLabel(self, Par1="", Divert=0):
        if Divert!=0:
            return "Divert " + str(Divert) + "; " + self.cmd + ' ' + str(Par1) + ';'
        else:
            return self.cmd + ' ' + str(Par1) + ';'

    def Configure(self, Par1="", Divert=1):
        panel = eg.ConfigPanel(self)
        mainSizer =wx.BoxSizer(wx.VERTICAL)

        Par1Ctrl=wx.TextCtrl(panel,-1,Par1)
        Par1Ctrl.SetMinSize((75,20))
        ParBox = panel.BoxedGroup( "Parameters: ", 
                                   (self.text.DescPar1, Par1Ctrl)
                                 )
        panel.sizer.Add(ParBox)

        if self.text.Diverting=="1":
            Textlbl=wx.StaticText(panel, -1, " ")
            eg.EqualizeWidths(ParBox.GetColumnItems(0))
            DivertCtrl = panel.SpinIntCtrl(Divert, 0, 15)        
            DivertBox = panel.BoxedGroup( "Divert: ",
                                        ( "Nodo unit: ", DivertCtrl)
                                         )
            eg.EqualizeWidths(DivertBox.GetColumnItems(0))
            panel.sizer.Add(Textlbl)
            panel.sizer.Add(DivertBox)

            while panel.Affirmed():
                panel.SetResult(Par1Ctrl.GetValue(), DivertCtrl.GetValue())
        else:
            while panel.Affirmed():
                panel.SetResult(Par1Ctrl.GetValue())
                

#===============================================================================

class NodoCommandPar2(eg.ActionClass):
    # class voor Nodo Commando's met Parameter-1 en Parameter-2

    class text:
        DescPar1=""
        DescPar2=""
        Diverting = ""
                
    def __call__(self, Par1="0", Par2="0", Divert=0):
        if Divert!=0:
            NodoCommand = "Divert " + str(Divert) + ";"
        else:
            NodoCommand=""

        NodoCommand+=self.cmd

        if len(Par1)>0:
            NodoCommand += ' ' + Par1
        else:
            NodoCommand += " 0"
        if len(Par2)>0:
            NodoCommand += ',' + Par2 + ';'
        else:
            NodoCommand += ",0;"
        eg.plugins.NodoSerial.plugin.Send(NodoCommand)

    def GetLabel(self, Par1="0", Par2="0", Divert=0):
        if Divert!=0:
            return "Divert " + str(Divert) + "; " + self.cmd + ' ' + str(Par1) + ',' + str(Par2) + ';'
        else:
            return self.cmd + ' ' + str(Par1) + ',' + str(Par2) + ';'

    def Configure(self, Par1="", Par2="", Divert=0):
        panel = eg.ConfigPanel(self)
        mainSizer =wx.BoxSizer(wx.VERTICAL)

        Par1Ctrl=wx.TextCtrl(panel,-1,Par1)
        Par1Ctrl.SetMinSize((75,20))
        Par2Ctrl=wx.TextCtrl(panel,-1,Par2)
        Par2Ctrl.SetMinSize((75,20))
        ParBox = panel.BoxedGroup( "Parameters: ", 
                                   (self.text.DescPar1, Par1Ctrl),
                                   (self.text.DescPar2, Par2Ctrl)
                                 )
        eg.EqualizeWidths(ParBox.GetColumnItems(0))
        panel.sizer.Add(ParBox)

        if self.text.Diverting=="1":
            Textlbl=wx.StaticText(panel, -1, " ")
            DivertCtrl = panel.SpinIntCtrl(Divert, 0, 15)        
            DivertBox = panel.BoxedGroup( "Divert: ",
                                        ( "Nodo unit: ", DivertCtrl)
                                         )
            eg.EqualizeWidths(DivertBox.GetColumnItems(0))
            panel.sizer.Add(Textlbl)
            panel.sizer.Add(DivertBox)

            while panel.Affirmed():
                panel.SetResult(Par1Ctrl.GetValue(), Par2Ctrl.GetValue(), DivertCtrl.GetValue())
        else:
            while panel.Affirmed():
                panel.SetResult(Par1Ctrl.GetValue(), Par2Ctrl.GetValue())
                    
#===============================================================================

class ClockSync(eg.ActionClass):

    def __call__(self):
        from datetime import datetime
        t = datetime.now ()
        eg.plugins.NodoSerial.plugin.Send("ClockSetYear " + str(t.year)[:2] + " " + str(t.year)[2:] + ";")
        eg.plugins.NodoSerial.plugin.Send("ClockSetDate " + str(t.day) + " " + str(t.month) + ";")
        eg.plugins.NodoSerial.plugin.Send("ClockSetTime " + str(t.hour) + " " + str(t.minute) + ";")
        # Nodo: 1=zondag, 7=zaterdag
        # function weekday: 0=maandag 6=zondag
        DOW = (t.weekday() + 2) % 7
        if DOW == 0:
            DOW = 7   # zaterdag!
        eg.plugins.NodoSerial.plugin.Send("ClockSetDow " + str(DOW) + ";")
        
#===============================================================================
          
class RawCommand(eg.ActionWithStringParameter):
    def __call__(self, data):
        eg.plugins.NodoSerial.plugin.Send(str(data) + ';')

    def GetLabel(self, data):
        return str(data) + ';'
        
#===============================================================================
        
class EventlistWrite(eg.ActionClass):
    class text:
        EventlistEvent="Als onderstaand event zich voordoet...\n\nEvent Par1,Par2"
        EventlistAction="...dan de volgende actie uitvoeren\n\nAction Par1,Par2"
        
    def GetLabel(self, EventlistAction, EventlistEvent):
        return "EventlistWrite; " + str(EventlistEvent) + '; ' + str(EventlistAction) + ';'

    def __call__(self, EventlistAction, EventlistEvent):
        eg.plugins.NodoSerial.plugin.Send("EventlistWrite;" + str(EventlistEvent) + ';' + str(EventlistAction) + ';' )

    def Configure(self,EventlistAction="",EventlistEvent=""):
        panel = eg.ConfigPanel(self)
        mainSizer =wx.BoxSizer(wx.VERTICAL)

        EventlistEventLbl=wx.StaticText(panel, -1, self.text.EventlistEvent)
        EventlistEventCtrl=wx.TextCtrl(panel,-1,EventlistEvent)
        EventlistEventCtrl.SetMinSize((75,20))
        mainSizer.Add(EventlistEventLbl,0,wx.TOP,20)
        mainSizer.Add(EventlistEventCtrl,0,wx.EXPAND)

        EventlistActionLbl=wx.StaticText(panel, -1, self.text.EventlistAction)
        EventlistActionCtrl=wx.TextCtrl(panel,-1,EventlistAction)
        EventlistActionCtrl.SetMinSize((75,20))
        mainSizer.Add(EventlistActionLbl,0,wx.wx.TOP,20)
        mainSizer.Add(EventlistActionCtrl,0,wx.EXPAND)        

        panel.sizer.Add(mainSizer)
        while panel.Affirmed():
            panel.SetResult(EventlistActionCtrl.GetValue(),EventlistEventCtrl.GetValue())

#===============================================================================

class NodoSerial(eg.PluginClass):
    def __init__(self):        
        self.serial = None
        eg.globals.hold = 0
        eg.globals.Unit = "?"
        eg.globals.ThisUnit = "1"
        
        group = self        
        for Command, DescriptionCommand, DescriptionPar1, DescriptionPar2, Dvrt in NodoCommandList:              
            if DescriptionCommand is None:
                # Nieuwe subgroep of terug naar vorig niveau
                if Command is None:
                    group = self
                else:
                    group = self.AddGroup(Command)                    
                continue
                                
            # er zijn een aantal commando's met een separate, elders gedefinieerde class
            if Command == 'EventlistWrite':
                 EventlistWrite.name = Command
                 EventlistWrite.description = DescriptionCommand
                 EventlistWrite.cmd = Command
                 group.AddAction(EventlistWrite)        
                 continue
                
            elif Command == 'RawCommand':
                 RawCommand.name = Command
                 RawCommand.description = DescriptionCommand
                 RawCommand.parameterDescription = DescriptionPar1
                 RawCommand.cmd = Command
                 group.AddAction(RawCommand)
                 continue

            elif Command == 'ClockSync':
                 ClockSync.name = Command
                 ClockSync.description = DescriptionCommand
                 ClockSync.cmd = Command
                 group.AddAction(ClockSync)
                 continue
            
            # voeg de overige standaard Nodo commandos toe                            
            if DescriptionPar1 is not None and DescriptionPar2 is None:
                # Commando met parameter-1                                    
                class Action(NodoCommandPar1):
                    name = Command
                    description = DescriptionCommand
                    cmd = Command
                Action.text.Diverting = str(Dvrt)
                Action.text.DescPar1 = DescriptionPar1                
                Action.__name__ = Command                
                group.AddAction(Action)

            elif DescriptionPar1 is not None and DescriptionPar2 is not None:
                # Commando met parameter-1 en parameter-2                                                    
                class Action(NodoCommandPar2):
                    name = Command
                    description = DescriptionCommand
                    cmd = Command
                                    
                Action.text.DescPar1 = DescriptionPar1
                Action.text.DescPar2 = DescriptionPar2                                
                Action.text.Diverting = str(Dvrt)
                Action.__name__ = Command                
                group.AddAction(Action)

            else:
                # Command zonder parameter
                if(Dvrt=="1"):
                    class Action(NodoCommand):
                        name = Command
                        cmd = Command
                        description = DescriptionCommand
                    Action.text.Diverting = str(Dvrt)

                else:
                    class Action(NodoCommandNoDivert):
                        name = Command
                        cmd = Command
                        description = DescriptionCommand

                Action.__name__ = Command
                group.AddAction(Action)

    def __start__(self, port):
        self.port = port
        self.serialThread = eg.SerialThread()
        self.serialThread.SetReadEventCallback(self.OnReceive)
        self.serialThread.Open(port, 19200)
        self.serialThread.SetRts()
        self.serialThread.Start()

    def __stop__(self):
        self.serialThread.Close()
 
    def Configure(self, port=0):
        panel = eg.ConfigPanel(self)
        portCtrl = panel.SerialPortChoice(port)
        panel.AddLine("Zorg er voor dat eerst de Arduino drivers correct zijn geinstalleerd.\n"),
        panel.AddLine("Bij gelijktijdig gebruik van andere plugins, niet dezelfde COM-poort toewijzen.\n\n"),
        panel.AddLine("Poort:", portCtrl)
        while panel.Affirmed():
            panel.SetResult(portCtrl.GetValue())

    # verzenden van het commando en gelijkertijd kijken of er een XOFF verzonden is
    def Send(self, StringToSend=""):
        #verzend de reeks tekens uit het Nodo commando
        print "< " + StringToSend
        for i in range(len(StringToSend)):        
            if eg.globals.hold == 1:
                while eg.globals.hold == 1:
                    # hier een timeout inbouwen ???
                    x = 0
            self.serialThread.Write(StringToSend[i])                                
    
    def OnReceive(self, serial):
      buffer = ""
      while True:
          b = serial.Read(1, 0.2)
          if b == "\n":
              eg.globals.Line = buffer
              self.ParseReceivedLine(buffer)
              self.info.eventPrefix = "Unit" + str(eg.globals.Unit)
              print "> " + buffer
              if eg.globals.Event !="" and (eg.globals.Direction=="Input" or eg.globals.Direction=="Internal"):
                self.TriggerEvent(eg.globals.Event)
              return
              
          elif b == "":
              # Niets ontvangen binnen timeout
              return
              
          elif b == chr(17):
              # XON
              eg.globals.hold = 0
              continue

          elif b == chr(19):
              # XOFF
              eg.globals.hold = 1
              continue
                         
          buffer += b

    def ParseReceivedLine(self, ReceivedString):

            # parse de ontvangen Nodo regel en plaats de delen in eg.globals voor later gebruik door de user     
            TagsToParse = ["ThisUnit", "Simulate=", "Direction=", "Unit=", "NodoVersion=", "Source="]

            # Zoek in de binnengekomen regels naar de Tags en haal bijbehorende waarde er uit.
            # Plaats deze vervolgens in de globale namespace eg.globals            
            for Tag in TagsToParse:
                x = ReceivedString.find(Tag, 0, len(ReceivedString))
                if x!=-1:
                    x+=len(Tag)
                    y = ReceivedString.find(",", x, len(ReceivedString))
                    eg.globals.__setattr__(Tag.strip("= "),ReceivedString[x:y].strip(", ()")) 
                      

            # Event is een ander geval. Hier de afzonderlijke delen uitparsen
            Tag="Event="
            x = ReceivedString.find(Tag, 0, len(ReceivedString))
            if x!=-1:
                x+=len(Tag)
                y = ReceivedString.find(")", x, len(ReceivedString))
                eg.globals.Event=ReceivedString[x:y].strip("()") 
            else:
                eg.globals.Event = ""
                      
            # Zoek Command commando deel
            Result = eg.globals.Event.partition(" ")
            eg.globals.Command = Result[0]
            # Zoek Par1 en Par2
            Params = Result[2].partition(",")
            eg.globals.Par1=str(Params[0])
            eg.globals.Par2=str(Params[2])

            # Van enkele events/commando's is het handig om de waarde te onthouden als globale variabele in eg.globals
            # Zoek in de binnengekomen regels naar de Tags en haal bijbehorende waarde er uit.
            # Plaats deze vervolgens in de globale namespace eg.globals            
            for cmd in CommandsToRemember:
                if eg.globals.Command == cmd: 
                    eg.globals.__setattr__("Unit" + eg.globals.Unit + "_" + cmd + "_" + str(eg.globals.Par1),eg.globals.Par2) 

            return

#===============================================================================

