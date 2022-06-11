/**
 * @file processArgs.h
 * 
 * @brief File which defines the function used by the client to parse its arguments
 * 
 */

#ifndef _PROCESS_ARGS_H_

/**
 * @brief Include guard
 * 
 */
#define _PROCESS_ARGS_H_

#include "request.h"

char parseArguments(int, char*[], Request);

#endif //_PROCESS_ARGS_H_