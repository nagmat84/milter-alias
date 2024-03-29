#ifndef _DAEMON_H_
#define _DAEMON_H_

/**
 * @file
 * @brief Functions for daemon management
 */

/**
 * Makes the application become a daemon.
 *
 * @return `-1` in case of an error, positive value after a successful fork
 * for the parent process, `0` after a successful fork for the child process.
 */
int deamonize( void );

/**
 * Cleans up leftovers from daemon.
 *
 * @return Zero on success, non-zero in case of failure.
 */
int cleanup_daemon( void );

#endif
