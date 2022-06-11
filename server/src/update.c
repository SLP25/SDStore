/**
 * @file update.c
 * 
 * @brief File implementing the #Update type
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "logging.h"
#include "update.h"


/**
 * @brief Default size for a string
 * 
 */
#define STR_SIZE 256

/**
 * @brief Creates an #Update from a #Request
 * 
 * @param u The #Update to create
 * @param r The given #Request
 */
void fromRequest(Update u, Request r) {
    u->type = U_REQUEST;
    u->request = r;
}

/**
 * @brief Reads an #Update from the given #PipeReader
 * 
 * @param pr The given #PipeReader
 * @param u The given #Update
 * 
 * @return true If the reading was successful
 * @return false If an error occurred
 */
bool readUpdate(PipeReader pr, Update u) 
{
    if (!readBytes(pr, sizeof(u->type), &u->type))
        return false;
    switch (u->type) {

        case U_REQUEST_FINISHED:
        case U_REQUEST:
        u->request = malloc(sizeof(REQUEST));
        return readRequest(pr, u->request);

        case U_FINISHED_OP:            
        return readBytes(pr, sizeof(u->operationId), &u->operationId);

        case U_SERVER_DISCONECTED:
        return true;

        default:
        printMessage(STDERR_FILENO, UNKNOWNUPDATETYPE);
        return true;
    }
    return true;
}

/**
 * @brief Writes an #Update to the given #PipeWritter
 * 
 * @param pw The given #PipeWritter
 * @param u The #Update to write
 * 
 * @return true If the writting was successful
 * @return false If an error occurred
 */
bool writeUpdate(PipeWritter pw, Update u) {
    switch (u->type) {

        case U_REQUEST:
        case U_REQUEST_FINISHED:
        writeBytes(pw, sizeof(u->type), &u->type);
        writeRequest(pw, u->request);
        break;

        case U_FINISHED_OP:
        writeBytes(pw, sizeof(u->type), &u->type);
        writeBytes(pw, sizeof(u->operationId), &u->operationId);
        break;

        case U_SERVER_DISCONECTED:
        writeBytes(pw, sizeof(u->type), &u->type);
        break;

        default:
        printMessage(STDERR_FILENO, UNKNOWNUPDATETYPE);
    }
    return flushPipe(pw);
}