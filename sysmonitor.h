#ifndef __SYSMONITOR_H
#define __SYSMONITOR_H


#include <libxfce4panel/xfce-panel-plugin.h>

typedef struct {
        XfcePanelPlugin *plugin;
        GtkWidget       *ebox;
        GtkWidget       *hvbox;
        GtkWidget       *show_label;
        char    *font;
} sys_monitor_t;


#define MAX_FONT_STR_LEN        128

//#define DEFAULT_FONT            "(default)"
#define DEFAULT_FONT            "Sans 6"


#endif
