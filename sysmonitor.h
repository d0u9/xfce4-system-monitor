#ifndef __SYSMONITOR_H
#define __SYSMONITOR_H


#include <libxfce4panel/xfce-panel-plugin.h>

typedef struct {
        XfcePanelPlugin *plugin;
        GtkWidget *ebox;
        GtkWidget *hvbox;
} sys_monitor_t;


#endif
