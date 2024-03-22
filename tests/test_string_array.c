#include <stdlib.h>
#include <check.h>

#include "../src/string_array.h"

char const * const TEST_STRING_1 = "test string 1";
char const * const TEST_STRING_2 = "test string 2";
char const * const TEST_STRING_3 = "test string 3";

/**
 * Identical to string_array_t.
 * Used for whitebox testing.
 */
struct string_array_test_t {
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

START_TEST( test_create_string_array_with_zero_capacity ) {
	struct string_array_test_t* arr = (struct string_array_test_t*) create_string_array( 0 );
	ck_assert_ptr_nonnull( arr );
	ck_assert_int_eq( arr->capacity, 0 );
	ck_assert_int_eq( arr->size, 0 );
	ck_assert_ptr_nonnull( arr->values );
	ck_assert_ptr_null( arr->values[0] );
	free_string_array( (struct string_array_t*) arr );
}
END_TEST

START_TEST( test_create_string_array_with_non_zero_capacity ) {
	struct string_array_test_t* arr = (struct string_array_test_t*) create_string_array( 20 );
	ck_assert_ptr_nonnull( arr );
	ck_assert_int_eq( arr->capacity, 20 );
	ck_assert_int_eq( arr->size, 0 );
	ck_assert_ptr_nonnull( arr->values );
	ck_assert_ptr_null( arr->values[0] );
	for( uintptr_t i = 1; i != 20; ++i ) {
		// Just assign some dummy pointer values to each entry in the array
		// as a quick-and-dirty test that the size of the underlying array is
		// indeed the capacity.
		// If it was not, this write access should trigger a segmentation fault
		// during tests with some luck.
		arr->values[i] = (char*)i;
	}
	free_string_array( (struct string_array_t*) arr );
}
END_TEST

START_TEST( test_get_string_array_size ) {
	struct string_array_t* arr = create_string_array( 1 );
	ck_assert_int_eq( get_string_array_size( arr ), 0 );
	push_onto_string_array( arr, TEST_STRING_1 );
	ck_assert_int_eq( get_string_array_size( arr ), 1 );
	free_string_array( arr );
}
END_TEST

START_TEST( test_push_onto_string_array ) {
	struct string_array_t* arr = create_string_array( 1 );
	push_onto_string_array( arr, TEST_STRING_1 );
	struct string_array_test_t* arr_test = (struct string_array_test_t*)arr;
	ck_assert_ptr_nonnull( arr_test->values[0] );
	ck_assert_ptr_ne( TEST_STRING_1, arr_test->values[0] );
	ck_assert_str_eq( TEST_STRING_1, arr_test->values[0] );
	ck_assert_ptr_null( arr_test->values[1] );
	free_string_array( arr );
}
END_TEST

START_TEST( test_push_onto_string_array_l ) {
	struct string_array_t* arr = create_string_array( 1 );

	const char str_without_nul[] = { 't', 'e', 's', 't' };

	push_onto_string_array_l( arr, str_without_nul, 4 );
	struct string_array_test_t* arr_test = (struct string_array_test_t*)arr;
	ck_assert_ptr_nonnull( arr_test->values[0] );
	ck_assert_ptr_ne( str_without_nul, arr_test->values[0] );
	ck_assert_str_eq( "test", arr_test->values[0] );
	ck_assert( arr_test->values[0][4] == '\0' );
	ck_assert_ptr_null( arr_test->values[1] );
	free_string_array( arr );
}
END_TEST

START_TEST( test_string_array_reallocation ) {
	struct string_array_t* arr = create_string_array( 2 );
	push_onto_string_array( arr, TEST_STRING_1 );
	push_onto_string_array( arr, TEST_STRING_2 );
	push_onto_string_array( arr, TEST_STRING_3 );
	struct string_array_test_t* arr_test = (struct string_array_test_t*)arr;
	ck_assert_int_eq( arr_test->capacity, 4 );

	ck_assert_ptr_nonnull( arr_test->values[0] );
	ck_assert_ptr_ne( TEST_STRING_1, arr_test->values[0] );
	ck_assert_str_eq( TEST_STRING_1, arr_test->values[0] );

	ck_assert_ptr_nonnull( arr_test->values[1] );
	ck_assert_ptr_ne( TEST_STRING_2, arr_test->values[1] );
	ck_assert_str_eq( TEST_STRING_2, arr_test->values[1] );

	ck_assert_ptr_nonnull( arr_test->values[2] );
	ck_assert_ptr_ne( TEST_STRING_3, arr_test->values[2] );
	ck_assert_str_eq( TEST_STRING_3, arr_test->values[2] );

	ck_assert_ptr_null( arr_test->values[3] );
	free_string_array( arr );
}
END_TEST

START_TEST( test_sort_string_array ) {
	struct string_array_t* arr = create_string_array( 4 );
	void* old = ((struct string_array_test_t*)arr)->values;

	push_onto_string_array( arr, TEST_STRING_2 );
	push_onto_string_array( arr, TEST_STRING_1 );
	push_onto_string_array( arr, TEST_STRING_3 );
	sort_string_array( arr );
	ck_assert_str_eq( TEST_STRING_1, get_string_array_at( arr, 0 ) );
	ck_assert_str_eq( TEST_STRING_2, get_string_array_at( arr, 1 ) );
	ck_assert_str_eq( TEST_STRING_3, get_string_array_at( arr, 2 ) );

	// Sorting should be in place
	ck_assert_ptr_eq( old,  ((struct string_array_test_t*)arr)->values );

	free_string_array( arr );
}
END_TEST

START_TEST( test_substract_string_array_one_element ) {
	struct string_array_t* arr = create_string_array( 4 );
	push_onto_string_array( arr, TEST_STRING_1 );
	push_onto_string_array( arr, TEST_STRING_2 );
	push_onto_string_array( arr, TEST_STRING_3 );
	ck_assert_int_eq( get_string_array_size( arr ), 3 );

	struct string_array_t* diff = create_string_array( 1 );
	push_onto_string_array( diff, TEST_STRING_2 );

	substract_string_array( arr, diff );
	ck_assert_int_eq( get_string_array_size( arr ), 2 );
	ck_assert_str_eq( TEST_STRING_1, get_string_array_at( arr, 0 ) );
	ck_assert_str_eq( TEST_STRING_3, get_string_array_at( arr, 1 ) );

	free_string_array( arr );
	free_string_array( diff );
}
END_TEST

START_TEST( test_substract_string_array_none ) {
	struct string_array_t* arr = create_string_array( 4 );
	push_onto_string_array( arr, TEST_STRING_1 );
	push_onto_string_array( arr, TEST_STRING_2 );
	push_onto_string_array( arr, TEST_STRING_3 );
	ck_assert_int_eq( get_string_array_size( arr ), 3 );

	struct string_array_t* diff = create_string_array( 1 );
	push_onto_string_array( diff, "doesn't exist" );

	substract_string_array( arr, diff );
	ck_assert_int_eq( get_string_array_size( arr ), 3 );
	ck_assert_str_eq( TEST_STRING_1, get_string_array_at( arr, 0 ) );
	ck_assert_str_eq( TEST_STRING_2, get_string_array_at( arr, 1 ) );
	ck_assert_str_eq( TEST_STRING_3, get_string_array_at( arr, 2 ) );

	free_string_array( arr );
	free_string_array( diff );
}
END_TEST

START_TEST( test_substract_string_array_with_duplicates ) {
	struct string_array_t* arr = create_string_array( 1 );
	push_onto_string_array( arr, TEST_STRING_1 );
	push_onto_string_array( arr, TEST_STRING_2 );
	push_onto_string_array( arr, TEST_STRING_2 );
	push_onto_string_array( arr, TEST_STRING_2 );
	push_onto_string_array( arr, TEST_STRING_3 );
	push_onto_string_array( arr, TEST_STRING_3 );
	ck_assert_int_eq( get_string_array_size( arr ), 6 );

	struct string_array_t* diff = create_string_array( 2 );
	push_onto_string_array( diff, TEST_STRING_1 );
	push_onto_string_array( diff, TEST_STRING_3 );

	substract_string_array( arr, diff );
	ck_assert_int_eq( get_string_array_size( arr ), 1 );
	ck_assert_str_eq( TEST_STRING_2, get_string_array_at( arr, 0 ) );

	free_string_array( arr );
	free_string_array( diff );
}
END_TEST

START_TEST( test_substract_string_array_with_duplicates_at_end ) {
	struct string_array_t* arr = create_string_array( 6 );
	push_onto_string_array( arr, TEST_STRING_1 );
	push_onto_string_array( arr, TEST_STRING_2 );
	push_onto_string_array( arr, TEST_STRING_2 );
	push_onto_string_array( arr, TEST_STRING_3 );
	push_onto_string_array( arr, TEST_STRING_3 );
	push_onto_string_array( arr, TEST_STRING_3 );
	ck_assert_int_eq( get_string_array_size( arr ), 6 );

	struct string_array_t* diff = create_string_array( 1 );
	push_onto_string_array( diff, TEST_STRING_2 );
	ck_assert_int_eq( get_string_array_size( diff ), 1 );

	substract_string_array( arr, diff );
	ck_assert_int_eq( get_string_array_size( arr ), 2 );
	ck_assert_str_eq( TEST_STRING_1, get_string_array_at( arr, 0 ) );
	ck_assert_str_eq( TEST_STRING_3, get_string_array_at( arr, 1 ) );

	free_string_array( arr );
	free_string_array( diff );
}
END_TEST

Suite* create_string_array_suite( void ) {
	Suite* s = suite_create( "string_array" );
	TCase* tc;

	tc = tcase_create( "test_create_string_array_with_zero_capacity" );
	tcase_add_test( tc, test_create_string_array_with_zero_capacity );
	suite_add_tcase( s, tc );

	tc = tcase_create( "test_create_string_array_with_non_zero_capacity" );
	tcase_add_test( tc, test_create_string_array_with_non_zero_capacity );
	suite_add_tcase( s, tc );

	tc = tcase_create( "test_get_string_array_size" );
	tcase_add_test( tc, test_get_string_array_size );
	suite_add_tcase( s, tc );

	tc = tcase_create( "test_push_onto_string_array" );
	tcase_add_test( tc, test_push_onto_string_array );
	suite_add_tcase( s, tc );

	tc = tcase_create( "test_push_onto_string_array_l" );
	tcase_add_test( tc, test_push_onto_string_array_l );
	suite_add_tcase( s, tc );

	tc = tcase_create( "test_string_array_reallocation" );
	tcase_add_test( tc, test_string_array_reallocation );
	suite_add_tcase( s, tc );

	tc = tcase_create( "test_sort_string_array" );
	tcase_add_test( tc, test_sort_string_array );
	suite_add_tcase( s, tc );

	tc = tcase_create( "test_substract_string_array_one_element" );
	tcase_add_test( tc, test_substract_string_array_one_element );
	suite_add_tcase( s, tc );

	tc = tcase_create( "test_substract_string_array_none" );
	tcase_add_test( tc, test_substract_string_array_none );
	suite_add_tcase( s, tc );

	tc = tcase_create( "test_substract_string_array_with_duplicates" );
	tcase_add_test( tc, test_substract_string_array_with_duplicates );
	suite_add_tcase( s, tc );

	tc = tcase_create( "test_substract_string_array_with_duplicates_at_end" );
	tcase_add_test( tc, test_substract_string_array_with_duplicates_at_end );
	suite_add_tcase( s, tc );

	return s;
}
