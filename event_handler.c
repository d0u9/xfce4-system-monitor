#include "event_handler.h"

gboolean system_monitor_size_changed(XfcePanelPlugin *plugin, gint size);
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


void system_monitor_free(XfcePanelPlugin *plugin, sys_monitor_t *base)
{
        free(base->font);
        free(base);
}


