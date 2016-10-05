#include <gtk/gtk.h>
#include <libxfce4ui/libxfce4ui.h>
#include <libxfce4panel/xfce-panel-plugin.h>
#include <string.h>

#include "settings.h"
#include "dialogs.h"
#include "event_handler.h"
#include "sysmonitor.h"

guint get_mseconds_by_level(enum interval level);
void set_update_rate(struct sys_monitor *base, enum interval rate);
void write_settings(XfcePanelPlugin *plugin, struct sys_monitor *base);
int set_font(struct sys_monitor *sys_monitor, const char *font_name);

static void system_monitor_construct(XfcePanelPlugin *plugin);
static void init_menu(XfcePanelPlugin *plugin, struct sys_monitor *base);
static void create_layout(struct sys_monitor *base);
static struct sys_monitor *alloc_memory(void);
static struct sys_monitor *init_gui(XfcePanelPlugin *plugin);
static void read_settings(XfcePanelPlugin *plugin, struct sys_monitor *base);
void write_settings(XfcePanelPlugin *plugin, struct sys_monitor *base);

/* register this plugin */
XFCE_PANEL_PLUGIN_REGISTER(system_monitor_construct);

static void system_monitor_construct(XfcePanelPlugin *plugin)
{
	struct sys_monitor *base = init_gui(plugin);

	g_signal_connect(G_OBJECT(plugin), "size-changed",
			 G_CALLBACK(system_monitor_size_changed), NULL);
	g_signal_connect(G_OBJECT(plugin), "free-data",
			 G_CALLBACK(system_monitor_free), base);
}

guint get_mseconds_by_level(enum interval level)
{
	guint update;
	switch(level) {
	case level_1: update = 1 * DEFAULT_INTERVAL_FACTOR; break;
	case level_2: update = 2 * DEFAULT_INTERVAL_FACTOR; break;
	case level_3: update = 3 * DEFAULT_INTERVAL_FACTOR; break;
	case level_4: update = 4 * DEFAULT_INTERVAL_FACTOR; break;
	default: update = DEFAULT_UPDATE_INTERVAL;
	}
	return update;
}

static struct sys_monitor * init_gui(XfcePanelPlugin *plugin)
{
	GtkOrientation orientation;
	GtkWidget      *ebox, *hvbox;
	struct sys_monitor  *base = NULL;

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
	struct core_s *core = NULL;
	core = max_temp_core(&base->sensor.cpu_list, core);

	read_settings(plugin, base);

	return base;
}

static void create_layout(struct sys_monitor *base)
{
	GtkWidget *layout_table;
	struct gui     *gui = &base->gui;

	layout_table = gtk_table_new (3, 2, TRUE);
	gtk_widget_show (layout_table);
	gtk_table_set_col_spacing (GTK_TABLE(layout_table), 0, 6);
	gtk_container_add (GTK_CONTAINER(base->hvbox), layout_table);
	gtk_box_pack_start(GTK_BOX(base->hvbox), layout_table, FALSE, FALSE, 0);
	base->layout_table = layout_table;

	GtkWidget *uplink_speed_label = gtk_label_new(DEFAULT_UPLINK_DISPLAY);
	gtk_misc_set_alignment(GTK_MISC(uplink_speed_label), 1, 1);
	gtk_label_set_width_chars(GTK_LABEL(uplink_speed_label),
				  MAX_UPLINK_SPEED_LABEL_WIDTH);
	gtk_widget_show(uplink_speed_label);
	gtk_label_set_width_chars(GTK_LABEL(uplink_speed_label),
				  MAX_UPLINK_SPEED_LABEL_WIDTH);
	gtk_table_attach_defaults(GTK_TABLE(layout_table),
				  uplink_speed_label, 0, 2, 0, 1);
	gui->uplink_speed_label = uplink_speed_label;

	GtkWidget *downlink_speed_label = gtk_label_new(DEFAULT_DOWNLINK_DISPLAY);
	gtk_misc_set_alignment(GTK_MISC(downlink_speed_label), 1, 1);
	gtk_label_set_width_chars(GTK_LABEL(downlink_speed_label),
				  MAX_DOWNLINK_SPEED_LABEL_WIDTH);
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
	gtk_misc_set_alignment(GTK_MISC(cpu_usage_label), 1, 1);
	gtk_table_attach_defaults(GTK_TABLE(layout_table),
				  cpu_usage_label, 0, 1, 2, 3);
	gui->cpu_usage_label = cpu_usage_label;

	GtkWidget *cpu_sensor_label = gtk_label_new(DEFAULT_CORE_TEMP_DISPLAY);
	gtk_widget_show(cpu_sensor_label);
	gtk_label_set_width_chars(GTK_LABEL(cpu_sensor_label),
				  MAX_CPU_SENSOR_LABEL_WIDTH);
	gtk_misc_set_alignment(GTK_MISC(cpu_sensor_label), 1, 1);
	gtk_table_attach_defaults(GTK_TABLE(layout_table),
				  cpu_sensor_label, 1, 2, 2, 3);
	gui->cpu_sensor_label = cpu_sensor_label;
}

static struct sys_monitor *alloc_memory(void)
{
	void *p[5] = {NULL};
	int count = 0;
	struct sys_monitor *base = NULL;

	base = p[count++] = calloc(1, sizeof(struct sys_monitor));
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

static void init_menu(XfcePanelPlugin *plugin, struct sys_monitor *base)
{
	xfce_panel_plugin_menu_show_about(plugin);
	g_signal_connect(G_OBJECT(plugin), "about",
			 G_CALLBACK(menu_about), NULL);

	xfce_panel_plugin_menu_show_configure(plugin);
	g_signal_connect(G_OBJECT(plugin), "configure-plugin",
			 G_CALLBACK(menu_properties), base);
}

int set_font(struct sys_monitor *sys_monitor, const char *font_name)
{
	PangoFontDescription *t_font = NULL;
	struct gui *gui = &sys_monitor->gui;
	// Regard struct gui struct as an array of pointer of GtkWidget
	// For the convenience of iteration.
	GtkWidget **widget_array = (GtkWidget **)gui;

	if (!strncmp(sys_monitor->font, font_name, MAX_FONT_STR_LEN))
		return 1;

	strncpy(sys_monitor->font, font_name, MAX_FONT_STR_LEN);
	t_font = pango_font_description_from_string(font_name);
	if (!t_font)
		return -1;

	int num = (int)(sizeof(struct gui) / sizeof(GtkWidget *));
	for (int i = 0; i < num; ++i) {
		gtk_widget_modify_font(GTK_WIDGET(*(widget_array + i)), t_font);
	}
	pango_font_description_free(t_font);

	return 0;
}

void set_update_rate(struct sys_monitor *base, enum interval level)
{
	guint update;

	base->update_interval = level;

	if (base->timer_id)
		g_source_remove(base->timer_id);

	update = get_mseconds_by_level(level);

	base->timer_id = g_timeout_add(update, (GtkFunction)timeout, base);
}

static void read_settings(XfcePanelPlugin *plugin, struct sys_monitor *base)
{
	char *file = NULL;
	const char *font = NULL;
	XfceRc *rc;
	enum interval interval = DEFAULT_UPDATE_INTERVAL;

	if ((file = xfce_panel_plugin_lookup_rc_file(plugin)) == NULL)
		goto setup;

	rc = xfce_rc_simple_open(file, TRUE);
	g_free(file);

	if (!rc)
		goto setup;

	interval = xfce_rc_read_int_entry(rc, "update_interval", interval);
	font = xfce_rc_read_entry(rc, "font", NULL);


	xfce_rc_close(rc);

setup:
	set_update_rate(base, interval);
	if (font)
		set_font(base, font);

}

void write_settings(XfcePanelPlugin *plugin, struct sys_monitor *base)
{
	char	*file = NULL;
	XfceRc	*rc;

	if ((file = xfce_panel_plugin_save_location(plugin, TRUE)) == NULL)
		return;

	rc = xfce_rc_simple_open(file, FALSE);
	g_free(file);

	if (!rc)
		return ;

	xfce_rc_write_int_entry(rc, "update_interval", base->update_interval);
	xfce_rc_write_entry(rc, "font", base->font);

	xfce_rc_close(rc);
}
