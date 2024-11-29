// superviseur.c

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>       // Pour O_* constants
#include <sys/mman.h>
#include <semaphore.h>
#include <string.h>
#include "structures.h"
#include "shared_memory.h"
#include "queue.h"

#define NB_TACHES 10
#define NB_ROBOTS_PAR_TYPE 1
#define NB_ROBOTS (NB_ROBOTS_PAR_TYPE * 3)

Robot robots[NB_ROBOTS]; // Tableau pour suivre les robots

FileTaches *files_taches; // Mémoire partagée pour les files de tâches

volatile sig_atomic_t tasks_done = 0;

// Déclarations des fonctions
void superviseur_init(int argc, char *argv[]);
void create_all_robots();
void create_robot(int robot_id, int type);
void check_robots_alive();
void sigchld_handler(int signo);
void init_signals();
void init_taches(int nombre_de_taches);
void superviseur_loop();

int main(int argc, char *argv[]) {
    superviseur_init(argc, argv);
    return 0;
}

void superviseur_init(int argc, char *argv[]) {
    int nombre_de_taches = NB_TACHES;

    if (argc >= 2) {
        nombre_de_taches = atoi(argv[1]);
        if (nombre_de_taches <= 0) {
            fprintf(stderr, "Erreur: Le nombre de tâches doit être un entier positif.\n");
            exit(EXIT_FAILURE);
        }
    }

    printf("Superviseur: Initialisation du superviseur avec %d tâches.\n", nombre_de_taches);
    init_signals(); // Initialisation des signaux pour gérer SIGCHLD

    // Initialiser la mémoire partagée pour les files de tâches
    int shm_fd_files_taches = shm_open(SHM_FILES_TACHES, O_CREAT | O_RDWR, 0666);
    if (shm_fd_files_taches == -1) {
        perror("Superviseur: Erreur lors de la création de la mémoire partagée");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(shm_fd_files_taches, sizeof(FileTaches) * 3) == -1) {
        perror("Superviseur: Erreur lors du redimensionnement de la mémoire partagée");
        exit(EXIT_FAILURE);
    }
    files_taches = mmap(NULL, sizeof(FileTaches) * 3, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd_files_taches, 0);
    if (files_taches == MAP_FAILED) {
        perror("Superviseur: Erreur lors du mapping de la mémoire partagée");
        exit(EXIT_FAILURE);
    }

    // Initialiser les files de tâches
    for (int i = 0; i < 3; i++) {
        files_taches[i].head = 0;
        files_taches[i].tail = 0;

        // Générer des noms fixes pour les sémaphores
        snprintf(files_taches[i].mutex_name, 64, "/mutex_sem_%d", i);
        snprintf(files_taches[i].items_name, 64, "/items_sem_%d", i);

        // Supprimer les sémaphores existants s'ils existent
        sem_unlink(files_taches[i].mutex_name);
        sem_unlink(files_taches[i].items_name);

        // Initialiser les sémaphores nommés
        sem_t *mutex = sem_open(files_taches[i].mutex_name, O_CREAT | O_EXCL, 0666, 1);
        sem_t *items = sem_open(files_taches[i].items_name, O_CREAT | O_EXCL, 0666, 0);

        if (mutex == SEM_FAILED || items == SEM_FAILED) {
            perror("Superviseur: Erreur lors de l'initialisation des sémaphores");
            exit(EXIT_FAILURE);
        }

        // Fermer les sémaphores dans le superviseur
        sem_close(mutex);
        sem_close(items);
    }

    // Initialiser les tâches et les ajouter à la file d'assemblage
    init_taches(nombre_de_taches);

    // Créer tous les robots
    create_all_robots();

    sleep(3); // Donne le temps aux robots de s'initialiser

    // Entrer dans la boucle principale du superviseur
    superviseur_loop();

    // Après superviseur_loop, toutes les tâches sont terminées
    // Envoyer SIGTERM à tous les robots pour les arrêter
    printf("Superviseur: Envoi de SIGTERM à tous les robots.\n");
    for (int i = 0; i < NB_ROBOTS; i++) {
        if (robots[i].is_alive) {
            kill(robots[i].pid, SIGTERM);
        }
    }

    // Attendre que tous les robots se terminent
    for (int i = 0; i < NB_ROBOTS; i++) {
        if (robots[i].is_alive) {
            waitpid(robots[i].pid, NULL, 0);
            robots[i].is_alive = 0;
            printf("Superviseur: Robot %d (PID: %d) a été terminé.\n", i, robots[i].pid);
        }
    }

    // Nettoyage des sémaphores nommés
    for (int i = 0; i < 3; i++) {
        sem_unlink(files_taches[i].mutex_name);
        sem_unlink(files_taches[i].items_name);
    }

    // Nettoyage de la mémoire partagée
    munmap(files_taches, sizeof(FileTaches) * 3);
    shm_unlink(SHM_FILES_TACHES);

    printf("Superviseur: Toutes les tâches ont été traitées.\n");
}

void init_taches(int nombre_de_taches) {
    // Créer les tâches et les ajouter à la file d'assemblage
    for (int i = 0; i < nombre_de_taches; i++) {
        Tache tache;
        tache.id = i;
        tache.type_actuel = ASSEMBLAGE;
        ajouter_tache(&files_taches[ASSEMBLAGE], tache);
    }
}

void superviseur_loop() {
    while (1) {
        check_robots_alive(); // Vérifie si les robots sont vivants

        // Vérifier si toutes les tâches ont été traitées
        int taches_restantes = 0;
        for (int i = 0; i < 3; i++) {
            sem_t *mutex = sem_open(files_taches[i].mutex_name, 0);
            if (mutex == SEM_FAILED) {
                perror("Superviseur: Erreur lors de l'ouverture du sémaphore mutex");
                exit(EXIT_FAILURE);
            }
            sem_wait(mutex);
            if (files_taches[i].head != files_taches[i].tail) {
                taches_restantes = 1;
            }
            sem_post(mutex);
            sem_close(mutex);
        }
        if (!taches_restantes) {
            printf("Superviseur: Toutes les tâches ont été traitées.\n");
            tasks_done = 1;
            break;
        }
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
                    if (!tasks_done) {
                        printf("Superviseur: Robot %d (PID: %d) est mort, création d'un nouveau robot.\n", i, robots[i].pid);
                        robots[i].is_alive = 0;
                        create_robot(i, robots[i].type_robot);
                    } else {
                        printf("Superviseur: Robot %d (PID: %d) est mort.\n", i, robots[i].pid);
                        robots[i].is_alive = 0;
                    }
                } else {
                    // Une autre erreur est survenue
                    perror("Superviseur: Erreur lors de la vérification du robot");
                }
            } else {
                // Le processus est vivant
                printf("Superviseur: Robot %d (PID: %d) est vivant, tout va bien.\n", i, robots[i].pid);
            }
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
