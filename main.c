#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#include <pthread.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <webkit/webkit.h>
#include <JavaScriptCore/JavaScript.h>

#include <linux/input.h>

#include "js_extension.h"
#include "css_extension.h"


static GtkWidget* uri_entry;


static GtkToolItem* itemUrl;
static GtkScrolledWindow* scrolled_window;

#if 0
static gchar* main_title;
static gdouble load_progress;
#endif
static guint status_context_id;

static pthread_t g_GtkMain;
static char g_url[1024];

static int (*g_Callback)(int type);

static GtkWidget*     g_window;
static GtkWidget*     g_vbox;
static GtkWidget*     g_main_window;
static WebKitWebView* g_web_view;
static GtkWidget*     g_toolbar;
static GtkStatusbar*  g_main_statusbar;

unsigned int g_framebuffer_width = 1280;
unsigned int g_framebuffer_height = 720;
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

#if 0
static void
update_title (GtkWindow* window)
{
    GString* string = g_string_new (main_title);
    g_string_append (string, " - WebKit Launcher");
    if (load_progress < 100)
        g_string_append_printf (string, " (%f%%)", load_progress);
    gchar* title = g_string_free (string, FALSE);
    gtk_window_set_title (window, title);
    g_free (title);
}
#endif

static void
link_hover_cb (WebKitWebView* page, const gchar* title, const gchar* link, gpointer data)
{
    /* underflow is allowed */
    gtk_statusbar_pop (g_main_statusbar, status_context_id);
    if (link)
        gtk_statusbar_push (g_main_statusbar, status_context_id, link);
}

#if 0
static void
notify_title_cb (WebKitWebView* web_view, GParamSpec* pspec, gpointer data)
{
    if (main_title)
        g_free (main_title);
    main_title = g_strdup (webkit_web_view_get_title(web_view));
    update_title (GTK_WINDOW (g_main_window));
}
#endif

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

#if 0
static void
notify_progress_cb (WebKitWebView* web_view, GParamSpec* pspec, gpointer data)
{
    load_progress = webkit_web_view_get_progress (web_view) * 100;
    update_title (GTK_WINDOW (g_main_window));
}
#endif

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

#if 0
    g_signal_connect (g_web_view, "notify::title", G_CALLBACK (notify_title_cb), g_web_view);
#endif
    g_signal_connect (g_web_view, "notify::load-status", G_CALLBACK (notify_load_status_cb), g_web_view);
#if 0
    g_signal_connect (g_web_view, "notify::progress", G_CALLBACK (notify_progress_cb), g_web_view);
#endif
    g_signal_connect (g_web_view, "hovering-over-link", G_CALLBACK (link_hover_cb), g_web_view);

    g_signal_connect (g_web_view, "focus-out-event", G_CALLBACK (focus_out_cb), g_web_view);

    g_signal_connect (g_web_view, "document-load-finished", G_CALLBACK (document_load_finished_cb), g_web_view);

    g_signal_connect (g_web_view, "window_object_cleared", G_CALLBACK (window_object_cleared_cb), g_web_view);

    return scrolled_window;
}

static GtkWidget*
create_statusbar ()
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
    
    g_main_window = create_window ();
    
    gtk_fixed_put(GTK_FIXED(fixed), g_vbox, 0, 0);
    gtk_widget_set_size_request(g_vbox, g_framebuffer_width, g_framebuffer_height);
    
    GtkWidget* statusLabel = gtk_label_new ("Status");
    gtk_fixed_put(GTK_FIXED(fixed), statusLabel, g_framebuffer_width - 200, 0);
    gtk_widget_set_size_request(statusLabel, 200, 100);
    
    gtk_container_add (GTK_CONTAINER (g_main_window), fixed);
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

    g_main_window = create_window ();

    gtk_fixed_put(GTK_FIXED(fixed), g_vbox, 0, 0);
    gtk_widget_set_size_request(g_vbox, g_framebuffer_width, g_framebuffer_height);

    GtkWidget* statusLabel = gtk_label_new ("Status");
    gtk_fixed_put(GTK_FIXED(fixed), statusLabel, g_framebuffer_width - 200, 0);
    gtk_widget_set_size_request(statusLabel, 200, 100);

    gtk_container_add (GTK_CONTAINER (g_main_window), fixed);

    webkit_web_view_load_uri (g_web_view, g_url);

    gtk_widget_grab_focus (GTK_WIDGET (g_web_view));
    gtk_widget_show_all (g_main_window);

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
    gtk_widget_hide_all (g_main_window);
}

void loadPage(char * url)
{
    printf("%s URL = %s\n", __func__, url);
    strncpy(g_url, url, 1024);
    //webkit_web_view_load_uri (g_web_view, (gchar*) url);
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



int
main (int argc, char* argv[])
{
#if 0
    printf("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
    unsigned char haveUrl = 0;
    int argCount = argc - 1;

    printf("%s:%s[%d] argCount=%d\n", __FILE__, __func__, __LINE__, argCount);

    for(int i = 0; i < argCount; i++)
    {
        if     (!strncmp("-s", argv[1 + i], 2))
        {
            printf("%s:%s[%d] parameter scale %s\n", __FILE__, __func__, __LINE__, argv[1 + i]);
            
            sscanf(argv[1 + i], "%dx%d", &g_framebuffer_width, &g_framebuffer_height);
            i++;
            printf("%s:%s[%d] fw=%d fh=%d\n", __FILE__, __func__, __LINE__, g_framebuffer_width, g_framebuffer_height);
        }
        else
            haveUrl = 1; 
    }

    argc = 0;
    argv= NULL;

    gtk_init (&argc, &argv);
    if (!g_thread_supported ())
        g_thread_init (NULL);


    GtkWidget* fixed = gtk_fixed_new();
    //screen_changed(fixed, NULL, NULL);
    g_vbox = gtk_vbox_new (FALSE, 0);
    gtk_box_pack_start (GTK_BOX (g_vbox), create_toolbar (), FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (g_vbox), GTK_WIDGET (create_browser ()), TRUE, TRUE, 0);
    gtk_box_pack_start (GTK_BOX (g_vbox), create_statusbar (), FALSE, FALSE, 0);

    g_main_window = create_window ();

    gtk_fixed_put(GTK_FIXED(fixed), g_vbox, 0, 0);
    gtk_widget_set_size_request(g_vbox, g_framebuffer_width, g_framebuffer_height);

    GtkWidget* statusLabel = gtk_label_new ("Status");
    gtk_fixed_put(GTK_FIXED(fixed), statusLabel, g_framebuffer_width - 200, 0);
    gtk_widget_set_size_request(statusLabel, 200, 100);

    gtk_container_add (GTK_CONTAINER (g_main_window), fixed);

    gchar* uri = (gchar*) (haveUrl ? argv[argCount] : "http://www.google.com/");
    webkit_web_view_load_uri (g_web_view, uri);

    gtk_widget_grab_focus (GTK_WIDGET (g_web_view));
    gtk_widget_show_all (g_main_window);

    toogleMode();

    g_default_scale = g_framebuffer_width / 1280.0f;
    handleZoomLock(0);

    gtk_main ();
#endif
    pthread_create(&g_GtkMain, NULL, GtkMain, NULL);
    while(1);

    return 0;
}

