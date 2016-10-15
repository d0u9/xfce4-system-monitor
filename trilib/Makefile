ifneq ($(BUILD_DIR),)
	BUILD_DIR = ../build
else
	BUILD_DIR = build
	CC = gcc
endif

CFLAGS += -Wall -fPIC -std=gnu99 -I. -I$(BUILD_DIR)

BIN = $(BUILD_DIR)/libtri.a

SRCS = $(wildcard *.c)
OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o)
DEPS = $(OBJS:%.o=%.d)

-include $(DEPS)

debug: DEBUG_FLAGS = -g -O -DDEBUG
debug: build

build: _PRE $(BIN)

$(BIN): $(OBJS)
	ar -cvq $(BIN) $^



$(BUILD_DIR)/%.o: %.c
	$(CC) $(DEBUG_FLAGS) $(CFLAGS) -MMD -c -o $@ $<

.PHONY: _PRE

_PRE:
	@test -d $(@D)/$(BUILD_DIR) || (mkdir -p $(@D)/$(BUILD_DIR) && echo "mkdir $(@D)/$(BUILD_DIR)";)

