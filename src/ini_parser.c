#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "ini_parser.h"
#include "log.h"
#include "runtime_setting.h"

static char const * const INI_INLINE_COMMENT_PREFIXES = ";#";
static char const * const INI_START_COMMENT_PREFIXES = ";#";
static size_t const INI_INITIAL_ALLOC = 200;

/**
 * Strips whitespace off end of the given string, in place.
 *
 * Modifies the provided string `s` by overwriting trailing whitespaces with
 * `NUL` characters.
 *
 * @param s The string to be modified in-place.
 * @return The in-place modified string `s`.
 */
static char* ini_rstrip( char* s ) {
	char* p = s + strlen(s);
	while (p > s && isspace((unsigned char)(*--p)))
		*p = '\0';
	return s;
}

/**
 * Strips whitespace off the start of the given string, in place.
 *
 * @param s The string to be modified in place.
 * @return Pointer to first non-whitespace char in given string.
 */
static char* ini_lskip( char* s ) {
	while (*s && isspace((unsigned char)(*s)))
		s++;
	return s;
}

/**
 * Returns pointer to first char (of chars) or inline comment in given string.
 *
 * Inline comment must be prefixed by a whitespace character to register as a
 * comment.
 *
 * @param s The haystack
 * @param chars The needles
 * @return Pointer to first matching char or to `NUL` at end of string if
 * not neither found.
 */
static char* ini_find_chars_or_comment( char const * s, char const * chars) {
	int was_space = 0;
	while (
		*s &&
		( !chars || !strchr(chars, *s) ) &&
		!( was_space && strchr( INI_INLINE_COMMENT_PREFIXES, *s ) )
	) {
		was_space = isspace( (unsigned char)(*s) );
		s++;
	}
	return (char*)s;
}

/* See documentation in header file. */
static int parse_ini_stream( FILE* stream, ini_handler_t handler ) {
	size_t line_size = INI_INITIAL_ALLOC;
	char* line_buf = (char*)malloc( line_size );
	if ( !line_buf )
		return -2;
	*line_buf = '\0';

	size_t section_size = INI_INITIAL_ALLOC;
	char* section_buf = (char*)malloc( section_size );
	if ( !section_buf ) {
		free( line_buf );
		return -2;
	}
	*section_buf = '\0';

	char* new_buffer = NULL;
	size_t offset;

	char* start = NULL;
	char* end = NULL;
	char* name = NULL;
	char* value = NULL;
	int line_no = 0;
	int first_error_line_no = 0;

	/* Scan through stream line by line */
	while ( fgets( line_buf, (int)line_size, stream ) != NULL ) {
		// Reallocate line buffer until entire line fits into buffer
		offset = strlen( line_buf );
		while (offset == line_size - 1 && line_buf[offset - 1] != '\n') {
			line_size *= 2;
			new_buffer = realloc( line_buf, line_size );
			if ( !new_buffer ) {
				free( line_buf );
				free( section_buf );
				return -2;
			}
			line_buf = new_buffer;
			new_buffer = NULL;
			if ( fgets( line_buf + offset, (int)( line_size - offset), stream ) == NULL )
				break;
			offset += strlen( line_buf + offset );
		}
		offset = 0;

		line_no++;

		start = line_buf;
		if (
			line_no == 1 &&
			(unsigned char)start[0] == 0xEF &&
			(unsigned char)start[1] == 0xBB &&
			(unsigned char)start[2] == 0xBF
		) {
			start += 3;
		}
		start = ini_lskip( ini_rstrip( start ) );

		if ( strchr( INI_START_COMMENT_PREFIXES, *start) != NULL ) {
			/* Start-of-line comment */
		} else if ( *start == '[' ) {
			/* A "[section]" line; reset section */
			*section_buf = '\0';
			end = ini_find_chars_or_comment( start + 1, "]");
			if ( *end == ']') {
				*end = '\0';
				strncpy( section_buf, start + 1, section_size );
				// Reallocate section buffer until entire name of section fits into buffer
				while ( section_buf[ section_size - 1 ] != '\0' ) {
					offset = section_size;
					section_size *= 2;
					new_buffer = realloc( section_buf, section_size );
					if ( !new_buffer ) {
						free( line_buf );
						free( section_buf );
						return -2;
					}
					section_buf = new_buffer;
					new_buffer = NULL;
					strncpy( section_buf + offset, start + 1 + offset, section_size - offset );
				}
				offset = 0;
			} else if ( first_error_line_no == 0 ) {
				/* No ']' found on section line and no previous error recorded */
				first_error_line_no = line_no;
			}
		} else if (*start) {
			/* Neither a comment nor a section line, must be a name[=:]value pair */
			end = ini_find_chars_or_comment( start, "=:" );
			if ( *end == '=' || *end == ':' ) {
				*end = '\0';
				name = ini_rstrip( start );
				value = end + 1;
				end = ini_find_chars_or_comment( value, NULL );
				if ( *end )
					*end = '\0';
				value = ini_lskip(value);
				ini_rstrip(value);

				/* Valid name[=:]value pair found, call handler */
				if ( handler( section_buf, name, value, line_no ) != 0 && first_error_line_no == 0 ) {
					// Handler returned error and no previous error recorded
					first_error_line_no = line_no;
				}
			} else if ( first_error_line_no == 0 ) {
				/* No '=' or ':' found on name[=:]value line  and no previous error recorded */
				first_error_line_no = line_no;
			}
		}
		*line_buf = '\0';
	}

	free( line_buf );
	free( section_buf );
	return first_error_line_no;
}

int parse_ini_file( ini_handler_t handler ) {
	FILE* file;
	int error;

	file = fopen( rt_setting.config_file, "r");
	if ( !file ) {
		log_msg( LOG_ERR, "Could not open %s: %s\n", rt_setting.config_file, strerror( errno ) );
		return -1;
	}
	error = parse_ini_stream( file, handler );
	fclose( file );
	return error;
}
