/* this is a simple tread pool implimentation based on
   https://youtu.be/_n2hE2gyPxU

   this can be further modified for better task que implimentation based on
   https://people.clarkson.edu/~jmatthew/cs644.archive/cs644.fa2001/proj/locksmith/code/ExampleTest/threadpool.c
*/

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define WORKER_THREAD_COUNT   4
#define TASK_QUEUE_SIZE       256

pthread_mutex_t mutexTaskQueue;
pthread_cond_t condTaskQueue;

typedef void (*taskFuncPtr)(int,int);

typedef struct Task 
{
    taskFuncPtr taskFunction;
    int arg1, arg2;
} Task;

Task taskQueue[TASK_QUEUE_SIZE];
int taskCount = 0;


void sum(int a, int b) {
   /* sleep given to simulate processing load */
    usleep(50000);
    int sum = a + b;
    printf("Sum of %d and %d is %d\n", a, b, sum);
}

void product(int a, int b) {
   /* sleep given to simulate processing load */
    usleep(50000);
    int prod = a * b;
    printf("Product of %d and %d is %d\n", a, b, prod);
}

/* create task queue by pushing tasks to a task queue array */
void submitTask(Task task) 
{
    pthread_mutex_lock(&mutexTaskQueue);
    taskQueue[taskCount] = task;
    taskCount++;
    pthread_mutex_unlock(&mutexTaskQueue);
    pthread_cond_signal(&condTaskQueue);
}

/* retrive task from task queue */
Task retriveTask()
{
   Task task;
   pthread_mutex_lock(&mutexTaskQueue);
   while ( taskCount == 0 )
   {
      pthread_cond_wait(&condTaskQueue, &mutexTaskQueue);
      /* here the conditional variable
         temporarly unlocks the mutex, so other waiting threads can work, then
         it again locks the mutex */
   }
   task = taskQueue[0];
   for(int i = 0; i < taskCount - 1; i++ )
   {
      taskQueue[i] = taskQueue[i + 1];
   }
   taskCount--;
   pthread_mutex_unlock(&mutexTaskQueue);

   return task;
}

void* workerThread(void* args) 
{
    while (1) 
    {
        Task task = retriveTask();
        task.taskFunction(task.arg1, task.arg2);
    }
}

int main() 
{

    pthread_t thread_id[WORKER_THREAD_COUNT];
    pthread_mutex_init(&mutexTaskQueue, NULL);
    pthread_cond_init(&condTaskQueue, NULL);

   /* creation of thread pool of fixed number of threads */
    for (int i = 0; i < WORKER_THREAD_COUNT; i++) 
    {
        if (pthread_create(&thread_id[i], NULL, &workerThread, NULL) != 0) 
        {
            perror("Failed to create worker thread");
        }
    }

    srand(time(NULL));
    for (int k = 0; k < 100; k++) {
        Task t = {
            .taskFunction = k % 2 == 0 ? &sum : &product,
            .arg1 = rand() % 100,
            .arg2 = rand() % 100
        };
        submitTask(t);
    }


    for (int j = 0; j < WORKER_THREAD_COUNT; j++) 
    {
        if (pthread_join(thread_id[j], NULL) != 0) 
        {
            perror("Failed to join worker thread");
        }
    }

    pthread_mutex_destroy(&mutexTaskQueue);
    pthread_cond_destroy(&condTaskQueue);

    return 0;
}