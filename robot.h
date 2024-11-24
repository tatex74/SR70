//
// Created by Bruno on 24/11/2024.
//

#ifndef ROBOT_H
#define ROBOT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "task.h"


#define NB_ASSEMBLY_ROBOT 5
#define NB_PAINTING_ROBOT 5
#define NB_CARRYING_ROBOT 5

void Robot(TaskType task);
void RobotsInit();
void waitRobots();


#endif //ROBOT_H
