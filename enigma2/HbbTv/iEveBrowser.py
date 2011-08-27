#killall rcS; killall enigma2; mount -tvfat /dev/sda1 /root; export LD_LIBRARY_PATH=/root/dfb2/lib:$LD_LIBRARY_PATH; enigma2

import os
import subprocess
import sys
from Tools.Directories import resolveFilename, SCOPE_PLUGINS

ctypes = None
try:
  print "Loading ctypes"
  ctypes = __import__("_ctypes")
  print "Loading ctypes - Done"
except Exception, ex:
  print "Loading ctypes - Failed (%s)" % ex
  ctypes = None

class c_int(ctypes._SimpleCData):
  _type_ = "i"

class CFunctionType(ctypes.CFuncPtr):
  _argtypes_ = (c_int, )
  _restype_ = c_int
  _flags_ = ctypes.FUNCFLAG_CDECL

gIEveBrowser = None

def _iEveBrowser__evtInfo(type):
  print "_iEveBrowser__evtInfo:", type
  if gIEveBrowser is not None:
    gIEveBrowser._evtInfo(type)
  return 0



class iEveBrowser():
  
  eveBrowser = None
  evtInfo = None
  
  def __init__(self):
    #os.system('export DFBARGS="pixelformat=ARGB,no-cursor,bg-none')
    os.environ["DFBARGS"] = "pixelformat=ARGB,no-cursor,bg-none,no-linux-input-grab,no-vt"
    
    
    try:
      print "Loading libevebrowser.so.0.0.0"
      lib = resolveFilename(SCOPE_PLUGINS) + "/SystemPlugins/HbbTv/lib/libevebrowser.so.0.0.0"
      self.eveBrowser = ctypes.dlopen(lib, ctypes.RTLD_GLOBAL)
      print "Loading libevebrowser.so.0.0.0 - Done"
    except Exception, ex:
      print "Loading libevebrowser.so.0.0.0 - Failed (%s)" % ex
      try:
        print "Loading libevebrowser.so.0.0.0"
        lib = "/usr/lib/libevebrowser.so.0.0.0"
        self.eveBrowser = ctypes.dlopen(lib, ctypes.RTLD_GLOBAL)
        print "Loading libevebrowser.so.0.0.0 - Done"
      except Exception, ex:
        print "Loading libevebrowser.so.0.0.0 - Failed (%s)" % ex
        return
    
    try:
      print "Registering functions"
      self._setDimension = ctypes.dlsym(self.eveBrowser, "setDimension")
      self._loadEveBrowser = ctypes.dlsym(self.eveBrowser, "loadEveBrowser")
      self._unloadEveBrowser = ctypes.dlsym(self.eveBrowser, "unloadEveBrowser")
      self._loadPage = ctypes.dlsym(self.eveBrowser, "loadPage")
      self._show = ctypes.dlsym(self.eveBrowser, "show")
      self._hide = ctypes.dlsym(self.eveBrowser, "hide")
      self._keyPress = ctypes.dlsym(self.eveBrowser, "keyPress")
      self._setCallback = ctypes.dlsym(self.eveBrowser, "setCallback")
      print "Registering functions - Done"
    except Exception, ex:
      print "Registering functions - Failed (%s)" % ex
      return
    
    try:
      print "Registering callback"

      self._EVTFUNC = CFunctionType(__evtInfo)
      ctypes.call_function(self._setCallback, (self._EVTFUNC, ))
      print "Registering callback - Done"
    except Exception, ex:
      print "Registering callback - Failed (%s)" % ex
      return
    
    global gIEveBrowser
    gIEveBrowser = self
    return
  
  ###
  # Framebuffer dimension
  def setDimension(self, w, h):
    if self.eveBrowser is not None:
      ctypes.call_function(self._setDimension, (w, h, ))
  
  ###
  # Load the webpage
  # Will not display it if hidden
  def loadPage(self, url):
    if self.eveBrowser is not None:
      ctypes.call_function(self._loadPage, (url, ))
  
  ###
  # Loads the browser
  # Will not display it if hidden
  def loadEveBrowser(self):
    if self.eveBrowser is not None:
      ctypes.call_function(self._loadEveBrowser, ())
  
  ###
  # Unloads the browser, give me my mem back
  # Will not display it if hidden
  def unloadEveBrowser(self):
    if self.eveBrowser is not None:
      ctypes.call_function(self._unloadEveBrowser, ())
  
  ###
  # Display browser
  def show(self):
    if self.eveBrowser is not None:
      ctypes.call_function(self._show, ())
  
  ###
  # Hide browser
  def hide(self):
    if self.eveBrowser is not None:
      ctypes.call_function(self._hide, ())
  
  KEY_TYPE_PRESS   = 0
  KEY_TYPE_RELEASE = 1
  KEY_TYPE_PRELL   = 2
  
  ###
  # Routs keypresses to the browser
  # type is one of the above KEY_TYPE_*
  def keyPress(self, key, type):
    if self.eveBrowser is not None:
      ctypes.call_function(self._keyPress, (key, type, ))
  
  
  EVT_EVE_BROWSER_LOADED = 0
  EVT_PAGE_LOADED = 1

  EVT_PIG_CHANGED = 2
  
  EVT_C_O_BIND_TO_CURRENT_CHANNEL = 100
  
  ###
  # Called when webpage has been loaded
  # There are differnt possible approches, either e2 displays the red button, or the browser
  def _evtInfo(self, type):
    print "_evtInfo:", type
    if type == self.EVT_EVE_BROWSER_LOADED:
      pass
    elif type == self.EVT_PAGE_LOADED:
      pass
    elif type == self.EVT_PIG_CHANGED:
      pass
    else:
      pass
    
    if self.evtInfo is not None:
      self.evtInfo(type)
  
  def setEvtInfoCallback(self, fnc):
    self.evtInfo = fnc
