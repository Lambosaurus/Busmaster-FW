
#include "Core.h"
#include "GPIO.h"
#include "UART.h"


#include "MCP425.h"


int main(void)
{
	CORE_Init();

	MCP425_Init();

	MCP425_SetResistance(0, 4700);
	MCP425_SetResistance(1, 4700);

	MCP425_SetTerminals(0, MCP425_T_None);
	MCP425_SetTerminals(1, MCP425_T_None);

	while(1)
	{
		CORE_Idle();
	}
}

