#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <string.h>
#include "shared_memory.h"


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

void* shared_memory_create(const char* shm_name, size_t size)
{
    int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1)
    {
        printf("Erreur lors de la création de la mémoire partagée %s", shm_name);
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shm_fd, size) == -1)
    {
        printf("Erreur lors du redimensionnement de la mémoire partagée %s", shm_name);
        shm_unlink(shm_name);
        exit(EXIT_FAILURE);
    }

    void* addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (addr == MAP_FAILED)
    {
        printf("Erreur lors du mappage de la mémoire partagée %s", shm_name);
        shm_unlink(shm_name);
        exit(EXIT_FAILURE);
    }

    return addr;
}