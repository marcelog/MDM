/*!
 * \file mdm_device_dslam_siemens_hix5300.h Defines for DSLAM SIEMENS HIX 5300.
 *
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#ifndef	_MDM_DEVICE_DSLAM_SIEMENS_HIX5300_H_
#define	_MDM_DEVICE_DSLAM_SIEMENS_HIX5300_H_

#include	<mdm_connection.h>
#include	<mdm_device.h>

/*! Commands. */
typedef enum
{
/* 0  */	MDM_DSLAM_SIEMENS_HIX5300_CMD_NOP,
/* 1  */	MDM_DSLAM_SIEMENS_HIX5300_CMD_SHOW_RUNNING_CONFIG,
	MDM_DSLAM_SIEMENS_HIX5300_CMD_DUMMY
} mdm_device_cmd_dslam_siemens_hix5300_t;

/*! Options for dslam connection. */
typedef struct
{
	mdm_connection_type_t connection_type; /*!< Connection type. */
	char target[256]; /*!< If serial, filename. If network, host:port. */
	char username[16]; /*!< Username for login. */
	char password[16]; /*!< Password for login. */
	char enable[16]; /*!< Password for enable. */
	int to_recv; /*!< If blocking >= 0, wait at most this amount of milliseconds
		before timeout. If -1, will block. */
	int to_connect; /*!< If < 1 timeouts when os decides, otherwise, time in
		milliseconds for timeouting a connect(). */
} mdm_device_dslam_siemens_hix5300_options_t;

/*! This will hold internal data regarding the current connection. */
typedef struct
{
	char prompt[80];
	int promptlen; /*!< We need the prompt length because when the dslam
	echoes our issued command, it adds the prompt length to make the command
	fit in 80 columns. */
} mdm_device_dslam_siemens_hix5300_data_t;

/*!
 * Used to post process dslam responses.
 * See #mdm_device_dslam_siemens_hix5300_post_exec_t
 */
typedef void (*mdm_device_dslam_siemens_hix5300_post_exec_t)(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);

extern const char *MDM_DEVICE_CMDNAME_DSLAM_SIEMENS_HIX5300_STR[];
extern const char *MDM_DEVICE_CMD_DSLAM_SIEMENS_HIX5300_STR[];

extern void mdm_device_dslam_siemens_hix5300_open(
	mdm_device_descriptor_t *, mdm_connection_type_t, mdm_operation_result_t *
);
extern void mdm_device_dslam_siemens_hix5300_exec(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);

extern void mdm_device_dslam_siemens_hix5300_close(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);

extern void mdm_device_dslam_siemens_hix5300_new(
	mdm_device_t *, mdm_operation_result_t *
);
extern void mdm_device_dslam_siemens_hix5300_free(mdm_device_t *);

extern void mdm_device_dslam_siemens_hix5300_prepare_exec(
	mdm_device_descriptor_t *, mdm_device_cmd_t,
	const char *, mdm_operation_result_t *
);

extern void mdm_device_dslam_siemens_hix5300_post_exec(
	mdm_device_descriptor_t *,
	mdm_device_cmd_t, mdm_operation_result_t *
);

extern void mdm_device_dslam_siemens_hix5300_check_error(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);

#endif
