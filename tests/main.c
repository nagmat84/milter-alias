#include <stdlib.h>
#include <check.h>

Suite* create_ext_string_suite( void );
Suite* create_string_array_suite( void );

int main( int argc, char* argv[] ) {
	SRunner* const sr = srunner_create( NULL );
	srunner_add_suite( sr, create_ext_string_suite() );
	srunner_add_suite( sr, create_string_array_suite() );

	if( argc == 0 || argc == 1 ) {
		srunner_set_fork_status( sr, CK_FORK );
	} else if (
		argc == 2 && (
			strcmp( argv[1], "-f" ) == 0 || strcmp( argv[1], "--fork" ) == 0
		)
	) {
		srunner_set_fork_status( sr, CK_FORK );
	} else if (
		argc == 2 && (
			strcmp( argv[1], "-n" ) == 0 || strcmp( argv[1], "--no-fork" ) == 0
		)
	) {
		srunner_set_fork_status( sr, CK_NOFORK );
	} else {
		srunner_free( sr );
		return EXIT_FAILURE;
	}

	srunner_run_all( sr, CK_NORMAL );
	int const number_failed = srunner_ntests_failed( sr );
	srunner_free( sr );
	return ( number_failed == 0 ) ? EXIT_SUCCESS : EXIT_FAILURE;
}
