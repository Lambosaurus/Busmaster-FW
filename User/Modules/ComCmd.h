#ifndef COMCMD_H
#define COMCMD_H

#include "STM32X.h"
#include "Command.h"

/*
 * PUBLIC DEFINITIONS
 */

/*
 * PUBLIC TYPES
 */

/*
 * PUBLIC FUNCTIONS
 */

void COMCMD_PrintRead(CmdLine_t * line, uint8_t * data, uint32_t count);

void COMCMD_PrintOk(CmdLine_t * line);
void COMCMD_PrintError(CmdLine_t * line);
void COMCMD_PrintWritten(CmdLine_t * line, uint32_t count);
void COMCMD_PrintNoInit(CmdLine_t * line, const char * name);

#endif //COMCMD_H
