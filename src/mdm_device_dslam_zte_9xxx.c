/*!
 * \file mdm_device_dslam_zte_9xxx.c DSLAM ZTE 9000 series implementation.
 *
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#include	<stdio.h>
#include	<string.h>
#include	<math.h>
#include	<errno.h>
#include	<unistd.h>
#include	<mdm_parser_dslam_zte_9xxx.h>
#include	<mdm.h>

#define	DSLAM_ZTE_9xxx_CMD_ARG	"%%ARG%%" /*!< Use this string to identify
	where arguments go inside the command. */

/*!
 * Corresponding command names from #mdm_device_cmd_dslam_zte_9xxx_t
 */
const char *MDM_DEVICE_CMDNAME_DSLAM_ZTE_9xxx_STR[] =
{
/* 0  */ "NOP",
/* 1  */ "Show Slots",
/* 2  */ "Show running config",
/* 3  */ "Show Service Profiles",
/* 4  */ "Software version",
/* 5  */ "Show Port ATM PVC",
/* 6  */ "Show Uptime",
/* 7  */ "Show Slot Ports",
/* 8  */ "Show SCB Detail",
/* 9  */ "Show Slot Detail",
/* 10 */ "Show Port Detail",
/* 11 */ "Configure timezone",
/* 12 */ "Get NTP",
/* 13 */ "Get Service Profile",
/* 14 */ "Create Service Profile",
/* 15 */ "Configure Service Profile",
/* 16 */ "Delete Service Profile",
/* 17 */ "Get mac addresses in slots",
/* 18 */ "Get mac addresses in port",
	NULL
};

/*!
 * Contains the number of arguments for each command.
 * For command 11:
 * arg1 = timezone in hours.
 *
 * For command 15:
 * arg1 =  Profile name.
 * arg2 =  AtucChanConfFastMinTxRate[32-131040](Def:32)
 * arg3 =  AtucChanConfInterMinTxRate[32-131040](Def:32)
 * arg4 =  AtucChanConfFastMaxTxRate[32-131040](Def:1024)
 * arg5 =  AtucChanConfInterMaxTxRate[32-131040](Def:1024)
 * arg6 =  AtucChanConfMaxInterDelay[0-255](Def:8)
 * arg7 =  AturChanConfFastMinTxRate[32-10240](Def:32)
 * arg8 =  AturChanConfInterMinTxRate[32-10240](Def:32)
 * arg9 =  AturChanConfFastMaxTxRate[32-10240](Def:512)
 * arg10 = AturChanConfInterMaxTxRate[32-10240])(Def:512)
 * arg11 = AturChanConfMaxInterDelay[0-255](Def:8)
 * arg12 = ConfProfileLineType[noChannel(1),fastOnly(2),interleavedOnly(3),fastOrInterleaved(4),fast&Interleaved(5)](Def:3)
 */
static int MDM_DEVICE_CMD_DSLAM_ZTE_9xxx_ARGSN[] =
{
/* 0  */	0,
/* 1  */ 0,
/* 2  */ 0,
/* 3  */ 0,
/* 4  */ 0,
/* 5  */ 1,
/* 6  */ 0,
/* 7  */ 1,
/* 8  */ 0,
/* 9  */ 1,
/* 10 */ 1,
/* 11 */ 1,
/* 12 */ 0,
/* 13 */ 1,
/* 14 */ 1,
/* 15 */ 12,
/* 16 */ 1,
/* 17 */ 1,
/* 18 */ 1
};

/*!
 * Corresponding processing routines post exec.
 */
const mdm_device_dslam_zte_9xxx_post_exec_t 
MDM_DEVICE_CMD_DSLAM_ZTE_9xxx_PROCESS[] =
{
/* 0  */ dslam_zte_9xxx_nop,
/* 1  */ dslam_zte_9xxx_show_slots,
/* 2  */ dslam_zte_9xxx_show_running_config,
/* 3  */ dslam_zte_9xxx_get_service_profiles,
/* 4  */ dslam_zte_9xxx_soft_version,
/* 5  */ dslam_zte_9xxx_port_pvc,
/* 6  */ dslam_zte_9xxx_uptime,
/* 7  */ dslam_zte_9xxx_show_slot_ports,
/* 8  */ dslam_zte_9xxx_show_scb,
/* 9  */ dslam_zte_9xxx_show_slot,
/* 10 */ dslam_zte_9xxx_show_port,
/* 11 */ dslam_zte_9xxx_configure_timezone,
/* 12 */ dslam_zte_9xxx_get_ntp,
/* 13 */ dslam_zte_9xxx_get_service_profile,
/* 14 */ dslam_zte_9xxx_create_service_profile,
/* 15 */ dslam_zte_9xxx_configure_service_profile,
/* 16 */ dslam_zte_9xxx_delete_service_profile,
/* 17 */ dslam_zte_9xxx_get_port_mac,
/* 18 */ dslam_zte_9xxx_get_port_mac
};

/*!
 * Corresponding command strings from #mdm_device_cmd_dslam_zte_9xxx_t
 */
const char *MDM_DEVICE_CMD_DSLAM_ZTE_9xxx_STR[] =
{
/* 0  */	"",
/* 1  */ "show card",
/* 2  */ "show running-config",
/* 3  */ "show adsl profile",
/* 4  */ "show version",
/* 5  */ "show atm pvc interface %%ARG%%",
/* 6  */ "show system",
/* 7  */ "show adsl status slot %%ARG%%",
/* 8  */ "show card selfSCB",
/* 9  */ "show card slot %%ARG%%",
/* 10 */ "show adsl status interface %%ARG%%",
/* 11 */ "configure ip ntp time-zone %%ARG%%",
/* 12 */ "show ip ntp",
/* 13 */ "show adsl profile %%ARG%%",
/* 14 */ "configure adsl-profile %%ARG%%\r\ny",
/* 15 */ "configure adsl-profile %%ARG%%\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n%%ARG%%\r\n%%ARG%%\r\n%%ARG%%\r\n%%ARG%%\r\n%%ARG%%\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n%%ARG%%\r\n%%ARG%%\r\n%%ARG%%\r\n%%ARG%%\r\n%%ARG%%\r\n\r\n%%ARG%%\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n",
/* 16 */ "configure no adsl-profile %%ARG%%",
/* 17 */ "sh fdb slot %%ARG%%",
/* 18 */ "sh fdb %%ARG%%",
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
 * login to DSLAM. Will also fill
 * \param d Device descriptor.
 * \param ct One of #mdm_connection_type_t
 * \param status Where to store the result.
 */
void
mdm_device_dslam_zte_9xxx_open(
	mdm_device_descriptor_t *d, mdm_connection_type_t ct,
	mdm_operation_result_t *status
)
{
	mdm_driver_telnet_options_t telnet_options;
	char *host_port_token = NULL;
	mdm_device_dslam_zte_9xxx_options_t *options;
	mdm_connection_options_t *conn_options;

	char buffer[4096];
	int bufflen;
	mdm_device_dslam_zte_9xxx_data_t *data =
		(mdm_device_dslam_zte_9xxx_data_t *)d->data;

	options = (mdm_device_dslam_zte_9xxx_options_t *)d->options;
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
	} while(strstr(buffer, "Login:") == NULL);
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
		if(strstr(buffer, "Login:") != NULL)
		{
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "Login incorrect!");
			return;
		}
	} while(strstr(buffer, "$ ") == NULL);
	/* It doesn't matter the prompt, but we need the length so we can strip
	 * the command lines later on. 
	 */
	host_port_token = strrchr(buffer, '\n');
	if(host_port_token < buffer)
	{
		data->promptlen = strlen(buffer);
	} else {
		data->promptlen = strlen(buffer) - (strrchr(buffer, '\n') - buffer);
	}
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Login successfull: promptlen: %d", data->promptlen);
#endif		
	return;
}

/*!
 * Will try to prepare a command for exec.
 * \param d Device descriptor.
 * \param c Command, one of #mdm_device_cmd_dslam_zte_9xxx_t
 * \param args Command arguments, normally a csv string.
 * \param status Where to store the result.
 */
void
mdm_device_dslam_zte_9xxx_prepare_exec(
	mdm_device_descriptor_t *d, mdm_device_cmd_t c,
	const char *args, mdm_operation_result_t *status
)
{
	const char *cmd = MDM_DEVICE_CMD_DSLAM_ZTE_9xxx_STR[c];
	int cmdlen = strlen(cmd);
	int cmdargsn = MDM_DEVICE_CMD_DSLAM_ZTE_9xxx_ARGSN[c];
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
		(strlen(DSLAM_ZTE_9xxx_CMD_ARG) * cmdargsn) -
		argstoken
	;
	precmdlen = 
		cmdlen + argslen -
		(strlen(DSLAM_ZTE_9xxx_CMD_ARG) * cmdargsn) -
		argstoken
	;
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG(
		"clen: %d, alen: %d, ctoklen: %d, atoklen: %d",
		cmdlen, argslen, strlen(DSLAM_ZTE_9xxx_CMD_ARG) * cmdargsn, argstoken
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
		cmdp2 = strstr(cmdp1, DSLAM_ZTE_9xxx_CMD_ARG);

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
		cmdp1 = cmdp2 + strlen(DSLAM_ZTE_9xxx_CMD_ARG);
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
mdm_device_dslam_zte_9xxx_close(
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
mdm_device_dslam_zte_9xxx_check_error(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	const char *error;

	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif

	if((error = strstr(d->exec_buffer, "ERROR: ")) != NULL) {
		status->status = MDM_OP_ERROR;
		snprintf(
			status->status_message, strrchr(error, '\n') - error, "%s", error
		);
	} else if((error = strstr(d->exec_buffer, "Error: ")) != NULL) {
		status->status = MDM_OP_ERROR;
		snprintf(
			status->status_message, strrchr(error, '\n') - error, "%s", error
		);
	} else if(strstr(d->exec_buffer, "Error: Bad command") != NULL) {
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Command syntax error?");
	}

	/* Done. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
}

/*!
 * Will exec a command. It will strip the prompt from
 * the final line and the issued command from the first n lines. See 
 * #mdm_device_dslam_zte_9xxx_data_t.promptlen
 * \param d Device descriptor.
 * \param status Where to store the result error code.
 */
void
mdm_device_dslam_zte_9xxx_exec(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	char tempbuffer[256];
	int tempbufferlen;
	char *lastlinebeforeprompt = NULL;
	char *foundcmd = NULL;
	char *prompt = NULL;
	/* We have to skip the issued command. */
	float linestoskip = ceilf((float)(d->exec_buffer_cmd_len) / (float)(80));
	char pagebuffer[64];
	int pagebufferlen = 0;
	int i = 0;
	
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
		if(strstr(tempbuffer, "(Q to quit)") != NULL)
		{
#if MDM_DEBUG_MESSAGES > 0
			MDM_LOGDEBUG("Paging");
#endif
			tempbufferlen -= 38;
			sleep(1);
			d->connection.send(&d->connection.descriptor, "", 0, status);
			if(status->status == MDM_OP_ERROR)
			{
				return;
			}
			for(i = 0; i < 39; i++)
			{
				pagebufferlen = 1;
				mdm_connection_recv(
					&d->connection, pagebuffer, &pagebufferlen, status
				);
				if(status->status == MDM_OP_ERROR)
				{
					return;
				}
			}
		}
		d->exec_buffer_len += tempbufferlen;
		strncat(d->exec_buffer, tempbuffer, tempbufferlen);
		prompt = strstr(d->exec_buffer, "$ ");
		if(prompt != NULL)
		{
			lastlinebeforeprompt = strrchr(d->exec_buffer, 13);
			*lastlinebeforeprompt = 0;
			lastlinebeforeprompt = strrchr(d->exec_buffer, 13);
			if(lastlinebeforeprompt != NULL)
				*lastlinebeforeprompt = 0;
			lastlinebeforeprompt = strrchr(d->exec_buffer, 13);
			if(lastlinebeforeprompt != NULL)
				*lastlinebeforeprompt = 0;
			break;
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
 * Called by #mdm_device_new to initialize a device dslam zte 9xxx.
 * \param d Device.
 * \param status Where to store the result.
 */
void
mdm_device_dslam_zte_9xxx_new(
	mdm_device_t *d, mdm_operation_result_t *status
)
{
	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif

	d->open = mdm_device_dslam_zte_9xxx_open;
	d->exec = mdm_device_dslam_zte_9xxx_exec;
	d->prepare_exec = mdm_device_dslam_zte_9xxx_prepare_exec;
	d->post_exec = mdm_device_dslam_zte_9xxx_post_exec;
	d->close = mdm_device_dslam_zte_9xxx_close;
	d->check_error = mdm_device_dslam_zte_9xxx_check_error;

	/* Done. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
	return;
}

/*!
 * Called by #mdm_device_free to deinitialize a device dslam zte 9xxx.
 * \param d Device.
 */
void
mdm_device_dslam_zte_9xxx_free(mdm_device_t *d)
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
 * This will process the output from #mdm_device_dslam_zte_9xxx_exec.
 * Dont call this directly, use #mdm_device_exec.
 * \param d Device descriptor.
 * \param c Device command used to get response.
 * \param status Result of the operation.
 */
void
mdm_device_dslam_zte_9xxx_post_exec(
	mdm_device_descriptor_t * d, 
	mdm_device_cmd_t c, mdm_operation_result_t *status

)
{
	if(MDM_DEVICE_CMD_DSLAM_ZTE_9xxx_PROCESS[c] == NULL)
	{
		d->exec_buffer_post_len = d->exec_buffer_len;
		memcpy(d->exec_buffer_post, d->exec_buffer, d->exec_buffer_len);
	} else {
		MDM_DEVICE_CMD_DSLAM_ZTE_9xxx_PROCESS[c](d, status);
	}
}
/*******************************************************************************
 * CODE ENDS.
 ******************************************************************************/
