#include <unistd.h>
#include <systemd/sd-daemon.h>
#include <string.h>
#include <errno.h>

#include "service_manager.h"
#include "runtime_setting.h"

static uint64_t const TIMEOUT_BARRIER = 5000000;

void notify_sm_ready( void ) {
	if (
		rt_setting.is_daemonized &&
		rt_setting.daemon_mode == DAEMON_MODE_SYSTEMD
	)
		sd_notifyf(
			0,
			"READY=1\n"
			"STATUS=Started successfully\n"
			"MAINPID=%lu\n",
			(unsigned long) getpid()
		);
}

void notify_sm_terminated( void ) {
	if (
		rt_setting.is_daemonized &&
		rt_setting.daemon_mode == DAEMON_MODE_SYSTEMD
	) {
		sd_notify(
			0,
			"STOPPING=1\n"
			"ERRNO=0"
			"STATUS=Exited successfully"
		);
		sd_notify_barrier( 0, TIMEOUT_BARRIER );
	}
}

void notify_sm_failed( int exit_code, char const * const error_msg ) {
	if (
		rt_setting.is_daemonized &&
		rt_setting.daemon_mode == DAEMON_MODE_SYSTEMD
	) {
		sd_notifyf(
			0,
			"STOPPING=1\n"
			"ERRNO=%d\n"
			"STATUS=Exited with failure: %s\n",
			exit_code,
			error_msg
		);
		sd_notify_barrier( 0, TIMEOUT_BARRIER );
	}
}
