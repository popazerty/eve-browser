#ifndef JS_EXTENSIONS_H_
#define JS_EXTENSIONS_H_

void registerJsFunctions(WebKitWebView* web_view, int (*fnc)(int type));
void registerSpecialJsFunctions(WebKitWebView* web_view);

#endif
