# Makefile

CC = gcc
CFLAGS = -Wall -pthread

all: clean superviseur robot

superviseur: superviseur.c queue.c shared_memory.c
	$(CC) $(CFLAGS) -o superviseur superviseur.c queue.c shared_memory.c

robot: robot.c queue.c shared_memory.c
	$(CC) $(CFLAGS) -o robot robot.c queue.c shared_memory.c

clean:
	rm -f superviseur robot
