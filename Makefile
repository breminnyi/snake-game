CC=gcc
CFLAGS=-Wall -Wextra
DEBUG_FLAGS = -ggdb -DDEBUG -O0
RELEASE_FLAGS = -O2
LIBS=-lncurses
SRC_DIR = src
SRC_FILES=$(wildcard $(SRC_DIR)/*.c)
BIN_DIR = bin
DEBUG_BIN=$(BIN_DIR)/debug/snake
RELEASE_BIN = $(BIN_DIR)/release/snake
WASM_BIN = $(BIN_DIR)/snake.wasm

.PHONY = all debug release clean wasm

all: debug release wasm

debug: CFLAGS += $(DEBUG_FLAGS)
debug: $(DEBUG_BIN)

release: CFLAGS += $(RELEASE_FLAGS)
release: $(RELEASE_BIN)

wasm: CFLAGS += -O0 -DDEBUG
wasm: $(WASM_BIN)

$(DEBUG_BIN): $(SRC_FILES)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $(SRC_FILES) $(LIBS)

$(RELEASE_BIN): $(SRC_FILES)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $(SRC_FILES) $(LIBS)

$(WASM_BIN): $(SRC_DIR)/game.c
	@mkdir -p $(@D)
	clang $(CFLAGS) --target=wasm32 --no-standard-libraries -Wl,--no-entry -Wl,--allow-undefined -Wl,--export=game_init -Wl,--export=game_handle_key -Wl,--export=game_update -o $@ $^

clean:
	rm -rf $(BIN_DIR)
