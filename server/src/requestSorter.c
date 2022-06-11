/**
 * @file requestSorter.c
 *
 * @brief File implementing the API used to sort incoming requests
 * 
 */


#include <stdlib.h>
#include <unistd.h>

#include "config.h"
#include "logging.h"
#include "pqueue.h"
#include "request.h"
#include "requestSorter.h"
#include "utils.h"

/**
 * @brief A type which allows managing the priority of #Requests for different programs
 * 
 */
struct requestSorter {
    PQueue* queues; ///< The array of all the priority queues being used
    int numberOfQueues; ///< The number of priority queues being used
};

/**
 * @brief Creates a new #RequestSorter
 * 
 * @param programCount   The number of different programs the server will run
 *
 * @return RequestSorter The created #RequestSorter
 * @return NULL          In case of error. The error will be logged
 * 
 */
RequestSorter newRequestSorter(int programCount) {
    RequestSorter sorter = malloc(sizeof(struct requestSorter));

    if(sorter) {
        sorter->numberOfQueues = programCount;
        sorter->queues = malloc(sizeof(PQueue) * programCount);

        if(sorter->queues) {
            for(int i = 0; i < programCount; i++) {
                sorter->queues[i] = createPQueue();
            }
        } else {
            printMessage(STDOUT_FILENO,REQUESTSORTERFAILEDALLOCQUEUES);
            free(sorter);
            sorter = NULL;
        }
    } else {
        printMessage(STDOUT_FILENO,REQUESTSORTERFAILEDALLOC);
   }

    return sorter;
}

/**
 * @brief Frees all the memory allocated to a #RequestSorter
 * 
 * @param sorter The #RequestSorter to free
 */
void deleteRequestSolver(RequestSorter sorter) {
    for(int i = 0; i < sorter->numberOfQueues; i++) {
        freePQueue(sorter->queues[i]);
    }
    
    free(sorter->queues);
    free(sorter);
}

/**
 * @brief Checks if there are any pending #Request in the sorter
 * 
 * @param sorter The given #RequestSorter
 * 
 * @return true  If there are pending requests
 * @return false If there aren't pending requests
 */
bool notEmpty(RequestSorter sorter) {
    for(int i = 0; i < sorter->numberOfQueues; i++) {
        if(!isEmpty(sorter->queues[i]))
            return true;
    }

    return false;
}


/**
 * @brief Pushes a new #Request to the priority queue of the given program
 * 
 * @param sorter        The #RequestSorter
 * @param request       The #Request to add
 * @param programId     The id of the program
 * 
 * @return true         If the push was successful
 * @return false        If the push failed (no space left, #Request dropped)
 */
bool enqueue(RequestSorter sorter, Request request, Config config) {
    int done[config->programCount];

    for(int i = 0; i < config->programCount; i++)
        done[i] = 0;

    for(int i = 0; i < request->operationCount; i++) {
        done[getProgramId(config, request->operations[i])]++;
    }

    for(int i = 0; i < config->programCount; i++) {
        if(!done[i]) {
            if(isFull(sorter->queues[i]))
                return false;
        }
    }

    for(int i = 0; i < config->programCount; i++) {
        if(done[i]) {
            push(sorter->queues[i], request);
        }
    }

    return true;
}

/**
 * @brief Gets the next #Request to be executed
 * 
 * @param sorter The given #RequestSorter
 * @param programCount The number of programs
 * @param availableInstances The array of available instances
 * 
 * @return Request The next #Request to execute
 * @return NULL    If there is no #Request to execute 
 */
Request nextInLine(RequestSorter sorter, Config config, int availableInstances[]) {

    int blocked[config->programCount];
    for(int i = 0; i < config->programCount; i++) {

        if(availableInstances[i]) {
            Request r = peek(sorter->queues[i]);

            if(r && getOperationCount(r, getProgramName(config, i)) > availableInstances[i])
                blocked[i] = 1;
            else    
                blocked[i] = 0;
        } else {
            blocked[i] = 1;
        }
    }


    Request tops[config->programCount];

    for(int i = 0; i < config->programCount; i++) {
       tops[i] = peek(sorter->queues[i]);
    }


    Request result = NULL;

    for(int i = 0; i < config->programCount; i++) {
        if(!blocked[i] && tops[i]) {
            bool approved = true;
            for(int j = 0; j < tops[i]->operationCount && approved; j++) {
                int opId=getProgramId(config, tops[i]->operations[j]);
                if(peek(sorter->queues[opId]) != tops[i] || blocked[opId])
                    approved = false;
            }

            if(approved && (!result || compareRequests(result, tops[i]) > 0))
                result = tops[i];
        }
    }

    if(result) {
        for(int i = 0; i < result->operationCount; i++) {
            int id = getProgramId(config, result->operations[i]);
            if(!blocked[id]) {
                //Prevent duplicated pops
                blocked[id] = 1;
                result = pop(sorter->queues[id]);
            }
        }
    }

    return result;
}