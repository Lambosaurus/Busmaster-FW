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

// General commands
static bool BC660_Wake(void);
static bool BC660_Set(const char * cmd, uint32_t timeout);
static bool BC660_Get(const char * cmd, uint32_t timeout, const char * fmt, ...);
static bool BC660_Scan(uint32_t timeout, const char * fmt, ...);
static bool BC660_Scan_VA(uint32_t timeout, const char * fmt, va_list va);

// Wrappers for specific commands
static bool BC660_IsSimReady(void);
static bool BC660_IsRegisteredToNetwork(void);
static bool BC660_IsPacketServiceAttached(void);
static bool BC660_IsIpAssigned(void);
static bool BC660_ReadSignalQuality(uint8_t * csq);

// Utility functions for line handlings.
static void BC660_SendCommand(const char * cmd);
static void BC660_FlushLines(void);
static void BC660_HandleURC(const char * reply);
static const char * BC660_ReadLine(uint32_t timeout, const char * prefix);


// Utility functions for string parsing.
static const char * BC660_FindChar(const char * str, char ch);
static void BC660_GetPrefix(const char * fmt, char * bfr, uint32_t bfr_size);
static uint32_t BC660_CountArgs(const char * fmt);


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

	// Disable echo.
	bool success = BC660_Set("E0", 250);

	success &= BC660_IsSimReady();

	// Put it into binary mode.
	success &= BC660_Set("+QICFG=\"dataformat\",1,1", 500);

	return success;
}

void BC660_Deinit(void)
{
	UART_Deinit(BC660_UART);
}

bool BC660_IsConnected(void)
{
	return BC660_Wake()
		&& BC660_IsPacketServiceAttached()
		&& BC660_IsIpAssigned();
}

uint8_t BC660_GetSignalQuality(void)
{
	uint8_t pct;
	if (BC660_Wake() && BC660_ReadSignalQuality(&pct))
	{
		return pct;
	}
	return 0;
}

bool BC660_UDP_Open(const char * addr, uint16_t remote_port, uint16_t local_port)
{
	if (BC660_Wake())
	{
		char bfr[256];
		snprintf(bfr, sizeof(bfr), "+QIOPEN=0,1,\"UDP\",\"%s\",%d,%d", addr, (int)remote_port, (int)local_port);
		if (BC660_Set(bfr, 500))
		{
			// Command will return OK immediately.

			// +QIOPEN: 0,0
			int id, err;
			if (BC660_Scan(10000, "+QIOPEN: %d,%d", &id, &err))
			{
				if (id == 1 && err == 0)
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool BC660_UDP_Write(const uint8_t * data, uint32_t size)
{
	return true;
}

uint32_t BC660_UDP_Read(uint8_t * data, uint32_t size)
{
	return true;
}

bool BC660_UDP_Close(void)
{
	if (BC660_Wake())
	{
		if (BC660_Set("+QICLOSE=1", 250) && BC660_Scan(1000, "CLOSE OK"))
		{
			return true;
		}
	}
	return false;
}

/*
AT+QICFG="dataformat",1,1
OK
AT+QIOPEN=0,0,"UDP","ec2-3-26-186-18.ap-southeast-2.compute.amazonaws.com",11001
OK
+QIOPEN: 0,0
AT+QISEND=0,4,"01020304"
OK
SEND OK
+QIURC: "recv",0,2,"4f4b"
AT+QICLOSE=1
OK
CLOSE OK
*/

/*
 * PRIVATE FUNCTIONS
 */

static bool BC660_ReadSignalQuality(uint8_t * pct)
{
	int csq, ber;
	if (BC660_Get("+CSQ", 250, "+CSQ: %d,%d", &csq, &ber))
	{
		*pct = csq * 100 / 31;
		return true;
	}
	return false;
}

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

static bool BC660_IsIpAssigned(void)
{
	int cid;
	char addr[32];
	if (BC660_Get("+CGPADDR=0", 500, "+CGPADDR: %d,\"%31s", &cid, addr))
	{
		// +CGPADDR: 0,"100.68.236.28"

		const char * end = BC660_FindChar(addr, '"');
		return (end - addr) > 4; // Check the address had a sensible length.
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
		BC660_SendCommand(cmd);

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


static bool BC660_Scan(uint32_t timeout, const char * fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	bool success = BC660_Scan_VA(timeout, fmt, va);
	va_end(va);
	return success;
}

static bool BC660_Scan_VA(uint32_t timeout, const char * fmt, va_list va)
{
	// Find the constant component of the string.
	// This will be used to select the correct string for matching.
	char prefix[16];
	BC660_GetPrefix(fmt, prefix, sizeof(prefix));

	const char * reply = BC660_ReadLine(timeout, prefix);

	if (reply != NULL)
	{
		int argc = BC660_CountArgs(fmt);

		if (argc > 0)
		{
			// Attempt parse the args.
			return vsscanf(reply, fmt, va) == argc;
		}
		else
		{
			// No args. Test for equality.
			return strcmp(reply, fmt) == 0;
		}
	}

	return false;
}

static bool BC660_Get(const char * cmd, uint32_t timeout, const char * fmt, ...)
{
	for (uint32_t i = 0; i < BC660_RETRIES; i++)
	{
		uint32_t end = CORE_GetTick() + timeout;
		// Send the command
		BC660_SendCommand(cmd);

		va_list va;
		va_start(va, fmt);
		bool parse_ok = BC660_Scan_VA(timeout, fmt, va);
		va_end(va);

		int32_t remaining = end - CORE_GetTick();
		if (remaining < 0) { remaining = 0; }

		// Now look for an OK.
		const char * reply = BC660_ReadLine(remaining, NULL);

		if (reply == NULL)
		{
			continue;
		}
		else if (strcmp(reply, "OK") == 0)
		{
			return parse_ok;
		}
		else
		{
			continue;
		}
	}
	return false;
}

static void BC660_SendCommand(const char * cmd)
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
		else if (prefix != NULL && strncmp(reply, prefix, prefix_len) == 0)
		{
			// Perhaps the user wants this reply?
			return reply;
		}
		else if (reply[0] == '+')
		{
			// This is a URC. Handle it.
			BC660_HandleURC(reply);
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
