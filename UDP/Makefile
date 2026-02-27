CC := gcc
CFLAGS := -Wall -Wextra

all: sender.o rcvr.o node.o
	$(CC) $(CFLAGS) -o sender sender.o node.o -lws2_32
	$(CC) $(CFLAGS) -o rcvr rcvr.o node.o -lws2_32

udp: sender.o rcvr.o node.o
	$(CC) $(CFLAGS) -o sender sender.o node.o -lws2_32
	$(CC) $(CFLAGS) -o rcvr rcvr.o node.o -lws2_32

sender.o: node.o sender.c
	$(CC) $(CFLAGS) -c sender.c -o sender.o

rcvr.o: node.o rcvr.c
	$(CC) $(CFLAGS) -c rcvr.c -o rcvr.o

node.o: node_public.h node_defs.h node.c node_proto.h
	$(CC) $(CFLAGS) -c node.c -o node.o

clean:
	del *.o *.exe