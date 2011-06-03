/*!
 * \file mdm_device_dslam_zte_9xxx.h Defines for DSLAM ZTE 9000 series.
 *
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#ifndef	_MDM_DEVICE_DSLAM_ZTE_9xxx_H_
#define	_MDM_DEVICE_DSLAM_ZTE_9xxx_H_

#include	<mdm_connection.h>
#include	<mdm_device.h>

/*! Commands. */
typedef enum
{
/* 0  */	MDM_DSLAM_ZTE_9xxx_CMD_NOP,
/* 1  */	MDM_DSLAM_ZTE_9xxx_CMD_SHOW_SLOTS,
/* 2  */	MDM_DSLAM_ZTE_9xxx_CMD_SHOW_RUNNING_CONFIG,
/* 3  */ MDM_DSLAM_ZTE_9xxx_CMD_SHOW_SERVICE_PROFILE,
/* 4  */ MDM_DSLAM_ZTE_9xxx_CMD_SOFT_VERSION,
/* 5  */ MDM_DSLAM_ZTE_9xxx_CMD_SHOW_PORT_ATM_PVC,
/* 6  */ MDM_DSLAM_ZTE_9xxx_CMD_SHOW_UPTIME,
/* 7  */ MDM_DSLAM_ZTE_9xxx_CMD_LOAD_PORTS_SLOT,
/* 8  */ MDM_DSLAM_ZTE_9xxx_CMD_SCB_DETAIL,
/* 9  */ MDM_DSLAM_ZTE_9xxx_CMD_SLOT_DETAIL,
/* 10 */ MDM_DSLAM_ZTE_9xxx_CMD_PORT_DETAIL,
/* 11 */ MDM_DSLAM_ZTE_9xxx_CMD_SET_TIMEZONE,
/* 12 */ MDM_DSLAM_ZTE_9xxx_CMD_GET_NTP,
/* 13 */ MDM_DSLAM_ZTE_9xxx_CMD_GET_SERVICE_PROFILE,
/* 14 */ MDM_DSLAM_ZTE_9xxx_CMD_CREATE_SERVICE_PROFILE,
/* 15 */ MDM_DSLAM_ZTE_9xxx_CMD_CONFIGURE_SERVICE_PROFILE,
/* 16 */ MDM_DSLAM_ZTE_9xxx_CMD_DELETE_SERVICE_PROFILE,
/* 17 */ MDM_DSLAM_ZTE_9xxx_CMD_GET_SLOT_MAC,
/* 18 */ MDM_DSLAM_ZTE_9xxx_CMD_GET_PORT_MAC,
/* 19 */ MDM_DSLAM_ZTE_9xxx_CMD_GET_ROUTES,
/* 20 */ MDM_DSLAM_ZTE_9xxx_CMD_GET_PORT_FULL,
/* 21 */ MDM_DSLAM_ZTE_9xxx_CMD_GET_PHYSICAL_PORT_INFO,
/* 22 */ MDM_DSLAM_ZTE_9xxx_CMD_GET_NETWORK_INFO,
/* 23 */ MDM_DSLAM_ZTE_9xxx_CMD_GET_CHANNEL_INFO,
/* 24 */ MDM_DSLAM_ZTE_9xxx_CMD_GET_FANS_INFO,
/* 25 */ MDM_DSLAM_ZTE_9xxx_CMD_CONFIGURE_NTP_SERVER,
/* 26 */ MDM_DSLAM_ZTE_9xxx_CMD_ENABLE_NTP,
/* 27 */ MDM_DSLAM_ZTE_9xxx_CMD_DISABLE_NTP,
	MDM_DSLAM_ZTE_9xxx_CMD_DUMMY
} mdm_device_cmd_dslam_zte_9xxx_t;

/*! Options for dslam connection. */
typedef struct
{
	mdm_connection_type_t connection_type; /*!< Connection type. */
	char target[256]; /*!< If serial, filename. If network, host:port. */
	char username[16]; /*!< Username for login. */
	char password[16]; /*!< Password for login. */
	int to_recv; /*!< If blocking >= 0, wait at most this amount of milliseconds
		before timeout. If -1, will block. */
	int to_connect; /*!< If < 1 timeouts when os decides, otherwise, time in
		milliseconds for timeouting a connect(). */
} mdm_device_dslam_zte_9xxx_options_t;

/*! This will hold internal data regarding the current connection. */
typedef struct
{
	int promptlen;
} mdm_device_dslam_zte_9xxx_data_t;

/*!
 * Used to post process dslam responses.
 * See #mdm_device_dslam_zte_9xxx_post_exec_t
 */
typedef void (*mdm_device_dslam_zte_9xxx_post_exec_t)(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);

extern const char *MDM_DEVICE_CMDNAME_DSLAM_ZTE_9xxx_STR[];
extern const char *MDM_DEVICE_CMD_DSLAM_ZTE_9xxx_STR[];

extern void mdm_device_dslam_zte_9xxx_open(
	mdm_device_descriptor_t *, mdm_connection_type_t, mdm_operation_result_t *
);
extern void mdm_device_dslam_zte_9xxx_exec(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void mdm_device_dslam_zte_9xxx_close(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);

extern void mdm_device_dslam_zte_9xxx_new(
	mdm_device_t *, mdm_operation_result_t *
);
extern void mdm_device_dslam_zte_9xxx_free(mdm_device_t *);

extern void mdm_device_dslam_zte_9xxx_prepare_exec(
	mdm_device_descriptor_t *, mdm_device_cmd_t,
	const char *, mdm_operation_result_t *
);

extern void mdm_device_dslam_zte_9xxx_post_exec(
	mdm_device_descriptor_t *,
	mdm_device_cmd_t, mdm_operation_result_t *
);

extern void mdm_device_dslam_zte_9xxx_check_error(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);

#endif
