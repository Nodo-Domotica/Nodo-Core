# This is a part of the EventGhost and the Nodo software.
# Copyright (C) 2012 Paul Tonkes <p.k.tonkes@gmail.com>


r"""<rst>
Plugin for the Nodo Domotica controller, based on Arduino  
(C) 2012 P.K.Tonkes@gmail.com

For more information, Take a look at: `www.nodo-domotica.nl`__.

.. image:: Nodo.jpg
   :align: center

__ http://www.nodo-domotica.nl
"""

import eg
import binascii

eg.RegisterPlugin(
    name="Nodo",
    description=__doc__,
    url="http://www.nodo-domotica.nl",
    author="P.K.Tonkes@gmail.com",
    version="Plugin version V3.1.0",
    canMultiLoad = False,
    createMacrosOnAdd = True,
)

#===============================================================================
            
CommandsToRemember = ("Error", "WiredOut", "WiredIn", "WiredAnalog", "KAKU", "NewKAKU", "Variable", "VariableSet")
TagsToRemember = ["ID", "TimeStamp", "ThisUnit", "Unit", "Version"]
               
#===============================================================================
class PluginVars:
    EventPrefix     = "Unit-{Unit}"                     # default prefix is "Unit-n" waarbij n het unitnummer is
    EventSuffix     = "{Event}"                         # default suffix is de tekst achter "event="
    XonXoffHold     = 0                                 # t.b.v. seriele handshaking xon/xoff
    NodoBusy        = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0] # per unit: is deze bezig met verwerking of niet NodoBusy[0] niet in gebruik, start bij 1]
    NodoBusyTimeout = 60                                # Tijd in seconden waarna een Timeout error ontstaat na een 'Busy On' event.

#===============================================================================
class ClockSync(eg.ActionClass):

    def __call__(self, Divert=1):

        # voer een divert toe als de gebruiker een andere bestemming heeft gekozen dan de huidige unit
        if eg.globals.ThisUnit!=str(Divert):
            DivertCommand = "Divert " + str(Divert) + "; "
        else:
            DivertCommand = ""
            
        from datetime import datetime
        t = datetime.now ()
        # Nodo: 1=zondag, 7=zaterdag
        # function weekday: 0=maandag 6=zondag
        DOW = (t.weekday() + 2) % 7
        if DOW == 0:
            DOW = 7   # zaterdag!

        #wacht als bekend is dat de Nodo bezig is met verwerking
        eg.plugins.NodoSerial.plugin.WaitBusyNodo(Divert)
        eg.plugins.NodoSerial.plugin.Send(DivertCommand + "ClockSetYear " + str(t.year)[:2] + " " + str(t.year)[2:] + ";")

        #wacht als bekend is dat de Nodo bezig is met verwerking
        eg.plugins.NodoSerial.plugin.WaitBusyNodo(Divert)
        eg.plugins.NodoSerial.plugin.Send(DivertCommand + "ClockSetDate " + str(t.day) + " " + str(t.month) + ";")

        #wacht als bekend is dat de Nodo bezig is met verwerking
        eg.plugins.NodoSerial.plugin.WaitBusyNodo(Divert)
        eg.plugins.NodoSerial.plugin.Send(DivertCommand + "ClockSetTime " + str(t.hour) + " " + str(t.minute) + ";")

        #wacht als bekend is dat de Nodo bezig is met verwerking
        eg.plugins.NodoSerial.plugin.WaitBusyNodo(Divert)
        eg.plugins.NodoSerial.plugin.Send(DivertCommand + "ClockSetDow " + str(DOW) + ";")

    def Configure(self, Divert=1):
        name = 'ClockSync'
        description = 'Sync the Nodo date,day and time with the computer system time.'
        cmd = 'ClockSync'
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
class NodoEvent(eg.ActionWithStringParameter):
    def __call__(self, data):
        eg.plugins.NodoSerial.plugin.Send(str(data))

    def GetLabel(self, data):
        return str(data)
                
#===============================================================================
class EventlistWrite(eg.ActionClass):
    class text:
        EventlistEvent="Als onderstaand event zich voordoet...\n\nEvent Par1,Par2"
        EventlistAction="...dan de volgende actie uitvoeren\n\nAction Par1,Par2"
        
    def GetLabel(self, EventlistEvent, EventlistAction):
        return "EventlistWrite; " + str(EventlistEvent) + '; ' + str(EventlistAction) + ';'

    def __call__(self, EventlistEvent, EventlistAction):
        eg.globals.Direction   = ""
        eg.plugins.NodoSerial.plugin.Send("EventlistWrite;" + str(EventlistEvent) + ';' + str(EventlistAction) + ';' )

    def Configure(self,EventlistEvent="", EventlistAction=""):
        name = 'EventlistWrite'
        description = 'Write events and actions to the eventlist'
        cmd = EventlistWrite.name
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
            panel.SetResult(EventlistEventCtrl.GetValue(),EventlistActionCtrl.GetValue())

#===============================================================================
class NodoSerial(eg.PluginClass):
    def __init__(self):        
        self.serial = None
        eg.globals.Unit        = "1"
        eg.globals.Direction   = ""
        eg.globals.Version     = "0"
        eg.globals.Output      = ""        
 
        NodoEvent.name = 'Event'
        NodoEvent.description = 'Send an event or command to the Nodo'
        NodoEvent.parameterDescription = 'One or more Events or commands, seperated by a semicolon.'
        NodoEvent.cmd = NodoEvent.name
        self.AddAction(NodoEvent)

        self.AddAction(ClockSync)
        self.AddAction(EventlistWrite)        

    def __start__(self, port, EventPrefix, EventSuffix):
        # Start de seriele communicatie
        self.port = port
        self.serialThread = eg.SerialThread()
        self.serialThread.SetReadEventCallback(self.OnReceive)
        self.serialThread.Open(port, 19200)
        self.serialThread.SetRts()
        self.serialThread.Start()

        # Haal het openingsscherm van de Nodo op om hieruit huidige Nodo te halen
        eg.plugins.EventGhost.Wait(2.0)        
        eg.plugins.NodoSerial.plugin.Send("Status Boot;")
        eg.plugins.EventGhost.Wait(0.5)

        if len(eg.globals.ThisUnit)==0:
            print "# Nodo Plugin: error! Nodo not found."
        

    def __stop__(self):
        self.serialThread.Close()
 
    def Configure(self, port=0, EventPrefix=PluginVars.EventPrefix, EventSuffix=PluginVars.EventSuffix):
        panel = eg.ConfigPanel(self)
        mainSizer =wx.BoxSizer(wx.VERTICAL)
    
        PortCtrl = panel.SerialPortChoice(port)
        CommunicationBox = panel.BoxedGroup( "Communication:",("COM: port ", PortCtrl))
        eg.EqualizeWidths(CommunicationBox.GetColumnItems(0))
        panel.sizer.Add(CommunicationBox)

        Textlbl=wx.StaticText(panel, -1, " ")
        panel.sizer.Add(Textlbl)

        PrefixCtrl=wx.TextCtrl(panel,-1,EventPrefix)
        SuffixCtrl=wx.TextCtrl(panel,-1,EventSuffix)
        SettingsBox = panel.BoxedGroup( "Events: ",( "Prefix ", PrefixCtrl),( "Suffix ", SuffixCtrl))
        eg.EqualizeWidths(SettingsBox.GetColumnItems(0))
        panel.sizer.Add(SettingsBox)

        while panel.Affirmed():
            panel.SetResult(PortCtrl.GetValue(),PrefixCtrl.GetValue(),SuffixCtrl.GetValue())
        
        PluginVars.EventPrefix=EventPrefix
        PluginVars.EventSuffix=EventSuffix

    # Wacht totdat een remote Nodo klaar is met verwerking
    def WaitBusyNodo(self, Unit):        
        
        # geef een melding als een Nodo bezig is
        from datetime import datetime
        t = datetime.now()
        SecCounter = t.hour*3600 + t.minute*60 + t.second
        Busy=0
        Nodo=0
        for NodoBusy in PluginVars.NodoBusy:
            if NodoBusy>1:
                if NodoBusy<(SecCounter-PluginVars.NodoBusyTimeout):
                    # 'Busy Off' event is niet ontvangen. Geef foutmelding maar wacht niet meer op deze Nodo.
                    print "# Nodo Plugin: Timeout error! Unit-" + str(Nodo)+ " still busy after " + str(SecCounter - NodoBusy + PluginVars.NodoBusyTimeout)+ " seconds, or missing \'Busy Off\' event."                    
                    PluginVars.NodoBusy[Nodo]=0
                            
                elif NodoBusy>SecCounter:                      
                    Busy+=1
                    print "# Nodo Plugin: Waiting for Nodo Unit-" + str(Nodo)
            Nodo+=1
              
        # Wacht enige tijd totdat alle Nodos een 'Busy off' hebben verzonden. 
        TimeOut=SecCounter+PluginVars.NodoBusyTimeout
        while Busy!=0 and TimeOut>SecCounter:        
            t = datetime.now()
            SecCounter = t.hour*3600 + t.minute*60 + t.second
            eg.plugins.EventGhost.Wait(0.1)
            Busy=0
            for NodoBusy in PluginVars.NodoBusy:
                if NodoBusy>1:                      
                    Busy+=1
 
        # Als de timeout opgetreden is, geef dan weer welke Nodo(s) hiervoor verantwoordelijk waren. 
        if Busy:
            Nodo=0
            for NodoBusy in PluginVars.NodoBusy:
                # alleen de Nodos weergeven waarbij de 'SendBusy' actief is
                if NodoBusy>1:
                    print "# Nodo Plugin: Timeout error! Unit-" + str(Nodo)+ " still busy after " + str(SecCounter - NodoBusy + PluginVars.NodoBusyTimeout)+ " seconds, or missing \'Busy Off\' event."                    
                    # Markeer voor deze Nodo dat de 'WaitBusy' van de plugin niet meer gebruikt wordt 
                    PluginVars.NodoBusy[Nodo]=0
                Nodo+=1

        # Zet de status van de Nodo op Busy omdat verwerking te snel gaat om 'Busy On' van de Nodo op tijd te ontvangen
        # echter alleen als de SendBusy actief is op de betreffende Nodo
        if PluginVars.NodoBusy[Unit]>0:
           PluginVars.NodoBusy[Unit]=SecCounter+PluginVars.NodoBusyTimeout  

    # verzenden van het commando en gelijkertijd kijken of er een XOFF verzonden is
    def Send(self, StringToSend=""):
        print "< " + StringToSend
        eg.globals.Output = StringToSend
        
        #verzend de reeks tekens uit het Nodo commando
        StringToSend+="\n"
        for i in range(len(StringToSend)):        
            if PluginVars.XonXoffHold == 1:
                while PluginVars.XonXoffHold == 1:
                    x = 0
            self.serialThread.Write(StringToSend[i])                                

        # kleine pause omdat anders kans dat het XOFF van de Nodo nog niet opgepikt is door de PC.
        eg.plugins.EventGhost.Wait(0.1)        
    
    # bij binnenkomst van input op de Seriele poort.
    def OnReceive(self, serial):
      buffer = ""
      while True:
          b = serial.Read(1, 0.2)
          if b == "\n":
              eg.globals.Input = buffer
              print buffer
              self.ParseReceivedLine(buffer)
               
              # Nodo events die ontvangen worden door de plugin moeten leiden tot een event in EventGhost echter alleen 
              # als deze input zijn voor de Nodo.
              # "Busy" Events ook afvangen want deze worden behandeld wordt door de plugin.
              if eg.globals.Event !="" and (eg.globals.Direction=="Input" or eg.globals.Direction=="Internal"):
                  if eg.globals.Command == "Busy":
                      pass
                  else:
                      self.info.eventPrefix = eg.ParseString(PluginVars.EventPrefix) 
                      self.TriggerEvent(eg.ParseString(PluginVars.EventSuffix))
              return
              
          elif b == "":
              # Niets ontvangen binnen timeout
              return
              
          elif b == chr(17):
              # XON: nodo heeft aangegeven weer klaar te staan. Ga verder met zenden van tekens
              PluginVars.XonXoffHold = 0
              continue

          elif b == chr(19):
              # XOFF: zet verzenden van tekens naar de seriele poort tijdelijk even in de hold zodat de buffer van de Nodo niet overloopt.
              PluginVars.XonXoffHold = 1
              continue
                         
          buffer += b

    # De van de seriele poort ontvangen regel parsen en relevante variabelen opslaan
    def ParseReceivedLine(self, ReceivedString):
            # parse de ontvangen Nodo regel en plaats de delen in eg.globals voor later gebruik door de user     

            # Om te voorkomen dat variabelen niet meer actuele gegevens bevatten
            eg.globals.Unit=""

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
                    if y<0 :
                        y = len(ReceivedString)                        
                    if y>0:
                        Tag=Tag.strip(" =")
                        eg.globals.__setattr__(Tag,ReceivedString[x:y].strip(", ;"))

            # Niet Nodo events krijgen geen Unit-tag mee
            if eg.globals.Unit == "":                                     
                eg.globals.Unit = "0"
                
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

            # Handshaking in een multi-nodo omgeving. 
            # als een Nodo "Busy On" heeft verzonden is deze bezig met verwerking
            # bewaar van alle units de Busy status in de tabel NodoBusy[]
             
            if eg.globals.Command == "Busy":

                # gebruik de systeemtijd om de timeout van een 'Busy' Nodo vast te stellen
                from datetime import datetime
                t = datetime.now()

                if eg.globals.Par1 == "On":
                    # zet de timeout tijd op nu + enige wachttijd
                    PluginVars.NodoBusy[int(eg.globals.Unit)] = t.hour*3600 + t.minute*60 + t.second + PluginVars.NodoBusyTimeout                  
                else:
                    # vul met een 1 om aan te geven dat er niet gewacht hoeft te worden maar de Nodo wel 
                    # gebruik maakt van 'SendBusy'
                    PluginVars.NodoBusy[int(eg.globals.Unit)] = 1                  

            # Van enkele events/commando's is het handig om de waarde te onthouden als globale variabele in eg.globals
            # Zoek in de binnengekomen regels naar de Tags en haal bijbehorende waarde er uit.
            # Plaats deze vervolgens in de globale namespace eg.globals            
            for cmd in CommandsToRemember:
                if cmd==eg.globals.Command:

                    # enkele events hebben ook een commando tegenhanger.
                    # bij opslaan in variabelen hiertussen geen onderscheid meer maken
                    if cmd == "VariableSet":
                        cmd="Variable"

                    # schrijf weg voor de gebruiker.
                    if eg.globals.Unit!="" and eg.globals.Unit!="0" :
                        eg.globals.__setattr__("Unit" + str(eg.globals.Unit) + "_" + cmd + "_" + str(eg.globals.Par1),eg.globals.Par2) 
                    else:
                        eg.globals.__setattr__(cmd + "_" + str(eg.globals.Par1),eg.globals.Par2) 
            return

#===============================================================================

