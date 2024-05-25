#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "extfile.h"

int mkpdir( char const * const pathname, mode_t const mode ) {
	char const * const lpos = strrchr( pathname, '/');
	if( lpos == NULL || lpos == pathname ) return 0;

	size_t const len = lpos - pathname;
	char * const dirpath = malloc( len + 1 );
	strncpy( dirpath, pathname, len );
	dirpath[len] = '\0';

	int const result = mkdir( dirpath, mode );

	free( dirpath );
	return result;
}
