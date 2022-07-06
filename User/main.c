
#include "Core.h"
#include "GPIO.h"
#include "USB.h"
#include "ADC.h"

#include "BC660.h"

#include <stdio.h>

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


int main(void)
{
	CORE_Init();
	GPIO_EnableOutput(LED_GRN_GPIO, LED_GRN_PIN, GPIO_PIN_RESET);
	GPIO_EnableOutput(LED_RED_GPIO, LED_RED_PIN, GPIO_PIN_RESET);
	MAIN_LedRed();
	ADC_Init();

	USB_Init();


	CORE_Delay(200);
	BC660_Init();


	MAIN_LedGrn();
	while(1)
	{
		bool connected = BC660_IsConnected();

		char bfr[64];
		snprintf(bfr, sizeof(bfr), "Connected: %d\r\n", connected);
		USB_CDC_WriteStr(bfr);

		CORE_Delay(1000);
	}
}

