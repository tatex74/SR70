# Makefile

CC = gcc
CFLAGS = -Wall -pthread

all: superviseur robot

superviseur: superviseur.c queue.c
	$(CC) $(CFLAGS) -o superviseur superviseur.c queue.c

robot: robot.c queue.c
	$(CC) $(CFLAGS) -o robot robot.c queue.c

clean:
	rm -f superviseur robot
