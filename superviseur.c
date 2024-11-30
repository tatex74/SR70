// superviseur.c

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include <sys/shm.h>
#include "structures.h"
#include "shared_memory.h"
#include "queue.h"
#include "superviseur.h"

Robot robots[NB_ROBOTS];
FileTaches *files_taches;
int *affectation;
int *tasks_done = NULL;
int nombre_de_taches = NB_ROBOTS;

int main(int argc, char *argv[])
{
    superviseur_init(argc, argv);
    return 0;
}

void superviseur_init(int argc, char *argv[])
{
    if (argc >= 2)
    {
        nombre_de_taches = atoi(argv[1]);
        if (nombre_de_taches <= 0)
        {
            fprintf(stderr, "Erreur: Le nombre de tâches doit être un entier positif.\n");
            exit(EXIT_FAILURE);
        }
    }

    printf("Superviseur: Initialisation avec %d tâches.\n", nombre_de_taches);

    init_signals();
    init_semaphores();
    init_shared_memory();
    init_files_taches();
    init_taches(nombre_de_taches);
    create_all_robots();

    sleep(3); // Laisser le temps aux robots de démarrer

    superviseur_loop();

    // Terminer les robots et nettoyer les ressources
    printf("Superviseur: Envoi de SIGTERM à tous les robots.\n");
    for (int i = 0; i < NB_ROBOTS; i++)
    {
        kill(robots[i].pid, SIGTERM);
    }

    for (int i = 0; i < NB_ROBOTS; i++)
    {
        waitpid(robots[i].pid, NULL, 0);
        printf("Superviseur: Robot %d (PID: %d) terminé.\n", i, robots[i].pid);
    }

    cleanup_resources();
    printf("Superviseur: Terminé.\n");
}

void init_shared_memory()
{
    
    files_taches = shared_memory_create(SHM_FILES_TACHES, sizeof(FileTaches) * NB_FILES_TACHES);
    affectation = shared_memory_create(SHM_AFFECTATION, sizeof(int) * NB_ROBOTS);
    tasks_done = shared_memory_create(SHM_TASKS_DONE, sizeof(int));

    *tasks_done = 0;

    for (int i = 0; i < NB_ROBOTS; i++) 
    {
        affectation[i] = -1;
    }
}

void init_semaphores() 
{
    sem_unlink(SEM_MUTEX_TASKS_DONE);
    sem_t *mutex_tasks_done = sem_open(SEM_MUTEX_TASKS_DONE, O_CREAT | O_EXCL, 0666, 1);
    if (mutex_tasks_done == SEM_FAILED)
    {
        perror("Superviseur: Erreur lors de l'initialisation du sémaphore mutex_tasks_done");
        cleanup_resources();
        exit(EXIT_FAILURE);
    }
    sem_close(mutex_tasks_done);
}

void init_files_taches()
{
    for (int i = 0; i < NB_FILES_TACHES; i++)
    {
        files_taches[i].head = 0;
        files_taches[i].tail = 0;

        snprintf(files_taches[i].mutex_name, 64, "/mutex_sem_%d_%d", getpid(), i);
        snprintf(files_taches[i].items_name, 64, "/items_sem_%d_%d", getpid(), i);

        sem_unlink(files_taches[i].mutex_name);
        sem_unlink(files_taches[i].items_name);

        sem_t *mutex = sem_open(files_taches[i].mutex_name, O_CREAT | O_EXCL, 0666, 1);
        sem_t *items = sem_open(files_taches[i].items_name, O_CREAT | O_EXCL, 0666, 0);

        if (mutex == SEM_FAILED || items == SEM_FAILED)
        {
            perror("Superviseur: Erreur lors de l'initialisation des sémaphores");
            cleanup_resources();
            exit(EXIT_FAILURE);
        }
    }
}

void cleanup_resources()
{
    for (int i = 0; i < NB_FILES_TACHES; i++)
    {
        sem_unlink(files_taches[i].mutex_name);
        sem_unlink(files_taches[i].items_name);
    }

    if (files_taches != NULL)
    {
        shmdt(files_taches);
    }
    remove_shared_memory(SHM_FILES_TACHES);

    if (affectation != NULL)
    {
        shmdt(affectation);
    }
    remove_shared_memory(SHM_AFFECTATION);

    if (tasks_done != NULL)
    {
        shmdt(tasks_done);
    }
    remove_shared_memory(SHM_TASKS_DONE);
}

void init_signals()
{
    if (signal(SIGCHLD, sigchld_handler) == SIG_ERR)
    {
        perror("Superviseur: Erreur lors de l'association de SIGCHLD");
        exit(EXIT_FAILURE);
    }
}

void init_taches(int nombre_de_taches)
{
    for (int i = 0; i < nombre_de_taches; i++)
    {
        Tache tache;
        tache.id = i;
        tache.type_actuel = ASSEMBLAGE;
        ajouter_tache(&files_taches[ASSEMBLAGE], tache);
    }
}

void superviseur_loop()
{
    while (*tasks_done < nombre_de_taches)
    {
        printf("Superviseur: Tâches terminées: %d\n", *tasks_done);
        sleep(5);
    }
    printf("Superviseur: Toutes les tâches sont terminées.\n");
}

void create_all_robots()
{
    int robot_id = 0;
    for (int type = 0; type < NB_FILES_TACHES; type++)
    {
        for (int i = 0; i < NB_ROBOTS_PAR_TYPE; i++)
        {
            create_robot(robot_id++, type);
        }
    }
}

void create_robot(int robot_id, int type)
{
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("Superviseur: Erreur lors de fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        char id_str[10], type_str[10];
        snprintf(id_str, 10, "%d", robot_id);
        snprintf(type_str, 10, "%d", type);
        execl("./robot", "./robot", id_str, type_str, NULL);
        perror("Superviseur: Erreur execl");
        exit(EXIT_FAILURE);
    }
    else
    {
        robots[robot_id].pid = pid;
        robots[robot_id].type_robot = type;
        printf("Superviseur: Robot %d créé (PID %d)\n", robot_id, pid);
    }
}

void sigchld_handler()
{
    pid_t pid;
    int status;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        for (int i = 0; i < NB_ROBOTS; i++)
        {
            if (robots[i].pid == pid)
            {
                if (WIFEXITED(status))
                {
                    robots[i].exit_status = WEXITSTATUS(status);
                }
                else
                {
                    robots[i].exit_status = 1;
                }

                printf("Superviseur: Robot %d (PID: %d) terminé avec statut %d.\n", i, pid, robots[i].exit_status);

                if (robots[i].exit_status != 0)
                {
                    printf("Superviseur: Redémarrage du robot %d.\n", i);
                    if (affectation[i] != -1)
                    {
                        Tache tache = {affectation[i], robots[i].type_robot};
                        ajouter_tache(&files_taches[robots[i].type_robot], tache);
                        affectation[i] = -1;
                        printf("Superviseur: Tâche %d réaffectée.\n", tache.id);
                    }
                    create_robot(i, robots[i].type_robot);
                }
                break;
            }
        }
    }
}
