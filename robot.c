#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include <time.h>
#include <sys/shm.h>
#include "structures.h"
#include "shared_memory.h"
#include "queue.h"
#include "robot.h"
#include "superviseur.h"
#include "constantes.h"

// L'id du robot
int robot_id;

// Le type de tâche que le robot peut traiter
TypeTache robot_type;

// La file de tâches partagée
FileTaches *files_taches;

// L'affectation des robots
int *affectation;

// Le compteur de tâches terminées
int *tasks_done;

// Mutex pour protéger le compteur de tâches terminées
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
        fprintf(stderr, "Robot : Erreur Arguments invalides.\n");
        exit(EXIT_FAILURE);
    }

    printf("Robot %d de type %s démarré.\n", robot_id, type_robot_to_string(robot_type));

    // Installer le gestionnaire de signal pour SIGTERM
    if (signal(SIGTERM, handle_sigterm) == SIG_ERR)
    {
        perror("Robot: Erreur lors de l'association de SIGTERM");
        exit(EXIT_FAILURE);
    }

    // Initialiser les mémoires partagées
    files_taches = open_shared_memory(SHM_FILES_TACHES, sizeof(FileTaches) * 3);
    affectation = open_shared_memory(SHM_AFFECTATION, sizeof(int) * NB_ROBOTS);
    tasks_done = open_shared_memory(SHM_TASKS_DONE, sizeof(int));

    // Ouvrir le sémaphore qui protège le compteur de tâches terminées
    mutex_tasks_done = open_semaphore(SEM_MUTEX_TASKS_DONE);

    // Initialiser le générateur de nombres aléatoires
    srand(time(NULL));

    // Boucle principale
    while (1)
    {
        Tache tache;
        if (retirer_tache(&files_taches[robot_type], &tache))
        {
            // On signale que le robot traite la tâche
            affectation[robot_id] = tache.id;

            // Simuler une panne aléatoire
            if (rand() % PANNE_PROBABILITE == 0)
            {
                printf("Robot %d de type %s est tombé en panne en traitant la tâche %d.\n", robot_id, type_robot_to_string(robot_type), tache.id);

                // Libérer les ressources avant de quitter
                shmdt(files_taches);
                shmdt(affectation);
                shmdt(tasks_done);
                exit(EXIT_FAILURE);
            }

            traiter_tache(&tache, tasks_done);

            // On signale que le robot a terminé la tâche
            affectation[robot_id] = -1;
        }
    }
    return 0;
}

/**
 * Intercepte le signal SIGTERM pour terminer le robot.
 */
void handle_sigterm()
{
    printf("Robot %d de type %s reçoit SIGTERM, s'arrête.\n", robot_id, type_robot_to_string(robot_type));
    // Libérer les mémoires partagées avant de quitter
    shmdt(files_taches);
    shmdt(affectation);
    shmdt(tasks_done);
    exit(0);
}

/**
 * Convertit un type de tâche en chaîne de caractères.
 * @param type Type de tâche.
 * @return Chaîne de caractères correspondante.
 */
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

/**
 * Traite une tâche en fonction du type de robot.
 * @param tache Tâche à traiter.
 * @param tasks_done Pointeur vers le compteur de tâches terminées.
 */
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

    // Simuler le temps de traitement
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
        sem_post(mutex_tasks_done);
    }
}
