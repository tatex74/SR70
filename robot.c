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
#include "constantes.h"

int robot_id;
TypeTache robot_type;
FileTaches *files_taches;
int *affectation;
sem_t *mutex_tasks_done;

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s robot_id robot_type\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    robot_id = atoi(argv[1]);
    robot_type = atoi(argv[2]);

    if (robot_id < 0 || robot_id >= NB_ROBOTS || robot_type < ASSEMBLAGE || robot_type > VERIFICATION)
    {
        fprintf(stderr, "Erreur: Arguments invalides.\n");
        exit(EXIT_FAILURE);
    }

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
            if (rand() % PANNE_PROBABILITE == 0)
            {
                printf("Robot %d de type %s est tombé en panne en traitant la tâche %d.\n", robot_id, type_robot_to_string(robot_type), tache.id);

                // Libérer les ressources avant de quitter
                munmap(files_taches, sizeof(FileTaches) * 3);
                munmap(affectation, sizeof(int) * NB_ROBOTS);
                munmap(tasks_done, sizeof(int));
                exit(EXIT_FAILURE);
            }

            traiter_tache(&tache, tasks_done);
            affectation[robot_id] = -1;
        }
    }
    return 0;
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

void traiter_tache(Tache *tache, int *tasks_done)
{
    printf("Robot %d de type %s exécute la tâche %d.\n", robot_id, type_robot_to_string(robot_type), tache->id);
    int sleep_time = 2;
    if (robot_type == ASSEMBLAGE)
    {
        sleep_time = 6;
    }
    else if (robot_type == PEINTURE)
    {
        sleep_time = 3;
    }

    sleep(sleep_time);


    if (robot_type == ASSEMBLAGE)
    {
        tache->type_actuel = PEINTURE;
        ajouter_tache(&files_taches[PEINTURE], *tache);
    }
    else if (robot_type == PEINTURE)
    {
        tache->type_actuel = VERIFICATION;
        ajouter_tache(&files_taches[VERIFICATION], *tache);
    }
    else
    {
        sem_wait(mutex_tasks_done);
        printf("Robot %d de type %s a terminé la tâche %d.\n", robot_id, type_robot_to_string(robot_type), tache->id);
        (*tasks_done)++;
        printf("Tâches terminées: %d\n", *tasks_done);
        sem_post(mutex_tasks_done);
    }
}
