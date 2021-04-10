
#include "ConfigCmd.h"
#include "ComCmd.h"
#include "EEPROM.h"
#include "CRC.h"
#include <string.h>

/*
 * PRIVATE DEFINITIONS
 */

#define CONFIG_SALT		0x40BB3F0C

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

static bool Config_Load(void);
static void Config_Default(void);
static void Config_Save(void);

/*
 * PRIVATE VARIABLES
 */

Config_t gConfig;
static const CmdNode_t gConfigMenu;

/*
 * PUBLIC FUNCTIONS
 */

const CmdNode_t * CONFIGCMD_InitMenu(void)
{
	if (!Config_Load())
	{
		Config_Default();
	}
	return &gConfigMenu;
}

/*
 * PRIVATE FUNCTIONS
 */

static bool Config_Load(void)
{
	Config_t config;
	EEPROM_Read(0, &config, sizeof(config));
	if (config.salt != CONFIG_SALT)
	{
		return false;
	}
	uint32_t crc = CRC32(0, (uint32_t *)&config, sizeof(config) - sizeof(uint32_t));
	if (crc != config.crc)
	{
		return false;
	}
	memcpy(&gConfig, &config, sizeof(config));
	return true;
}

static void Config_Default(void)
{
	Config_t config = {
		.byte_delimiter = '[',
		.byte_space = ' ',
		.echo = true,
	};
	memcpy(&gConfig, &config, sizeof(config));
}

static void Config_Save(void)
{
	gConfig.salt = 0x40BB3F0C;
	gConfig.crc = CRC32(0, (uint32_t *)&gConfig, sizeof(gConfig) - sizeof(uint32_t));
	EEPROM_Write(0, &gConfig, sizeof(gConfig));
}

/*
 * FUNCTION NODES
 */

static const CmdArg_t gConfigFormatArgs[] = {
	{
		.type = CmdArg_String,
		.name = "delimiter",
	}
};

static void CONFIGCMD_Format(CmdLine_t * line, CmdArgValue_t * args)
{
	const char * fmt = args[0].str;

	bool success = true;

	char delimiter = 0;

	switch (*fmt)
	{
	case '"':
	case '\'':
	case '<':
	case '[':
		delimiter = *fmt++;
		break;
	case ',':
		break;
	default:
		success = false;
	}

	char space = 0;

	if (success)
	{
		if (*fmt == ',')
		{
			fmt++;
			switch (*fmt)
			{
			case ' ':
				if (delimiter == 0)
				{
					success = false;
				}
				// fallthrough
			case '-':
			case ':':
			case ',':
				space = *fmt++;
				break;
			default:
				success = false;
			}
		}
	}

	if (success)
	{
		gConfig.byte_delimiter = delimiter;
		gConfig.byte_space = space;
		char * test_str = "Hello\xF0\r\n";
		COMCMD_PrintRead(line, (uint8_t *)test_str, strlen(test_str));
	}
	else
	{
		Cmd_Printf(line, "Format specifiers is a bit goofy. Ask tim for now.\r\n");
		Cmd_Printf(line, "Note: \"'s or ['s will be needed to permit argument parsing.\r\n");
	}
}

static const CmdNode_t gConfigFormatNode = {
	.type = CmdNode_Function,
	.name = "format",
	.func = {
		.arglen = LENGTH(gConfigFormatArgs),
		.args = gConfigFormatArgs,
		.callback = CONFIGCMD_Format
	}
};

static const CmdArg_t gConfigEchoArgs[] = {
	{
		.type = CmdArg_Bool,
		.name = "enable",
	}
};

static void CONFIGCMD_Echo(CmdLine_t * line, CmdArgValue_t * args)
{
	bool enable = args[0].boolean;
	gConfig.echo = enable;
	COMCMD_PrintOk(line);
}

static const CmdNode_t gConfigEchoNode = {
	.type = CmdNode_Function,
	.name = "echo",
	.func = {
		.arglen = LENGTH(gConfigEchoArgs),
		.args = gConfigEchoArgs,
		.callback = CONFIGCMD_Echo
	}
};


static void CONFIGCMD_Load(CmdLine_t * line, CmdArgValue_t * args)
{
	if (Config_Load())
	{
		COMCMD_PrintOk(line);
	}
	else
	{
		COMCMD_PrintError(line);
	}
}

static const CmdNode_t gConfigLoadNode = {
	.type = CmdNode_Function,
	.name = "load",
	.func = {
		.arglen = 0,
		.callback = CONFIGCMD_Load
	}
};

static void CONFIGCMD_Save(CmdLine_t * line, CmdArgValue_t * args)
{
	Config_Save();
	COMCMD_PrintOk(line);
}

static const CmdNode_t gConfigSaveNode = {
	.type = CmdNode_Function,
	.name = "save",
	.func = {
		.arglen = 0,
		.callback = CONFIGCMD_Save
	}
};

static void CONFIGCMD_Default(CmdLine_t * line, CmdArgValue_t * args)
{
	Config_Default();
	COMCMD_PrintOk(line);
}

static const CmdNode_t gConfigDefaultNode = {
	.type = CmdNode_Function,
	.name = "default",
	.func = {
		.arglen = 0,
		.callback = CONFIGCMD_Default
	}
};

static const CmdNode_t * gConfigItems[] =
{
	&gConfigFormatNode,
	&gConfigEchoNode,
	&gConfigLoadNode,
	&gConfigSaveNode,
	&gConfigDefaultNode,
};

static const CmdNode_t gConfigMenu =
{
	.type = CmdNode_Menu,
	.name = "config",
	.menu = {
		.count = LENGTH(gConfigItems),
		.nodes = gConfigItems
	}
};

/*
 * INTERRUPT ROUTINES
 */
