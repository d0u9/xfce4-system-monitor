#include <gtk/gtk.h>
#include <libxfce4panel/xfce-panel-plugin.h>
#include <string.h>

#include "settings.h"
#include "dialogs.h"
#include "event_handler.h"
#include "sysmonitor.h"

static void system_monitor_construct(XfcePanelPlugin *plugin);
static void init_menu(XfcePanelPlugin *plugin, sys_monitor_t *base);
static void create_layout(sys_monitor_t *base);
/*static void set_update_rate(sys_monitor_t *base, guint rate);*/
static void set_update_rate(sys_monitor_t *base, enum interval rate);
static int set_font(sys_monitor_t *sys_monitor, const char *font_name);
static sys_monitor_t *alloc_memory(void);
static sys_monitor_t *init_gui(XfcePanelPlugin *plugin);

/* register this plugin */
XFCE_PANEL_PLUGIN_REGISTER(system_monitor_construct);


static void system_monitor_construct(XfcePanelPlugin *plugin)
{
        sys_monitor_t *base = init_gui(plugin);

        g_signal_connect(G_OBJECT(plugin), "size-changed",
                         G_CALLBACK(system_monitor_size_changed), NULL);
        g_signal_connect(G_OBJECT(plugin), "free-data",
                         G_CALLBACK(system_monitor_free), base);
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
        set_font(base, DEFAULT_FONT);
        init_menu(plugin, base);

        update_net(&base->net);
        update_speed_str(&base->net, base->update_interval);
        update_cpu(&base->cpu);
        update_sensor(&base->sensor);

        update_sensor(&base->sensor);
        core_s_t *core = NULL;
        core = max_temp_core(&base->sensor.cpu_list, core);
        sleep(1);

        set_update_rate(base, DEFAULT_UPDATE_INTERVAL);

        return base;
}

static void create_layout(sys_monitor_t *base)
{
        GtkWidget       *layout_table;
        gui_t           *gui = &base->gui;

        layout_table = gtk_table_new (3, 2, TRUE);
        gtk_widget_show (layout_table);
        gtk_table_set_col_spacing (GTK_TABLE(layout_table), 0, 6);
        gtk_container_add (GTK_CONTAINER(base->hvbox), layout_table);
        gtk_box_pack_start(GTK_BOX(base->hvbox), layout_table, FALSE, FALSE, 0);
        base->layout_table = layout_table;

        GtkWidget *uplink_speed_label = gtk_label_new(DEFAULT_UPLINK_DISPLAY);
        gtk_widget_show(uplink_speed_label);
        gtk_label_set_width_chars(GTK_LABEL(uplink_speed_label),
                                  MAX_UPLINK_SPEED_LABEL_WIDTH);
        gtk_table_attach_defaults(GTK_TABLE(layout_table),
                                  uplink_speed_label, 0, 2, 0, 1);
        gui->uplink_speed_label = uplink_speed_label;

        GtkWidget *downlink_speed_label = gtk_label_new(DEFAULT_DOWNLINK_DISPLAY);
        gtk_widget_show(downlink_speed_label);
        gtk_label_set_width_chars(GTK_LABEL(downlink_speed_label),
                                  MAX_DOWNLINK_SPEED_LABEL_WIDTH);
        gtk_table_attach_defaults(GTK_TABLE(layout_table),
                                  downlink_speed_label, 0, 2, 1, 2);
        gui->dowlink_speed_label = downlink_speed_label;

        GtkWidget *cpu_usage_label = gtk_label_new(DEFAULT_CPU_LOAD_DISPLAY);
        gtk_widget_show(cpu_usage_label);
        gtk_label_set_width_chars(GTK_LABEL(cpu_usage_label),
                                  MAX_CPU_USAGE_LABEL_WIDTH);
        gtk_table_attach_defaults(GTK_TABLE(layout_table),
                                  cpu_usage_label, 0, 1, 2, 3);
        gui->cpu_usage_label = cpu_usage_label;

        GtkWidget *cpu_sensor_label = gtk_label_new(DEFAULT_CORE_TEMP_DISPLAY);
        gtk_widget_show(cpu_sensor_label);
        gtk_label_set_width_chars(GTK_LABEL(cpu_sensor_label),
                                  MAX_CPU_SENSOR_LABEL_WIDTH);
        gtk_table_attach_defaults(GTK_TABLE(layout_table),
                                  cpu_sensor_label, 1, 2, 2, 3);
        gui->cpu_sensor_label = cpu_sensor_label;
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

        if (init_cpu(&base->cpu) < 0)
                goto error;

        if (init_net(&base->net) < 0)
                goto error;

        if (init_sensor(&base->sensor) < 0)
                goto error;

        return base;

error:
        for (int i = 0; i < count - 1; ++i)
                free(p[i]);
        return NULL;
}


static void init_menu(XfcePanelPlugin *plugin, sys_monitor_t *base)
{
        xfce_panel_plugin_menu_show_about(plugin);
        g_signal_connect(G_OBJECT(plugin), "about",
                         G_CALLBACK(menu_about), NULL);

        xfce_panel_plugin_menu_show_configure(plugin);
        g_signal_connect(G_OBJECT(plugin), "configure-plugin",
                         G_CALLBACK(menu_properties), base);
}


static int set_font(sys_monitor_t *sys_monitor, const char *font_name)
{
        PangoFontDescription *t_font = NULL;
        gui_t *gui = &sys_monitor->gui;
        // Regard gui_t struct as an array of pointer of GtkWidget
        // For the convenience of iteration.
        GtkWidget **widget_array = (GtkWidget **)gui;

        if (!strncmp(sys_monitor->font, font_name, MAX_FONT_STR_LEN))
                return 1;

        t_font = pango_font_description_from_string(font_name);
        if (!t_font)
                return -1;

        int num = (int)(sizeof(gui_t) / sizeof(GtkWidget *));
        for (int i = 0; i < num; ++i) {
                gtk_widget_modify_font(GTK_WIDGET(*(widget_array + i)), t_font);
        }
        pango_font_description_free(t_font);

        return 0;
}


static void set_update_rate(sys_monitor_t *base, enum interval rate)
{
        guint update;

        base->update_interval = rate;

        if (base->timer_id)
                g_source_remove(base->timer_id);

        update = rate;

        base->timer_id = g_timeout_add(update, (GtkFunction)timeout, base);
}
