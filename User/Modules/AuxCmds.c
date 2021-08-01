
#include "AuxCmds.h"
#include "ComCmd.h"
#include "ConfigCmd.h"

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

static const Cmd_Node_t gVoutNode;
static const Cmd_Node_t gVrefNode;
static const Cmd_Node_t gTempNode;

/*
 * PUBLIC FUNCTIONS
 */

const Cmd_Node_t * AUXCMD_InitVref(void)
{
	return &gVrefNode;
}

const Cmd_Node_t * AUXCMD_InitVout(void)
{
	GPIO_EnableOutput(VOUT_EN_GPIO, VOUT_EN_PIN, !gConfig.default_vout);
	return &gVoutNode;
}

const Cmd_Node_t * AUXCMD_InitTemp(void)
{
	return &gTempNode;
}

/*
 * PRIVATE FUNCTIONS
 */

/*
 * FUNCTION NODES
 */

static void AUXCMD_Vref(Cmd_Line_t * line, Cmd_ArgValue_t * args)
{
	uint32_t vref = ADC_ReadVRef();
	Cmd_ReplyLevel_t level = Cmd_Reply_Info;
	if (vref < 2200 || vref > 3400)
	{
		level = Cmd_Reply_Warn;
	}
	Cmd_Printf(line, level, "%lumV\r\n", vref);
}

static const Cmd_Node_t gVrefNode = {
	.type = Cmd_Node_Function,
	.name = "vref",
	.func = {
		.arglen = 0,
		.callback = AUXCMD_Vref
	}
};

static void AUXCMD_Temp(Cmd_Line_t * line, Cmd_ArgValue_t * args)
{
	int32_t temp = ADC_ReadDieTemp();
	Cmd_ReplyLevel_t level = Cmd_Reply_Info;
	if (temp <= 0 || temp > 40)
	{
		level = Cmd_Reply_Warn;
	}
	Cmd_Printf(line, level, "%d degC\r\n", temp);
}

static const Cmd_Node_t gTempNode = {
	.type = Cmd_Node_Function,
	.name = "temp",
	.func = {
		.arglen = 0,
		.callback = AUXCMD_Temp
	}
};

static const Cmd_Arg_t gVoutArgs[] = {
	{
		.type = Cmd_Arg_Bool,
		.name = "enable",
	}
};

static void AUXCMD_Vout(Cmd_Line_t * line, Cmd_ArgValue_t * args)
{
	bool enable = args[0].boolean;
	GPIO_Write(VOUT_EN_GPIO, VOUT_EN_PIN, !enable);
	COMCMD_PrintOk(line);
}

static const Cmd_Node_t gVoutNode = {
	.type = Cmd_Node_Function,
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
