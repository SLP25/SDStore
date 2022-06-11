/**
 * @file update.h
 * 
 * @brief File declaring the API for the #Update type
 * 
 */

#ifndef _UPDATE_H_

/**
 * @brief Include guard
 * 
 */
#define _UPDATE_H_

#include "pipeWrapper.h"
#include "request.h"
#include "utils.h"

/**
 * @brief The different types of #Update
 * 
 */
typedef enum updateType {
    U_REQUEST, ///< A new #Request has arrived
    U_REQUEST_FINISHED, ///< A #Request has finished executing
    U_FINISHED_OP, ///< An operation has finished
    U_SERVER_DISCONECTED ///< The server has disconnected
} UpdateType;

/**
 * @brief An update sent by the relay or the job managers to the router
 * 
 */
typedef struct update {
    UpdateType type; ///< The type of update
    Request request; ///< The #Request
    int operationId; ///< The id of the operation
} UPDATE, * Update;

void fromRequest(Update, Request);
bool readUpdate(PipeReader, Update);
bool writeUpdate(PipeWritter, Update);

#endif // _UPDATE_H_