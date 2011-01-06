/** 
This file contains the js <-> c bindings 
**/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#include <webkit/webkit.h>
#include <JavaScriptCore/JavaScript.h>

#include "js_debug.h"

/******************************************/

// Registering single funxtions
void register_javascript_function(WebKitWebView* web_view, const char *name, JSObjectCallAsFunctionCallback callback)
{
    WebKitWebFrame *frame = webkit_web_view_get_main_frame(WEBKIT_WEB_VIEW(web_view));
    JSContextRef ctx = webkit_web_frame_get_global_context(frame);
    JSObjectRef global = JSContextGetGlobalObject(ctx);
    JSObjectRef func = JSObjectMakeFunctionWithCallback(ctx, NULL, callback);
    JSStringRef jsname = JSStringCreateWithUTF8CString(name);
    JSObjectSetProperty(ctx, global, jsname, func,0, NULL);
    JSStringRelease(jsname);
}


/******************************************/

// HTMLObjectElement - Bindings
// Visibility setting is a workaround to get video object invisible 
char s_o_bindToCurrentChannel[] = 
    "HTMLObjectElement.prototype.bindToCurrentChannel = function() { \
    c_o_bindToCurrentChannel(); \
    document.getElementById(\"video\").style.visibility = \"hidden\"; \
}";

JSValueRef
c_o_bindToCurrentChannel (JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, 
    size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    /*printf("%s - CALLED (argumentCount=%d)\n", __func__, argumentCount);

    printJSObjectRef(ctx, function);

    printJSObjectRef(ctx, thisObject);

    for(unsigned int i = 0; i < argumentCount; i++)
    {
        printJSValueRef(ctx, arguments[i]);
    }*/

    return NULL;
}

char s_o_getChannelConfig[] = 
"HTMLObjectElement.prototype.getChannelConfig = function() {\
    c_o_getChannelConfig(); \
	return new Channel();\
}";

JSValueRef
c_o_getChannelConfig (JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, 
    size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    printf("%s - CALLED\n", __func__);
    return NULL;
}

char s_o_play[] = 
"HTMLObjectElement.prototype.play = function(speed) {\
    c_o_play(speed, this.data); \
}";

JSValueRef
c_o_play (JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, 
    size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    printf("%s - CALLED (argumentCount=%d)\n", __func__, argumentCount);

    for(unsigned int i = 0; i < argumentCount; i++)
    {
        printJSValueRef(ctx, arguments[i], exception);
    }

    return NULL;
}

char s_o_stop[] = 
"HTMLObjectElement.prototype.stop = function() {\
    c_o_stop(); \
}";

JSValueRef
c_o_stop (JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, 
    size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    printf("%s - CALLED (argumentCount=%d)\n", __func__, argumentCount);

    for(unsigned int i = 0; i < argumentCount; i++)
    {
        printJSValueRef(ctx, arguments[i], exception);
    }

    return NULL;
}

char s_o_release[] = 
"HTMLObjectElement.prototype.release = function() {\
    c_o_release(); \
}";

JSValueRef
c_o_release (JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, 
    size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    printf("%s - CALLED (argumentCount=%d)\n", __func__, argumentCount);

    for(unsigned int i = 0; i < argumentCount; i++)
    {
        printJSValueRef(ctx, arguments[i], exception);
    }

    return NULL;
}

char s_o_setFullScreen[] = 
"HTMLObjectElement.prototype.setFullScreen = function(show) {\
    c_o_setFullScreen(show); \
}";

JSValueRef
c_o_setFullScreen (JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, 
    size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    printf("%s - CALLED (argumentCount=%d)\n", __func__, argumentCount);

    for(unsigned int i = 0; i < argumentCount; i++)
    {
        printJSValueRef(ctx, arguments[i], exception);
    }

    return NULL;
}

char s_o_seek[] = 
"HTMLObjectElement.prototype.seek = function(millis) {\
    c_o_seek(millis); \
}";

JSValueRef
c_o_seek (JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, 
    size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    printf("%s - CALLED (argumentCount=%d)\n", __func__, argumentCount);

    for(unsigned int i = 0; i < argumentCount; i++)
    {
        printJSValueRef(ctx, arguments[i], exception);
    }

    return NULL;
}

///////////////////////////////////77

void registerJsFunctions(WebKitWebView* web_view)
{
    webkit_web_view_execute_script(web_view, s_o_bindToCurrentChannel);
    register_javascript_function(web_view, "c_o_bindToCurrentChannel", c_o_bindToCurrentChannel);

    webkit_web_view_execute_script(web_view, s_o_play);
    register_javascript_function(web_view, "c_o_play", c_o_play);

    webkit_web_view_execute_script(web_view, s_o_stop);
    register_javascript_function(web_view, "c_o_stop", c_o_stop);

    webkit_web_view_execute_script(web_view, s_o_release);
    register_javascript_function(web_view, "c_o_release", c_o_release);

    webkit_web_view_execute_script(web_view, s_o_setFullScreen);
    register_javascript_function(web_view, "c_o_setFullScreen", c_o_setFullScreen);

    webkit_web_view_execute_script(web_view, s_o_seek);
    register_javascript_function(web_view, "c_o_seek", c_o_seek);
}

// This function can be used to force displaying hbbtvlib errors
void registerSpecialJsFunctions(WebKitWebView* web_view)
{
    char scriptError[] = "alert(hbbtvlib_lastError);";
    webkit_web_view_execute_script(web_view, scriptError);
}

