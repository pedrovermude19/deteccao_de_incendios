#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <pthread.h>

#define SIZE 30

extern pthread_t sensor_threads[SIZE][SIZE];
extern pthread_mutex_t mutexes[SIZE][SIZE];
extern pthread_cond_t conditions[SIZE][SIZE];
extern pthread_mutex_t central_mutex;
extern pthread_cond_t central_cond;
extern int new_event;
extern char forest[SIZE][SIZE];

void initialize_forest();
void* sensor_function(void* args);
void* fire_generator(void* args);
void* central_control(void* args);
void propagate_fire(int x, int y);
void send_to_central(int x, int y);
void combat_fire();
void print_forest();

#endif
