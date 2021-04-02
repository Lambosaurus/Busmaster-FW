
#include "Core.h"
#include "GPIO.h"
#include "UART.h"
#include "I2C.h"
#include "USB.h"

#include "MCP425.h"
#include <string.h>
#include <stdio.h>


typedef struct {
	uint32_t count;
	char bfr[256];
} Line_t;

static void HandleLine(const char * line, uint32_t count)
{
	char reply[256];
	uint32_t size = snprintf(reply, sizeof(reply), "Read: \"%s\"\r\n", line);
	USB_Write((uint8_t *)reply, size);
}

static void BuildLine( Line_t * line, const uint8_t * data, uint32_t count )
{
	for (uint32_t i = 0; i < count; i++)
	{
		char ch = (char)data[i];
		switch (ch)
		{
		case 0:
		case '\n':
		case '\r':
			line->bfr[line->count] = 0;
			if (line->count)
			{
				HandleLine(line->bfr, line->count);
			}
			line->count = 0;
			break;
		default:
			if (line->count < sizeof(line->bfr) - 1)
			{
				// Need to leave room for at least a null char.
				line->bfr[line->count++] = ch;
			}
			else
			{
				// Discard the line
				line->count = 0;
			}
			break;
		}
	}
}

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

	Line_t line;
	line.count = 0;

	while(1)
	{
		uint8_t bfr[32];
		uint32_t count = USB_Read(bfr, sizeof(bfr));
		if (count)
		{
			BuildLine(&line, bfr, count);
		}

		CORE_Idle();
	}
}

