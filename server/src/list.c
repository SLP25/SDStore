
#include "list.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Get the number of elements in the list
 * 
 * @param r the list to see in
 * @return the number of elements in the list
 */
int getNumberInArray(RequestsList r){
    return r->numberInArray;
}


/**
 * @brief inserts a new freePosition to the list
 * 
 * @param old the old head of the freePositions list
 * @param new the new value to put at the head of the list
 */
void insertFreePositions(FreePositions *old,int new){
    FreePositions newTop = malloc(sizeof(freepositions));
    newTop->pos=new;
    FreePositions oldp=*old;
    newTop->next=oldp;
    *old=newTop;
}
/**
 * @brief gets the latest freed position in the list if it exists
 * 
 * @param old the old head of the freed positions list
 * @param pos where to put the value in the head of the list if it exists
 * @return true if it had a value in the head of the list
 * @return false if there were no values in the freed list
 */
bool popFreePositions(FreePositions *old,int *pos){
    if (!(*old)) return false;
    *pos=(*old)->pos;
    FreePositions temp= *old;
    (*old)=(*old)->next;
    free(temp);
    return true;
}
/**
 * @brief inserts a request into the dynamic list of requests
 * 
 * @param rl the list of requests
 * @param r the requests to insert in the list 
 */
void insertRequest(RequestsList rl,Request r){
    int newPos;
    bool success=popFreePositions(&(rl->positions),&newPos); //had positions free
    if (!success){
        if (rl->numberInArray>=rl->maxInArray){//doesn't have space
            rl->maxInArray *= 2;
            rl->requests = realloc(rl->requests, sizeof(Request) * rl->maxInArray);
        }
        newPos=rl->numberInArray++;
    }
    r->timeOfArrival=newPos;
    rl->requests[newPos]=r;
}
/**
 * @brief Removes a request from the dynamic list of requests in a given position
 * 
 * @param rl the list of requests
 * @param pos the request to remove
 */
void removeRequest(RequestsList rl,int pos){
    freeRequest(rl->requests[pos]);
    rl->requests[pos]=NULL;
    insertFreePositions(&(rl->positions),pos);
}

/**
 * @brief destroys a FreePositions list
 * 
 * @param b the FreePosition to destroy
 */
void nukeFreePositions(FreePositions b){
    if (b){
        nukeFreePositions(b->next);
        free(b);
    }

}

/**
 * @brief creates a new Empty Request list
 * 
 * @return a new RequestsList 
 */
RequestsList initRequestList(){
    RequestsList r=malloc(sizeof(requestslist));
    r->maxInArray =128;
    r->requests =malloc(sizeof(Request)*r->maxInArray);
    r->numberInArray =0;
    r->positions = NULL ;
    return r;
}
/**
 * @brief frees the list of requests
 * 
 * @warning this function does not free the Requests inside the list that up to the developer to do
 * 
 * @param r the list of requests to be freed
 */
void freeRequestList(RequestsList r){
    free(r->requests);
    nukeFreePositions(r->positions);
    free(r);
}
