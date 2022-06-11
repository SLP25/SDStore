/**
 * @file utils.h
 * 
 * @brief File defining common macros, typedefs and functions
 */

#ifndef _UTILS_H_

/**
 * @brief Include guard
 */
#define _UTILS_H_

/**
 * @brief Alias for file descriptor numbers
 * 
 */
typedef int file_d;

/**
 * @brief Alias for boolean types
 * 
 */
typedef char bool;

/**
 * @brief Alias for true logical value
 * 
 */
#define true 1

/**
 * @brief Alias for false logical value
 * 
 */
#define false 0

/**
 * @brief Inline function for computing the minimum of two values
 * 
 */
#define MIN(a, b) ((a) < (b) ? (a) : (b))




/**
 * @brief Utility for printing a line to standard out without printf (debugging only)
 * 
 */
#define PRINTLN(str) (write(STDOUT_FILENO, str "\n", sizeof(str) + 1))
//#define PRINTLN(str)

/**
 * @brief The name of the fifo used by the server to receive requests from the clients
 * 
 */
#define SERVER_NAME "SDStore"

#endif // _UTILS_H_