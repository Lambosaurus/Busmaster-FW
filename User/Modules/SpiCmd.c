
#include "SpiCmd.h"
#include "SPI.h"
#include "GPIO.h"
#include "ComCmd.h"

/*
 * PRIVATE DEFINITIONS
 */

#define SPI_RX_MAX		256

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

static void SPICMD_CS_Select(void);
static void SPICMD_CS_Deselect(void);

/*
 * PRIVATE VARIABLES
 */

static bool gSpiEnabled;
static bool gSpiAutoSelect;
static const CmdNode_t gSpiMenu;

/*
 * PUBLIC FUNCTIONS
 */

const CmdNode_t * SPICMD_InitMenu(void)
{
	gSpiEnabled = false;
	gSpiAutoSelect = true;

	return &gSpiMenu;
}

/*
 * PRIVATE FUNCTIONS
 */


static void SPICMD_CS_Select(void)
{
	if (gSpiAutoSelect)
	{
		GPIO_Reset(SPI_CS_GPIO, SPI_CS_PIN);
	}
}

static void SPICMD_CS_Deselect(void)
{
	if (gSpiAutoSelect)
	{
		GPIO_Set(SPI_CS_GPIO, SPI_CS_PIN);
	}
}

/*
 * FUNCITON NODES
 */

static const CmdArg_t gSpiInitArgs[] = {
	{
		.name = "frequency",
		.type = CmdArg_Number,
	},
	{
		.name = "mode",
		.type = CmdArg_Number,
	}
};

static void SPICMD_Init(CmdLine_t * line, CmdArgValue_t * argv)
{
	uint32_t bitrate = argv[0].number;
	uint32_t moden = argv[1].number;

	if (gSpiEnabled)
	{
		SPI_Deinit(BUS_SPI);
	}

	SPIMode_t mode;
	switch (moden)
	{
	case 0:
		mode = SPI_MODE0;
		break;
	case 1:
		mode = SPI_MODE1;
		break;
	case 2:
		mode = SPI_MODE2;
		break;
	case 3:
		mode = SPI_MODE3;
		break;
	default:
		Cmd_Printf(line, CmdReply_Error, "spi mode must be 0-3\r\n");
		return;
	}

	SPI_Init(BUS_SPI, bitrate, mode);
	GPIO_EnableOutput(SPI_CS_GPIO, SPI_CS_PIN, GPIO_PIN_SET);
	gSpiEnabled = true;

	uint32_t actual_bitrate = BUS_SPI->bitrate;
	if (actual_bitrate != bitrate)
	{
		Cmd_Printf(line, CmdReply_Warn, "spi frequency truncated to %d\r\n", actual_bitrate);
	}
	COMCMD_PrintOk(line);
}

static const CmdNode_t gSpiInitNode = {
	.type = CmdNode_Function,
	.name = "init",
	.func = {
		.args = gSpiInitArgs,
		.arglen = LENGTH(gSpiInitArgs),
		.callback = SPICMD_Init,
	}
};

static void SPICMD_Deinit(CmdLine_t * line, CmdArgValue_t * argv)
{
	GPIO_Deinit(SPI_CS_GPIO, SPI_CS_PIN);
	if (gSpiEnabled)
	{
		SPI_Deinit(BUS_SPI);
		gSpiEnabled = false;
	}
	COMCMD_PrintOk(line);
}

static const CmdNode_t gSpiDeinitNode = {
	.type = CmdNode_Function,
	.name = "deinit",
	.func = {
		.arglen = 0,
		.callback = SPICMD_Deinit,
	}
};

static const CmdArg_t gSpiSelectArgs[] = {
	{
		.name = "enable",
		.type = CmdArg_Bool,
	}
};

static void SPICMD_Select(CmdLine_t * line, CmdArgValue_t * argv)
{
	uint32_t enable = argv[0].boolean;
	if (!gSpiEnabled)
	{
		COMCMD_PrintNoInit(line, "spi");
		return;
	}

	if (gSpiAutoSelect)
	{
		Cmd_Printf(line, CmdReply_Info, "auto select disabled\r\n");
		gSpiAutoSelect = false;
	}
	GPIO_Write(SPI_CS_GPIO, SPI_CS_PIN, !enable);
	COMCMD_PrintOk(line);
}

static const CmdNode_t gSpiSelectNode = {
	.type = CmdNode_Function,
	.name = "select",
	.func = {
		.arglen = LENGTH(gSpiSelectArgs),
		.args = gSpiSelectArgs,
		.callback = SPICMD_Select,
	}
};

static void SPICMD_AutoSelect(CmdLine_t * line, CmdArgValue_t * argv)
{
	if (!gSpiEnabled)
	{
		COMCMD_PrintNoInit(line, "spi");
		return;
	}
	gSpiAutoSelect = true;
	GPIO_Set(SPI_CS_GPIO, SPI_CS_PIN);
	Cmd_Printf(line, CmdReply_Info, "auto select enabled\r\n");
}

static const CmdNode_t gSpiAutoselectNode = {
	.type = CmdNode_Function,
	.name = "autoselect",
	.func = {
		.arglen = 0,
		.callback = SPICMD_AutoSelect,
	}
};

static const CmdArg_t gSpiWriteArgs[] = {
	{
		.name = "payload",
		.type = CmdArg_Bytes,
	}
};

static void SPICMD_Write(CmdLine_t * line, CmdArgValue_t * argv)
{
	if (!gSpiEnabled)
	{
		COMCMD_PrintNoInit(line, "spi");
		return;
	}

	CmdArgValue_t * data = &argv[0];

	SPICMD_CS_Select();
	SPI_Write(BUS_SPI, data->bytes.data, data->bytes.size);
	SPICMD_CS_Deselect();

	COMCMD_PrintWritten(line, data->bytes.size);
}

static const CmdNode_t gSpiWriteNode = {
	.type = CmdNode_Function,
	.name = "write",
	.func = {
		.args = gSpiWriteArgs,
		.arglen = LENGTH(gSpiWriteArgs),
		.callback = SPICMD_Write,
	}
};

static const CmdArg_t gSpiReadArgs[] = {
	{
		.name = "count",
		.type = CmdArg_Number,
	}
};

static void SPICMD_Read(CmdLine_t * line, CmdArgValue_t * argv)
{
	if (!gSpiEnabled)
	{
		COMCMD_PrintNoInit(line, "spi");
		return;
	}

	uint32_t count = argv[0].number;
	if (count > SPI_RX_MAX)
	{
		count = SPI_RX_MAX;
		Cmd_Printf(line, CmdReply_Warn, "count truncated to %d\r\n", count);
	}
	uint8_t * data = Cmd_Malloc(line, count);

	SPICMD_CS_Select();
	SPI_Read(BUS_SPI, data, count);
	SPICMD_CS_Deselect();

	COMCMD_PrintRead(line, data, count);
}

static const CmdNode_t gSpiReadNode = {
	.type = CmdNode_Function,
	.name = "read",
	.func = {
		.args = gSpiReadArgs,
		.arglen = LENGTH(gSpiReadArgs),
		.callback = SPICMD_Read,
	}
};

static const CmdArg_t gSpiTransferArgs[] = {
	{
		.name = "payload",
		.type = CmdArg_Bytes,
	}
};

static void SPICMD_Transfer(CmdLine_t * line, CmdArgValue_t * argv)
{
	if (!gSpiEnabled)
	{
		COMCMD_PrintNoInit(line, "spi");
		return;
	}

	CmdArgValue_t * data = &argv[0];

	// Write straight into the tx buffer.
	SPICMD_CS_Select();
	SPI_Transfer(BUS_SPI, data->bytes.data, data->bytes.data, data->bytes.size);
	SPICMD_CS_Deselect();

	COMCMD_PrintRead(line, data->bytes.data, data->bytes.size);
}

static const CmdNode_t gSpiTransferNode = {
	.type = CmdNode_Function,
	.name = "transfer",
	.func = {
		.args = gSpiTransferArgs,
		.arglen = LENGTH(gSpiTransferArgs),
		.callback = SPICMD_Transfer,
	}
};

static const CmdNode_t * gSpiFunctions[] = {
		&gSpiInitNode,
		&gSpiDeinitNode,
		&gSpiWriteNode,
		&gSpiReadNode,
		&gSpiTransferNode,
		&gSpiAutoselectNode,
		&gSpiSelectNode,
};

static const CmdNode_t gSpiMenu = {
	.type = CmdNode_Menu,
	.name = "spi",
	.menu = {
		.nodes = gSpiFunctions,
		.count = LENGTH(gSpiFunctions)
	}
};


/*
 * INTERRUPT ROUTINES
 */
