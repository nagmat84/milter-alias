#ifndef _EXTFILE_H_
#define _EXTFILE_H_

#include <sys/types.h>

/**
 * @file
 * @brief Functions for extended file handling.
 */

/**
 * Creates the (parent) directory for the given path.
 *
 * The method truncates the provided pathname at the last separator (`'/'`)
 * and creates the resulting directory.
 * This means, if the pathname does not end with a separator (e.g.
 * `"/foo/bla/something"`), then the last component is considered to
 * as a file name and the last-but-one component is created (e.g. `"bla"`).
 * If the pathname does end with a separator (e.g. `"/foo/bla/something/"`),
 * then the last component is considered a directory and is created (e.g.
 * `"something"`).
 *
 * The arguments and return value are the same as for POSIX `mkdir`.
 *
 * @param pathname The path to be created
 * @param mode     The mode of the new directory.
 * @return Zero on success. On error, -1 is returned and `errno` is set to
 * indicate the error.
 */
int mkpdir( char const * const pathname, mode_t const mode );

#endif
