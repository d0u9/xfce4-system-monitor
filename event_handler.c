#include <string.h>
#include <stdio.h>
#include "event_handler.h"
#include "cpu.h"
#include "network.h"
#include "trilib/log.h"

gboolean system_monitor_size_changed(XfcePanelPlugin *plugin, gint size);
gboolean timeout(struct sys_monitor *base);
void system_monitor_free(XfcePanelPlugin *plugin, struct sys_monitor *base);

gboolean system_monitor_size_changed(XfcePanelPlugin *plugin, gint size)
{
	GtkOrientation orientation = xfce_panel_plugin_get_orientation(plugin);

	if (orientation == GTK_ORIENTATION_HORIZONTAL)
		gtk_widget_set_size_request(GTK_WIDGET(plugin), -1, -1);
	else
		gtk_widget_set_size_request(GTK_WIDGET(plugin), size, -1);

	return TRUE;
}

gboolean timeout(struct sys_monitor *base)
{
	gchar str[32] = {0};

	/* update cpu usage */
	update_cpu(&base->cpu);
	snprintf(str, 31, "%.1f%%", base->cpu.total.load / 100.0);
	gtk_label_set_text(GTK_LABEL(base->gui.cpu_usage_label), str);

	/* update network */
	update_net(&base->net);
	update_speed_str(&base->net, get_mseconds_by_level(base->update_interval));
	printl_debug("send = %s, recv = %s\n",
		base->net.send_speed, base->net.recv_speed);
	gtk_label_set_text(GTK_LABEL(base->gui.dowlink_speed_label),
			   base->net.recv_speed);
	gtk_label_set_text(GTK_LABEL(base->gui.uplink_speed_label),
			   base->net.send_speed);

	/* update sensor date */
	update_sensor(&base->sensor);
	struct core_s *core = NULL;
	if ((core = max_temp_core(&base->sensor.cpu_list, core))) {
		sprintf(str, "%.1f "STR_TEMPERATURE_SIGN, core->input / 1000.0f);
		printl_debug("label = %s, %s\n", core->label, str);
		gtk_label_set_text(GTK_LABEL(base->gui.cpu_sensor_label), str);
	}

	printl_debug("Timeout !!!\n");
	printl_info("---------------------------\n");

	return TRUE;
}


void system_monitor_free(XfcePanelPlugin *plugin, struct sys_monitor *base)
{
	free_cpu(&base->cpu);
	free_net(&base->net);
	free(base->font);
	free(base);
}
