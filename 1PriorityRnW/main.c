#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int sv = 0;
int c_readers = 0;
int c_writers = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;   // init Mutex
pthread_cond_t cv_reader = PTHREAD_COND_INITIALIZER; // cv For reader
pthread_cond_t cv_writer = PTHREAD_COND_INITIALIZER; // cv for writer

void *readers(void *param)
{
    for (int i = 1; i <= 20; i++)
    {
        usleep(rand() % 100000);

        pthread_mutex_lock(&mutex);
        {
            while (c_writers > 0) // Waiting for writers to finish.
            {
                pthread_cond_wait(&cv_reader, &mutex);
            }
            c_readers++;
            printf("VAR(R): %d\n", sv);
            printf("    |_> Current Readers: %d\n", c_readers);
            printf("    |_> Current Writers: %d\n", c_writers);
            c_readers--;

            if (c_readers == 0)
            {
                pthread_cond_broadcast(&cv_writer);
            }
        }
        pthread_mutex_unlock(&mutex);
    }

    printf("Reader is Exiting\n");
    fflush(stdout);
    return 0;
}

void *writers(void *param)
{
    for (int i = 1000; i <= 1020; i++)
    {
        usleep(rand() % 100000);

        pthread_mutex_lock(&mutex);
        c_writers++;
        while (c_readers > 0)
        {
            pthread_cond_wait(&cv_writer, &mutex);
        }
        sv = i;
        printf("VAR(W): %d\n", sv);
        printf("    |_> Current Readers: %d\n", c_readers);
        printf("    |_> Current Writers: %d\n", c_writers);
        c_writers--;
        pthread_mutex_unlock(&mutex);
        pthread_cond_broadcast(&cv_reader);
        pthread_cond_broadcast(&cv_writer);
    }

    printf("Writer is Exiting\n");
    fflush(stdout);
    return 0;
}

int main(int argc, char *argv[])
{
    int n_readers;
    int n_writers;
    printf("Enter no. of readers: ");
    scanf("%d", &n_readers);
    printf("Enter no. of writers: ");
    scanf("%d", &n_writers);

    // arrays of reader + writer threads.
    pthread_t readers_tid[n_readers];
    pthread_t writers_tid[n_writers];

    for (int i = 0; i < n_readers; i++)
    {
        if (pthread_create(&readers_tid[i], NULL, readers, NULL) != 0)
        {
            fprintf(stderr, "Unable to init Reader%d", i + 1);
            exit(1);
        }
        printf("Reader%d initiated\n", i + 1);
    }

    for (int i = 0; i < n_writers; i++)
    {
        if (pthread_create(&writers_tid[i], NULL, writers, NULL) != 0)
        {
            fprintf(stderr, "Unable to init Writer%d", i + 1);
            exit(1);
        }
        printf("Writer%d initiated\n", i + 1);
    }

    // await readers::>
    for (int i = 0; i < n_readers; i++)
    {
        pthread_join(readers_tid[i], NULL);
    }

    // await writers::>
    for (int i = 0; i < n_writers; i++)
    {
        pthread_join(writers_tid[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cv_reader);
    pthread_cond_destroy(&cv_writer);
    printf("Bye Bye!!");
    return 0;
}