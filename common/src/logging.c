/**
 * @file logging.c
 * 
 * @brief File implementing the logging API for the server and client
 * 
 */

#include <string.h>
#include <time.h>
#include <unistd.h>

#include "logging.h"


/**
 * @brief The list of the prefixes used for each error type
 * 
 */
#define LIST_OF_ERROR_IDENTIFIERS\
    ENTRY(FATAL_ERROR,  "[FATAL] ") \
    ENTRY(ERROR,        "[ERROR] ") \
    ENTRY(WARNING,      "[WARNI] ") \
    ENTRY(INFO,         "[INFOR] ")


/**
 * @brief Gets the error message and the type of a given code
 * 
 * @param errorCode The code of the error
 * @param type Where to save the type of the error
 * @param error Where to save the description
 */
void getErrorMessage(int errorCode,ERROR_TYPE* type,char error[]){

    /*
    Copies the error message of the given error to the given string and the type to the
    *type pointer
    */
    #define ENTRY(a,b,c) \
        case a: \
            strcpy(error, c); \
            *type=b; \
            break;

    switch(errorCode) {
        LIST_OF_ERROR_DESCRIPTION
    }

    #undef ENTRY
}

/**
 * @brief Gets the prefix used to display the given error type
 * 
 * @param error_type The given error type
 * @param error The buffer to write the prefix to
 * @param bytes The number of characters to write
 */
void getOutputErrorType(ERROR_TYPE error_type, char error[],int bytes){

    /*

    Jumps to the case of the given error type, and copies the given number of bytes from 
    the error prefix to the given buffer

    */
    #define ENTRY(a,b) \
        case a: \
            for (int i = 0; i < bytes;i++) error[i] = b[i];\
            break;

    switch(error_type) {
        //This is the macro defined in this file not the header
        LIST_OF_ERROR_IDENTIFIERS
        default:
            break;
    }
    #undef ENTRY
}

/**
 * @brief Gets the current time as a string in the from "YYYY-mm-dd hh:mm:ss :"
 * 
 * @param c The string to write the time to
 */
void getCurrentTime(char c[]){
    time_t timer;
    struct tm* tm_info;
    timer = time(NULL);
    tm_info = localtime(&timer);
    strftime(c, 28, "%Y-%m-%d %H:%M:%S :", tm_info);
}

/**
 * @brief Writes the given message to the given file descriptor
 * 
 * @param fd The descriptor of the file to write to
 * @param errorCode The given error
 */
void printMessage(file_d fd, ERROR_CODE errorCode){
    ERROR_TYPE type = INFO;
    char c[8+21+256];
    getCurrentTime(c+8);
    getErrorMessage(errorCode,&type,c+8+20);
    getOutputErrorType(type,c,8);
    write(fd,c,strlen(c));
}
