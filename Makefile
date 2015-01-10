CC = gcc
CFLAGS = -Wall -Wextra -g
CPPFLAGS = -I/usr/include/libxml2``
LDLIBS = -lxml2

.PHONY: all clean

all: xmlparser mpd_policy

xmlparser: xmlparser.o

xmlparser.o: xmlparser.c

mpd_policy: mpd_policy.o

mpd_policy.o: mpd_policy.c

clean:
	-rm -f *.o *~ xmlparser mpd_policy
