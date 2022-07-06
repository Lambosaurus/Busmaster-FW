#include "BC660.h"

#include "CORE.h"
#include "UART.h"
#include "Line.h"

#include <stdarg.h>
#include <stdio.h>

/*
 * PRIVATE DEFINITIONS
 */

#define BC660_RETRIES	3

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

static bool BC660_Wake(void);
static bool BC660_Set(const char * cmd, uint32_t timeout);
static bool BC660_Get(const char * cmd, uint32_t timeout, const char * fmt, ...);

static void BC660_SendAtCommand(const char * cmd);
static void BC660_FlushLines(void);
static void BC660_HandleURC(const char * reply);
static const char * BC660_ReadLine(uint32_t timeout, const char * prefix);

static bool BC660_IsSimReady(void);
static bool BC660_IsRegisteredToNetwork(void);
static bool BC660_IsPacketServiceAttached(void);

/*
 * PRIVATE VARIABLES
 */

static struct {
	struct {
		Line_t line;
		char bfr[128];
	} rx;
} gBC660;

/*
 * PUBLIC FUNCTIONS
 */

bool BC660_Init(void)
{
	UART_Init(BC660_UART, 115200, UART_Mode_Default);
	Line_Init(&gBC660.rx.line, gBC660.rx.bfr, sizeof(gBC660.rx.bfr));

	bool success = BC660_Set("E0", 250); // Disable echo.
	success &= BC660_IsSimReady();

	return success;
}

void BC660_Deinit(void)
{
	UART_Deinit(BC660_UART);
}

bool BC660_IsConnected(void)
{
	return BC660_IsRegisteredToNetwork();
}

uint8_t BC660_GetSignalQuality(void)
{
	return 0;
}

bool BC660_UDP_Open(const char * addr, uint16_t port)
{
	return false;
}

bool BC660_UDP_Write(const uint8_t * data, uint32_t size)
{
	return false;
}

uint32_t BC660_UDP_Read(uint8_t * data, uint32_t size)
{
	return false;
}

bool BC660_UDP_Close(void)
{
	return false;
}

/*
 * PRIVATE FUNCTIONS
 */

static bool BC660_IsSimReady(void)
{
	char cpin_status[6];
	if (BC660_Get("+CPIN?", 250, "+CPIN: %5s", cpin_status))
	{
		return strcmp(cpin_status, "READY") == 0;
	}
	return false;
}

static bool BC660_IsRegisteredToNetwork(void)
{
	int urc_en, status;
	if (BC660_Get("+CREG?", 250, "+CREG: %d,%d", &urc_en, &status))
	{
		return status == 1;
	}
	return false;
}

static bool BC660_IsPacketServiceAttached(void)
{
	int status;
	if (BC660_Get("+CGATT?", 250, "+CGATT: %d", &status))
	{
		return status == 1;
	}
	return false;
}

static void BC660_HandleURC(const char * reply)
{
	if (strncmp(reply, "+SGACT: ", 8) == 0)
	{
		__BKPT();
	}
}


// Ensures the BC660 is awake for subsequent commands.
static bool BC660_Wake(void)
{
	BC660_FlushLines();
	UART_WriteStr(BC660_UART, "\r\n");
	// Send an plain AT, and expect an OK.
	return BC660_Set("", 250);
}

// A simple AT command, expecting an OK or ERROR response.
static bool BC660_Set(const char * cmd, uint32_t timeout)
{
	for (uint32_t i = 0; i < BC660_RETRIES; i++)
	{
		BC660_SendAtCommand(cmd);

		const char * reply = BC660_ReadLine(250, NULL);
		if (strcmp(reply, "OK") == 0)
		{
			return true;
		}
	}
	return false;
}

static const char * BC660_FindChar(const char * str, char ch)
{
	while (*str != 0 && *str != ch)
	{
		str++;
	}
	return str;
}

// Finds the constant prefix from a format string.
// "+CREG: %d,%d" -> "+CREG: "
static void BC660_GetPrefix(const char * fmt, char * bfr, uint32_t bfr_size)
{
	// Search for the first '%' symbol (or null terminator)
	const char * end = BC660_FindChar(fmt, '%');

	// Copy into a new buffer and null terminate it.
	uint32_t size = end - fmt;
	if (size >= bfr_size) { size = bfr_size - 1; }
	memcpy(bfr, fmt, size);
	bfr[size] = 0;
}

// Counts the number of arguments in a format string
// "+CREG: %d,%d" -> 2
static uint32_t BC660_CountArgs(const char * fmt)
{
	uint32_t count = 0;
	while (1)
	{
		fmt = BC660_FindChar(fmt, '%');
		if (*fmt == 0)
		{
			// end of string
			return count;
		}
		else // *fmt == '%'
		{
			fmt++;
			if (*fmt == '%')
			{
				// "%%" should not be counted.
				fmt++;
			}
			else
			{
				count++;
			}
		}
	}
}

static bool BC660_Get(const char * cmd, uint32_t timeout, const char * fmt, ...)
{
	char prefix[16];
	BC660_GetPrefix(fmt, prefix, sizeof(prefix));
	int fmt_args = BC660_CountArgs(fmt);

	for (uint32_t i = 0; i < BC660_RETRIES; i++)
	{
		uint32_t end = CORE_GetTick() + timeout;
		// Send the command
		BC660_SendAtCommand(cmd);

		// First read should hopefully get us a line to scanf.
		const char * reply = BC660_ReadLine(timeout, prefix);
		int parsed = -1;

		if (reply == NULL)
		{
			continue;
		}
		else
		{
			// Attempt to parse it.
			va_list va;
			va_start(va, fmt);
			parsed = vsscanf(reply, fmt, va);
			va_end(va);
		}

		int32_t remaining = end - CORE_GetTick();
		if (remaining < 0) { remaining = 0; }

		// Now look for an OK.
		reply = BC660_ReadLine(remaining, prefix);

		if (reply == NULL)
		{
			continue;
		}
		else if (strcmp(reply, "OK") == 0)
		{
			return parsed == fmt_args;
		}
		else
		{
			continue;
		}
	}
	return false;
}

static void BC660_SendAtCommand(const char * cmd)
{
	BC660_FlushLines();
	UART_WriteStr(BC660_UART, "AT");
	UART_WriteStr(BC660_UART, cmd);
	UART_WriteStr(BC660_UART, "\r\n");
}

// Reads a line, not exceeding the given timeout.
// Blank lines and URC's will be filtered out
// A string prefix can be supplied to permit specific URC's as responses.
static const char * BC660_ReadLine(uint32_t timeout, const char * prefix)
{
	uint32_t prefix_len = prefix == NULL ? 0 : strlen(prefix);
	uint32_t end = CORE_GetTick() + timeout;
	while (1)
	{
		int32_t remaining = end - CORE_GetTick();
		if (remaining < 0) { remaining = 0; }

		const char * reply = Line_ParseUart(&gBC660.rx.line, BC660_UART, remaining);

		if (reply == NULL)
		{
			// Timeout
			break;
		}
		else if (reply[0] == 0)
		{
			// strlen(reply) == 0. Ignore.
			continue;
		}
		else if (reply[0] == '+')
		{
			// This is a URC. Perhaps the user wants it?
			if (prefix != NULL && strncmp(reply, prefix, prefix_len) == 0)
			{
				return reply;
			}
			else
			{
				BC660_HandleURC(reply);
			}
			continue;
		}
		else
		{
			// A valid non URC line. Return it to the user.
			return reply;
		}
	}
	return NULL;
}

static void BC660_FlushLines(void)
{
	// Due to an error, we may have some queued OK's or ERROR's.
	// These will confuse the next command - so clear them out now.
	uint32_t start = CORE_GetTick();
	while (CORE_GetTick() - start < 50)
	{
		// Read until we consume all characters.
		const char * reply = BC660_ReadLine(0, NULL);
		if (reply == NULL)
		{
			break;
		}
	}
	return;
}

/*
 * INTERRUPT ROUTINES
 */
