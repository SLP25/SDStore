/**
 * @file main.c 
 * 
 * @brief File implementing the server's main entry point function
 * 
 */

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

#include "config.h"
#include "logging.h"
#include "pipeWrapper.h"
#include "request.h"
#include "router.h"
#include "update.h"
#include "utils.h"


file_d fifoInputFd; //< the file descriptor of the fifo entrance to the server

/**
 * @brief the handler in case a SIGTERM is received.
 * 
 * @param sig the code of the signal received.
 */
void TermHandler(int sig)
{
    if (sig == SIGTERM)
        close(fifoInputFd);
}



/**
 * @brief Runs the relay of the server.
 * 
 * The job of the relay is to transmit the requests from the clients to the router
 * 
 * @param output The descriptor of the write-end of the pipe to the router
 */
void runRelay(file_d output)
{
    PIPE_WRITTER pw;
    initPipeWritter(&pw, output);
    if (!fork()){//just cause its needed in order for open in O_RDONLY to not block.
        file_d t=open(SERVER_NAME,O_WRONLY);
        close(t);
        _exit(0);
    }
    fifoInputFd = open(SERVER_NAME, O_RDONLY);
    file_d tempOut = open(SERVER_NAME, O_WRONLY);

     if (fifoInputFd < 0) {
            printMessage(STDERR_FILENO, OPENFAILED);
            return;
        }
    PIPE_READER pr;
    initPipeReader(&pr, fifoInputFd);

    Request request = malloc(sizeof(REQUEST));
    UPDATE update;

    while (readRequest(&pr, request))
    {
        printMessage(STDOUT_FILENO, REQUESTRECEIVED);
        //send to router as an update through pipe (output)
        fromRequest(&update, request);
        writeUpdate(&pw, &update);
        freeRequestContent(request);
    }
    update.type=U_SERVER_DISCONECTED;
    writeUpdate(&pw, &update);
    close(tempOut);
    free(request);
    }

/**
 * @brief Server's main entry point
 * 
 * @param argc The number of arguments
 * @param argv The arguments of the server {"sdstored", "<config-file>", "<path-to-transformation-executables>"}
 * 
 * @return 0 If server ran successfully
 * @return 1 If an error occurred
 * 
 */
int main(int argc, char* argv[]) {
    signal(SIGTERM,TermHandler);
    
    if (argc < 3) {
        PRINTLN("Too few arguments");
        return 1;
    }
        
    if (argc > 3) {
        PRINTLN("Too many arguments");
        return 1;
    }

    //read config
    CONFIG config;
    if (!loadConfig(argv[1], &config)) {
        PRINTLN("Error importing config");
        return 1;
    }
  
    printMessage(STDOUT_FILENO, CONFIGLOADED);
        
    //create input fifo
    if (mkfifo(SERVER_NAME, 0660) == -1) {
        printMessage(STDERR_FILENO, PIPECREATEFAILED);
        return 1;
    }

    file_d router_pipe[2];

    //create pipe relay -> router
    if (pipe(router_pipe) == -1) {
        unlink(SERVER_NAME);
        printMessage(STDERR_FILENO, PIPECREATEFAILED);
        return 1;
    }
    //create router
    pid_t router_pid = fork();
    if (!router_pid) {
        runRouter(&config, router_pipe[0], router_pipe[1], argv[2]); //send write end too to pass along to the managers
        _exit(0);
    }

    close(router_pipe[0]);

    runRelay(router_pipe[1]);
    close(router_pipe[1]);
    unlink(SERVER_NAME);
    printMessage(STDERR_FILENO, SERVEREXITING);
    waitpid(router_pid,NULL,0);
    printMessage(STDERR_FILENO, SERVEREXITED);

    return 0;
}