#include <gtk/gtk.h>
#include <libxfce4panel/xfce-panel-plugin.h>

static void system_monitor_construct(XfcePanelPlugin *plugin);
XFCE_PANEL_PLUGIN_REGISTER(system_monitor_construct);

static gboolean sample_size_changed (XfcePanelPlugin *plugin, gint size)
{
        GtkOrientation orientation;

        orientation = xfce_panel_plugin_get_orientation(plugin);
        if (orientation == GTK_ORIENTATION_HORIZONTAL)
                gtk_widget_set_size_request(GTK_WIDGET(plugin), -1, -1);
        else
                gtk_widget_set_size_request(GTK_WIDGET(plugin), size, -1);

        return TRUE;
}

static void system_monitor_construct(XfcePanelPlugin *plugin)
{
        GtkWidget *label;
        label = gtk_label_new("Hello World!");
        gtk_container_add(GTK_CONTAINER(plugin), GTK_WIDGET(label));
        gtk_widget_show(label);

        g_signal_connect (G_OBJECT(plugin), "size-changed",
                          G_CALLBACK(sample_size_changed), NULL);
}

