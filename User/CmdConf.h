#ifndef COMMAND_CONF_H
#define COMMAND_CONF_H


/*
 * BUFFER CONFIGURATION
 */

// Maximum number of arguments to parse.
#define CMD_MAX_ARGS	8

// Maximum line length
#define CMD_MAX_LINE	256

/*
 * TERMINAL INPUT CONFIGURATION
 * 		These are recommended for interfacing with PuTTY-like terminals
 */

// Support for ANSI input sequences, such as arrow keys.
#define CMD_USE_ANSI

// Allow tab completion of commands
#define CMD_USE_TABCOMPLETE

/*
 * TERMINAL OUTPUT CONFIGURATION
 * 		These are recommended for interfacing with PuTTY-like terminals
 * 		These are must also be enabled within the Cmd_LineConfig_t
 */

// Emit the bell char '\a' for errors and ignored commands
#define CMD_USE_BELL

// Echo input chars
#define CMD_USE_ECHO

// Emit color output sequences, for warnings and errors.
#define CMD_USE_COLOR

/*
 * ARGUMENT PARSING CONFIGURATION
 * 		These support different argument format options
 */

// Support escape sequences for string parsing and formatting
#define CMD_STRING_ESC

#endif //COMMAND_CONF_H
