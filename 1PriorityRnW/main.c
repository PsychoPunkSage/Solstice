#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int sv = 0;
int c_readers = 0;
int c_writers = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;   // init Mutex
pthread_cond_t cv_reader = PTHREAD_COND_INITIALIZER; // cv For reader
pthread_cond_t cv_writer = PTHREAD_COND_INITIALIZER; // cv for writer

void *readers(void *param) {}

void *writers(void *param) {}

int main(int argc, char *argv[])
{
    pthread_t tid1, tid2; // init 2 threads
    int n_readers;
    int n_writers;
    printf("Enter no. of readers: ");
    scanf("%d", &n_readers);
    printf("Enter no. of writers: ");
    scanf("%d", &n_writers);

    for (int i = 0; i < n_readers; i++)
    {
        if (pthread_create(&tid1, NULL, readers, NULL) != 0)
        {
            fprintf(stderr, "Unable to init Reader");
            exit(1);
        }
        printf("Reader%d initiated\n", i + 1);
    }

    for (int i = 0; i < n_writers; i++)
    {
        if (pthread_create(&tid2, NULL, writers, NULL) != 0)
        {
            fprintf(stderr, "Unable to init Writer");
            exit(1);
        }
        printf("Writer%d initiated\n", i + 1);
    }

    pthread_join(tid1, NULL); // wait for readers
    pthread_join(tid2, NULL); // wait for writers
    printf("Bye Bye!!");
}