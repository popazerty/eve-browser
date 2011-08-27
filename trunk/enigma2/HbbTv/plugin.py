from enigma import fbClass, eServiceReference, eTimer, iServiceInformation, getDesktop
from Plugins.Plugin import PluginDescriptor
import subprocess
from Tools.Directories import resolveFilename, SCOPE_PLUGINS
from Screens.Screen import Screen
from Components.ActionMap import ActionMap
import sys
from iEveBrowser import iEveBrowser

gUrl = ""

###
# This Screen is only active if the browser is fullscreen
class EveBrowser(Screen):
  # Pseudo Screen
  skin = """<screen name="EveBrowser" position="0,0" zPosition="10" size="1,1" backgroundColor="transparent" title="EveBrowser" flags="wfNoBorder">
    </screen>
  """
  def __init__(self, session, url):
    Screen.__init__(self, session)
    
    #fbClass.getInstance().lock()
    
    self.eveBrowser = iEveBrowser()
    print "a"
    dSize = getDesktop(0).size()
    self.eveBrowser.setDimension(1280, 720)
    #self.eveBrowser.setDimension(dSize.width(), dSize.height())
    print "b"
    #self.eveBrowser.loadEveBrowser()
    print "c"
    if len(url) == 0:
      url = "http://tv-html.irt.de/hbbtv/interop05/index.php" #"http://tv-html.irt.de/hbbtv/tests/" #"http://itv.ard.de/ardtext/"
    self.eveBrowser.loadPage(url)
    print "d"
    #TODO: Add addionional keys for routing to browser
    self["eveBrowserActions"] = ActionMap(["OkCancelActions", "ColorActions", "DirectionActions", "NumberActions"],
    {
      "red": self.keyRed,
      "green": self.keyGreen,
      "yellow": self.keyYellow,
      "blue": self.keyBlue,
      "up": self.keyUp,
      "down": self.keyDown,
      "left": self.keyLeft,
      "right": self.keyRight,
      "ok": self.keyOk,
      "1": self.key1,
      "2": self.key2,
      "3": self.key3,
      "4": self.key4,
      "5": self.key5,
      "6": self.key6,
      "7": self.key7,
      "8": self.key8,
      "9": self.key9,
      "0": self.key0,
    }, -2)
    

  def show(self):
    self.eveBrowser.show()
  
  def close(self):
    self.eveBrowser.unloadEveBrowser()
    #fbClass.getInstance().unlock()
  
  def keyRed(self):
    self.keyPressed("red")
  
  def keyGreen(self):
    self.keyPressed("green")
  
  def keyYellow(self):
    self.keyPressed("yellow")
  
  def keyBlue(self):
    self.keyPressed("blue")
  
  def keyOk(self):
    self.keyPressed("ok")
  
  def keyUp(self):
    self.keyPressed("up")
  
  def keyDown(self):
    self.keyPressed("down")
  
  def keyLeft(self):
    self.keyPressed("left")
  
  def keyRight(self):
    self.keyPressed("right")
  
  def key1(self):
    self.keyPressed("1")
  
  def key2(self):
    self.keyPressed("2")
  
  def key3(self):
    self.keyPressed("3")
  
  def key4(self):
    self.keyPressed("4")
  
  def key5(self):
    self.keyPressed("5")
  
  def key6(self):
    self.keyPressed("6")
  
  def key7(self):
    self.keyPressed("7")
  
  def key8(self):
    self.keyPressed("8")
  
  def key9(self):
    self.keyPressed("9")
  
  def key0(self):
    self.keyPressed("0")
  
  def keyPressed(self, key):
    self.eveBrowser.keyPress(key, self.eveBrowser.KEY_TYPE_PRESS)
    self.eveBrowser.keyPress(key, self.eveBrowser.KEY_TYPE_RELEASE)

###
# This Screen displays the RED Button
class REDButton(Screen):
	skin = """<screen name="REDButton" position="50,50" zPosition="10" size="34,45" backgroundColor="transparent" title="REDButton" flags="wfNoBorder">
		<eLabel position="0,0" size="34,45" backgroundColor="#FF0000" />
	</screen>
	"""

	def __init__(self, session, url):
		Screen.__init__(self, session)
		self.session = session
		self.url = url
		
		self["redButtonActions"] = ActionMap(["SetupActions", "ColorActions"],
		{
			"red": self.keyRed,
			#"cancel": self.close,
		}, -2)
	
	def keyRed(self):
		self.session.open(EveBrowser, self.url)

class HbbTv():
	
	pid = ""
	
	def __init__(self, session):
		self.session = session
		
		# This is a hack, instead we should register to the channel changed event
		self.poll_timer = eTimer()
		self.poll_timer.callback.append(self.__eventInfoChanged)
	
	def __eventInfoChanged(self):
		if self.session is not None and self.session.nav is not None:
			from enigma import eServiceCenter
			serviceHandler = eServiceCenter.getInstance()
			ref = self.session.nav.getCurrentService()
			if ref is not None:
				d = ref.stream().getStreamingData()
				demux = 0
				if d.has_key("demux"):
					demux = d["demux"]
				if d.has_key("pids"):
					for pid in d["pids"]:
						if pid is not None and len(pid) == 2 and pid[1] == "pmt":
							print pid[0]
							if self.pid != pid[0]:
								self.pid = pid[0]
								self.checkForHbbTVService(demux, pid[0])
								return
	
	def checkForHbbTVService(self, demux, pmtPid):
		p = subprocess.Popen((resolveFilename(SCOPE_PLUGINS) + "/SystemPlugins/HbbTv/bin/hbbtvscan-sh4", 
			"-p", str(pmtPid), "-d", "/dev/dvb/adapter0/demux" + str(demux)), stdout=subprocess.PIPE)
		out = p.communicate()[0]
		for line in out.split("\n"):
			if line.startswith("URL"):
				elements = line.split("\"")
				url = elements[1]
				print url
				type = elements[5]
				if type == "AUTOSTART":
					# Here we have to think about what we want 
					# Either we load pages always in backgroudn so that hbbtv pages are displayed shortly after red button press
					# and that the red button option is only displayed after fully loading a page
					# Or we only start the browser after a user has pressed the red button 
					# this would be wy more memory efficiant as tha browser doesnt have to 
					# run always, but the first page will be displayed slower
					
					#self.session.open(EveBrowser, url)
					
					# For the moment the 2nd aproach seems to be a better solution
					#self.displayREDButton(url)
					global gUrl
					gUrl = url
					
					return
	
	def start(self, session):
		self.session = session
		#self.session.open(EveBrowser, "http://www.google.de")
		#self.displayREDButton("http://itv.ard.de/ardtext/")
		self.poll_timer.start(5000)
	
	def displayREDButton(self, url):
		self.session.open(REDButton, url)

global_session = None
global_hbbtv = None

def autostart(reason, **kwargs):
	print "B"*60
	global global_session
	global global_hbbtv
	if reason == 0:
		print "starting hbbtv"
		global_hbbtv = HbbTv(global_session)

def sessionstart(reason, session):
	print "A"*60
	global global_session
	global global_hbbtv
	global_session = session
	global_hbbtv.start(global_session)

def main(session, **kwargs):
	global gUrl
	session.open(EveBrowser, gUrl)

def Plugins(**kwargs):
	return [
		PluginDescriptor(name="HbbTv", description="HbbTv", where = PluginDescriptor.WHERE_EXTENSIONSMENU, fnc=main),
		PluginDescriptor(name = "HbbTv", description = "HbbTv", where = PluginDescriptor.WHERE_AUTOSTART, fnc = autostart),
		PluginDescriptor(where = PluginDescriptor.WHERE_SESSIONSTART, fnc = sessionstart)
	]
