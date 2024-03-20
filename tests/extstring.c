#include <stdlib.h>
#include <check.h>

#include "../src/extstring.h"

static char const * const TEST_STRING = "test string";

START_TEST( test_str_replace_all_null ) {
	ck_assert_ptr_null( str_replace( NULL, NULL, NULL ) );
}
END_TEST

START_TEST( test_str_replace_null ) {
	char * result = str_replace( TEST_STRING, NULL, NULL );
	ck_assert_ptr_nonnull( result );
	ck_assert_ptr_ne( result, TEST_STRING );
	ck_assert_str_eq( result, TEST_STRING );
	free( result );
}
END_TEST

START_TEST( test_str_replace_with_null ) {
	char * result = str_replace( TEST_STRING, "string", NULL );
	ck_assert_ptr_nonnull( result );
	ck_assert_ptr_ne( result, TEST_STRING );
	ck_assert_str_eq( result, "test " );
	free( result );
}
END_TEST

START_TEST( test_str_replace_with_empty ) {
	char * result = str_replace( TEST_STRING, "string", "" );
	ck_assert_ptr_nonnull( result );
	ck_assert_ptr_ne( result, TEST_STRING );
	ck_assert_str_eq( result, "test " );
	free( result );
}
END_TEST

START_TEST( test_str_replace_at_end ) {
	char * result = str_replace( TEST_STRING, "string", "char string" );
	ck_assert_ptr_nonnull( result );
	ck_assert_ptr_ne( result, TEST_STRING );
	ck_assert_str_eq( result, "test char string" );
	free( result );
}
END_TEST

START_TEST( test_str_replace_at_middle ) {
	char * result = str_replace( TEST_STRING, " ", " char " );
	ck_assert_ptr_nonnull( result );
	ck_assert_ptr_ne( result, TEST_STRING );
	ck_assert_str_eq( result, "test char string" );
	free( result );
}
END_TEST

START_TEST( test_str_replace_at_start ) {
	char * result = str_replace( TEST_STRING, "test", "trial" );
	ck_assert_ptr_nonnull( result );
	ck_assert_ptr_ne( result, TEST_STRING );
	ck_assert_str_eq( result, "trial string" );
	free( result );
}
END_TEST

START_TEST( test_str_replace_all ) {
	char * result = str_replace( TEST_STRING, "test string", "" );
	ck_assert_ptr_nonnull( result );
	ck_assert_ptr_ne( result, TEST_STRING );
	ck_assert_str_eq( result, "" );
	free( result );
}
END_TEST

START_TEST( test_str_or_null_with_null ) {
	ck_assert_str_eq( str_or_null( NULL ), "(null)" );
}
END_TEST

START_TEST( test_str_or_null_with_str ) {
	ck_assert_str_eq( str_or_null( TEST_STRING ), TEST_STRING );
}
END_TEST

Suite* create_ext_string_suite( void ) {
	Suite* s = suite_create( "ext_string" );
	TCase* tc;

	tc = tcase_create( "test_str_replace_all_null" );
	tcase_add_test( tc, test_str_replace_all_null );
	suite_add_tcase( s, tc );

	tc = tcase_create( "test_str_replace_null" );
	tcase_add_test( tc, test_str_replace_null );
	suite_add_tcase( s, tc );

	tc = tcase_create( "test_str_replace_with_null" );
	tcase_add_test( tc, test_str_replace_with_null );
	suite_add_tcase( s, tc );

	tc = tcase_create( "test_str_replace_with_empty" );
	tcase_add_test( tc, test_str_replace_with_empty );
	suite_add_tcase( s, tc );

	tc = tcase_create( "test_str_replace_at_end" );
	tcase_add_test( tc, test_str_replace_at_end );
	suite_add_tcase( s, tc );

	tc = tcase_create( "test_str_replace_at_middle" );
	tcase_add_test( tc, test_str_replace_at_middle );
	suite_add_tcase( s, tc );

	tc = tcase_create( "test_str_replace_at_start" );
	tcase_add_test( tc, test_str_replace_at_start );
	suite_add_tcase( s, tc );

	tc = tcase_create( "test_str_replace_all" );
	tcase_add_test( tc, test_str_replace_all );
	suite_add_tcase( s, tc );

	tc = tcase_create( "test_str_or_null_with_null" );
	tcase_add_test( tc, test_str_or_null_with_null );
	suite_add_tcase( s, tc );

	tc = tcase_create( "test_str_or_null_with_str" );
	tcase_add_test( tc, test_str_or_null_with_str );
	suite_add_tcase( s, tc );

	return s;
}
