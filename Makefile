CC = gcc
M4 = m4

PROGRAM_NAME = sysmonitor
BIN = $(PROGRAM_NAME)
LIB = lib$(BIN).so
BUILD_DIR = build
M4_SCRIPT = configure.m4

ICON = xfce4-system-monitor-plugin.png

.SUFFIXES: .h.in .h .desktop.in .desktop

SRCS = $(wildcard *.c)
OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o)
DEPS = $(OBJS:%.o=%.d)
INS  = $(wildcard *.in)
OUTS = $(INS:%.in=$(BUILD_DIR)/%)

CFLAGS += -Wall -fPIC $(DEBUG_FLAGS) -std=c99 -I. -I$(BUILD_DIR)
EXTRA_CFLAGS = `pkg-config --cflags libxfce4panel-1.0` `pkg-config --cflags gtk+-3.0 pkg-config --cflags libxfce4ui-1`
EXTRA_LDFLAGS = `pkg-config --libs libxfce4panel-1.0` `pkg-config --libs gtk+-3.0 pkg-config --libs libxfce4ui-1`

release: DEBUG_FLAGS = -O2
release: build

build: _PRE $(OUTS) $(BIN)

$(BIN): $(OBJS)
	$(CC) $(DEBUG_FLAGS) $(CFLAGS) $(EXTRA_CFLAGS) $(EXTRA_LDFLAGS) -shared -o $(BUILD_DIR)/$(LIB) $^

-include $(DEPS)

$(BUILD_DIR)/%.o: %.c
	$(CC) $(DEBUG_FLAGS) $(CFLAGS) $(EXTRA_CFLAGS) $(EXTRA_LDFLAGS) -MMD -c -o $@ $<

$(BUILD_DIR)/%.h: %.h.in $(M4_SCRIPT)
	$(M4) $(M4_SCRIPT) $^ > $@

$(BUILD_DIR)/%.desktop: %.desktop.in
	$(M4) $(M4_SCRIPT) $^ > $@


.PHONY: debug all install uninstall clean _pre

debug: DEBUG_FLAGS = -O -g -DDEBUG
debug: build

all: release install

clean:
	rm -fr *.o *.d build

install:
	cp $(BUILD_DIR)/$(LIB) /usr/lib/xfce4/panel-plugins
	cp $(BUILD_DIR)/$(PROGRAM_NAME).desktop /usr/share/xfce4/panel-plugins
	cp ./icons/16x16/$(ICON) /usr/share/icons/hicolor/16x16/apps
	cp ./icons/22x22/$(ICON) /usr/share/icons/hicolor/22x22/apps
	cp ./icons/32x32/$(ICON) /usr/share/icons/hicolor/32x32/apps
	cp ./icons/48x48/$(ICON) /usr/share/icons/hicolor/48x48/apps
	cp ./icons/128x128/$(ICON) /usr/share/icons/hicolor/128x128/apps
	cp ./icons/256x256/$(ICON) /usr/share/icons/hicolor/256x256/apps
	gtk-update-icon-cache -f -t /usr/share/icons/hicolor

uninstall:
	rm -fr /usr/lib/xfce4/panel-plugins/$(LIB)
	rm -fr /usr/share/xfce4/panel-plugins/$(PROGRAM_NAME).desktop
	rm -fr /usr/share/icons/hicolor/16x16/apps/$(ICON)
	rm -fr /usr/share/icons/hicolor/22x22/apps/$(ICON)
	rm -fr /usr/share/icons/hicolor/32x32/apps/$(ICON)
	rm -fr /usr/share/icons/hicolor/48x48/apps/$(ICON)
	rm -fr /usr/share/icons/hicolor/128x128/apps/$(ICON)
	rm -fr /usr/share/icons/hicolor/256x256/apps/$(ICON)
	gtk-update-icon-cache -f -t /usr/share/icons/hicolor

_PRE:
	@test -d $(@D)/$(BUILD_DIR) || (mkdir -p $(@D)/$(BUILD_DIR) && echo "mkdir $(@D)/$(BUILD_DIR)";)

