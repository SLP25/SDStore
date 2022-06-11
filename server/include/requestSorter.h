/**
 * @file requestSorter.h
 *
 * @brief File which defines the API used to sort the incoming requests
 * 
 */

#ifndef _REQUEST_SORTER_H_

/**
 * @brief Include guard
 * 
 */
#define _REQUEST_SORTER_H_

#include "config.h"
#include "request.h"
#include "utils.h"

typedef struct requestSorter *RequestSorter;

RequestSorter newRequestSorter(int);

void deleteRequestSolver(RequestSorter);

bool enqueue(RequestSorter, Request, Config);

Request nextInLine(RequestSorter, Config, int[]);

bool notEmpty(RequestSorter);

#endif // _REQUEST_SORTER_H_