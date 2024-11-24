//
// Created by Bruno on 24/11/2024.
//

#ifndef TASK_H
#define TASK_H

enum task {
    assembling,
    painting,
    carrying
};

enum state {
    running,
    paused,
    waiting_for_resources,
    completed
};

#endif //TASK_H
