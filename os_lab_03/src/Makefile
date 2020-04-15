CC = gcc
LD = gcc

CCFLAGS = -g
LDFLAGS = -g

l3: main.o queue.o
	$(LD) $(LDFLAGS) -pthread -o  l3 main.o queue.o

maim.o: main.c queue.h
	$(CC) $(CCFLAGS) -c main.c -o main.o

queue.o: queue.c queue.h
	$(CC) $(CCFLAGS) -c queue.c -o queue.o

clear:
	rm -f *.o l3
