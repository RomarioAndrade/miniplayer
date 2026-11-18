CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g -Iinclude
BUILD = build
APP = $(BUILD)/miniplayer
.PHONY: all test clean
all: $(APP)
$(BUILD):
	mkdir -p $(BUILD)
$(APP): src/main.c src/miniplayer.c include/miniplayer.h | $(BUILD)
	$(CC) $(CFLAGS) src/main.c src/miniplayer.c -o $@
$(BUILD)/test_musica: tests/test_musica.c src/musica.c | $(BUILD)
	$(CC) $(CFLAGS) $^ -o $@
test: $(BUILD)/test_musica 
	@set -e; for t in $(BUILD)/test_musica; do echo "== $$t =="; ./$$t; done
$(BUILD)/test_m04_incremental: missoes/M04/test_m04.c src/lse.c include/lse.h
	@mkdir -p build
	$(M04_CC) $(M04_CFLAGS) missoes/M04/test_m04.c src/lse.c -o $@
test_m04: build/test_m04_incremental
	./build/test_m04_incremental
clean:
	rm -rf $(BUILD)
