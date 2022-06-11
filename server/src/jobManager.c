/**
 * @file jobManager.c
 * 
 * @brief File implementing the functions responsible for handling the execution of a #Request
 * 
 */


#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "config.h"
#include "jobManager.h"
#include "logging.h"
#include "request.h"
#include "update.h"
#include "utils.h"

/**
 * @brief Gets the name of the pipe to output the subroutine data to.
 * 
 * The name is the pid of the process
 * 
 * @param string The string to write to
 */
void getOutputPipeName(char* string) {
    sprintf(string, "%d", getpid());
}

/**
 * @brief Executes a child process with redirected standard input and output
 * 
 * @param in The descriptor of the file to redirect to standard input
 * @param out The descriptor of the file to redirect standard output to
 * @param binPath The path of the executable file
 * @param operation The name of the operation
 * 
 * @return pid_t The pid of the child process
 */
pid_t execOperation(file_d in, file_d out, char*binPath, char*operation) {
    pid_t pid;
    if (!(pid = fork ())){
        dup2 (in, STDIN_FILENO);
        dup2 (out, STDOUT_FILENO);
        close (in);
        close (out);
        char exe[strlen(binPath) + strlen(operation) + 1];
        exe[0] = '\0';
        strcat(exe, binPath);
        strcat(exe,operation);
        execl(exe, exe, NULL);
        _exit(0);
    }
    return pid;
}




/**
 * @brief Main function for the process responsible for handling the instances of a subprogram in the server
 * 
 * @param programName  The name of the subprogram
 * @param fifoName     The name of the FIFO to create
 * @param maxInstances The maximum number of instances of the program that can be running at once
 * @param binPath      The path to the binaries used
 * @param config       The #Config of the server
 * @param fifo         The descriptor of the FIFO to write the updates to
 * 
 * @return 1           On success
 * @return 0           If an error occured
 */
void runJobHandler(Request request, file_d fifo, char* binPath, Config config) {
    file_d fd[2];
    file_d in = open(request->inputFile, O_RDONLY);
    if (in<0) printMessage(STDERR_FILENO, CANTOPENINPUTFILE);


    pid_t pids[request->operationCount];
    int opsId[request->operationCount];

    PIPE_WRITTER pw;
    initPipeWritter(&pw, fifo);

    //Setup pipes for the stdin and stdout of children
    for (int i = 0; i < request->operationCount; i++){
        if (i ==request->operationCount-1){
            fd[1]=open(request->outputFile, O_WRONLY | O_TRUNC | O_CREAT, 0660);
            if (fd[1]<0) printMessage(STDERR_FILENO, CANTOPENOUTPUTFILE);
        }
        else 
            pipe(fd);

        pids[i]=execOperation(in, fd[1],binPath,request->operations[i]);
        opsId[i] = getProgramId(config, request->operations[i]);

        close (fd[1]);

        in = fd [0];
    }
    close(in);
    close(fd[1]);
    UPDATE update;

    //Wait for children to finish executing
    for(int i = 0; i < request->operationCount; i++) {
        int status;

        waitpid(pids[i],&status,0);

        //Check for success
        if(__WIFEXITED(status)) {
            if(!__WEXITSTATUS(status)) {
                update.operationId = opsId[i];
                update.type = U_FINISHED_OP;
                writeUpdate(&pw, &update);
            } else {
                printMessage(STDERR_FILENO, UNEXPECTEDERROR);
                return;
            }
        } else {
            printMessage(STDERR_FILENO, UNEXPECTEDERROR);
            return;
        }
    }

    //Request has finished
    //Notify router
    update.request = request;
    update.type = U_REQUEST_FINISHED;
    writeUpdate(&pw, &update);

}