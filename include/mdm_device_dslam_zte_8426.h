/*!
 * \file mdm_device_dslam_zte_8426.h Defines for DSLAM ZTE 8426.
 *
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#ifndef	_MDM_DEVICE_DSLAM_ZTE_8426_H_
#define	_MDM_DEVICE_DSLAM_ZTE_8426_H_

#include	<mdm_connection.h>
#include	<mdm_device.h>

/*! Commands. */
typedef enum
{
/* 0  */	MDM_DSLAM_ZTE_8426_CMD_SOFT_VERSION,
/* 1  */	MDM_DSLAM_ZTE_8426_CMD_GET_NETWORK_INFO,
/* 2  */	MDM_DSLAM_ZTE_8426_CMD_GET_MAC,
/* 3  */	MDM_DSLAM_ZTE_8426_CMD_SHOW_SLOTS,
/* 4  */	MDM_DSLAM_ZTE_8426_CMD_GET_ROUTES,
/* 5  */	MDM_DSLAM_ZTE_8426_CMD_SHOW_PORT_ATM_PVC,
/* 6  */	MDM_DSLAM_ZTE_8426_CMD_NOP,
/* 7  */	MDM_DSLAM_ZTE_8426_CMD_SHOW_SERVICE_PROFILES,
/* 8  */	MDM_DSLAM_ZTE_8426_CMD_SHOW_SLOT_PORTS,
/* 9  */	MDM_DSLAM_ZTE_8426_CMD_REBOOT,
/* 10 */ MDM_DSLAM_ZTE_8426_CMD_CREATE_PROFILE,
/* 11 */ MDM_DSLAM_ZTE_8426_CMD_DELETE_PROFILE,
/* 12 */ MDM_DSLAM_ZTE_8426_CMD_GET_SERVICE_PROFILE,
/* 13 */ MDM_DSLAM_ZTE_8426_CMD_GET_ALARMS,
/* 14 */ MDM_DSLAM_ZTE_8426_CMD_GET_PORT_ADMINOPER_STATUS,
/* 15 */ MDM_DSLAM_ZTE_8426_CMD_ADMIN_UP,
/* 16 */ MDM_DSLAM_ZTE_8426_CMD_ADMIN_DOWN,
	MDM_DSLAM_ZTE_8426_CMD_DUMMY
} mdm_device_cmd_dslam_zte_8426_t;

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
} mdm_device_dslam_zte_8426_options_t;

/*! This will hold internal data regarding the current connection. */
typedef struct
{
	int promptlen;
} mdm_device_dslam_zte_8426_data_t;

/*!
 * Used to post process dslam responses.
 * See #mdm_device_dslam_zte_8426_post_exec_t
 */
typedef void (*mdm_device_dslam_zte_8426_post_exec_t)(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);

extern const char *MDM_DEVICE_CMDNAME_DSLAM_ZTE_8426_STR[];
extern const char *MDM_DEVICE_CMD_DSLAM_ZTE_8426_STR[];

extern void mdm_device_dslam_zte_8426_open(
	mdm_device_descriptor_t *, mdm_connection_type_t, mdm_operation_result_t *
);
extern void mdm_device_dslam_zte_8426_exec(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void mdm_device_dslam_zte_8426_close(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);

extern void mdm_device_dslam_zte_8426_new(
	mdm_device_t *, mdm_operation_result_t *
);
extern void mdm_device_dslam_zte_8426_free(mdm_device_t *);

extern void mdm_device_dslam_zte_8426_prepare_exec(
	mdm_device_descriptor_t *, mdm_device_cmd_t,
	const char *, mdm_operation_result_t *
);

extern void mdm_device_dslam_zte_8426_post_exec(
	mdm_device_descriptor_t *,
	mdm_device_cmd_t, mdm_operation_result_t *
);

extern void mdm_device_dslam_zte_8426_check_error(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);

#endif
