#ifndef _INI_PARSER_H
#define _INI_PARSER_H

/**
 * @file
 * @brief Functions for INI-file handling
 */

/**
 * Prototype of INI-entry handler function.
 */
typedef int ( *ini_handler_t )( char const * section, char const * name, char const * value, int line_no );

/**
 * Parses the programs INI-file.
 *
 * For each (attribute, value)-pair parsed, the function calls `handler`
 * and passes the section, attribute and value to `handler`.
 * Handler should return nonzero on success, zero on error.
 *
 * @return `0` on success, `-1` on file open error, `-2` on memory allocation
 * or (positive) line number of first error on parse error.
 */
int parse_ini_file( ini_handler_t handler );

#endif
