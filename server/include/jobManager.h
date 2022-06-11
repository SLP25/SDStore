/**
 * @file jobManager.h
 * 
 * @brief File declaring the API used for the handling of the instances of a certain sub program within
 * the server
 * 
 */

#ifndef _JOB_MANAGER_H_

/**
 * @brief Include guard
 */
#define _JOB_MANAGER_H_

#include "config.h"
#include "request.h"
#include "utils.h"

void runJobHandler(Request, file_d, char*, Config);

#endif // _JOB_MANAGER_H_