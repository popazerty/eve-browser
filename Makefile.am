lib_LTLIBRARIES = libevebrowser.la

libevebrowser_la_SOURCES = main.c js_extension.c js_debug.c

libevebrowser_la_CFLAGS = -std=c99 -DDFB $(FUSION_CFLAGS) $(PNG_CFLAGS) $(WEBKIT_CFLAGS) $(DFBINT_CFLAGS)
libevebrowser_la_CXXFLAGS = -std=c99 -DDFB $(FUSION_CFLAGS) $(PNG_CFLAGS) $(WEBKIT_CFLAGS) $(DFBINT_CFLAGS)
libevebrowser_la_LIBADD = $(FUSION_LIBS) $(PNG_LIBS) $(WEBKIT_LIBS) $(DFBINT_LIBS)





