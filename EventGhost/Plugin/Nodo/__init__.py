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

NodoCommandList =  (   
      ('Configuratie', None, None, None),
          ('Reset','Reset de Nodo. Alle instellingen worden teruggezet naar default waarden.',None,None),
          ('Unit','Verander het Unit adres van de Nodo','Unitnummer[1-15]',None),

      ('EventList', None, None,None),
          ('EventlistWrite', 'Schrijf een regel in de Eventlist om een macro samen te stellen.', None,None),
          ('EventlistShow','Laat de EventList zien',None,None),
          ('EventlistErase','Wis de inhoud van de Eventlist',None,None),

      ('KlikAanKlikUit', None, None,None),                                      
          ('SendKAKU','Schakelt een KlikAanKlikUit ontvanger volgens de conventionele codering', 'Adres[A1..P16]','Command[On,Off]'),
          ('KAKU','Klik-Aan-JKlik-Uit event','Adres [A1..P16]','Status [ON,OFF]'),
          ('SendNewKAKU','Schakelt of dimt een KlikAanKlikUit ontvanger volgens de automatische codering','Adres[0-255]','Command[On,Off,1..16]'),
          ('NewKAKU','Klik-Aan-JKlik-Uit event volgens nieuwe, automatische codering','Adres [0..255]','Commando [ON,OFF,1..16]'),

      ('UserEvent', None, None,None),                                      
          ('UserEvent','Event vrij te definieren en gebruiken. Nulwaarde heeft een wildcard functie.', 'Waarde[0..255]','Waarde[0..255]'),
          ('SendUserEvent','Verzend een UserEvent.', 'Waarde[0..255]','Waarde[0..255]'),
          ('SendVarUserEvent','Verzend een Event samengesteld uit twee variabelen.','Variabele [1..15]','Variabele [1..15]'),

      ('Timers', None, None,None),                                      
          ('TimerRandom','Stel de timer in met een willekeurige tijd','Timer nummer. [1..15]','Maximaal aantal minuten. [1...255]'),
          ('TimerReset','Reset timer zonder event te genereren','Timer [0,1...15] 0 = alle',None),
          ('TimerSet','Stel een timer is.','Timer [1..15]','Minuten [1..255]'),
          ('Timer','Timer Event','Timer nummer. [1..15]',None),

      ('Clock', None, None,None),                                      
          ('ClockSync','Zet de klok van de Nodo gelijk aan die van de computer',None,None),
          ('ClockSetDate','Zet handmatig de datum an de Nodo','Dag [1..31]','Maand [1..12]'),
          ('ClockSetDOW','Zet handmatig de dag van de week','Weekdag [1..7] 1=zondag',None),
          ('ClockSetTime','Zet handmatig de tijd van de Nodo','Uren [0..23]','Minuten [0..59]'),
          ('ClockSetYear','Stel handmatig het huidige jaar in','Eeuw [19..21]','Jaar [00..99]'),
          ('ClockSun','Levert match in eventlist op zondag voor opgegeven uren:minuten','Uren [0..23]','Minuten [0..59]'),
          ('ClockMon','Levert match in eventlist op maandag voor opgegeven uren:minuten','Uren [0..23]','Minuten [0..59]'),
          ('ClockTue','Levert match in eventlist op dinsdag voor opgegeven uren:minuten','Uren [0..23]','Minuten [0..59]'),
          ('ClockWed','Levert match in eventlist op woensdag voor opgegeven uren:minuten','Uren [0..23]','Minuten [0..59]'),
          ('ClockThu','Levert match in eventlist op donderdag voor opgegeven uren:minuten','Uren [0..23]','Minuten [0..59]'),
          ('ClockFri','Levert match in eventlist op vrijdag voor opgegeven uren:minuten','Uren [0..23]','Minuten [0..59]'),
          ('ClockSat','Levert match in eventlist op zaterdag voor opgegeven uren:minuten','Uren [0..23]','Minuten [0..59]'),
          ('ClockAll','Levert match in wildcard op voor opgegeven uren:minuten','Uren [0..23]','Minuten [0..59]'),

      ('Variabelen', None, None,None),                                      
          ('Variable','Event treedt op als de inhoud van een variabele wijzigt.','Variabele [1..15]','Waarde [0..255]'),
          ('VariableClear','Wis inhoud van een variabele','Variabele [0,1...15] 0 = alle',None),
          ('VariableDec','Tel een waarde op bij een variabele','Variabele [1..15]','Waarde [1..255]'),
          ('VariableInc','Trek een waarde af van een variabele','Variabele [1..15]','Waarde [1..255]'),
          ('VariableSet','Zet een variabele','Variabele [1..15]','Waarde [0..255]'),
          ('VariableVariable','Vul een variabele met de waarde van een andere variabele','Variabele [1..15]','Variabele [1..15]'),
          ('SendVarUserEvent','Verzend een Event samengesteld uit twee variabelen.','Variabele [1..15]','Variabele [1..15]'),
          ('BreakOnVarEqu','Breekt uitvoer van een macro af als variabele gelijk is aan waarde.','Variabele [1..15]','Waarde [0..255]'),
          ('BreakOnVarLess','Breekt uitvoer van een macro af als variabele minder is dan waarde','Variabele [1..15]','Waarde [0..255]'),
          ('BreakOnVarMore','Breekt uitvoer van een macro af als variabele meer is dan waarde','Variabele [1..15]','Waarde [0..255]'),
          ('BreakOnVarNEqu','Breekt uitvoer van een macro af als variabele ongelijk is aan waarde','Variabele [1..15]','Waarde [0..255]'),

      ('Signaalverwerking IR & RF', None, None,None),                                      
          ('WaitFreeRF','Wacht voordat een RF signaal wordt verzonden op een vrije ether','[Off,Series,All]','Tijd [1..255] x 0.1 sec.'),
          ('RawsignalCopy','Stuurt exact een signaal door tussen RF en IR','[IR2RF, RF2IR]','Seconden [1..255]'),
          ('RawsignalGet','Haal de RAW codes op van het eerstvolgende signaal dat wordt ontvangen',None,None),
          ('RawsignalPut','Verzend een RAW signaal','Pulsetijd, Spacetijd, ...,...,0',None),
          ('ReceiveSettings','geavanceerde instellingen voor analyse van ontvangen signalen','Timeout [1..255] milliseconde','Scherpte [1..255]'),
          ('SendSignal','Verzend de inhoud van de RAW buffer (nogmaals)',None,None),
          ('TransmitSettings','Geef aan naar welke poorten verzonden moet worden.','Poort [IR, RF, IR&RF]',None),

      ('Wired', None, None,None),                                      
          ('VariableWiredAnalog','Vul een variabele met de waarde van een WIRED-IN','Variabele [1..15]','Poort [1..4]'),
          ('WiredAnalog','Lees de analoge waarde van een WIRED-IN poort','Poort [1..4]',None),
          ('WiredIn','Lees de status van een WIRED-IN poort','Poort [1..4]',None),
          ('WiredOut','Zet de status van een WIRED_OUT poort','Poort [1..4]','Status [On,Off] '),
          ('WiredPullup','Stel de pull-up weerstand in van een WIRED-IN poort','Poort [1..4]','Pull-up [Off,On]'),
          ('WiredRange','Stel voor een WIRED-IN poort het analoge bereik in waarbinnen analoge waarden worden gemeten','Poort [1..4]','Bereik [0..4]'),
          ('WiredSmittTrigger','Stel de SmittTrigger in van een WIRED-IN poort','Poort [1..4]','Smitt-trigger [0..255]'),
          ('WiredThreshold','Stel de threshold in van een WIRED-IN poort','Poort [1..4]','Threshold [0..255]'),

      ('Debugging', None, None,None),                                      
          ('SimulateDay','Simuleer de tijd ','[1,7]',None),
          ('Trace','Trace modus voor eenvoudig debuggen','Trace [ON,OFF]','Tijd [ON,OFF]'),

      ('Overig', None, None,None),                                      
          ('NodoCommand', 'Stuur een commando naar de Nodo.', 'Meerdere commandos van elkaar scheiden met een puntkomma.\n\nCommand Par1,Par2',None),
          ('Boot','Event reedt op na een reset',None,None),
          ('Status','Haal de status op van de Nodo','Nodo commando (optioneel)','Parameter-1 behorend bij Nodo commando (optioneel)\n[1..4] voor Wired...\n[1..15] voor TimerSet,VariableSet\n'),
          ('Simulate','Simulate voorkomt dat er via RF wordt verzonden. Handig voor testdoeleinden.','[ON,OFF]',None),
          ('Confirm','Geeft na ontvangst een bevestiging dat het singaal door de Nodo is ontvangen','Confirm [On,Off]',None),
          ('Delay','Wacht','Tijd in seconden [Off, 1..255]','Queue events [On,Off]'),
          ('Sound','Geef geluidsignaal','Signaal. [0..7]','Herhalingen. [1..255]'),

      (None, None, None,None),              
)

#===============================================================================

class NodoCommand(eg.ActionClass):
    # class voor Nodo Command's zonder parameters van de gebruiker
    def __call__(self):
        eg.plugins.NodoSerial.plugin.Send(self.cmd  + ';')

#===============================================================================

class NodoCommandPar1(eg.ActionClass):
    # class voor Nodo Commando's met Parameter-1

    class text:
        DescPar1=""
                
    def __call__(self, Par1=""):
        NodoCommand=self.cmd
        if len(Par1)>0:
            NodoCommand += ' ' + Par1
        else:
            NodoCommand += " 0"
        NodoCommand+=';'
        eg.plugins.NodoSerial.plugin.Send(NodoCommand)

    def GetLabel(self, Par1):
        return self.cmd + ' ' + str(Par1) + ';'

    def Configure(self, Par1=""):
        panel = eg.ConfigPanel(self)
        mainSizer =wx.BoxSizer(wx.VERTICAL)
        Par1Lbl=wx.StaticText(panel, -1, self.text.DescPar1)
        Par1Ctrl=wx.TextCtrl(panel,-1,Par1)
        Par1Ctrl.SetMinSize((20,20))
        mainSizer.Add(Par1Lbl,0,wx.TOP,20)
        mainSizer.Add(Par1Ctrl,0,wx.EXPAND)
        panel.sizer.Add(mainSizer)
        
        while panel.Affirmed():
            panel.SetResult(Par1Ctrl.GetValue())
                

#===============================================================================

class NodoCommandPar2(eg.ActionClass):
    # class voor Nodo Commando's met Parameter-1 en Parameter-2

    class text:
        DescPar1=""
        DescPar2=""
                
    def __call__(self, Par1="", Par2=""):
        NodoCommand=self.cmd
        if len(Par1)>0:
            NodoCommand += ' ' + Par1
        else:
            NodoCommand += " 0"
        if len(Par2)>0:
            NodoCommand += ',' + Par2 + ';'
        else:
            NodoCommand += ",0"
        NodoCommand+=';'
        eg.plugins.NodoSerial.plugin.Send(NodoCommand)

    def GetLabel(self, Par1, Par2):
        return self.cmd + ' ' + str(Par1) + ',' + str(Par2) + ';'

    def Configure(self,Par1="",Par2=""):
        panel = eg.ConfigPanel(self)
        mainSizer =wx.BoxSizer(wx.VERTICAL)
        
        Par1Lbl=wx.StaticText(panel, -1, self.text.DescPar1)
        Par1Ctrl=wx.TextCtrl(panel,-1,Par1)
        Par1Ctrl.SetMinSize((100,20))
        mainSizer.Add(Par1Lbl,0,wx.TOP,20)
        mainSizer.Add(Par1Ctrl,0,wx.EXPAND)
        
        Par2Lbl=wx.StaticText(panel, -1, self.text.DescPar2)
        Par2Ctrl=wx.TextCtrl(panel,-1,Par2)
        Par2Ctrl.SetMinSize((100,20))
        mainSizer.Add(Par2Lbl,0,wx.wx.TOP,20)
        mainSizer.Add(Par2Ctrl,0,wx.EXPAND)        
        
        panel.sizer.Add(mainSizer)
        
        while panel.Affirmed():
            panel.SetResult(Par1Ctrl.GetValue(),Par2Ctrl.GetValue())
                
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
        
class NodoCommandRaw(eg.ActionWithStringParameter):
    def __call__(self, data):
        eg.plugins.NodoSerial.plugin.Send(str(data) + ';')

    def GetLabel(self, Data):
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
        EventlistEventCtrl.SetMinSize((100,20))
        mainSizer.Add(EventlistEventLbl,0,wx.TOP,20)
        mainSizer.Add(EventlistEventCtrl,0,wx.EXPAND)

        EventlistActionLbl=wx.StaticText(panel, -1, self.text.EventlistAction)
        EventlistActionCtrl=wx.TextCtrl(panel,-1,EventlistAction)
        EventlistActionCtrl.SetMinSize((100,20))
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
        group = self        
        for Command, DescriptionCommand, DescriptionPar1, DescriptionPar2 in NodoCommandList:              

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
                
            elif Command == 'NodoCommand':
                 NodoCommandRaw.name = Command
                 NodoCommandRaw.description = DescriptionCommand
                 NodoCommandRaw.parameterDescription = DescriptionPar1
                 NodoCommandRaw.cmd = Command
                 group.AddAction(NodoCommandRaw)
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
                Action.__name__ = Command                
                group.AddAction(Action)

            else:
                # Command zonder parameter
                class Action(NodoCommand):
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
        panel.AddLine("Zorg er voor dat u eerst de Arduino drivers correct heeft geinstalleerd.\n"),
        panel.AddLine("Bij gelijktijdig gebruik van de Serial-plugin, niet dezelfde COM-poort toewijzen.\n\n"),
        panel.AddLine("Poort:", portCtrl)
        while panel.Affirmed():
            panel.SetResult(portCtrl.GetValue())

    # verzenden van het commando en gelijkertijd kijken of er een XOFF verzonden is
    def Send(self, StringToSend=""):
        #verzend de reeks tekens uit het Nodo commando
        print "Serial.write: " + StringToSend
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
              self.TriggerEvent(buffer)
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

#===============================================================================

