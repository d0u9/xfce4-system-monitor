#include <stdio.h>
#include "event_handler.h"
#include "cpu.h"

gboolean system_monitor_size_changed(XfcePanelPlugin *plugin, gint size);
gboolean timeout(sys_monitor_t *base);
void system_monitor_free(XfcePanelPlugin *plugin, sys_monitor_t *base);


gboolean system_monitor_size_changed(XfcePanelPlugin *plugin, gint size)
{
        GtkOrientation orientation = xfce_panel_plugin_get_orientation(plugin);

        if (orientation == GTK_ORIENTATION_HORIZONTAL)
                gtk_widget_set_size_request(GTK_WIDGET(plugin), -1, -1);
        else
                gtk_widget_set_size_request(GTK_WIDGET(plugin), size, -1);

        return TRUE;
}


gboolean timeout(sys_monitor_t *base)
{
        gchar str[32] = {0};

        /* update cpu usage */
        update_cpu(&base->cpu);
        snprintf(str, 31, "%.1f%%", base->cpu.total.load / 100.0);
        gtk_label_set_text(GTK_LABEL(base->gui.cpu_usage_label), str);

        g_print("Timeout !!!\n");

        return TRUE;
}


void system_monitor_free(XfcePanelPlugin *plugin, sys_monitor_t *base)
{
        free_cpu(&base->cpu);
        free(base->font);
        free(base);
}
