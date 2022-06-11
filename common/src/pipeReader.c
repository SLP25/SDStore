/**
 * @file pipeReader.c
 * 
 * @brief File implementing the wrapper for reading from a pipe / fifo
 * 
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "logging.h"
#include "pipeWrapper.h"

/**
 * @brief Initializes a #PipeReader
 * 
 * @param pr The #PipeReader to initialize
 * @param fd The file descriptor to read data from
 */
void initPipeReader(PipeReader pr, file_d fd) {
    pr->pipe = fd;
    pr->available = 0;
    pr->pos = 0;
}

/**
 * @brief Checks if the buffer of a #PipeReader is empty
 * 
 * @param pr The given #PipeReader
 * 
 * @return true If the buffer is empty
 * @return false If the buffer is not empty
 */
bool isBufferEmpty(PipeReader pr) {
    return pr->pos >= pr->available;
}

//if false, the pipe was closed
/**
 * @brief Reads data from the given #PipeReader
 * 
 * Returns false if the pipe was closed
 * 
 * @param pr The given #PipeReader
 * 
 * @return true If the reading was successful
 * @return false If the pipe/fifo was closed
 */
bool readPipe(PipeReader pr) {
    pr->available = read(pr->pipe, pr->buffer, READER_BUFFER_SIZE);
    pr->pos = 0;
    return pr->available > 0;
}

/**
 * @brief Reads the given number of bytes from a #PipeReader
 * 
 * If the pipe/fifo was closed before n bytes were read, returns false. The program halts until the required
 * number of bytes were read or the pipe closes.
 * 
 * @param pr The given #PipeReader
 * @param n The number of bytes to read
 * @param buff The buffer to write data to
 * 
 * @return true If the given number of bytes were read successfully
 * @return false If the pipe/fifo closed before the given number of bytes were read
 */ 
bool readBytes(PipeReader pr, int n, void* buff) {

    while (n > 0) {
        //No more data to read
        if (isBufferEmpty(pr) && !readPipe(pr))
            return false;

        int read = MIN(n, pr->available - pr->pos);

        //Copy data to buffer
        memcpy(buff, pr->buffer + pr->pos, read);
        pr->pos += read;
        buff = (void*)((char*)buff + read);
        n -= read;
    }

    return true;
}

//reads until next \0
//if there is no \0, the program halts until \0 is received
//if the pipe is closed before \0 is reached, false is returned
//n is the size to edit in dest (\0 included)
/**
 * @brief Reads a string from a #PipeReader.
 * 
 * It will read until the next null byte in the pipe. If there is no null byte, the program halts until it
 * receives or or the pipe closes
 * 
 * @param pr The #PipeReader to read from
 * @param dest The destination string
 * @param n The maximum number of bytes to read ('\0' included)
 * 
 * @return true If the reading was successful
 * @return false If the reading failed
 */
bool readString(PipeReader pr, char* dest, int n)
{
    while (n > 0)
    {
        if (isBufferEmpty(pr) && !readPipe(pr))
            return false;

        int read = MIN(n, pr->available - pr->pos);

        strncpy(dest, pr->buffer + pr->pos, read);
        read = strlen(dest);
        pr->pos += read + 1;
        dest += read;
        n -= read;

        if (*dest == '\0')
            return true;
    }

    printMessage(STDERR_FILENO, BUFFERREACHEND);
    return true;
}