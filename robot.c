#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "robot.h"
#include "structures.h"

// On stocke l'id du robot et son type
int robot_id;
TypeTache type_robot;

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Robot: Erreur, veuillez spécifier l'index du robot et son type.\n");
        exit(EXIT_FAILURE);
    }

    robot_id = atoi(argv[1]);
    type_robot = atoi(argv[2]);
    robot_init();

    // Simuler le travail du robot
    while (1) {
        // Ici, vous pouvez ajouter le code spécifique du robot
        // Par exemple, simuler une tâche avec sleep
        sleep(1); // Simule une tâche en attente pendant 1 seconde
    }
    return 0;
}

void robot_init() {
    printf("Robot %d: Initialisation du robot de type %s.\n", robot_id, type_robot_to_string(type_robot));
    // Plus besoin de configurer le gestionnaire de signaux
}

// Fonction pour retourner le type du robot en chaîne de caractères
char *type_robot_to_string(TypeTache type) {
    switch (type) {
        case ASSEMBLAGE:
            return "assemblage";
        case PEINTURE:
            return "peinture";
        case VERIFICATION:
            return "vérification";
        default:
            return "inconnu";
    }
}
