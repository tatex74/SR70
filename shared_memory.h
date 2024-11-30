// shared_memory.h

#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <stddef.h>
#include <semaphore.h>

// Ouvre une mémoire partagée existante
void *open_shared_memory(const char *name, size_t size);

// Crée une mémoire partagée
void* shared_memory_create(const char* shm_name, size_t size);

// Ouvre un sémaphore POSIX existant
sem_t *open_semaphore(const char *name);

// Supprime une mémoire partagée
void remove_shared_memory(const char *name);

#endif // SHARED_MEMORY_H
