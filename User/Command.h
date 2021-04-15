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
	CmdArg_Bool,
	CmdArg_Number,
	CmdArg_Bytes,
	CmdArg_String,
} CmdArgType_t;

typedef enum {
	CmdNode_Function,
	CmdNode_Menu,
} CmdNodeType_t;

typedef enum {
	CmdReply_Info,
	CmdReply_Warn,
	CmdReply_Error,
} CmdReplyLevel_t;

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
		const char * str;
		bool boolean;
	};
} CmdArgValue_t;

typedef struct CmdNode_s {
	const char * name;
	uint8_t type;
	union {
		struct {
			const CmdNode_t ** nodes;
			uint32_t count;
		}menu;
		struct {
			const CmdArg_t * args;
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
	const CmdNode_t * root;
	struct {
		void * heap;
		uint32_t size;
		void * head;
	}mem;
	struct {
		bool color;
		bool bell;
	}cfg;
} CmdLine_t;

/*
 * PUBLIC FUNCTIONS
 */

void Cmd_Init(CmdLine_t * line, const CmdNode_t * root, void (*print)(const uint8_t * data, uint32_t size), void * memory, uint32_t size);
void Cmd_Parse(CmdLine_t * line, const uint8_t * data, uint32_t count);
void Cmd_Print(CmdLine_t * line, CmdReplyLevel_t level, const char * data, uint32_t count);
void Cmd_Printf(CmdLine_t * line, CmdReplyLevel_t level, const char * fmt, ...);
void Cmd_Prints(CmdLine_t * line, CmdReplyLevel_t level, const char * str);
void * Cmd_Malloc(CmdLine_t * line, uint32_t size);
void Cmd_Free(CmdLine_t * line, void * ptr);

#endif //COMMAND_H
