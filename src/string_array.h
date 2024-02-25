#ifndef _STRING_ARRAY_H_
#define _STRING_ARRAY_H_

/**
 * @file
 * @brief Compounds and functions for advanced string array handling.
 */

/**
 * A string array.
 */
struct string_array_t;

/**
 * Creates a string array with the given capacity.
 *
 * @param capacity The capacity which shall be reserved for array entries
 * @return The pointer to the allocated string array.
 */
struct string_array_t* create_string_array( size_t capacity );

/**
 * Frees a string array which has previously been allocated with
 * ::create_string_array(size_t).
 *
 * @param array The array to be freed.
 */
void free_string_array( struct string_array_t* array );

/**
 * Returns the number of actual elements in the given array.
 *
 * @param array The array.
 * @return Number of elemets in the array.
 */
size_t get_string_array_size( struct string_array_t const * array );

/**
 * Returns the string at the given position in the array.
 *
 * Note, the function does not perform any boundary checking.
 * If `pos` points to a position larger than the size of the error, the
 * behaviour is undefined.
 * The most likely consequence will be a segmentation fault.
 *
 * @param array The array.
 * @param pos The index of the string to be returned.
 * @return The string at the given position.
 */
char const* get_string_array_at( struct string_array_t const * array, size_t const pos );

/**
 * Pushes a string to the end of the given array.
 *
 * On success, this will increase the size of the array by one.
 * If the current capacity of the array is not sufficient, the array is
 * reallocated.
 * In that case the runtime of the method is linear in the current size of
 * the array, as the array has to be moved to a new position in memory.
 * The method makes a deep copy of the passed string.
 *
 * @param array The array.
 * @param str The null-terminated string to be pushed onto the array.
 * @return A pointer to the deep copy of the passed string at the end of
 * the array, or `NULL` in case of an error.
 */
char const * push_onto_string_array( struct string_array_t* array, char const * str );

/**
 * Pushes a string to the end of the given array.
 *
 * Similar to ::push_onto_string_array(string_array_t*, char const *).
 * The only difference is that the passed string does not need to be
 * null-terminated and hence the length of the string must be given
 * explicitly.
 *
 * @see ::push_onto_string_array(string_array_t*, char const*).
 * @param array The array.
 * @param str The string to be pushed onto the array.
 * @param len The length of `str`.
 * @return A pointer to the deep copy of the passed string at the end of
 * the array, or `NULL` in case of an error.
 * The deep copy is null-terminated, even if the original string was not.
 */
char const * push_onto_string_array_l( struct string_array_t* array, char const * str, size_t len );

/**
 * Sorts the strings of the array in alpabetical order.
 *
 * Sorting is performed in-place.
 *
 * @param array The array
 */
void sort_string_array( struct string_array_t* array );

/**
 * Substracts elements of one sorted array from another sorted array and
 * removes duplicates.
 *
 * For efficiency reasons, both arrays must be sorted.
 * See ::sort_string_array() for that.
 * If any array is not sorted, the result is undefined.
 *
 * @param array The array from which elements shall be removed; the array
 * is modified in place and likely smaller afterwards
 * @param diff The array with element which shall be removed from `array`.
 */
void substract_string_array( struct string_array_t * const array, struct string_array_t const * const diff );

#endif
