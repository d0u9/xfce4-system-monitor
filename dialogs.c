#include <gtk/gtk.h>
#include <libxfce4panel/xfce-panel-plugin.h>
#include <libxfce4ui/libxfce4ui.h>

#include "settings.h"
#include "dialogs.h"

void menu_about(XfcePanelPlugin *plugin);
void menu_properties(XfcePanelPlugin *plugin, sys_monitor_t *base);

static void cb_response(GtkWidget *dlg, gint response, sys_monitor_t *base);
static void cb_update_interval(GtkComboBox *combo, sys_monitor_t *base);

static void setup_update_interval_option(GtkBox *vbox, GtkSizeGroup *sg, sys_monitor_t *base);
static GtkBox *create_tab(void);
static GtkWidget *create_drop_down(GtkBox *tab, GtkSizeGroup *sg, const gchar *name,
                             const gchar **items, gsize num_items, guint init);
static GtkBox *create_option_line(GtkBox *tab, GtkSizeGroup *sg, const gchar *name);

/* This function is invoked when properties dialog close */
static void cb_response(GtkWidget *dlg, gint response, sys_monitor_t *base)
{
        gtk_widget_destroy(dlg);
        xfce_panel_plugin_unblock_menu(base->plugin);
	write_settings(base->plugin, base);
}


static void cb_update_interval(GtkComboBox *combo, sys_monitor_t *base)
{
        set_update_rate(base, gtk_combo_box_get_active(combo));
}



static GtkBox *create_tab(void)
{
        GtkBox *tab = NULL;
        tab = GTK_BOX(gtk_vbox_new(FALSE, G_CONFIG_BORDER));
        gtk_container_set_border_width(GTK_CONTAINER(tab), G_CONFIG_BORDER);
        gtk_widget_show(GTK_WIDGET(tab));
        return tab;
}


static GtkBox *create_option_line(GtkBox *tab, GtkSizeGroup *sg, const gchar *name)
{
        GtkBox    *line;
        GtkWidget *label;

        line = GTK_BOX(gtk_hbox_new(FALSE, G_CONFIG_BORDER));
        gtk_widget_show(GTK_WIDGET(line));
        gtk_box_pack_start(GTK_BOX(tab), GTK_WIDGET(line), FALSE, FALSE, 0);

        if (name)
        {
                label = gtk_label_new(name);
                gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
                gtk_size_group_add_widget(sg, label);
                gtk_widget_show(label);
                gtk_box_pack_start(GTK_BOX(line), GTK_WIDGET(label), FALSE, FALSE, 0);
        }

        return line;
}


static GtkWidget *create_drop_down(GtkBox *tab, GtkSizeGroup *sg, const gchar *name,
                             const gchar **items, gsize num_items, guint init)
{
        GtkBox    *option_line;
        GtkWidget *combo;

        option_line = create_option_line(tab, sg, name);

        combo = gtk_combo_box_new_text();
        for(gsize i = 0; i < num_items; i++)
        {
                gtk_combo_box_append_text(GTK_COMBO_BOX(combo), items[i]);
        }
        gtk_combo_box_set_active(GTK_COMBO_BOX(combo), init);
        gtk_box_pack_start(GTK_BOX(option_line), combo, FALSE, FALSE, 0);
        gtk_widget_show(combo);

        return combo;
}


static void setup_update_interval_option(GtkBox *vbox, GtkSizeGroup *sg, sys_monitor_t *base)
{
        GtkWidget   *combo;
        gsize       num_items;
        const gchar *items[] = {DEFAULT_STR_FASTEST,
                                DEFAULT_STR_FAST,
                                DEFAULT_STR_NORMAL,
                                DEFAULT_STR_SLOW
        };
        num_items = sizeof( items ) / sizeof( gchar* );

        combo = create_drop_down(vbox, sg, "Update Interval:", items, num_items,
                         base->update_interval);
        g_signal_connect(combo, "changed", G_CALLBACK(cb_update_interval), base);
}


void menu_properties(XfcePanelPlugin *plugin, sys_monitor_t *base)
{
        GtkWidget       *gw_configuration;
        GtkWidget       *gw_notebook;
        GtkWidget       *label;
        GtkBox          *gb_tab1, *gb_tab2;
        GtkSizeGroup    *gs_size_group;
        g_print("---------------- Properties menu is triggered --------------------\n");
        xfce_panel_plugin_block_menu(plugin);

        gw_configuration = xfce_titled_dialog_new_with_buttons("Configuration",
                        GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(plugin))),
                        GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_NO_SEPARATOR,
                        GTK_STOCK_CLOSE,
                        GTK_RESPONSE_OK,
                        NULL
                        );

        /* close */
        g_signal_connect(gw_configuration, "response", G_CALLBACK(cb_response), base);

        gtk_window_set_icon_name(GTK_WINDOW(gw_configuration), ICON);
        gs_size_group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);

        /* create tab 1 */
        gb_tab1 = create_tab();
        setup_update_interval_option(gb_tab1, gs_size_group, base);

        /* create tab 2 */
        /*gb_tab2 = create_tab();*/

        gw_notebook = gtk_notebook_new();
        gtk_widget_show(gw_notebook);

        label = gtk_label_new("Generic");
        gtk_notebook_append_page(GTK_NOTEBOOK(gw_notebook), GTK_WIDGET(gb_tab1), GTK_WIDGET(label));
        /*label = gtk_label_new("Others");*/
        /*gtk_notebook_append_page(GTK_NOTEBOOK(gw_notebook), GTK_WIDGET(gb_tab2), GTK_WIDGET(label));*/

        gtk_box_pack_start(GTK_BOX(GTK_DIALOG(gw_configuration)->vbox), GTK_WIDGET(gw_notebook), TRUE, TRUE, 0);

        gtk_widget_show(gw_configuration);
}


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
