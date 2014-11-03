CC = gcc
CFLAGS = -Wall -Wextra -g
CPPFLAGS = -I/usr/include/libxml2``
LDLIBS = -lxml2

.PHONY: all clean

all: xmlparser

xmlparser: xmlparser.o

xmlparser.o: xmlparser.c

clean:
	-rm -f *.o *~ xmlparser
