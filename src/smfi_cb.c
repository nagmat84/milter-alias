#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <libmilter/mfdef.h>

#include "smfi_cb.h"
#include "priv_data.h"
#include "extldap.h"
#include "extstring.h"
#include "log.h"

static char * const AUTH_ACCT_MACRO = "{auth_authen}";

sfsistat mlfi_envfrom_cb( SMFICTX * ctx, char* envfrom[] ) {
	char const * const auth_acct = smfi_getsymval( ctx, AUTH_ACCT_MACRO );

	// Incoming mails from public SMTP servers have no autenticated session.
	// In this case auth_acct is NULL or empty and we don't have to save anything.
	if ( auth_acct == NULL || *auth_acct == '\0' ) {
		return SMFIS_CONTINUE;
	}

	// If we have an authenticated session, we save session information
	// in the private data area for later use in EOM callback.

	struct priv_data_t* priv_data = create_priv_data();

	// envfrom - Null-terminated SMTP command arguments;
	// argv[0] is guaranteed to be the sender address.
	// Later arguments are the ESMTP arguments.
	//
	// See: https://fossies.org/linux/sendmail/libmilter/docs/xxfi_envfrom.html
	//
	// Normalize,envelope from address.
	// In case it is surrounded by angular brackets , e.g. `<local@domain.tld>`,
	// remove the brackets.
	size_t const env_from_addr_len = strlen( envfrom[0] );
	char* env_from_addr = NULL;
	if ( envfrom[0][0] == '<' ) {
		env_from_addr = malloc( env_from_addr_len - 1 );
		strncpy( env_from_addr, envfrom[0] + 1, env_from_addr_len - 2 );
		env_from_addr[ env_from_addr_len - 2 ] = '\0';
	} else {
		env_from_addr = malloc( env_from_addr_len + 1 );
		strncpy( env_from_addr, envfrom[0], env_from_addr_len );
		env_from_addr[ env_from_addr_len ] = '\0';
	}
	set_priv_data_envelope_sender( priv_data, env_from_addr );
	free( env_from_addr );
	set_priv_data_auth_acct( priv_data, auth_acct );

	log_msg(
		LOG_DEBUG,
		"mlfi_env_from_cb (%p): envelope sender:       %s\n",
		(void*)priv_data,
		str_or_null( priv_data->envelope_sender )
	);
	log_msg(
		LOG_DEBUG,
		"mlfi_env_from_cb (%p): authenticated account: %s\n",
		(void*)priv_data,
		str_or_null( priv_data->auth_acct )
	);

	// Save pointer to private data in SMFI context
	smfi_setpriv( ctx, priv_data );
	return SMFIS_CONTINUE;
}

sfsistat mlfi_eom_cb( SMFICTX* ctx ) {
	struct priv_data_t* priv_data = (struct priv_data_t*) smfi_getpriv( ctx );

	// If we do not have any private data, the current mail is likely an
	// incoming mail from a public SMTP server without an authenticated
	// session.
	// In this case, there is nothing to do for us.
	if( priv_data == NULL ) {
		return SMFIS_CONTINUE;
	}

	struct string_array_t* list_addresses = search_mail_addresses_of_list( priv_data->envelope_sender );

	if( get_string_array_size( list_addresses ) != 0 ) {
		log_msg(
			LOG_DEBUG,
			"mlfi_eom_cb (%p): sender is a mailing list: %s\n",
			(void*)priv_data,
			priv_data->envelope_sender
		);
		struct string_array_t* own_mail_addresses = search_mail_addresses_by_account( priv_data->auth_acct );
		sort_string_array( list_addresses );
		sort_string_array( own_mail_addresses );
		substract_string_array( list_addresses, own_mail_addresses );
		free_string_array( own_mail_addresses );
		size_t const size = get_string_array_size( list_addresses );
		char const * bcc;
		for( size_t i = 0; i != size; ++i ) {
			bcc = (char*)get_string_array_at( list_addresses, i );
			log_msg( LOG_DEBUG, "mlfi_eom_cb (%p): adding bcc to: %s\n", (void*)priv_data, bcc );
			smfi_addrcpt( ctx, (char*)bcc );
		}
	} else {
		log_msg(
			LOG_DEBUG,
			"mlfi_eom_cb (%p): sender is not a mailing list: %s\n",
			(void*)priv_data,
			priv_data->envelope_sender
		);
	}

	free_string_array( list_addresses );
	free_priv_data( priv_data );
	smfi_setpriv( ctx, NULL );

	return SMFIS_CONTINUE;
}
