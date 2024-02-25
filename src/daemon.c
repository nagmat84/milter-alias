#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>

#include "daemon.h"
#include "log.h"
#include "runtime_setting.h"

static char const * const DEV_NULL = "/dev/null";

static int become_session_leader( void ) {
	if( setsid() == -1 ) {
		log_msg( LOG_CRIT, "setsid failed: %s\n", strerror( errno ) );
		return -1;
	}
	if( chdir( "/" ) == -1 ) {
		log_msg( LOG_ERR, "chdir failed: %s\n", strerror( errno ) );
		return -1;
	}
	return 0;
}

static int detach_std_io( void ) {
	int const fd = open( DEV_NULL, O_RDWR );
	if( fd == -1 ) {
		log_msg( LOG_ERR, "could not open %s: %s\n", DEV_NULL, strerror( errno ) );
		return -1;
	}
	if( dup2( fd, STDIN_FILENO ) == -1 ) {
		log_msg( LOG_ERR, "could not redirect STDIN to %s: %s\n", DEV_NULL, strerror( errno ) );
		return -1;
	}
	if( dup2( fd, STDOUT_FILENO ) == -1 ) {
		log_msg( LOG_ERR, "could not redirect STDOUT to  %s: %s\n", DEV_NULL, strerror( errno ) );
		return -1;
	}
	if( dup2( fd, STDERR_FILENO ) == -1 ) {
		log_msg( LOG_ERR, "could not redirect STDERR to  %s: %s\n", DEV_NULL, strerror( errno ) );
		return -1;
	}
	if( fd > STDERR_FILENO ) {
		if( close( fd ) == -1 ) {
			log_msg( LOG_ERR, "could not close %s: %s\n", DEV_NULL, strerror( errno ) );
			return -1;
		}
	}
	return 0;
}

static int create_pid_file( void ) {
	if( rt_setting.pid_file != NULL ) {
		if( unlink( rt_setting.pid_file ) != 0 && errno != ENOENT ) {
			log_msg( LOG_WARNING, "could not unlink old pid file %s: %s", rt_setting.pid_file, strerror( errno ) );
		}
		int const save_umask = umask( 022 );
		FILE* pid_file = fopen( rt_setting.pid_file, "w" );
		umask( save_umask );
		if( pid_file == NULL ) {
			log_msg( LOG_ERR, "could not open %s: %s\n", rt_setting.pid_file, strerror( errno ) );
			return -1;
		}
		fprintf( pid_file, "%ld\n", (long) getpid() );
		if( ferror( pid_file ) ) {
			log_msg( LOG_ERR, "could not write to pid file %s: %s\n", rt_setting.pid_file, strerror( errno ) );
			clearerr( pid_file );
			fclose( pid_file );
			return -1;
		}
		if( fclose( pid_file ) != 0 ) {
			log_msg( LOG_ERR, "could not close pid file %s: %s\n", rt_setting.pid_file, strerror( errno ) );
			return -1;
		}
	}
	return 0;
}

int deamonize( void ) {
	pid_t const pid = fork();
	if( pid != 0 ) // either parent (of succesful fork) or error
		return pid;
	// Child process
	rt_setting.is_daemonized = 1;
	if( become_session_leader() != 0 )
		return -1;
	if( detach_std_io() != 0 )
		return -1;
	if( create_pid_file() != 0 )
		return -1;
	return 0;
}
