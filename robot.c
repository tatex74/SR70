// robot.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <string.h>
#include <time.h>
#include "structures.h"
#include "shared_memory.h"
#include "queue.h"
#include "robot.h"
#include "superviseur.h"

int robot_id;
TypeTache robot_type;
FileTaches *files_taches;
int *affectation;
sem_t *mutex_tasks_done;

void *open_shared_memory(const char *name, size_t size);
sem_t *open_semaphore(const char *name);
void handle_sigterm(int signo);
char *type_robot_to_string(TypeTache type);

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s robot_id robot_type\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    robot_id = atoi(argv[1]);
    robot_type = atoi(argv[2]);

    printf("Robot %d de type %s démarré.\n", robot_id, type_robot_to_string(robot_type));

    // Installer le gestionnaire de signal pour SIGTERM
    struct sigaction sa_term;
    sa_term.sa_handler = handle_sigterm;
    sigemptyset(&sa_term.sa_mask);
    sa_term.sa_flags = 0;
    if (sigaction(SIGTERM, &sa_term, NULL) == -1)
    {
        perror("Robot: Erreur lors de l'association de SIGTERM");
        exit(EXIT_FAILURE);
    }

    // Initialiser les mémoires partagées
    files_taches = open_shared_memory(SHM_FILES_TACHES, sizeof(FileTaches) * 3);
    affectation = open_shared_memory(SHM_AFFECTATION, sizeof(int) * NB_ROBOTS);
    int *tasks_done = open_shared_memory(SHM_TASKS_DONE, sizeof(int));

    // Ouvrir le sémaphore
    mutex_tasks_done = open_semaphore(SEM_MUTEX_TASKS_DONE);

    // Initialiser le générateur de nombres aléatoires
    srand(time(NULL));

    // Boucle principale
    while (1)
    {
        Tache tache;
        if (retirer_tache(&files_taches[robot_type], &tache))
        {
            affectation[robot_id] = tache.id;

            // Simuler une panne aléatoire
            if (rand() % 2 == 0)
            {
                printf("Robot %d de type %s est tombé en panne en traitant la tâche %d.\n", robot_id, type_robot_to_string(robot_type), tache.id);
                
                // Libérer les ressources avant de quitter
                munmap(files_taches, sizeof(FileTaches) * 3);
                munmap(affectation, sizeof(int) * NB_ROBOTS);
                munmap(tasks_done, sizeof(int));
                exit(EXIT_FAILURE);
            }

            printf("Robot %d de type %s exécute la tâche %d.\n", robot_id, type_robot_to_string(robot_type), tache.id);
            
            // Simuler le temps de traitement en fonction du type de tâche
            sleep(robot_type == ASSEMBLAGE ? 6 : (robot_type == PEINTURE ? 3 : 2));

            if (robot_type == ASSEMBLAGE)
            {
                tache.type_actuel = PEINTURE;
                ajouter_tache(&files_taches[PEINTURE], tache);
            }
            else if (robot_type == PEINTURE)
            {
                tache.type_actuel = VERIFICATION;
                ajouter_tache(&files_taches[VERIFICATION], tache);
            }
            else
            {
                sem_wait(mutex_tasks_done);
                printf("Robot %d de type %s a terminé la tâche %d.\n", robot_id, type_robot_to_string(robot_type), tache.id);
                (*tasks_done)++;
                printf("Tâches terminées: %d\n", *tasks_done);
                sem_post(mutex_tasks_done);
            }

            affectation[robot_id] = -1;
        }
    }
    return 0;
}

void *open_shared_memory(const char *name, size_t size)
{
    int shm_fd = shm_open(name, O_RDWR, 0666);
    if (shm_fd == -1)
    {
        perror("Erreur lors de l'ouverture de la mémoire partagée");
        exit(EXIT_FAILURE);
    }

    void *addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (addr == MAP_FAILED)
    {
        perror("Erreur lors du mapping de la mémoire partagée");
        exit(EXIT_FAILURE);
    }

    return addr;
}

sem_t *open_semaphore(const char *name)
{
    sem_t *sem = sem_open(name, 0);
    if (sem == SEM_FAILED)
    {
        perror("Erreur lors de l'ouverture du sémaphore");
        exit(EXIT_FAILURE);
    }
    return sem;
}

void handle_sigterm(int signo)
{
    printf("Robot %d de type %s reçoit SIGTERM, s'arrête.\n", robot_id, type_robot_to_string(robot_type));
    exit(0);
}

char *type_robot_to_string(TypeTache type)
{
    switch (type)
    {
    case ASSEMBLAGE:
        return "ASSEMBLAGE";
    case PEINTURE:
        return "PEINTURE";
    case VERIFICATION:
        return "VERIFICATION";
    default:
        return "UNKNOWN";
    }
}
