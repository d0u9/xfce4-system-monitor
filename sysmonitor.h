#ifndef __SYSMONITOR_H
#define __SYSMONITOR_H


#include <libxfce4panel/xfce-panel-plugin.h>

typedef struct {
        GtkWidget       *uplink_speed_label;
        GtkWidget       *dowlink_speed_label;
        GtkWidget       *cpu_usage_label;
        GtkWidget       *cpu_sensor_label;
} gui_t;

typedef struct {
        XfcePanelPlugin *plugin;
        GtkWidget       *ebox;
        GtkWidget       *hvbox;
        GtkWidget       *layout_table;
        guint           timer_id;
        guint           update_interval;
        gui_t           gui;
        char    *font;
} sys_monitor_t;


#define MAX_FONT_STR_LEN        128

//#define DEFAULT_FONT            "(default)"
#define DEFAULT_FONT            "Sans 6"

#define MAX_UPLINK_SPEED_LABEL_WIDTH    12
#define MAX_DOWNLINK_SPEED_LABEL_WIDTH  12
#define MAX_CPU_USAGE_LABEL_WIDTH       6
#define MAX_CPU_SENSOR_LABEL_WIDTH      6

#endif
