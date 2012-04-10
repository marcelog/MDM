/*!
 * \file mdm_device.c This file contains all the necessary code to implement
 * generic device drivers. All device drivers will use these functions. You 
 * should use these functions which will provide you with a layer on top of
 * the device drivers layer. This is the last layer for mdm.
 *
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#include	<mdm.h>

/*!
 * This holds the strings associated to each of the supported device
 * drivers.
 */
static const char *MDM_DEVICE_TYPE_STRING[] = {
	"DSLAM_ALCATEL_73xx",
	"DSLAM_ZTE_8426",
	"DSLAM_ZTE_9xxx",
	"DSLAM_HUAWEI_MA5600",
    "DSLAM_SIEMENS_HIX5300",
	"SIEMENS EWSD",
	"DUMMY"
};

/*******************************************************************************
 * CODE STARTS.
 ******************************************************************************/

/*!
 * Checks for a valid device type. Valid device are defined in
 * #mdm_device_type_t and the corresponding strings are in 
 * #MDM_DEVICE_TYPE_STRING. A valid device type is contained between
 * values #MDM_DEVICE_DSLAM_ALCATEL_73xx and #MDM_DEVICE_DUMMY (lower and higher
 * values of the enum #mdm_device_type_t).
 *
 * \param d Device type to test.
 * \return 1 (true) if device type is valid. 0 (false) otherwise.
 */
int
mdm_device_isvalid_type(mdm_device_type_t d)
{
	return (d >= MDM_DEVICE_DSLAM_ALCATEL_73xx && d < MDM_DEVICE_DUMMY);
}

/*!
 * \param d Device type.
 * \return NULL if d is an invalid device type. Otherwise, a const char *
 * pointing to one of the elements of #MDM_DEVICE_TYPE_STRING indicating
 * the device type in human readable form.
 */
const char *
mdm_device_2string(mdm_device_type_t d)
{
	return mdm_connection_isvalid_type(d) ? MDM_DEVICE_TYPE_STRING[d] : NULL;
}

/*!
 * Use this to get the cmd for a specific command for a specific device.
 * \param d Device type.
 * \param c Device command.
 * \return NULL if d is an invalid device type or c is not a valid command for
 * that device d. Otherwise, a const char * pointing to one of the elements of
 * the device commands in human readable form.
 */
const char *
mdm_devicecmd_2string(mdm_device_type_t d, mdm_device_cmd_t c)
{
	if(!mdm_device_isvalid_type(d))
		return NULL;
	switch(d)
	{
		case MDM_DEVICE_DSLAM_ALCATEL_73xx:
			return
				mdm_devicecmd_isvalid(d, c) ?
				MDM_DEVICE_CMD_DSLAM_ALCATEL_73xx_STR[c] :
				NULL
			;
		case MDM_DEVICE_DSLAM_ZTE_8426:
			return
				mdm_devicecmd_isvalid(d, c) ?
				MDM_DEVICE_CMD_DSLAM_ZTE_8426_STR[c] :
				NULL
			;
		case MDM_DEVICE_DSLAM_ZTE_9xxx:
			return
				mdm_devicecmd_isvalid(d, c) ?
				MDM_DEVICE_CMD_DSLAM_ZTE_9xxx_STR[c] :
				NULL
			;
		case MDM_DEVICE_DSLAM_HUAWEI_MA5600:
			return
				mdm_devicecmd_isvalid(d, c) ?
				MDM_DEVICE_CMDNAME_DSLAM_HUAWEI_MA5600_STR[c] :
				NULL
			;
        case MDM_DEVICE_DSLAM_SIEMENS_HIX5300:
            return
                mdm_devicecmd_isvalid(d, c) ?
                MDM_DEVICE_CMDNAME_DSLAM_SIEMENS_HIX5300_STR[c] :
                NULL
        ;
		default:
			return NULL;
	}
}

/*!
 * Use this to get the cmd name for a specific command for a specific device.
 * \param d Device type.
 * \param c Device command.
 * \return NULL if d is an invalid device type or c is not a valid command for
 * that device d. Otherwise, a const char * pointing to one of the elements of
 * the device commands name in human readable form.
 */
const char *
mdm_devicecmd_2stringname(mdm_device_type_t d, mdm_device_cmd_t c)
{
	if(!mdm_device_isvalid_type(d))
		return NULL;
	switch(d)
	{
		case MDM_DEVICE_DSLAM_ALCATEL_73xx:
			return
				mdm_devicecmd_isvalid(d, c) ?
				MDM_DEVICE_CMDNAME_DSLAM_ALCATEL_73xx_STR[c] :
				NULL
			;
		case MDM_DEVICE_DSLAM_ZTE_8426:
			return
				mdm_devicecmd_isvalid(d, c) ?
				MDM_DEVICE_CMDNAME_DSLAM_ZTE_8426_STR[c] :
				NULL
			;
		case MDM_DEVICE_DSLAM_ZTE_9xxx:
			return
				mdm_devicecmd_isvalid(d, c) ?
				MDM_DEVICE_CMDNAME_DSLAM_ZTE_9xxx_STR[c] :
				NULL
			;
		case MDM_DEVICE_DSLAM_HUAWEI_MA5600:
			return
				mdm_devicecmd_isvalid(d, c) ?
				MDM_DEVICE_CMDNAME_DSLAM_HUAWEI_MA5600_STR[c] :
				NULL
			;
        case MDM_DEVICE_DSLAM_SIEMENS_HIX5300:
            return
                mdm_devicecmd_isvalid(d, c) ?
                MDM_DEVICE_CMDNAME_DSLAM_SIEMENS_HIX5300_STR[c] :
                NULL
            ;
		default:
			return NULL;
	}
}

/*!
 * Checks for a valid device command in a particular device.
 *
 * \param d Device type to test.
 * \param c Device command to test.
 * \return 1 (true) if device command is valid. 0 (false) otherwise.
 */
int
mdm_devicecmd_isvalid(mdm_device_type_t d, mdm_device_cmd_t c)
{
	int max;
	if(!mdm_device_isvalid_type(d))
		return 0;
	switch(d)
	{
		case MDM_DEVICE_DSLAM_ALCATEL_73xx:
			max = MDM_DSLAM_ALCATEL_73xx_CMD_DUMMY;
			break;
		case MDM_DEVICE_DSLAM_ZTE_8426:
			max = MDM_DSLAM_ZTE_8426_CMD_DUMMY;
			break;
		case MDM_DEVICE_DSLAM_ZTE_9xxx:
			max = MDM_DSLAM_ZTE_9xxx_CMD_DUMMY;
			break;
		case MDM_DEVICE_DSLAM_HUAWEI_MA5600:
			max = MDM_DSLAM_HUAWEI_MA5600_CMD_DUMMY;
			break;
        case MDM_DEVICE_DSLAM_SIEMENS_HIX5300:
            max = MDM_DSLAM_SIEMENS_HIX5300_CMD_DUMMY;
            break;
		default:
			return 0;
	}
	return (c >= 0 && c < max);
}

/*!
 * Allocates a new device to use.
 * \param d Device structure.
 * \param t Device type, one of #mdm_device_type_t.
 * \param status Where to store the result.
 */
void
mdm_device_new(
	mdm_device_t *d, mdm_device_type_t t, mdm_operation_result_t *status
)
{
	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif

	/* Init. */
	if(d == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Device is null.");
		goto mdm_device_new_done;
	}
	d->descriptor.data = NULL;
	d->descriptor.options = NULL;
	status->status = MDM_OP_OK;
	
	/* Check connection type. */
	if(!mdm_device_isvalid_type(t))
	{
		status->status = MDM_OP_ERROR;
		sprintf(
			status->status_message, "Device type is invalid: %d, aborting.", t
		);
		goto mdm_device_new_done;
	}

	/* Ok, proceed. */
	d->type = t;
	
	/* Allocate data & descriptor. */
	d->descriptor.data = (mdm_device_data_t *)
		MDM_MALLOC(MDM_DEVICE_MAX_DATA_SIZE)
	;
	if(d->descriptor.data == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error allocating memory for data");
		goto mdm_device_new_done;
	}
	d->descriptor.options = (mdm_device_options_t *)
		MDM_MALLOC(MDM_DEVICE_MAX_OPTIONS_SIZE)
	;
	if(d->descriptor.options == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error allocating memory for options");
		goto mdm_device_new_done;
	}

	/* Allocate space for buffers. */
	d->descriptor.exec_buffer_cmd = (char *)
		MDM_MALLOC(MDM_DEVICE_EXEC_BUFFER_CMD_MAX_LEN)
	;
	if(d->descriptor.exec_buffer_cmd == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error allocating exec_buffer_cmd.");
		goto mdm_device_new_done;
	}

	d->descriptor.exec_buffer_post = (char *)
		MDM_MALLOC(MDM_DEVICE_EXEC_BUFFER_POST_MAX_LEN)
	;
	if(d->descriptor.exec_buffer_post == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error allocating exec_buffer_post.");
		goto mdm_device_new_done;
	}

	d->descriptor.exec_buffer = (char *)
		MDM_MALLOC(MDM_DEVICE_EXEC_BUFFER_MAX_LEN)
	;
	if(d->descriptor.exec_buffer == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error allocating exec_buffer.");
		goto mdm_device_new_done;
	}

	/* Proceed with setup. */
	memset(&d->descriptor.connection, 0, sizeof(mdm_connection_t));
	status->status = MDM_OP_OK;
	switch(t)
	{
		case MDM_DEVICE_DSLAM_ALCATEL_73xx:
			mdm_device_dslam_alcatel_73xx_new(d, status);
			break;
		case MDM_DEVICE_DSLAM_ZTE_8426:
			mdm_device_dslam_zte_8426_new(d, status);
			break;
		case MDM_DEVICE_DSLAM_ZTE_9xxx:
			mdm_device_dslam_zte_9xxx_new(d, status);
			break;
		case MDM_DEVICE_DSLAM_HUAWEI_MA5600:
			mdm_device_dslam_huawei_ma5600_new(d, status);
			break;
        case MDM_DEVICE_DSLAM_SIEMENS_HIX5300:
            mdm_device_dslam_siemens_hix5300_new(d, status);
            break;
		case MDM_DEVICE_SIEMENS_EWSD:
		default:
			status->status = MDM_OP_ERROR;
			sprintf(
				status->status_message,
				"Device type is invalid: %d, aborting.", t
			);
			return;
	}

	/* Done. */
mdm_device_new_done:
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
	return;
}

/*!
 * Clean up the device allocated with #mdm_device_new.
 * \param d Device.
 */
void
mdm_device_free(mdm_device_t *d)
{
	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif
	if(d == NULL)
	{
		MDM_LOGWARN("Device is null.");
		goto mdm_device_free_done;
	}
	
	/* Check connection type. */
	if(!mdm_connection_isvalid_type(d->type))
	{
		MDM_LOGERROR("Device type is invalid: %d, aborting.", d->type);
		goto mdm_device_free_done;
	}

	switch(d->type)
	{
		case MDM_DEVICE_DSLAM_ALCATEL_73xx:
			mdm_device_dslam_alcatel_73xx_free(d);
			break;
		case MDM_DEVICE_DSLAM_ZTE_8426:
			mdm_device_dslam_zte_8426_free(d);
			break;
		case MDM_DEVICE_DSLAM_ZTE_9xxx:
			mdm_device_dslam_zte_9xxx_free(d);
			break;
		case MDM_DEVICE_DSLAM_HUAWEI_MA5600:
			mdm_device_dslam_huawei_ma5600_free(d);
			break;
        case MDM_DEVICE_DSLAM_SIEMENS_HIX5300:
            mdm_device_dslam_siemens_hix5300_free(d);
            break;
		case MDM_DEVICE_SIEMENS_EWSD:
			break;
		default:
			MDM_LOGERROR("Device type is invalid: %d, aborting.", d->type);
			return;
	}

	mdm_connection_free(&d->descriptor.connection);
	MDM_MFREE(&d->descriptor.exec_buffer_cmd);
	MDM_MFREE(&d->descriptor.exec_buffer_post);
	MDM_MFREE(&d->descriptor.exec_buffer);
	MDM_MFREE(&d->descriptor.data);
	MDM_MFREE(&d->descriptor.options);
	
	/* Done. */
mdm_device_free_done:
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
	return;
}

/*!
 * Open the device so we can issue commands with #mdm_device_exec. Will fill
 * #mdm_device_descriptor_t.options with needed options.
 * \param d Device structure allocated with #mdm_device_new.
 * \param c Connection to use, one of #mdm_connection_type_t.
 * \param options Options like host, port, user, password, etc.
 * \param optionslen Options length (sizeof(whatever_options_you_are_using)).
 * \param status Where to store the result.
 */ 
void
mdm_device_open(
	mdm_device_t *d, mdm_connection_type_t c,
	mdm_device_options_t *options, int optionslen,
	mdm_operation_result_t *status
)
{
	struct timeval time_start;
	struct timeval time_end;

	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif

	if((d == NULL) || (d->open == NULL))
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Device or open is null.");
	} else {
		gettimeofday(&time_start, NULL);
		status->status = MDM_OP_OK;
		memcpy(d->descriptor.options, options, optionslen);
		d->open(&d->descriptor, c, status);
		if(status->status == MDM_OP_ERROR) {
			MDM_LOGWARN("Open returned error: %s", status->status_message);
		}
	}

	/* Done. */
	gettimeofday(&time_end, NULL);
	MDM_LOGPERF(
		"Device Open took: %g secs.",
		((double)time_end.tv_sec - (double)time_start.tv_sec) + 
		(
			((double)time_end.tv_usec / (double)1000000) -
			((double)time_start.tv_usec / (double)1000000)
		)
	);
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
	return;
}

/*!
 * Closes the connection open for a device.
 * \param d Device structure allocated with #mdm_device_new.
 * \param status Where to store the result.
 */
void
mdm_device_close(mdm_device_t *d, mdm_operation_result_t *status)
{
	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif

	if((d != NULL) && (d->close != NULL)) {
		status->status = MDM_OP_OK;
		d->close(&d->descriptor, status);
		mdm_connection_close(&d->descriptor.connection, status);
	} else {
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Device or close is null.");
	}

	/* Done. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
	return;
}

/*!
 * Used to execute commands. Will first try to prepare the command via
 * #mdm_device_prepare_exec_t for the particular device driver.
 * \param d Device structure allocated with #mdm_device_new.
 * \param c Device command, device dependant.
 * \param args Command arguments, normally a csv string.
 * \param status Where to store the result error code.
 */
void
mdm_device_exec(
	mdm_device_t *d, mdm_device_cmd_t c, const char *args,
	mdm_operation_result_t *status
)
{
	struct timeval time_start;
	struct timeval time_end;

	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif
	
	/* Init. */
	if(d == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Device is null.");
		goto mdm_device_exec_done;
	}
	/* Check command type. */
	if(!mdm_devicecmd_isvalid(d->type, c))
	{
		status->status = MDM_OP_ERROR;
		sprintf(
			status->status_message, "Device command is invalid: %d, aborting.", c
		);
		goto mdm_device_exec_done;
	}
	
	/* Init. */
	status->status = MDM_OP_OK;
	d->descriptor.exec_buffer_len = 0;
	d->descriptor.exec_buffer_post_len = 0;
	d->descriptor.exec_buffer_cmd_len = 0;
	
	memset(
		d->descriptor.exec_buffer_cmd, 0, MDM_DEVICE_EXEC_BUFFER_CMD_MAX_LEN
	);
	memset(
		d->descriptor.exec_buffer_post, 0, MDM_DEVICE_EXEC_BUFFER_POST_MAX_LEN
	);
	memset(
		d->descriptor.exec_buffer, 0, MDM_DEVICE_EXEC_BUFFER_CMD_MAX_LEN
	);

	/* Prepare command. The resulting buffer goes to exec(). */
	gettimeofday(&time_start, NULL);
	d->prepare_exec(&d->descriptor, c, args, status);
	
	/* If everything's ok, execute it. The result goes to post_exec(). */
	if(status->status == MDM_OP_OK)
	{
		gettimeofday(&time_end, NULL);
		MDM_LOGPERF(
			"Device Pre Exec took: %g secs.",
			((double)time_end.tv_sec - (double)time_start.tv_sec) + 
			(
				((double)time_end.tv_usec / (double)1000000) -
				((double)time_start.tv_usec / (double)1000000)
			)
		);
		gettimeofday(&time_start, NULL);
		d->exec(&d->descriptor, status);
	} else {
		MDM_LOGWARN("PrepareExec returned error: %s", status->status_message);
	}
	
	/* If everything's ok, post process it. The result is returned to
	 * the caller.
	 */
	if(status->status == MDM_OP_OK)
	{
		gettimeofday(&time_end, NULL);
		MDM_LOGPERF(
			"Device Exec took: %g secs.",
			((double)time_end.tv_sec - (double)time_start.tv_sec) + 
			(
				((double)time_end.tv_usec / (double)1000000) -
				((double)time_start.tv_usec / (double)1000000)
			)
		);

		/* Check for errors in response. */
		d->check_error(&d->descriptor, status);
		if(status->status != MDM_OP_ERROR)
		{
			gettimeofday(&time_start, NULL);
			d->post_exec(&d->descriptor, c, status);
			gettimeofday(&time_end, NULL);
			MDM_LOGPERF(
				"Device Post Exec took: %g secs.",
				((double)time_end.tv_sec - (double)time_start.tv_sec) + 
				(
					((double)time_end.tv_usec / (double)1000000) -
					((double)time_start.tv_usec / (double)1000000)
				)
			);
		} else {
			MDM_LOGWARN("CheckError returned error: %s", status->status_message);
		}
	} else {
		MDM_LOGWARN("Exec returned error: %s", status->status_message);
	}
	/* Done. */
mdm_device_exec_done:
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
	return;
}

/*!
 * Used to get #mdm_device_descriptor_t.exec_buffer_cmd
 *
 * \param d Device structure allocated with #mdm_device_new.
 *
 * \return Original device response buffer.
 */
const char *
mdm_device_get_buffer(mdm_device_t *d)
{
	if(d == NULL)
	{
		MDM_LOGWARN("Device is null");
		return NULL;
	}
	return d->descriptor.exec_buffer;
}

/*!
 * Used to get #mdm_device_descriptor_t.exec_buffer_cmd
 *
 * \param d Device structure allocated with #mdm_device_new.
 *
 * \return Prepared command buffer.
 */
const char *
mdm_device_get_buffer_cmd(mdm_device_t *d)
{
	if(d == NULL)
	{
		MDM_LOGWARN("Device is null");
		return NULL;
	}
	return d->descriptor.exec_buffer_cmd;
}

/*!
 * Used to get #mdm_device_descriptor_t.exec_buffer_post
 *
 * \param d Device structure allocated with #mdm_device_new.
 *
 * \return Post process buffer.
 */
const char *
mdm_device_get_buffer_post(mdm_device_t *d)
{
	if(d == NULL)
	{
		MDM_LOGWARN("Device is null");
		return NULL;
	}
	return d->descriptor.exec_buffer_post;
}

/*!
 * Used to get #mdm_device_descriptor_t.exec_buffer_len
 *
 * \param d Device structure allocated with #mdm_device_new.
 *
 * \return Length for the original device response buffer.
 */
int
mdm_device_get_buffer_len(mdm_device_t *d)
{
	if(d == NULL)
	{
		MDM_LOGWARN("Device is null");
		return 0;
	}
	return d->descriptor.exec_buffer_len;
}

/*!
 * Used to get #mdm_device_descriptor_t.exec_buffer_cmd_len
 *
 * \param d Device structure allocated with #mdm_device_new.
 *
 * \return Length for the prepared command buffer.
 */
int
mdm_device_get_buffer_cmd_len(mdm_device_t *d)
{
	if(d == NULL)
	{
		MDM_LOGWARN("Device is null");
		return 0;
	}
	return d->descriptor.exec_buffer_cmd_len;
}

/*!
 * Used to get #mdm_device_descriptor_t.exec_buffer_post_len
 *
 * \param d Device structure allocated with #mdm_device_new.
 *
 * \return Length for the post process buffer.
 */
int
mdm_device_get_buffer_post_len(mdm_device_t *d)
{
	if(d == NULL)
	{
		MDM_LOGWARN("Device is null");
		return 0;
	}
	return d->descriptor.exec_buffer_post_len;
}

/*******************************************************************************
 * CODE ENDS.
 ******************************************************************************/
