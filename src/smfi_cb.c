#include <stddef.h>
#include <libmilter/mfdef.h>

#include "smfi_cb.h"
#include "priv_data.h"
#include "extldap.h"
#include "log.h"

static char * const AUTH_ACCT_MACRO = "{auth_authen}";

sfsistat mlfi_envfrom_cb( SMFICTX * ctx, char* envfrom[] ) {
	int return_code = 0;
	struct priv_data_t* priv_data = create_priv_data();

	// envfrom - Null-terminated SMTP command arguments;
	// argv[0] is guaranteed to be the sender address.
	// Later arguments are the ESMTP arguments.
	//
	// See: https://fossies.org/linux/sendmail/libmilter/docs/xxfi_envfrom.html
	return_code |= set_priv_data_envelope_sender( priv_data, envfrom[0] );
	return_code |= set_priv_data_auth_acct( priv_data, smfi_getsymval( ctx, AUTH_ACCT_MACRO ) );

	if( return_code != 0 ) {
		log_msg( LOG_CRIT, "mlfi_envfrom_cb failed: could not determine envelope sender or authenticated account\n" );
		free_priv_data( priv_data );
		return SMFIS_TEMPFAIL;
	}

	log_msg(
		LOG_DEBUG,
		"mlfi_env_from_cb (%p): envelope sender:       %s\n",
		(void*)priv_data,
		priv_data->envelope_sender
	);
	log_msg(
		LOG_DEBUG,
		"mlfi_env_from_cb (%p): authenticated account: %s\n",
		(void*)priv_data,
		priv_data->auth_acct
	);

	// Save pointer to private data in SMFI context
	smfi_setpriv( ctx, priv_data );
	return SMFIS_CONTINUE;
}

sfsistat mlfi_eom_cb( SMFICTX* ctx ) {
	struct priv_data_t* priv_data = (struct priv_data_t*) smfi_getpriv( ctx );

	// If we do not have any private data something went wrong
	if( priv_data == NULL ) {
		log_msg( LOG_CRIT, "mlfi_eom_cb called, but could not find data from previous mlfi_envfrom_cb\n" );
		return SMFIS_TEMPFAIL;
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
