#ifndef AUXCMDS_H
#define AUXCMDS_H

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

const Cmd_Node_t * AUXCMD_InitTemp(void);
const Cmd_Node_t * AUXCMD_InitVref(void);
const Cmd_Node_t * AUXCMD_InitVout(void);

#endif //VOUTCMD_H
