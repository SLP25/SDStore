/**
 * @file pipeWrapper.h
 * 
 * @brief File defining functions related to reading / writing to / from pipes and fifos
 */

#ifndef _PIPEWRAPPER_H_

/**
 * @brief Include guard
 */
#define _PIPEWRAPPER_H_

#include "utils.h"

/**
 * @brief The size (bytes) of the buffer for a #PipeReader
 * 
 */
#define READER_BUFFER_SIZE 16384

/**
 * @brief The size (bytes) of the buffer for a #PipeWriter
 * 
 */
#define WRITTER_BUFFER_SIZE 16384

/**
 * @brief Wrapper for reading data from a pipe / fifo
 * 
 */
typedef struct pipeReader {
    file_d pipe; ///< The file descriptor of the pipe/fifo
    char buffer[READER_BUFFER_SIZE]; ///< The intermediate buffer to which the data is written
    int available;  ///< The number of bytes available for reading to the buffer
    int pos;        ///< The next position to read from the buffer
} PIPE_READER, * PipeReader;

void initPipeReader(PipeReader, file_d);
bool readBytes(PipeReader, int, void*);
char readString(PipeReader, char*, int n);


/**
 * @brief Wrapper for writing data to a pipe / fifo
 * 
 */
typedef struct pipeWritter {
    file_d pipe; ///< The file descriptor of the pipe/fifo
    char buffer[WRITTER_BUFFER_SIZE]; ///< The intermediate buffer to which the data is stored before being written
    int pos; ///< The next position to write from the buffer

} PIPE_WRITTER, * PipeWritter;

void initPipeWritter(PipeWritter, file_d);
void writeBytes(PipeWritter, int, void*);
void writeString(PipeWritter, char*);
bool flushPipe(PipeWritter);

#endif // _PIPEWRAPPER_H_