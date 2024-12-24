#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int sv = 0;
int readers = 0;
int writers = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;   // init Mutex
pthread_cond_t cv_reader = PTHREAD_COND_INITIALIZER; // cv For reader
pthread_cond_t cv_writer = PTHREAD_COND_INITIALIZER; // cv for writer
