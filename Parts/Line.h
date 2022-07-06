#ifndef LINE_H
#define LINE_H

#include "STM32X.h"
#include "UART.h"

/*
 * PUBLIC DEFINITIONS
 */

/*
 * PUBLIC TYPES
 */

typedef struct {
	char * bfr;
	uint32_t size;
	uint32_t index;
} Line_t;

/*
 * PUBLIC FUNCTIONS
 */

void Line_Init(Line_t * line, char * bfr, uint32_t size);
char * Line_Parse(Line_t * line, char ch);
void Line_Clear(Line_t * line);

// Helper function - does a blocking line read on a UART.
const char * Line_ParseUart(Line_t * line, UART_t * uart, uint32_t timeout);

/*
 * EXTERN DECLARATIONS
 */

#endif //LINE_H
