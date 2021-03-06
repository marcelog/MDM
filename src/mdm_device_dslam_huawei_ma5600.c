/*!
 * \file mdm_device_dslam_huawei_ma5600.c DSLAM HUAWEI MA5600 implementation.
 *
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#include	<stdio.h>
#include	<string.h>
#include	<math.h>
#include	<errno.h>
#include	<unistd.h>
#include	<mdm_parser_dslam_huawei_ma5600.h>
#include	<mdm.h>

#define	DSLAM_HUAWEI_MA5600_CMD_ARG	"%%ARG%%" /*!< Use this string to
	identify where arguments go inside the command. */

/*!
 * Corresponding command names from #mdm_device_cmd_dslam_huawei_ma5600_t
 */
const char *MDM_DEVICE_CMDNAME_DSLAM_HUAWEI_MA5600_STR[] =
{
/* 0  */ "NOP",
/* 1  */ "Uptime",
/* 2  */ "Backplane Version",
/* 3  */ "Slot Version",
/* 4  */ "Show backplane temperature",
/* 5  */ "Show backplane memory",
/* 6  */ "Show board description",
/* 7  */ "Show board",
/* 8  */ "Display all ports state",
/* 9  */ "Display all mac addresses",
/* 10 */ "Show port",
/* 11 */ "Show syslog output configuration",
/* 12 */ "Show full port",
/* 13 */ "Show line profiles",
/* 14 */ "Show line profile",
/* 15 */ "Show service port",
/* 16 */ "Show alarms",
/* 17 */ "Show CPU",
/* 18 */ "Show routes",
/* 19 */ "Show timezone",
/* 20 */ "Show boards",
/* 21 */ "Show frames",
	NULL
};

/*!
 * Contains the number of arguments for each command.
 */
static int MDM_DEVICE_CMD_DSLAM_HUAWEI_MA5600_ARGSN[] =
{
/* 0  */ 0,
/* 1  */ 0,
/* 2  */ 1,
/* 3  */ 1,
/* 4  */ 1,
/* 5  */ 1,
/* 6  */ 1,
/* 7  */ 1,
/* 8  */ 0,
/* 9  */ 0,
/* 10 */ 1,
/* 11 */ 0,
/* 12 */ 1,
/* 13 */ 0,
/* 14 */ 1,
/* 15 */ 0,
/* 16 */ 0,
/* 17 */ 1,
/* 18 */ 0,
/* 19 */ 0,
/* 20 */ 1,
/* 21 */ 0
};

/*!
 * Corresponding processing routines post exec.
 */
const mdm_device_dslam_huawei_ma5600_post_exec_t 
MDM_DEVICE_CMD_DSLAM_HUAWEI_MA5600_PROCESS[] =
{
/* 0  */ NULL,
/* 1  */ NULL,
/* 2  */ NULL,
/* 3  */ NULL,
/* 4  */ NULL,
/* 5  */ NULL,
/* 6  */ NULL,
/* 7  */ NULL,
/* 8  */ NULL,
/* 9  */ NULL,
/* 10 */ NULL,
/* 11 */ NULL,
/* 12 */ NULL,
/* 13 */ NULL,
/* 14 */ dslam_huawei_ma5600_service_profile,
/* 15 */ dslam_huawei_ma5600_service_ports,
/* 16 */ dslam_huawei_ma5600_alarms,
/* 17 */ dslam_huawei_ma5600_cpu,
/* 18 */ dslam_huawei_ma5600_routes,
/* 19 */ dslam_huawei_ma5600_timezone,
/* 20 */ dslam_huawei_ma5600_boards,
/* 21 */ dslam_huawei_ma5600_frames
};

/*!
 * Corresponding command strings from #mdm_device_cmd_dslam_huawei_ma5600_t
 */
const char *MDM_DEVICE_CMD_DSLAM_HUAWEI_MA5600_STR[] =
{
/* 0  */	"",
/* 1  */ "display sysuptime",
/* 2  */ "display version backplane %%ARG%%",
/* 3  */ "display version %%ARG%%",
/* 4  */ "display temperature %%ARG%%",
/* 5  */ "display mem %%ARG%%",
/* 6  */ "display board desc %%ARG%%", /* 0, 0/0, 0/1 */
/* 7  */ "display board %%ARG%%", /* 0, 0/0, 0/1 */
/* 8  */ "display adsl port state all\r\n",
/* 9  */ "display mac-address all\r\n",
/* 10 */ "display interface adsl %%ARG%%", /* 0/4/0 */
/* 11 */ "display syslog output configuration",
/* 12 */ "display line operation port %%ARG%%\r\ny",
/* 13 */ "display adsl line-profile\r\n",
/* 14 */ "display adsl line-profile %%ARG%%",
/* 15 */ "display service-port all sort-by port",
/* 16 */ "display alarm list all\r\n",
/* 17 */ "display cpu %%ARG%%",
/* 18 */ "display ip routing-table\r\n",
/* 19 */ "display timezone",
/* 20 */ "display board %%ARG%%", // 0, 0/0
/* 21 */ "display frame info",
	NULL
};

/*******************************************************************************
 * Forward Declarations.
 ******************************************************************************/

/*******************************************************************************
 * CODE STARTS.
 ******************************************************************************/

/*!
 * Will try to open a connection, calling #mdm_connection_open and trying to
 * login to DSLAM. 
 * \param d Device descriptor.
 * \param ct One of #mdm_connection_type_t
 * \param status Where to store the result.
 */
void
mdm_device_dslam_huawei_ma5600_open(
	mdm_device_descriptor_t *d, mdm_connection_type_t ct,
	mdm_operation_result_t *status
)
{
	mdm_driver_telnet_options_t telnet_options;
	char *host_port_token = NULL;
	mdm_device_dslam_huawei_ma5600_options_t *options;
	mdm_connection_options_t *conn_options;
	int i;
	char buffer[4096];
	int bufflen;
	char *envcmds[] = {
        "undo alarm output all",
        "undo smart",
		"scroll 512",
		"idle-timeout 255",
		"undo interactive",
		"undo terminal monitor",
		"enable"
	};
	int envcmdslen[] = { 21, 10, 10, 16, 16, 21, 6};
	
	mdm_device_dslam_huawei_ma5600_data_t *data =
		(mdm_device_dslam_huawei_ma5600_data_t *)d->data;
	options = (mdm_device_dslam_huawei_ma5600_options_t *)d->options;
	switch(ct)
	{
		case MDM_CONNECTION_TELNET:
			mdm_connection_new(&d->connection, MDM_CONNECTION_TELNET, status);
			if(status->status == MDM_OP_ERROR)
				return;
			/* Copy options. */
			host_port_token = strchr(options->target, ':');
			if(host_port_token == NULL)
			{
				status->status = MDM_OP_ERROR;
				sprintf(status->status_message, "Missing : in target.");
				return;
			}
			snprintf(
				telnet_options.host, host_port_token - options->target + 1,
				"%s", options->target
			);
			snprintf(
				telnet_options.service, sizeof(telnet_options.service),
				"%s", host_port_token + 1
			);
			telnet_options.to_recv = options->to_recv;
			telnet_options.to_connect = options->to_connect;
			telnet_options.echo_cancel = 0;
			telnet_options.send_eol = 1;
#if MDM_DEBUG_MESSAGES > 0
			MDM_LOGDEBUG(
				"Using %s:%s", telnet_options.host, telnet_options.service
			);
#endif
			conn_options = (mdm_connection_options_t)&telnet_options;
			break;
		default:
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "Connection type not supported.");
			return;
	}
	/* Finally, open the connection to the host. */
	mdm_connection_open(&d->connection, conn_options, status);
	if(status->status == MDM_OP_ERROR)
	{
		return;
	}

	do
	{
		bufflen = sizeof(buffer);
		memset(buffer, 0, bufflen);
		mdm_connection_recv(&d->connection, buffer, &bufflen, status);
		if(status->status == MDM_OP_ERROR)
		{
			return;
		}
		if(bufflen > 0)
		{
#if MDM_DEBUG_MESSAGES > 0
			MDM_LOGDEBUG("Got %s", buffer);
#endif
		} else if(bufflen != MDM_TELNET_DRIVER_ONLY_TELNET_DATA) {
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "Login timeout.");
			return;
		}
	} while(strstr(buffer, "User name:") == NULL);
	mdm_connection_send(
		&d->connection, options->username, strlen(options->username), status
	);
	if(status->status == MDM_OP_ERROR)
	{
		return;
	}
	/* Wait for password. */
	do
	{
		bufflen = sizeof(buffer);
		memset(buffer, 0, bufflen);
		mdm_connection_recv(&d->connection, buffer, &bufflen, status);
		if(status->status == MDM_OP_ERROR)
		{
			return;
		}
		if(bufflen > 0)
		{
#if MDM_DEBUG_MESSAGES > 0
			MDM_LOGDEBUG("Got %s", buffer);
#endif
		} else if(bufflen != MDM_TELNET_DRIVER_ONLY_TELNET_DATA) {
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "Login timeout.");
			return;
		}
		if(strstr(buffer, "Reenter times have reached the upper limit.") != NULL)
		{
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "Too many connections.");
			return;
		}
	} while(strstr(buffer, "User password:") == NULL);
	mdm_connection_send(
		&d->connection, options->password, strlen(options->password), status
	);
	if(status->status == MDM_OP_ERROR)
	{
		return;
	}
	/* Wait for login result. */
	do
	{
		bufflen = sizeof(buffer);
		memset(buffer, 0, bufflen);
		mdm_connection_recv(&d->connection, buffer, &bufflen, status);
		if(status->status == MDM_OP_ERROR)
		{
			return;
		}
		if(bufflen > 0)
		{
#if MDM_DEBUG_MESSAGES > 0
			MDM_LOGDEBUG("Got %s", buffer);
#endif
		} else if(bufflen != MDM_TELNET_DRIVER_ONLY_TELNET_DATA) {
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "Login timeout.");
			return;
		}
		if(strstr(buffer, "Username or password invalid") != NULL)
		{
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "Login incorrect!");
			return;
        } else if(strstr(buffer, "Reenter times have reached the upper limit")) {
            status->status = MDM_OP_ERROR;
            sprintf(status->status_message, "No more connections allowed by DSLAM");
            return;
        }
	} while(strstr(buffer, ">") == NULL);
	/* It doesn't matter the prompt, but we need the length so we can strip
	 * the command lines later on. 
	 */
	host_port_token = strrchr(buffer, '\n');
	/**
	 * TODO: is this ok?
	 */
	if(host_port_token < buffer)
	{
		data->promptlen = strlen(buffer) - 2;
		snprintf(data->prompt, data->promptlen, "%s", buffer);
	} else {
		data->promptlen = strlen(buffer) - (strrchr(buffer, '\n') - buffer) - 2;
		snprintf(data->prompt, data->promptlen, "%s", strrchr(buffer, '\n') + 1);
	}
	/* Set environment properties. */
	for(i = 0; i < 7; i++)
	{
		d->exec_buffer_cmd_len = envcmdslen[i];
		snprintf(
			d->exec_buffer_cmd, d->exec_buffer_cmd_len + 1, "%s", envcmds[i]
		);
		mdm_device_dslam_huawei_ma5600_exec(d, status);
		if(status->status == MDM_OP_ERROR)
		{
			return;
		}
	}
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Login successful: promptlen: %s %d", data->prompt, data->promptlen);
#endif
	return;
}

/*!
 * Will try to prepare a command for exec.
 * \param d Device descriptor.
 * \param c Command, one of #mdm_device_cmd_dslam_huawei_ma5600_t
 * \param args Command arguments, normally a csv string.
 * \param status Where to store the result.
 */
void
mdm_device_dslam_huawei_ma5600_prepare_exec(
	mdm_device_descriptor_t *d, mdm_device_cmd_t c,
	const char *args, mdm_operation_result_t *status
)
{
	const char *cmd = MDM_DEVICE_CMD_DSLAM_HUAWEI_MA5600_STR[c];
	int cmdlen = strlen(cmd);
	int cmdargsn = MDM_DEVICE_CMD_DSLAM_HUAWEI_MA5600_ARGSN[c];
	int passedargs = 0;
	int argslen = 0;
	int argstoken = 0;
	int precmdlen;
	
	const char *p2 = NULL;
	const char *p1 = NULL;
	const char *cmdp1 = NULL;
	const char *cmdp2 = NULL;

	/* Count number of passed arguments. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Counting arguments.");
#endif
	if(args != NULL)
	{
		argslen += strlen(args);
		passedargs++;
		p1 = args;
		while((p1 = strchr(p1, ',')) != NULL)
		{
			p1++;
			passedargs++;
			argstoken++;
		}
	}	
	/* Validate arguments. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Arguments #%d", passedargs);
#endif
	if(cmdargsn != passedargs)
	{
		status->status = MDM_OP_ERROR;
		sprintf(
			status->status_message,
			"Invalid number of arguments. Got: %d and needed %d",
			passedargs, cmdargsn
		);
		return;
	}
	
	/*
	 * Calculate bufflen needed, minus the length of the tokens of the command
	 * and the arguments.
	 */
	d->exec_buffer_cmd_len =
		cmdlen + argslen -
		(strlen(DSLAM_HUAWEI_MA5600_CMD_ARG) * cmdargsn) -
		argstoken
	;
	precmdlen = 
		cmdlen + argslen -
		(strlen(DSLAM_HUAWEI_MA5600_CMD_ARG) * cmdargsn) -
		argstoken
	;
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG(
		"clen: %d, alen: %d, ctoklen: %d, atoklen: %d",
		cmdlen, argslen, strlen(DSLAM_HUAWEI_MA5600_CMD_ARG) * cmdargsn, argstoken
	);
#endif

	/* If command has no arguments, copy the buffer and return immediately. */
	if(cmdargsn == 0)
	{
		snprintf(d->exec_buffer_cmd, d->exec_buffer_cmd_len + 1, "%s", cmd);
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG(
		"Prepared cmd %d (%d): %.*s", cmdlen,
		d->exec_buffer_cmd_len, d->exec_buffer_cmd_len, d->exec_buffer_cmd
	);
#endif
		return;
	}
	/* Process arguments. */
	for(cmdp1 = cmd, p1 = args; passedargs > 0; passedargs--)
	{
		/* Find next arg token in device command. */
		cmdp2 = strstr(cmdp1, DSLAM_HUAWEI_MA5600_CMD_ARG);

		/* If none found, copy to the end of the command. */
		if(cmdp2 == NULL)
			cmdp2 = cmdp1 + strlen(cmdp1);

		/* Find next arg token in args. */
		p2 = strchr(p1, ',');

		/* If none cound, copy to the end of the arguments. */
		if(p2 == NULL)
			p2 = p1 + strlen(p1);

		/* Concat first part of the command. */
		strncat(d->exec_buffer_cmd, cmdp1, cmdp2 - cmdp1);

		/* Concat argument. */
		strncat(d->exec_buffer_cmd, p1, p2 - p1);
#if MDM_DEBUG_MESSAGES > 0
		MDM_LOGDEBUG("Argument: %.*s", p2 - p1, p1);
#endif

		/* Advance pointers. */
		p1 = p2 + 1;
		cmdp1 = cmdp2 + strlen(DSLAM_HUAWEI_MA5600_CMD_ARG);
	}
	/* Concat last part of the command. */
	strncat(d->exec_buffer_cmd, cmdp1, cmdp2 - cmdp1);
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG(
		"Prepared cmd (%d): %.*s",
		d->exec_buffer_cmd_len, d->exec_buffer_cmd_len, d->exec_buffer_cmd
	);
#endif
	return;
}

/*! Will close the connection. Will call #mdm_connection_close and
 * #mdm_connection_free.
 * \param d Device descriptor.
 * \param status Where to store the result.
 */
void
mdm_device_dslam_huawei_ma5600_close(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif

	/* Done. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
	return;
}

/*!
 * Will search for known error strings in response buffer.
 * \param d Device descriptor.
 * \param status Where to store the result error code.
 * \return void
 */
void
mdm_device_dslam_huawei_ma5600_check_error(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	const char *error;
	const char *tmp;
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif
	if((error = strstr(d->exec_buffer, "Failure:")) != NULL) {
		tmp = strchr(d->exec_buffer, '\n');
		if (tmp == NULL || tmp > error) {
			status->status = MDM_OP_ERROR;
			snprintf(
                status->status_message, d->exec_buffer_len, "%s", d->exec_buffer
			);
		}
	} else if((error = strstr(d->exec_buffer, "% Parameter error")) != NULL) {
		status->status = MDM_OP_ERROR;
		snprintf(
			status->status_message, strrchr(error, '\n') - error, "%s", error
		);
    } else if((error = strstr(d->exec_buffer, "the error locates at")) != NULL) {
        status->status = MDM_OP_ERROR;
        snprintf(
            status->status_message, d->exec_buffer_len, "%s", d->exec_buffer
        );
    }

	/*% Unknown command*/
	/* Done. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
}

/*!
 * Will exec a command. It will strip the prompt from
 * the final line and the issued command from the first n lines. See 
 * #mdm_device_dslam_huawei_ma5600_data_t.promptlen
 * \param d Device descriptor.
 * \param status Where to store the result error code.
 */
void
mdm_device_dslam_huawei_ma5600_exec(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif
	char tempbuffer[256];
	int tempbufferlen;
	char *lastlinebeforeprompt = NULL;
	char *foundcmd = NULL;
	char *prompt = NULL;
	mdm_device_dslam_huawei_ma5600_data_t *data =
		(mdm_device_dslam_huawei_ma5600_data_t *)d->data;
	/* We have to skip the issued command. */
	float linestoskip = ceilf((float)(d->exec_buffer_cmd_len) / (float)(80));
	
	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif

	/* Send command. */
	mdm_connection_send(
		&d->connection, d->exec_buffer_cmd, d->exec_buffer_cmd_len, status
	);
	if(status->status == MDM_OP_ERROR)
	{
		return;
	}

	/* Wait for result. */
	do
	{
		tempbufferlen = sizeof(tempbuffer);
		memset(tempbuffer, 0, tempbufferlen);
		mdm_connection_recv(&d->connection, tempbuffer, &tempbufferlen, status);
		if(status->status == MDM_OP_ERROR)
		{
			return;
		}
		if(tempbufferlen > 0)
		{
#if MDM_DEBUG_MESSAGES > 0
			MDM_LOGDEBUG("Got %s", tempbuffer);
#endif
		} else {
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "Receive timeout.");
			return;
		}
		d->exec_buffer_len += tempbufferlen;
		strncat(d->exec_buffer, tempbuffer, tempbufferlen);
		prompt = strstr(d->exec_buffer, ">");
		if (prompt != NULL) {
			prompt += data->promptlen;
			if (*prompt == '#' || *prompt == '>') {
				lastlinebeforeprompt = strrchr(d->exec_buffer, 13) - 1;
				*lastlinebeforeprompt = 0;
				break;
			}
		}
	} while(1);
	/*
	 * Strip command from response: We end up skipping the length of the command.
	 * Use post buffer as temp.
	 */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG(
		"Skipping %f lines from cmdlen: %d.", linestoskip, d->exec_buffer_cmd_len
	);
#endif
	memcpy(d->exec_buffer_post, d->exec_buffer, strlen(d->exec_buffer));
	foundcmd = d->exec_buffer_post;
	do
	{
		foundcmd = strstr(foundcmd, "\r\n");
		if(foundcmd == NULL)
			break;
		linestoskip--;
		foundcmd += 2;
	} while(linestoskip > 0);
	if(foundcmd != NULL)
	{
#if MDM_DEBUG_MESSAGES > 0
		MDM_LOGDEBUG("Stripping cmd.");
#endif
		d->exec_buffer_len = strlen(foundcmd);
		snprintf(d->exec_buffer, d->exec_buffer_len, "%s", foundcmd);
	} else {
#if MDM_DEBUG_MESSAGES > 0
		MDM_LOGDEBUG("No output from command?");
#endif
		d->exec_buffer_len = strlen(d->exec_buffer_post) - d->exec_buffer_cmd_len;
		snprintf(
			d->exec_buffer, d->exec_buffer_len, "%s",
			d->exec_buffer_post + d->exec_buffer_cmd_len
		);
	}
	/* Done. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
}

/*!
 * Called by #mdm_device_new to initialize a device dslam huawei ma5600.
 * \param d Device.
 * \param status Where to store the result.
 */
void
mdm_device_dslam_huawei_ma5600_new(
	mdm_device_t *d, mdm_operation_result_t *status
)
{
	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif

	d->open = mdm_device_dslam_huawei_ma5600_open;
	d->exec = mdm_device_dslam_huawei_ma5600_exec;
	d->prepare_exec = mdm_device_dslam_huawei_ma5600_prepare_exec;
	d->post_exec = mdm_device_dslam_huawei_ma5600_post_exec;
	d->close = mdm_device_dslam_huawei_ma5600_close;
	d->check_error = mdm_device_dslam_huawei_ma5600_check_error;

	/* Done. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
	return;
}

/*!
 * Called by #mdm_device_free to deinitialize a device dslam huawei ma5600.
 * \param d Device.
 */
void
mdm_device_dslam_huawei_ma5600_free(mdm_device_t *d)
{
	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif

	/* Done. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
}

/*!
 * This will process the output from #mdm_device_dslam_huawei_ma5600_exec.
 * Dont call this directly, use #mdm_device_exec.
 * \param d Device descriptor.
 * \param c Device command used to get response.
 * \param status Result of the operation.
 */
void
mdm_device_dslam_huawei_ma5600_post_exec(
	mdm_device_descriptor_t * d, 
	mdm_device_cmd_t c, mdm_operation_result_t *status

)
{
	if(MDM_DEVICE_CMD_DSLAM_HUAWEI_MA5600_PROCESS[c] == NULL)
	{
		d->exec_buffer_post_len = d->exec_buffer_len;
		memcpy(d->exec_buffer_post, d->exec_buffer, d->exec_buffer_len);
	} else {
		MDM_DEVICE_CMD_DSLAM_HUAWEI_MA5600_PROCESS[c](d, status);
	}
}
/*******************************************************************************
 * CODE ENDS.
 ******************************************************************************/
