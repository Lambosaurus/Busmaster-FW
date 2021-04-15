
#include "ComCmd.h"

#include "NParse.h"
#include <stdio.h>
#include "ConfigCmd.h"

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
		char delimiter = gConfig.byte_delimiter;

		if (delimiter == '\'' || delimiter == '\"')
		{
			uint32_t size = (count * 4) + 12;
			char * bfr = Cmd_Malloc(line, size);
			uint32_t written = 0;
			written += snprintf(bfr, size, "read: %c", delimiter);
			written += NFormat_String(bfr + written, size - written, data, count, delimiter);
			written += snprintf(bfr + written, size - written, "%c\r\n", delimiter);
			Cmd_Print(line, CmdReply_Info, bfr, written);
			Cmd_Free(line, bfr);
		}
		else
		{
			char space = gConfig.byte_space;
			uint32_t size = (count * (space ? 3 : 2)) + 12;
			char * bfr = Cmd_Malloc(line, size);
			uint32_t written = 0;

			written += snprintf(bfr, size, "read: ");
			if (delimiter)
			{
				bfr[written++] = delimiter;
			}
			written += NFormat_Hex(bfr + written, data, count, space);
			if (delimiter)
			{
				// Close braces are always 2 chars after the open.
				bfr[written++] = delimiter + 2;
			}
			written += snprintf(bfr + written, size - written, "\r\n");
			Cmd_Print(line, CmdReply_Info, bfr, written);
			Cmd_Free(line, bfr);
		}
	}
	else
	{
		Cmd_Prints(line, CmdReply_Info, "read: 0 bytes\r\n");
	}
}

void COMCMD_PrintOk(CmdLine_t * line)
{
	Cmd_Prints(line, CmdReply_Info, "ok\r\n");
}

void COMCMD_PrintError(CmdLine_t * line)
{
	Cmd_Prints(line, CmdReply_Error, "error\r\n");
}

void COMCMD_PrintWritten(CmdLine_t * line, uint32_t count)
{
	Cmd_Printf(line, CmdReply_Info, "%d bytes written\r\n", count);
}

void COMCMD_PrintNoInit(CmdLine_t * line, const char * name)
{
	Cmd_Printf(line, CmdReply_Error, "%s not initialised\r\n", name);
}


/*
 * PRIVATE FUNCTIONS
 */

/*
 * INTERRUPT ROUTINES
 */
