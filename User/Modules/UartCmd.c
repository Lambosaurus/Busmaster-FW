
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
static const Cmd_Node_t gUartMenu;

/*
 * PUBLIC FUNCTIONS
 */

const Cmd_Node_t * UARTCMD_InitMenu(void)
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

static const Cmd_Arg_t gUartInitArgs[] = {
	{
		.name = "baud",
		.type = Cmd_Arg_Number,
	}
};

static void UARTCMD_Init(Cmd_Line_t * line, Cmd_ArgValue_t * argv)
{
	uint32_t baudrate = argv[0].number;

	if (gUartEnabled)
	{
		UART_Deinit(BUS_UART);
	}

	UART_Init(BUS_UART, baudrate, UART_Mode_Default);
	gUartEnabled = true;
	COMCMD_PrintOk(line);
}

static const Cmd_Node_t gUartInitNode = {
	.type = Cmd_Node_Function,
	.name = "init",
	.func = {
		.args = gUartInitArgs,
		.arglen = LENGTH(gUartInitArgs),
		.callback = UARTCMD_Init,
	}
};

static void UARTCMD_Deinit(Cmd_Line_t * line, Cmd_ArgValue_t * argv)
{
	if (gUartEnabled)
	{
		UART_Deinit(BUS_UART);
		gUartEnabled = false;
	}
	COMCMD_PrintOk(line);
}

static const Cmd_Node_t gUartDeinitNode = {
	.type = Cmd_Node_Function,
	.name = "deinit",
	.func = {
		.arglen = 0,
		.callback = UARTCMD_Deinit,
	}
};

static const Cmd_Arg_t gUartWriteArgs[] = {
	{
		.name = "payload",
		.type = Cmd_Arg_Bytes,
	}
};

static void UARTCMD_Write(Cmd_Line_t * line, Cmd_ArgValue_t * argv)
{
	if (!gUartEnabled)
	{
		COMCMD_PrintNoInit(line, "uart");
		return;
	}

	Cmd_ArgValue_t * data = &argv[0];
	UART_Write(BUS_UART, data->bytes.data, data->bytes.size);
	COMCMD_PrintWritten(line, data->bytes.size);
}

static const Cmd_Node_t gUartWriteNode = {
	.type = Cmd_Node_Function,
	.name = "write",
	.func = {
		.args = gUartWriteArgs,
		.arglen = LENGTH(gUartWriteArgs),
		.callback = UARTCMD_Write,
	}
};

static const Cmd_Arg_t gUartReadArgs[] = {
	{
		.name = "count",
		.type = Cmd_Arg_Number | Cmd_Arg_Optional,
	}
};

static void UARTCMD_Read(Cmd_Line_t * line, Cmd_ArgValue_t * argv)
{
	if (!gUartEnabled)
	{
		COMCMD_PrintNoInit(line, "uart");
		return;
	}

	uint32_t count = argv[0].present ? argv[0].number : UART_RX_MAX;
	if (count > UART_RX_MAX)
	{
		count = UART_RX_MAX;
		COMCMD_PrintTruncation(line, "count", count);
	}
	uint8_t * data = Cmd_Malloc(line, count);
	uint32_t read = UART_Read(BUS_UART, data, count);
	COMCMD_PrintRead(line, data, read);
}

static const Cmd_Node_t gUartReadNode = {
	.type = Cmd_Node_Function,
	.name = "read",
	.func = {
		.args = gUartReadArgs,
		.arglen = LENGTH(gUartReadArgs),
		.callback = UARTCMD_Read,
	}
};

static void UARTCMD_Ready(Cmd_Line_t * line, Cmd_ArgValue_t * argv)
{
	if (!gUartEnabled)
	{
		COMCMD_PrintNoInit(line, "uart");
		return;
	}

	uint32_t ready = UART_ReadCount(BUS_UART);
	Cmd_Printf(line, Cmd_Reply_Info, "%d bytes ready\r\n", ready);
}

static const Cmd_Node_t gUartReadyNode = {
	.type = Cmd_Node_Function,
	.name = "ready",
	.func = {
		.arglen = 0,
		.callback = UARTCMD_Ready,
	}
};

static const Cmd_Node_t * gUartFunctions[] = {
		&gUartInitNode,
		&gUartDeinitNode,
		&gUartWriteNode,
		&gUartReadNode,
		&gUartReadyNode,
};

static const Cmd_Node_t gUartMenu = {
	.type = Cmd_Node_Menu,
	.name = "uart",
	.menu = {
		.nodes = gUartFunctions,
		.count = LENGTH(gUartFunctions)
	}
};


/*
 * INTERRUPT ROUTINES
 */
