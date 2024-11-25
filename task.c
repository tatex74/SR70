//
// Created by Bruno on 24/11/2024.
//

#include "task.h"


void taskManager() {
    Queue* productionQueue = createQueue();



    free(productionQueue);
}

Queue* createQueue() {
    Queue* q = (Queue*) malloc(sizeof(Queue));
    q->front = q->rear = NULL;
    return q;
}


void enqueue(Queue* q, Task task) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->task = task;
    newNode->next = NULL;

    if (q->rear == NULL) {
        q->front = q->rear = newNode;
        return;
    }

    q->rear->next = newNode;
    q->rear = newNode;
}


Task dequeue(Queue* q) {
    if (q->front == NULL) {
        printf("Empty task queue\n");
        Task errorTask = {0};
        return errorTask;
    }

    Node* temp = q->front;
    Task task = temp->task;

    q->front = q->front->next;

    if (q->front == NULL) {
        q->rear = NULL;
    }

    free(temp);
    return task;
}


void displayQueue(const Queue* q) {
    Node* current = q->front;

    if (current == NULL) {
        printf("Empty queue\n");
        return;
    }

    while (current != NULL) {
        printf("%s\n", current->task.name);
        current = current->next;
    }
    printf("\n");
}


void freeQueue(Queue* q) {
    Node* current = q->front;

    while (current != NULL) {
        Node* temp = current;
        current = current->next;
        free(temp);
    }

    free(q);
}