#include <stdlib.h>
#include <sysexits.h>
#include <unistd.h>

#include "smfi.h"
#include "runtime_setting.h"
#include "log.h"
#include "daemon.h"
#include "service_manager.h"
#include "extldap.h"

int main( int argc, char* argv[] ) {
	int result_code = init_rt_setting();
	if( result_code != EX_OK ) {
		return result_code;
	}

	result_code = parse_cli( argc, argv);
	if( result_code != EX_OK ) {
		cleanup_rt_setting();
		return result_code;
	}

	if( rt_setting.shall_print_usage ) {
		print_usage( argv[0] );
		cleanup_rt_setting();
		return EX_OK;
	}

	if( rt_setting.shall_print_version ) {
		print_version( argv[0] );
		cleanup_rt_setting();
		return EX_OK;
	}

	result_code = parse_ini();
	if( result_code != EX_OK ) {
		cleanup_rt_setting();
		return result_code;
	}

	if( rt_setting.daemon_mode != DAEMON_MODE_FOREGROUND ) {
		result_code = deamonize();
		if( result_code > 0 ) // parent process of successful fork
			return EX_OK;
		if( result_code == -1 ) { // error (either parent or child)
			notify_sm_failed( EX_OSERR, "could not fork" );
			return EX_OSERR;
		}
	}

	open_log();
	log_msg( LOG_NOTICE, "%s started\n", argv[0] );
	notify_sm_ready();

	log_rt_settings();

	result_code = setup_smfi();
	if( result_code != EX_OK ) {
		log_msg( LOG_NOTICE, "%s terminating ...\n", argv[0] );
		notify_sm_failed( result_code, "initialization failed" );
		close_log();
		cleanup_rt_setting();
		return result_code;
	}

	result_code = connect_ldap();
	if( result_code != EX_OK ) {
		log_msg( LOG_NOTICE, "%s terminating ...\n", argv[0] );
		notify_sm_failed( result_code, "initialization failed" );
		close_log();
		cleanup_rt_setting();
		return result_code;
	}

	result_code = smfi_main();
	if( result_code != MI_SUCCESS ) {
		result_code = EX_SOFTWARE;
		log_msg( LOG_ERR, "smfi_main failed\n" );
		notify_sm_failed( result_code, "main filter loop failed" );
	}

	result_code = disconnect_ldap();
	if( result_code != EX_OK ) {
		log_msg( LOG_ERR, "smfi_main failed\n" );
		notify_sm_failed( result_code, "disconnecting from LDAP failed" );
	}

	result_code = cleanup_smfi();
	if( result_code != EX_OK ) {
		result_code = EX_SOFTWARE;
		log_msg( LOG_ERR, "cleanup_smfi failed\n" );
		notify_sm_failed( result_code, "cleanup of mail filter failed" );
	}

	result_code = cleanup_daemon();
	if( result_code != EX_OK ) {
		result_code = EX_SOFTWARE;
		log_msg( LOG_ERR, "cleanup_daemon failed\n" );
		notify_sm_failed( result_code, "cleanup of daemon failed" );
	}

	log_msg( LOG_NOTICE, "%s terminating ...\n", argv[0] );
	notify_sm_terminated();
	close_log();
	cleanup_rt_setting();
	return result_code;
}
