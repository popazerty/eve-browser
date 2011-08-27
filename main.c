#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <string.h>
#include <pthread.h>
#include <linux/input.h>
#include <unistd.h>

#ifdef GTK
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <webkit/webkit.h>
#endif

#ifdef DFB
#include <direct/thread.h>

#include <glib.h>
#include <glib-object.h>

#include <lite/lite.h>
#include <lite/window.h>

#include <leck/textbutton.h>
#include <leck/textline.h>

#include <webkit/webkitdfb.h>
#include <webkit/webview.h>

#include <core/input.h>
#endif

#include <JavaScriptCore/JavaScript.h>

#include "js_extension.h"
#include "css_extension.h"

static int (*g_Callback)(int type);
static pthread_t g_BrowserMain;
static char g_url[1024] = "http://itv.ard.de/ardtext/";
static unsigned int g_framebuffer_width = 1280;
static unsigned int g_framebuffer_height = 720;
static int g_run = 1;

#ifdef DFB
static LiteWindow     *g_window   = NULL;
static LiteWebView    *g_webview  = NULL;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static IDirectFBWindow *g_dfb_window = NULL;
#endif

////////////////////////////////////////////

//Should be changed to setPage and loadPage
void loadPage(char * url)
{
    printf("%s URL = %s\n", __func__, url);
    strncpy(g_url, url, 1024);
/*#ifdef GTK
    webkit_web_view_load_uri (g_web_view, g_url);
#else //DFB
    lite_webview_load(g_webview, g_url);
#endif*/
}

void setDimension(int w, int h)
{
    printf("%s:%d\n", __func__, __LINE__);
    g_framebuffer_width = w;
    g_framebuffer_height = h;
}

void setCallback(int (*fnc)(int type))
{
    g_Callback = fnc;
    g_Callback(0);
}

#ifdef DFB
static DFBResult
on_key_press( DFBWindowEvent* evt, void *ctx ) {
printf(".");
   if (evt->key_code == 0 && evt->key_id != 0xf600)
   {     
    printf("on_key_press\n");
    /*printf("\tevt->clazz: %02x\n", evt->clazz);
    printf("\tevt->type: %02x\n", evt->type);
    printf("\tevt->flags: %02x\n", evt->flags);
    printf("\tevt->window_id: %02x\n", evt->window_id);
    printf("\tevt->key_code: %02x\n", evt->key_code);
    printf("\tevt->key_id: %02x\n", evt->key_id);
    printf("\tevt->key_symbol: %02x\n", evt->key_symbol);
    printf("\tevt->modifiers: %02x\n", evt->modifiers);
    printf("\tevt->locks: %02x\n", evt->locks);*/
      
      return DFB_OK;
   }

    evt->key_id = (DFBInputDeviceKeyIdentifier)0x0;

    return DFB_FAILURE;
}

#define KEY_TYPE_PRESS 0
#define KEY_TYPE_RELEASE 1

/*
on_key_press
        evt->clazz: 02
        evt->type: 100
        evt->flags: 00
        evt->window_id: 01
        evt->key_code: 6c
        evt->key_symbol: f003
        evt->modifiers: 00
        evt->locks: 00
on_key_press
        evt->clazz: 02
        evt->type: 200
        evt->flags: 00
        evt->window_id: 01
        evt->key_code: 6c
        evt->key_symbol: f003
        evt->modifiers: 00
        evt->locks: 00
keyPress down press
on_key_press
        evt->clazz: 00
        evt->type: 100
        evt->flags: 00
        evt->window_id: 00
        evt->key_code: 00
        evt->key_symbol: 00
        evt->modifiers: 01
        evt->locks: 00
keyPress down release
on_key_press
        evt->clazz: 00
        evt->type: 200
        evt->flags: 00
        evt->window_id: 00
        evt->key_code: 00
        evt->key_symbol: 00
        evt->modifiers: 01
        evt->locks: 00
*/

void keyPress(char * key, int type)
{
    printf("%s %s %s\n", __func__, key, type==0?"press":"release");

#if 1 //WindowEvent
    DFBWindowEvent* event = ( DFBWindowEvent*)malloc(sizeof(DFBWindowEvent));
    memset(event, 0, sizeof(DFBWindowEvent));

    event->clazz = DFEC_WINDOW;

    if(type == KEY_TYPE_PRESS)
        event->type = DWET_KEYDOWN;
    else if(type == KEY_TYPE_RELEASE)
        event->type = DWET_KEYUP;
    else
        return;

    event->flags    = DWEF_NONE;
    event->window_id = 1;
#else //InputEvent
    DFBInputEvent* event = ( DFBInputEvent*)malloc(sizeof(DFBInputEvent));
    memset(event, 0, sizeof(DFBInputEvent));
    
     if(type == KEY_TYPE_PRESS)
        event->type = DIET_KEYPRESS;
    else if(type == KEY_TYPE_RELEASE)
        event->type = DIET_KEYRELEASE;
    else
        return;
#endif

    //gettimeofday(&(event->timestamp), NULL);
   
    if     (!strcmp(key, "red")) {
        event->key_id = DIKI_F5; //'t';
        event->key_symbol =  DIKS_F5;
    } else if(!strcmp(key, "green")) {
        event->key_id = DIKI_F6; //'u';
        event->key_symbol =  DIKS_F6;
    } else if(!strcmp(key, "yellow")) {
        event->key_id = DIKI_F7; //'v';
        event->key_symbol =  DIKS_F7;
    } else if(!strcmp(key, "blue")) {
        event->key_id = DIKI_F8; //'w';
        event->key_symbol =  DIKS_F8;

    } else if(!strcmp(key, "up")) {
        event->key_id = DIKI_UP; //f643
        event->key_symbol =  DIKS_CURSOR_UP;
    } else if(!strcmp(key, "down")) {
        event->key_id = DIKI_DOWN;
        event->key_symbol =  DIKS_CURSOR_DOWN; //(DFBInputDeviceKeySymbol)0xf003;
    } else if(!strcmp(key, "left")) {
        event->key_id = DIKI_LEFT;
        event->key_symbol =  DIKS_CURSOR_LEFT;
    } else if(!strcmp(key, "right")) {
        event->key_id = DIKI_RIGHT;
        event->key_symbol =  DIKS_CURSOR_RIGHT;
    
    } else if(!strcmp(key, "ok")) {
        event->key_id = DIKI_ENTER;
        event->key_symbol =  DIKS_ENTER;
    
    } else if(!strcmp(key, "1")) {
        event->key_id = DIKI_1;
        event->key_symbol =  DIKS_2;
    } else if(!strcmp(key, "2")) {
        event->key_id = DIKI_2;
        event->key_symbol =  DIKS_2;
    } else if(!strcmp(key, "3")) {
        event->key_id = DIKI_3;
        event->key_symbol =  DIKS_3;
    } else if(!strcmp(key, "4")) {
        event->key_id = DIKI_4;
        event->key_symbol =  DIKS_4;
    } else if(!strcmp(key, "5")) {
        event->key_id = DIKI_5;
        event->key_symbol =  DIKS_5;
    } else if(!strcmp(key, "6")) {
        event->key_id = DIKI_6;
        event->key_symbol =  DIKS_6;
    } else if(!strcmp(key, "7")) {
        event->key_id = DIKI_7;
        event->key_symbol =  DIKS_7;
    } else if(!strcmp(key, "8")) {
        event->key_id = DIKI_8;
        event->key_symbol =  DIKS_8;
    } else if(!strcmp(key, "9")) {
        event->key_id = DIKI_9;
        event->key_symbol =  DIKS_9;
    } else if(!strcmp(key, "0")) {
        event->key_id = DIKI_0;
        event->key_symbol =  DIKS_0;

    //TODO: Confirm these
    } else if(!strcmp(key, "play")) {
        event->key_id = DIKI_P;
        event->key_symbol =  DIKS_CAPITAL_P;
    } else if(!strcmp(key, "pause")) {
        event->key_id = DIKI_P; // PAUSE IS Q but it seems that P is Toggle PlayPause
        event->key_symbol =  DIKS_CAPITAL_P;
    } else if(!strcmp(key, "stop")) {
        event->key_id = DIKI_S;
        event->key_symbol =  DIKS_CAPITAL_S;
    } else if(!strcmp(key, "rewind")) {
        event->key_id = DIKI_R;
        event->key_symbol =  DIKS_CAPITAL_R;
    } else if(!strcmp(key, "fastforward")) {
        event->key_id = DIKI_F;
        event->key_symbol =  DIKS_CAPITAL_F;
    } else {
        event->key_id = (DFBInputDeviceKeyIdentifier)0;
    }
    
    if(event->key_id != 0) {
        printf("Injecting: event.key_id=%02x\n", event->key_id);
        //pthread_mutex_lock(&mutex);
#if 0 // LITE_INJECTION_WORKS
        lite_webview_handleKeyboardEvent( g_webview, event );
#else
         if(g_dfb_window != NULL)
             g_dfb_window->SendEvent(g_dfb_window, event);
#endif
        //pthread_mutex_unlock(&mutex);
        return;
    }

    free(event);

    return;
}

void on_webview_doc_loaded( LiteWebView *webview, void *data )
{
   lite_webview_set_transparent(webview, true);
}

static int timer_id;

static DFBResult timeout_cb(void* data)
{
    g_main_context_iteration(NULL, FALSE);
    lite_enqueue_window_timeout(200, timeout_cb, NULL, &timer_id);
    return DFB_OK;
}

void *BrowserMain(void * argument)
{
    printf("%s:%d\n", __func__, __LINE__);

    int argc = 0;
    char**argv = NULL;


    pthread_mutex_init (&mutex, NULL);
    
    g_type_init();
    g_thread_init(NULL);

    lite_open( &argc, &argv );

    WebKitDFB_Initialize( lite_get_dfb_interface() );

    IDirectFBDisplayLayer *layer;
    DFBDisplayLayerConfig  config;
    lite_get_layer_interface( &layer );
    layer->GetConfiguration( layer, &config );

    DFBRectangle windowRect        = {   0,  0, config.width, config.height };
    DFBRectangle webviewRect       = {   0,  0, config.width, config.height };

    lite_new_window( NULL, &windowRect, DWCAPS_NONE, liteNoWindowTheme, "WebKitDFB", &g_window );
    
    lite_new_webview( LITE_BOX(g_window), &webviewRect, liteDefaultWebViewTheme, &g_webview);

    lite_on_webview_doc_loaded ( g_webview, on_webview_doc_loaded, NULL );

    lite_on_raw_window_keyboard(g_window, on_key_press, g_webview );

    lite_focus_box( LITE_BOX(g_webview) );

    lite_set_window_opacity(g_window, 0xff);

    g_window->bg.enabled = DFB_FALSE;
    //lite_set_window_background_color(g_window, 0xff, 0, 0, 0xff);

    registerJsFunctions(g_webview, g_Callback);

    lite_webview_load(g_webview, g_url);
    lite_webview_set_transparent(g_webview, true);

    // FAKE KEY INTERFACE
    //IDirectFB *dfb;
    //dfb = lite_get_dfb_interface();
    //IDirectFBDisplayLayer *layer = NULL;
    //dfb->GetDisplayLayer(dfb, DLID_PRIMARY, &layer);
    layer->GetWindow(layer, 1, &g_dfb_window);

    lite_enqueue_window_timeout(200, timeout_cb, NULL, &timer_id);
    g_run = 1;
    while (g_run) {
        pthread_mutex_lock(&mutex);

        g_main_context_iteration(NULL, FALSE);
        lite_window_event_loop(g_window, 1);
        pthread_mutex_unlock(&mutex);
    }

   lite_close();

   return NULL;
}

#include <unistd.h>
int
main (int argc, char* argv[])
{
    pthread_create(&g_BrowserMain, NULL, BrowserMain, NULL);
    //pthread_join(g_BrowserMain, NULL);
    while(1) {
    sleep(2);
    keyPress("blue", 0);
    keyPress("blue", 1);
    }

    return 0;
}

void loadEveBrowser()
{
}

void unloadEveBrowser()
{
    g_run = 0;
}

void show()
{
    pthread_create(&g_BrowserMain, NULL, BrowserMain, NULL);
}

void hide()
{
    printf("%s:%d\n", __func__, __LINE__);
    //gtk_widget_hide_all (g_window);
}

#endif

#ifdef GTK
static GtkWidget* uri_entry;


static GtkToolItem* itemUrl;
static GtkScrolledWindow* scrolled_window;
static guint status_context_id;






static GtkWidget*     g_window;
static GtkWidget*     g_vbox;
static WebKitWebView* g_web_view;
static GtkWidget*     g_toolbar;
static GtkStatusbar*  g_main_statusbar;


float g_default_scale = 1.0f;
///////////////////////////
///////////////////////////

static void window_object_cleared_cb(   WebKitWebView *frame,
                                        gpointer context,
                                        gpointer arg3,
                                        gpointer user_data)
{
    printf("window_object_cleared_cb\n");

    registerJsFunctions(g_web_view, g_Callback);
}

///////////////////////7

static void
activate_uri_entry_cb (GtkWidget* entry, gpointer data)
{
    const gchar* uri = gtk_entry_get_text (GTK_ENTRY (entry));
    g_assert (uri);
    webkit_web_view_load_uri (g_web_view, uri);
}

static void
link_hover_cb (WebKitWebView* page, const gchar* title, const gchar* link, gpointer data)
{
    /* underflow is allowed */
    gtk_statusbar_pop (g_main_statusbar, status_context_id);
    if (link)
        gtk_statusbar_push (g_main_statusbar, status_context_id, link);
}

static void
notify_load_status_cb (WebKitWebView* web_view, GParamSpec* pspec, gpointer data)
{
    if (webkit_web_view_get_load_status (web_view) == WEBKIT_LOAD_COMMITTED) {
        WebKitWebFrame* frame = webkit_web_view_get_main_frame (web_view);
        const gchar* uri = webkit_web_frame_get_uri (frame);
        if (uri)
            gtk_entry_set_text (GTK_ENTRY (uri_entry), uri);
    }
}

static void
destroy_cb (GtkWidget* widget, gpointer data)
{
    gtk_main_quit ();
}


static gboolean
focus_out_cb (GtkWidget* widget, GdkEvent * event, gpointer data)
{
    printf("%s > \n", __func__);
    gtk_widget_grab_focus(widget);
    return false;
}


static void
document_load_finished_cb (GtkWidget* widget, WebKitWebFrame * arg1, gpointer data)
{
    registerCssExtension(g_web_view);

    // Only use if debugging is needed
    //registerSpecialJsFunctions(g_web_view);
}


void goBack()
{
    webkit_web_view_go_back(g_web_view);
}

void gtk_widget_set_can_focus(GtkWidget* wid, gboolean can)
{
    if(can)
        GTK_WIDGET_SET_FLAGS(wid, GTK_CAN_FOCUS);
    else
        GTK_WIDGET_UNSET_FLAGS(wid, GTK_CAN_FOCUS);
}

static GtkScrolledWindow*
create_browser ()
{
    scrolled_window = (GtkScrolledWindow*)gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    g_web_view = WEBKIT_WEB_VIEW (webkit_web_view_new ());
    webkit_web_view_set_transparent(g_web_view, true);
    gtk_container_add (GTK_CONTAINER (scrolled_window), GTK_WIDGET (g_web_view));
    webkit_web_view_set_full_content_zoom(g_web_view, true);

    g_signal_connect (g_web_view, "notify::load-status", G_CALLBACK (notify_load_status_cb), g_web_view);
    g_signal_connect (g_web_view, "hovering-over-link", G_CALLBACK (link_hover_cb), g_web_view);

    g_signal_connect (g_web_view, "focus-out-event", G_CALLBACK (focus_out_cb), g_web_view);

    g_signal_connect (g_web_view, "document-load-finished", G_CALLBACK (document_load_finished_cb), g_web_view);

    g_signal_connect (g_web_view, "window_object_cleared", G_CALLBACK (window_object_cleared_cb), g_web_view);

    return scrolled_window;
}

static GtkWidget*
create_statusbar ()void *GtkMain(void * argument)
{
    printf("%s:%d\n", __func__, __LINE__);

    int argc = 0;
    char**argv = NULL;

    unsigned char haveUrl = 0;
    int argCount = 0;
{
    g_main_statusbar = GTK_STATUSBAR (gtk_statusbar_new ());
    gtk_widget_set_can_focus(GTK_WIDGET (g_main_statusbar), false);
    status_context_id = gtk_statusbar_get_context_id (g_main_statusbar, "Link Hover");
    
    return (GtkWidget*)g_main_statusbar;
}

static GtkWidget*
create_toolbar ()
{
    g_toolbar = gtk_toolbar_new ();
    gtk_widget_set_can_focus(GTK_WIDGET (g_toolbar), false);

#if GTK_CHECK_VERSION(2,15,0)
    gtk_orientable_set_orientation (GTK_ORIENTABLE (g_toolbar), GTK_ORIENTATION_HORIZONTAL);
#else
    gtk_toolbar_set_orientation (GTK_TOOLBAR (g_toolbar), GTK_ORIENTATION_HORIZONTAL);
#endif
    gtk_toolbar_set_style (GTK_TOOLBAR (g_toolbar), GTK_TOOLBAR_BOTH_HORIZ);


    /* The URL entry */
    itemUrl = gtk_tool_item_new ();
    gtk_widget_set_can_focus(GTK_WIDGET (itemUrl), false);
    gtk_tool_item_set_expand (itemUrl, TRUE);
    uri_entry = gtk_entry_new ();
    gtk_container_add (GTK_CONTAINER (itemUrl), uri_entry);
    g_signal_connect (G_OBJECT (uri_entry), "activate", G_CALLBACK (activate_uri_entry_cb), NULL);
    gtk_toolbar_insert (GTK_TOOLBAR (g_toolbar), itemUrl, -1);

    return g_toolbar;
}

/**
 * This toogles the background of the window
 **/
static void
toogleBackground (void)
{
    printf("%s > \n", __func__);
    static gboolean isTransparent = false;
    isTransparent = !isTransparent;
    webkit_web_view_set_transparent(g_web_view, isTransparent);
}

static gboolean isShown = true;

static void
toogleMode (void)
{
    printf("%s > \n", __func__);
    if(isShown)
    {
        gtk_widget_set_size_request(g_vbox, g_framebuffer_width, g_framebuffer_height);
        gtk_widget_hide(GTK_WIDGET (g_main_statusbar));
        gtk_widget_hide(GTK_WIDGET (g_toolbar));
    }
    else
    {
        gtk_widget_set_size_request(g_vbox, g_framebuffer_width-200, g_framebuffer_height);
        gtk_widget_show(GTK_WIDGET (g_main_statusbar));
        gtk_widget_show(GTK_WIDGET (g_toolbar));

        gtk_widget_grab_focus(GTK_WIDGET (itemUrl));
    }
    isShown = !isShown;
}

static gboolean gIsNumLock = false;
static void toogleNumLock()
{
    gIsNumLock = !gIsNumLock;
}

static gboolean gIsZoomLock = false;
static void toggleZoomLock()
{
    gIsZoomLock = !gIsZoomLock;
}

static void handleZoomLock(int value)
{
    if(value > 0)
        webkit_web_view_zoom_in(g_web_view);
    else if(value < 0)
        webkit_web_view_zoom_out(g_web_view);
    else
        webkit_web_view_set_zoom_level(g_web_view, g_default_scale );
}

static gboolean
on_key_press (GtkWidget* widget, GdkEventKey *event, gpointer data)
{

if(event->send_event == 0)
    return true;

printf("POST----------\n");
printf("type = %d\n", event->type);
printf("window = %8x\n", event->window);
printf("send_event = %d\n", event->send_event );
printf("time = %8x\n", event->time);
printf("state = %d\n", event->state);
printf("keyval = %d\n", event->keyval);
//printf("length = %d\n", event->length);
//printf("string = %s\n", event->string);
printf("hardware_keycode = %d\n", event->hardware_keycode);
printf("group = %d\n", event->group);
printf("is_modifier = %d\n", event->is_modifier);

    if (event->type == GDK_KEY_PRESS)
    {

        if(gIsZoomLock)
        {
            switch(event->keyval)
            {
            case GDK_Up:     handleZoomLock(+1); break;
            case GDK_Return: handleZoomLock(0); break;
            case GDK_Down:   handleZoomLock(-1); break;
            default: break;
            }
        }

        if (event->keyval == GDK_F1)
        {
            toogleMode();
        }
        else if (isShown && event->keyval == GDK_F2)
        {
            //toogleBackground();
            toggleZoomLock();
        }
        else if (isShown && event->keyval == GDK_Num_Lock)
        {
            toogleNumLock();
        }
        else if (isShown && event->keyval == GDK_F4)
        {
            gboolean rtv;
            event->keyval = GDK_Tab;
            gtk_signal_emit_by_name(GTK_OBJECT (g_window), "key-press-event", event, &rtv);
            return true;
        }
        else if (isShown && event->keyval == GDK_F3)
        {
            gboolean rtv;
            event->keyval = GDK_Tab;
            event->state |= GDK_SHIFT_MASK;
            gtk_signal_emit_by_name(GTK_OBJECT (g_window), "key-press-event", event, &rtv);
            return true;
        }
        else if (isShown && event->keyval >= 0xFFB0 && event->keyval <= 0xFFB9)
        {
            gIsNumLock = true;
            //event->state |= GDK_MOD1_MASK;
            switch(event->keyval) {
            case GDK_KP_1: event->keyval = '.'; break;
            case GDK_KP_2: event->keyval = 'a'; break;
            case GDK_KP_3: event->keyval = 'd'; break;
            case GDK_KP_4: event->keyval = 'g'; break;
            case GDK_KP_5: event->keyval = 'j'; break;
            case GDK_KP_6: event->keyval = 'm'; break;
            case GDK_KP_7: event->keyval = 'p'; break;
            case GDK_KP_8: event->keyval = 't'; break;
            case GDK_KP_9: event->keyval = 'w'; break;
            case GDK_KP_0: event->keyval = '+'; break;
            default: break;
            }
        }
        else if (isShown && event->keyval == GDK_KP_Down )
        {
            gboolean rtv;
            event->keyval = '2';
            event->state |= GDK_MOD1_MASK;
            gtk_signal_emit_by_name(GTK_OBJECT (g_window), "key-press-event", event, &rtv);
            return true;
        }
        else if (isShown && event->keyval == GDK_KP_Up )
        {
            gboolean rtv;
            event->keyval = '8';
            event->state |= GDK_MOD1_MASK;
            gtk_signal_emit_by_name(GTK_OBJECT (g_window), "key-press-event", event, &rtv);
            return true;
        }
        else if (isShown && event->keyval == GDK_KP_Left )
        {
            gboolean rtv;
            event->keyval = '4';
            event->state |= GDK_MOD1_MASK;
            gtk_signal_emit_by_name(GTK_OBJECT (g_window), "key-press-event", event, &rtv);
            return true;
        }
        else if (isShown && event->keyval == GDK_KP_Right )
        {
            gboolean rtv;
            event->keyval = '6';
            event->state |= GDK_MOD1_MASK;
            gtk_signal_emit_by_name(GTK_OBJECT (g_window), "key-press-event", event, &rtv);
            return true;
        }
        else if (isShown && event->keyval == GDK_KP_Home )
        {
            gboolean rtv;
            event->keyval = '7';
            event->state |= GDK_MOD1_MASK;
            gtk_signal_emit_by_name(GTK_OBJECT (g_window), "key-press-event", event, &rtv);
            return true;
        }
        else if (isShown && event->keyval == GDK_KP_Page_Up )
        {
            gboolean rtv;
            event->keyval = '9';
            event->state |= GDK_MOD1_MASK;
            gtk_signal_emit_by_name(GTK_OBJECT (g_window), "key-press-event", event, &rtv);
            return true;
        }
        else if (isShown && event->keyval == GDK_KP_End )
        {
            gboolean rtv;
            event->keyval = '1';
            event->state |= GDK_MOD1_MASK;
            gtk_signal_emit_by_name(GTK_OBJECT (g_window), "key-press-event", event, &rtv);
            return true;
        }
        else if (isShown && event->keyval == GDK_KP_Page_Down )
        {
            gboolean rtv;
            event->keyval = '3';
            event->state |= GDK_MOD1_MASK;
            gtk_signal_emit_by_name(GTK_OBJECT (g_window), "key-press-event", event, &rtv);
            return true;
        }
        else if (isShown && (event->keyval == GDK_KP_Insert || event->keyval == 0xFFFFFF))
        {
            gboolean rtv;
            event->keyval = '0';
            event->state |= GDK_MOD1_MASK;
            gtk_signal_emit_by_name(GTK_OBJECT (g_window), "key-press-event", event, &rtv);
            return true;
        }
        else if (isShown && event->keyval == GDK_KP_Begin )
        {
            gboolean rtv;
            event->keyval = '5';
            event->state |= GDK_MOD1_MASK;
            gtk_signal_emit_by_name(GTK_OBJECT (g_window), "key-press-event", event, &rtv);
            return true;
        }
        else if (event->keyval == GDK_BackSpace)
            goBack();
    }
    return false;
}


static gboolean expose_event(GtkWidget * widget, GdkEventExpose * event)
{
    printf("%s\n", __func__);
    cairo_t *cr;

    cr = gdk_cairo_create(GDK_DRAWABLE(widget->window));
    gdk_cairo_region(cr, event->region);
    cairo_clip(cr);

    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_source_rgba(cr, 0.5, 0.5, 0.5, 1.0);
    cairo_paint(cr);
    cairo_destroy(cr);
    return false;
}

static GtkWidget*
create_window ()
{
    g_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size (GTK_WINDOW (g_window), g_framebuffer_width, g_framebuffer_height);
    gtk_widget_set_name (g_window, "eve-browser");
    gtk_window_set_decorated(GTK_WINDOW(g_window), false);

    GdkScreen *screen = gtk_widget_get_screen(g_window);
    gtk_widget_set_colormap(g_window, gdk_screen_get_rgba_colormap(screen));
    gtk_widget_set_app_paintable(g_window, true);
    gtk_widget_realize(g_window);
    gdk_window_set_back_pixmap(g_window->window, NULL, false);

    g_signal_connect(g_window, "expose-event", G_CALLBACK(expose_event), g_window);

    g_signal_connect (g_window, "destroy", G_CALLBACK (destroy_cb), NULL);
    g_signal_connect (g_window, "key-press-event", G_CALLBACK (on_key_press), NULL);
    g_signal_connect (g_window, "key-release-event", G_CALLBACK (on_key_press), NULL);

    //g_signal_connect (g_window, "screen-changed", G_CALLBACK (screen_changed), NULL);
    return g_window;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#define KEY_TYPE_PRESS 0
#define KEY_TYPE_RELEASE 1

void keyPress(char * key, int type)
{
    printf("%s %s\n", __func__, key);
    gboolean rtv;
/*
  GdkEventType type;
  GdkWindow *window;
  gint8 send_event;
  guint32 time;
  guint state;
  guint keyval;
  gint length;
  gchar *string;
  guint16 hardware_keycode;
  guint8 group;
  guint is_modifier : 1;
*/

    //GdkEventKey * event = (GdkEventKey*)malloc(sizeof(GdkEventKey));
    GdkEvent* event = gdk_event_new(GDK_KEY_PRESS); 


    if(type == KEY_TYPE_PRESS)
        ((GdkEventKey*)event)->type = GDK_KEY_PRESS; 
    else if(type == KEY_TYPE_RELEASE)    
        ((GdkEventKey*)event)->type = GDK_KEY_RELEASE;
    else
        return;
    
    ((GdkEventKey*)event)->window = g_window->window;  

    ((GdkEventKey*)event)->send_event = 1;
    ((GdkEventKey*)event)->time = GDK_CURRENT_TIME;
    ((GdkEventKey*)event)->state = 0;
    
    ((GdkEventKey*)event)->keyval = 0;
    //((GdkEventKey*)event)->length = 1;
    
    if     (!strcmp(key, "red"))
        ((GdkEventKey*)event)->keyval = GDK_F5; //'t';
    else if(!strcmp(key, "green"))
        ((GdkEventKey*)event)->keyval = GDK_F6; //'u';
    else if(!strcmp(key, "yellow"))
        ((GdkEventKey*)event)->keyval = GDK_F7; //'v';
    else if(!strcmp(key, "blue"))
        ((GdkEventKey*)event)->keyval = GDK_F8; //'w';

    else if(!strcmp(key, "up"))
        ((GdkEventKey*)event)->keyval = GDK_Up;
    else if(!strcmp(key, "down"))
        ((GdkEventKey*)event)->keyval = GDK_Down;
    else if(!strcmp(key, "left"))
        ((GdkEventKey*)event)->keyval = GDK_Left;
    else if(!strcmp(key, "right"))
        ((GdkEventKey*)event)->keyval = GDK_Right;
    
    else if(!strcmp(key, "ok"))
        ((GdkEventKey*)event)->keyval = GDK_Return;
    else if(!strcmp(key, "play"))
        ((GdkEventKey*)event)->keyval = 'P';
    else if(!strcmp(key, "pause"))
        ((GdkEventKey*)event)->keyval = 'P'; // PAUSE IS Q but it seems that P is Toggle PlayPause
    else if(!strcmp(key, "stop"))
        ((GdkEventKey*)event)->keyval = 'S';
    else if(!strcmp(key, "rewind"))
        ((GdkEventKey*)event)->keyval = 'R';
    else if(!strcmp(key, "fastforward"))
        ((GdkEventKey*)event)->keyval = 'F';


    else {
        GdkKeymapKey* keys; 
        gint n_keys; 
        gdk_keymap_get_entries_for_keyval(gdk_keymap_get_default(), ((GdkEventKey*)event)->keyval, &keys, &n_keys); 

        ((GdkEventKey*)event)->hardware_keycode = keys[0].keycode; 
    }

    ((GdkEventKey*)event)->group = 0;
    ((GdkEventKey*)event)->is_modifier = 0;
    
  
    if(((GdkEventKey*)event)->keyval != 0) {
        //if(type == KEY_TYPE_PRESS) {
            //gtk_signal_emit_by_name(GTK_OBJECT (g_window), "key-press-event", event, &rtv);
            gdk_event_put(event); 
        //}
        //else if(type == KEY_TYPE_RELEASE)   
        //    gtk_signal_emit_by_name(GTK_OBJECT (g_window), "key-release-event", event, &rtv);
        return;
    }

    //event->state |= GDK_MOD1_MASK;
    return;
}



void loadEveBrowser()
{
    int argc = 0;
    printf("%s:%d\n", __func__, __LINE__);
    gtk_init_check(&argc, NULL);
    if (!g_thread_supported ())
        g_thread_init (NULL);
    
    printf("%s:%d\n", __func__, __LINE__);

    GtkWidget* fixed = gtk_fixed_new();
    g_vbox = gtk_vbox_new (FALSE, 0);
    gtk_box_pack_start (GTK_BOX (g_vbox), create_toolbar (), FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (g_vbox), GTK_WIDGET (create_browser ()), TRUE, TRUE, 0);
    gtk_box_pack_start (GTK_BOX (g_vbox), create_statusbar (), FALSE, FALSE, 0);
    
    g_window = create_window ();
    
    gtk_fixed_put(GTK_FIXED(fixed), g_vbox, 0, 0);
    gtk_widget_set_size_request(g_vbox, g_framebuffer_width, g_framebuffer_height);
    
    GtkWidget* statusLabel = gtk_label_new ("Status");
    gtk_fixed_put(GTK_FIXED(fixed), statusLabel, g_framebuffer_width - 200, 0);
    gtk_widget_set_size_request(statusLabel, 200, 100);
    
    gtk_container_add (GTK_CONTAINER (g_window), fixed);
}

void unloadEveBrowser()
{
    gtk_main_quit();
}

int main (int argc, char* argv[]);


void *GtkMain(void * argument)
{
    printf("%s:%d\n", __func__, __LINE__);

    int argc = 0;
    char**argv = NULL;

    unsigned char haveUrl = 0;
    int argCount = 0;

    gtk_init (&argc, &argv);
    if (!g_thread_supported ())
        g_thread_init (NULL);

    GtkWidget* fixed = gtk_fixed_new();
    //screen_changed(fixed, NULL, NULL);
    g_vbox = gtk_vbox_new (FALSE, 0);
    gtk_box_pack_start (GTK_BOX (g_vbox), create_toolbar (), FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (g_vbox), GTK_WIDGET (create_browser ()), TRUE, TRUE, 0);
    gtk_box_pack_start (GTK_BOX (g_vbox), create_statusbar (), FALSE, FALSE, 0);

    g_window = create_window ();

    gtk_fixed_put(GTK_FIXED(fixed), g_vbox, 0, 0);
    gtk_widget_set_size_request(g_vbox, g_framebuffer_width, g_framebuffer_height);

    GtkWidget* statusLabel = gtk_label_new ("Status");
    gtk_fixed_put(GTK_FIXED(fixed), statusLabel, g_framebuffer_width - 200, 0);
    gtk_widget_set_size_request(statusLabel, 200, 100);

    gtk_container_add (GTK_CONTAINER (g_window), fixed);

    webkit_web_view_load_uri (g_web_view, g_url);

    gtk_widget_grab_focus (GTK_WIDGET (g_web_view));
    gtk_widget_show_all (g_window);

    toogleMode();

    g_default_scale = g_framebuffer_width / 1280.0f;
    handleZoomLock(0);

    g_Callback(1);

    gtk_main ();
    return NULL;
}

void show()
{


    pthread_create(&g_GtkMain, NULL, GtkMain, NULL);
}

void hide()
{
    printf("%s:%d\n", __func__, __LINE__);
    gtk_widget_hide_all (g_window);
}





int
main (int argc, char* argv[])
{
    pthread_create(&g_GtkMain, NULL, GtkMain, NULL);
    while(1);

    return 0;
}
#endif

