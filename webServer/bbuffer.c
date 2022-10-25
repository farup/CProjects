#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include "bbuffer.h"
#include "sem.h"

/*
 * Bounded Buffer implementation to manage int values that supports multiple
 * readers and writers.
 *
 * The bbuffer module uses the sem module API to synchronize concurrent access
 * of readers and writers to the bounded buffer.
 */

/* Opaque type of a Bounded Buffer.
 * ...you need to figure out the contents of struct BNDBUF yourself
 */

typedef struct BNDBUF
{
    int *values;
    int head, tail, num_entries, size;

    SEM *semEmpty; // Stores the number of empty slots in the buffer.
    SEM *semFull;
} BNDBUF;

/* Creates a new Bounded Buffer.
 *
 * This function creates a new bounded buffer and all the helper data
 * structures required by the buffer, including semaphores for
 * synchronization. If an error occurs during the initialization the
 * implementation shall free all resources already allocated by then.
 *
 * Parameters:
 *
 * size     The number of integers that can be stored in the bounded buffer.
 *
 * Returns:
 *
 * handle for the created bounded buffer, or NULL if an error occured.
 */
BNDBUF *bb_init(unsigned int size)
{
    BNDBUF *bb = malloc(sizeof(struct BNDBUF));
    if (!bb)
    {
        printf("Error");
        return NULL;
    }
    bb->size = size;
    bb->values = malloc(sizeof(int) * size);

    bb->head = 0;
    bb->tail = 0;

    if (!bb->values)
    {
        free(bb);
        return NULL;
    }

    bb->semEmpty = sem_init(size); // Init with no empty slots
    bb->semFull = sem_init(0);
    return bb;
}

/* Destroys a Bounded Buffer.
 *
 * All resources associated with the bounded buffer are released.
 *
 * Parameters:
 *
 * bb       Handle of the bounded buffer that shall be freed.
 */
void bb_del(BNDBUF *bb)
{
    sem_del(bb->semEmpty);
    sem_del(bb->semFull);
    free(bb->values);
    free(bb);
}

/* Retrieve an element from the bounded buffer.
 *
 * This function removes an element from the bounded buffer. If the bounded
 * buffer is empty, the function blocks until an element is added to the
 * buffer.
 *
 * Parameters:
 *
 * bb         Handle of the bounded buffer.
 *
 * Returns:
 *
 * the int element
 */

int bb_get(BNDBUF *bb)
{
    int x;
    P(bb->semFull); // Want to wait unitl atleast one element in the buffer.

    x = bb->values[bb->tail];
    bb->tail = (bb->tail + 1) % bb->size;

    V(bb->semEmpty); // Increament empty slots, because we have taken one
    // printf("Retrieved an element %i\n", x);
    return x;
}

/* Add an element to the bounded buffer.
 *
 * This function adds an element to the bounded buffer. If the bounded
 * buffer is full, the function blocks until an element is removed from
 * the buffer.
 *
 * Parameters:
 *
 * bb     Handle of the bounded buffer.
 * fd     Value that shall be added to the buffer.
 *
 * Returns:
 *
 * the int element
 */
void bb_add(BNDBUF *bb, int fd)
{

    P(bb->semEmpty);           //  Decrement the semaphore for the emptyslots, or wait if no empty slots.
    bb->values[bb->head] = fd; //
    bb->head = (bb->head + 1) % bb->size;
    // printf("After bb_add head is %i\n", bb->head);
    // printf("After bb_add tail is %i\n", bb->tail);
    V(bb->semFull); //  Incremeant the semaphore for the semFull, since we have added an element.
                    // printf("Added an element %i\n", fd);
}

/*
https://www.youtube.com/watch?v=l6zkaJFjUbM&t=937s&ab_channel=CodeVault**/
