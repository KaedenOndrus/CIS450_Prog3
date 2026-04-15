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

#define NUM_CARS 8

//crossing times in seconds
const int LEFT_TIME = 5;
const int STRAIGHT_TIME = 4;
const int RIGHT_TIME = 3;

//Location of arrival (direction the car is heading, so HOL_N means the car is heading north, so it arrived from the south)
sem_t HOL_N;
sem_t HOL_E;
sem_t HOL_S;
sem_t HOL_W;

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

//locks for integer counters
sem_t lockN_to_N;
sem_t lockN_to_E;
sem_t lockN_to_S;
sem_t lockN_to_W;
sem_t lockE_to_E;
sem_t lockE_to_S;
sem_t lockE_to_W;
sem_t lockE_to_N;
sem_t lockS_to_S;
sem_t lockS_to_W;
sem_t lockS_to_N;
sem_t lockS_to_E;
sem_t lockW_to_W;
sem_t lockW_to_N;
sem_t lockW_to_E;
sem_t lockW_to_S;

//integer counters
int cntN_to_N;
int cntN_to_E;
int cntN_to_S;
int cntN_to_W;
int cntE_to_E;
int cntE_to_S;
int cntE_to_W;
int cntE_to_N;
int cntS_to_S;
int cntS_to_W;
int cntS_to_N;
int cntS_to_E;
int cntW_to_W;
int cntW_to_N;
int cntW_to_E;
int cntW_to_S;

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
    {.cid = 1, .arrival_time = 1.1, .dir_original = 'N', .dir_target = 'N'},
    {.cid = 2, .arrival_time = 2.2, .dir_original = 'N', .dir_target = 'N'},
    {.cid = 3, .arrival_time = 3.3, .dir_original = 'N', .dir_target = 'W'},
    {.cid = 4, .arrival_time = 4.4, .dir_original = 'S', .dir_target = 'S'},
    {.cid = 5, .arrival_time = 5.5, .dir_original = 'S', .dir_target = 'E'},
    {.cid = 6, .arrival_time = 6.6, .dir_original = 'N', .dir_target = 'N'},
    {.cid = 7, .arrival_time = 7.7, .dir_original = 'E', .dir_target = 'N'},
    {.cid = 8, .arrival_time = 8.8, .dir_original = 'W', .dir_target = 'N'}
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

void route_acquire(sem_t *lock, sem_t *route, int *counter) {
    sem_wait(lock);
    if (++*counter == 1) {
        sem_wait(route);
    }
    sem_post(lock);
}

void route_release(sem_t *lock, sem_t *route, int *counter) {
    sem_wait(lock);
    if (--*counter == 0) {
        sem_post(route);
    }
    sem_post(lock);
}

void ArriveIntersection(directions dir) {
    printf("Time %.1f: Car %d (%c %c) arriving\n", get_time(), dir.cid, decode(dir.dir_original), decode(dir.dir_target));
    fflush(stdout);

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

    //acquire the required semaphores depending on the movement direction of the car
    switch (dir.dir_original) {
        case 'N':
            switch (dir.dir_target) {
                case 'N':
                    sem_wait(&N_to_N);
                    route_acquire(&lockE_to_E, &E_to_E, &cntE_to_E);
                    route_acquire(&lockE_to_N, &E_to_N, &cntE_to_N);
                    route_acquire(&lockS_to_E, &S_to_E, &cntS_to_E);
                    route_acquire(&lockW_to_N, &W_to_N, &cntW_to_N);
                    route_acquire(&lockW_to_S, &W_to_S, &cntW_to_S);
                    route_acquire(&lockW_to_W, &W_to_W, &cntW_to_W);
                    sem_post(&N_to_N);
                    break;
                case 'E':
                    sem_wait(&N_to_E);
                    route_acquire(&lockE_to_E, &E_to_E, &cntE_to_E);
                    route_acquire(&lockS_to_E, &S_to_E, &cntS_to_E);
                    sem_post(&N_to_E);
                    break;
                case 'W':
                    sem_wait(&N_to_W);
                    route_acquire(&lockE_to_E, &E_to_E, &cntE_to_E);
                    route_acquire(&lockE_to_N, &E_to_N, &cntE_to_N);
                    route_acquire(&lockS_to_S, &S_to_S, &cntS_to_S);
                    route_acquire(&lockS_to_W, &S_to_W, &cntS_to_W);
                    route_acquire(&lockW_to_S, &W_to_S, &cntW_to_S);
                    route_acquire(&lockW_to_W, &W_to_W, &cntW_to_W);
                    sem_post(&N_to_W);
                    break;
            }
            break;
        case 'E':
            switch (dir.dir_target) {
                case 'E':
                    sem_wait(&E_to_E);
                    route_acquire(&lockN_to_E, &N_to_E, &cntN_to_E);
                    route_acquire(&lockN_to_N, &N_to_N, &cntN_to_N);
                    route_acquire(&lockN_to_W, &N_to_W, &cntN_to_W);
                    route_acquire(&lockS_to_E, &S_to_E, &cntS_to_E);
                    route_acquire(&lockS_to_S, &S_to_S, &cntS_to_S);
                    route_acquire(&lockW_to_S, &W_to_S, &cntW_to_S);
                    sem_post(&E_to_E);
                    break;
                case 'S':
                    sem_wait(&E_to_S);
                    route_acquire(&lockS_to_S, &S_to_S, &cntS_to_S);
                    route_acquire(&lockW_to_S, &W_to_S, &cntW_to_S);
                    sem_post(&E_to_S);
                    break;
                case 'N':
                    sem_wait(&E_to_N);
                    route_acquire(&lockN_to_N, &N_to_N, &cntN_to_N);
                    route_acquire(&lockN_to_W, &N_to_W, &cntN_to_W);
                    route_acquire(&lockS_to_E, &S_to_E, &cntS_to_E);
                    route_acquire(&lockS_to_S, &S_to_S, &cntS_to_S);
                    route_acquire(&lockW_to_N, &W_to_N, &cntW_to_N);
                    route_acquire(&lockW_to_W, &W_to_W, &cntW_to_W);
                    sem_post(&E_to_N);
                    break;
            }
            break;
        case 'S':
            switch (dir.dir_target) {
                case 'S':
                    sem_wait(&S_to_S);
                    route_acquire(&lockE_to_E, &E_to_E, &cntE_to_E);
                    route_acquire(&lockE_to_N, &E_to_N, &cntE_to_N);
                    route_acquire(&lockE_to_S, &E_to_S, &cntE_to_S);
                    route_acquire(&lockN_to_W, &N_to_W, &cntN_to_W);
                    route_acquire(&lockW_to_S, &W_to_S, &cntW_to_S);
                    route_acquire(&lockW_to_W, &W_to_W, &cntW_to_W);
                    sem_post(&S_to_S);
                    break;
                case 'W':
                    sem_wait(&S_to_W);
                    route_acquire(&lockN_to_W, &N_to_W, &cntN_to_W);
                    route_acquire(&lockW_to_W, &W_to_W, &cntW_to_W);
                    sem_post(&S_to_W);
                    break;
                case 'E':
                    sem_wait(&S_to_E);
                    route_acquire(&lockE_to_E, &E_to_E, &cntE_to_E);
                    route_acquire(&lockE_to_N, &E_to_N, &cntE_to_N);
                    route_acquire(&lockN_to_E, &N_to_E, &cntN_to_E);
                    route_acquire(&lockN_to_N, &N_to_N, &cntN_to_N);
                    route_acquire(&lockW_to_S, &W_to_S, &cntW_to_S);
                    route_acquire(&lockW_to_W, &W_to_W, &cntW_to_W);
                    sem_post(&S_to_E);
                    break;
            }
            break;
        case 'W':
            switch (dir.dir_target) {
                case 'W':
                    sem_wait(&W_to_W);
                    route_acquire(&lockE_to_N, &E_to_N, &cntE_to_N);
                    route_acquire(&lockN_to_N, &N_to_N, &cntN_to_N);
                    route_acquire(&lockN_to_W, &N_to_W, &cntN_to_W);
                    route_acquire(&lockS_to_E, &S_to_E, &cntS_to_E);
                    route_acquire(&lockS_to_S, &S_to_S, &cntS_to_S);
                    route_acquire(&lockS_to_W, &S_to_W, &cntS_to_W);
                    sem_post(&W_to_W);
                    break;
                case 'N':
                    sem_wait(&W_to_N);
                    route_acquire(&lockE_to_N, &E_to_N, &cntE_to_N);
                    route_acquire(&lockN_to_N, &N_to_N, &cntN_to_N);
                    sem_post(&W_to_N);
                    break;
                case 'S':
                    sem_wait(&W_to_S);
                    route_acquire(&lockE_to_E, &E_to_E, &cntE_to_E);
                    route_acquire(&lockE_to_S, &E_to_S, &cntE_to_S);
                    route_acquire(&lockN_to_N, &N_to_N, &cntN_to_N);
                    route_acquire(&lockN_to_W, &N_to_W, &cntN_to_W);
                    route_acquire(&lockS_to_E, &S_to_E, &cntS_to_E);
                    route_acquire(&lockS_to_S, &S_to_S, &cntS_to_S);
                    sem_post(&W_to_S);
                    break;
            }
            break;
    }

    //The car may now cross the intersection as all of the prerequisite semaphores have been acquired
    //This is handled by the Car function that calls the CrossIntersection function after ArriveIntersection
}

void CrossIntersection(directions dir) {
    printf("Time %.1f: Car %d (%c %c)          crossing\n", get_time(), dir.cid, decode(dir.dir_original), decode(dir.dir_target));
    fflush(stdout);

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
    fflush(stdout);

    //release the movement direction semaphores that were acquired for crossing
    switch (dir.dir_original) {
        case 'N':
            switch (dir.dir_target) {
                case 'N':
                    route_release(&lockW_to_W, &W_to_W, &cntW_to_W);
                    route_release(&lockW_to_S, &W_to_S, &cntW_to_S);
                    route_release(&lockW_to_N, &W_to_N, &cntW_to_N);
                    route_release(&lockS_to_E, &S_to_E, &cntS_to_E);
                    route_release(&lockE_to_N, &E_to_N, &cntE_to_N);
                    route_release(&lockE_to_E, &E_to_E, &cntE_to_E);
                    break;
                case 'E':
                    route_release(&lockE_to_E, &E_to_E, &cntE_to_E);
                    route_release(&lockS_to_E, &S_to_E, &cntS_to_E);
                    break;
                case 'W':
                    route_release(&lockW_to_W, &W_to_W, &cntW_to_W);
                    route_release(&lockW_to_S, &W_to_S, &cntW_to_S);
                    route_release(&lockS_to_W, &S_to_W, &cntS_to_W);
                    route_release(&lockS_to_S, &S_to_S, &cntS_to_S);
                    route_release(&lockE_to_N, &E_to_N, &cntE_to_N);
                    route_release(&lockE_to_E, &E_to_E, &cntE_to_E);
                    break;
            }
            break;
        case 'E':
            switch (dir.dir_target) {
                case 'E':
                    route_release(&lockW_to_S, &W_to_S, &cntW_to_S);
                    route_release(&lockS_to_S, &S_to_S, &cntS_to_S);
                    route_release(&lockS_to_E, &S_to_E, &cntS_to_E);
                    route_release(&lockN_to_W, &N_to_W, &cntN_to_W);
                    route_release(&lockN_to_N, &N_to_N, &cntN_to_N);
                    route_release(&lockN_to_E, &N_to_E, &cntN_to_E);
                    break;
                case 'S':
                    route_release(&lockS_to_S, &S_to_S, &cntS_to_S);
                    route_release(&lockW_to_S, &W_to_S, &cntW_to_S);
                    break;
                case 'N':
                    route_release(&lockW_to_W, &W_to_W, &cntW_to_W);
                    route_release(&lockW_to_N, &W_to_N, &cntW_to_N);
                    route_release(&lockS_to_S, &S_to_S, &cntS_to_S);
                    route_release(&lockS_to_E, &S_to_E, &cntS_to_E);
                    route_release(&lockN_to_W, &N_to_W, &cntN_to_W);
                    route_release(&lockN_to_N, &N_to_N, &cntN_to_N);
                    break;
            }
            break;
        case 'S':
            switch (dir.dir_target) {
                case 'S':
                    route_release(&lockW_to_W, &W_to_W, &cntW_to_W);
                    route_release(&lockW_to_S, &W_to_S, &cntW_to_S);
                    route_release(&lockN_to_W, &N_to_W, &cntN_to_W);
                    route_release(&lockE_to_S, &E_to_S, &cntE_to_S);
                    route_release(&lockE_to_N, &E_to_N, &cntE_to_N);
                    route_release(&lockE_to_E, &E_to_E, &cntE_to_E);
                    break;
                case 'W':
                    route_release(&lockW_to_W, &W_to_W, &cntW_to_W);
                    route_release(&lockN_to_W, &N_to_W, &cntN_to_W);
                    break;
                case 'E':
                    route_release(&lockW_to_W, &W_to_W, &cntW_to_W);
                    route_release(&lockW_to_S, &W_to_S, &cntW_to_S);
                    route_release(&lockN_to_N, &N_to_N, &cntN_to_N);
                    route_release(&lockN_to_E, &N_to_E, &cntN_to_E);
                    route_release(&lockE_to_N, &E_to_N, &cntE_to_N);
                    route_release(&lockE_to_E, &E_to_E, &cntE_to_E);
                    break;
            }
            break;
        case 'W':
            switch (dir.dir_target) {
                case 'W':
                    route_release(&lockS_to_W, &S_to_W, &cntS_to_W);
                    route_release(&lockS_to_S, &S_to_S, &cntS_to_S);
                    route_release(&lockS_to_E, &S_to_E, &cntS_to_E);
                    route_release(&lockN_to_W, &N_to_W, &cntN_to_W);
                    route_release(&lockN_to_N, &N_to_N, &cntN_to_N);
                    route_release(&lockE_to_N, &E_to_N, &cntE_to_N);
                    break;
                case 'N':
                    route_release(&lockN_to_N, &N_to_N, &cntN_to_N);
                    route_release(&lockE_to_N, &E_to_N, &cntE_to_N);
                    break;
                case 'S':
                    route_release(&lockS_to_S, &S_to_S, &cntS_to_S);
                    route_release(&lockS_to_E, &S_to_E, &cntS_to_E);
                    route_release(&lockN_to_W, &N_to_W, &cntN_to_W);
                    route_release(&lockN_to_N, &N_to_N, &cntN_to_N);
                    route_release(&lockE_to_S, &E_to_S, &cntE_to_S);
                    route_release(&lockE_to_E, &E_to_E, &cntE_to_E);
                    break;
            }
            break;
    }
}

void* Car(void* arg) {
    directions dir = *(directions*)arg;
    //wait until the arrival time of the car has been reached
    while (get_time() < dir.arrival_time) {
        usleep(100000); //sleep for 100ms to avoid busy waiting
    }
    ArriveIntersection(dir);
    CrossIntersection(dir);
    ExitIntersection(dir);
    
    return NULL;
}

int main(void) {
    setbuf(stdout, NULL);
    
    //initialize the semaphores
    sem_init(&HOL_N, 0, 1);
    sem_init(&HOL_E, 0, 1);
    sem_init(&HOL_S, 0, 1);
    sem_init(&HOL_W, 0, 1);
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

    sem_init(&lockN_to_N, 0, 1);
    sem_init(&lockN_to_E, 0, 1);
    sem_init(&lockN_to_S, 0, 1);
    sem_init(&lockN_to_W, 0, 1);
    sem_init(&lockE_to_E, 0, 1);
    sem_init(&lockE_to_S, 0, 1);
    sem_init(&lockE_to_W, 0, 1);
    sem_init(&lockE_to_N, 0, 1);
    sem_init(&lockS_to_S, 0, 1);
    sem_init(&lockS_to_W, 0, 1);
    sem_init(&lockS_to_N, 0, 1);
    sem_init(&lockS_to_E, 0, 1);
    sem_init(&lockW_to_W, 0, 1);
    sem_init(&lockW_to_N, 0, 1);
    sem_init(&lockW_to_E, 0, 1);
    sem_init(&lockW_to_S, 0, 1);

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
    printf("All cars have crossed\n");
    fflush(stdout);
    return 0;
}