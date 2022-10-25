#include "sem.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/*
 * Semaphore implementation for the synchronization of POSIX threads.
 *
 * This module implements counting P/V semaphores suitable for the
 * synchronization of POSIX threads. POSIX mutexes and condition variables
 * shall be utilized to implement the semaphore operations.
 */

/* Opaque type of a semaphore.
 * ...you need to figure out the contents of struct SEM yourself!
 */
typedef struct SEM
{

    int semCounter;
    pthread_mutex_t lock;
    pthread_cond_t condition; // Condition variable can prevent continuously polling on mutex lock.

} SEM;

/* Creates a new semaphore.
 *
 * This function creates a new semaphore. If an error occurs during the
 * initialization, the implementation shall free all resources already
 * allocated so far.
 *
 * Parameters:
 *
 * initVal      the initial value of the semaphore
 *
 * Returns:
 *
 * handle for the created semaphore, or NULL if an error occured.
 */
SEM *sem_init(int initVal)
{
    SEM *semaphore = malloc(sizeof(struct SEM)); // Allocating memory space

    if (!semaphore) // If malloc fail to allocate required space, it returns NULL.
    {
        perror("\n Space allocation failed\n");
        free(semaphore);
    }

    if (pthread_mutex_init(&semaphore->lock, NULL) != 0) // If failed init mutex lock
    {
        perror("\n Mutex init failed\n");
        free(semaphore);
    }

    if (pthread_cond_init(&semaphore->condition, NULL) != 0)
    {
        perror("\n Conditon init faield\n");
        free(semaphore);
    }

    semaphore->semCounter = initVal; // Initialize semCounter with initValue, numbers of possible threads operating at the same time.

    return semaphore;
}

/* Destroys a semaphore and frees all associated resources.
 *
 * Parameters:
 *
 * sem           handle of the semaphore to destroy
 *
 * Returns:
 *
 * 0 on success, negative value on error.
 *
 * In case of an error, not all resources may have been freed, but
 * nevertheless the semaphore handle must not be used any more.
 */
int sem_del(SEM *sem)
{
    if (pthread_mutex_destroy(&sem->lock) != 0)
    {
        free(sem);
        return 1; // "Error code" for failed desctruction of mutex
    }

    if (pthread_cond_destroy(&sem->condition) != 0)
    {
        free(sem);
        return 2; // "Error code" for failed desctruction of condtition
    }

    free(sem);
    return 0;
}

/* P (wait) operation.
 *
 * Attempts to decrement the semaphore value by 1. If the semaphore value
 * is 0, the operation blocks until a V operation increments the value and
 * the P operation succeeds.
 *
 * Parameters:
 *
 * sem           handle of the semaphore to decrement
 */
void P(SEM *sem)
{

    pthread_mutex_lock(&sem->lock); // Locking by semaphores mutex lock.
    while (sem->semCounter < 1)     // Waiting condtion
    {
        pthread_cond_wait(&sem->condition, &sem->lock); // Waiting or listening on a signal from another thread, which shares condition variable and lock.
    }
    sem->semCounter -= 1;
    pthread_mutex_unlock(&sem->lock);
}

/* V (signal) operation.
 *
 * Increments the semaphore value by 1 and notifies P operations that are
 * blocked on the semaphore of the change.
 *
 * Parameters:
 *
 * sem           handle of the semaphore to increment
 */
void V(SEM *sem)
{
    pthread_mutex_lock(&sem->lock);
    sem->semCounter += 1;

    pthread_cond_signal(&sem->condition); // Signals to possible waiting threads. Result; wakeing up threads, to check conditions again.
    pthread_mutex_unlock(&sem->lock);
}

/*
Code inspiration from youtube: "Condition Variables in C" By CodeVault
https://www.youtube.com/watch?v=0sVGnxg6Z3k&ab_channel=CodeVault
**/
