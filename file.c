// queue.c

#include <stdlib.h>
#include <stdio.h>
#include "file.h"

// Fonction pour initialiser les files de tâches
void init_files_taches(FileTaches *files_taches)
{
    for (int i = 0; i < 3; i++)
    {
        files_taches[i].tete = NULL;
        files_taches[i].queue = NULL;

        // Initialisation des sémaphores
        if (sem_init(&files_taches[i].sem_mutex, 1, 1) == -1)
        {
            perror("Erreur lors de l'initialisation du sémaphore pour les files de tâches");
            exit(EXIT_FAILURE);
        }

        if (sem_init(&files_taches[i].sem_taches, 1, 0) == -1)
        {
            perror("Erreur lors de l'initialisation du sémaphore pour les tâches dans les files");
            exit(EXIT_FAILURE);
        }
    }
}

// Fonction pour ajouter une tâche dans une file, retourne 1 si la tâche a été ajoutée, 0 sinon
void ajouter_tache(FileTaches *file, Tache tache)
{
    Node *nouveau = malloc(sizeof(Node));
    if (!nouveau)
    {
        perror("Erreur d'allocation mémoire pour la nouvelle tâche");
        exit(EXIT_FAILURE);
    }
    nouveau->tache = tache;
    nouveau->suivant = NULL;


    sem_wait(&file->sem_mutex);
    if (file->queue == NULL)
    {
        file->tete = file->queue = nouveau;
    }
    else
    {
        file->queue->suivant = nouveau;
        file->queue = nouveau;
    }
    sem_post(&file->sem_mutex);

    // Notifie qu'il y a une tâche dans la file
    sem_post(&file->sem_taches);
}

// Fonction pour retirer une tâche d'une file, retourne 1 si une tâche a été retirée, 0 sinon
int retirer_tache(FileTaches *file, Tache *tache) {
    // Attendre qu'une tâche soit disponible
    sem_wait(&file->sem_taches);

    // Protéger l'accès à la file avec le sémaphore mutex
    sem_wait(&file->sem_mutex);
    Node *temp = file->tete;

    if (temp == NULL) {
        // Improbable, mais on ne sait jamais
        sem_post(&file->sem_mutex);
        return 0;
    }

    // Copie de la tâche
    *tache = temp->tache;
    file->tete = file->tete->suivant;
    
    // Si la file est vide, on met à jour la queue
    if (file->tete == NULL) {
        file->queue = NULL;
    }

    // Libérer le nœud
    sem_post(&file->sem_mutex);
    free(temp);
    return 1;
}