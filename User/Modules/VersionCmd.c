
#include "VersionCmd.h"
#include "ComCmd.h"

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

static const CmdNode_t gVersionNode;

/*
 * PUBLIC FUNCTIONS
 */

const CmdNode_t * VERSIONCMD_InitMenu(void)
{
	return &gVersionNode;
}

/*
 * PRIVATE FUNCTIONS
 */

/*
 * FUNCTION NODES
 */

static void VERSIONCMD_PrintVersion(CmdLine_t * line, CmdArgValue_t * args)
{
	Cmd_Printf(line, "Busmaster v0.2\r\n");
}

static const CmdNode_t gVersionNode = {
	.type = CmdNode_Function,
	.name = "version",
	.func = {
		.arglen = 0,
		.callback = VERSIONCMD_PrintVersion
	}
};

/*
 * INTERRUPT ROUTINES
 */
