#ifndef __EVENT_HANDLER_H
#define __EVENT_HANDLER_H

#include <gtk/gtk.h>
#include <libxfce4panel/xfce-panel-plugin.h>

#include "sysmonitor.h"

extern gboolean system_monitor_size_changed(XfcePanelPlugin *plugin, gint size);
extern gboolean timeout(sys_monitor_t *base);
extern void system_monitor_free(XfcePanelPlugin *plugin, sys_monitor_t *base);

#endif

