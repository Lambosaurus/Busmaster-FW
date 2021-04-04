
#include "ComCmd.h"

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

void COMCMD_PrintRead(CmdLine_t * line, uint8_t * data, uint32_t count)
{
	char * bfr = Cmd_Malloc(line, count*2 + 2);
	NFormat_Hex(bfr, data, count);
	Cmd_Printf(line, "read: %s\r\n", bfr);
	Cmd_Free(line, bfr);
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
