
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

static bool NParse_FixedUint(const char ** str, uint32_t length, uint32_t * value);

/*
 * PUBLIC FUNCTIONS
 */

bool NParse_Kuint(const char ** str, uint32_t * value)
{
	uint32_t prefix;
	if (NParse_Uint(str, &prefix))
	{
		uint32_t power = 0;
		char ch = **str;
		if (ch == 'k' || ch == 'K')
		{
			power = 3;
		}
		else if (ch == 'm' || ch == 'M')
		{
			power = 6;
		}
		if (power > 0)
		{
			*str += 1;
			for (uint32_t p = 0; p < power; p++)
			{
				prefix *= 10;
			}

			uint32_t suffix;
			if (NParse_FixedUint(str, power, &suffix))
			{
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

static bool NParse_FixedUint(const char ** str, uint32_t length, uint32_t * value)
{
	const char * start = *str;
	if (!NParse_Uint(str, value))
	{
		return false;
	}

	uint32_t count = *str - start;
	while (count > length)
	{
		count -= 1;
		*value /= 10;
	}
	while (count < length)
	{
		count += 1;
		*value *= 10;
	}
	return true;
}


/*
 * INTERRUPT ROUTINES
 */
