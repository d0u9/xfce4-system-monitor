#include <gtk/gtk.h>
#include <libxfce4panel/xfce-panel-plugin.h>
#include <string.h>

#include "settings.h"
#include "dialogs.h"
#include "sysmonitor.h"

static void system_monitor_construct(XfcePanelPlugin *plugin);
static void init_menu(XfcePanelPlugin *plugin);
static void genmon_free(XfcePanelPlugin *plugin, sys_monitor_t *base);
static int set_font(sys_monitor_t *sys_monitor, const char *font_name);
static sys_monitor_t *alloc_memory(void);
static gboolean sample_size_changed(XfcePanelPlugin *plugin, gint size);
static sys_monitor_t * init_gui(XfcePanelPlugin *plugin);

/* register this plugin */
XFCE_PANEL_PLUGIN_REGISTER(system_monitor_construct);


static void system_monitor_construct(XfcePanelPlugin *plugin)
{
        sys_monitor_t *base = init_gui(plugin);

        g_signal_connect(G_OBJECT(plugin), "size-changed",
                         G_CALLBACK(sample_size_changed), NULL);
        g_signal_connect(G_OBJECT(plugin), "free-data",
                         G_CALLBACK(genmon_free), base);
}


static gboolean sample_size_changed(XfcePanelPlugin *plugin, gint size)
{
        GtkOrientation orientation = xfce_panel_plugin_get_orientation(plugin);

        if (orientation == GTK_ORIENTATION_HORIZONTAL)
                gtk_widget_set_size_request(GTK_WIDGET(plugin), -1, -1);
        else
                gtk_widget_set_size_request(GTK_WIDGET(plugin), size, -1);

        return TRUE;
}


static sys_monitor_t * init_gui(XfcePanelPlugin *plugin)
{
        GtkOrientation  orientation;
        GtkWidget       *ebox, *hvbox;
        sys_monitor_t   *base = NULL;

        base = alloc_memory();
        base->plugin = plugin;

        ebox = gtk_event_box_new();
        gtk_widget_show(ebox);
        gtk_container_add(GTK_CONTAINER(plugin), ebox);
        xfce_panel_plugin_add_action_widget(plugin, ebox);
        base->ebox = ebox;

        orientation = xfce_panel_plugin_get_orientation(plugin);
        hvbox = xfce_hvbox_new(orientation, FALSE, 0);
        gtk_widget_show(hvbox);
        gtk_container_add(GTK_CONTAINER(ebox), hvbox);
        gtk_container_set_border_width(GTK_CONTAINER(hvbox), 0);
        base->hvbox = hvbox;

        GtkWidget *show_label = gtk_label_new("Hello World!");
        gtk_widget_show(show_label);
        gtk_box_pack_start(GTK_BOX(hvbox), show_label, FALSE, FALSE, 0);
        base->show_label = show_label;

        init_menu(plugin);
        set_font(base, DEFAULT_FONT);

        return base;
}


static sys_monitor_t *alloc_memory(void)
{
        void *p[5] = {NULL};
        int count = 0;
        sys_monitor_t *base = NULL;

        base = p[count++] = calloc(1, sizeof(sys_monitor_t));
        if (!base) goto error;

        base->font = p[count++] = calloc(MAX_FONT_STR_LEN, 1);
        if (!base->font) goto error;

        return base;

error:
        for (int i = 0; i < count - 1; ++i)
                free(p[i]);
        return NULL;
}


static void init_menu(XfcePanelPlugin *plugin)
{
        xfce_panel_plugin_menu_show_about(plugin);

        g_signal_connect(G_OBJECT(plugin), "about",
                         G_CALLBACK(menu_about), NULL);
}


static void genmon_free(XfcePanelPlugin *plugin, sys_monitor_t *base)
{
        free(base->font);
        free(base);
}


static int set_font(sys_monitor_t *sys_monitor, const char *font_name)
{
        sys_monitor_t *base = sys_monitor;
        PangoFontDescription *t_font = NULL;

        if (!strncmp(base->font, font_name, MAX_FONT_STR_LEN))
                return 1;

        t_font = pango_font_description_from_string(font_name);
        if (!t_font)
                return -1;

        gtk_widget_modify_font(GTK_WIDGET(base->show_label), t_font);
        pango_font_description_free(t_font);

        return 0;
}

