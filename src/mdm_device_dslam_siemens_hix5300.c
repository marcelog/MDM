/*!
 * \file mdm_device_dslam_siemens_hix5300.c DSLAM SIEMENS HIX 5300 implementation.
 *
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#include	<stdio.h>
#include	<string.h>
#include	<math.h>
#include	<errno.h>
#include	<unistd.h>
#include	<mdm_parser_dslam_siemens_hix5300.h>
#include	<mdm.h>

#define	DSLAM_SIEMENS_HIX5300_CMD_ARG	"%%ARG%%" /*!< Use this string to
	identify where arguments go inside the command. */

/*!
 * Corresponding command names from #mdm_device_cmd_dslam_siemens_hix5300_t
 */
const char *MDM_DEVICE_CMDNAME_DSLAM_SIEMENS_HIX5300_STR[] =
{
/* 0  */ "NOP",
/* 1  */ "Show running config",
	NULL
};

/*!
 * Contains the number of arguments for each command.
 */
static int MDM_DEVICE_CMD_DSLAM_SIEMENS_HIX5300_ARGSN[] =
{
/* 0  */ 0,
/* 1  */ 0
};

/*!
 * Corresponding processing routines post exec.
 */
const mdm_device_dslam_siemens_hix5300_post_exec_t
MDM_DEVICE_CMD_DSLAM_SIEMENS_HIX5300_PROCESS[] =
{
/* 0  */ dslam_siemens_hix5300_nop,
/* 1  */ dslam_siemens_hix5300_nop
};

/*!
 * Corresponding command strings from #mdm_device_cmd_dslam_siemens_hix5300_t
 */
const char *MDM_DEVICE_CMD_DSLAM_SIEMENS_HIX5300_STR[] =
{
/* 0  */	"",
/* 1  */	"show running-config",
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
mdm_device_dslam_siemens_hix5300_open(
	mdm_device_descriptor_t *d, mdm_connection_type_t ct,
	mdm_operation_result_t *status
)
{
	mdm_driver_telnet_options_t telnet_options;
	mdm_driver_ssh2_options_t ssh2_options;
	char *host_port_token = NULL;
	mdm_device_dslam_siemens_hix5300_options_t *options;
	mdm_connection_options_t *conn_options;
	int i;
	char buffer[4096];
	int bufflen;
	char *envcmds[] = {
		"terminal length 0"
	};
	int envcmdslen[] = { 17 };

	mdm_device_dslam_siemens_hix5300_data_t *data =
		(mdm_device_dslam_siemens_hix5300_data_t *)d->data;
	options = (mdm_device_dslam_siemens_hix5300_options_t *)d->options;
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
		case MDM_CONNECTION_SSH2:
			mdm_connection_new(&d->connection, MDM_CONNECTION_SSH2, status);
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
				ssh2_options.host, host_port_token - options->target + 1,
				"%s", options->target
			);
			snprintf(
				ssh2_options.service, sizeof(ssh2_options.service),
				"%s", host_port_token + 1
			);
			snprintf(
				ssh2_options.username, sizeof(ssh2_options.username),
				"%s", options->username
			);
			snprintf(
				ssh2_options.password, sizeof(ssh2_options.password),
				"%s", options->password
			);
			ssh2_options.to_recv = options->to_recv;
			ssh2_options.to_connect = options->to_connect;
			ssh2_options.echo_cancel = 0;
#if MDM_DEBUG_MESSAGES > 0
			MDM_LOGDEBUG(
				"Using %s:%s", ssh2_options.host, ssh2_options.service
			);
#endif
			conn_options = (mdm_connection_options_t)&ssh2_options;
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
	} while(strstr(buffer, "login:") == NULL);
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
	} while(strstr(buffer, "Password:") == NULL);
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
		if(strstr(buffer, "Login incorrect") != NULL)
		{
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "Login incorrect!");
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
		snprintf(data->prompt, data->promptlen + 1, "%s", buffer);
	}
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Login successful: promptlen: |%s| %d", data->prompt, data->promptlen);
#endif
	/* Set environment properties. */
	for(i = 0; i < 1; i++)
	{
		d->exec_buffer_cmd_len = envcmdslen[i];
		snprintf(
			d->exec_buffer_cmd, d->exec_buffer_cmd_len + 1, "%s", envcmds[i]
		);
		mdm_device_dslam_siemens_hix5300_exec(d, status);
		if(status->status == MDM_OP_ERROR)
		{
			return;
		}
	}
    mdm_connection_send(&d->connection, "enable", 6, status);
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
#if MDM_DEBUG_MESSAGES > 0
        MDM_LOGDEBUG("Got %s", buffer);
#endif
    } while(strstr(buffer, "word") == NULL);
    mdm_connection_send(&d->connection, options->enable, strlen(options->enable), status);
    if(status->status == MDM_OP_ERROR)
    {
        return;
    }
    do {
        bufflen = sizeof(buffer);
        memset(buffer, 0, bufflen);
        mdm_connection_recv(&d->connection, buffer, &bufflen, status);
        if(status->status == MDM_OP_ERROR)
        {
            return;
        }
#if MDM_DEBUG_MESSAGES > 0
        MDM_LOGDEBUG("Got %s", buffer);
#endif
        if(strstr(buffer, "word") != NULL)
        {
            status->status = MDM_OP_ERROR;
            sprintf(status->status_message, "Bad enable password");
            return;
        }
    } while(strstr(buffer, "#") == NULL);
#if MDM_DEBUG_MESSAGES > 0
               MDM_LOGDEBUG("Enable OK");
#endif

	return;
}

/*!
 * Will try to prepare a command for exec.
 * \param d Device descriptor.
 * \param c Command, one of #mdm_device_cmd_dslam_siemens_hix5300_t
 * \param args Command arguments, normally a csv string.
 * \param status Where to store the result.
 */
void
mdm_device_dslam_siemens_hix5300_prepare_exec(
	mdm_device_descriptor_t *d, mdm_device_cmd_t c,
	const char *args, mdm_operation_result_t *status
)
{
	const char *cmd = MDM_DEVICE_CMD_DSLAM_SIEMENS_HIX5300_STR[c];
	int cmdlen = strlen(cmd);
	int cmdargsn = MDM_DEVICE_CMD_DSLAM_SIEMENS_HIX5300_ARGSN[c];
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
		(strlen(DSLAM_SIEMENS_HIX5300_CMD_ARG) * cmdargsn) -
		argstoken
	;
	precmdlen =
		cmdlen + argslen -
		(strlen(DSLAM_SIEMENS_HIX5300_CMD_ARG) * cmdargsn) -
		argstoken
	;
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG(
		"clen: %d, alen: %d, ctoklen: %d, atoklen: %d",
		cmdlen, argslen, strlen(DSLAM_SIEMENS_HIX5300_CMD_ARG) * cmdargsn, argstoken
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
		cmdp2 = strstr(cmdp1, DSLAM_SIEMENS_HIX5300_CMD_ARG);

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
		cmdp1 = cmdp2 + strlen(DSLAM_SIEMENS_HIX5300_CMD_ARG);
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
mdm_device_dslam_siemens_hix5300_close(
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
mdm_device_dslam_siemens_hix5300_check_error(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	const char *error;
	const char *tmp;
	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif
	if((error = strstr(d->exec_buffer, "Failure:")) != NULL) {
		tmp = strchr(d->exec_buffer, '\n');
		if (tmp == NULL || tmp > error) {
			status->status = MDM_OP_ERROR;
			snprintf(
				status->status_message, strrchr(error, '\n') - error, "%s", error
			);
		}
	} else if((error = strstr(d->exec_buffer, "% Parameter error")) != NULL) {
		status->status = MDM_OP_ERROR;
		snprintf(
			status->status_message, d->exec_buffer_len, "%s", d->exec_buffer
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
 * #mdm_device_dslam_siemens_hix5300_data_t.promptlen
 * \param d Device descriptor.
 * \param status Where to store the result error code.
 */
void
mdm_device_dslam_siemens_hix5300_exec(
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
	mdm_device_dslam_siemens_hix5300_data_t *data =
		(mdm_device_dslam_siemens_hix5300_data_t *)d->data;
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
		prompt = strstr(d->exec_buffer, data->prompt);
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
 * Called by #mdm_device_new to initialize a device dslam siemens hix5300.
 * \param d Device.
 * \param status Where to store the result.
 */
void
mdm_device_dslam_siemens_hix5300_new(
	mdm_device_t *d, mdm_operation_result_t *status
)
{
	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif

	d->open = mdm_device_dslam_siemens_hix5300_open;
	d->exec = mdm_device_dslam_siemens_hix5300_exec;
	d->prepare_exec = mdm_device_dslam_siemens_hix5300_prepare_exec;
	d->post_exec = mdm_device_dslam_siemens_hix5300_post_exec;
	d->close = mdm_device_dslam_siemens_hix5300_close;
	d->check_error = mdm_device_dslam_siemens_hix5300_check_error;

	/* Done. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
	return;
}

/*!
 * Called by #mdm_device_free to deinitialize a device dslam siemens hix5300.
 * \param d Device.
 */
void
mdm_device_dslam_siemens_hix5300_free(mdm_device_t *d)
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
 * This will process the output from #mdm_device_dslam_siemens_hix5300_exec.
 * Dont call this directly, use #mdm_device_exec.
 * \param d Device descriptor.
 * \param c Device command used to get response.
 * \param status Result of the operation.
 */
void
mdm_device_dslam_siemens_hix5300_post_exec(
	mdm_device_descriptor_t * d,
	mdm_device_cmd_t c, mdm_operation_result_t *status

)
{
	if(MDM_DEVICE_CMD_DSLAM_SIEMENS_HIX5300_PROCESS[c] == NULL)
	{
		d->exec_buffer_post_len = d->exec_buffer_len;
		memcpy(d->exec_buffer_post, d->exec_buffer, d->exec_buffer_len);
	} else {
		MDM_DEVICE_CMD_DSLAM_SIEMENS_HIX5300_PROCESS[c](d, status);
	}
}
/*******************************************************************************
 * CODE ENDS.
 ******************************************************************************/