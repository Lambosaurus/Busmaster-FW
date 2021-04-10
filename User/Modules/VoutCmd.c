
#include "VoutCmd.h"
#include "ComCmd.h"
#include "GPIO.h"

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

static const CmdNode_t gVoutNode;

/*
 * PUBLIC FUNCTIONS
 */

const CmdNode_t * VOUTCMD_InitMenu(void)
{
	GPIO_EnableOutput(VOUT_EN_GPIO, VOUT_EN_PIN, GPIO_PIN_SET);
	return &gVoutNode;
}

/*
 * PRIVATE FUNCTIONS
 */

/*
 * FUNCTION NODES
 */

static const CmdArg_t gVoutArgs[] = {
	{
		.type = CmdArg_Bool,
		.name = "enable",
	}
};

static void VOUTCMD_Vout(CmdLine_t * line, CmdArgValue_t * args)
{
	bool enable = args[0].boolean;
	GPIO_Write(VOUT_EN_GPIO, VOUT_EN_PIN, !enable);
	COMCMD_PrintOk(line);
}

static const CmdNode_t gVoutNode = {
	.type = CmdNode_Function,
	.name = "vout",
	.func = {
		.arglen = LENGTH(gVoutArgs),
		.args = gVoutArgs,
		.callback = VOUTCMD_Vout
	}
};

/*
 * INTERRUPT ROUTINES
 */
