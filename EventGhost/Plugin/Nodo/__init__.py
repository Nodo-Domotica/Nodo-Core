# Bugs:
# -  sendvaruserevent niet divertable
#
# Idee:
# - Logging naar file
# - borgen dat de Boot altijd plaatsvindt.
# - korte wachttijd na opstarten plugin om Nodo de boot uit te kunnen laten voeren




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
    version="Plugin version 0.02 beta",
    canMultiLoad = False,
    createMacrosOnAdd = True,
)

#===============================================================================

CommandsToRemember = ("WiredOut", "WiredIn", "WiredAnalog", "KAKU", "NewKAKU", "Variable", "UserEvent")
TagsToRemember = ["ThisUnit", "Simulate", "Direction", "Unit", "Version", "Source"]


NodoCommandList =  (   
      # Commando, Par1-label, Par1-bereik, Par2-label, Par2-bereik, Beschrijving, Divertable
      ('System','',[''],'',[''],'',0,),
          ('Status','Nodo commando',['ClockDaylight','ClockSetDate','ClockSetDOW','ClockSetTime','ClockSetYear','TimerSetMin','Boot','Simulate','UserEvent','Confirm','Unit','WaitFreeRF','ReceiveSettings','TransmitSettings','All','WiredAnalog','WiredIn','WiredOut','WiredPullup','WiredRange','WiredSmittTrigger','WiredThreshold'],'Parameter-1 van Nodo commando',['0..15'],'Opvragen van settings zoals opgeslagen in de Nodo',1,),
          ('Reset','',[''],'',[''],'Nodo terugzetten naar fabrieksinstelling. ALLE SETTING WORDEN GEWIST!',0,),

      ('EventList','',[''],'',[''],'',0,),
          ('EventlistErase','',[''],'',[''],'Wis de EventList',0,),
          ('EventlistWrite','',[''],'',[''],'Schrijf een regel naar de EventList',0,),
          ('EventlistShow','',[''],'',[''],'Laat de inhoud van de Eventlist zien.',0,),

      ('Klik-Aan-Klik-Uit','',[''],'',[''],'',0,),
          ('SendKAKU','Adres',['KAKU'],'Status',['On','Off'],'Verzend een Klik-Aan-Klik-Uit signaal volgens conventionele codering.',1,),
          ('KAKU','Adres',['KAKU'],'Status',['On','Off'],'Ontvangen Klik-Aan-Klik-Uit signaal.',1,),
          ('SendNewKAKU','Adres',['0..255'],'Commando',['On','Off','1','2','3','4','5','6','7','8','9','10','11','12','13','14','15','16'],'Verzend een Klik-Aan-Klik-Uit signaal volgens de automatische codering.',1,),
          ('NewKAKU','Adres',['0..255'],'Commando',['On','Off','1','2','3','4','5','6','7','8','9','10','11','12','13','14','15','16'],'Ontvangen Klik-Aan-Klik-Uit signaal volgens de automatische codering.',1,),

      ('Wired','',[''],'',[''],'',0,),
          ('WiredIn','Poort',['1..4'],'Waarde',['0','1'],'Wired-in status',1,),
          ('WiredOut','Poort',['1..4'],'Status',['On','Off'],'Wired-out status',1,),
          ('WiredPullup','Poort',['1..4'],'Status',['On','Off'],'Activeer de interne pull-up weerstand.',1,),
          ('WiredRange','Poort',['1..4'],'Bereik',['1..4'],'Stel het analoge meetbereik is van een Wired-in poort.',1,),
          ('WiredSmittTrigger','Poort',['1..4'],'Waarde',['0..255'],'Stel de SmittTrigger waarde in voor een Wired-In poort.',1,),
          ('WiredThreshold','Poort',['1..4'],'Waarde',['0..255'],'Stel de Threshold waarde in voor een Wired-In poort',1,),

      ('Tijd','',[''],'',[''],'',0,),
          ('ClockSync','',[''],'',[''],'Zet de klok van de nodo gelijk aan de klok van de computer.',1,),
          ('ClockDaylight','Status',['0..4'],'',[''],'Gegenereert event dat plaats vindt gekoppeld aan de zonsopkomst en -ondergangsmomenten.',1,),
          ('ClockSetDate','Dag',['1..31'],'Maand',['1..12'],'Verander de datum van de klok in de Nodo',1,),
          ('ClockSetDOW','Weekdag (1=zondag)',['1..7'],'',[''],'Verander de dag van de week in de klok in de Nodo',1,),
          ('ClockSetTime','Uren',['0..23'],'Minuten',['0..59'],'Verander de tijd van de klok in de Nodo',1,),
          ('ClockSetYear','Eeuw',['19..21'],'Jaar',['0..99'],'Verander het jaartal van de klok in de Nodo',1,),
          ('DaylightSaving','',[''],'',[''],'Dit event treed op als de Nodo een omschakeling tussen zomertijd / wintertijd heeft uitgevoerd.',1,),
          ('Delay','Seconden',['0..255'],'Queue',['On','Off'],'Wacht het opgegeven aantal seconden met verwerking. Parameter-1 is de tijd in seconden.',1,),
          ('Simulate','Status',['On','Off'],'',[''],'Simulatie modus. Verzend geen RF signalen.',0,),
          ('SimulateDay','Dagen',['1','7'],'',[''],'Simuleert tijd om de werking van de Eventlist te testen.',0,),
          ('Timer','Timer',['1..15'],'',[''],'Event dat zich voordoet als een timer is afgelopen.',1,),
          ('TimerRandom','Timer',['1..15'],'Max. minuten',['1..255'],'Zet timer op een willekeurige tijd.',1,),
          ('TimerSetMin','Timer',['1..15'],'Minuten',['0..255'],'Stel een timer in (minuten)',1,),
          ('TimerSetSec','Timer',['1..15'],'Seconden',['0..255'],'Stel een timer in (seconden)',1,),

      ('Multi Nodo','',[''],'',[''],'',0,),
          ('Confirm','Confirm',['On','Off'],'',[''],'Verzend ontvangstbevestiging bij binnenkomst van een Event.',1,),
          ('Divert','Unit [1..15]',['1..15'],'',[''],'Stuur eerstvolgende Event/Commando door naar andere Nodo.',0,),
          ('Unit','Unit',['1..15'],'',[''],'Stel Nodo unitnummer in.',0,),

      ('Signaal verwerking','',[''],'',[''],'',0,),
          ('RawsignalCopy','Richting',['ir2rf','rf2ir'],'Seconden',['0..255'],'Verzend een binnengekomen RF/IR signaal direct naar IR/RF.',1,),
          ('RawsignalGet','',[''],'',[''],'Wacht op een IR of RF event en geeft het signaal dat zich vervolgens in de buffer bevindt. De reeks met pulsen kan worden gebruikt om later met RawsignalPut weer te verzenden.',0,),
          ('RawsignalPut','Pulsetijd, Spacetijd, ...,...,0',[''],'',[''],'Verzend een RAW pulsreeks',0,),
          ('ReceiveSettings','Timeout ( milliseconde)',['0..255'],'Scherpte',['0..255'],'Settings analyse van op IR of RF binnengekomen RAW signalen aanpassen.',1,),
          ('SendSignal','',[''],'',[''],'Verzend huidige inhoud van de RAW signaalbuffer.',1,),
          ('TransmitSettings','Poort',['ir2rf','rf2ir'],'',[''],'Instellen van de settings voor  verzenden van Events.',1,),
          ('WaitFreeRF','Pause ( x 100mSec.)',['0..255'],'Window ( x 100mSec.)',['0..255'],'Stel wachten op vrije 433Mhz. ether in.',1,),

      ('Variabelen','',[''],'',[''],'',0,),
          ('Variable','Variabele',['1..15'],'Waarde',['0..255'],'Event gegenereerd door de Nodo naar aanleiding van het wijzigen van een variabele.',1,),
          ('VariableDec','Variabele',['1..15'],'Waarde',['1..255'],'Verlaagt de variabele zoals aangegeven met parameter-1 met de waarde parameter-2. ',1,),
          ('VariableInc','Variabele',['1..15'],'Waarde',['1..255'],'Verhoogt de variabele zoals aangegeven met parameter-1 met de waarde parameter-2. ',1,),
          ('VariableSet','Variabele (0=Wildcard)',['1..15'],'Waarde',['0..255'],'Ken een waarde toe aan een variabele.',1,),
          ('VariableVariable','Variabele',['1..15'],'Variabele',['1..15'],'Ken aan een variabele de waarde van een andere variabele toe.',1,),
          ('VariableWiredAnalog','Variabele',['1..15'],'Poort',['1..4'],'Variabele zoals opgegeven met parameter-1 gelijk maken aan de analoge waarde van een WIRED_IN poort.',1,),

      ('Diversen','',[''],'',[''],'',0,),
          ('SendVarUserEvent','Variabele',['1..15'],'Variabele',['1..15'],'Verzendt een UserEvent zoals het commando SendUserEvent, echter met de inhoud van een variabele',0,),
          ('UserEvent','Waarde',['0..255'],'Waarde (0=Wildcard)',['0..255'],'UserEvent is een type event dat vrij door de gebruiker kan worden ingezet voor eigen doeleinden.',1,),
          ('SendUserEvent','Waarde',['0..255'],'Waarde',['0..255'],'Verzend een UserEvent naar de poorten zoals ingesteld met TransmitSettings.',1,),
          ('Sound','Signaal',['0..7'],'Herhalingen',['0..255'],'Geeft een geluidssignaal.',1,),
          ('Display','View',['All','Direction','Source','Unit','Timestamp','Trace', 'Tag', 'Serial', 'Reset'],'Status',['On','Off'],'Hiermee kunt u instellen hoe binnengekomen en verzonden events worden weergegeven.',0,),
          ('Boot','',[''],'',[''],'Dit is een event dat altijd eenmalig plaatsvindt na opstarten van de Nodo.',1,),
          ('RawCommand','Commando',[''],'',[''],'Vrij in te geven Nodo commando. Als er meer commando s worden opgegeven, deze dan van elkaar scheiden met een puntkomma.',0,),

)

#===============================================================================

class KAKU:
  addresses = ['A1','A2','A3','A4','A5','A6','A7','A8','A9','A10','A11','A12','A13','A14','A15','A16',
               'B1','B2','B3','B4','B5','B6','B7','B8','B9','B10','B11','B12','B13','B14','B15','B16',
               'C1','C2','C3','C4','C5','C6','C7','C8','C9','C10','C11','C12','C13','C14','C15','C16',
               'D1','D2','D3','D4','D5','D6','D7','D8','D9','D10','D11','D12','D13','D14','D15','D16',
               'E1','E2','E3','E4','E5','E6','E7','E8','E9','E10','E11','E12','E13','E14','E15','E16',
               'F1','F2','F3','F4','F5','F6','F7','F8','F9','F10','F11','F12','F13','F14','F15','F16',
               'G1','G2','G3','G4','G5','G6','G7','G8','G9','G10','G11','G12','G13','G14','G15','G16',
               'H1','H2','H3','H4','H5','H6','H7','H8','H9','H10','H11','H12','H13','H14','H15','H16',
               'I1','I2','I3','I4','I5','I6','I7','I8','I9','I10','I11','I12','I13','I14','I15','I16',
               'J1','J2','J3','J4','J5','J6','J7','J8','J9','J10','J11','J12','J13','J14','J15','J16',
               'K1','K2','K3','K4','K5','K6','K7','K8','K9','K10','K11','K12','K13','K14','K15','K16',
               'L1','L2','L3','L4','L5','L6','L7','L8','L9','L10','L11','L12','L13','L14','L15','L16',
               'M1','M2','M3','M4','M5','M6','M7','M8','M9','M10','M11','M12','M13','M14','M15','M16',
               'N1','N2','N3','N4','N5','N6','N7','N8','N9','N10','N11','N12','N13','N14','N15','N16',
               'O1','O2','O3','O4','O5','O6','O7','O8','O9','O10','O11','O12','O13','O14','O15','O16',
               'P1','P2','P3','P4','P5','P6','P7','P8','P9','P10','P11','P12','P13','P14','P15','P16',]

#===============================================================================

class NodoCommand(eg.ActionClass):
    # class voor standaard Nodo Commando's met opties voor Parameter-1, Parameter-2 en divert
    
    class text:
        Par1Label = ""
        Par1Range = ""
        Par2Label = ""
        Par2Range = ""
        Diverting = ""
        NodoCommand = ""
                        
    def GetNodoCommand(self, Par1Choice=0 , Par2Choice=0 , Divert=1):
        # voeg een divert toe als de gebruiker een andere bestemming heeft gekozen dan de huidige unit
        self.text.NodoCommand = ""
        if self.text.Diverting == "1":
            if (eg.globals.ThisUnit <> str(Divert)):
                self.text.NodoCommand = "Divert " + str(Divert) + "; "
        self.text.NodoCommand+=self.cmd

        # Als Par1 in gebruik, dan kan Par1 een keuze uit een lijst zijn, een integer uit een opgegeven bereik of een KAKU
        if len(self.text.Par1Label)>0: 
            self.text.NodoCommand+=' '
            if self.text.Par1Range[0]=="KAKU":
                self.text.NodoCommand+=KAKU.addresses[Par1Choice]
                            
            elif (self.text.Par1Range[0].find('..', 0, len(self.text.Par1Range[0])))>0:
                self.text.NodoCommand+=str(Par1Choice)                            
            else:          
                self.text.NodoCommand+=self.text.Par1Range[Par1Choice]
            
        # Als Par2 gebruikt, dan kan Par2 een keuze uit een lijst zijn, een integer uit een opgegeven bereik of een vrij in te geven veld
        if len(self.text.Par2Label)>0: 
            self.text.NodoCommand+=','
            if (self.text.Par2Range[0].find('..', 0, len(self.text.Par2Range[0])))>0:
                self.text.NodoCommand+=str(Par2Choice)                                
            else:          
                self.text.NodoCommand+=self.text.Par2Range[Par2Choice]
            
        self.text.NodoCommand+=';'        
        return self.text.NodoCommand


    def __call__(self, Par1Choice=0 , Par2Choice=0 , Divert=1):
        # Verzend het commando naar de Nodo 
        eg.plugins.NodoSerial.plugin.Send(self.GetNodoCommand(Par1Choice,Par2Choice,Divert))
    
    
    def GetLabel(self, Par1Choice=0, Par2Choice=0, Divert=1):        
        return self.GetNodoCommand(Par1Choice,Par2Choice,Divert)

    def Configure(self, Par1Choice=0, Par2Choice=0, Divert=1):
        Par1Used=len(self.text.Par1Label)
        Par2Used=len(self.text.Par2Label)
        DivertUsed=int(self.text.Diverting)     

        panel = eg.ConfigPanel(self)
        mainSizer =wx.BoxSizer(wx.VERTICAL)
        
        # Als parameter-1 bij dit commando gebruikt kan worden, dan de gebruikersinput hiervoor gereed maken
        if Par1Used:
            # Parameter-1 kan een keuze uit een lijst zijn of een waarden bereik of een KAKU adres        
            if self.text.Par1Range[0]=="KAKU":
                Par1Ctrl=panel.Choice(Par1Choice,KAKU.addresses)
            else:
                Range=self.text.Par1Range[0]
                x=Range.find('..', 0, len(Range))
        
                if x>0:
                    MinRange=int(Range[:x])
                    MaxRange=int(Range[x+2:])
                    Par1Ctrl=panel.SpinIntCtrl(Par1Choice, MinRange, MaxRange)        
                else:        
                    Par1Ctrl=panel.Choice(Par1Choice,self.text.Par1Range)

        # Als parameter-2 bij dit commando gebruikt kan worden, dan de gebruikersinput hiervoor gereed maken
        if Par2Used:
            # Parameter-2 kan een keuze uit een lijst zijn of een waarden bereik        
            Range=self.text.Par2Range[0]
            x=Range.find('..', 0, len(Range))
            if x>0:
                MinRange=int(Range[:x])
                MaxRange=int(Range[x+2:])
                Par2Ctrl=panel.SpinIntCtrl(Par2Choice, MinRange, MaxRange)        
            else:        
                Par2Ctrl=panel.Choice(Par2Choice,self.text.Par2Range)

        # Er zijn nu nul, een of twee parameters die in een box geplaatst moeten worden
        if Par1Used and not Par2Used:
            ParBox = panel.BoxedGroup( "Parameters: ",(self.text.Par1Label, Par1Ctrl))
            
        elif Par1Used and Par2Used:
            ParBox = panel.BoxedGroup( "Parameters: ",(self.text.Par1Label, Par1Ctrl),(self.text.Par2Label, Par2Ctrl))
            
        if Par1Used:
            eg.EqualizeWidths(ParBox.GetColumnItems(0))
            panel.sizer.Add(ParBox)

        # Als diverting bij dit commando gebruikt kan worden, dan de gebruikersinput hiervoor gereed maken
        if DivertUsed:
            Textlbl=wx.StaticText(panel, -1, " ")
            DivertCtrl = panel.SpinIntCtrl(Divert, 0, 15)        
            DivertBox = panel.BoxedGroup( "Divert: ",( "Nodo unit: ", DivertCtrl))
            eg.EqualizeWidths(DivertBox.GetColumnItems(0))
            panel.sizer.Add(Textlbl)
            panel.sizer.Add(DivertBox)

        Par1Choice=Par1Choice
        Par2Choice=Par2Choice

        # Haal de gebruikte resultaten uit het panel op       
        while panel.Affirmed():
            if DivertUsed:
                if Par2Used:
                    panel.SetResult(Par1Ctrl.GetValue(), Par2Ctrl.GetValue(), DivertCtrl.GetValue())
                elif Par1Used:
                    panel.SetResult(Par1Ctrl.GetValue(), 0, DivertCtrl.GetValue())                
                else:
                    panel.SetResult(0, 0, DivertCtrl.GetValue())
            else:
                if Par2Used:
                    panel.SetResult(Par1Ctrl.GetValue(), Par2Ctrl.GetValue(),0 )
                elif Par1Used:
                    panel.SetResult(Par1Ctrl.GetValue(),0 ,0 )                
                else:
                    panel.SetResult(0, 0, 0)
                    
            self.text.NodoCommand=self.GetNodoCommand(Par1Choice,Par2Choice,Divert)
                                
#===============================================================================

class ClockSync(eg.ActionClass):

    def __call__(self, Divert=1):
        from datetime import datetime
        t = datetime.now ()

        # voer een divert toe als de gebruiker een andere bestemming heeft gekozen dan de huidige unit
        if eg.globals.ThisUnit<>Divert:
            DivertCommand = "Divert " + str(Divert) + "; "
        else:
            DivertCommand = ""
            
        eg.plugins.NodoSerial.plugin.Send(DivertCommand + "ClockSetYear " + str(t.year)[:2] + " " + str(t.year)[2:] + ";")
        eg.plugins.NodoSerial.plugin.Send(DivertCommand + "ClockSetDate " + str(t.day) + " " + str(t.month) + ";")
        eg.plugins.NodoSerial.plugin.Send(DivertCommand + "ClockSetTime " + str(t.hour) + " " + str(t.minute) + ";")

        # Nodo: 1=zondag, 7=zaterdag
        # function weekday: 0=maandag 6=zondag
        DOW = (t.weekday() + 2) % 7
        if DOW == 0:
            DOW = 7   # zaterdag!
        eg.plugins.NodoSerial.plugin.Send(DivertCommand + "ClockSetDow " + str(DOW) + ";")

    def Configure(self,Divert=1):
        panel = eg.ConfigPanel(self)
        mainSizer =wx.BoxSizer(wx.VERTICAL)
    
        Textlbl=wx.StaticText(panel, -1, " ")
        DivertCtrl = panel.SpinIntCtrl(Divert, 0, 15)        
        DivertBox = panel.BoxedGroup( "Divert: ",( "Nodo unit: ", DivertCtrl))
        eg.EqualizeWidths(DivertBox.GetColumnItems(0))
        panel.sizer.Add(Textlbl)
        panel.sizer.Add(DivertBox)
        while panel.Affirmed():
            panel.SetResult(DivertCtrl.GetValue())

    def GetLabel(self, Divert=1):
        return "Divert " + str(Divert) + '; ClockSet...;'
  
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
        eg.globals.Unit = "1"
        eg.globals.ThisUnit = "1"
        eg.globals.Direction=""
        eg.globals.Version="?"
        
        group = self        
        for Command, DescriptionPar1, RangePar1, DescriptionPar2, RangePar2, DescriptionCommand, Dvrt in NodoCommandList:              
            if len(DescriptionCommand)==0:
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
            else:
                class Action(NodoCommand):
                    name = Command
                    description = DescriptionCommand
                    cmd = Command
                                                
                Action.text.Par1Label = DescriptionPar1
                Action.text.Par1Range = RangePar1
                Action.text.Par2Label = DescriptionPar2
                Action.text.Par2Range = RangePar2
                Action.text.Diverting = str(Dvrt)
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
    
    # bij binnenkomst van input op de Seriele poort.
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
              # XON: nodo heeft aangegeven weer klaar te staan. Ga verder met zenden van tekens
              eg.globals.hold = 0
              continue

          elif b == chr(19):
              # XOFF: zet verzenden van tekens naar de seriele poort tijdelijk even in de hold zodat de buffer van de Nodo niet overloopt.
              eg.globals.hold = 1
              continue
                         
          buffer += b

    # De van de seriele poort ontvangen regel parsen en relevante variabelen opslaan
    def ParseReceivedLine(self, ReceivedString):
            # parse de ontvangen Nodo regel en plaats de delen in eg.globals voor later gebruik door de user     
            # Zoek in de binnengekomen regels naar de Tags en haal bijbehorende waarde er uit.
            # Plaats deze vervolgens in de globale namespace eg.globals            
            for Tag in TagsToRemember:
                Tag+="="
                x = ReceivedString.find(Tag, 0, len(ReceivedString))
                if x!=-1:
                    x+=len(Tag)
                    y = ReceivedString.find(",", x, len(ReceivedString))
                    if y<0:
                        y = ReceivedString.find(";", x, len(ReceivedString))
                    if y<0:
                        y = ReceivedString.find(".", x, len(ReceivedString))
                    if y<0:
                        y = ReceivedString.find(" ", x, len(ReceivedString))
                    if y<0 :
                        y = len(ReceivedString)                        
                    if y>0:
                        # ken waarde aan variabele toe een global variabele
                        eg.globals.__setattr__(Tag.strip("= "),ReceivedString[x:y].strip(", ()=;"))
                        # ??? print "Tag gevonden: " + Tag.strip("= ") + "= >" +  ReceivedString[x:y].strip(", ()=;") + "<  Start=" + str(x) + " eind=" + str(y) 

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

