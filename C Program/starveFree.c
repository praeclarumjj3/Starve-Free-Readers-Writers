#include <pthread.h>
#include <stdio.h>
#include <stdatomic.h>
#include <math.h>

typedef volatile struct {
  volatile atomic_int val;
  volatile atomic_flag mut;
} mysem_t; // Semaphore

typedef atomic_flag mymut_t; // Mutex

#define acquire(m) while (atomic_flag_test_and_set(m)) // Mutex Lock
#define release(m) atomic_flag_clear(m) // Mutex Release/Unlock

int wait(mysem_t * s) {
  acquire(&s->mut);                 
  while (atomic_load(&s->val) <= 0);
  atomic_fetch_sub(&s->val, 1);
  release(&s->mut);
  return 0;
} // wait

int signal(mysem_t * s) {
  atomic_fetch_add(&s->val, 1);
  return 0;
} // signal

int init(mysem_t * s, int value){
  atomic_init(&s->val, value);
  return 0;
} // init


mysem_t queueMutex;

volatile mymut_t readerMutex = ATOMIC_FLAG_INIT, writerMutex = ATOMIC_FLAG_INIT; // false; true = locked; false = unlocked

int resource = 1; // Resource

unsigned int readers = 0; // number of readers accessing the resource

void *reader(void *readerIndex)
{

    wait(&queueMutex);

    acquire(&readerMutex);
    if (readers == 0)
        acquire(&writerMutex);
    // increment the number of users.
    readers++;

    signal(&queueMutex);
    release(&readerMutex);

    printf("Reader %d reads resource as %d\n",*((int *)readerIndex),resource);

    acquire(&readerMutex);
    readers--;
    if( readers == 0 )
        release(&writerMutex);
    release(&readerMutex);

    return readerIndex;
}

void *writer(void* writerIndex)
{
    wait(&queueMutex);
    acquire(&writerMutex);
    signal(&queueMutex);

    resource  = pow(2,*((int *)writerIndex));
    printf("Writer %d modifies resource as %d\n",*((int *)writerIndex),resource);

    release(&writerMutex);

    return writerIndex;
}

int main()
{
    // Declaring pthreads or posix threads having 10 readers and 10 writers in our system.

    pthread_t readers[10], writers[10];

    // Initialization of queue semaphore
    init(&queueMutex, 1);

    //numLabels for writers amd readers
    int numLabels[10] = {1,2,3,4,5,6,7,8,9,10};

    /*
        pthread_create: used to create a new thread
        Syntax:        int pthread_create(pthread_t * thread, 
                        const pthread_attr_t * attr, 
                        void * (*start_routine)(void *), 
                        void *arg);
        Parameters:
            thread: pointer to an unsigned integer value that returns the thread id of the thread created.
            attr: pointer to a structure that is used to define thread attributes like detached state, scheduling policy, stack address, etc. Set to NULL for default thread attributes.
            start_routine: pointer to a subroutine that is executed by the thread. The return type and parameter type of the subroutine must be of type void *. The function has a single attribute but if multiple values need to be passed to the function, a struct must be used.
            arg: pointer to void that contains the arguments to the function defined in the earlier argument
    */

    for(int i = 0; i < 10; i++) {
        pthread_create(&readers[i], NULL, reader, (void *)&numLabels[i]);
        pthread_create(&writers[i], NULL, writer, (void *)&numLabels[i]);
    }

    /*
       The pthread_join() function waits for the thread specified by
       thread to terminate.  If that thread has already terminated, then
       pthread_join() returns immediately.  The thread specified by
       thread must be joinable.
    */

    for(int i = 0; i < 10; i++) {
        pthread_join(readers[i], NULL);
        pthread_join(writers[i], NULL);
    }
    
    return 0;
}