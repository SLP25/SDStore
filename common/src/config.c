/**
 * @file config.c
 * 
 * @brief File implementing the loading of the configuration file
 * 
 */

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "utils.h"

/**
 * @brief Maximum number of characters to read from file at once
 * 
 */
#define BUFFER_SIZE 128

/**
 * @brief Get the id of the given program
 * 
 * @param config The #Config
 * @param name The name of the program
 * 
 * @return int The id of the program (-1 if no such program)
 */
int getProgramId(Config config, char* name) {
    for(int i = 0; i < config->programCount; i++) 
        if(!strcmp(config->programs[i], name))
            return i;

    return -1;
}

/**
 * @brief Retrieves the integer in a line of the config file
 * 
 * @note  The buffer is modified. The ' ' character and the '\n' are replaced by a null terminator
 * 
 * @param buffer     The given buffer
 * @param bufferSize The size of the buffer
 * @param offset     The offset needed to put the descriptor pointing to the correct byte for the start of the next line
 * @return int       The number of instances allowed to run for a given program
 */
int extractNumber(char* buffer, int bufferSize, int* offset) {
    int count = 0;
    int start = 0;
    int result = 0;
    for(; count < bufferSize; count++) {
        if(buffer[count] == ' ') {
            buffer[count] = '\0';
            start = count + 1;
        } else if(buffer[count] == '\n') {
            buffer[count] = '\0';
            count++;
            result = atoi(&buffer[start]);  //TODO: trocar por safeStrToInt ??
            break;
        }
    }

    *offset = bufferSize - count;

    return result;
}

/**
 * @brief Loads the configuration from a file
 * 
 * @param fileName The name of the file containing the configuration
 * @param config   The #Config to write to
 * 
 * @return true on success
 * @return false on failure
 */
bool loadConfig(char* fileName, Config config) {
    bool result = true;

    char buffer[BUFFER_SIZE];
    int bufferPosition = 0;
    file_d file = open(fileName, O_RDONLY);

    if(file >= 0) {
        for(int i = 0; result; i++) {
            int bytesRead = read(file, buffer, BUFFER_SIZE - bufferPosition);
            if(bytesRead == -1)
                result = false;

            if(bytesRead == 0) {
                config->programCount = i;
                break;
            }       

            int offset;
            
            config->instances[i] = extractNumber(buffer, bytesRead, &offset);
            strcpy(config->programs[i], buffer);
            if(lseek(file, -offset, SEEK_CUR) < 0) {
                result = false;
                break;
            }
        }
        close(file);
    } else {
        result = false;
    }

    return result;
}

/**
 * @brief Gets the name of the program with the given id
 * 
 * @param config The given #Config
 * @param id The given id
 * 
 * @return char* The name of the program
 */
char* getProgramName(Config config, int id) {
    return config->programs[id];
}