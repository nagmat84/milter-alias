#ifndef _SERVICE_MANAGER_H_
#define _SERVICE_MANAGER_H_

/**
 * @file
 * @brief Functions for communication with the service manager (i.e. systemd).
 */

/**
 * Notifies the service manager that the application is ready to filter mails.
 *
 * If the application is not deamonized (i.e. if ::rt_setting_t::is_daemonized
 * equals `0`) or the daemon mode is not set to SystemD (i.e if
 * ::rt_setting_t::daemon_mode is unequal to ::DAEMON_MODE_SYSTEMD ), then
 * this function is a no-op.
 */
void notify_sm_ready( void );

/**
 * Notifies the service manager that the application is about to terminate
 * gracefully after it has been requested so.
 *
 * If the application is not deamonized (i.e. if ::rt_setting_t::is_daemonized
 * equals `0`) or the daemon mode is not set to SystemD (i.e if
 * ::rt_setting_t::daemon_mode is unequal to ::DAEMON_MODE_SYSTEMD ), then
 * this function is a no-op.
 */
void notify_sm_terminated( void );

/**
 * Notifies the service manager that the application failed to start and
 * is about to terminate with an error.
 *
 * If the application is not deamonized (i.e. if ::rt_setting_t::is_daemonized
 * equals `0`) or the daemon mode is not set to SystemD (i.e if
 * ::rt_setting_t::daemon_mode is unequal to ::DAEMON_MODE_SYSTEMD ), then
 * this function is a no-op.
 *
 * @param exit_code The exit code of the application; should be the same
 * as returned by the application main function.
 * @param error_msg An error message associated to the exit code.
 */
void notify_sm_failed( int exit_code, char const * const error_msg );

#endif
