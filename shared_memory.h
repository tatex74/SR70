// shared_memory.h

#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <stddef.h>
#include <semaphore.h>

void *open_shared_memory(key_t key, size_t size);
void* shared_memory_create(key_t key, size_t size);
sem_t *open_semaphore(const char *name);
void remove_shared_memory(key_t key);

#endif // SHARED_MEMORY_H
