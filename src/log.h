#ifndef _LOG_H_
#define _LOG_H_

/**
 * @file
 * @brief Functions for logging
 */

#include <syslog.h>

/**
 * Opens connection to the system logger, if the program is supposed to
 * daemonize.
 *
 * If the runtime settings indicate, that the program shall daemonize, then
 * this method opens a connection to the system logger using the settings
 * as specified by `runtime_config` (e.g. log facility, log ID, log level,
 * etc.).
 * If the runtime settings indicate, that the program shall not daemonize,
 * then this method is a no-op.
 */
void open_log( void );

/**
 * Closes the connection to the system logger.
 */
void close_log( void );

/**
 * Logs a message to the system logger or standard error output, depending
 * on runtime settings and state of program.
 *
 * The log message `format` is only printed to the system log or standard
 * error output, if `priority` is equally or more severe than the set
 * log level indicated by `runtime_config`.
 *
 * If the program is daemonized, then the log message is written to the
 * system log.
 * If the program shall daemonize (but hasn't done so yet), the log message
 * is written to the system log _and_ to standard error output.
 * If the program runs in foreground mode (and will never daemonize), then
 * the log message is written to standard error output only.
 *
 * @param priority The priority level of the log message.
 * @param format A `printf`-like format string containing the log message.
 */
void log_msg( int const priority, char const * const format, ... );

/**
 * Converts a numeric log level (0..7) to its name.
 *
 * @param log_level A numeric log level between 0 (emergency) and 7 (debug).
 * @return The name of the log level.
 */
char const * convert_log_level_2_str( int const log_level );

/**
 * Converts the name of a log level to its numeric value.
 *
 * @param str The name of the log level.
 * @return The log level between 0 (emergency) and 7 (debug), if the passed
 * name has been recognized as a log level, or -1 in case of an conversion
 * error.
 */
int convert_str_2_log_level( char const * const str );

/**
 * Converts a numeric facility value (0..23) to its name.
 *
 * @param log_facility A numeric facility value between 0 (KERN) and 23 (LOCAL7).
 * @return The name of the facility or `NULL` if the facility is undefined.
 */
char const * convert_log_facility_2_str( int log_facility );

/**
 * Converts the name of a facility to its numeric value.
 *
 * @param str The name of the facility.
 * @return The facility value between 0 (KERN) and 32 (LOCAL7), if the passed
 * name has been recognized as a facility, or -1 in case of an conversion
 * error.
 */
int convert_str_2_log_facility( char const * const str );

#endif
