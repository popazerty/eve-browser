/** 
This file contains the js <-> c bindings 
**/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#ifdef GTK
#include <webkit/webkit.h>
#endif
#ifdef DFB
#include <webkit/webkitdfb.h>
#include <glib.h>
#include <webkit/webview.h>
#endif

#include <JavaScriptCore/JavaScript.h>

#include "js_debug.h"

static int (*g_Callback)(int type) = NULL;

/******************************************/

#ifdef DFB
void webkit_web_view_execute_script(LiteWebView* web_view, char* script)
{
    lite_webview_execute_script(web_view, script);
}
#endif

// Registering single funxtions
#ifdef GTK
void register_javascript_function(WebKitWebView* web_view, const char *name, JSObjectCallAsFunctionCallback callback)
#endif
#ifdef DFB
void register_javascript_function(LiteWebView* web_view, const char *name, JSObjectCallAsFunctionCallback callback)
#endif
{
#ifdef GTK
    WebKitWebFrame *frame = webkit_web_view_get_main_frame(WEBKIT_WEB_VIEW(web_view));
    JSContextRef ctx = webkit_web_frame_get_global_context(frame);
#endif
#ifdef DFB
    JSContextRef ctx = lite_webview_get_global_context(web_view);
#endif
    JSObjectRef global = JSContextGetGlobalObject(ctx);
    JSObjectRef func = JSObjectMakeFunctionWithCallback(ctx, NULL, callback);
    JSStringRef jsname = JSStringCreateWithUTF8CString(name);
    JSObjectSetProperty(ctx, global, jsname, func,0, NULL);
    JSStringRelease(jsname);
}


/******************************************/

char s_o_ApplicationManager[] = 
"function ApplicationManager() {\
}";


char s_o_getOwnerApplication[] = 
"HTMLObjectElement.prototype.getOwnerApplication = function(document) {\
    c_o_getOwnerApplication(document); \
    return new ApplicationManager(); \
}";

JSValueRef
c_o_getOwnerApplication (JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, 
    size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    printf("%s - CALLED (argumentCount=%d)\n", __func__, argumentCount);

    for(unsigned int i = 0; i < argumentCount; i++)
    {
        printJSValueRef(ctx, arguments[i], exception);
    }

    return NULL;
}


char s_o_ApplicationManager_createApplication[] = 
"ApplicationManager.prototype.createApplication = function(url, unknown) {\
    window.location.href = url; \
    c_o_ApplicationManager_createApplication(url, unknown); \
    return true; \
}";

JSValueRef
c_o_ApplicationManager_createApplication (JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, 
    size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    printf("%s - CALLED (argumentCount=%d)\n", __func__, argumentCount);

    for(unsigned int i = 0; i < argumentCount; i++)
    {
        printJSValueRef(ctx, arguments[i], exception);
    }

    return NULL;
}

char s_o_ApplicationManager_destroyApplication[] = 
"ApplicationManager.prototype.destroyApplication = function() {\
    c_o_ApplicationManager_destroyApplication(); \
    return true; \
}";

JSValueRef
c_o_ApplicationManager_destroyApplication (JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, 
    size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    printf("%s - CALLED (argumentCount=%d)\n", __func__, argumentCount);

    for(unsigned int i = 0; i < argumentCount; i++)
    {
        printJSValueRef(ctx, arguments[i], exception);
    }

    return NULL;
}

char s_o_ApplicationManager_show[] = 
"ApplicationManager.prototype.show = function() {\
    c_o_ApplicationManager_show(); \
    return true; \
}";

JSValueRef
c_o_ApplicationManager_show (JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, 
    size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    printf("%s - CALLED (argumentCount=%d)\n", __func__, argumentCount);

    for(unsigned int i = 0; i < argumentCount; i++)
    {
        printJSValueRef(ctx, arguments[i], exception);
    }

    return NULL;
}

char s_o_ApplicationManager_hide[] = 
"ApplicationManager.prototype.hide = function() {\
    c_o_ApplicationManager_hide(); \
    return true; \
}";

JSValueRef
c_o_ApplicationManager_hide (JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, 
    size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    printf("%s - CALLED (argumentCount=%d)\n", __func__, argumentCount);

    for(unsigned int i = 0; i < argumentCount; i++)
    {
        printJSValueRef(ctx, arguments[i], exception);
    }

    return NULL;
}

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
    printf("%s - CALLED (argumentCount=%d)\n", __func__, argumentCount);

    for(unsigned int i = 0; i < argumentCount; i++)
    {
        printJSValueRef(ctx, arguments[i], exception);
    }

    if(g_Callback != NULL)
        g_Callback(100);

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

    /*for(unsigned int i = 0; i < argumentCount; i++)
    {
        printJSValueRef(ctx, arguments[i], exception);
    }*/

    if(argumentCount == 2) {
        float speed = JSValueToNumber(ctx, arguments[0], exception);
        char * url = NULL;
        int urlLen = JSValueToString(ctx, arguments[1], exception, &url);
        if(urlLen > 0) {
            char urlZdfBad[] = "http://www.metafilegenerator.de/ondemand/zdf/hbbtv/geoloc_zdf-none/";
            if(!strncmp(url, urlZdfBad, strlen(urlZdfBad)))
            {
                char urlZdf[] = "mms://ondemand.msmedia.zdf.newmedia.nacamar.net/zdf/data/msmedia/zdf/";
                int urlFixedLen = strlen(urlZdf) + strlen(url) - strlen(urlZdfBad);
                char urlFixed[urlFixedLen];
                strncpy(urlFixed, urlZdf, strlen(urlZdf));
                strncpy(urlFixed + strlen(urlZdf), url + strlen(urlZdfBad), strlen(url) - strlen(urlZdfBad));

                free(url);
                url = (char*)malloc(sizeof(char) * (urlFixedLen+2));
                strncpy(url, urlFixed, urlFixedLen);
                url[urlFixedLen - 3] = 'w';
                url[urlFixedLen - 2] = 'm';
                url[urlFixedLen - 1] = 'v';
                url[urlFixedLen] = '\0';
                url[urlFixedLen+1] = '\0'; // bug in libeplayer3 for mms
            }
            printf("%s:%s[%d] speed=%f url=%s [%d]\n", __FILE__, __func__, __LINE__, speed, url, urlLen);
        }
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

#ifdef GTK
void registerJsFunctions(WebKitWebView* web_view, int (*fnc)(int type))
#endif
#ifdef DFB
void registerJsFunctions(LiteWebView* web_view, int (*fnc)(int type))
#endif
{
    g_Callback = fnc;

    webkit_web_view_execute_script(web_view, s_o_getOwnerApplication);
    register_javascript_function(web_view, "c_o_getOwnerApplication", c_o_getOwnerApplication);

    webkit_web_view_execute_script(web_view, s_o_ApplicationManager);
    //register_javascript_function(web_view, "c_o_ApplicationManager", c_o_ApplicationManager);

    webkit_web_view_execute_script(web_view, s_o_ApplicationManager_createApplication);
    register_javascript_function(web_view, "c_o_ApplicationManager_createApplication", c_o_ApplicationManager_createApplication);

    webkit_web_view_execute_script(web_view, s_o_ApplicationManager_destroyApplication);
    register_javascript_function(web_view, "c_o_ApplicationManager_destroyApplication", c_o_ApplicationManager_destroyApplication);

    webkit_web_view_execute_script(web_view, s_o_ApplicationManager_show);
    register_javascript_function(web_view, "c_o_ApplicationManager_show", c_o_ApplicationManager_show);

    webkit_web_view_execute_script(web_view, s_o_ApplicationManager_hide);
    register_javascript_function(web_view, "c_o_ApplicationManager_hide", c_o_ApplicationManager_hide);

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
#ifdef GTK
void registerSpecialJsFunctions(WebKitWebView* web_view)
#endif
#ifdef DFB
void registerSpecialJsFunctions(LiteWebView* web_view)
#endif
{
    char scriptError[] = "alert(hbbtvlib_lastError);";
    webkit_web_view_execute_script(web_view, scriptError);
}

