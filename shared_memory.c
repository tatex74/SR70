#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <errno.h>
#include "shared_memory.h"
#include "constantes.h"

// Fonction pour mapper le nom de la mémoire partagée à un projet ID unique pour ftok
int get_shm_proj_id(const char *name)
{
    if (strcmp(name, SHM_FILES_TACHES) == 0)
        return 1;
    else if (strcmp(name, SHM_AFFECTATION) == 0)
        return 2;
    else if (strcmp(name, SHM_TASKS_DONE) == 0)
        return 3;
    else
        return 4; // ID par défaut ou erreur
}

void *open_shared_memory(const char *name, size_t size)
{
    key_t key = ftok("/tmp", get_shm_proj_id(name));
    if (key == -1)
    {
        perror("ftok failed in open_shared_memory");
        exit(EXIT_FAILURE);
    }

    int shm_id = shmget(key, size, 0666);
    if (shm_id == -1)
    {
        perror("shmget failed in open_shared_memory");
        exit(EXIT_FAILURE);
    }

    void *addr = shmat(shm_id, NULL, 0);
    if (addr == (void *) -1)
    {
        perror("shmat failed in open_shared_memory");
        exit(EXIT_FAILURE);
    }

    return addr;
}

void* shared_memory_create(const char* name, size_t size)
{
    key_t key = ftok("/tmp", get_shm_proj_id(name));
    if (key == -1)
    {
        perror("ftok failed in shared_memory_create");
        exit(EXIT_FAILURE);
    }

    int shm_id = shmget(key, size, IPC_CREAT | IPC_EXCL | 0666);
    if (shm_id == -1)
    {
        if (errno == EEXIST)
        {
            // La mémoire partagée existe déjà, essayer de l'ouvrir
            shm_id = shmget(key, size, 0666);
            if (shm_id == -1)
            {
                perror("shmget failed to open existing shared memory in shared_memory_create");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            perror("shmget failed in shared_memory_create");
            exit(EXIT_FAILURE);
        }
    }

    void *addr = shmat(shm_id, NULL, 0);
    if (addr == (void *) -1)
    {
        perror("shmat failed in shared_memory_create");
        exit(EXIT_FAILURE);
    }

    // Si créé nouvellement, initialiser la mémoire à zéro
    if (errno != EEXIST)
    {
        memset(addr, 0, size);
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

void remove_shared_memory(const char *name)
{
    key_t key = ftok("/tmp", get_shm_proj_id(name));
    if (key == -1)
    {
        perror("ftok failed in remove_shared_memory");
        return;
    }

    int shm_id = shmget(key, 0, 0);
    if (shm_id == -1)
    {
        perror("shmget failed in remove_shared_memory");
        return;
    }

    if (shmctl(shm_id, IPC_RMID, NULL) == -1)
    {
        perror("shmctl IPC_RMID failed in remove_shared_memory");
    }
}
