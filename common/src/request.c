/**
 * @file request.c
 * 
 * @brief File implementing the #Request type.
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "request.h"
#include "logging.h"
#include "config.h"

/**
 * @brief The default size of a string in a #Request
 * 
 */
#define STR_SIZE 256

/**
 * @brief Gets the number of times the given transfomation occurs in a #Request
 * 
 * @param r The given #Request
 * @param op The given transformation
 * 
 * @return int The number of times the transformation occurs in the #Request
 */
int getOperationCount(Request r, char* op) {
    int res = 0;
    
    for(int i = 0; i < r->operationCount; i++) {
        if(strcmp(r->operations[i], op) == 0)
            ++res;
    }

    return res;
}

/**
 * @brief Compares two #Request by their priority
 * 
 * One #Request is 'greater than' another one if it has a higher priority or, if they
 * have the same priority, it has a lower time of arrival
 * 
 * @param r1 The first #Request
 * @param r2 The second #Request
 * 
 * @return >0 If the first #Request has higher priority
 * @return =0 If they have the same priority (should never happen)
 * @return <0 If the second #Request has higher priority
 * 
 */
int compareRequests(Request r1, Request r2) {
    if(r1->priority == r2->priority) 
        return r1->timeOfArrival - r2->timeOfArrival;

    return r1->priority - r2 -> priority;
}

/**
 * @brief Reads a #Request from a #PipeReader
 * 
 * @param pr The given #PipeReader
 * @param r The #Request to write to
 * 
 * @return true If the reading was successful 
 * @return false If the reading failed
 */
bool readRequest(PipeReader pr, Request r) {
    if (!readBytes(pr, sizeof(r->type), &r->type))
        return false;
    
    switch (r->type) {

        case STATUS:
            r->sender = malloc(STR_SIZE * sizeof(char));
            readString(pr, r->sender, STR_SIZE);
            return true;
            
        case PROCESS_FILE:

            r->sender = malloc(STR_SIZE * sizeof(char));
            readString(pr, r->sender, STR_SIZE);
            readBytes(pr, sizeof(r->timeOfArrival), &r->timeOfArrival);
            readBytes(pr, sizeof(r->senderFD), &r->senderFD);
            readBytes(pr, sizeof(r->priority), &r->priority);

            r->inputFile = malloc(STR_SIZE * sizeof(char));
            r->outputFile = malloc(STR_SIZE * sizeof(char));

            readString(pr, r->inputFile, STR_SIZE);
            readString(pr, r->outputFile, STR_SIZE);


            readBytes(pr, sizeof(r->operationCount), &r->operationCount);
            r->operations = malloc(r->operationCount * sizeof(char*));

            for (int i = 0; i < r->operationCount; i++) {
                r->operations[i] = malloc(STR_SIZE * sizeof(char));
                readString(pr, r->operations[i], STR_SIZE);
            }

            return true;

        default:
            printMessage(STDERR_FILENO, UNKNOWNREQUESTTYPE);
            return false;
    }
}

/**
 * @brief Writes the given #Request to a #PipeWritter
 * 
 * @param pw The given #PipeWritter
 * @param r The given #Request
 * 
 * @return true If the writting was successful
 * @return false If the writting failed
 */
bool writeRequest(PipeWritter pw, Request r) {
    switch (r->type) {
        case STATUS:
            writeBytes(pw, sizeof(r->type), &r->type);
            writeString(pw, r->sender);
            break;

        case PROCESS_FILE:
            writeBytes(pw, sizeof(r->type), &r->type);
            writeString(pw, r->sender);
            writeBytes(pw, sizeof(r->timeOfArrival), &r->timeOfArrival);
            writeBytes(pw, sizeof(r->senderFD), &r->senderFD);
            writeBytes(pw, sizeof(r->priority), &r->priority);
        
            writeString(pw, r->inputFile);
            writeString(pw, r->outputFile);

            writeBytes(pw, sizeof(r->operationCount), &r->operationCount);
            
            for (int i = 0; i < r->operationCount; i++)
                writeString(pw, r->operations[i]);
            break;

        default:
            printMessage(STDERR_FILENO, UNKNOWNREQUESTTYPE);
            break;
    }
    
    return true;
}

/**
 * @brief Converts a #Request to a string
 * 
 * @param request The given #Request
 * 
 * @return char* The string form of the #Request
 */
char* requestToString(Request request) {
    
    //Number of predefined characters in the string 'proc-file', spaces, etc
    int length = 17 + strlen(request->inputFile) + strlen(request->outputFile);
    
    for(int i = 0; i < request->operationCount; i++)
        length += strlen(request->operations[i]) + 4;

    char* result = malloc(sizeof(char) * length);
    result[0] = '\0';
    sprintf(result, "PRIORITY: %d %s -> ", request->priority, request->inputFile);

    for(int i = 0; i < request->operationCount; i++) {
        strcat(result, request->operations[i]);

        strcat(result, " -> ");
    }
    strcat(result,request->outputFile);
    strcat(result, "\n");
    return result;
}

/**
 * @brief Gets the string to send to the client regarding the status of the server
 * 
 * @param config The server #Config
 * @param availableInstances The available instances of each transformation
 * @param requests The list of all #Request to have arrived at the server
 * @param requestCount The number of #Request to have arrived at the server
 * 
 * @return char* The status string
 */
char* getRequestStatus(Config config, int availableInstances[], Request* requests, int requestCount){
    int capacity = STR_SIZE;
    char *a=malloc(capacity);
    *a = '\0';
    int length = 0;
    char temp[307]; //307 is the maximum length output for snprintf "transform..." 

    //Print the list of active requests in the server
    for(int i = 0; i < requestCount; i++) {
        //Ignore requests set to NULL (already left the server)
        if(requests[i]) {
            //Add the task prefix + numbering
            snprintf(temp, STR_SIZE, "%s task #%d:", requests[i]->running?"Running":"Pending",requests[i]->timeOfArrival);
            int len = strlen(temp);
            //String capacity is not enough
            if(length + len >= capacity) {
                capacity *= 2;
                a = realloc(a, capacity);
            }
            strcat(a, temp);
            length += len;

            //Add the string corresponding to the request
            char* op = requestToString(requests[i]);

            len = strlen(op);
            //String capacity is not enough
            while(length + len >= capacity) {
                capacity *= 2;
                a = realloc(a, capacity);
            }

            strcat(a, op);
            length += len;
            free(op);
        }
    }
    //Add the available / max instances of all transformations
    for(int i = 0; i < config->programCount; i++) {

        snprintf(temp, 306, "transform %s: %d/%d (running/max)\n", config->programs[i], config->instances[i] - availableInstances[i], config->instances[i]);
        int len = strlen(temp);
        if(length + len >= capacity) {
            capacity *= 2;
            a = realloc(a, capacity);
        }
        strcat(a, temp);
        length += len;
    }
    return a;
}

/**
 * @brief Frees the memory allocated to a #Request
 * 
 * @param r The given #Request
 */
void freeRequest(Request r) {
    switch (r->type) {
        case STATUS:
            free(r->sender);
            break;

        case PROCESS_FILE:
            free(r->inputFile);
            free(r->outputFile);
            free(r->sender);
            for(int i = 0; i < r->operationCount; i++)
                free(r->operations[i]);
            free(r->operations);
            break;
        default:
            break;
    }
    free(r);
}

/**
 * @brief Frees the memory allocated inside a #Request
 * 
 * @param r The given #Request
 */
void freeRequestContent(Request r) {
    switch (r->type) {
        case STATUS:
            free(r->sender);
            break;

        case PROCESS_FILE:
            free(r->inputFile);
            free(r->outputFile);
            free(r->sender);
            for(int i = 0; i < r->operationCount; i++)
                free(r->operations[i]);
            free(r->operations);
            break;
        default:
            break;
    }
}