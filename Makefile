# Makefile

CC = gcc
CFLAGS = -Wall -pthread

all: superviseur robot

superviseur: superviseur.c
	$(CC) $(CFLAGS) -o superviseur superviseur.c

robot: robot.c
	$(CC) $(CFLAGS) -o robot robot.c

clean:
	rm -f superviseur robot
