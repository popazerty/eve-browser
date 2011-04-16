#ifndef JS_EXTENSIONS_H_
#define JS_EXTENSIONS_H_

#ifdef GTK
void registerSpecialJsFunctions(WebKitWebView* web_view);
#endif

#ifdef GTK
void registerJsFunctions(WebKitWebView* web_view, int (*fnc)(int type));
#else //DFB
void registerJsFunctions(LiteWebView* web_view, int (*fnc)(int type));
#endif

#endif
