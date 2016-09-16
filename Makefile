CC = gcc
M4 = m4

PROGRAM_NAME = sysmonitor
BIN = $(PROGRAM_NAME)
LIB = lib$(BIN).so
BUILD_DIR = build
M4_SCRIPT = configure.m4

.SUFFIXES: .h.in .h .desktop.in .desktop

SRCS = $(wildcard *.c)
OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o)
DEPS = $(OBJS:%.o=%.d)
INS  = $(wildcard *.in)
OUTS = $(INS:%.in=$(BUILD_DIR)/%)

CFLAGS += -Wall -fPIC $(DEBUG_FLAGS) -std=c99 -I. -I$(BUILD_DIR)
EXTRA_CFLAGS = `pkg-config --cflags libxfce4panel-1.0`
EXTRA_LDFLAGS = `pkg-config --libs libxfce4panel-1.0`

release: DEBUG_FLAGS = -O2
release: build

build: _PRE $(OUTS) $(BIN)

$(BIN): $(OBJS)
	$(CC) $(DEBUG_FLAGS) $(CFLAGS) $(EXTRA_CFLAGS) $(EXTRA_LDFLAGS) -shared -o $(BUILD_DIR)/$(LIB) $^

-include $(DEPS)

$(BUILD_DIR)/%.o: %.c
	$(CC) $(DEBUG_FLAGS) $(CFLAGS) $(EXTRA_CFLAGS) $(EXTRA_LDFLAGS) -MMD -c -o $@ $<

$(BUILD_DIR)/%.h: %.h.in
	$(M4) $(M4_SCRIPT) $^ > $@

$(BUILD_DIR)/%.desktop: %.desktop.in
	$(M4) $(M4_SCRIPT) $^ > $@


.PHONY: debug all install clean _pre

debug: DEBUG_FLAGS = -O -g -DDEBUG
debug: build

all: release install

clean:
	rm -fr *.o *.d build

install:
	cp $(BUILD_DIR)/$(LIB) /usr/lib/xfce4/panel-plugins
	cp $(BUILD_DIR)/$(PROGRAM_NAME).desktop /usr/share/xfce4/panel-plugins

_PRE:
	@test -d $(@D)/$(BUILD_DIR) || (mkdir -p $(@D)/$(BUILD_DIR) && echo "mkdir $(@D)/$(BUILD_DIR)";)

