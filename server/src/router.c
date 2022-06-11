/**
 * @file router.c 
 * 
 * @brief File implementing the router
 * 
 */

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "config.h"
#include "jobManager.h"
#include "logging.h"
#include "pipeWrapper.h"
#include "request.h"
#include "requestSorter.h"
#include "router.h"
#include "update.h"
#include "utils.h"
#include "list.h"

/**
 * @brief Checks if a #Request is valid
 * 
 * A #Request is valid if it has at least 1 operation, input, output and senders attributes
 * set
 * 
 * @param config  The server #Config
 * @param request The given #Request
 * 
 * @return true If the #Request is valid 
 * @return false If the #Request is not valid
 */
bool validateRequest(Config config, Request request) {

    if (request->inputFile == NULL
     || request->outputFile == NULL
     || request->operations == NULL
     || request->sender == NULL
     || request->operationCount <= 0) {
        printMessage(STDERR_FILENO,REQUESTWASNOTVALIDATED);
        return false;
    }

    for (int i = 0; i < request->operationCount; i++) {
        if (getProgramId(config, request->operations[i]) == -1) {
            printMessage(STDERR_FILENO,REQUESTWASNOTVALIDATED);
            return false;
        }
    }

    printMessage(STDERR_FILENO,REQUESTWASVALIDATED);
    return true;
}

/**
 * @brief Sends the given string to a client
 * 
 * @param client The file descriptor of the client's fifo
 * @param msg The string to send
 * 
 * @return true If the string was sent successfully
 * @return false If an error occurred
 */
bool answerClient(file_d client, char* msg)
{
    PIPE_WRITTER P;
    initPipeWritter(&P,client);

    writeString(&P, msg);
    return flushPipe(&P);
}

/**
 * @brief Gets the string to send to the client after the #Request has finished executing
 * 
 * @param request The given #Request
 * 
 * @return char* The string to send to the client
 */
char* getRequestEndResult(Request request) {
    char* res = malloc(256);

    //Get the size of the input and output files
    file_d in = open(request->inputFile,O_RDONLY);
    file_d out = open(request->outputFile,O_RDONLY);
    snprintf(res, 256, "Concluded (bytes input: %ld, bytes output: %ld)", lseek(in,0,SEEK_END), lseek(out,0,SEEK_END));
    close(in);
    close(out);
    return res;
}

/**
 * @brief Runs the router of the server
 * 
 * The router is the 'brain' of the server. It is responsible for receiving #Request from clients, determining
 * which one will execute and notify the clients about the progress.
 * 
 * @param config The #Config of the server
 * @param pipe_read The input pipe of the router
 * @param pipe_write The output pipe of the router (used to send to subprocesses)
 * @param binPath The path of the executables of the transformations
 */
void runRouter(Config config, file_d pipe_read, file_d pipe_write, char* binPath) {
    bool up = true;
    int inRouter=0;
    PIPE_READER pr;
    RequestSorter sorter = newRequestSorter(config->programCount);
    initPipeReader(&pr, pipe_read);
    int availableProcesses[config->programCount];
    for (int i = 0; i < config->programCount;i++) availableProcesses[i] = config->instances[i];

    UPDATE update;
    char *a;

    RequestsList requests = initRequestList();
    
    while ((up || inRouter) && readUpdate(&pr, &update))// || !notEmpty(sorter)) //readUpdate is always true while im holding the write end of the pipe
    {
        switch(update.type)
        {
            case U_REQUEST: 
                update.request->senderFD=open(update.request->sender, O_WRONLY);
                update.request->running=false;

                if (update.request->senderFD>=0)
                    printMessage(STDERR_FILENO,CREATEDWRITEPIPETOCLIENT);
                else break;
                switch (update.request->type)
                {
                    //if status send status to client through fifo
                    case STATUS:
                        printMessage(STDERR_FILENO,STATUSREQUEST);
                        a = getRequestStatus(config, availableProcesses, requests->requests, getNumberInArray(requests));
                        answerClient(update.request->senderFD,a);
                        close(update.request->senderFD);
                        free(a);
                        freeRequest(update.request);
                        break;

                    //if proc_file add to list
                    case PROCESS_FILE:
                        printMessage(STDERR_FILENO,PROCESSFILEREQUEST);
                        if (validateRequest(config, update.request)) {
                            inRouter++;
                            insertRequest(requests,update.request);
                            enqueue(sorter, update.request,  config);
                            answerClient(update.request->senderFD, "Pending");
                        }
                        else{
                            answerClient(update.request->senderFD, "Request received");
                            answerClient(update.request->senderFD, "Request not considered valid");
                            answerClient(update.request->senderFD, "Concluded");
                            close(update.request->senderFD);
                            freeRequest(update.request);
                        }
                        break;
                }
                break;
                
            //if operation finished mark as available
            case U_FINISHED_OP:
                printMessage(STDERR_FILENO,OPERATIONFINISHED);
                availableProcesses[update.operationId]++;
                break;

            case U_SERVER_DISCONECTED:
                up = false;
                break;

            case U_REQUEST_FINISHED:
                inRouter--;
                printMessage(STDERR_FILENO,REQUESTFINISHED);
                a = getRequestEndResult(update.request);
                answerClient(update.request->senderFD, a);
                close(update.request->senderFD);
                removeRequest(requests,update.request->timeOfArrival);

                free(a);
                freeRequest(update.request);
                break;

            default:
                printMessage(STDERR_FILENO, UNKNOWNUPDATETYPE);
                freeRequest(update.request);
                break;
        }
        Request r = nextInLine(sorter, config, availableProcesses);
        if (r != NULL){
            for (int i = 0; i <r->operationCount; i++)
                availableProcesses[getProgramId(config,r->operations[i])]--;
            r->running=true;
            if (!fork()) {

                answerClient(r->senderFD, "Processing");
                close(pipe_read);
                runJobHandler(r, pipe_write, binPath, config);
                freeRequest(r);

                _exit(0);
            }
        }
    }

    freeRequestList(requests);
    deleteRequestSolver(sorter);
    close(pipe_read);
    printMessage(STDERR_FILENO, ROUTEREXITED);
    
}