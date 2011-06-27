/*!
 * \file mdm_device_dslam_huawei_ma5600.h Defines for DSLAM HUAWEI MA5600.
 *
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#ifndef	_MDM_DEVICE_DSLAM_HUAWEI_MA5600_H_
#define	_MDM_DEVICE_DSLAM_HUAWEI_MA5600_H_

#include	<mdm_connection.h>
#include	<mdm_device.h>

/*! Commands. */
typedef enum
{
/* 0  */	MDM_DSLAM_HUAWEI_MA5600_CMD_NOP,
/* 1  */ MDM_DSLAM_HUAWEI_MA5600_CMD_UPTIME,
/* 2  */ MDM_DSLAM_HUAWEI_MA5600_CMD_BACKPLANE_VERSION, // 0, 1
/* 3  */ MDM_DSLAM_HUAWEI_MA5600_CMD_SLOT_VERSION, // 0/0, 0/1
/* 4  */ MDM_DSLAM_HUAWEI_MA5600_CMD_BACKPLANE_TEMPERATURE, // 0, 1
/* 5  */ MDM_DSLAM_HUAWEI_MA5600_CMD_BACKPLANE_MEMORY, // 0/0
/* 6  */ MDM_DSLAM_HUAWEI_MA5600_CMD_BACKPLANE_DESCRIPTION, // 0, 0/0
/* 7  */ MDM_DSLAM_HUAWEI_MA5600_CMD_BOARD_DESCRIPTION, // 0, 0/0
/* 8  */ MDM_DSLAM_HUAWEI_MA5600_CMD_PORT_STATE_ALL,
	MDM_DSLAM_HUAWEI_MA5600_CMD_DUMMY
} mdm_device_cmd_dslam_huawei_ma5600_t;

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
} mdm_device_dslam_huawei_ma5600_options_t;

/*! This will hold internal data regarding the current connection. */
typedef struct
{
	int promptlen;
	char prompt[80];
} mdm_device_dslam_huawei_ma5600_data_t;

/*!
 * Used to post process dslam responses.
 * See #mdm_device_dslam_huawei_ma5600_post_exec_t
 */
typedef void (*mdm_device_dslam_huawei_ma5600_post_exec_t)(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);

extern const char *MDM_DEVICE_CMDNAME_DSLAM_HUAWEI_MA5600_STR[];
extern const char *MDM_DEVICE_CMD_DSLAM_HUAWEI_MA5600_STR[];

extern void mdm_device_dslam_huawei_ma5600_open(
	mdm_device_descriptor_t *, mdm_connection_type_t, mdm_operation_result_t *
);
extern void mdm_device_dslam_huawei_ma5600_exec(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void mdm_device_dslam_huawei_ma5600_close(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);

extern void mdm_device_dslam_huawei_ma5600_new(
	mdm_device_t *, mdm_operation_result_t *
);
extern void mdm_device_dslam_huawei_ma5600_free(mdm_device_t *);

extern void mdm_device_dslam_huawei_ma5600_prepare_exec(
	mdm_device_descriptor_t *, mdm_device_cmd_t,
	const char *, mdm_operation_result_t *
);

extern void mdm_device_dslam_huawei_ma5600_post_exec(
	mdm_device_descriptor_t *,
	mdm_device_cmd_t, mdm_operation_result_t *
);

extern void mdm_device_dslam_huawei_ma5600_check_error(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);

#endif
