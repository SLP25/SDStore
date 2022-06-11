/**
 * @file jobManager.h
 * 
 * @brief File declaring the API used for the handling of the instances of a certain sub program within
 * the server
 * 
 */

#ifndef _LIST_H_

/**
 * @brief Include guard
 */
#define _LIST_H_

#include "config.h"
#include "request.h"
#include "utils.h"

typedef struct freePositions{
    int pos;
    struct freePositions *next;
}*FreePositions,freepositions;


typedef struct requestsList{
    Request* requests;
    int numberInArray;
    int maxInArray;
    FreePositions positions;
}*RequestsList,requestslist;

int getNumberInArray(RequestsList);
void insertFreePositions(FreePositions*,int);
bool popFreePositions(FreePositions*,int*);
void insertRequest(RequestsList,Request);
void nukeFreePositions(FreePositions);
void removeRequest(RequestsList,int);
RequestsList initRequestList();
void freeRequestList(RequestsList);


#endif // _JOB_MANAGER_H_