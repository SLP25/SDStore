/**
 * @file pqueue.h
 * 
 * @brief File defining the API used to communicate with a priority queue for requests
 * 
 */

#ifndef _PQUEUE_H_

/**
 * @brief Include guard
 * 
 */
#define _PQUEUE_H_

/**
 * @brief The maximum number of elements in the queue at any given time
 * 
 */
#define QUEUE_SIZE 100000

#include "request.h"
#include "utils.h"

typedef struct pqueue *PQueue;

PQueue createPQueue();
bool isEmpty(PQueue);
bool isFull(PQueue);
bool push(PQueue, Request);
Request peek(PQueue);
Request pop(PQueue);
void freePQueue(PQueue);

#endif // _PQUEUE_H_