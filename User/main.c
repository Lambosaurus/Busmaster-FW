
#include "Core.h"
#include "GPIO.h"
#include "UART.h"
#include "I2C.h"


#include "MCP425.h"


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

	CORE_Delay(100);

	uint8_t tx[1] = { 0x75 };
	uint8_t rx[1];

	if (I2C_Transfer(BUS_I2C, 0x68, tx, sizeof(tx), rx, sizeof(rx)) && rx[0] == 0x68)
	{
		GPIO_Set(LED_GRN_GPIO, LED_GRN_PIN);
	}
	else
	{
		GPIO_Set(LED_RED_GPIO, LED_RED_PIN);
	}


	while(1)
	{
		CORE_Idle();
	}
}

