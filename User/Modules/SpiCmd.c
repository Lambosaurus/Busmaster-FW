
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
static const Cmd_Node_t gSpiMenu;

/*
 * PUBLIC FUNCTIONS
 */

const Cmd_Node_t * SPICMD_InitMenu(void)
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

static const Cmd_Arg_t gSpiInitArgs[] = {
	{
		.name = "frequency",
		.type = Cmd_Arg_Number,
	},
	{
		.name = "mode",
		.type = Cmd_Arg_Number | Cmd_Arg_Optional,
	}
};

static void SPICMD_Init(Cmd_Line_t * line, Cmd_ArgValue_t * argv)
{
	uint32_t bitrate = argv[0].number;
	uint32_t moden = argv[1].present ? argv[1].number : 0;

	if (gSpiEnabled)
	{
		SPI_Deinit(BUS_SPI);
	}

	SPI_Mode_t mode;
	switch (moden)
	{
	case 0:
		mode = SPI_Mode_0;
		break;
	case 1:
		mode = SPI_Mode_1;
		break;
	case 2:
		mode = SPI_Mode_2;
		break;
	case 3:
		mode = SPI_Mode_3;
		break;
	default:
		Cmd_Prints(line, Cmd_Reply_Error, "spi mode must be 0-3\r\n");
		return;
	}

	SPI_Init(BUS_SPI, bitrate, mode);
	GPIO_EnableOutput(SPI_CS_GPIO, SPI_CS_PIN, GPIO_PIN_SET);
	gSpiEnabled = true;

	uint32_t actual_bitrate = BUS_SPI->bitrate;
	if (actual_bitrate != bitrate)
	{
		COMCMD_PrintTruncation(line, "frequency", actual_bitrate);
	}
	COMCMD_PrintOk(line);
}

static const Cmd_Node_t gSpiInitNode = {
	.type = Cmd_Node_Function,
	.name = "init",
	.func = {
		.args = gSpiInitArgs,
		.arglen = LENGTH(gSpiInitArgs),
		.callback = SPICMD_Init,
	}
};

static void SPICMD_Deinit(Cmd_Line_t * line, Cmd_ArgValue_t * argv)
{
	GPIO_Deinit(SPI_CS_GPIO, SPI_CS_PIN);
	if (gSpiEnabled)
	{
		SPI_Deinit(BUS_SPI);
		gSpiEnabled = false;
	}
	COMCMD_PrintOk(line);
}

static const Cmd_Node_t gSpiDeinitNode = {
	.type = Cmd_Node_Function,
	.name = "deinit",
	.func = {
		.arglen = 0,
		.callback = SPICMD_Deinit,
	}
};

static const Cmd_Arg_t gSpiSelectArgs[] = {
	{
		.name = "enable",
		.type = Cmd_Arg_Bool,
	}
};

static void SPICMD_Select(Cmd_Line_t * line, Cmd_ArgValue_t * argv)
{
	uint32_t enable = argv[0].boolean;
	if (!gSpiEnabled)
	{
		COMCMD_PrintNoInit(line, "spi");
		return;
	}

	if (gSpiAutoSelect)
	{
		Cmd_Prints(line, Cmd_Reply_Info, "auto select disabled\r\n");
		gSpiAutoSelect = false;
	}
	GPIO_Write(SPI_CS_GPIO, SPI_CS_PIN, !enable);
	COMCMD_PrintOk(line);
}

static const Cmd_Node_t gSpiSelectNode = {
	.type = Cmd_Node_Function,
	.name = "select",
	.func = {
		.arglen = LENGTH(gSpiSelectArgs),
		.args = gSpiSelectArgs,
		.callback = SPICMD_Select,
	}
};

static void SPICMD_AutoSelect(Cmd_Line_t * line, Cmd_ArgValue_t * argv)
{
	if (!gSpiEnabled)
	{
		COMCMD_PrintNoInit(line, "spi");
		return;
	}
	gSpiAutoSelect = true;
	GPIO_Set(SPI_CS_GPIO, SPI_CS_PIN);
	Cmd_Prints(line, Cmd_Reply_Info, "auto select enabled\r\n");
}

static const Cmd_Node_t gSpiAutoselectNode = {
	.type = Cmd_Node_Function,
	.name = "autoselect",
	.func = {
		.arglen = 0,
		.callback = SPICMD_AutoSelect,
	}
};

static const Cmd_Arg_t gSpiWriteArgs[] = {
	{
		.name = "payload",
		.type = Cmd_Arg_Bytes,
	}
};

static void SPICMD_Write(Cmd_Line_t * line, Cmd_ArgValue_t * argv)
{
	if (!gSpiEnabled)
	{
		COMCMD_PrintNoInit(line, "spi");
		return;
	}

	Cmd_ArgValue_t * data = &argv[0];

	SPICMD_CS_Select();
	SPI_Write(BUS_SPI, data->bytes.data, data->bytes.size);
	SPICMD_CS_Deselect();

	COMCMD_PrintWritten(line, data->bytes.size);
}

static const Cmd_Node_t gSpiWriteNode = {
	.type = Cmd_Node_Function,
	.name = "write",
	.func = {
		.args = gSpiWriteArgs,
		.arglen = LENGTH(gSpiWriteArgs),
		.callback = SPICMD_Write,
	}
};

static const Cmd_Arg_t gSpiReadArgs[] = {
	{
		.name = "count",
		.type = Cmd_Arg_Number,
	}
};

static void SPICMD_Read(Cmd_Line_t * line, Cmd_ArgValue_t * argv)
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
		COMCMD_PrintTruncation(line, "count", count);
	}
	uint8_t * data = Cmd_Malloc(line, count);

	SPICMD_CS_Select();
	SPI_Read(BUS_SPI, data, count);
	SPICMD_CS_Deselect();

	COMCMD_PrintRead(line, data, count);
}

static const Cmd_Node_t gSpiReadNode = {
	.type = Cmd_Node_Function,
	.name = "read",
	.func = {
		.args = gSpiReadArgs,
		.arglen = LENGTH(gSpiReadArgs),
		.callback = SPICMD_Read,
	}
};

static const Cmd_Arg_t gSpiTransferArgs[] = {
	{
		.name = "payload",
		.type = Cmd_Arg_Bytes,
	}
};

static void SPICMD_Transfer(Cmd_Line_t * line, Cmd_ArgValue_t * argv)
{
	if (!gSpiEnabled)
	{
		COMCMD_PrintNoInit(line, "spi");
		return;
	}

	Cmd_ArgValue_t * data = &argv[0];

	// Write straight into the tx buffer.
	SPICMD_CS_Select();
	SPI_Transfer(BUS_SPI, data->bytes.data, data->bytes.data, data->bytes.size);
	SPICMD_CS_Deselect();

	COMCMD_PrintRead(line, data->bytes.data, data->bytes.size);
}

static const Cmd_Node_t gSpiTransferNode = {
	.type = Cmd_Node_Function,
	.name = "transfer",
	.func = {
		.args = gSpiTransferArgs,
		.arglen = LENGTH(gSpiTransferArgs),
		.callback = SPICMD_Transfer,
	}
};

static const Cmd_Node_t * gSpiFunctions[] = {
		&gSpiInitNode,
		&gSpiDeinitNode,
		&gSpiWriteNode,
		&gSpiReadNode,
		&gSpiTransferNode,
		&gSpiAutoselectNode,
		&gSpiSelectNode,
};

static const Cmd_Node_t gSpiMenu = {
	.type = Cmd_Node_Menu,
	.name = "spi",
	.menu = {
		.nodes = gSpiFunctions,
		.count = LENGTH(gSpiFunctions)
	}
};


/*
 * INTERRUPT ROUTINES
 */
