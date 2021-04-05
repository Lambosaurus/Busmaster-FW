
#include "Core.h"
#include "GPIO.h"
#include "USB.h"

#include "Command.h"
#include "Modules\I2cCmd.h"
#include "Modules\SpiCmd.h"


static void MAIN_PrintVersion(CmdLine_t * line, CmdArgValue_t * args)
{
	Cmd_Printf(line, "Busmaster v0.1\r\n");
}

const static CmdNode_t gVersionNode = {
	.type = CmdNode_Function,
	.name = "version",
	.func = {
		.arglen = 0,
		.callback = MAIN_PrintVersion
	}
};

const static CmdNode_t * gRootItems[3];

const static CmdNode_t gRootMenu = {
	.type = CmdNode_Menu,
	.name = "root",
	.menu = {
		.nodes = gRootItems,
		.count = LENGTH(gRootItems),
	}
};

static uint8_t gMemory[4096];

int main(void)
{
	CORE_Init();

	GPIO_EnableOutput(VOUT_EN_GPIO, VOUT_EN_PIN, GPIO_PIN_RESET);
	GPIO_EnableOutput(LED_GRN_GPIO, LED_GRN_PIN, GPIO_PIN_RESET);
	GPIO_EnableOutput(LED_RED_GPIO, LED_RED_PIN, GPIO_PIN_RESET);

	gRootItems[0] = &gVersionNode;
	gRootItems[1] = I2CCMD_InitMenu();
	gRootItems[2] = SPICMD_InitMenu();

	CmdLine_t line;
	Cmd_Init(&line, &gRootMenu, USB_Write, (void*)gMemory, sizeof(gMemory));

	USB_Init();

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

