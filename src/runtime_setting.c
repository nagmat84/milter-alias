#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <getopt.h>
#include <sysexits.h>
#include <ctype.h>

#include "runtime_setting.h"
#include "log.h"
#include "ini_parser.h"
#include "extstring.h"

char const * const VERSION = "0.1.0";

static char const * const CONFIG_FILE_DEFAULT = "/etc/milter-alias.conf";

int const DAEMON_MODE_FOREGROUND = 0;

int const DAEMON_MODE_FORK = 1;

int const DAEMON_MODE_SYSTEMD = 2;

static int const DAEMON_MODE_DEFAULT = DAEMON_MODE_FORK;

static char const * const PID_FILE_DEFAULT = "/run/milter-alias/milter-alias.pid";

static char const * const SOCKET_FILE_DEFAULT = "/run/milter-alias/milter-alias.sock";

static int const LOG_FACILITY_DEFAULT = LOG_MAIL;

static int const LOG_LEVEL_DEFAULT = LOG_WARNING;

static char const * const CLI_OPTS = "c:d:fhl:p:s:v";

struct rt_setting_t rt_setting = {
	0,                               /* shall_print_usage */
	0,                               /* shall_print_version */
	DAEMON_MODE_DEFAULT,             /* daemon_mode */
	0,                               /* is_daemonized */
	NULL,                            /* config_file */
	NULL,                            /* pid_file */
	NULL,                            /* socket_file */
	{ NULL, NULL, NULL },            /* ldap_bind.{host, dn, passwd } */
	{ NULL, NULL, { NULL, NULL } },  /* ldap_mail_acct_query.{base_dn, filter_template, result_attributes } */
	{ NULL, NULL, { NULL, NULL } },  /* ldap_mail_list_query.{base_dn, filter_template, result_attributes } */
	NULL,                            /* log_ident */
	LOG_FACILITY_DEFAULT,            /* lof_facility */
	LOG_LEVEL_DEFAULT                /* log_level */
};

void print_usage( char const * const prog_name ) {
	fprintf( stdout, "Usage: %s [OPTIONS]\n", prog_name );
	fprintf( stdout, "Options are:\n" );
	fprintf( stdout, "    -c config_file  Path to config file; default: %s\n", CONFIG_FILE_DEFAULT );
	fprintf( stdout, "    -d daemon_mode  Daemon mode; 0 = foreground, 1 = traditional fork, 2 = systemd notify support; default: %d\n", DAEMON_MODE_DEFAULT );
	fprintf( stdout, "    -f              Run in the foreground\n" );
	fprintf( stdout, "    -h              Print this information and exit\n" );
	fprintf( stdout, "    -l log_level    Log level; a Syslog level such as \"debug\" or a number between 0 and 7; default: %d\n", LOG_LEVEL_DEFAULT );
	fprintf( stdout, "    -p pid_file     Path to pid file; default: %s\n", PID_FILE_DEFAULT );
	fprintf( stdout, "    -s socket_file  Path to socket; default: %s\n", SOCKET_FILE_DEFAULT );
	fprintf( stdout, "    -v              Report the version and exit\n" );
}

void print_version( char const * const prog_name ) {
	fprintf( stdout, "%s %s\n", prog_name, VERSION );
}

int init_rt_setting( void ) {
	rt_setting.config_file = malloc( strlen( CONFIG_FILE_DEFAULT ) + 1 );
	rt_setting.pid_file = malloc( strlen( PID_FILE_DEFAULT ) + 1 );
	rt_setting.socket_file = malloc( strlen( SOCKET_FILE_DEFAULT ) + 1 );

	if(
		rt_setting.config_file == NULL ||
		rt_setting.pid_file == NULL ||
		rt_setting.socket_file == NULL
	) {
		cleanup_rt_setting();
		return -1;
	}

	strcpy( rt_setting.config_file, CONFIG_FILE_DEFAULT );
	strcpy( rt_setting.pid_file, PID_FILE_DEFAULT );
	strcpy( rt_setting.socket_file, SOCKET_FILE_DEFAULT );

	return 0;
}

void cleanup_rt_setting( void ) {
	free( rt_setting.config_file );
	rt_setting.config_file = NULL;
	free( rt_setting.pid_file );
	rt_setting.pid_file = NULL;
	free( rt_setting.socket_file );
	rt_setting.socket_file = NULL;

	free( rt_setting.ldap_bind.host );
	rt_setting.ldap_bind.dn = NULL;
	free( rt_setting.ldap_bind.host );
	rt_setting.ldap_bind.dn = NULL;
	free( rt_setting.ldap_bind.passwd );
	rt_setting.ldap_bind.passwd = NULL;

	free( rt_setting.ldap_mail_acct_query.base_dn );
	rt_setting.ldap_mail_acct_query.base_dn = NULL;
	free( rt_setting.ldap_mail_acct_query.filter_template );
	rt_setting.ldap_mail_acct_query.filter_template = NULL;
	free( rt_setting.ldap_mail_acct_query.result_attributes[0] );
	rt_setting.ldap_mail_acct_query.result_attributes[0] = NULL;

	free( rt_setting.ldap_mail_list_query.base_dn );
	rt_setting.ldap_mail_list_query.base_dn = NULL;
	free( rt_setting.ldap_mail_list_query.filter_template );
	rt_setting.ldap_mail_list_query.filter_template = NULL;
	free( rt_setting.ldap_mail_list_query.result_attributes[0] );
	rt_setting.ldap_mail_list_query.result_attributes[0] = NULL;
}

static int parse_log_level( char const * const value ) {
	rt_setting.log_level = convert_str_2_log_level( value );
	if ( rt_setting.log_level == -1 ) {
		rt_setting.log_level = LOG_LEVEL_DEFAULT;
		return EX_USAGE;
	}
	return EX_OK;
}

static int parse_log_facility( char const * const value ) {
	rt_setting.log_facility = convert_str_2_log_facility( value );
	if ( rt_setting.log_facility == -1 ) {
		rt_setting.log_facility = LOG_FACILITY_DEFAULT;
		return EX_USAGE;
	}
	return EX_OK;
}

static char const * const DAEMON_MODE_STRINGS[3][5] = {
	{ "foreground", "Foreground", "FOREGROUND", "0", NULL },
	{ "fork",       "Fork",       "FORK",       "1", NULL },
	{ "systemd",    "SYSTEMD",    "Systemd",    "2", NULL }
};

char const * convert_daemon_mode_2_str( int const daemon_mode ) {
	if ( DAEMON_MODE_FOREGROUND <= daemon_mode && daemon_mode <= DAEMON_MODE_SYSTEMD )
		return DAEMON_MODE_STRINGS[daemon_mode][0];
	return NULL;
}

int convert_str_2_daemon_mode( char const * const str ) {
	for( int i = DAEMON_MODE_FOREGROUND; i <= DAEMON_MODE_SYSTEMD; ++i ) {
		for( int j = 0; DAEMON_MODE_STRINGS[i][j] != NULL; ++j ) {
			if ( strcmp( DAEMON_MODE_STRINGS[i][j], str ) == 0 )
				return i;
		}
	}
	return -1;
}

static int parse_daemon_mode( char const * const value ) {
	rt_setting.daemon_mode = convert_str_2_daemon_mode( value );
	if ( rt_setting.daemon_mode == -1 ) {
		rt_setting.daemon_mode = DAEMON_MODE_DEFAULT;
		return EX_USAGE;
	}
	return EX_OK;
}

static int parse_cli_option_with_mandatory_str( char** config, char const opt, char const * const arg ) {
	if( arg == NULL || *arg == '\0' ) {
		log_msg( LOG_CRIT, "Command line option -%c with invalid, empty argument\n", opt );
		return EX_USAGE;
	}
	*config = realloc( *config, strlen( arg ) + 1 );
	strcpy( *config, arg );
	return EX_OK;
}

int parse_cli( int argc, char* argv[] ) {
	opterr = 0;
	int c = '\0';
	int result_code = EX_OK;

	while( ( c = getopt( argc, argv, CLI_OPTS ) ) != EOF ) {
		switch( c ) {
			case 'c':
				result_code |= parse_cli_option_with_mandatory_str( &(rt_setting.config_file), 'c', optarg );
				log_msg( LOG_DEBUG, "Set config_file via command line to: %s\n", rt_setting.config_file );
				break;
			case 'd':
				if( optarg == NULL || *optarg == '\0' ) {
					log_msg( LOG_CRIT, "Command line option -d with invalid, empty argument\n" );
					result_code |= EX_USAGE;
				} else if (
					parse_daemon_mode( optarg ) != EX_OK
				) {
					log_msg( LOG_CRIT, "Command line option -d with invalid argument: %s\n", optarg );
					result_code |= EX_USAGE;
				}
				log_msg(
					LOG_DEBUG,
					"Set deamon_mode via command line to: %d (%s)\n",
					rt_setting.daemon_mode,
					convert_daemon_mode_2_str( rt_setting.daemon_mode )
				);
				break;
			case 'f':
				rt_setting.daemon_mode = DAEMON_MODE_FOREGROUND;
				log_msg(
					LOG_DEBUG,
					"Set deamon_mode via command line to: %d (%s)\n",
					rt_setting.daemon_mode,
					convert_daemon_mode_2_str( rt_setting.daemon_mode )
				);
				break;
			case 'h':
				rt_setting.shall_print_usage = 1;
				rt_setting.daemon_mode = DAEMON_MODE_FOREGROUND;
				break;
			case 'l':
				if( optarg == NULL || *optarg == '\0' ) {
					log_msg( LOG_CRIT, "Command line option -l with invalid, empty argument\n" );
					result_code |= EX_USAGE;
				} else if (
					parse_log_level( optarg ) != EX_OK
				) {
					log_msg( LOG_CRIT, "Command line option -l with invalid argument: %s\n", optarg );
					result_code |= EX_USAGE;
				}
				log_msg(
					LOG_DEBUG,
					"Set log_level via command line to: %d (%s)\n",
					rt_setting.log_level,
					convert_log_level_2_str( rt_setting.log_level )
				);
				break;
			case 'p':
				result_code |= parse_cli_option_with_mandatory_str( &(rt_setting.pid_file), 'p', optarg );
				log_msg( LOG_DEBUG, "Set pid_file via command line to: %s\n", rt_setting.pid_file );
				break;
			case 's':
				result_code |= parse_cli_option_with_mandatory_str( &(rt_setting.socket_file), 's', optarg );
				log_msg( LOG_DEBUG, "Set socket_file via command line to: %s\n", rt_setting.socket_file );
				break;
			case 'v':
				rt_setting.shall_print_version = 1;
				rt_setting.daemon_mode = DAEMON_MODE_FOREGROUND;
				break;
			case '?':
			default:
				if( isgraph( optopt ) == 0 ) {
					log_msg( LOG_CRIT, "Invalid command line\n" );
				} else {
					if( optarg == NULL ) {
						log_msg( LOG_CRIT, "Unknown command line option -%c\n", optopt );
					} else if ( *optarg == '\0' ) {
						log_msg( LOG_CRIT, "Invalid command line option -%c with empty argument\n", optopt );
					} else {
						log_msg( LOG_CRIT, "Invalid command line option -%c with argument: %s\n", optopt, optarg );
					}
				}
				result_code |= EX_USAGE;
				break;
		}
	}

	return result_code;
}

static int parse_ini_option_with_mandatory_str(
		char** config_entry,
		char const * const section,
		char const * const name,
		char const * const value,
		int const line_no
) {
	if ( value == NULL || *value == '\0' ) {
		log_msg( LOG_ERR, "Invalid, empty value in section \"%s\" for option \"%s\" at line %d\n", section, name, line_no );
		return -1;
	}
	*config_entry = realloc( *config_entry, strlen( value ) + 1 );
	strcpy( *config_entry, value );
	return 0;
}

static int parse_ini_section_general(
	char const * const section,
	char const * const name,
	char const * const value,
	int const line_no
) {
	size_t value_length = 0;

	if (
		strcmp( "DAEMON MODE", name ) == 0 ||
		strcmp( "daemon mode", name ) == 0 ||
		strcmp( "MODE", name ) == 0 ||
		strcmp( "mode", name ) == 0
	) {
		if ( parse_daemon_mode( value ) != EX_OK ) {
			log_msg( LOG_ERR, "Invalid value in section \"%s\" for option \"%s\" at line %d: %s\n", section, name, line_no, value );
			return -1;
		}
		log_msg(
			LOG_DEBUG,
			"Set deamon_mode via config file to: %d (%s)\n",
			rt_setting.daemon_mode,
			convert_daemon_mode_2_str( rt_setting.daemon_mode )
		);
	} else if (
		strcmp( "PID FILE", name ) == 0 ||
		strcmp( "pid file", name ) == 0
	) {
		free( rt_setting.pid_file );
		rt_setting.pid_file = NULL;
		value_length = strlen( value );
		if ( value_length != 0 ) {
			rt_setting.pid_file = malloc( value_length + 1 );
			strcpy( rt_setting.pid_file, value );
		}
		log_msg(
			LOG_DEBUG, "Set pid_file via config file to: %s\n", str_or_null( rt_setting.pid_file )
		);
	} else if (
		strcmp( "SOCKET FILE", name ) == 0 ||
		strcmp( "socket file", name ) == 0 ||
		strcmp( "SOCKET", name ) == 0 ||
		strcmp( "socket", name ) == 0
	) {
		int const ret = parse_ini_option_with_mandatory_str(
			&(rt_setting.socket_file),
			section,
			name,
			value,
			line_no
		);
		log_msg(
			LOG_DEBUG, "Set socket_file via config file to: %s\n", str_or_null( rt_setting.socket_file )
		);
		return ret;
	}
	return 0;
}

static int parse_ini_section_ldap(
	char const * const section,
	char const * const name,
	char const * const value,
	int const line_no
) {
	char** config_entry = NULL;
	char const * config_name = NULL;

	if (
		strcmp( "BIND HOST", name ) == 0 ||
		strcmp( "bind host", name ) == 0
	) {
		config_entry = &(rt_setting.ldap_bind.host);
		config_name = "ldap_bind.host";
	} else if (
		strcmp( "BIND DN", name ) == 0 ||
		strcmp( "bind dn", name ) == 0
	) {
		config_entry = &(rt_setting.ldap_bind.dn);
		config_name = "ldap_bind.dn";
	} else if (
		strcmp( "BIND PWD", name ) == 0 ||
		strcmp( "bind pwd", name ) == 0 ||
		strcmp( "BIND PASSWD", name ) == 0 ||
		strcmp( "bind passwd", name ) == 0 ||
		strcmp( "BIND PASSWORD", name ) == 0 ||
		strcmp( "bind password", name ) == 0
	) {
		config_entry = &(rt_setting.ldap_bind.passwd);
		config_name = "ldap_bind.passwd";
	} else if (
		strcmp( "MAIL ACCT BASE", name ) == 0 ||
		strcmp( "mail acct base", name ) == 0
	) {
		config_entry = &(rt_setting.ldap_mail_acct_query.base_dn);
		config_name = "ldap_mail_acct_query.base_dn";
	} else if (
		strcmp( "MAIL ACCT FILTER", name ) == 0 ||
		strcmp( "mail acct filter", name ) == 0
	) {
		config_entry = &(rt_setting.ldap_mail_acct_query.filter_template);
		config_name = "ldap_mail_acct_query.filter_template";
	} else if (
		strcmp( "MAIL ACCT RESULT", name ) == 0 ||
		strcmp( "mail acct result", name ) == 0
	) {
		config_entry = &(rt_setting.ldap_mail_acct_query.result_attributes[0]);
		config_name = "ldap_mail_acct_query.result_attribute";
	} else if (
		strcmp( "MAIL LIST BASE", name ) == 0 ||
		strcmp( "mail list base", name ) == 0
	) {
		config_entry = &(rt_setting.ldap_mail_list_query.base_dn);
		config_name = "ldap_mail_acct_query.base_dn";
	} else if (
		strcmp( "MAIL LIST FILTER", name ) == 0 ||
		strcmp( "mail list filter", name ) == 0
	) {
		config_entry = &(rt_setting.ldap_mail_list_query.filter_template);
		config_name = "ldap_mail_acct_query.filter_template";
	} else if (
		strcmp( "MAIL LIST RESULT", name ) == 0 ||
		strcmp( "mail list result", name ) == 0
	) {
		config_entry = &(rt_setting.ldap_mail_list_query.result_attributes[0]);
		config_name = "ldap_mail_acct_query.result_attribute";
	} else {
		log_msg( LOG_ERR, "Unknown option in section \"%s\" at line %d: %s\n", section, line_no, name );
		return -1;
	}

	int const ret = parse_ini_option_with_mandatory_str( config_entry, section, name, value, line_no );
	log_msg(
		LOG_DEBUG,
		"Set %s via config file to: %s\n",
		config_name,
		*config_entry
	);
	return ret;
}

static int parse_ini_section_log(
	char const * const section,
	char const * const name,
	char const * const value,
	int const line_no
) {
	size_t value_length = 0;

	if (
		strcmp( "IDENT", name ) == 0 ||
		strcmp( "ident", name ) == 0
	) {
		free( rt_setting.log_ident );
		rt_setting.log_ident = NULL;
		value_length = strlen( value );
		if ( value_length != 0 ) {
			rt_setting.log_ident = malloc( value_length + 1 );
			strcpy( rt_setting.log_ident, value );
		}
		log_msg(
			LOG_DEBUG, "Set log identity via config file to: %s\n", str_or_null( rt_setting.log_ident )
		);
	} else if (
		strcmp( "FACILITY", name ) == 0 ||
		strcmp( "facility", name ) == 0
	) {
		if ( parse_log_facility( value ) != EX_OK ) {
			log_msg( LOG_ERR, "Invalid value in section \"%s\" for option \"%s\" at line %d: %s\n", section, name, line_no, value );
			return -1;
		}
		log_msg(
			LOG_DEBUG,
			"Set log facility via config file to: %d (%s)\n",
			rt_setting.log_facility,
			convert_log_facility_2_str( rt_setting.log_facility )
		);
	} else if (
		strcmp( "LEVEL", name ) == 0 ||
		strcmp( "level", name ) == 0
	) {
		if ( parse_log_level( value ) != EX_OK ) {
			log_msg( LOG_ERR, "Invalid value in section \"%s\" for option \"%s\" at line %d: %s\n", section, name, line_no, value );
			return -1;
		}
		log_msg(
			LOG_DEBUG,
			"Set log level via config file to: %d (%s)\n",
			rt_setting.log_level,
			convert_log_level_2_str( rt_setting.log_level )
		);
	} else {
		log_msg( LOG_ERR, "Unknown option in section \"%s\" at line %d: %s\n", section, line_no, name );
		return -1;
	}
	return 0;
}

int handle_ini_entry(
	char const * section,
	char const * name,
	char const * value,
	int line_no
) {
	if(
		strcmp( "GENERAL", section ) == 0 ||
		strcmp( "General", section ) == 0 ||
		strcmp( "general", section ) == 0
	) {
		return parse_ini_section_general( section, name, value, line_no );
	} else if (
		strcmp( "LDAP", section ) == 0 ||
		strcmp( "Ldap", section ) == 0 ||
		strcmp( "ldap", section ) == 0
	) {
		return parse_ini_section_ldap( section, name, value, line_no );
	} else if (
		strcmp( "LOG", section ) == 0 ||
		strcmp( "Log", section ) == 0 ||
		strcmp( "log", section ) == 0 ||
		strcmp( "LOGGING", section ) == 0 ||
		strcmp( "Logging", section ) == 0 ||
		strcmp( "logging", section ) == 0
	) {
		return parse_ini_section_log( section, name, value, line_no );
	} else {
		log_msg( LOG_ERR, "Unknown ini-section \"%s\" at line %d\n", section, line_no );
		return -1;
	}

	return 0;
}

int parse_ini( void ) {
	int const result = parse_ini_file( handle_ini_entry );
	if( result == 0 ) {
		return EX_OK;
	} else if( result == -1 ) {
		// permission error
		return EX_NOPERM;
	} else if( result == -2 ) {
		// out of memory
		return EX_OSERR;
	} else {
		// any other non-zero code indicates an erroneous config line and hence
		// a usage error
		return EX_USAGE;
	}
	return EX_OK;
}

void log_rt_settings( void ) {
	log_msg( LOG_INFO, "Runtime setting daemon_mode:                                %d (%s)\n", rt_setting.daemon_mode, convert_daemon_mode_2_str( rt_setting.daemon_mode ) );
	log_msg( LOG_INFO, "Runtime setting config_file:                                %s\n", str_or_null( rt_setting.config_file ) );
	log_msg( LOG_INFO, "Runtime setting pid_file:                                   %s\n", str_or_null( rt_setting.pid_file ) );
	log_msg( LOG_INFO, "Runtime setting socket_file:                                %s\n", str_or_null( rt_setting.socket_file ) );
	log_msg( LOG_INFO, "Runtime setting ldap_bind.host:                             %s\n", str_or_null( rt_setting.ldap_bind.host ) );
	log_msg( LOG_INFO, "Runtime setting ldap_bind.dn:                               %s\n", str_or_null( rt_setting.ldap_bind.dn ) );
	log_msg( LOG_INFO, "Runtime setting ldap_bind.passwd:                           %s\n", str_or_null( rt_setting.ldap_bind.passwd ) );
	log_msg( LOG_INFO, "Runtime setting ldap_mail_acct_query.base_dn:               %s\n", str_or_null( rt_setting.ldap_mail_acct_query.base_dn ) );
	log_msg( LOG_INFO, "Runtime setting ldap_mail_acct_query.filter_template:       %s\n", str_or_null( rt_setting.ldap_mail_acct_query.filter_template ) );
	log_msg( LOG_INFO, "Runtime setting ldap_mail_acct_query.result_attributes[0]:  %s\n", str_or_null( rt_setting.ldap_mail_acct_query.result_attributes[0] ) );
	log_msg( LOG_INFO, "Runtime setting ldap_mail_list_query.base_dn:               %s\n", str_or_null( rt_setting.ldap_mail_list_query.base_dn ) );
	log_msg( LOG_INFO, "Runtime setting ldap_mail_list_query.filter_template:       %s\n", str_or_null( rt_setting.ldap_mail_list_query.filter_template ) );
	log_msg( LOG_INFO, "Runtime setting ldap_mail_list_query.result_attributes[0]:  %s\n", str_or_null( rt_setting.ldap_mail_list_query.result_attributes[0] ) );
	log_msg( LOG_INFO, "Runtime setting log_ident:                                  %s\n", str_or_null( rt_setting.log_ident ) );
	log_msg( LOG_INFO, "Runtime setting log_facility:                               %d (%s)\n", rt_setting.log_facility, convert_log_facility_2_str(rt_setting.log_facility) );
	log_msg( LOG_INFO, "Runtime setting log_level:                                  %d (%s)\n", rt_setting.log_level, convert_log_level_2_str(rt_setting.log_level) );
}
