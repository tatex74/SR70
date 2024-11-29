#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "robot.h"

// une variable myData pour stocker les données
struct MyData myData;

int main(int argc, char *argv[]) {
    myData.index = atoi(argv[1]);
    robot_init();
    while (1) {}
    return 0;
}

void is_alive(int signo, siginfo_t *info, void *context) {
    (void)signo;
    (void)context;

    // Affiche les informations du robot
    printf("Robot %d: Mon superviseur me demande si je suis en vie, je lui envoie un signal.\n", myData.index);

    // Envoie un signal au parent
    pid_t parent_pid = getppid();
    if (kill(parent_pid, SIGUSR1) == -1) {
        printf("Robot %d: Erreur lors de l'envoi du signal avec kill\n", myData.index);
        exit(EXIT_FAILURE);
    }
}

void robot_init() {
    printf("Robot %d: Initialisation du robot.\n", myData.index);

    // Configure le gestionnaire de signaux
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = is_alive;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("Robot: Erreur lors de l'association du gestionnaire de signal");
        exit(EXIT_FAILURE);
    }
}