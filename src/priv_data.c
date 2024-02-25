#include <stdlib.h>
#include <string.h>

#include "priv_data.h"

struct priv_data_t* create_priv_data( void ) {
	struct priv_data_t * const result = malloc( sizeof( struct priv_data_t ) );
	if( result == NULL )
		return NULL;
	result->envelope_sender = NULL;
	result->auth_acct = NULL;
	return result;
}

void free_priv_data( struct priv_data_t * const priv_data ) {
	if( priv_data == NULL ) return;
	free( priv_data->envelope_sender );
	priv_data->envelope_sender = NULL;
	free( priv_data->auth_acct );
	priv_data->auth_acct = NULL;
	free( priv_data );
}

int set_priv_data_envelope_sender( struct priv_data_t * const priv_data, char const * const envelope_sender ) {
	if( priv_data == NULL || envelope_sender == NULL )
		return 1;
	free( priv_data->envelope_sender );
	priv_data->envelope_sender = malloc( strlen( envelope_sender ) );
	if( priv_data->envelope_sender == NULL ) {
		return 1;
	}
	strcpy( priv_data->envelope_sender, envelope_sender );
	return 0;
}

int set_priv_data_auth_acct( struct priv_data_t * const priv_data, char const * const auth_acct ) {
	if( priv_data == NULL || auth_acct == NULL )
		return 1;
	free( priv_data->auth_acct );
	priv_data->auth_acct = malloc( strlen( auth_acct ) );
	if( priv_data->auth_acct == NULL ) {
		return 1;
	}
	strcpy( priv_data->auth_acct, auth_acct );
	return 0;
}
