#ifndef CONFIGCMD_H
#define CONFIGCMD_H

#include "STM32X.h"
#include "Command.h"

/*
 * PUBLIC DEFINITIONS
 */

typedef struct {
	uint32_t salt;
	char byte_delimiter;
	char byte_space;
	bool echo;
	bool color;
	bool bell;
	bool default_vout;
	uint32_t crc;
} Config_t;

extern Config_t gConfig;

/*
 * PUBLIC TYPES
 */

/*
 * PUBLIC FUNCTIONS
 */

const CmdNode_t * CONFIGCMD_InitMenu(void);

#endif //CONFIGCMD_H
