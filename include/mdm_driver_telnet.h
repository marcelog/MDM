/*!
 * \file mdm_driver_telnet.h This is a telnet driver. Uses libtelnet.
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#ifndef	_MDM_DRIVER_TELNET_H_
#define	_MDM_DRIVER_TELNET_H_

#include	<libtelnet/libtelnet.h>
#include	<mdm_connection.h>

#define	MDM_TELNET_DRIVER_ONLY_TELNET_DATA		-65532 /*!< Used to signal
	caller that we only got telnet protocol data and no application data. */

/*!
 * This are the options we use.
 */
typedef struct
{
	char host[1024]; /*!< IPv4 or FQDN of target host. */
	char service[16]; /*!< TCP Port or service name to connect to. */
	int to_recv; /*!< If blocking >= 0, wait at most this amount of milliseconds
		before timeout. If -1, will block. */
	int to_connect; /*!< If < 1 timeouts when os decides, otherwise, time in
		milliseconds for timeouting a connect(). */
	int echo_cancel; /*!< Turn on echo cancellation, useful for devices that
		dont respect a dont for telnet echo option. */
	int send_eol; /*!< Send (1) or not send (0) an eol after data. */
} mdm_driver_telnet_options_t;

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
		we can check against the reply and avoid bad use of echo telnet option. */
	int sbufferlen; /*!< Length of last transmission. */
	char buffer[65535]; /*!< Internal buffer used to receive and translate
		telnet replies. Used by #_event_handler. */
	int bufflen; /*!< Length of #mdm_driver_telnet_data_t.buffer after
		telnet processing. Used by #_event_handler. */
	int received; /*!< Used by #_event_handler to signal that new data has
		arrived. */
} mdm_driver_telnet_data_t;

extern void mdm_driver_telnet_open(
	mdm_connection_descriptor_t *, mdm_operation_result_t *
);
extern void mdm_driver_telnet_close(
	mdm_connection_descriptor_t *, mdm_operation_result_t *
);
extern void mdm_driver_telnet_setup(
	mdm_connection_descriptor_t *, mdm_connection_options_t *,
	mdm_operation_result_t *
);

extern void mdm_driver_telnet_send(
	mdm_connection_descriptor_t *, const char *,
	int, mdm_operation_result_t *
);

extern void mdm_driver_telnet_recv(
	mdm_connection_descriptor_t *, void *,
	int *, mdm_operation_result_t *
);

extern void mdm_driver_telnet_new(mdm_connection_t *, mdm_operation_result_t *);
extern void mdm_driver_telnet_free(mdm_connection_t *);

#endif
