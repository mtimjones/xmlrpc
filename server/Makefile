#
# Makefile for emhttp
#

CC = gcc

OBJS =	emhttp.o xmlparser.o init.o userfunc.o response.o

all:	emxmlp

emxmlp: $(OBJS)
	$(CC) -o emxmlp $(OBJS)

.c.o:
	$(CC) $(CFLAGS) -Wall -c $<

clean:
	rm -f emxmlp *.o
