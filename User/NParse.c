
#include "NParse.h"

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

bool NParse_Kuint(const char ** str, uint32_t * value)
{
	uint32_t prefix;
	if (NParse_Uint(str, &prefix))
	{
		uint32_t mul = 1;
		char ch = **str;
		if (ch == 'k' || ch == 'K')
		{
			mul = 1000;
		}
		else if (ch == 'm' || ch == 'M')
		{
			mul = 1000000;
		}
		if (mul > 1)
		{
			*str += 1;
			prefix *= mul;
			uint32_t suffix;
			if (NParse_Uint(str, &suffix))
			{
				// TODO: Fix this JANK
				prefix += suffix;
			}
		}

		*value = prefix;
		return true;
	}
	else
	{
		return false;
	}
}

bool NParse_Uint(const char ** str, uint32_t * value)
{
	uint32_t v = 0;
	const char * head = *str;
	while (1)
	{
		char ch = *head;
		if (ch >= '0' && ch <= '9')
		{
			v = (v * 10) + (ch - '0');
			head++;
		}
		else
		{
			break;
		}
	}
	*value = v;
	if (head > *str)
	{
		// Ensure we read at least 1 char
		*str = head;
		return true;
	}
	return false;
}

bool NParse_Bytes(const char ** str, uint8_t * value, uint32_t size, uint32_t * count)
{
	return false;
}

/*
 * PRIVATE FUNCTIONS
 */



/*
 * INTERRUPT ROUTINES
 */
