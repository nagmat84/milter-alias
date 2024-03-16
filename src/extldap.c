#define _GNU_SOURCE
#include <ldap.h>
#include <sysexits.h>
#include <string.h>
#include <stdlib.h>

#include "runtime_setting.h"
#include "log.h"
#include "extstring.h"
#include "string_array.h"

static LDAP* ldap_handle = NULL;

static int const LDAP_PROTOCOL_VERSION = LDAP_VERSION3;

int connect_ldap( void ) {
	int result = LDAP_SUCCESS;
	result = ldap_initialize( &ldap_handle, rt_setting.ldap_bind.host );
	if ( result != LDAP_SUCCESS ) {
		log_msg( LOG_ERR, "ldap_initialize failed: %s (%d)\n", ldap_err2string( result ), result );
		return EX_SOFTWARE;
	}

	result = ldap_set_option( ldap_handle, LDAP_OPT_PROTOCOL_VERSION, &LDAP_PROTOCOL_VERSION );
	if ( result != LDAP_OPT_SUCCESS ) {
		log_msg( LOG_ERR, "ldap_set_option failed: %s (%d)\n", ldap_err2string( result ), result );
		return EX_SOFTWARE;
	}

	if( rt_setting.ldap_bind.dn == NULL ) {
		// Anonymous simple bind
		result = ldap_sasl_bind_s( ldap_handle, NULL, LDAP_SASL_SIMPLE, NULL, NULL, NULL, NULL );
	} else {
		// Simple bind with DN
		struct berval passwd = { 0, NULL };
		passwd.bv_val = ber_strdup( rt_setting.ldap_bind.passwd );
		passwd.bv_len = strlen( passwd.bv_val );
		result = ldap_sasl_bind_s( ldap_handle, rt_setting.ldap_bind.dn, LDAP_SASL_SIMPLE, &passwd, NULL, NULL, NULL );
		ber_memfree( passwd.bv_val );
	}
	if ( result != LDAP_SUCCESS ) {
		log_msg( LOG_ERR, "ldap_sasl_bind_s (simple) failed: %s (%d)\n", ldap_err2string( result ), result );
		return EX_IOERR;
	}

	log_msg(
		LOG_INFO,
		"connect_ldap: succeeded (host = \"%s\", user = \"%s\")\n",
		rt_setting.ldap_bind.host,
		rt_setting.ldap_bind.passwd
	);

	return EX_OK;
}

int disconnect_ldap( void ) {
	int result = ldap_unbind_ext_s( ldap_handle, NULL, NULL );
	ldap_handle = NULL;
	if ( result != LDAP_SUCCESS ) {
		log_msg( LOG_ERR, "ldap_unbind_ext_s failed: %s (%d)\n", ldap_err2string( result ), result );
		return EX_IOERR;
	}
	return EX_OK;
}

static void decompose_mail_address( char const * const addr, char** local, char** domain ) {
	char const * at = strchrnul( addr, '@' );
	*local = malloc( at - addr + 1 );
	*( stpncpy( *local, addr, at - addr ) ) = '\0';
	if( *at == '@' ) ++at;
	*domain = malloc( strlen( at ) + 1 );
	strcpy( *domain, at );
}

/**
 * Substitutes the placeholders in a template with parts of the provided mail
 * address.
 *
 * The following placeholders are supported:
 *
 *  - `%u` is replaced by the entire mail address
 *  - `%d` is replaced by the domain part
 *  - `%n` is replaced by the local part
 *
 * @param template The template which with place holders
 * @param mail_address The mail address to use to substitute the place holders
 * @return The constructed filter; the caller must free the result
 */
static char* replace_placeholders( char const * const template, char const * const mail_address ) {
	// TODO: Make this code safe against injection attacks.
	// We replace the place holders by user-supplied data without escaping.
	// TODO: Escape the following special LDAP symbols as follows
	//  - ( --> \28  (left paranthese)
	//  - ) --> \29  (right paranthese)
	//  - & --> \26  (AND operator)
	//  - * --> \2a  (wildcard match)
	//  - / --> \2f  (slash)
	//  - \ --> \5c  (backslash)
	//  - | --> \7c  (OR operator)
	//  - < --> \3c  (SMALLER THAN operator)
	//  - = --> \3d  (EQUALITY operator)
	//  - > --> \3e  (GREATER THAN operator)
	//  - ~ --> \7e  (SIMILARITY operator)
	char* local = NULL;
	char* domain = NULL;
	decompose_mail_address( mail_address, &local, &domain );
	char * const buf1 = str_replace( template, "%u", mail_address );
	char * const buf2 = str_replace( buf1, "%d", domain );
	char* filter = str_replace( buf2, "%n", local );
	free( buf1 );
	free( buf2 );
	free( local );
	free( domain );
	return filter;
}

static struct string_array_t* search_mail_addresses(
	char const * const filter,
	char const * const base,
	char** const result_attributes
) {
	log_msg( LOG_DEBUG, "search_mail_addresses: LDAP base: %s\n", base );
	log_msg( LOG_DEBUG, "search_mail_addresses: LDAP filter: %s\n", filter );

	LDAPMessage* ldap_result_msg = NULL;
	int result_code = ldap_search_ext_s(
		ldap_handle,
		base,
		LDAP_SCOPE_SUBTREE,
		filter,
		result_attributes,
		0,    // attrsonly: include values in response as well
		NULL, // serverctrls: no special server controls
		NULL, // clientctrls: no special client controls
		NULL, // timeout: unlimited
		0,    // sizelimit: unlimited
		&ldap_result_msg
	);

	if ( result_code != LDAP_SUCCESS ) {
		log_msg( LOG_ERR, "search_mail_addresses: ldap_search_ext_s failed: %s (%d)\n", ldap_err2string( result_code ), result_code );
		ldap_msgfree( ldap_result_msg );
		return NULL;
	}

	// Collect all mail addresses in a dynamic array
	// As an estimate we assume that there are approx. 3 mail addresses
	// per found entity.
	// If the array turns out to be too small, it will be re-allocated.
	int const result_size = ldap_count_entries( ldap_handle, ldap_result_msg );
	if ( result_size == -1 ) {
		log_msg( LOG_ERR, "search_mail_addresses: ldap_count_entries failed\n" );
		ldap_msgfree( ldap_result_msg );
		return NULL;
	}
	log_msg( LOG_DEBUG, "search_mail_addresses: LDAP result size: %d\n", result_size );
	struct string_array_t* result = create_string_array( 3 * result_size );
	if ( result_size == 0 ) {
		// short-cut in case of an empty result set
		// return a list with zero elements
		return result;
	}

	BerElement* berptr = NULL;
	char const * value = NULL;
	for(
		LDAPMessage* ldap_entry_msg = ldap_first_entry( ldap_handle, ldap_result_msg );
		ldap_entry_msg != NULL;
		ldap_entry_msg = ldap_next_entry( ldap_handle, ldap_entry_msg )
	) {
		for(
			char* ldap_attr = ldap_first_attribute( ldap_handle, ldap_entry_msg, &berptr );
			ldap_attr != NULL;
			ldap_attr = ldap_next_attribute( ldap_handle, ldap_entry_msg, berptr )
		) {
			struct berval** values = ldap_get_values_len( ldap_handle, ldap_entry_msg, ldap_attr );
			for( int i = 0; values[i] != NULL; ++i ) {
				value = push_onto_string_array_l( result, values[i]->bv_val, values[i]->bv_len );
				log_msg( LOG_DEBUG, "search_mail_addresses: found mail address: %s\n", value );
			}
			ldap_value_free_len( values );
			ldap_memfree( ldap_attr );
		}
		ber_free( berptr, 0 );
		berptr = NULL;
	}
	ldap_msgfree( ldap_result_msg );

	return result;
}

struct string_array_t* search_mail_addresses_of_list( const char* const sender ) {
	char * const filter = replace_placeholders(
		rt_setting.ldap_mail_list_query.filter_template,
		sender
	);
	char * const base_dn = replace_placeholders(
		rt_setting.ldap_mail_list_query.base_dn,
		sender
	);
	struct string_array_t* result = search_mail_addresses(
		filter, base_dn, rt_setting.ldap_mail_list_query.result_attributes
	);
	free( filter );
	free( base_dn );
	return result;
}

struct string_array_t* search_mail_addresses_by_account( char const * const acct ) {
	char * const filter = replace_placeholders(
		rt_setting.ldap_mail_acct_query.filter_template,
		acct
	);
	char * const base_dn = replace_placeholders(
		rt_setting.ldap_mail_acct_query.base_dn,
		acct
	);
	struct string_array_t* result = search_mail_addresses(
		filter, base_dn, rt_setting.ldap_mail_acct_query.result_attributes
	);
	free( filter );
	free( base_dn );
	return result;
}
