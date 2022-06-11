/**
 * @file pqueue.c
 * 
 * @brief File implementing a priority queue
 * 
 */

#include <stdlib.h>

#include "pqueue.h"
#include "request.h"
#include "utils.h"

/**
 * @brief Swaps two elements.
 * 
 * @note Allocates a variable named 'temp'
 * 
 */
#define SWAP(a,b) Request temp = a; \
                  a = b; \
                  b = temp

/**
 * @brief The parent of a node in the i-th position
 * 
 */
#define PARENT(i) (i - 1) / 2

/**
 * @brief The left child of a node in the i-th position
 * 
 */
#define LEFT_CHILD(i) 2 * i + 1

/**
 * @brief The right child of a node in the i-th position
 * 
 */
#define RIGHT_CHILD(i) 2 * i + 2

/**
 * @brief Represents a priority queue of #Request sorted by their priority + time of arrival (FIFO)
 * 
 */
struct pqueue {
    int numberElements; ///< The number of elements in the queue
    Request requests[QUEUE_SIZE]; ///< The heap of #Request corresponding to the queue
};

/**
 * @brief Creates an empty priority queue.
 * 
 * It is m'alloced and must be freed after being used
 * 
 * @return PQueue An empty #PQueue
 */
PQueue createPQueue() {
    PQueue queue = malloc(sizeof(struct pqueue));
    if(queue) {
        queue->numberElements = 0;
    }
    return queue;
}

/**
 * @brief Performs a bubble up operation in order to maintain the heap invariant
 * 
 * @param pqueue    The #PQueue
 * @param position  The position in the heap of the current element
 */
void bubbleUp(PQueue pqueue, int position) {
    while (position > 0 && pqueue->requests[PARENT(position)] < pqueue->requests[position]) {
 
        SWAP(pqueue->requests[PARENT(position)], pqueue->requests[position]);
 
        position = PARENT(position);
    }
}

/**
 * @brief Performs a bubble down operation in order to maintain the heap invariant
 * 
 * @param pqueue    The #PQueue
 * @param position  The position in the heap of the current element
 */
void bubbleDown(PQueue pqueue, int position) {
    int maxIndex = position;
 
    int l = LEFT_CHILD(position);
 
    if (l < pqueue->numberElements && pqueue->requests[l] > pqueue->requests[maxIndex]) {
        maxIndex = l;
    }
 
    int r = RIGHT_CHILD(position);
 
    if (r < pqueue->numberElements && pqueue->requests[r] > pqueue->requests[maxIndex]) {
        maxIndex = r;
    }
 
    if (position != maxIndex) {
        SWAP(pqueue->requests[position], pqueue->requests[maxIndex]);
        bubbleDown(pqueue, maxIndex);
    }
}

/**
 * @brief Checks if the given #PQueue is empty
 * 
 * @param pqueue The given #PQueue
 * 
 * @return true  If it is empty
 * @return false If it is not empty
 */
bool isEmpty(PQueue pqueue) {
    return pqueue->numberElements == 0;
}

/**
 * @brief Checks if the given #PQueue is full
 * 
 * @param pqueue The given #PQueue
 * 
 * @return true  If it is empty
 * @return false If it is not empty
 */
bool isFull(PQueue pqueue) {
    return pqueue->numberElements == QUEUE_SIZE;
}

/**
 * @brief Gets the top of the queue without poping
 * 
 * @param pqueue The given #PQueue
 * 
 * @return Request The top of the queue
 * @return NULL If the queue is empty
 */
Request peek(PQueue pqueue) {
    if(pqueue->numberElements == 0)
        return NULL;

    return pqueue->requests[0];
}

/**
 * @brief Pushes a new element to the priority queue
 * 
 * @param pqueue  The given priority queue
 * @param request The element to push
 * 
 * @return true   If the insertion was successful
 * @return false  If the insertion failed
 */
bool push(PQueue pqueue, Request request) {
    if(pqueue->numberElements == QUEUE_SIZE) {
        return false;
    }

    pqueue->requests[pqueue->numberElements] = request;
    pqueue->numberElements++;
    bubbleUp(pqueue, pqueue->numberElements - 1);

    return true;
}

/**
 * @brief Pops the top of a #PQueue
 * 
 * @param pqueue The given #PQueue
 * 
 * @return Request The request with the highest priority
 * @return NULL    If the #PQueue is empty 
 */
Request pop(PQueue pqueue) {
    if(pqueue->numberElements == 0)
        return NULL;

    Request result = pqueue->requests[0];

    pqueue->numberElements--;
    pqueue->requests[0] = pqueue->requests[pqueue->numberElements];

    bubbleDown(pqueue, 0);

    return result;
}

/**
 * @brief Frees the allocated memory to the #PQueue
 * 
 * @param pqueue The queue to free
 */
void freePQueue(PQueue pqueue) {
    for(int i = 0; i < pqueue->numberElements; i++) {
        if (pqueue->requests[i])
            freeRequest(pqueue->requests[i]);
    }
    free(pqueue);
}