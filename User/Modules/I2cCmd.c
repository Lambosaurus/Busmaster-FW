
#include "I2cCmd.h"

#include "MCP425.h"
#include "I2C.h"

/*
 * PRIVATE DEFINITIONS
 */

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

static void I2CCMD_Init(CmdLine_t * line, CmdArgValue_t * argv);
static void I2CCMD_Deinit(CmdLine_t * line, CmdArgValue_t * argv);
static void I2CCMD_Scan(CmdLine_t * line, CmdArgValue_t * argv);

static void I2CCMD_SetPullups(uint32_t r);

/*
 * PRIVATE VARIABLES
 */

static bool gI2cEnabled;

static CmdArg_t gI2cInitArgs[] = {
	{
		.name = "speed",
		.type = CmdArg_Number,
	},
	{
		.name = "pullup",
		.type = CmdArg_Number,
	}
};

static CmdNode_t gI2cFunctions[] = {
	{
		.type = CmdNode_Function,
		.name = "init",
		.func = {
			.args = gI2cInitArgs,
			.arglen = LENGTH(gI2cInitArgs),
			.callback = I2CCMD_Init,
		}
	},
	{
		.type = CmdNode_Function,
		.name = "deinit",
		.func = {
			.arglen = 0,
			.callback = I2CCMD_Deinit,
		}
	},
	{
		.type = CmdNode_Function,
		.name = "scan",
		.func = {
			.arglen = 0,
			.callback = I2CCMD_Scan,
		}
	}
};

/*
 * PUBLIC FUNCTIONS
 */

void I2CCMD_InitMenu(CmdNode_t * menu)
{
	menu->type = CmdNode_Menu;
	menu->name = "i2c";
	menu->menu.count = LENGTH(gI2cFunctions);
	menu->menu.nodes = gI2cFunctions;

	MCP425_Init();
	I2CCMD_SetPullups(0);
	gI2cEnabled = false;
}

/*
 * PRIVATE FUNCTIONS
 */

static void I2CCMD_Init(CmdLine_t * line, CmdArgValue_t * argv)
{
	uint32_t speed = argv[0].number;
	uint32_t pullup = argv[1].number;
	I2CCMD_SetPullups(pullup);

	if (gI2cEnabled)
	{
		I2C_Deinit(BUS_I2C);
	}
	I2C_Init(BUS_I2C, speed);
	gI2cEnabled = true;

	uint32_t actual_speed = BUS_I2C->mode;
	if (actual_speed != speed)
	{
		Cmd_Printf(line, "i2c speed truncated to %d\r\n", actual_speed);
	}
}

static void I2CCMD_Deinit(CmdLine_t * line, CmdArgValue_t * argv)
{
	I2CCMD_SetPullups(0);

	if (gI2cEnabled)
	{
		I2C_Deinit(BUS_I2C);
		gI2cEnabled = false;
	}
}

static void I2CCMD_Scan(CmdLine_t * line, CmdArgValue_t * argv)
{
	if (!gI2cEnabled)
	{
		Cmd_Printf(line, "i2c not initialised.\r\n");
		return;
	}

	uint32_t found = 0;
	for (uint8_t i = 1; i < 127; i++)
	{
		if (I2C_Scan(BUS_I2C, i))
		{
			found += 1;
			Cmd_Printf(line, "device found on 0x%02X\r\n", i);
		}
	}
	Cmd_Printf(line, "%d devices found.\r\n", found);
}

static void I2CCMD_SetPullups(uint32_t r)
{
	if (r > 10000)
	{
		r = 10000;
	}

	if (r == 0)
	{
		// Disconnect wipers
		MCP425_SetTerminals(0, MCP425_T_None);
		MCP425_SetTerminals(1, MCP425_T_None);
	}
	else
	{
		MCP425_SetResistance(0, r);
		MCP425_SetResistance(1, r);
		MCP425_SetTerminals(0, MCP425_T_W | MCP425_T_B);
		MCP425_SetTerminals(1, MCP425_T_W | MCP425_T_B);
	}
}

/*
 * INTERRUPT ROUTINES
 */
