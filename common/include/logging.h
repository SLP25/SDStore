/**
 * @file logging.h
 * 
 * @brief File declaring the function used for logging to the standard output / error, as well as declaring
 * all the different warnings / errors of the applications
 * 
 */

#ifndef __LOGGING_H__

/**
 * @brief Include guard
 * 
 */
#define __LOGGING_H__

#include "utils.h"

/**
 * @brief The different types of errors that can occur
 * 
 */
typedef enum errorsType{
    FATAL_ERROR=0, ///< An error that cannot be safely recovered from
    ERROR=1, ///< An error which can safely be recovered from
    WARNING=2, ///< A warning
    INFO=3 ///< A message displayed for information only
} ERROR_TYPE;

/**
 * @brief this is the list of all the errors associating their code with their type and description
 * description: max of 256
 */
#define LIST_OF_ERROR_DESCRIPTION \
    ENTRY(CONFIGLOADED, INFO, "Config file loaded\n") \
    ENTRY(REQUESTRECEIVED, INFO, "Request received\n") \
    ENTRY(REQUESTRECEIVEDBYROUTER, INFO, "Request received in router\n") \
    ENTRY(CREATEDWRITEPIPETOCLIENT,INFO,"Created a write pipe to client\n") \
    ENTRY(REQUESTWASVALIDATED,INFO,"Request was considered valid\n") \
    ENTRY(REQUESTWASNOTVALIDATED,WARNING,"Request was considered invalid\n") \
    ENTRY(STATUSREQUEST,INFO,"Status was requested\n") \
    ENTRY(PROCESSFILEREQUEST,INFO,"Process file was requested\n") \
    ENTRY(SERVEREXITING,INFO,"Server exiting\n") \
    ENTRY(SERVEREXITED,INFO,"Server exited\n") \
    ENTRY(RELAYEXITED,INFO,"Relay exited\n") \
    ENTRY(ROUTEREXITED,INFO,"Router exited\n") \
    ENTRY(WRITEFAILED,ERROR,"flushPipe: write failed\n") \
    ENTRY(RUNJOBHANDLE,ERROR,"main(server): runJobHandler returned unexpectedly\n") \
    ENTRY(BUFFERREACHEND,WARNING,"readString: buffer capacity reached. the returned string is not null-terminated\n") \
    ENTRY(PIPECREATEFAILED,ERROR,"The pipe could not be created\n") \
    ENTRY(OPENFAILED,ERROR,"Error opening file\n") \
    ENTRY(UNKNOWNREQUESTTYPE,ERROR,"Unknown request type\n") \
    ENTRY(UNKNOWNUPDATETYPE,ERROR,"Unknown update type\n") \
    ENTRY(RELAYDELETEDFIFO,WARNING,"FIFO " SERVER_NAME " persisted after server closed. Clean up performed by Relay\n") \
    ENTRY(REQUESTDROPPED, WARNING, "Bad Request\n") \
    ENTRY(UNEXPECTEDERROR, ERROR, "An error occured in a process\n") \
    ENTRY(OPERATIONFINISHED,INFO,"Operation finished successfully\n") \
    ENTRY(REQUESTFINISHED,INFO,"Request finished successfully\n") \
    ENTRY(CANTOPENINPUTFILE,ERROR,"Cant open input file does it exist?\n") \
    ENTRY(CANTOPENOUTPUTFILE,ERROR,"Cant open output file does it exist?\n") \
    ENTRY(MALLOCFAILED, FATAL_ERROR, "Cannot allocate memory\n") \
    ENTRY(SERVERCLOSED, FATAL_ERROR, "The server is closed\n") \
    ENTRY(REQUESTSORTERFAILEDALLOCQUEUES, FATAL_ERROR, "Couldn't malloc the queues inside the requestSorter\n") \
    ENTRY(REQUESTSORTERFAILEDALLOC,FATAL_ERROR,"Couldn't malloc the requestSorter\n")

/**
 * @brief All the different errors that can occur in the applications
 * 
 */
typedef enum ErrorCode {
    #define ENTRY(a,b,c) a,
    LIST_OF_ERROR_DESCRIPTION
    #undef ENTRY
} ERROR_CODE;

void printMessage(file_d fd, ERROR_CODE errorCode);

#endif // __LOGGING_H__