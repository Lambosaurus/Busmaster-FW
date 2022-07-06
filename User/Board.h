#ifndef BOARD_H
#define BOARD_H

#define STM32L0
//#define STM32F0

// Core config
//#define CORE_USE_HSE
//#define CORE_USE_TICK_IRQ

// ADC config
#define ADC_VREF	        3300

// GPIO config
//#define GPIO_USE_IRQS
//#define GPIO_IRQ0_ENABLE

// TIM config
//#define TIM_USE_IRQS
//#define TIM2_ENABLE

// UART config
#define UART2_GPIO		GPIOA
#define UART2_PINS		(GPIO_PIN_2 | GPIO_PIN_3)
#define UART2_AF		GPIO_AF4_USART2
#define UART_BFR_SIZE   128

// SPI config
#define SPI1_GPIO		GPIOB
#define SPI1_PINS		(GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5)
#define SPI1_AF			GPIO_AF0_SPI1
#define SPI_CS_GPIO		GPIOA
#define SPI_CS_PIN		GPIO_PIN_15

// USB config
#define USB_ENABLE
#define USB_CLASS_CDC
#define USB_CDC_BFR_SIZE	256

// I2C config
#define I2C1_GPIO		GPIOB
#define I2C1_PINS		(GPIO_PIN_6 | GPIO_PIN_7)
#define I2C1_AF			GPIO_AF1_I2C1
#define I2C_USE_FASTMODEPLUS

// BSPI config
#define BSPI_GPIO		GPIOA
#define BSPI_MISO		GPIO_PIN_4
#define BSPI_MOSI		GPIO_PIN_7
#define BSPI_SCK		GPIO_PIN_6

// GPIO config
#define LED_GRN_GPIO	GPIOC
#define LED_GRN_PIN		GPIO_PIN_15
#define LED_RED_GPIO	GPIOC
#define LED_RED_PIN		GPIO_PIN_14

#define VOUT_EN_GPIO	GPIOA
#define VOUT_EN_PIN		GPIO_PIN_10

#define	MCP425_CS_GPIO	GPIOA
#define MCP425_CS_PIN	GPIO_PIN_5

#define PX_GPIO			GPIOA
#define P0_PIN			GPIO_PIN_0
#define P1_PIN			GPIO_PIN_3
#define P2_PIN			GPIO_PIN_2
#define P3_PIN			GPIO_PIN_1

#define P0_AIN			ADC_CHANNEL_0
#define P1_AIN			ADC_CHANNEL_3
#define P2_AIN			ADC_CHANNEL_2
#define P3_AIN			ADC_CHANNEL_1

#define BUS_SPI			SPI_1
#define BUS_I2C			I2C_1
#define BUS_UART		UART_2

#define BC660_UART		BUS_UART


#endif /* BOARD_H */
