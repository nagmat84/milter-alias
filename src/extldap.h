#ifndef _EXTLDAP_H_
#define _EXTLDAP_H_

/**
 * @file
 * @brief Compounds and functions for LDAP handling
 */

#include "string_array.h"

/**
 * Opens connection to LDAP server.
 */
int connect_ldap( void );

/**
 * Closes connection to LDAP server.
 */
int disconnect_ldap( void );

/**
 * Gets mail address members of mailing list by mailing list address.
 *
 * @param sender The mail address of the mailing list
 * @return String array with mail addresses
 */
struct string_array_t* search_mail_addresses_of_list( const char* const sender );

/**
 * Gets all mail addresses associated to the authenticated user account.
 *
 * The mail addresses are those which the account may use as its sender
 * identity.
 *
 * @param acct The authenticated account name (i.e. the "uid")
 * @return String array with mail addresses
 */
struct string_array_t* search_mail_addresses_by_account( char const * const acct );

#endif
