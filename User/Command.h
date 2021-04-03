#ifndef COMMAND_H
#define COMMAND_H

#include "STM32X.h"

/*
 * PUBLIC DEFINITIONS
 */

#define LENGTH(x)		(sizeof(x) / sizeof(*x))
#define CMD_MAX_ARGS	8

/*
 * PUBLIC TYPES
 */

typedef struct CmdNode_s CmdNode_t;
typedef struct CmdLine_s CmdLine_t;

typedef enum {
	CmdArg_Number,
	CmdArg_Bytes,
} CmdArgType_t;

typedef enum {
	CmdNode_Function,
	CmdNode_Menu,
} CmdNodeType_t;

typedef struct {
	const char * name;
	uint8_t type;
} CmdArg_t;

typedef struct {
	union {
		struct {
			uint32_t size;
			uint8_t * data;
		}bytes;
		uint32_t number;
	};
} CmdArgValue_t;

typedef struct CmdNode_s {
	const char * name;
	uint8_t type;
	union {
		struct {
			CmdNode_t * nodes;
			uint32_t count;
		}menu;
		struct {
			CmdArg_t * args;
			uint32_t arglen;
			void (*callback)(CmdLine_t * line, CmdArgValue_t * argv);
		}func;
	};
} CmdNode_t;

typedef struct CmdLine_s {
	struct {
		uint32_t index;
		uint32_t size;
		char * data;
	}bfr;
	void (*print)(const uint8_t * data, uint32_t size);
	CmdNode_t * root;
} CmdLine_t;

/*
 * PUBLIC FUNCTIONS
 */

void Cmd_Init(CmdLine_t * line, CmdNode_t * root, void (*print)(const uint8_t * data, uint32_t size), char * buffer, uint32_t size);
void Cmd_Parse(CmdLine_t * line, const uint8_t * data, uint32_t count);
void Cmd_Printf(CmdLine_t * line, const char * fmt, ...);

#endif //COMMAND_H
