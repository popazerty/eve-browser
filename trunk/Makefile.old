

TUFSBOX="/home/schischu/git/at7500/tdt/tufsbox"
BINS="${TUFSBOX}/devkit/sh4/bin"
CDK="${TUFSBOX}/cdkroot"


CC            = ${BINS}/sh4-linux-gcc
CXX           = ${BINS}/sh4-linux-g++
CFLAGS        = -std=c99 -pipe -Os -Wall -W -D_REENTRANT $(DEFINES)
CXXFLAGS      = -pipe -Os -Wall -W -D_REENTRANT $(DEFINES)
INCPATH       = -I${CDK}/usr/include \
-I${CDK}/usr/include/glib-2.0 \
-I${CDK}/usr/lib/gtk-2.0/include \
-I${CDK}/usr/include/webkit-1.0 \
-I${CDK}/usr/include/gtk-2.0 \
-I${CDK}/usr/lib/glib-2.0/include \
-I${CDK}/usr/include/libsoup-2.4 \
-I${CDK}/usr/include/directfb \
-I${CDK}/usr/include/directfb-internal \
-I.
LINK          = ${BINS}/sh4-linux-g++
LFLAGS        = -Wl,-O1
LIBS          = $(SUBLIBS)  -L${CDK}/usr/lib  -lfusion -lwebkitgtk-1.0 -lpng -lm  -lpthread
AR            = ${BINS}/sh4-linux-ar cqs
RANLIB        = ${BINS}/sh4-linux-ranlib

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = main.cpp js_extension.c js_debug.c
OBJECTS       = main.o js_extension.o js_debug.o

#SOURCES       = main.c js_debug.c js_extension.c css_extension.c
#OBJECTS       = main.o js_debug.o js_extension.o css_extension.o

TARGET        = eve-browser

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET)

$(TARGET):  $(OBJECTS)  
	$(CXX) -DDFB $(CXXFLAGS) $(INCPATH) $(LFLAGS) -o $(TARGET) $(SOURCES) $(OBJCOMP) $(LIBS); \
	$(CXX) -DDFB -fPIC $(INCPATH) -c $(SOURCES); \
	$(CXX) -shared -W1,-E,-soname,lib$(TARGET).so.0 -o lib$(TARGET).so.0.0.0 $(OBJECTS) $(LIBS)
    
    
