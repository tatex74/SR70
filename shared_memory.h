// shared_memory.h

#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

void *open_shared_memory(const char *name, size_t size);
sem_t *open_semaphore(const char *name);
void* shared_memory_create(const char* shm_name, size_t size);

#endif // SHARED_MEMORY_H
