#ifndef __DIALOGS_H
#define __DIALOGS_H

#include <libxfce4panel/xfce-panel-plugin.h>
#include "sysmonitor.h"

extern void menu_about(XfcePanelPlugin *plugin);
extern void menu_properties(XfcePanelPlugin *plugin, sys_monitor_t *base);

#endif
