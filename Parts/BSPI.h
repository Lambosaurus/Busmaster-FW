#ifndef BSPI_H
#define BSPI_H

#include "STM32X.h"

/*
 * PUBLIC DEFINITIONS
 */

/*
 * PUBLIC TYPES
 */

/*
 * PUBLIC FUNCTIONS
 */

/*
 * PUBLIC FUNCTIONS
 */

// Initialisation
void BSPI_Init(uint32_t bitrate);
void BSPI_Deinit(void);

// Transaction functions
void BSPI_Tx(const uint8_t * data, uint32_t count);
void BSPI_Rx(uint8_t * data, uint32_t count);
void BSPI_TxRx(const uint8_t * txdata, uint8_t * rxdata, uint32_t count);
uint8_t BSPI_Xfer(uint8_t data);


#endif //BSPI_H
