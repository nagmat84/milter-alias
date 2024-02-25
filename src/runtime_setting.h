#ifndef _RUNTIME_CONFIG_H_
#define _RUNTIME_CONFIG_H_

/**
 * @file
 * @brief Compounds and functions for logging
 */

#include <ldap.h>

/**
 * The application version (e.g. `"1.2.3"`).
 */
extern char const * const VERSION;

/**
 * Constant to indicate that application shall run in foreground.
 *
 * @see rt_setting_t::daemon_mode
 */
extern int const DAEMON_MODE_FOREGROUND;

/**
 * Constant to indicate that application shall run in background.
 *
 * @see rt_setting_t::daemon_mode
 */
extern int const DAEMON_MODE_FORK;

/**
 * Constant to indicate that application shall run in background and send
 * notifications to systemd.
 *
 * @see rt_setting_t::daemon_mode
 */
extern int const DAEMON_MODE_SYSTEMD;

/**
 * Keeps information for binding to LDAP server.
 */
struct ldap_bind_t {
	char* host; /**< The hostname */
	char* dn; /**< The DN of the entity used for authentication */
	char* passwd; /**< The password */
};

/**
 * Stores parameters for an LDAP query.
 */
struct ldap_query_parms_t {
	char* base_dn; /**< The DN used as the query's base */
	/**
	 * The LDAP filter
	 *
	 * The filter may contain the follwing special placeholders:
	 *
	 *  - `%u` is replaced by the authenticated user ID
	 *  - `%d` is replaced by the domain part of the authenticated user ID
	 *  - `%n` is replaced by the local part of the authanticated user ID
	 *
	 * If the user ID does not match the pattern `local-part@domain`, then
	 * the local part, i.e `%n`, will equal the entire user ID and the domain
	 * part, i.e. `%d`, will be empty.
	 */
	char* filter_template;
	/**
	 * NULL-terminated array of attributes to be queried
	 *
	 * Currently, only a single attribute per query is supported.
	 * Hence, the array has constant size 2 and `result_attributes[1] == NULL`
	 * always holds.
	 */
	char* result_attributes[2];
};

/**
 * Holds the current runtime settings and state of the application.
 */
struct rt_setting_t {
	int shall_print_usage; /**< Non-zero, if usage shall be printed to CLI. */
	int shall_print_version; /**< Non-zero, if application version shall be printed to CLI. */
	int daemon_mode; /**< Either ::DAEMON_MODE_FOREGROUND, ::DAEMON_MODE_FORK or ::DAEMON_MODE_SYSTEMD. */
	int is_daemonized; /**< Non-zero, after the application has been daemonized. */
	char* config_file; /**< Path to the application's INI-file. */
	char* pid_file; /**< Path to the application's PID file. */
	char* socket_file; /**< Path to the application's milter socket. */
	struct ldap_bind_t ldap_bind; /**< LDAP binding setting. */
	struct ldap_query_parms_t ldap_mail_acct_query; /**< Definition of LDAP query to receive mail addresses for a user account. */
	struct ldap_query_parms_t ldap_mail_list_query; /**< Definition of LDAP query to receive mail members of a mainling list. */
	char* log_ident; /**< Identity to be used for logging. */
	int log_facility; /**< Facility to be used for logging. */
	int log_level; /**< Treshold level to be used for logging. */
};

/**
 * Global instance which holds the current runtime settings and state of the application.
 */
extern struct rt_setting_t rt_setting;

/**
 * @brief Prints usage information to CLI.
 * @param prog_name Name of program.
 */
void print_usage( char const * const prog_name );

/**
 * @brief Prints version information to CLI.
 * @param prog_name Name of program.
 */
void print_version( char const * const prog_name );

/**
 * @brief Initializes the global object ::rt_setting.
 *
 * @return Zero on success, non-zero on failure
 */
int init_rt_setting( void );

/**
 * @brief Cleans up the global object ::rt_setting.
 */
void cleanup_rt_setting( void );

/**
 * @brief Parses the CLI options and stores them in ::rt_setting.
 *
 * @param argc Number of CLI arguments
 * @param argv Array of CLI arguments
 * @return Zero on success, non-zero on failure
 */
int parse_cli( int argc, char* argv[] );

/**
 * Parses the application's INI-file and stores the result in ::rt_setting.
 */
int parse_ini( void );

/**
 * Logs the current runtime setting as stored in ::rt_setting with log level
 * "INFO".
 */
void log_rt_settings( void );

#endif
