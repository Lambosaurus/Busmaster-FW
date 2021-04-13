
#include "UartCmd.h"
#include "UART.h"
#include "ComCmd.h"

/*
 * PRIVATE DEFINITIONS
 */

#define UART_RX_MAX		256

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

/*
 * PRIVATE VARIABLES
 */

static bool gUartEnabled;
static const CmdNode_t gUartMenu;

/*
 * PUBLIC FUNCTIONS
 */

const CmdNode_t * UARTCMD_InitMenu(void)
{
	gUartEnabled = false;
	return &gUartMenu;
}

/*
 * PRIVATE FUNCTIONS
 */

/*
 * FUNCITON NODES
 */

static const CmdArg_t gUartInitArgs[] = {
	{
		.name = "baud",
		.type = CmdArg_Number,
	}
};

static void UARTCMD_Init(CmdLine_t * line, CmdArgValue_t * argv)
{
	uint32_t baudrate = argv[0].number;

	if (gUartEnabled)
	{
		UART_Deinit(BUS_UART);
	}

	UART_Init(BUS_UART, baudrate);
	gUartEnabled = true;
	COMCMD_PrintOk(line);
}

static const CmdNode_t gUartInitNode = {
	.type = CmdNode_Function,
	.name = "init",
	.func = {
		.args = gUartInitArgs,
		.arglen = LENGTH(gUartInitArgs),
		.callback = UARTCMD_Init,
	}
};

static void UARTCMD_Deinit(CmdLine_t * line, CmdArgValue_t * argv)
{
	if (gUartEnabled)
	{
		UART_Deinit(BUS_UART);
		gUartEnabled = false;
	}
	COMCMD_PrintOk(line);
}

static const CmdNode_t gUartDeinitNode = {
	.type = CmdNode_Function,
	.name = "deinit",
	.func = {
		.arglen = 0,
		.callback = UARTCMD_Deinit,
	}
};

static const CmdArg_t gUartWriteArgs[] = {
	{
		.name = "payload",
		.type = CmdArg_Bytes,
	}
};

static void UARTCMD_Write(CmdLine_t * line, CmdArgValue_t * argv)
{
	if (!gUartEnabled)
	{
		COMCMD_PrintNoInit(line, "uart");
		return;
	}

	CmdArgValue_t * data = &argv[0];
	UART_Write(BUS_UART, data->bytes.data, data->bytes.size);
	COMCMD_PrintWritten(line, data->bytes.size);
}

static const CmdNode_t gUartWriteNode = {
	.type = CmdNode_Function,
	.name = "write",
	.func = {
		.args = gUartWriteArgs,
		.arglen = LENGTH(gUartWriteArgs),
		.callback = UARTCMD_Write,
	}
};

static const CmdArg_t gUartReadArgs[] = {
	{
		.name = "count",
		.type = CmdArg_Number,
	}
};

static void UARTCMD_Read(CmdLine_t * line, CmdArgValue_t * argv)
{
	if (!gUartEnabled)
	{
		COMCMD_PrintNoInit(line, "uart");
		return;
	}

	uint32_t count = argv[0].number;
	if (count > UART_RX_MAX)
	{
		count = UART_RX_MAX;
		Cmd_Printf(line, CmdReply_Warn, "count truncated to %d\r\n", count);
	}
	uint8_t * data = Cmd_Malloc(line, count);
	uint32_t read = UART_Read(BUS_UART, data, count);
	COMCMD_PrintRead(line, data, read);
}

static const CmdNode_t gUartReadNode = {
	.type = CmdNode_Function,
	.name = "read",
	.func = {
		.args = gUartReadArgs,
		.arglen = LENGTH(gUartReadArgs),
		.callback = UARTCMD_Read,
	}
};

static void UARTCMD_Ready(CmdLine_t * line, CmdArgValue_t * argv)
{
	if (!gUartEnabled)
	{
		COMCMD_PrintNoInit(line, "uart");
		return;
	}

	uint32_t ready = UART_ReadCount(BUS_UART);
	Cmd_Printf(line, CmdReply_Info, "%d bytes ready\r\n", ready);
}

static const CmdNode_t gUartReadyNode = {
	.type = CmdNode_Function,
	.name = "ready",
	.func = {
		.arglen = 0,
		.callback = UARTCMD_Ready,
	}
};

static const CmdNode_t * gUartFunctions[] = {
		&gUartInitNode,
		&gUartDeinitNode,
		&gUartWriteNode,
		&gUartReadNode,
		&gUartReadyNode,
};

static const CmdNode_t gUartMenu = {
	.type = CmdNode_Menu,
	.name = "uart",
	.menu = {
		.nodes = gUartFunctions,
		.count = LENGTH(gUartFunctions)
	}
};


/*
 * INTERRUPT ROUTINES
 */
