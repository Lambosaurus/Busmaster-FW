
#include "I2cCmd.h"

#include "MCP425.h"
#include "I2C.h"
#include "ComCmd.h"

/*
 * PRIVATE DEFINITIONS
 */

#define I2C_RX_MAX		256

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

static void I2CCMD_SetPullups(uint32_t r);

/*
 * PRIVATE VARIABLES
 */

static bool gI2cEnabled;
const static Cmd_Node_t gI2cMenu;

/*
 * PUBLIC FUNCTIONS
 */

const Cmd_Node_t * I2CCMD_InitMenu(void)
{
	MCP425_Init();
	I2CCMD_SetPullups(0);
	gI2cEnabled = false;
	return &gI2cMenu;
}

/*
 * PRIVATE FUNCTIONS
 */

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
 * FUNCTION NODES
 */

static const Cmd_Arg_t gI2cInitArgs[] = {
	{
		.name = "speed",
		.type = Cmd_Arg_Number,
	},
	{
		.name = "pullup",
		.type = Cmd_Arg_Number | Cmd_Arg_Optional,
	}
};

static void I2CCMD_Init(Cmd_Line_t * line, Cmd_ArgValue_t * argv)
{
	uint32_t speed = argv[0].number;
	uint32_t pullup = argv[1].present ? argv[1].number : 0;

	if (pullup != 0)
	{
		if (pullup < 1000)
		{
			pullup = 1000;
			COMCMD_PrintTruncation(line, "pullup", pullup);
		}
		else if (pullup > 10000)
		{
			pullup = 10000;
			COMCMD_PrintTruncation(line, "pullup", pullup);
		}
	}
	I2CCMD_SetPullups(pullup);

	if (speed < 1000)
	{
		Cmd_Prints(line, Cmd_Reply_Warn, "Selected speed is low. The unit for speed is Hz, not KHz.\r\n");
	}

	if (gI2cEnabled)
	{
		I2C_Deinit(BUS_I2C);
	}
	I2C_Init(BUS_I2C, speed);
	gI2cEnabled = true;

	uint32_t actual_speed = BUS_I2C->mode;
	if (actual_speed != speed)
	{
		COMCMD_PrintTruncation(line, "speed", actual_speed);
	}
	COMCMD_PrintOk(line);
}

static const Cmd_Node_t gI2cInitNode = {
	.type = Cmd_Node_Function,
	.name = "init",
	.func = {
		.args = gI2cInitArgs,
		.arglen = LENGTH(gI2cInitArgs),
		.callback = I2CCMD_Init,
	}
};

static void I2CCMD_Deinit(Cmd_Line_t * line, Cmd_ArgValue_t * argv)
{
	I2CCMD_SetPullups(0);

	if (gI2cEnabled)
	{
		I2C_Deinit(BUS_I2C);
		gI2cEnabled = false;
	}
	COMCMD_PrintOk(line);
}

static const Cmd_Node_t gI2cDeinitNode = {
	.type = Cmd_Node_Function,
	.name = "deinit",
	.func = {
		.arglen = 0,
		.callback = I2CCMD_Deinit,
	}
};

static void I2CCMD_Scan(Cmd_Line_t * line, Cmd_ArgValue_t * argv)
{
	if (!gI2cEnabled)
	{
		COMCMD_PrintNoInit(line, "i2c");
		return;
	}

	uint32_t found = 0;
	for (uint8_t i = 1; i < 127; i++)
	{
		if (I2C_Scan(BUS_I2C, i))
		{
			found += 1;
			Cmd_Printf(line, Cmd_Reply_Info, "device found on 0x%02X\r\n", i);
		}
	}
	Cmd_Printf(line, Cmd_Reply_Info, "%d devices found.\r\n", found);
}

static const Cmd_Node_t gI2cScanNode = {
	.type = Cmd_Node_Function,
	.name = "scan",
	.func = {
		.arglen = 0,
		.callback = I2CCMD_Scan,
	}
};

static const Cmd_Arg_t gI2cWriteArgs[] = {
	{
		.name = "address",
		.type = Cmd_Arg_Number,
	},
	{
		.name = "payload",
		.type = Cmd_Arg_Bytes,
	}
};

static void I2CCMD_Write(Cmd_Line_t * line, Cmd_ArgValue_t * argv)
{
	if (!gI2cEnabled)
	{
		COMCMD_PrintNoInit(line, "i2c");
		return;
	}

	uint32_t address = argv[0].number;
	Cmd_ArgValue_t * txdata = &argv[1];
	if (address > 0x7F)
	{
		Cmd_Prints(line, Cmd_Reply_Error, "I2C address cannot exceed 0x7F\r\n");
		return;
	}

	if (I2C_Write(BUS_I2C, address, txdata->bytes.data, txdata->bytes.size))
	{
		COMCMD_PrintWritten(line, txdata->bytes.size);
	}
	else
	{
		COMCMD_PrintError(line);
	}
}

static const Cmd_Node_t gI2cWriteNode = {
	.type = Cmd_Node_Function,
	.name = "write",
	.func = {
		.args = gI2cWriteArgs,
		.arglen = LENGTH(gI2cWriteArgs),
		.callback = I2CCMD_Write,
	}
};

static const Cmd_Arg_t gI2cReadArgs[] = {
	{
		.name = "address",
		.type = Cmd_Arg_Number,
	},
	{
		.name = "count",
		.type = Cmd_Arg_Number,
	}
};

static void I2CCMD_Read(Cmd_Line_t * line, Cmd_ArgValue_t * argv)
{
	if (!gI2cEnabled)
	{
		COMCMD_PrintNoInit(line, "i2c");
		return;
	}

	uint32_t address = argv[0].number;
	uint32_t rxcount = argv[1].number;

	if (address > 0x7F)
	{
		Cmd_Prints(line, Cmd_Reply_Error, "I2C address cannot exceed 0x7F\r\n");
		return;
	}
	if (rxcount > I2C_RX_MAX)
	{
		rxcount = I2C_RX_MAX;
		COMCMD_PrintTruncation(line, "count", rxcount);
	}
	uint8_t * rxdata = Cmd_Malloc(line, rxcount);

	if (I2C_Read(BUS_I2C, address, rxdata, rxcount))
	{
		COMCMD_PrintRead(line, rxdata, rxcount);
	}
	else
	{
		COMCMD_PrintError(line);
	}
}

static const Cmd_Node_t gI2cReadNode = {
	.type = Cmd_Node_Function,
	.name = "read",
	.func = {
		.args = gI2cReadArgs,
		.arglen = LENGTH(gI2cReadArgs),
		.callback = I2CCMD_Read,
	}
};

static const Cmd_Arg_t gI2cTransferArgs[] = {
	{
		.name = "address",
		.type = Cmd_Arg_Number,
	},
	{
		.name = "payload",
		.type = Cmd_Arg_Bytes,
	},
	{
		.name = "count",
		.type = Cmd_Arg_Number,
	}
};

static void I2CCMD_Transfer(Cmd_Line_t * line, Cmd_ArgValue_t * argv)
{
	if (!gI2cEnabled)
	{
		COMCMD_PrintNoInit(line, "i2c");
		return;
	}

	uint32_t address = argv[0].number;
	Cmd_ArgValue_t * txdata = &argv[1];
	uint32_t rxcount = argv[2].number;

	if (address > 0x7F)
	{
		Cmd_Prints(line, Cmd_Reply_Error, "I2C address cannot exceed 0x7F\r\n");
		return;
	}
	if (rxcount > I2C_RX_MAX)
	{
		rxcount = I2C_RX_MAX;
		COMCMD_PrintTruncation(line, "count", rxcount);
	}
	uint8_t * rxdata = Cmd_Malloc(line, rxcount);

	if (I2C_Transfer(BUS_I2C, address, txdata->bytes.data, txdata->bytes.size, rxdata, rxcount))
	{
		COMCMD_PrintWritten(line, txdata->bytes.size);
		COMCMD_PrintRead(line, rxdata, rxcount);
	}
	else
	{
		COMCMD_PrintError(line);
	}
}

static const Cmd_Node_t gI2cTransferNode = {
	.type = Cmd_Node_Function,
	.name = "transfer",
	.func = {
		.args = gI2cTransferArgs,
		.arglen = LENGTH(gI2cTransferArgs),
		.callback = I2CCMD_Transfer,
	}
};

static const Cmd_Node_t * gI2cFunctions[] = {
	&gI2cInitNode,
	&gI2cDeinitNode,
	&gI2cScanNode,
	&gI2cWriteNode,
	&gI2cReadNode,
	&gI2cTransferNode,
};

static const Cmd_Node_t gI2cMenu = {
	.type = Cmd_Node_Menu,
	.name = "i2c",
	.menu = {
		.count = LENGTH(gI2cFunctions),
		.nodes = gI2cFunctions
	}
};

/*
 * INTERRUPT ROUTINES
 */
