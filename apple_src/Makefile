CC = gcc
CXX = g++
CFLAGS = -Os -s -static -Wall -Wextra -Werror -pedantic

.PHONY: all clean
all: init apple
clean:
	rm -f init apple

init: init.c
	$(CC) $(CFLAGS) $< -o $@
	strip --strip-all $@

apple: apple.cpp
	$(CXX) $(CFLAGS) $< -o $@
	strip --strip-all $@
