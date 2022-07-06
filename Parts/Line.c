
#include "Line.h"
#include "CORE.h"

/*
 * PRIVATE DEFINITIONS
 */

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

/*
 * PRIVATE VARIABLES
 */

/*
 * PUBLIC FUNCTIONS
 */

void Line_Init(Line_t * line, char * bfr, uint32_t size)
{
	line->bfr = bfr;
	line->size = size;
	line->index = 0;
}

char * Line_Parse(Line_t * line, char ch)
{
	char * response = NULL;
	switch (ch)
	{
	case '\r':
	case '\n':
		if (line->index)
		{
			line->bfr[line->index] = 0;
			response = line->bfr;
		}
		line->index = 0;
		break;
	default:
		if (line->index >= line->size)
		{
			// Line overrun. Discard line.
			line->index = 0;
		}
		line->bfr[line->index++] = ch;
		break;
	}
	return response;
}

void Line_Clear(Line_t * line)
{
	line->index = 0;
}

const char * Line_ParseUart(Line_t * line, UART_t * uart, uint32_t timeout)
{
	uint32_t start = CORE_GetTick();
	while (1)
	{
		uint32_t count = UART_ReadCount(uart);
		while (count--)
		{
			char * reply = Line_Parse(line, UART_Pop(uart));
			if (reply)
			{
				return reply;
			}
		}

		if (CORE_GetTick() - start >= timeout)
		{
			return NULL;
		}

		// Only perform the idle call if the timeout is non zero.
		CORE_Idle();
	}
}

/*
 * PRIVATE FUNCTIONS
 */

/*
 * INTERRUPT ROUTINES
 */

