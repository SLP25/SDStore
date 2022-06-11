/**
 * @file processArgs.c
 * 
 * @brief File implementing the function which parses the client's arguments
 * 
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "logging.h"

#include "../include/processArgs.h"
#include "request.h"

/**
 * @brief Parses the given arguments into a ::STATUS #Request
 * 
 * @param argc      The number of arguments
 * @param argv      The arguments
 * @param request   The #Request to write to
 * 
 * @return 1        If the parsing is successful
 * @return 0        If the parsing failed
 */
bool parseStatus(int argc, char* argv[], Request request)  {
    //Check for the correct number of arguments and that
    //the second argument corresponds to the status command
    if(argc == 2 && !strcmp(argv[1], STATUS_COMMAND)) {
        request->type = STATUS;
        return true;
    } 

    return false;
}

/**
 * @brief Safely converts a string to an integer
 * 
 * @param str The string to convert
 * @param val The integer in which to write the value
 * 
 * @return 1  If the string corresponds to an integer
 * @return 0  If the string is not an integer
 */
bool safeStrToInt(char* str, int* val) {
    int result = 0;

    while(*str) {
        if(*str >= '0' && *str <= '9') {
            result *= 10;
            result += *str - '0';
        } else {
            return false;
        }
        str++;
    }

    *val = result;
    return true;
}

/**
 * @brief Parses the given arguments into a ::PROC_FILE #Request
 * 
 * @param argc      The number of arguments
 * @param argv      The arguments
 * @param request   The #Request to write to
 * 
 * @return 1        If the parsing is successful
 * @return 0        If the parsing failed
 */
bool parseProcFile(int argc, char* argv[], Request request)  {
    //Check for the correct number of arguments and that
    //the second argument corresponds to the proc_file command
    if(argc >= 5 && !strcmp(argv[1], PROC_FILE_COMMAND)) {
        request->type = PROCESS_FILE;
        
        //Client should not fill this with valid data
        request->senderFD=-1;
        request->running = false;
        //Default priority value
        request->priority = 0;
        int currentArg = 3;
        if(!strcmp(argv[2], "-p") && safeStrToInt(argv[3], &request->priority))
            ++currentArg;
        else
            currentArg = 2;

        //IO files
        request->inputFile = malloc(strlen(argv[currentArg]) + 1);
        request->outputFile = malloc(strlen(argv[currentArg + 1]) + 1);

        //Check if malloc failed
        if(!request->inputFile || !request->outputFile) {
            printMessage(STDERR_FILENO, MALLOCFAILED);
            _exit(1);
        }

        strcpy(request->inputFile, argv[currentArg++]);
        strcpy(request->outputFile, argv[currentArg++]);

        //Operations
        request->operationCount = argc - currentArg;
        request->operations = malloc(sizeof(char*) * request->operationCount);
        if(!request->operations) {
            printMessage(STDERR_FILENO, MALLOCFAILED);
            _exit(1);
        }
        for(int i = currentArg; i < argc; i++) {
            request->operations[i - currentArg] = malloc(strlen(argv[i]) + 1);
            if(!request->operations[i - currentArg]) {
                printMessage(STDERR_FILENO, MALLOCFAILED);
                _exit(1);
            }
            request->operations[i - currentArg] = argv[i];
        }
        return true;
    } 

    return false;
}



/**
 * @brief Parses the given arguments into the proper #Request to send to the server
 * 
 * @param argc      The number of arguments
 * @param argv      The arguments
 * @param request   The #Request to write to
 * 
 * @return 1        If the parsing is successful
 * @return 0        If the parsing failed
 */
bool parseArguments(int argc, char* argv[], Request request) {
    return parseStatus(argc, argv, request) || parseProcFile(argc, argv, request);
}