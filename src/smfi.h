#ifndef _SMFI_H_
#define _SMFI_H_

/**
 * @file
 * @brief Compounds and functions to setup the mail milter.
 */

#include <libmilter/mfapi.h>

/**
 * @brief The mail filter name.
 */
extern char * const FILTER_NAME;

/**
 * @brief Defines restrictions on what this mail filter may do.
 *
 * The filter may only add or delete recipients.
 */
extern int const FILTER_FLAGS;

/**
 * @brief The description of the mail filter.
 *
 * This structure declares what callbacks this filter provides and is
 * passed to the milter library to announce this filter's capabilities.
 */
extern struct smfiDesc const FILTER_DESC;

/**
 * Sets up the mail filter.
 *
 * @return Zero on success, non-zero in case of failure.
 */
int setup_smfi( void );

/**
 * Cleans up leftovers from mail filter.
 *
 * @return Zero on success, non-zero in case of failure.
 */
int cleanup_smfi( void );

#endif
