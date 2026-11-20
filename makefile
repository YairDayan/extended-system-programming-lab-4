CC = gcc
CFLAGS = -Wall -g -m32

all: hexeditplus digits

hexeditplus: task1.c
	$(CC) $(CFLAGS) task1.c -o hexeditplus

digits: task4.c
	$(CC) $(CFLAGS) -fno-pie -fno-stack-protector -O0 task4.c -o digits

clean:
	rm -f hexeditplus digits

.PHONY: all clean
