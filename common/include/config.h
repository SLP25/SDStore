/**
 * @file config.h       
 * 
 * @brief File defining the API for loading the configuration file
 * 
 */

#ifndef _CONFIG_H_

/**
 * @brief Include guard
 * 
 */
#define _CONFIG_H_

/**
 * @brief The number of different programs mentioned in the config file
 * 
 */
#define NUMBER_PROGRAMS 7

/**
 * @brief Maximum number of characters for the name of a program mentioned in the config file
 * 
 */
#define MAX_PROGRAM_SIZE 256

/**
 * @brief Structure used to represent the configuration of the server
 * 
 */
typedef struct config {
    int instances[NUMBER_PROGRAMS]; ///< Maximum number of instances of the programs
    char programs[NUMBER_PROGRAMS][MAX_PROGRAM_SIZE]; ///< Names of the programs
    int programCount; ///< Number of programs
} CONFIG, * Config;


char loadConfig(char*, Config);

int getNumberInstances(char*, Config);

int getProgramId(Config, char*);

char* getProgramName(Config, int);

#endif // _CONFIG_H_