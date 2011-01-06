CC            = sh4-linux-gcc
CXX           = sh4-linux-g++
CFLAGS        = -std=c99 -pipe -Os -Wall -W -D_REENTRANT $(DEFINES)
CXXFLAGS      = -pipe -Os -Wall -W -D_REENTRANT $(DEFINES)
INCPATH       = -I/opt/STM/STLinux-2.3/devkit/sh4/target/usr/include \
-I/opt/STM/STLinux-2.3/devkit/sh4/target/usr/include/glib-2.0 \
-I/opt/STM/STLinux-2.3/devkit/sh4/target/usr/lib/gtk-2.0/include \
-I/opt/STM/STLinux-2.3/devkit/sh4/target/usr/include/pango-1.0 \
-I/opt/STM/STLinux-2.3/devkit/sh4/target/usr/include/webkit-1.0 \
-I/opt/STM/STLinux-2.3/devkit/sh4/target/usr/include/gtk-2.0 \
-I/opt/STM/STLinux-2.3/devkit/sh4/target/usr/lib/glib-2.0/include \
-I/opt/STM/STLinux-2.3/devkit/sh4/target/usr/include/cairo \
-I/opt/STM/STLinux-2.3/devkit/sh4/target/usr/include/atk-1.0 \
-I/opt/STM/STLinux-2.3/devkit/sh4/target/usr/include/libsoup-2.4 \
-I.
LINK          = sh4-linux-g++
LFLAGS        = -Wl,-O1
LIBS          = $(SUBLIBS)  -L/opt/STM/STLinux-2.3/devkit/sh4/target/usr/lib -lwebkit-1.0 -leplayer3 -lpng -lm -lass -lpthread -lavformat -lavdevice
AR            = sh4-linux-ar cqs
RANLIB        = sh4-linux-ranlib

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = main.c js_debug.c js_extension.c css_extension.c libeplayer3.c

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
	$(CXX) $(CXXFLAGS) $(INCPATH) $(LFLAGS) -o $(TARGET) $(SOURCES) $(OBJCOMP) $(LIBS)
