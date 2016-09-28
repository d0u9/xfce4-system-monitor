#include <gtk/gtk.h>
#include <libxfce4panel/xfce-panel-plugin.h>

#include "settings.h"
#include "dialogs.h"

void menu_about(XfcePanelPlugin *plugin);
void menu_properties(XfcePanelPlugin *plugin, sys_monitor_t *base);

void menu_about(XfcePanelPlugin *plugin)
{
        GdkPixbuf *icon;
        const gchar *auth[] = {AUTHOR1, NULL};

        icon = xfce_panel_pixbuf_from_source(ICON, NULL, 48);
        gtk_show_about_dialog(NULL,
                        "logo", icon,
                        "wrap_license", TRUE,
                        "license", TEXT_LICENSE,
                        "version", TEXT_VERSION,
                        "program-name", TEXT_PROGRAM_NAME,
                        "comments", TEXT_COMMENT,
                        "website", TEXT_WEBSITE,
                        "copyright", TEXT_COPYRIGHT,
                        "authors", auth,
                        NULL);

        if(icon)
                g_object_unref(G_OBJECT(icon));
}


void menu_properties(XfcePanelPlugin *plugin, sys_monitor_t *base)
{
        g_print("---------------- Properties menu is triggered --------------------");
}
