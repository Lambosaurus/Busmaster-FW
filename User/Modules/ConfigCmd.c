
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
static const Cmd_Node_t gConfigMenu;

/*
 * PUBLIC FUNCTIONS
 */

const Cmd_Node_t * CONFIGCMD_InitMenu(void)
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
		.bell = true,
		.color = true,
		.default_vout = false,
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

static const Cmd_Arg_t gConfigFormatArgs[] = {
	{
		.type = Cmd_Arg_String,
		.name = "delimiter",
	}
};

static void CONFIGCMD_Format(Cmd_Line_t * line, Cmd_ArgValue_t * args)
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
		Cmd_Prints(line, Cmd_Reply_Warn, "Format specifiers is a bit goofy. Ask tim for now.\r\n");
		Cmd_Prints(line, Cmd_Reply_Error, "Note: \"'s or ['s will be needed to permit argument parsing.\r\n");
	}
}

static const Cmd_Node_t gConfigFormatNode = {
	.type = Cmd_Node_Function,
	.name = "format",
	.func = {
		.arglen = LENGTH(gConfigFormatArgs),
		.args = gConfigFormatArgs,
		.callback = CONFIGCMD_Format
	}
};

static const Cmd_Arg_t gConfigEnableArgs[] = {
	{
		.type = Cmd_Arg_Bool,
		.name = "enable",
	}
};

static void CONFIGCMD_Echo(Cmd_Line_t * line, Cmd_ArgValue_t * args)
{
	bool enable = args[0].boolean;
	gConfig.echo = enable;
	COMCMD_PrintOk(line);
}

static const Cmd_Node_t gConfigEchoNode = {
	.type = Cmd_Node_Function,
	.name = "echo",
	.func = {
		.arglen = LENGTH(gConfigEnableArgs),
		.args = gConfigEnableArgs,
		.callback = CONFIGCMD_Echo
	}
};

static void CONFIGCMD_Color(Cmd_Line_t * line, Cmd_ArgValue_t * args)
{
	bool enable = args[0].boolean;
	gConfig.color = enable;
	COMCMD_PrintOk(line);
}

static const Cmd_Node_t gConfigColorNode = {
	.type = Cmd_Node_Function,
	.name = "color",
	.func = {
		.arglen = LENGTH(gConfigEnableArgs),
		.args = gConfigEnableArgs,
		.callback = CONFIGCMD_Color
	}
};

static void CONFIGCMD_Bell(Cmd_Line_t * line, Cmd_ArgValue_t * args)
{
	bool enable = args[0].boolean;
	gConfig.bell = enable;
	COMCMD_PrintOk(line);
}

static const Cmd_Node_t gConfigBellNode = {
	.type = Cmd_Node_Function,
	.name = "bell",
	.func = {
		.arglen = LENGTH(gConfigEnableArgs),
		.args = gConfigEnableArgs,
		.callback = CONFIGCMD_Bell
	}
};

static void CONFIGCMD_DefaultVout(Cmd_Line_t * line, Cmd_ArgValue_t * args)
{
	bool enable = args[0].boolean;
	gConfig.default_vout = enable;
	COMCMD_PrintOk(line);
}

static const Cmd_Node_t gConfigDefaultVoutNode = {
	.type = Cmd_Node_Function,
	.name = "boot-vout",
	.func = {
		.arglen = LENGTH(gConfigEnableArgs),
		.args = gConfigEnableArgs,
		.callback = CONFIGCMD_DefaultVout
	}
};

static void CONFIGCMD_Load(Cmd_Line_t * line, Cmd_ArgValue_t * args)
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

static const Cmd_Node_t gConfigLoadNode = {
	.type = Cmd_Node_Function,
	.name = "load",
	.func = {
		.arglen = 0,
		.callback = CONFIGCMD_Load
	}
};

static void CONFIGCMD_Save(Cmd_Line_t * line, Cmd_ArgValue_t * args)
{
	Config_Save();
	COMCMD_PrintOk(line);
}

static const Cmd_Node_t gConfigSaveNode = {
	.type = Cmd_Node_Function,
	.name = "save",
	.func = {
		.arglen = 0,
		.callback = CONFIGCMD_Save
	}
};

static void CONFIGCMD_Default(Cmd_Line_t * line, Cmd_ArgValue_t * args)
{
	Config_Default();
	COMCMD_PrintOk(line);
}

static const Cmd_Node_t gConfigDefaultNode = {
	.type = Cmd_Node_Function,
	.name = "default",
	.func = {
		.arglen = 0,
		.callback = CONFIGCMD_Default
	}
};

static const Cmd_Node_t * gConfigSetItems[] =
{
	&gConfigFormatNode,
	&gConfigEchoNode,
	&gConfigColorNode,
	&gConfigBellNode,
	&gConfigDefaultVoutNode,
};

static const Cmd_Node_t gConfigSetMenu =
{
	.type = Cmd_Node_Menu,
	.name = "set",
	.menu = {
		.count = LENGTH(gConfigSetItems),
		.nodes = gConfigSetItems
	}
};

static const Cmd_Node_t * gConfigItems[] =
{
	&gConfigSetMenu,
	&gConfigLoadNode,
	&gConfigSaveNode,
	&gConfigDefaultNode,
};

static const Cmd_Node_t gConfigMenu =
{
	.type = Cmd_Node_Menu,
	.name = "config",
	.menu = {
		.count = LENGTH(gConfigItems),
		.nodes = gConfigItems
	}
};

/*
 * INTERRUPT ROUTINES
 */
