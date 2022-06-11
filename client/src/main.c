/**
 * @file main.c
 *
 * @brief File containing the client's main entry point
 * 
 */

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "logging.h"
#include "processArgs.h"

/**
 * @brief Gets the name of the pipe to output the subroutine data to.
 * 
 * The name is the PID of the process
 * 
 * @param string The string to write to
 */
void getResponsePipeName(char* string) {
    sprintf(string, "%d", getpid());
}

/**
 * @brief Main client's entry point
 * 
 * @param argc The number of arguments
 * @param argv The arguments of the client
 *
 * The arguments are {"sdstore" "status"} for a status request, or
 * {"sdstore" "proc-file" "<priority (0-5)>" "<input-file>" "<output-file>" "<transformation-1>" "<transformation-2>" ...}
 * 
 * The priority is optional.
 * 
 * @return 0 On success
 */
int main(int argc, char* argv[])
{
    REQUEST r;

    if (!parseArguments(argc, argv, &r)) {
        PRINTLN("Invalid arguments");
        return 1;
    }

    char clientFifoName[32];
    getResponsePipeName(clientFifoName);

    if (mkfifo(clientFifoName, 0660) == -1) {
        printMessage(STDERR_FILENO, PIPECREATEFAILED);
        return 1;
    }

    r.sender = clientFifoName;
    file_d serverFifo = open(SERVER_NAME, O_WRONLY);

    if (serverFifo < 0) {
        unlink(clientFifoName);
        PRINTLN("Server not reacheable");
        return 1;
    }

    PIPE_WRITTER pw;
    initPipeWritter(&pw, serverFifo);
    writeRequest(&pw, &r);
    flushPipe(&pw);
    close(serverFifo);

    //Create the FIFO for the server to reply to
    file_d clientFifo = open(clientFifoName, O_RDONLY);

    if (clientFifo < 0) {
        unlink(clientFifoName);
        printMessage(STDIN_FILENO, OPENFAILED);
        return 1;
    }

    PIPE_READER pr;
    initPipeReader(&pr, clientFifo);
    //Server responses do not exceed 4096 bytes
    char response[4096];
    
    /*
    Await for server to send response. Exit when pipe closes
    */
    while (readString(&pr, response, sizeof(response)))
    {
        int len = strlen(response);

        //Add a '\n' and '\0' to the end of the received string,
        //as the server does not terminate messages with '\n'
        response[len++] = '\n'; 
        response[len] = '\0';
        write(STDOUT_FILENO, response, len);
    }

    close(clientFifo);
    unlink(clientFifoName);
    return 0;
}