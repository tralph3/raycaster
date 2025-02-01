#ifndef SYNC_H
#define SYNC_H

#define TO_STRING(s) #s

#define INIT_MUTEX(mutex_name) do {                             \
        if (pthread_mutex_init(&mutex_name, NULL) != 0) {       \
            printf("Failed creating " TO_STRING(mutex_name));   \
            exit(1);                                            \
        }                                                       \
    } while(0)

#define INIT_SEM(sem_name, init) do {                       \
        if (sem_init(&sem_name, 0, init) != 0) {            \
            printf("Failed creating " TO_STRING(sem_name)); \
            exit(1);                                        \
        }                                                   \
    } while(0)

#endif
