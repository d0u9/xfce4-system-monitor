#ifndef __SYSMONITOR_H
#define __SYSMONITOR_H

#include <libxfce4panel/xfce-panel-plugin.h>

#include "cpu.h"
#include "network.h"
#include "sensor.h"

typedef struct {
        GtkWidget       *uplink_speed_label;
        GtkWidget       *dowlink_speed_label;
        GtkWidget       *cpu_usage_label;
        GtkWidget       *cpu_sensor_label;
} gui_t;

enum interval {
        level_1 = 250,
        level_2 = 500,
        level_3 = 750,
        level_4 = 1000
};

typedef struct {
        XfcePanelPlugin *plugin;
        GtkWidget       *ebox;
        GtkWidget       *hvbox;
        GtkWidget       *layout_table;
        enum interval   update_interval;
        guint           timer_id;
        gui_t           gui;
        cpu_t           cpu;
        net_t           net;
        sensor_t        sensor;
        char    *font;
} sys_monitor_t;


//#define DEFAULT_FONT            "(default)"
#define DEFAULT_FONT            "Sans 6"
#define DEFAULT_UPDATE_INTERVAL level_3

#define DEFAULT_UPLINK_DISPLAY          "N/A B/s \u25B3"
#define DEFAULT_DOWNLINK_DISPLAY        "N/A B/s \u25BD"
#define DEFAULT_CPU_LOAD_DISPLAY        "0.0 %"
#define DEFAULT_CORE_TEMP_DISPLAY       "N/A \u2103"


#define MAX_FONT_STR_LEN        128
#define MAX_FILE_LINE_LEN       256
#define MAX_PATH_STR_LEN        128

#define MAX_UPLINK_SPEED_LABEL_WIDTH    12
#define MAX_DOWNLINK_SPEED_LABEL_WIDTH  12
#define MAX_CPU_USAGE_LABEL_WIDTH       6
#define MAX_CPU_SENSOR_LABEL_WIDTH      6


#define CONST_Ki                (1 << 10)
#define CONST_Mi                (1 << 20)
#define CONST_Gi                (1 << 30)
#define CONST_EPSILON           (0.009)

#define STR_UPLINK_SIGN_IDLE    "\u25B3"
#define STR_DOWNLINK_SIGN_IDLE  "\u25BD"
#define STR_UPLINK_SIGN_BUSY    "\u25B2"
#define STR_DOWNLINK_SIGN_BUSY  "\u25BC"
#define STR_TEMPERATURE_SIGN    "\u2103"

#endif
