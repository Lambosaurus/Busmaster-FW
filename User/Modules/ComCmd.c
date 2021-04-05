
#include "ComCmd.h"

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

/*
 * PUBLIC FUNCTIONS
 */

void COMCMD_PrintRead(CmdLine_t * line, uint8_t * data, uint32_t count)
{
	if (count > 0)
	{
		//*
		char * bfr = Cmd_Malloc(line, count*2 + 2);
		NFormat_Hex(bfr, data, count);
		Cmd_Printf(line, "read: %s\r\n", bfr);
		Cmd_Free(line, bfr);
		/*/
		char delimiter = '"';
		uint32_t size = count * 4 + 2;
		char * bfr = Cmd_Malloc(line, size);
		uint32_t written = 0;
		written += snprintf(bfr, size, "read: %c", delimiter);
		written += NFormat_String(bfr + written, size - written, data, count, delimiter);
		written += snprintf(bfr + written, size - written, "%c\r\n", delimiter);
		line->print((uint8_t *)bfr, written);
		Cmd_Free(line, bfr);
		//*/
	}
	else
	{
		Cmd_Printf(line, "read: 0 bytes\r\n");
	}
}

void COMCMD_PrintOk(CmdLine_t * line)
{
	Cmd_Printf(line, "ok\r\n");
}

void COMCMD_PrintError(CmdLine_t * line)
{
	Cmd_Printf(line, "error\r\n");
}

void COMCMD_PrintWritten(CmdLine_t * line, uint32_t count)
{
	Cmd_Printf(line, "%d bytes written\r\n", count);
}

void COMCMD_PrintNoInit(CmdLine_t * line, const char * name)
{
	Cmd_Printf(line, "%s not initialised\r\n", name);
}


/*
 * PRIVATE FUNCTIONS
 */

/*
 * INTERRUPT ROUTINES
 */
