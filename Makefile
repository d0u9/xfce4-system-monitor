CC = gcc
PROGRAM_NAME = sysmonitor
BIN = $(PROGRAM_NAME)
LIB = lib$(BIN).so
BUILD_DIR = build

SRCS = $(wildcard *.c)
OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o)
DEPS = $(OBJS:%.o=%.d)

CFLAGS += -Wall -fPIC $(DEBUG_FLAGS) -I.
EXTRA_CFLAGS = `pkg-config --cflags libxfce4panel-1.0`
EXTRA_LDFLAGS = `pkg-config --libs libxfce4panel-1.0`

release: DEBUG_FLAGS = -O2
release: build

build: _pre $(BIN)

$(BIN): $(OBJS)
	$(CC) $(DEBUG_FLAGS) $(CFLAGS) $(EXTRA_CFLAGS) $(EXTRA_LDFLAGS) -shared -o $(BUILD_DIR)/$(LIB) $^

-include $(DEPS)

$(BUILD_DIR)/%.o: %.c
	$(CC) $(DEBUG_FLAGS) $(CFLAGS) $(EXTRA_CFLAGS) $(EXTRA_LDFLAGS) -MMD -c -o $@ $<

.PHONY: debug all install clean _pre

debug: DEBUG_FLAGS = -O -g -DDEBUG
debug: build

all: release install

clean:
	rm -fr *.o *.d build

install:
	cp $(BUILD_DIR)/$(LIB) /usr/lib/xfce4/panel-plugins
	cp $(PROGRAM_NAME).desktop /usr/share/xfce4/panel-plugins

_pre:
	@mkdir -p $(BUILD_DIR)

