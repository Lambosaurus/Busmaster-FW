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
bool NParse_String(const char ** str, char * value, uint32_t size, uint32_t * count);
bool NParse_Hex(const char ** str, uint32_t * value);

uint32_t NFormat_Hex(char * str, uint8_t * hex, uint32_t count, char space);
uint32_t NFormat_String(char * str, uint32_t size, uint8_t * data, uint32_t count, char delimiter);


#endif //NPARSE_H
