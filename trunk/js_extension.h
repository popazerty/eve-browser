#ifndef JS_EXTENSIONS_H_
#define JS_EXTENSIONS_H_

#ifdef DFB
#include <webkit/webkitdfb.h>
#include <webkit/webview.h>
#endif

#ifdef GTK
void registerSpecialJsFunctions(WebKitWebView* web_view);
#endif

#ifdef GTK
void registerJsFunctions(WebKitWebView* web_view, int (*fnc)(int type));
#endif
#ifdef DFB
void registerJsFunctions(LiteWebView* web_view, int (*fnc)(int type));
#endif

#endif
