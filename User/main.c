
#include "Core.h"
#include "GPIO.h"
#include "UART.h"
#include "I2C.h"
#include "USB.h"

#include "MCP425.h"
#include <string.h>
#include <stdio.h>



#include "Command.h"


void Testcommand(CmdLine_t * line, CmdArgValue_t * argv)
{
	uint32_t n1 = argv[0].number;
	uint32_t n2 = argv[1].number;
	Cmd_Printf(line, "Args %d, %d\r\n", n1, n2);
}

CmdArg_t gI2cInitArgs[] = {
	{
		.name = "speed",
		.type = CmdArg_Number,
	},
	{
		.name = "pullup",
		.type = CmdArg_Number,
	}
};

CmdNode_t gI2cFunctions[] = {
	{
		.type = CmdNode_Function,
		.name = "init",
		.func = {
			.args = gI2cInitArgs,
			.arglen = LENGTH(gI2cInitArgs),
			.callback = Testcommand,
		}
	}
};

CmdNode_t gI2cMenu = {
	.type = CmdNode_Menu,
	.name = "i2c",
	.menu = {
		.count = LENGTH(gI2cFunctions),
		.nodes = gI2cFunctions,
	}
};

CmdNode_t gRootMenu = {
	.type = CmdNode_Menu,
	.name = "root",
	.menu = {
		.nodes = &gI2cMenu,
		.count = 1,
	}
};

int main(void)
{
	CORE_Init();

	MCP425_Init();

	MCP425_SetResistance(0, 4700);
	MCP425_SetResistance(1, 4700);
	MCP425_SetTerminals(0, MCP425_T_W | MCP425_T_B);
	MCP425_SetTerminals(1, MCP425_T_W | MCP425_T_B);

	GPIO_EnableOutput(VOUT_EN_GPIO, VOUT_EN_PIN, GPIO_PIN_RESET);
	GPIO_EnableOutput(LED_GRN_GPIO, LED_GRN_PIN, GPIO_PIN_RESET);
	GPIO_EnableOutput(LED_RED_GPIO, LED_RED_PIN, GPIO_PIN_RESET);

	I2C_Init(BUS_I2C, I2C_Mode_Fast);

	USB_Init();

	char buffer[256];
	CmdLine_t line;
	Cmd_Init(&line, &gRootMenu, USB_Write, buffer, sizeof(buffer));

	while(1)
	{
		uint8_t bfr[32];
		uint32_t count = USB_Read(bfr, sizeof(bfr));
		if (count)
		{
			Cmd_Parse(&line, bfr, count);
		}

		CORE_Idle();
	}
}

