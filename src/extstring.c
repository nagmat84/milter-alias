#include <stdlib.h>
#include <string.h>

#include "extstring.h"

char* str_replace( char const * orig, char const * old, char const * new ) {
	char *result; // the return string
	char const * ins;    // the next insert point
	char * tmp;
	int len_old;  // length of old (the string to replace)
	int len_new;  // length of new (the string to replace old with)
	int len_front; // distance between current occurence of `old` and end of previous occurence of `old`
	int count;    // number of replacements

	if ( orig == NULL )
		return NULL;
	if( old == NULL ) {
		result = malloc( strlen(orig) + 1 );
		strcpy( result, orig );
		return result;
	}
	len_old = strlen( old );
	if ( len_old == 0 ) {
		result = malloc( strlen(orig) + 1 );
		strcpy( result, orig );
		return result;
	}
	if ( new == NULL )
		new = "";
	len_new = strlen( new );

	// count the number of replacements needed
	ins = orig;
	for ( count = 0; (tmp = strstr(ins, old)); ++count ) {
		ins = tmp + len_old;
	}

	tmp = result = malloc( strlen( orig ) + (len_new - len_old) * count + 1 );

	if ( result == NULL )
		return NULL;

	// first time through the loop, all the variable are set correctly
	// from here on,
	//    tmp points to the end of the result string processed so far
	//    ins points to the next occurrence of `old` in orig
	//    orig points to the remainder of orig after "end of `old`"
	while ( count-- ) {
		ins = strstr( orig, old );
		len_front = ins - orig;
		tmp = strncpy( tmp, orig, len_front) + len_front;
		tmp = strcpy( tmp, new) + len_new;
		orig += len_front + len_old; // move to next "end of `old`"
	}
	strcpy( tmp, orig );
	return result;
}

char const * str_or_null( char const * const str ) {
	return str ? str : "(null)";
}
