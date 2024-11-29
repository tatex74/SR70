#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include "structures.h"
#include "superviseur.h"

#define NB_TACHES 10
#define NB_ROBOTS_PAR_TYPE 1
#define NB_ROBOTS (NB_ROBOTS_PAR_TYPE * 3)

Robot robots[NB_ROBOTS]; // Tableau pour suivre les robots

void superviseur_init() {
    printf("Superviseur: Initialisation du superviseur.\n");
    init_signals(); // Initialisation des signaux pour gérer SIGCHLD

    create_all_robots();

    sleep(3); // Donne le temps aux robots de s'initialiser

    while (1) {
        check_robots_alive(); // Vérifie si les robots sont vivants
        sleep(5); // Intervalle entre les vérifications
    }
}

void create_all_robots() {
    int robot_id = 0;
    for (int type = 0; type < 3; type++) {
        for (int i = 0; i < NB_ROBOTS_PAR_TYPE; i++) {
            create_robot(robot_id, type);
            robot_id++;
        }
    }
}

void create_robot(int robot_id, int type) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("Superviseur: Erreur lors de la création du processus fils");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Processus fils (robot)
        char type_str[10];
        sprintf(type_str, "%d", type);
        char id_str[10];
        sprintf(id_str, "%d", robot_id);
        execl("./robot", "./robot", id_str, type_str, NULL);
        perror("Superviseur: Erreur lors de l'exécution de execl");
        exit(EXIT_FAILURE);
    } else {
        // Processus parent (superviseur)
        robots[robot_id].pid = pid;
        robots[robot_id].is_alive = 1;
        robots[robot_id].type_robot = type; // Initialiser le type du robot
        printf("Superviseur: Robot %d créé avec PID %d\n", robot_id, pid);
    }
}

void check_robots_alive() {
    for (int i = 0; i < NB_ROBOTS; i++) {
        if (robots[i].is_alive) {
            // Vérifie si le robot est vivant en utilisant kill(pid, 0)
            if (kill(robots[i].pid, 0) == -1) {
                if (errno == ESRCH) {
                    // Le processus n'existe plus
                    printf("Superviseur: Robot %d (PID: %d) est mort, création d'un nouveau robot.\n", i, robots[i].pid);
                    robots[i].is_alive = 0;
                    create_robot(i, robots[i].type_robot);
                } else {
                    // Une autre erreur est survenue
                    perror("Superviseur: Erreur lors de la vérification du robot");
                }
            } else {
                // Le processus est vivant
                printf("Superviseur: Robot %d (PID: %d) est vivant, tout va bien.\n", i, robots[i].pid);
            }
        } else {
            // Le robot est marqué comme mort, le recréer
            printf("Superviseur: Robot %d est mort, création d'un nouveau robot.\n", i);
            create_robot(i, robots[i].type_robot);
        }
    }
}

void sigchld_handler(int signo) {
    (void)signo;
    pid_t pid;
    int status;

    // Récupère tous les processus enfants terminés
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        // Trouver le robot correspondant
        for (int i = 0; i < NB_ROBOTS; i++) {
            if (robots[i].pid == pid) {
                robots[i].is_alive = 0;
                printf("Superviseur: Robot %d (PID: %d) s'est terminé.\n", i, pid);
                break;
            }
        }
    }
}

void init_signals() {
    struct sigaction sa;
    sa.sa_handler = sigchld_handler; // Gestionnaire pour SIGCHLD
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;

    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("Superviseur: Erreur lors de l'association de SIGCHLD");
        exit(EXIT_FAILURE);
    }
}

int main() {
    superviseur_init();
    return 0;
}
