/*
Name: Kaeden Ondrus
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>

const int NUM_CARS = 8;

//crossing times in seconds
const int LEFT_TIME = 5;
const int STRAIGHT_TIME = 4;
const int RIGHT_TIME = 3;

//Location of arrival (direction the car is heading, so HOL_N means the car is heading north, so it arrived from the south)
sem_t HOL_N;
sem_t HOL_E;
sem_t HOL_S;
sem_t HOL_W;
//Semaphore for the first car to arrive at the actual intersection
sem_t ArrivedFirst;
//Semaphores for each of the movement directions
sem_t N_to_N;
sem_t N_to_E;
sem_t N_to_S;
sem_t N_to_W;
sem_t E_to_E;
sem_t E_to_S;
sem_t E_to_W;
sem_t E_to_N;
sem_t S_to_S;
sem_t S_to_W;
sem_t S_to_N;
sem_t S_to_E;
sem_t W_to_W;
sem_t W_to_N;
sem_t W_to_E;
sem_t W_to_S;


struct timespec start_time;

//returns the elapsed time in seconds since the start of the program
double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec - start_time.tv_sec) + (ts.tv_nsec - start_time.tv_nsec) / 1e9;
}

typedef struct _directions {
    int cid;
    double arrival_time;
    char dir_original;
    char dir_target;
} directions;

directions cars[NUM_CARS] = {
    {1, 1.1, 'N', 'N'},
    {2, 2.2, 'N', 'N'},
    {3, 3.3, 'N', 'W'},
    {4, 4.4, 'S', 'S'},
    {5, 5.5, 'S', 'E'},
    {6, 6.6, 'N', 'N'},
    {7, 7.7, 'E', 'N'},
    {8, 8.8, 'W', 'N'}
};

char decode(char direction){
    switch (direction) {
        case 'N':
            return '^';
        case 'E':
            return '>';
        case 'S':
            return 'v';
        case 'W':
            return '<';
    }
}


void ArriveIntersection(directions dir) {
    printf("Time %.1f: Car %d (%c %c) arriving\n", get_time(), dir.cid, decode(dir.dir_original), decode(dir.dir_target));

    //simulate the time taken to arrive at the intersection (2 seconds)
    usleep(2000000);

    //wait until the car is at the front if its line
    switch (dir.dir_original) {
        case 'N':
            sem_wait(&HOL_N);
            break;
        case 'E':
            sem_wait(&HOL_E);
            break;
        case 'S':
            sem_wait(&HOL_S);
            break;
        case 'W':
            sem_wait(&HOL_W);
            break;
    }

    //wait until the car is the first to arrive at any of the stop signs (first to the actual intersection)
    sem_wait(&ArrivedFirst);

    //acquire the required semaphores depending on the movement direction of the car
    switch (dir.dir_original) {
        case 'N':
            switch (dir.dir_target) {
                case 'N':
                    sem_wait(&N_to_N);
                    sem_wait(&E_to_E);
                    sem_wait(&E_to_N);
                    sem_wait(&S_to_E);
                    sem_wait(&W_to_W);
                    sem_wait(&W_to_N);
                    sem_wait(&W_to_S);
                    break;
                case 'E':
                    sem_wait(&N_to_E);
                    sem_wait(&E_to_E);
                    sem_wait(&S_to_E);
                    break;
                case 'W':
                    sem_wait(&N_to_W);
                    sem_wait(&E_to_E);
                    sem_wait(&E_to_N);
                    sem_wait(&S_to_S);
                    sem_wait(&S_to_W);
                    sem_wait(&W_to_S);
                    sem_wait(&W_to_W);
                    break;
            }
            break;
        case 'E':
            switch (dir.dir_target) {
                case 'E':
                    sem_wait(&E_to_E);
                    sem_wait(&S_to_S);
                    sem_wait(&S_to_E);
                    sem_wait(&W_to_S);
                    sem_wait(&N_to_N);
                    sem_wait(&N_to_E);
                    sem_wait(&N_to_W);
                    break;
                case 'S':
                    sem_wait(&E_to_S);
                    sem_wait(&S_to_S);
                    sem_wait(&W_to_S);
                    break;
                case 'N':
                    sem_wait(&E_to_N);
                    sem_wait(&S_to_S);
                    sem_wait(&S_to_E);
                    sem_wait(&W_to_W);
                    sem_wait(&W_to_N);
                    sem_wait(&N_to_W);
                    sem_wait(&N_to_N);
                    break;
            }
            break;
        case 'S':
            switch (dir.dir_target) {
                case 'S':
                    sem_wait(&S_to_S);
                    sem_wait(&W_to_W);
                    sem_wait(&W_to_S);
                    sem_wait(&N_to_W);
                    sem_wait(&E_to_E);
                    sem_wait(&E_to_S);
                    sem_wait(&E_to_N);
                    break;
                case 'W':
                    sem_wait(&S_to_W);
                    sem_wait(&W_to_W);
                    sem_wait(&N_to_W);
                    break;
                case 'E':
                    sem_wait(&S_to_E);
                    sem_wait(&W_to_W);
                    sem_wait(&W_to_S);
                    sem_wait(&N_to_N);
                    sem_wait(&N_to_E);
                    sem_wait(&E_to_N);
                    sem_wait(&E_to_E);
                    break;
            }
            break;
        case 'W':
            switch (dir.dir_target) {
                case 'W':
                    sem_wait(&W_to_W);
                    sem_wait(&N_to_N);
                    sem_wait(&N_to_W);
                    sem_wait(&E_to_N);
                    sem_wait(&S_to_S);
                    sem_wait(&S_to_W);
                    sem_wait(&S_to_E);
                    break;
                case 'N':
                    sem_wait(&W_to_N);
                    sem_wait(&N_to_N);
                    sem_wait(&E_to_N);
                    break;
                case 'S':
                    sem_wait(&W_to_S);
                    sem_wait(&N_to_N);
                    sem_wait(&N_to_W);
                    sem_wait(&E_to_E);
                    sem_wait(&E_to_S);
                    sem_wait(&S_to_E);
                    sem_wait(&S_to_S);
                    break;
            }
            break;
    }

    //The car may now cross the intersection as all of the prerequisite semaphores have been acquired
    //This is handled by the Car function
}

void CrossIntersection(directions dir) {
    printf("Time %.1f: Car %d (%c %c)          crossing\n", get_time(), dir.cid, decode(dir.dir_original), decode(dir.dir_target));

    //release head of line semaphores as crossing begins
    switch(dir.dir_original) {
        case 'N':
            sem_post(&HOL_N);
            break;
        case 'E':
            sem_post(&HOL_E);
            break;
        case 'S':
            sem_post(&HOL_S);
            break;
        case 'W':
            sem_post(&HOL_W);
            break;
    }

    //simulate the time taken to cross the intersection based on the movement direction of the car
    int time;
    if (dir.dir_original == dir.dir_target) {
        time = STRAIGHT_TIME;
    } else {
        if ((dir.dir_original == 'N' && dir.dir_target == 'W') ||
            (dir.dir_original == 'S' && dir.dir_target == 'E') ||
            (dir.dir_original == 'E' && dir.dir_target == 'N') ||
            (dir.dir_original == 'W' && dir.dir_target == 'S')) {
            time = LEFT_TIME;
        } else {
            time = RIGHT_TIME;
        }
    }
    usleep(time * 1000000);
}

void ExitIntersection(directions dir) {
    printf("Time %.1f: Car %d (%c %c)                   exiting\n", get_time(), dir.cid, decode(dir.dir_original), decode(dir.dir_target));

    //release the movement direction semaphores that were acquired for crossing
    switch (dir.dir_original) {
        case 'N':
            switch (dir.dir_target) {
                case 'N':
                    sem_post(&N_to_N);
                    sem_post(&E_to_E);
                    sem_post(&E_to_N);
                    sem_post(&S_to_E);
                    sem_post(&W_to_W);
                    sem_post(&W_to_N);
                    sem_post(&W_to_S);
                    break;
                case 'E':
                    sem_post(&N_to_E);
                    sem_post(&E_to_E);
                    sem_post(&S_to_E);
                    break;
                case 'W':
                    sem_post(&N_to_W);
                    sem_post(&E_to_E);
                    sem_post(&E_to_N);
                    sem_post(&S_to_S);
                    sem_post(&S_to_W);
                    sem_post(&W_to_S);
                    sem_post(&W_to_W);
                    break;
            }
            break;
        case 'E':
            switch (dir.dir_target) {
                case 'E':
                    sem_post(&E_to_E);
                    sem_post(&S_to_S);
                    sem_post(&S_to_E);
                    sem_post(&W_to_S);
                    sem_post(&N_to_N);
                    sem_post(&N_to_E);
                    sem_post(&N_to_W);
                    break;
                case 'S':
                    sem_post(&E_to_S);
                    sem_post(&S_to_S);
                    sem_post(&W_to_S);
                    break;
                case 'N':
                    sem_post(&E_to_N);
                    sem_post(&S_to_S);
                    sem_post(&S_to_E);
                    sem_post(&W_to_W);
                    sem_post(&W_to_N);
                    sem_post(&N_to_W);
                    sem_post(&N_to_N);
                    break;
            }
            break;
        case 'S':
            switch (dir.dir_target) {
                case 'S':
                    sem_post(&S_to_S);
                    sem_post(&W_to_W);
                    sem_post(&W_to_S);
                    sem_post(&N_to_W);
                    sem_post(&E_to_E);
                    sem_post(&E_to_S);
                    sem_post(&E_to_N);
                    break;
                case 'W':
                    sem_post(&S_to_W);
                    sem_post(&W_to_W);
                    sem_post(&N_to_W);
                    break;
                case 'E':
                    sem_post(&S_to_E);
                    sem_post(&W_to_W);
                    sem_post(&W_to_S);
                    sem_post(&N_to_N);
                    sem_post(&N_to_E);
                    sem_post(&E_to_N);
                    sem_post(&E_to_E);
                    break;
            }
            break;
        case 'W':
            switch (dir.dir_target) {
                case 'W':
                    sem_post(&W_to_W);
                    sem_post(&N_to_N);
                    sem_post(&N_to_W);
                    sem_post(&E_to_N);
                    sem_post(&S_to_S);
                    sem_post(&S_to_W);
                    sem_post(&S_to_E);
                    break;
                case 'N':
                    sem_post(&W_to_N);
                    sem_post(&N_to_N);
                    sem_post(&E_to_N);
                    break;
                case 'S':
                    sem_post(&W_to_S);
                    sem_post(&N_to_N);
                    sem_post(&N_to_W);
                    sem_post(&E_to_E);
                    sem_post(&E_to_S);
                    sem_post(&S_to_E);
                    sem_post(&S_to_S);
                    break;
            }
            break;
    }
}

void Car(directions dir) {
    //wait until the arrival time of the car has been reached
    while (get_time() < dir.arrival_time) {
        usleep(100000); //sleep for 100ms to avoid busy waiting
    }
    ArriveIntersection(dir);
    CrossIntersection(dir);
    ExitIntersection(dir);
}

int main(void) {
    
    //initialize the semaphores
    sem_init(&HOL_N, 0, 1);
    sem_init(&HOL_E, 0, 1);
    sem_init(&HOL_S, 0, 1);
    sem_init(&HOL_W, 0, 1);
    sem_init(&ArrivedFirst, 0, 1);
    sem_init(&N_to_N, 0, 1);
    sem_init(&N_to_E, 0, 1);
    sem_init(&N_to_S, 0, 1);
    sem_init(&N_to_W, 0, 1);
    sem_init(&E_to_E, 0, 1);
    sem_init(&E_to_S, 0, 1);
    sem_init(&E_to_W, 0, 1);
    sem_init(&E_to_N, 0, 1);
    sem_init(&S_to_S, 0, 1);
    sem_init(&S_to_W, 0, 1);
    sem_init(&S_to_N, 0, 1);
    sem_init(&S_to_E, 0, 1);
    sem_init(&W_to_W, 0, 1);
    sem_init(&W_to_N, 0, 1);
    sem_init(&W_to_E, 0, 1);
    sem_init(&W_to_S, 0, 1);

    //get start time for the program
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    //create threads for each of the cars
    pthread_t threads[NUM_CARS];
    //load the car data from the cars array and create a thread for each car to execute the Car function
    for (int i = 0; i < NUM_CARS; i++) {
        pthread_create(&threads[i], NULL, Car, &cars[i]);
    }
    
    //wait for all threads to finish
    for (int i = 0; i < NUM_CARS; i++) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}