
#include "NParse.h"
#include <stdio.h>

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
static char Nparse_Lowchar(char ch);
static bool NParse_Nibble(char ch, uint32_t * n);

/*
 * PUBLIC FUNCTIONS
 */

static bool NParse_HexPrefix(const char ** str)
{
	const char * start = *str;
	if (*start == '0')
	{
		// We want to allow 0x or 0h prefix.
		start += 1;
	}
	char pfx = Nparse_Lowchar(*start);
	if (pfx == 'x' || pfx == 'h')
	{
		*str = start+1;
		return true;
	}
	return false;
}

bool NParse_Kuint(const char ** str, uint32_t * value)
{
	if (NParse_HexPrefix(str))
	{
		return NParse_Hex(str, value);
	}

	uint32_t prefix;
	if (NParse_Uint(str, &prefix))
	{
		uint32_t power = 0;
		char ch = Nparse_Lowchar(**str);
		if (ch == 'k')
		{
			power = 3;
		}
		else if (ch == 'm')
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
	if (head > *str)
	{
		// Ensure we read at least 1 char
		*value = v;
		*str = head;
		return true;
	}
	return false;
}

bool NParse_Hex(const char ** str, uint32_t * value)
{
	uint32_t v = 0;
	const char * head = *str;
	while (1)
	{
		uint32_t d;
		if (!NParse_Nibble(*head, &d))
		{
			break;
		}
		head++;
		v = (v << 4) | d;
	}
	if (head > *str)
	{
		// Ensure we read at least 1 char
		*value = v;
		*str = head;
		return true;
	}
	return false;
}

bool NParse_Byte(const char ** str, uint8_t * value)
{
	const char * head = *str;

	uint32_t high;
	uint32_t low;
	if (NParse_Nibble(*head++, &high) && NParse_Nibble(*head++, &low))
	{
		*str = head;
		*value = (high << 4) | low;
		return true;
	}
	return false;
}

bool NParse_Bytes(const char ** str, uint8_t * value, uint32_t size, uint32_t * count)
{
	uint32_t n = 0;
	while ( NParse_Byte(str, value++) )
	{
		n++;
		if (n >= size)
		{
			break;
		}
		char next = **str;
		if (next == '-' || next == ':' || next == ',' || next == ' ')
		{
			// Bytes may use these as delimiters.
			(*str)++;
		}
	}
	*count = n;
	return true;
}

static const char gEscCharmap[] = "abtnvfr";

bool NParse_String(const char ** str, char * value, uint32_t size, uint32_t * count)
{
	bool escaped = false;
	uint32_t written;
	const char * head = *str;
	for (written = 0; written < size - 1;)
	{
		char ch = *head++;
		if (ch == 0)
		{
			head--;
			break;
		}
		else if (escaped)
		{
			bool esc_found = false;
			for (uint32_t i = 0; i < sizeof(gEscCharmap); i++)
			{
				if (ch == gEscCharmap[i])
				{
					ch = '\a' + i;
					esc_found = true;
					break;
				}
			}
			if (!esc_found)
			{
				// We did not match onto the esc table
				if (ch == 'x')
				{
					if (!NParse_Byte(&head, (uint8_t*)&ch))
					{
						return false;
					}
				}
				// Otherwise we just interpret it as literal.
				// Other chars (', ", \, ?) can get their literal interpretation.
			}
			escaped = false;
			*value++ = ch;
			written++;
		}
		else if (ch == '\\')
		{
			escaped = true;
		}
		else
		{
			*value++ = ch;
			written++;
		}
	}
	*value = 0; // Enforce null char.
	*str = head;
	*count = written;
	return true;
}

uint32_t NFormat_String(char * str, uint32_t size, uint8_t * data, uint32_t count, char delimiter)
{
	char * end = str + size;
	while (count-- && str < end)
	{
		char ch = (char)*data++;

		if (ch == delimiter || ch == '\\' || (ch >= '\a' && ch <= '\r'))
		{
			if (end - str <= 2)
			{
				break;
			}
			*str++ = '\\';
			if (ch != delimiter && ch != '\\')
			{
				ch = gEscCharmap[ch - '\a'];
			}
			*str++ = ch;
		}
		else if (ch >= ' ' && ch <= '~')
		{
			*str++ = ch;
		}
		else
		{
			if (end - str <= 4)
			{
				break;
			}
			str += sprintf(str, "\\x%02X", ch);
		}
	}
	*str = 0;
	return str - (end - size);
}

uint32_t NFormat_Hex(char * str, uint8_t * hex, uint32_t count)
{
	char * start = str;
	while(count--)
	{
		str += sprintf(str, "%02X", *hex++);
	}
	*str = 0;
	return str - start;
}

/*
 * PRIVATE FUNCTIONS
 */

static bool NParse_Nibble(char ch, uint32_t * n)
{
	ch = Nparse_Lowchar(ch);
	if (ch >= '0' && ch <= '9')
	{
		*n = ch - '0';
		return true;
	}
	else if (ch >= 'a' && ch <= 'f')
	{
		*n = ch - 'a' + 10;
		return true;
	}
	return false;
}

static char Nparse_Lowchar(char ch)
{
	if (ch >= 'A' && ch <= 'Z')
	{
		return ch - 'A' + 'a';
	}
	return ch;
}

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
