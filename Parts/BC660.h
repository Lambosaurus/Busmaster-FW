#ifndef BC660_H
#define BC660_H

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

#ifndef BC660_MAX_PACKET
#define BC660_MAX_PACKET		64
#endif

/*
 * PUBLIC FUNCTIONS
 */


bool BC660_Init(void);
void BC660_Deinit(void);

bool BC660_IsConnected(void);
uint8_t BC660_GetSignalQuality(void); // Returns a 0 to 100% indicator

bool BC660_UDP_Open(const char * addr, uint16_t remote_port, uint16_t local_port);
bool BC660_UDP_Write(const uint8_t * data, uint32_t size);
uint32_t BC660_UDP_Read(uint8_t * data, uint32_t size, uint32_t timeout);
bool BC660_UDP_Close(void);


#endif //BC660_H
