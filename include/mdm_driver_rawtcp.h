/*!
 * \file mdm_driver_rawtcp.h This is a generic tcp driver.
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#ifndef	_MDM_DRIVER_RAWTCP_H_
#define	_MDM_DRIVER_RAWTCP_H_

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
	int to_recv; /*!< If blocking >= 0, wait at most this amount of milliseconds
		before timeout. If -1, will block. */
	int to_connect; /*!< If < 1 timeouts when os decides, otherwise, time in
		milliseconds for timeouting a connect(). */
} mdm_driver_rawtcp_options_t;

/*!
 * This is our internal data struct.
 */
typedef struct
{
	int s; /*!< Socket file descriptor. */
	struct addrinfo *res; /*!< Used to resolve host address or fqdn. */
	struct timeval tv_sr; /*!< Used to control socket polling when
		reading/writing. */
	struct timeval tv_connect; /*!< Used to control socket polling when
		connect()ing. */
} mdm_driver_rawtcp_data_t;

extern void mdm_driver_rawtcp_open(
	mdm_connection_descriptor_t *, mdm_operation_result_t *
);
extern void mdm_driver_rawtcp_close(
	mdm_connection_descriptor_t *, mdm_operation_result_t *
);
extern void mdm_driver_rawtcp_setup(
	mdm_connection_descriptor_t *, mdm_connection_options_t *,
	mdm_operation_result_t *
);

extern void mdm_driver_rawtcp_send(
	mdm_connection_descriptor_t *, const char *,
	int, mdm_operation_result_t *
);

extern void mdm_driver_rawtcp_recv(
	mdm_connection_descriptor_t *, void *,
	int *, mdm_operation_result_t *
);

extern void mdm_driver_rawtcp_new(mdm_connection_t *, mdm_operation_result_t *);
extern void mdm_driver_rawtcp_free(mdm_connection_t *);

#endif
