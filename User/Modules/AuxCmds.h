#ifndef AUXCMDS_H
#define AUXCMDS_H

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

const CmdNode_t * AUXCMD_InitTemp(void);
const CmdNode_t * AUXCMD_InitVref(void);
const CmdNode_t * AUXCMD_InitVout(void);

#endif //VOUTCMD_H
