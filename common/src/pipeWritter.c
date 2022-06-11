/**
 * @file pipeWritter.c
 * 
 * @brief File implementing the wrapping of writing data to a pipe/fifo
 * 
 */


#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "logging.h"
#include "pipeWrapper.h"

/**
 * @brief Initializes a #PipeWritter
 * 
 * @param pw The given #PipeWritter
 * @param fd The file descriptor the #PipeWritter should write to
 */
void initPipeWritter(PipeWritter pw, file_d fd) {
    pw->pipe = fd;
    pw->pos = 0;
}

/**
 * @brief Checks if the buffer of a #PipeWritter is full
 * 
 * @param pw The given #PipeWritter
 * 
 * @return true If the buffer is full
 * @return false If the buffer is not full
 */
bool isBufferFull(PipeWritter pw) {
    return pw->pos >= WRITTER_BUFFER_SIZE;
}

/**
 * @brief Writes the given amount of bytes through a #PipeWritter
 * 
 * The #PipeWritter must be flushed for the data to be sent
 * 
 * @param pw The given #PipeWritter
 * @param n The number of bytes to write
 * @param arr The bytes to write
 */
void writeBytes(PipeWritter pw, int n, void* arr) {
    while (n > 0) {

        if (isBufferFull(pw))
            flushPipe(pw);

        int write = MIN(n, WRITTER_BUFFER_SIZE - pw->pos);
        memcpy(pw->buffer + pw->pos, arr, write);
        pw->pos += write;
        n -= write;
    }
}

/**
 * @brief Writes a string through a #PipeWritter
 * 
 * The #PipeWritter must be flushed for the data to be sent
 * 
 * @param pw The given #PipeWritter
 * @param str The string to write
 * 
 */
void writeString(PipeWritter pw, char* str) {
    int n = strlen(str);
    writeBytes(pw, n + 1, str);
}

/**
 * @brief Flushes the #PipeWritter , i.e., writes to the pipe the data
 * in its buffer
 * 
 * @param pw The given #PipeWritter
 */
bool flushPipe(PipeWritter pw) {
    if (write(pw->pipe, pw->buffer, pw->pos) < 0) {
        printMessage(STDERR_FILENO, WRITEFAILED);
        return false;
    }
        
    
    pw->pos = 0;
    return true;
}