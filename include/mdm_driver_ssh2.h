/*!
 * \file mdm_driver_ssh2.h This is a telnet driver. Uses libssh2. Only supports
 * password authentication, no keys.
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#ifndef	_MDM_DRIVER_SSH2_H_
#define	_MDM_DRIVER_SSH2_H_

#include	"libssh2_config.h"
#include	<libssh2/libssh2.h>
#include	<mdm_connection.h>

/*!
 * This are the options we use.
 */
typedef struct
{
	char host[1024]; /*!< IPv4 or FQDN of target host. */
	char service[16]; /*!< TCP Port or service name to connect to. */
	int to_connect; /*!< If < 1 timeouts when os decides, otherwise, time in
		milliseconds for timeouting a connect(). */
	int to_recv; /*!< If blocking >= 0, wait at most this amount of milliseconds
		before timeout. If -1, will block. */
	char username[32];
	char password[32];
	int echo_cancel; /*!< Turn on echo cancellation. */
} mdm_driver_ssh2_options_t;

/*!
 * This is our internal data struct.
 */
typedef struct
{
	telnet_t *telnet; /*!< Internal libtelnet data. */
	int s; /*!< Socket file descriptor. */
	struct addrinfo *res; /*!< Used to resolve host address or fqdn. */
	struct timeval tv_sr; /*!< Used to control socket polling when
		reading/writing. */
	struct timeval tv_connect; /*!< Used to control socket polling when
		connect()ing. */
	char sbuffer[65535]; /*!< Internal buffer to store last transmission so
		we can check against the reply and avoid echo. */
	int sbufferlen; /*!< Length of last transmission. */
    LIBSSH2_SESSION *session; /*!< Used by libssh2. */
    LIBSSH2_CHANNEL *channel; /*!< Used by libssh2. */
    const char *fingerprint; /*!< Host fingerprint. */
} mdm_driver_ssh2_data_t;

extern void mdm_driver_ssh2_open(
	mdm_connection_descriptor_t *, mdm_operation_result_t *
);
extern void mdm_driver_ssh2_close(
	mdm_connection_descriptor_t *, mdm_operation_result_t *
);
extern void mdm_driver_ssh2_setup(
	mdm_connection_descriptor_t *, mdm_connection_options_t *,
	mdm_operation_result_t *
);

extern void mdm_driver_ssh2_send(
	mdm_connection_descriptor_t *, const char *,
	int, mdm_operation_result_t *
);

extern void mdm_driver_ssh2_recv(
	mdm_connection_descriptor_t *, void *,
	int *, mdm_operation_result_t *
);

extern void mdm_driver_ssh2_new(mdm_connection_t *, mdm_operation_result_t *);
extern void mdm_driver_ssh2_free(mdm_connection_t *);

#endif
