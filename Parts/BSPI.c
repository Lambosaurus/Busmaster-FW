
#include "BSPI.h"
#include "GPIO.h"

/*
 * PRIVATE DEFINITIONS
 */

#define TX_BLANK	0xFF

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

static void BSPI_DelayUS(uint32_t us);

/*
 * PRIVATE VARIABLES
 */

static uint32_t gBitDelay;

/*
 * PUBLIC FUNCTIONS
 */

void BSPI_Init(uint32_t bitrate)
{
	GPIO_EnableInput(BSPI_GPIO, BSPI_MISO, GPIO_NOPULL);
	GPIO_EnableOutput(BSPI_GPIO, BSPI_MOSI | BSPI_SCK, GPIO_PIN_SET);

	gBitDelay = 1000000 / (bitrate * 2);
	if (gBitDelay == 0)
	{
		gBitDelay += 1;
	}
}

void BSPI_Deinit(void)
{
	GPIO_Deinit(BSPI_GPIO, BSPI_MISO | BSPI_MOSI | BSPI_SCK);
}

void BSPI_Write(const uint8_t * data, uint32_t count)
{
	for (uint32_t i = 0; i < count; i++)
	{
		BSPI_Xfer(data[i]);
	}
}

void BSPI_Read(uint8_t * data, uint32_t count)
{
	for (uint32_t i = 0; i < count; i++)
	{
		data[i] = BSPI_Xfer(TX_BLANK);
	}
}

void BSPI_Transfer(const uint8_t * txdata, uint8_t * rxdata, uint32_t count)
{
	for (uint32_t i = 0; i < count; i++)
	{
		rxdata[i] = BSPI_Xfer(txdata[i]);
	}
}


uint8_t BSPI_Xfer(uint8_t data)
{
	uint32_t rx = 0;

	for (uint32_t b = (1 << 7); b > 0; b >>= 1)
	{
		GPIO_Reset(BSPI_GPIO, BSPI_SCK);
		GPIO_Write(BSPI_GPIO, BSPI_MOSI, data & b);
		BSPI_DelayUS(gBitDelay);
		if (GPIO_Read(BSPI_GPIO, BSPI_MISO)) { rx |= b; }
		GPIO_Set(BSPI_GPIO, BSPI_SCK);
		BSPI_DelayUS(gBitDelay);
	}

	return rx;
}


/*
 * PRIVATE FUNCTIONS
 */

static void BSPI_DelayUS(uint32_t us)
{
	for (volatile uint32_t i=0; i!=us; i++)
	{
		__NOP();
	}
}

/*
 * INTERRUPT ROUTINES
 */
