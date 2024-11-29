//
// Created by Bruno on 24/11/2024.
//

#ifndef TASK_H
#define TASK_H

#include <stdio.h>
#include <stdlib.h>
#include "robot.h"


typedef struct Task {
    char* name;
    RobotType robotNeeded;
    int duration;
    int priority;
} Task;


typedef struct BigTask {
    Task* task;
} BigTask;


typedef enum taskState {
    running,
    paused,
    waiting_for_resources,
    completed
} TaskState;


typedef struct Node {
    Task task;
    struct Node* next;
} Node;


typedef struct Queue {
    Node* front;
    Node* rear;
} Queue;

void taskManager();
Queue* createQueue();
void enqueue(Queue* q, Task task);
Task dequeue(Queue* q);
void displayQueue(const Queue* q);
void freeQueue(Queue* q);


#endif //TASK_H
