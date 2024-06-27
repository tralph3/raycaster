CC:=gcc
INCLUDE:=-I./include
CFLAGS:=-Wall -Wextra -pedantic -lm $(INCLUDE) $(shell pkg-config --cflags --libs raylib)
FILES:=$(wildcard src/*.c)
DUSE_WAYLAND_DISPLAY=TRUE

all: prepare main

prepare:
	mkdir -p build

%.o: %.c
	$(CC) $(CFLAGS) -c $^

main: $(FILES)
	$(CC) $(CFLAGS) -o build/$@ $^

debug: $(FILES)
	$(CC) $(CFLAGS) -ggdb -o build/$@ $^

release: $(FILES)
	$(CC) $(CFLAGS) -O2 -o build/$@ $^

profile: $(FILES)
	$(CC) $(CFLAGS) -pg -o build/$@ $^

clean:
	rm -rf ./build
