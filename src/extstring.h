#ifndef _EXTSTRING_H_
#define _EXTSTRING_H_

/**
 * @file
 * @brief Functions for extended string processing.
 */

/**
 * Replaces a all occurrences of a substring with another substring.
 *
 * This method allocates a new string buffer and returns it.
 * The caller must free that string buffer.
 * If nothing is replaced (e.g. if the substring is not found), the returned
 * buffer is a copy of the original string which must be freed by the caller.
 * In case of an error, the method returns `NULL`.
 *
 * Source: https://stackoverflow.com/a/779960/2690527
 *
 * @param orig The original string
 * @param old  The substring to be replaced
 * @param new  The substring for replacement
 * @return A freshly allocated buffer containing the result string or `NULL`
 * in case of an error.
 */
char* str_replace( char const * orig, char const * old, char const * new );

/**
 * Returns the passed string or the literal `"(null)"`, if argument equals `NUL`.
 *
 * @param str The string
 * @return The string or the literal `"(null)"`
 */
char const * str_or_null( char const * const str );

#endif
