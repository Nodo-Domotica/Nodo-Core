# This is a part of the EventGhost and the Nodo software.
# Copyright (C) 2014 Paul Tonkes <p.k.tonkes@gmail.com>


r"""<rst>
Plugin for the Nodo Domotica controller V3.7
(C) 2014 P.K.Tonkes@gmail.com

For more information, Take a look at: `www.nodo-domotica.nl`__.

.. image:: Nodo.jpg
   :align: center

__ http://www.nodo-domotica.nl
"""

import eg
import binascii
import time

eg.RegisterPlugin(
    name="Nodo",
    description=__doc__,
    url="http://www.nodo-domotica.nl",
    author="P.K.Tonkes@gmail.com",
    version="Plugin version V3.7",
    canMultiLoad = False,
    createMacrosOnAdd = True,
)

#===============================================================================
            
TagsToRemember = ["Input", "ThisUnit", "Unit", "Version", "Event"]
               
#===============================================================================
class PluginVars:
    EventPrefix     = "Unit-{ThisUnit}"                 # default prefix is "Unit-n" waarbij n het unitnummer is
    EventSuffix     = "{Input}.{Event}"                 # default suffix is de tekst achter "event="
    XonXoffHold     = 0                                 # t.b.v. seriele handshaking xon/xoff


#===============================================================================          
class NodoEvent(eg.ActionWithStringParameter):
    def __call__(self, data):
        eg.plugins.NodoSerial.plugin.Send(str(data))

    def GetLabel(self, data):
        return str(data)
                
#===============================================================================
class EventlistWrite(eg.ActionClass):
    class text:
        EventlistEvent="Event\n\nEvent Par1,Par2"
        EventlistAction="Action to execute\n\nAction Par1,Par2"
        
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
        eg.globals.Unit        = "0"
        eg.globals.ThisUnit    = "0"
        eg.globals.Direction   = ""
        eg.globals.Version     = "0"
        eg.globals.Output      = ""        
        NodoEvent.name = 'Event'
        NodoEvent.description = 'Send an event or command to the Nodo'
        NodoEvent.parameterDescription = 'One or more Events or commands, seperated by a semicolon.'
        NodoEvent.cmd = NodoEvent.name

        self.AddAction(NodoEvent)
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
        eg.plugins.NodoSerial.plugin.Send("Status")
        eg.plugins.EventGhost.Wait(1.0)

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

    # verzenden van het commando en gelijkertijd kijken of er een XOFF verzonden is
    def Send(self, StringToSend=""):
        eg.globals.Output = StringToSend
        
        #verzend de reeks tekens uit het Nodo commando, zolang de Nodo met XOn aangeeft data te kunnen ontvangen
        StringToSend+="\n"
        
        for i in range(len(StringToSend)):        

            x=0 
            PluginVars.DataReceived = 0 # wissen om zo te kijken of er nog seriele data binnenkomt.
            while PluginVars.XonXoffHold == 1: #Zolang nog geen XON ontvangen of de Nodo nog zendt

                TimeoutTimer = int(time.time()) + 2 # timeout in seconden

                # Wacht zolang nog geen XON ontvangen, maar niet langer dan de timeout waarde
                while PluginVars.XonXoffHold == 1 and (TimeoutTimer > int(time.time())):
                    eg.plugins.EventGhost.Wait(0.1)

                #als er een timeout is opgetreden
                if TimeoutTimer <= int(time.time()):
                    x=x+1                                

                    if PluginVars.DataReceived==1:
                      PluginVars.DataReceived = 0 # wissen om zo nog steeds te monitoren of er nog data binnenkomt.
                      
                    else:
                      self.serialThread.Write(chr(17)) # voor de zekerheid een XON sturen zodat Nodo en PC niet op elkaar wachten.

                # er is nog steeds geen XON ontvangen en de Nodo zendt geen data en we hebben 30 pogingen van 2 seconden = 1 minuut wachten
                # gedurende deze tijd niets van de Nodo vernomen. Dan mogen we er vanuit gaan dat er iets niet goed is gegaan
                if x>=30 and PluginVars.DataReceived==0 and PluginVars.DataReceived==0: 
                    PluginVars.XonXoffHold = 0 # XON/XOFF resetten, de PC en Nodo wachten op elkaar.
                    print "*** Error: No response from Nodo" #???

            self.serialThread.Write(StringToSend[i])                                

        # kleine pause omdat anders kans dat het XOFF van de Nodo nog niet opgepikt is door de PC.
        eg.plugins.EventGhost.Wait(0.5) #???        
    
#===============================================================================

    # bij binnenkomst van input op de Seriele poort.
    def OnReceive(self, serial):
      buffer = ""
      while True:
          b = serial.Read(1, 0.2)
          if b == "\n":
              eg.globals.Input = buffer
              print buffer

              # Zoek in de binnengekomen regels naar de Tags en haal bijbehorende waarde er uit.
              # Plaats deze vervolgens in de globale namespace eg.globals
              eg.globals.Event  = "" 
              eg.globals.Unit   = ""
              eg.globals.Input  = ""

              for Tag in TagsToRemember:
                  Tag+="="
                  x = buffer.find(Tag, 0, len(buffer))
                  if x!=-1:
                      x+=len(Tag)
                      y = buffer.find(";", x, len(buffer))
                      if y<0 :
                          y = len(buffer)                        
                      if y>0:
                          Tag=Tag.strip("=")
                          eg.globals.__setattr__(Tag,buffer[x:y].strip(";"))
  
              # Nodo events die ontvangen worden door de plugin moeten leiden tot een event in EventGhost echter alleen 
              # als deze input zijn voor de Nodo.
              if eg.globals.Event !="" and eg.globals.Input !="":
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
                         
          PluginVars.DataReceived = 1
          buffer += b

#===============================================================================

