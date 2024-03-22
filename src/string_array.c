#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "string_array.h"

/**
 * A string array.
 */
struct string_array_t {
	/**
	 * The capacity of the array.
	 *
	 * This is the number of elements which can be stored in the array without
	 * reallocation.
	 */
	size_t capacity;
	size_t size; /**< The number of actual elements in the array; always smaller than capacity. */
	char** values; /**< The internal pointer to the beginning of the memory block */
};

struct string_array_t* create_string_array( size_t capacity ) {
	struct string_array_t* result = malloc( sizeof( struct string_array_t ) );
	if( result == NULL )
		return NULL;
	result->capacity = capacity;
	result->size = 0;
	result->values = malloc( (capacity + 1) * sizeof( char* ) );
	if( result->values == NULL ) {
		free( result );
		return NULL;
	}
	result->values[0] = NULL;
	return result;
}

void free_string_array( struct string_array_t* array ) {
	for( size_t i = 0; i != array->size; ++i ) {
		free( array->values[i] );
	}
	free( array->values );
	free( array );
}

size_t get_string_array_size( struct string_array_t const * array ) {
	return array->size;
}

char const* get_string_array_at( struct string_array_t const * array, size_t const pos ) {
	return array->values[pos];
}

char const * push_onto_string_array( struct string_array_t* array, char const * str ) {
	return push_onto_string_array_l( array, str, strlen( str ) );
}

char const * push_onto_string_array_l( struct string_array_t* array, char const * str, size_t len ) {
	if( array->size == array->capacity ) {
		array->capacity *= 2;
		char** new_buf = realloc( array->values, (array->capacity + 1) * sizeof( char* ) );
		if ( new_buf == NULL ) {
			// could not increase array, return NULL to indicate error
			return NULL;
		}
		array->values = new_buf;
	}
	array->values[ array->size ] = malloc( len + 1 );
	if ( array->values[ array->size ] != NULL ) {
		strncpy( array->values[ array->size ], str, len );
		array->values[ array->size ][len] = '\0';
	}
	array->size++;
	array->values[ array->size ] = NULL;
	// Note: If we were able to increase the array size, but could allocate
	// memory to copy the string, the line below returns `NULL` (instead of
	// a pointer to the newly allocated string) which is actually what we want
	// to indicate an error.
	return array->values[ array->size - 1 ];
}

static int comp( void const * a, void const * b ) {
	char const * const * const aa = (char const * const *)a;
	char const * const * const bb = (char const * const *)b;
	return strcmp( *aa, *bb );
}

void sort_string_array( struct string_array_t* array ) {
	qsort( array->values, array->size, sizeof(char*), comp );
}

void substract_string_array( struct string_array_t * const array, struct string_array_t const * const diff ) {
	size_t i1 = 0; // the smaller position in `array` where the next element to-be-kept is inserted
	size_t i2 = 0; // the larger position in `array` of the element to inspect next
	size_t j = 0; // the position in `diff` of the element to compare to
	int comp_res = 0;
	while( i2 != array->size && j != diff->size ) {
		comp_res = strcmp( array->values[i2], diff->values[j] );
		if ( comp_res == 0 ) {
			// `array[i2]` is identical to `diff[j]`
			// advance `i2` as `array[i2]` is discarded from result
			// do not advance `j` as elements are not required to be unique (only
			// sorted) and `array[i2+1]` might equal `diff[j]` as well which needs
			// to be discarded in the next iteration as well
			free( array->values[i2] );
			++i2;
		} else if ( comp_res < 0 ) {
			// `array[i2]` is smaller than `diff[j]`
			// this means `array[i2]` is not an element of `diff` and must be kept
			// If `array[i2]` is not yet stored at `array[i1-1], store `array[i2]`
			// at `array[i1]` for keeping and anvance both positions
			// If `array[i2]` is already stored at `array[i1-1]` (note elements of
			// `array` are not required to be unique), then discard `array[i2]`
			// because we already have it once and only advance `i2`
			if( i1 > 0 && strcmp( array->values[i1-1], array->values[i2] ) == 0 ) {
				free( array->values[i2] );
				++i2;
			} else {
				array->values[i1] = array->values[i2];
				++i1;
				++i2;
			}
		} else {
			// `array[i2]` is larger than `diff[j]`
			// this means `array[i2]` passed by `diff[j]`
			// advance `j` and try again
			++j;
		}
	}
	// If we reached the end of `diff` but not of `array` keep the remaining
	// elements in `array` but skip duplicates
	while( i2 != array->size ) {
		if( i1 > 0 && strcmp( array->values[i1-1], array->values[i2] ) == 0 ) {
			free( array->values[i2] );
			++i2;
		} else {
			array->values[i1] = array->values[i2];
			++i1;
			++i2;
		}
	}
	// terminate list with `NULL` and adjust size
	array->values[i1] = NULL;
	array->size = i1;
}
