#include <stdio.h>

#include "robot.h"

int main(void)
{
    RobotsInit();
    taskManager();
    shutdown();
    return 0;
}
