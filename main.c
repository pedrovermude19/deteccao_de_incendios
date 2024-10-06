#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "functions.h"

int main() {
    initialize_forest();

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            int* args = malloc(2 * sizeof(int));
            args[0] = i;
            args[1] = j;
            pthread_create(&sensor_threads[i][j], NULL, sensor_function, (void*)args);
        }
    }


    pthread_t fire_thread;
    pthread_create(&fire_thread, NULL, fire_generator, NULL);


    pthread_t central_thread;
    pthread_create(&central_thread, NULL, central_control, NULL);


    while (1) {
        print_forest();
        sleep(1);
    }

    return 0;
}
