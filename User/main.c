
#include "Core.h"
#include "GPIO.h"
#include "USB.h"
#include "ADC.h"

#include "BC660.h"
#include "Packet.h"

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

#define DEVID	0x12100603

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
				uint8_t packet[64];
				uint32_t packet_size = Packet_Write(packet, DEVID, msg, sizeof(msg));

				if (BC660_UDP_Write(packet, packet_size))
				{
					USB_CDC_WriteStr("Send ok\r\n");
				}

				uint8_t rx[10];
				uint32_t read = BC660_UDP_Read(rx, sizeof(rx), 5000);
				if (read)
				{
					char bfr[32];
					snprintf(bfr, sizeof(bfr), "Recv: %d bytes: %02X%02X\r\n", read, rx[0], rx[1]);
					USB_CDC_WriteStr(bfr);
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

