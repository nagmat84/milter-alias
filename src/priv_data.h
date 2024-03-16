#ifndef _PRIV_DATA_H_
#define _PRIV_DATA_H_

/**
 * @file
 * @brief Compounds and functions to handle the application-specific private
 * data of the milter.
 */

/**
 * Holds the application-specific data for a single milter invocation, i.e SMTP session.
 */
struct priv_data_t {
	char* envelope_sender; /**< The sender as given by SMTP `MAIL FROM:`. */
	char* auth_acct; /**< The authenticated user ID of the SMPT session. */
};

/**
 * @brief Creates a new object of type priv_data_t
 *
 * @return Pointer to the freshly created object or `NULL`
 */
struct priv_data_t* create_priv_data( void );

/**
 * @brief Frees an object of type priv_data_t
 *
 * @param priv_data Pointer to the object to be freed.
 */
void free_priv_data( struct priv_data_t * const priv_data );

/**
 * @brief Sets the envelope sender.
 *
 * Note, the function is NULL-pointer safe.
 *
 * @param priv_data Pointer to the object to be modified.
 * @param envelope_sender Null-terminated string with the value to be set.
 * @return Zero in case of success, non-zero otherwise
 */
int set_priv_data_envelope_sender( struct priv_data_t * const priv_data, char const * const envelope_sender );

/**
 * @brief Sets the authenticated account.
 *
 * Note, the function is NULL-pointer safe.
 *
 * @param priv_data Pointer to the object to be modified.
 * @param auth_acct Null-terminated string with the value to be set.
 * @return Zero in case of success, non-zero otherwise
 */
int set_priv_data_auth_acct( struct priv_data_t * const priv_data, char const * const auth_acct );

#endif
