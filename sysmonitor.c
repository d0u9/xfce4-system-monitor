#include <gtk/gtk.h>
#include <libxfce4panel/xfce-panel-plugin.h>
#include <string.h>

#include "settings.h"
#include "dialogs.h"
#include "sysmonitor.h"

static void system_monitor_construct(XfcePanelPlugin *plugin);
static void init_menu(XfcePanelPlugin *plugin);
static void create_layout(sys_monitor_t *base);
static void genmon_free(XfcePanelPlugin *plugin, sys_monitor_t *base);
static int set_font(GtkWidget *widget, const char *font_name);
static gboolean sample_size_changed(XfcePanelPlugin *plugin, gint size);
static sys_monitor_t *alloc_memory(void);
static sys_monitor_t *init_gui(XfcePanelPlugin *plugin);

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
        gtk_container_set_border_width(GTK_CONTAINER(hvbox), 3);
        base->hvbox = hvbox;


        create_layout(base);
        init_menu(plugin);

        return base;
}

static void create_layout(sys_monitor_t *base)
{
        GtkWidget       *layout_table;

        layout_table = gtk_table_new (3, 2, TRUE);
        gtk_widget_show (layout_table);
        gtk_table_set_col_spacing (GTK_TABLE(layout_table), 0, 6);
        gtk_container_add (GTK_CONTAINER(base->hvbox), layout_table);
        gtk_box_pack_start(GTK_BOX(base->hvbox), layout_table, FALSE, FALSE, 0);
        base->layout_table = layout_table;

        GtkWidget *uplink_speed_label = gtk_label_new("900 MB/s U");
        gtk_widget_show(uplink_speed_label);
        set_font(uplink_speed_label, DEFAULT_FONT);
        gtk_label_set_width_chars(GTK_LABEL(uplink_speed_label),
                                  MAX_UPLINK_SPEED_LABEL_WIDTH);
        gtk_table_attach_defaults(GTK_TABLE(layout_table),
                                  uplink_speed_label, 0, 2, 0, 1);
        base->uplink_speed_label = uplink_speed_label;

        GtkWidget *downlink_speed_label = gtk_label_new("812 MB/s D");
        gtk_widget_show(downlink_speed_label);
        set_font(downlink_speed_label, DEFAULT_FONT);
        gtk_label_set_width_chars(GTK_LABEL(downlink_speed_label),
                                  MAX_DOWNLINK_SPEED_LABEL_WIDTH);
        gtk_table_attach_defaults(GTK_TABLE(layout_table),
                                  downlink_speed_label, 0, 2, 1, 2);
        base->dowlink_speed_label = downlink_speed_label;

        GtkWidget *cpu_usage_label = gtk_label_new("60.6%");
        gtk_widget_show(cpu_usage_label);
        set_font(cpu_usage_label, DEFAULT_FONT);
        gtk_label_set_width_chars(GTK_LABEL(cpu_usage_label),
                                  MAX_CPU_USAGE_LABEL_WIDTH);
        gtk_table_attach_defaults(GTK_TABLE(layout_table),
                                  cpu_usage_label, 0, 1, 2, 3);
        base->cpu_usage_label = cpu_usage_label;

        GtkWidget *cpu_sensor_label = gtk_label_new("52 oC");
        gtk_widget_show(cpu_sensor_label);
        set_font(cpu_sensor_label, DEFAULT_FONT);
        gtk_label_set_width_chars(GTK_LABEL(cpu_sensor_label),
                                  MAX_CPU_SENSOR_LABEL_WIDTH);
        gtk_table_attach_defaults(GTK_TABLE(layout_table),
                                  cpu_sensor_label, 1, 2, 2, 3);
        base->cpu_sensor_label = cpu_sensor_label;
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


static int set_font(GtkWidget *widget, const char *font_name)
{
        PangoFontDescription *t_font = NULL;

        if (!strncmp(base->font, font_name, MAX_FONT_STR_LEN))
                return 1;

        t_font = pango_font_description_from_string(font_name);
        if (!t_font)
                return -1;

        gtk_widget_modify_font(GTK_WIDGET(widget), t_font);
        pango_font_description_free(t_font);

        return 0;
}

