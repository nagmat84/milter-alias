#define _DEFAULT_SOURCE
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "log.h"
#include "runtime_setting.h"

static char const * const LOG_LEVEL_EMERG_STRINGS[] = { "EMERGENCY", "emergency", "EMERG", "emerg", "0", NULL };
static char const * const LOG_LEVEL_ALERT_STRINGS[] = { "ALERT", "alert", "1", NULL };
static char const * const LOG_LEVEL_CRITICAL_STRINGS[] = { "CRITICAL", "critical", "CRIT", "crit", "2", NULL };
static char const * const LOG_LEVEL_ERROR_STRINGS[] = { "ERROR", "error", "ERR", "err", "3", NULL };
static char const * const LOG_LEVEL_WARNING_STRINGS[] = { "WARNING", "warning", "WARN", "warn", "4", NULL };
static char const * const LOG_LEVEL_NOTICE_STRINGS[] = { "NOTICE", "notice", "5", NULL };
static char const * const LOG_LEVEL_INFO_STRINGS[] = { "INFO", "info", "6", NULL };
static char const * const LOG_LEVEL_DEBUG_STRINGS[] = { "DEBUG", "debug", "7", NULL };

static char const * const * const LOG_LEVEL_2_STRINGS[] = {
	LOG_LEVEL_EMERG_STRINGS,
	LOG_LEVEL_ALERT_STRINGS,
	LOG_LEVEL_CRITICAL_STRINGS,
	LOG_LEVEL_ERROR_STRINGS,
	LOG_LEVEL_WARNING_STRINGS,
	LOG_LEVEL_NOTICE_STRINGS,
	LOG_LEVEL_INFO_STRINGS,
	LOG_LEVEL_DEBUG_STRINGS
};

static char const * const LOG_FACILITY_USER_STRINGS[] = { "USER", "user", NULL };
static char const * const LOG_FACILITY_MAIL_STRINGS[] = { "MAIL", "mail", NULL };
static char const * const LOG_FACILITY_DAEMON_STRINGS[] = { "DAEMON", "daemon", NULL };
static char const * const LOG_FACILITY_LOCAL0_STRINGS[] = { "LOCAL0", "local0", "LOCAL 0", "local 0", "LOCAL_0", "local_0", NULL };
static char const * const LOG_FACILITY_LOCAL1_STRINGS[] = { "LOCAL1", "local1", "LOCAL 1", "local 1", "LOCAL_1", "local_1", NULL };
static char const * const LOG_FACILITY_LOCAL2_STRINGS[] = { "LOCAL2", "local2", "LOCAL 2", "local 2", "LOCAL_2", "local_2", NULL };
static char const * const LOG_FACILITY_LOCAL3_STRINGS[] = { "LOCAL3", "local3", "LOCAL 3", "local 3", "LOCAL_3", "local_3", NULL };
static char const * const LOG_FACILITY_LOCAL4_STRINGS[] = { "LOCAL4", "local4", "LOCAL 4", "local 4", "LOCAL_4", "local_4", NULL };
static char const * const LOG_FACILITY_LOCAL5_STRINGS[] = { "LOCAL5", "local5", "LOCAL 5", "local 5", "LOCAL_5", "local_5", NULL };
static char const * const LOG_FACILITY_LOCAL6_STRINGS[] = { "LOCAL6", "local6", "LOCAL 6", "local 6", "LOCAL_6", "local_6", NULL };
static char const * const LOG_FACILITY_LOCAL7_STRINGS[] = { "LOCAL7", "local7", "LOCAL 7", "local 7", "LOCAL_7", "local_7", NULL };

static char const * const * const LOG_FACILITY_2_STRINGS[] = {
	NULL, /* KERN */
	LOG_FACILITY_USER_STRINGS,
	LOG_FACILITY_MAIL_STRINGS,
	LOG_FACILITY_DAEMON_STRINGS,
	NULL, /* AUTH */
	NULL, /* SYSLOG */
	NULL, /* LPR */
	NULL, /* NEWS */
	NULL, /* UUCP */
	NULL, /* CRON */
	NULL, /* AUTHPRIV */
	NULL, /* FTP */
	NULL, /* (12) RESERVED FOR SYSTEM */
	NULL, /* (13) RESERVED FOR SYSTEM */
	NULL, /* (14) RESERVED FOR SYSTEM */
	NULL, /* (15) RESERVED FOR SYSTEM */
	LOG_FACILITY_LOCAL0_STRINGS,
	LOG_FACILITY_LOCAL1_STRINGS,
	LOG_FACILITY_LOCAL2_STRINGS,
	LOG_FACILITY_LOCAL3_STRINGS,
	LOG_FACILITY_LOCAL4_STRINGS,
	LOG_FACILITY_LOCAL5_STRINGS,
	LOG_FACILITY_LOCAL6_STRINGS,
	LOG_FACILITY_LOCAL7_STRINGS
};

void open_log( void ) {
	if( rt_setting.daemon_mode != DAEMON_MODE_FOREGROUND )
		openlog( rt_setting.log_ident, LOG_NDELAY, rt_setting.log_facility );
}

void close_log(void) {
	closelog();
}

void log_msg( int const priority, char const * const format, ... ) {
	if( rt_setting.log_level < priority )
		return;
	va_list args;
	va_start( args, format );
	if( rt_setting.is_daemonized ) {
		vsyslog( priority | rt_setting.log_facility, format, args );
	} else if( rt_setting.daemon_mode != DAEMON_MODE_FOREGROUND ) {
		vsyslog( priority | rt_setting.log_facility, format, args );
		vfprintf( stdout, format, args );
	} else {
		vfprintf( stdout, format, args );
	}
	va_end( args );
}

char const * convert_log_level_2_str( int const log_level ) {
	if ( LOG_EMERG <= log_level && log_level <= LOG_DEBUG )
		return LOG_LEVEL_2_STRINGS[log_level][0];
	return NULL;
}

int convert_str_2_log_level( char const * const str ) {
	for( int i = LOG_EMERG; i <= LOG_DEBUG; ++i ) {
		for( int j = 0; LOG_LEVEL_2_STRINGS[i][j] != NULL; ++j ) {
			if ( strcmp( LOG_LEVEL_2_STRINGS[i][j], str ) == 0 )
				return i;
		}
	}
	return -1;
}

char const * convert_log_facility_2_str( int log_facility ) {
	if ( LOG_KERN <= log_facility && log_facility <= LOG_LOCAL7 ) {
		char const * const * const arr =  LOG_FACILITY_2_STRINGS[log_facility >> 3];
		if ( arr != NULL )
			return arr[0];
	}
	return NULL;
}

int convert_str_2_log_facility( char const * const str ) {
	for( int i = (LOG_KERN >> 3); i <= (LOG_LOCAL7 >> 3); ++i ) {
		if ( LOG_FACILITY_2_STRINGS[i] == NULL )
			continue;
		for( int j = 0; LOG_FACILITY_2_STRINGS[i][j] != NULL; ++j ) {
			if ( strcmp( LOG_FACILITY_2_STRINGS[i][j], str ) == 0 )
				return i << 3;
		}
	}
	return -1;
}
