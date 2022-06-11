/**
 * @file request.h
 * 
 * @brief File defining the common API to use the #Request type, which is the type used in the client-server communication
 */

#ifndef _REQUEST_H_

/**
 * @brief Include guard
 */
#define _REQUEST_H_


#include "config.h"
#include "pipeWrapper.h"
#include "utils.h"

/**
 * @brief Number of different programs supported
 * 
 */
#define PROGRAM_COUNT 7

/**
 * @brief The different types of requests a client can send to the server
 * 
 */
typedef enum requestType {
    STATUS,      ///< Requesting the server status
    PROCESS_FILE ///< Requesting to process a file
} RequestType;

/**
 * @brief The string corresponding to the ::STATUS command
 * 
 */
#define STATUS_COMMAND "status"

/**
 * @brief The string corresponding to the ::PROC_FILE command
 * 
 */
#define PROC_FILE_COMMAND "proc-file"

/**
 * @brief The type used to represent a #Request from the client to the server
 * 
 */
typedef struct request {
    RequestType type; ///< The type of the request
    int priority; ///< The priority of the request (from 0 to 5)
    char* sender;   ///< The name of the input fifo of the client that sent the request
    file_d senderFD; ///< The writter to the client that sent the request
    char* inputFile; ///< The name of the input file
    char* outputFile; ///< The name of the output file
    int operationCount; ///< The number of operations requested
    char** operations; ///< The request operations
    int timeOfArrival; ///< Time of arrival in the server
    bool running; ///< Whether the server is processing the request
} REQUEST, * Request;

int getOperationCount(Request, char*);
int compareRequests(Request, Request);
bool readRequest(PipeReader, Request);
bool writeRequest(PipeWritter, Request);
char* requestToString(Request);
char* getRequestStatus(Config, int[], Request*, int);
void freeRequest(Request);
void freeRequestContent(Request);

#endif // _REQUEST_H_