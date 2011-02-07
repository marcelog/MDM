/*!
 * \file mdm_driver_rawtcp_server.h This is a generic tcp server.
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#ifndef	_MDM_DRIVER_RAWTCP_SERVER_H_
#define	_MDM_DRIVER_RAWTCP_SERVER_H_

#include	<sys/types.h>
#include	<sys/socket.h>
#include	<sys/time.h>
#include	<netinet/in.h>
#include	<mdm_connection.h>

/*!
 * This are the options we use.
 */
typedef struct
{
	char host[1024]; /*!< IPv4 or FQDN of target host. */
	char service[16]; /*!< TCP Port or service name to connect to. */
	int to_accept; /*!< Timeout for accept(). -1 means block, 0, means return
		inmediately. > 0 in millis, timeout for accepting connections. */
	int reuse; /*!< Should bind reuse the address. */
	int backlog; /*!< Maximum connections not accepted. */
	int clients_block; /*!< Should new clients block on read. 
		Direcly affects and #mdm_driver_rawtcp_options_t.to_recv and
		#mdm_driver_rawtcp_data_t.tv_sr. */
	int clients_to_recv; /*!< Timeout in milliseconds for new clients recv. 
		Direcly affects #mdm_driver_rawtcp_options_t.to_recv and
		#mdm_driver_rawtcp_data_t.tv_sr. */
} mdm_driver_rawtcp_server_options_t;

/*!
 * This is our internal data struct.
 */
typedef struct
{
	int s; /*!< Socket file descriptor. */
	struct addrinfo *res; /*!< Used to resolve host address or fqdn. */
	struct timeval tv; /*!< Used to control socket accept polling. */
} mdm_driver_rawtcp_server_data_t;

extern void mdm_driver_rawtcp_server_open(
	mdm_connection_descriptor_t *, mdm_operation_result_t *
);
extern void mdm_driver_rawtcp_server_close(
	mdm_connection_descriptor_t *, mdm_operation_result_t *
);
extern void mdm_driver_rawtcp_server_setup(
	mdm_connection_descriptor_t *, mdm_connection_options_t *,
	mdm_operation_result_t *
);

extern void mdm_driver_rawtcp_server_send(
	mdm_connection_descriptor_t *, const char *,
	int, mdm_operation_result_t *
);

extern void mdm_driver_rawtcp_server_recv(
	mdm_connection_descriptor_t *, void *,
	int *, mdm_operation_result_t *
);

extern void mdm_driver_rawtcp_server_new(
	mdm_connection_t *, mdm_operation_result_t *
);
extern void mdm_driver_rawtcp_server_free(
	mdm_connection_t *
);

#endif
