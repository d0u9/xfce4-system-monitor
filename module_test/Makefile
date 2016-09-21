CC = gcc

CFLAGS += -Wall -fPIC -ggdb3 -std=c99 -I. -I.. -O0
EXTRA_CFLAGS = `pkg-config --cflags libxfce4panel-1.0` `pkg-config --cflags gtk+-3.0`
EXTRA_LDFLAGS = `pkg-config --libs libxfce4panel-1.0` `pkg-config --libs gtk+-3.0`

all:
	@echo "Please specific one target"

list: list_test.c ../list.c ../list.h
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(EXTRA_LDFLAGS) list_test.c ../list.c -o list.out

sensor: sensor_test.c ../sensor.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(EXTRA_LDFLAGS) sensor_test.c ../sensor.c ../list.c -o sensor.out
