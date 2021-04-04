
#include "Core.h"
#include "GPIO.h"
#include "USB.h"

#include "Command.h"
#include "Modules\I2cCmd.h"


CmdNode_t gRootItems[1];

const CmdNode_t gRootMenu = {
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

	I2CCMD_InitMenu(&gRootItems[0]);

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

