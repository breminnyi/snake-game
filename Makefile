CC=gcc
CFLAGS=-Wall -Wextra -pedantic
DEBUG_FLAGS = -ggdb -DDEBUG -O0
RELEASE_FLAGS = -O2
LIBS=-lncurses
SRC_FILES=$(wildcard src/*.c)
BIN_DIR = bin
DEBUG_BIN=$(BIN_DIR)/debug/snake
RELEASE_BIN = $(BIN_DIR)/release/snake

.PHONY = all debug release clean

all: debug release

debug: CFLAGS += $(DEBUG_FLAGS)
debug: $(DEBUG_BIN)

release: CFLAGS += $(RELEASE_FLAGS)
release: $(RELEASE_BIN)

$(DEBUG_BIN): $(SRC_FILES)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $(SRC_FILES) $(LIBS)

$(RELEASE_BIN): $(SRC_FILES)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $(SRC_FILES) $(LIBS)

clean:
	rm -rf $(BIN_DIR)
