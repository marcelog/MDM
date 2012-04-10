/*!
 * \file mdm_device.h Contains types, definitions, and constants to be used
 * by device drivers and applications. Basically the interface to 
 * mdm_devices. You should use these functions which will provide you with
 * a layer on top of the device drivers layer. This is the last layer for mdm.
 *
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#ifndef	_MDM_DEVICE_H_
#define	_MDM_DEVICE_H_

#include	<mdm_operation.h>
#include	<mdm_connection.h>

/*!
 * Make 4 memory pages the maximum size for device options.
 */
#define	MDM_DEVICE_MAX_OPTIONS_SIZE	1048576

/*!
 * Make 4 memory pages the maximum size for device data.
 */
#define	MDM_DEVICE_MAX_DATA_SIZE	1048576
/*!
 * Maximum length to store prepared command, relates to
 * #mdm_device_descriptor_t.exec_buffer_cmd
 */
#define	MDM_DEVICE_EXEC_BUFFER_CMD_MAX_LEN	1048576	

/*!
 * Maximum length to store post process response, relates to
 * #mdm_device_descriptor_t.exec_buffer_post
 */
#define	MDM_DEVICE_EXEC_BUFFER_POST_MAX_LEN	1048576

/*!
 * Maximum length to store device original response, relates to
 * #mdm_device_descriptor_t.exec_buffer
 */
#define	MDM_DEVICE_EXEC_BUFFER_MAX_LEN	1048576

/*!
 * Supported devices.
 */
typedef enum
{
	MDM_DEVICE_DSLAM_ALCATEL_73xx,/*!< DSLAM Alcatel 73xx family. */
	MDM_DEVICE_DSLAM_ZTE_8426,		/*!< DSLAM ZTE 8426 family. */
	MDM_DEVICE_DSLAM_ZTE_9xxx,		/*!< DSLAM ZTE 9xxx family. */
	MDM_DEVICE_DSLAM_HUAWEI_MA5600, /*!< DSLAM HUAWEI MA5600. */
	MDM_DEVICE_SIEMENS_EWSD,		/*!< Telephone Switch Siemens EWSD. */
    MDM_DEVICE_SIEMENS_HIX_5300,    /*!< DSLAM SIEMENS HIX 5300. */
	MDM_DEVICE_DUMMY					/*!< Dummy, dont use it. */
} mdm_device_type_t;

/*! Generic way to implement commands. Send this with #mdm_device_exec_t. */
typedef	int	mdm_device_cmd_t;

typedef void * mdm_device_data_t; /*!< This is a generic way for device
	drivers to store intenal data. */

/*!
 * Generic way to pass options when using a device.
 */
typedef void * mdm_device_options_t; /*!< This is a generic way for device
	drivers to store options like username and passwords. */

/*!
 * Every device has a descriptor which identifies all its properties and
 * contains the connection to be used, internal data and options.
 */
typedef struct
{
	mdm_connection_t connection; /*!< Connection in use. */
	mdm_device_data_t data; /*!< Device specific data. */
	mdm_device_options_t options; /*!< Options, different for each driver. */
	char *exec_buffer_cmd; /*!< Buffer used to store prepared command. */
	int exec_buffer_cmd_len; /*!< Resulting length for prepared command. */
	char *exec_buffer_post; /*!< Buffer used to store post processed result. */
	int exec_buffer_post_len; /*!< Resulting length for post processed result. */
	char *exec_buffer; /*!< Buffer used to store original device response. */
	int exec_buffer_len; /*!< Resulting length of original device response. */
} mdm_device_descriptor_t;

/*!
 * This one is used to open the device with a particular connection. Dont call
 * this directly, use #mdm_device_open.
 */
typedef void (*mdm_device_open_t)(
	mdm_device_descriptor_t *, mdm_connection_type_t, mdm_operation_result_t *
);

/*!
 * This one is used to close the connection used for a device. Dont call this
 * directly, use #mdm_device_close.
 */
typedef void (*mdm_device_close_t)(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);

/*!
 * Used to exec commands for a device through its open connection. Dont call
 * this directly, use #mdm_device_exec.
 */
typedef void (*mdm_device_exec_t)(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);

/*!
 * Used to prepare commands for exec in a device through its open connection.
 * Dont call this directly, use #mdm_device_exec.
 */
typedef void (*mdm_device_prepare_exec_t)(
	mdm_device_descriptor_t *, mdm_device_cmd_t,
	const char *, mdm_operation_result_t *
);

/*!
 * Used to post parse commands for exec in a device.
 * Dont call this directly, use #mdm_device_exec.
 */
typedef void (*mdm_device_post_exec_t)(
	mdm_device_descriptor_t *, 
	mdm_device_cmd_t, mdm_operation_result_t *
);

/*!
 * Used to check for errors in command response.
 * Dont call this directly, use #mdm_device_exec.
 */
typedef void (*mdm_device_check_error_t)(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);

/*!
 * Used to check when to stop reading response for a command.
 * Dont call this directly, use #mdm_device_exec.
 */
typedef void (*mdm_device_check_done_t)(char *, int);

/*!
 * Device structure.
 */
typedef struct
{
	mdm_device_type_t type; /*!< Device type. */
	mdm_device_descriptor_t descriptor; /*!< Device descriptor. */
	mdm_device_open_t open; /*!< To open connection to device
		implemented by driver. */
	mdm_device_exec_t exec; /*!< To exec commands in the device
		implemented by driver. */
	mdm_device_prepare_exec_t prepare_exec; /*!< To prepare exec commands in the
		device implemented by driver. */
	mdm_device_post_exec_t post_exec; /*!< To post process executed commands. */
	mdm_device_close_t close; /*!< To close connection to device
		implemented by driver. */
	mdm_device_check_error_t check_error; /*!< To check for errors in
		response. */
	mdm_device_check_done_t check_done; /*!< To check when to stop reading for
		a command. */
} mdm_device_t;

extern const char *mdm_device_2string(mdm_device_type_t);
extern int mdm_device_isvalid_type(mdm_device_type_t);
extern void mdm_device_new(
	mdm_device_t *, mdm_device_type_t, mdm_operation_result_t *
);
extern void mdm_device_free(mdm_device_t *c);
extern const char *mdm_devicecmd_2stringname(
	mdm_device_type_t, mdm_device_cmd_t
);
extern const char *mdm_devicecmd_2string(mdm_device_type_t, mdm_device_cmd_t);
extern int mdm_devicecmd_isvalid(mdm_device_type_t, mdm_device_cmd_t);
extern void mdm_device_open(
	mdm_device_t *, mdm_connection_type_t,
	mdm_device_options_t *, int, mdm_operation_result_t *
);
extern void mdm_device_close(mdm_device_t *, mdm_operation_result_t *);
extern void mdm_device_exec(
	mdm_device_t *, mdm_device_cmd_t, const char *,
	mdm_operation_result_t *
);

extern const char *mdm_device_get_buffer(mdm_device_t *);
extern const char *mdm_device_get_buffer_cmd(mdm_device_t *);
extern const char *mdm_device_get_buffer_post(mdm_device_t *);

extern int mdm_device_get_buffer_len(mdm_device_t *);
extern int mdm_device_get_buffer_cmd_len(mdm_device_t *);
extern int mdm_device_get_buffer_post_len(mdm_device_t *);
#endif
