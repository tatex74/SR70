#ifndef ROBOT_H
#define ROBOT_H

#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

void robot_init();
void is_alive(int signo, siginfo_t *info, void *context);

struct MyData {
    int index;
};

#endif // ROBOT_H
