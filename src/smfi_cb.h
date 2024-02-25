#ifndef _SMFI_CB_H_
#define _SMFI_CB_H_

/**
 * @file
 * @brief Functions (callbacks) for milter processing.
 */

#include <libmilter/mfapi.h>

/**
 * @brief Saves relevant information of the SMTP `MAIL FROM` stage for later use.
 *
 * This callback does not actually do anything except stashing away relevant
 * information in the private data area of the session context for later use
 * during the `END OF MESSAGE` stage.
 * Unless the process runs out of memory, this method does not fail.
 */
sfsistat mlfi_envfrom_cb( SMFICTX * ctx, char* envfrom[] );

/**
 * If the sender address is an alias, this method resolves the alias and
 * adds all members of the alias except the current sender as recipients.
 */
sfsistat mlfi_eom_cb( SMFICTX* ctx );

#endif
