// shared_memory.h

#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <stddef.h>
#include <semaphore.h>

// Ouvre une mémoire partagée existante
void *open_shared_memory(key_t key, size_t size);

// Crée une mémoire partagée
void* shared_memory_create(key_t key, size_t size);

// Ouvre un sémaphore POSIX existant
sem_t *open_semaphore(const char *name);

// Supprime une mémoire partagée
void remove_shared_memory(key_t key);

#endif // SHARED_MEMORY_H
