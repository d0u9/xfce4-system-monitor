#!/bin/bash

name=sysmonitor
source=(sysmonitor.c)

echo "Start compiling..."
gcc -Wall -shared -o lib${name}.so -fPIC ${source[*]} $(pkg-config --cflags --libs libxfce4panel-1.0) $(pkg-config --cflags --libs gtk+-2.0) || \
	{ echo "Compiling failed!"; exit 10; }

echo "Start installing.."
cp lib${name}.so /usr/lib/xfce4/panel-plugins
cp ${name}.desktop /usr/share/xfce4/panel-plugins

echo "Finished successfully!"
