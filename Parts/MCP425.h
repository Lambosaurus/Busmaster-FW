#ifndef MCP425_H
#define MCP425_H

#include "STM32X.h"

/*
 * PUBLIC DEFINITIONS
 */

#define MCP425_R_MAX	10000

/*
 * PUBLIC TYPES
 */

typedef enum {
	MCP425_T_None = 0x00,
	MCP425_T_B = 0x01,
	MCP425_T_W = 0x02,
	MCP425_T_A = 0x04,
	MCP425_T_ALL = (MCP425_T_A | MCP425_T_W | MCP425_T_B)
} MCP425_Terminal_t;

/*
 * PUBLIC FUNCTIONS
 */

/*
 * PUBLIC FUNCTIONS
 */

bool MCP425_Init(void);
void MCP425_Deinit(void);

void MCP425_SetTerminals(uint8_t ch, MCP425_Terminal_t tcon);

// Resistance defined between W and B
void MCP425_SetResistance(uint8_t ch, uint32_t resistance);

#endif //MCP425_H
