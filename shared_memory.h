// shared_memory.h

#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#define SHM_FILES_TACHES "/shm_files_taches"
#define SHM_AFFECTATION "/shm_affectation"
#define SHM_TASKS_DONE "/shm_tasks_done"
#define SEM_MUTEX_TASKS_DONE "/sem_mutex_tasks_done"

void *open_shared_memory(const char *name, size_t size);
sem_t *open_semaphore(const char *name);
void* shared_memory_create(const char* shm_name, size_t size);

#endif // SHARED_MEMORY_H
