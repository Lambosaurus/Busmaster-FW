
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
		static bool once = true;

		if (once && BC660_IsConnected())
		{
			once = false;

			if (BC660_UDP_Open("vm.tlembedded.com", 11001, 11002))
			{
				uint8_t msg[] = { 0x01, 0x02, 0x03, 0x04 };
				if (BC660_UDP_Write(msg, sizeof(msg)))
				{
					USB_CDC_WriteStr("Send ok\r\n");
				}
			}
			if (BC660_UDP_Close())
			{
				USB_CDC_WriteStr("Close ok\r\n");
			}

		}

		CORE_Delay(1000);
	}
}

