#include <stdlib.h>
#include <check.h>

Suite* create_ext_string_suite( void );

int main( void ) {

	Suite* const s = create_ext_string_suite();
	SRunner* const sr = srunner_create( s );

	srunner_run_all( sr, CK_NORMAL );
	int const number_failed = srunner_ntests_failed( sr );
	srunner_free( sr );
	return ( number_failed == 0 ) ? EXIT_SUCCESS : EXIT_FAILURE;
}
