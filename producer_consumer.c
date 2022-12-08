/* this is a simple tread pool implimentation based on
   https://youtu.be/l6zkaJFjUbM for beginners

   this can be further modified for better usage
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

#define THREAD_NUM 8
#define BUFFER_SIZE 10

sem_t semBufferEmpty;
sem_t semBufferFull;
pthread_mutex_t mutexBuffer;

/* common buffer can be a shared memoery in pratical applications */
int buffer[BUFFER_SIZE];
int count = 0;

void *producer(void *args)
{
    while (1)
    {
        /* Produce -- simulate */
        int x = rand() % 100;
        sleep(1);

        /* Add to the buffer only if there is space */
        /* Decrement the empty status counter */
        sem_wait(&semBufferEmpty);
        pthread_mutex_lock(&mutexBuffer);
        buffer[count] = x;
        count++;
        pthread_mutex_unlock(&mutexBuffer);
        /* Increment the full status counter */
        sem_post(&semBufferFull);
    }
}

void *consumer(void *args)
{
    while (1)
    {
        int y;

        /* Remove from the buffer if buffer is not empty */
        /* Decrement the full status counter */
        sem_wait(&semBufferFull);
        pthread_mutex_lock(&mutexBuffer);
        y = buffer[count - 1];
        count--;
        pthread_mutex_unlock(&mutexBuffer);
        /* Increment the empty status counter */
        sem_post(&semBufferEmpty);

        /* Consume- -- simulate */
        printf("Got %d\n", y);
        sleep(1);
    }
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    pthread_t th[THREAD_NUM];
    pthread_mutex_init(&mutexBuffer, NULL);
    sem_init(&semBufferEmpty, 0, BUFFER_SIZE);
    sem_init(&semBufferFull, 0, 0);
    int i;
    for (i = 0; i < THREAD_NUM; i++)
    {
        /* creating THREAD_NUM-1 producers and just one consumer */
        if (i > 0)
        {
            if (pthread_create(&th[i], NULL, &producer, NULL) != 0)
            {
                perror("Failed to create thread");
            }
        }
        else
        {
            if (pthread_create(&th[i], NULL, &consumer, NULL) != 0)
            {
                perror("Failed to create thread");
            }
        }
    }

    for (i = 0; i < THREAD_NUM; i++)
    {
        if (pthread_join(th[i], NULL) != 0)
        {
            perror("Failed to join thread");
        }
    }

    sem_destroy(&semBufferEmpty);
    sem_destroy(&semBufferFull);
    pthread_mutex_destroy(&mutexBuffer);
    
    return 0;
}