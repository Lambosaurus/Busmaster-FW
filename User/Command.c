
#include "Command.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "NParse.h"

/*
 * PRIVATE DEFINITIONS
 */

/*
 * PRIVATE TYPES
 */

typedef struct {
	uint32_t index;
	const char * str;
	uint32_t size;
	bool quoted;
}Token_t;

/*
 * PRIVATE PROTOTYPES
 */

static void Cmd_Run(CmdLine_t * line, CmdNode_t * node, const char * str);
static void Cmd_Execute(CmdLine_t * line, const char * str);

/*
 * PRIVATE VARIABLES
 */

/*
 * PUBLIC FUNCTIONS
 */

void Cmd_Init(CmdLine_t * line, CmdNode_t * root, void (*print)(const uint8_t * data, uint32_t size), char * buffer, uint32_t size)
{
	line->bfr.index = 0;
	line->bfr.data = buffer;
	line->bfr.size = size;
	line->root = root;
	line->print = print;
}

void Cmd_Parse(CmdLine_t * line, const uint8_t * data, uint32_t count)
{
	for (uint32_t i = 0; i < count; i++)
	{
		char ch = (char)data[i];
		switch (ch)
		{
		case 0:
		case '\n':
		case '\r':
			line->bfr.data[line->bfr.index] = 0;
			if (line->bfr.index)
			{
				Cmd_Execute(line, line->bfr.data);
			}
			line->bfr.index = 0;
			break;
		default:
			if (line->bfr.index < line->bfr.size - 1)
			{
				// Need to leave room for at least a null char.
				line->bfr.data[line->bfr.index++] = ch;
			}
			else
			{
				// Discard the line
				line->bfr.index = 0;
			}
			break;
		}
	}
}

void Cmd_Printf(CmdLine_t * line, const char * fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    char bfr[256];
    uint32_t written = vsnprintf(bfr, sizeof(bfr), fmt, ap);
    va_end(ap);
    line->print((uint8_t *)bfr, written);
}

/*
 * PRIVATE FUNCTIONS
 */

static const char * Cmd_ParseToken(Token_t * token, const char * str)
{
	while (1)
	{
		// Find the first charachter
		char ch = *str;
		if (ch == ' ' || ch == '\t')
		{
			str++;
		}
		else if (ch == 0)
		{
			return NULL;
		}
		else
		{
			break;
		}
	}
	if (*str == '"')
	{
		// Check if its a quoted token
		str++;
		token->str = str;
		while (1)
		{
			char ch = *str;
			if (ch == '"')
			{
				const char * end = str;
				token->size = end - token->str;
				return str + 1;
			}
			else if (ch == 0)
			{
				return NULL;
			}
			else
			{
				str++;
			}
		}
	}
	else // Non quoted token
	{
		token->str = str;
		while (1)
		{
			char ch = *str;
			if (ch == ' ' || ch == '\t' || ch == 0)
			{
				const char * end = str;
				token->size = end - token->str;
				return str;
			}
			else
			{
				str++;
			}
		}
	}
}

static const char * Cmd_NextToken(const char ** str)
{
	static char bfr[256];
	Token_t token;
	const char * end = Cmd_ParseToken(&token, *str);
	if (end == NULL)
	{
		return NULL;
	}
	*str = end;
	memcpy(bfr, token.str, token.size);
	bfr[token.size] = 0;
	return bfr;
}

static void Cmd_Execute(CmdLine_t * line, const char * str)
{
	Cmd_Run(line, line->root, str);
}

static bool Cmd_ParseArg(CmdArg_t * arg, CmdArgValue_t * value, const char * str)
{
	switch (arg->type)
	{
	case CmdArg_Number:
		return NParse_Kuint(&str, &value->number) && (*str == 0);
	case CmdArg_Bytes:
		return NParse_Bytes(&str, value->bytes.data, 256, &value->bytes.size) && (*str == 0);
	default:
		return false;
	}
}

static const char * Cmd_ArgTypeStr(CmdArg_t * arg)
{
	switch (arg->type)
	{
	case CmdArg_Number:
		return "number";
	case CmdArg_Bytes:
		return "bytes";
	default:
		return "UNKNOWN";
	}
}

static void Cmd_RunMenu(CmdLine_t * line, CmdNode_t * node, const char * str)
{
	const char * item = Cmd_NextToken(&str);
	if (item == NULL)
	{
		Cmd_Printf(line, "'%s' is a <menu>\r\n", node->name);
	}
	else
	{
		CmdNode_t * child = NULL;
		for (uint32_t i = 0; i < node->menu.count; i++)
		{
			if (strcmp(node->menu.nodes[i].name, item) == 0)
			{
				child = &node->menu.nodes[i];
				break;
			}
		}
		if (child == NULL)
		{
			Cmd_Printf(line, "'%s' is not an item within '%s'\r\n", item, node->name);
		}
		else
		{
			Cmd_Run(line, child, str);
		}
	}
}

static void Cmd_RunFunction(CmdLine_t * line, CmdNode_t * node, const char * str)
{
	CmdArgValue_t args[CMD_MAX_ARGS];
	uint32_t argn = 0;
	for (argn = 0; argn < node->func.arglen; argn++)
	{
		CmdArg_t * arg = &node->func.args[argn];
		const char * token = Cmd_NextToken(&str);
		if (token == NULL || !Cmd_ParseArg(arg, args + argn, token))
		{
			Cmd_Printf(line, "Argument %d: '%s' must be <%s>\r\n", argn+1, arg->name, Cmd_ArgTypeStr(arg));
			return;
		}
	}
	if (argn != node->func.arglen)
	{
		Cmd_Printf(line, "%d arguments required by '%s'\r\n", node->func.arglen, node->name);
	}
	else
	{
		node->func.callback(line, args);
	}
}

static void Cmd_Run(CmdLine_t * line, CmdNode_t * node, const char * str)
{
	switch (node->type)
	{
	case CmdNode_Menu:
		Cmd_RunMenu(line, node, str);
		break;
	case CmdNode_Function:
		Cmd_RunFunction(line, node, str);
		break;
	}
}



/*
 * INTERRUPT ROUTINES
 */
