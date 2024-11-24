//
// Created by Bruno on 24/11/2024.
//

#ifndef TASK_H
#define TASK_H

#include <stdio.h>
#include <stdlib.h>

typedef struct Task {
    char* name;
    enum task_type type;
    int priority;
} Task;


typedef enum taskType {
    assembling,
    painting,
    carrying
} TaskType;


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


#endif //TASK_H
