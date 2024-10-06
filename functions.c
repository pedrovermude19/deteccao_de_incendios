#include "functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_t sensor_threads[SIZE][SIZE];
pthread_mutex_t mutexes[SIZE][SIZE];
pthread_cond_t conditions[SIZE][SIZE];
pthread_mutex_t central_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t central_cond = PTHREAD_COND_INITIALIZER;
int new_event = 0;
char forest[SIZE][SIZE];


void initialize_forest() {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            forest[i][j] = '-'; 
            pthread_mutex_init(&mutexes[i][j], NULL);
            pthread_cond_init(&conditions[i][j], NULL);
        }
    }
}


void* sensor_function(void* args) {
    int x = ((int*)args)[0];
    int y = ((int*)args)[1];
    free(args);

    while (1) {
        pthread_mutex_lock(&mutexes[x][y]);

        if (forest[x][y] == '@') {
            propagate_fire(x, y);
        }

        pthread_mutex_unlock(&mutexes[x][y]);

        usleep(100000);
    }

    return NULL;
}


void* fire_generator(void* args) {
    while (1) {
        sleep(3); 
        int x = rand() % SIZE;
        int y = rand() % SIZE;

        pthread_mutex_lock(&mutexes[x][y]);
        if (forest[x][y] == '-') {
            forest[x][y] = '@';
            printf("Incêndio iniciado em (%d, %d)\n", x, y);
        }
        pthread_mutex_unlock(&mutexes[x][y]);
    }

    return NULL;
}


void propagate_fire(int x, int y) {
    forest[x][y] = '/'; 


    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue; // Ignora a célula atual
            int nx = x + i;
            int ny = y + j;
            if (nx >= 0 && nx < SIZE && ny >= 0 && ny < SIZE) {
                pthread_mutex_lock(&mutexes[nx][ny]);
                if (forest[nx][ny] == '-') {
                    forest[nx][ny] = '@'; 
                }
                pthread_mutex_unlock(&mutexes[nx][ny]);
            }
        }
    }

    if (x == 0 || x == SIZE-1 || y == 0 || y == SIZE-1) {
        send_to_central(x, y);
    }
}


void send_to_central(int x, int y) {
    pthread_mutex_lock(&central_mutex);
    new_event = 1;
    pthread_cond_signal(&central_cond);
    pthread_mutex_unlock(&central_mutex);
}


void* central_control(void* args) {
    while (1) {
        pthread_mutex_lock(&central_mutex);
        while (!new_event) {
            pthread_cond_wait(&central_cond, &central_mutex);
        }

        combat_fire();

        new_event = 0;
        pthread_mutex_unlock(&central_mutex);
    }

    return NULL;
}

void combat_fire() {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            pthread_mutex_lock(&mutexes[i][j]);
            if (forest[i][j] == '@') {
                forest[i][j] = '/'; 
                printf("Fogo combatido em (%d, %d)\n", i, j);
            }
            pthread_mutex_unlock(&mutexes[i][j]);
        }
    }
}


void print_forest() {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            printf("%c ", forest[i][j]);
        }
        printf("\n");
    }
}
