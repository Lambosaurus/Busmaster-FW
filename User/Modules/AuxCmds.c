
#include "AuxCmds.h"
#include "ComCmd.h"

#include "ADC.h"
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
static const CmdNode_t gVrefNode;
static const CmdNode_t gTempNode;

/*
 * PUBLIC FUNCTIONS
 */

const CmdNode_t * AUXCMD_InitVref(void)
{
	return &gVrefNode;
}

const CmdNode_t * AUXCMD_InitVout(void)
{
	GPIO_EnableOutput(VOUT_EN_GPIO, VOUT_EN_PIN, GPIO_PIN_SET);
	return &gVoutNode;
}

const CmdNode_t * AUXCMD_InitTemp(void)
{
	return &gTempNode;
}

/*
 * PRIVATE FUNCTIONS
 */

/*
 * FUNCTION NODES
 */

static void AUXCMD_Vref(CmdLine_t * line, CmdArgValue_t * args)
{
	uint32_t vref = ADC_ReadVRef();
	CmdReplyLevel_t level = CmdReply_Info;
	if (vref < 2200 || vref > 3400)
	{
		level = CmdReply_Warn;
	}
	Cmd_Printf(line, level, "%lumV\r\n", vref);
}

static const CmdNode_t gVrefNode = {
	.type = CmdNode_Function,
	.name = "vref",
	.func = {
		.arglen = 0,
		.callback = AUXCMD_Vref
	}
};

static void AUXCMD_Temp(CmdLine_t * line, CmdArgValue_t * args)
{
	int32_t temp = ADC_ReadDieTemp();
	CmdReplyLevel_t level = CmdReply_Info;
	if (temp <= 0 || temp > 40)
	{
		level = CmdReply_Warn;
	}
	Cmd_Printf(line, level, "%d degC\r\n", temp);
}

static const CmdNode_t gTempNode = {
	.type = CmdNode_Function,
	.name = "temp",
	.func = {
		.arglen = 0,
		.callback = AUXCMD_Temp
	}
};

static const CmdArg_t gVoutArgs[] = {
	{
		.type = CmdArg_Bool,
		.name = "enable",
	}
};

static void AUXCMD_Vout(CmdLine_t * line, CmdArgValue_t * args)
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
		.callback = AUXCMD_Vout
	}
};

/*
 * INTERRUPT ROUTINES
 */
