CC:=gcc
INCLUDE:=-I./include -I./raylib/src/ -L./raylib/src
CFLAGS:=-ffast-math -Wall -Wextra -pedantic -lraylib -lm $(INCLUDE)
FILES:=$(wildcard src/*.c)
DUSE_WAYLAND_DISPLAY=TRUE

.PHONY: prepare raylib

all: prepare main

prepare: raylib
	mkdir -p build

raylib:
	make -C ./raylib/src PLATFORM=PLATFORM_DESKTOP

%.o: %.c
	$(CC) $(CFLAGS) -c $^

main: $(FILES)
	$(CC) -o build/$@ $^ $(CFLAGS)

debug: $(FILES)
	$(CC) -ggdb -o build/$@ $^ $(CFLAGS)

release: $(FILES)
	$(CC) -O3 -o build/$@ $^ $(CFLAGS)

profile: $(FILES)
	$(CC) -pg -o build/$@ $^ $(CFLAGS)

clean:
	make -C ./raylib/src clean
	rm -rf ./build
