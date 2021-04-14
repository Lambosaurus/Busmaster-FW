
#include "Core.h"
#include "GPIO.h"
#include "USB.h"

#include "Command.h"
#include "Modules\I2cCmd.h"
#include "Modules\SpiCmd.h"
#include "Modules\UartCmd.h"
#include "Modules\VersionCmd.h"
#include "Modules\ConfigCmd.h"
#include "Modules\VoutCmd.h"


static const CmdNode_t * gRootItems[6];

static const CmdNode_t gRootMenu = {
	.type = CmdNode_Menu,
	.name = "root",
	.menu = {
		.nodes = gRootItems,
		.count = LENGTH(gRootItems),
	}
};

static inline void MAIN_LedRed(void)
{
	GPIO_Set(LED_RED_GPIO, LED_RED_PIN);
	GPIO_Reset(LED_GRN_GPIO, LED_GRN_PIN);
}

static inline void MAIN_LedGrn(void)
{
	GPIO_Set(LED_GRN_GPIO, LED_GRN_PIN);
	GPIO_Reset(LED_RED_GPIO, LED_RED_PIN);
}

static uint8_t gMemory[4096];

int main(void)
{
	CORE_Init();
	GPIO_EnableOutput(LED_GRN_GPIO, LED_GRN_PIN, GPIO_PIN_RESET);
	GPIO_EnableOutput(LED_RED_GPIO, LED_RED_PIN, GPIO_PIN_RESET);
	MAIN_LedRed();

	gRootItems[0] = VERSIONCMD_InitMenu();
	gRootItems[1] = CONFIGCMD_InitMenu();
	gRootItems[2] = VOUTCMD_InitMenu();
	gRootItems[3] = I2CCMD_InitMenu();
	gRootItems[4] = SPICMD_InitMenu();
	gRootItems[5] = UARTCMD_InitMenu();

	CmdLine_t line;
	Cmd_Init(&line, &gRootMenu, USB_Write, (void*)gMemory, sizeof(gMemory));

	USB_Init();

	MAIN_LedGrn();
	while(1)
	{
		uint8_t bfr[64];
		uint32_t count = USB_Read(bfr, sizeof(bfr));
		if (count)
		{
			if (gConfig.echo)
			{
				USB_Write(bfr, count);
			}

			MAIN_LedRed();
			line.cfg.color = gConfig.color;
			line.cfg.bell = gConfig.bell;
			Cmd_Parse(&line, bfr, count);
			MAIN_LedGrn();
		}

		CORE_Idle();
	}
}

