#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "robot.h"
#include "structures.h"

// On store l'id du robot et son type
int robot_id;
TypeTache type_robot;

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Robot: Erreur, veuillez spécifier l'index du robot et son type.\n");
        exit(EXIT_FAILURE);
    }

    robot_id = atoi(argv[1]);
    type_robot = atoi(argv[2]);
    robot_init();
    while (1) {}
    return 0;
}

void is_alive(int signo, siginfo_t *info, void *context) {
    (void)signo;
    (void)context;

    // on récupère le type du robot
    // TypeTache type = type_robot;

    // Affiche les informations du robot
    // printf("Robot %s %d: Mon superviseur me demande si je suis en vie, je lui envoie un signal.\n", type_robot_to_string(type), robot_id);

    // Envoie un signal au parent
    pid_t parent_pid = getppid();
    if (kill(parent_pid, SIGUSR1) == -1) {
        printf("Robot %d: Erreur lors de l'envoi du signal avec kill\n", robot_id);
        exit(EXIT_FAILURE);
    }
}

// une fonction pour retourner le type du robot en string
char *type_robot_to_string(TypeTache type) {
    switch (type) {
        case ASSEMBLAGE:
            return "assemblage";
        case PEINTURE:
            return "peinture";
        case VERIFICATION:
            return "verification";
        default:
            return "inconnu";
    }
}

void robot_init() {
    printf("Robot %d: Initialisation du robot.\n", robot_id);

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