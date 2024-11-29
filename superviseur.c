#include <sys/signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include "structures.h"
#include "superviseur.h"

#define NB_TACHES 10
#define NB_ROBOTS_PAR_TYPE 1
#define NB_ROBOTS (NB_ROBOTS_PAR_TYPE * 3)

Robot robots[NB_ROBOTS]; // Tableau pour suivre les robots
volatile sig_atomic_t reponses[NB_ROBOTS]; // Tableau pour suivre les réponses des robots

void superviseur_init() {
    printf("Superviseur: Initialisation du superviseur.\n");
    init_signals();

    create_all_robots();

    sleep(3); // Donne le temps aux robots de s'initialiser

    while (1) {
        send_signal_to_check_alive();
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
    reponses[robot_id] = 0;
    pid_t pid = fork();
    if (pid == -1) {
        perror("Superviseur: Erreur lors de la création du processus fils");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        char type_str[10];
        sprintf(type_str, "%d", type);
        char id_str[10];
        sprintf(id_str, "%d", robot_id);
        execl("./robot", "./robot", id_str, type_str, NULL);
        perror("Superviseur: Erreur lors de l'exécution de execl");
        exit(EXIT_FAILURE);
    } else {
        robots[robot_id].pid = pid;
        robots[robot_id].is_alive = 1;
    }
}

void init_signals() {
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO; // Utilisation de siginfo_t pour les signaux
    sa.sa_sigaction = is_alive_received; // Gestionnaire de signal avec siginfo_t
    sigemptyset(&sa.sa_mask);

    // Associer SIGUSR1 au gestionnaire is_alive_received
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("Superviseur: Erreur lors de l'association de SIGUSR1");
        exit(EXIT_FAILURE);
    }
}

void is_alive_received(int signo, siginfo_t *info, void *context) {
    (void)signo;
    (void)context;

    pid_t sender_pid = info->si_pid;

    for (int i = 0; i < NB_ROBOTS; i++) {
        if (robots[i].pid == sender_pid) {
            reponses[i] = 1; // Le robot a répondu
            // printf("Superviseur: Signal reçu de Robot %d (PID: %d), marqué comme vivant.\n", i, sender_pid);
            return;
        }
    }
    // printf("Superviseur: Signal reçu d'un processus inconnu (PID: %d).\n", sender_pid);
}

void send_signal_to_check_alive() {
    for (int i = 0; i < NB_ROBOTS; i++) {
        if (reponses[i]) {
            // Le robot a répondu correctement
            printf("Superviseur: Robot %d (PID: %d) est vivant, tout va bien.\n", i, robots[i].pid);
            reponses[i] = 0; // Réinitialise le drapeau pour le prochain cycle
        } else if (robots[i].is_alive) {
            // Pas de réponse -> marquer comme "en attente de réponse"
            // printf("Superviseur: Envoi d'un signal au Robot %d (PID: %d) pour vérifier qu'il est en vie.\n", i, robots[i].pid);
            if (kill(robots[i].pid, SIGUSR1) == -1) {
                perror("Superviseur: Erreur lors de l'envoi du signal");
                robots[i].is_alive = 0; // Marque le robot comme mort si le signal échoue
            }
        } else {
            // Le robot est mort -> le recréer
            printf("Superviseur: Robot %d est mort, création d'un nouveau robot.\n", i);
            create_robot(i, robots[i].type_robot);
        }
    }
}

int main() {
    superviseur_init();
    return 0;
}
