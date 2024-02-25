#include <stdlib.h>
#include <stdio.h>
#include <sysexits.h>

#include "smfi.h"
#include "smfi_cb.h"
#include "log.h"
#include "runtime_setting.h"

char * const FILTER_NAME = "milter-alias";

int const FILTER_FLAGS = SMFIF_ADDRCPT|SMFIF_DELRCPT;

struct smfiDesc const FILTER_DESC = {
	FILTER_NAME,
	SMFI_VERSION,
	FILTER_FLAGS,
	NULL,             // connection info callback
	NULL,             // SMTP HELO command callback
	mlfi_envfrom_cb,  // envelope sender callback
	NULL,             // envelope recipient callback
	NULL,             // header callback
	NULL,             // end of header callback
	NULL,             // body block callback
	mlfi_eom_cb,      // end of message callback
	NULL,             // message aborted callback
	NULL,             // connection cleanup callback
	NULL,             // unknown SMTP commands callback
	NULL,             // DATA callback
	NULL              // option negotiation callback
};

int smfi_setup( void ) {
	if( smfi_setconn( rt_setting.socket_file ) != MI_SUCCESS ) {
		log_msg( LOG_CRIT, "smfi_setconn failed\n" );
		return EX_UNAVAILABLE;
	}

	if( smfi_register( FILTER_DESC ) != MI_SUCCESS ) {
		log_msg( LOG_CRIT, "smfi_register failed\n" );
		return EX_UNAVAILABLE;
	}

	return EX_OK;
}
