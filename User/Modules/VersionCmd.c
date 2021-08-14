
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

static const Cmd_Node_t gVersionNode;

/*
 * PUBLIC FUNCTIONS
 */

const Cmd_Node_t * VERSIONCMD_InitMenu(void)
{
	return &gVersionNode;
}

/*
 * PRIVATE FUNCTIONS
 */

/*
 * FUNCTION NODES
 */

static void VERSIONCMD_PrintVersion(Cmd_Line_t * line, Cmd_ArgValue_t * args)
{
	Cmd_Prints(line, Cmd_Reply_Info, "Busmaster v0.10\r\n");
}

static const Cmd_Node_t gVersionNode = {
	.type = Cmd_Node_Function,
	.name = "version",
	.func = {
		.arglen = 0,
		.callback = VERSIONCMD_PrintVersion
	}
};

/*
 * INTERRUPT ROUTINES
 */
