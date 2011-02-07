/*!
 * \file mdm_connection.h Contains types, definitions, and constants to be used
 * by connection drivers and applications. Basically the interface to 
 * mdm_connections. You should use these functions which will provide you with
 * a layer on top of the connection drivers layer.
 *
 * \todo Implement SSH1
 * \todo Implement HTTP
 * \todo Implement HTTPS
 * \todo Implement SNMPv1
 * \todo Implement SNMPv2
 * \todo Implement SNMPv3
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#ifndef	_MDM_CONNECTION_H_
#define	_MDM_CONNECTION_H_

#include	<mdm_operation.h>

/*!
 * Make 4 memory pages the maximum size for connections options.
 */
#define	MDM_CONNECTION_MAX_OPTIONS_SIZE	1048576

/*!
 * Make 4 memory pages the maximum size for connections data.
 */
#define	MDM_CONNECTION_MAX_DATA_SIZE	1048576

/*!
 * Each one of these is a connection type. Each connection type needs to be
 * implemented by a connection driver. All these connection types should be
 * obtained and initialized via funcionts in mdm_connection.c
 */
typedef enum
{
	MDM_CONNECTION_TELNET,  /*!< Telnet protocol connection. */
	MDM_CONNECTION_SSH1,    /*!< SSH1 protocol connection. */
	MDM_CONNECTION_SSH2,    /*!< SSH2 protocol connection. */
	MDM_CONNECTION_HTTP,    /*!< HTTP protocol connection. Uses CURL. */
	MDM_CONNECTION_HTTPS,   /*!< HTTP over SSL protocol connection. Uses CURL. */
	MDM_CONNECTION_RAW_TCP, /*!< Raw TCP connection. */
	MDM_CONNECTION_RAW_TCP_SERVER, /*!< Raw TCP SERVER connection. */
	MDM_CONNECTION_SERIAL,  /*!< Serial RS-232 protocol connection. */
	MDM_CONNECTION_SNMP1,   /*!< SNMPv1 protocol driver. */
	MDM_CONNECTION_SNMP2,   /*!< SNMPv2 protocol driver. */
	MDM_CONNECTION_SNMP3,   /*!< SNMPv3 protocol driver. */
	MDM_CONNECTION_DUMMY    /*!< For testing, dummy driver, does nothing. */
} mdm_connection_type_t;

/*!
 * Generic way to pass options when allocating a new connection. Each driver
 * must implement its own options.
 */
typedef void * mdm_connection_options_t;

/*!
 * Generic way to save data specific to each driver. Each driver
 * must implement its own.
 */
typedef void * mdm_driver_connection_data_t;

/*!
 * Every connection has a descriptor which identifies all its properties and
 * contains all the necessary data not portable across drivers to handle a
 * particular connection.
 */
typedef struct
{
	mdm_driver_connection_data_t data; /*!< Connection specific data. */
	mdm_connection_options_t options; /*!< Options, different for each driver. */
} mdm_connection_descriptor_t;

/*!
 * This one is implemented by the corresponding driver.
 * Will open the connection. Dont call this directly, use #mdm_connection_open.
 */
typedef void (*mdm_connection_open_t)(
	mdm_connection_descriptor_t *, mdm_operation_result_t *);

/*!
 * This one is implemented by the corresponding driver.
 * Will close the connection. Dont call this directly, use
 * #mdm_connection_close.
 */
typedef void (*mdm_connection_close_t)(
	mdm_connection_descriptor_t *, mdm_operation_result_t *
);

/*!
 * This one is implemented by the corresponding driver.
 * Will send a buffer. Dont call this directly, use #mdm_connection_send.
 */
typedef void (*mdm_connection_send_t)(
	mdm_connection_descriptor_t *, const char *,
	int, mdm_operation_result_t *
);

/*!
 * This one is implemented by the corresponding driver.
 * Will receive a buffer. Dont call this directly, use #mdm_connection_recv.
 */
typedef void (*mdm_connection_recv_t)(
	mdm_connection_descriptor_t *, void *,
	int *, mdm_operation_result_t *
);

/*!
 * This one is implemented by the corresponding driver.
 * Will setup the connection. Dont call this directly, use #mdm_connection_open.
 */
typedef void (*mdm_connection_setup_t)(
	mdm_connection_descriptor_t *, mdm_connection_options_t *,
	mdm_operation_result_t *
);

/*!
 * Generic connection information.
 */
typedef struct
{
	mdm_connection_descriptor_t descriptor; /*!< Connection descriptor. */
	mdm_connection_type_t type; /*!< Connection type. */
	mdm_connection_open_t open; /*!< To open connections,
		implemented by driver. */
	mdm_connection_close_t close; /*!< To close connections,
		implemented by driver. */
	mdm_connection_setup_t setup; /*!< To setup the connection and options. */
	mdm_connection_send_t send; /*!< To send a buffer. */
	mdm_connection_recv_t recv; /*!< To send a buffer. */
} mdm_connection_t;

extern void mdm_connection_cleanup(void);
extern void mdm_connection_init(void);
extern const char *mdm_connection_2string(mdm_connection_type_t);
extern int mdm_connection_isvalid_type(mdm_connection_type_t);
extern void mdm_connection_new(
	mdm_connection_t *, mdm_connection_type_t, mdm_operation_result_t *
);
extern void mdm_connection_free(mdm_connection_t *c);
extern void mdm_connection_open(
	mdm_connection_t *, mdm_connection_options_t *, mdm_operation_result_t *
);
extern void mdm_connection_close(mdm_connection_t *, mdm_operation_result_t *);
extern void mdm_connection_setup(
	mdm_connection_t *, mdm_connection_options_t *,
	mdm_operation_result_t *
);
extern void mdm_connection_send(
	mdm_connection_t *, const char *,
	int, mdm_operation_result_t *
);

extern void mdm_connection_recv(
	mdm_connection_t *, void *,
	int *, mdm_operation_result_t *
);
#endif
