CC = gcc
CFLAGS = -Wall -lm `sdl2-config --cflags --libs`
PREFIX = /usr/local

all: circi isog

circi: bin/circi
isog: bin/isog

bin/circi: bin
	${CC} -o bin/circi circi/circi.c ${CFLAGS}
bin/isog: bin
	${CC} -o bin/isog isog/isog.c ${CFLAGS}

bin:
	mkdir bin
clean:
	rm -f bin/*
install:
	cp bin/* ${PREFIX}/bin/

.phony: all clean install circi isog
