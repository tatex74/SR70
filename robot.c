//
// Created by Bruno on 24/11/2024.
//

#include "robot.h"



void RobotsInit() {

    pid_t pid;


    for (int i = 0; i < NB_ASSEMBLY_ROBOT; i++) {
        pid = fork();

        if (pid < 0) {
            perror("fork error");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            Robot(assembling);
            exit(EXIT_SUCCESS);
        }
    }
    for (int i = 0; i < NB_PAINTING_ROBOT; i++) {
        pid = fork();

        if (pid < 0) {
            perror("fork error");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            Robot(painting);
            exit(EXIT_SUCCESS);
        }
    }
    for (int i = 0; i < NB_CARRYING_ROBOT; i++) {
        pid = fork();

        if (pid < 0) {
            perror("fork error");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            Robot(carrying);
            exit(EXIT_SUCCESS);
        }
    }


    waitRobots();

}


void Robot(Task task) {
    TaskState current_state;
    Task current_task = task;

    while ("la file de tache n'est pas vide");

}


void waitRobots() {
    for (int i = 0; i < NB_ASSEMBLY_ROBOT + NB_PAINTING_ROBOT + NB_CARRYING_ROBOT; i++) {
        wait(NULL);
    }
}


