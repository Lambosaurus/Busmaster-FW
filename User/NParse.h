#ifndef NPARSE_H
#define NPARSE_H

#include "STM32X.h"

/*
 * PUBLIC DEFINITIONS
 */

/*
 * PUBLIC TYPES
 */

/*
 * PUBLIC FUNCTIONS
 */

bool NParse_Uint(const char ** str, uint32_t * value);
bool NParse_Kuint(const char ** str, uint32_t * value);
bool NParse_Bytes(const char ** str, uint8_t * value, uint32_t size, uint32_t * count);

#endif //NPARSE_H
