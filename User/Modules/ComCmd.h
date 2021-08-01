#ifndef COMCMD_H
#define COMCMD_H

#include <Cmd.h>
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

void COMCMD_PrintRead(Cmd_Line_t * line, uint8_t * data, uint32_t count);

void COMCMD_PrintOk(Cmd_Line_t * line);
void COMCMD_PrintError(Cmd_Line_t * line);
void COMCMD_PrintWritten(Cmd_Line_t * line, uint32_t count);
void COMCMD_PrintNoInit(Cmd_Line_t * line, const char * name);
void COMCMD_PrintTruncation(Cmd_Line_t * line, const char * name, uint32_t value);

#endif //COMCMD_H
