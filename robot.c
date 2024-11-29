// robot.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h> // Pour O_* constants
#include <sys/mman.h>
#include <semaphore.h>
#include <string.h>
#include "structures.h"
#include "shared_memory.h"
#include "queue.h"

int robot_id;
TypeTache robot_type;
FileTaches *files_taches;

// Gestionnaire pour le signal SIGTERM
void handle_sigterm(int signo) {
    printf("Robot %d de type %d reçoit SIGTERM, s'arrête.\n", robot_id, robot_type);
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s robot_id robot_type\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    robot_id = atoi(argv[1]);
    robot_type = atoi(argv[2]);

    printf("Robot %d de type %d démarré.\n", robot_id, robot_type);

    // Installer le gestionnaire de signal pour SIGTERM
    struct sigaction sa_term;
    sa_term.sa_handler = handle_sigterm;
    sigemptyset(&sa_term.sa_mask);
    sa_term.sa_flags = 0;
    if (sigaction(SIGTERM, &sa_term, NULL) == -1) {
        perror("Robot: Erreur lors de l'association de SIGTERM");
        exit(EXIT_FAILURE);
    }

    // Ouvrir la mémoire partagée pour les files de tâches
    int shm_fd_files_taches = shm_open(SHM_FILES_TACHES, O_RDWR, 0666);
    if (shm_fd_files_taches == -1) {
        perror("Robot: Erreur lors de l'ouverture de la mémoire partagée");
        exit(EXIT_FAILURE);
    }
    files_taches = mmap(NULL, sizeof(FileTaches) * 3, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd_files_taches, 0);
    if (files_taches == MAP_FAILED) {
        perror("Robot: Erreur lors du mapping de la mémoire partagée");
        exit(EXIT_FAILURE);
    }

    // Boucle principale
    while (1) {
        Tache tache;
        // Retirer une tâche de la file correspondante
        if (retirer_tache(&files_taches[robot_type], &tache)) {
            // Simuler une panne aléatoire
            if (rand() % 10 == 0) {
                printf("Robot %d de type %d est tombé en panne en traitant la tâche %d.\n", robot_id, robot_type, tache.id);
                // Le robot termine
                exit(EXIT_FAILURE);
            }

            // Exécuter la tâche
            printf("Robot %d de type %d exécute la tâche %d.\n", robot_id, robot_type, tache.id);
            sleep(2); // Simuler le temps de traitement

            // Mettre à jour le type de la tâche pour l'étape suivante
            if (robot_type == ASSEMBLAGE) {
                tache.type_actuel = PEINTURE;
                // Ajouter la tâche à la file de peinture
                ajouter_tache(&files_taches[PEINTURE], tache);
            } else if (robot_type == PEINTURE) {
                tache.type_actuel = VERIFICATION;
                // Ajouter la tâche à la file de vérification
                ajouter_tache(&files_taches[VERIFICATION], tache);
            } else {
                // Tâche terminée
                printf("Robot %d de type %d a terminé la tâche %d.\n", robot_id, robot_type, tache.id);
            }
        }
    }

    return 0;
}
