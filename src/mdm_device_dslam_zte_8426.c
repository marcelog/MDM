/*!
 * \file mdm_device_dslam_zte_8426.c DSLAM ZTE 8426 implementation.
 * \todo better timeout handling when using serial driver.
 * \todo add management of MDM_TELNET_DRIVER_ONLY_TELNET_DATA
 *
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#include	<stdio.h>
#include	<string.h>
#include	<math.h>
#include	<errno.h>
#include	<mdm_device_dslam_zte_8426.h>
#include	<mdm_parser_dslam_zte_8426.h>
#include	<mdm.h>

#define	DSLAM_ZTE_8426_CMD_ARG	"%%ARG%%" /*!< Use this string to identify
	where arguments go inside the command. */

/*!
 * Corresponding command names from #mdm_device_cmd_dslam_zte_8426_t
 */
const char *MDM_DEVICE_CMDNAME_DSLAM_ZTE_8426_STR[] =
{
/* 0  */	"Software Version",
/* 1  */	"Get Network information",
/* 2  */	"Get MAC",
/* 3  */ "Show Slots",
/* 4  */ "Get Routes",
/* 5  */ "Show Port ATM PVC",
/* 6  */ "NOP",
/* 7  */ "Show Service Profiles",
/* 8  */ "Show Slot Ports",
/* 9  */ "Reboot",
/* 10 */ "Create Service Profile",
/* 11 */ "Delete Service Profile",
/* 12 */ "Get Service Profile",
/* 13 */ "Get alarms",
/* 14 */ "Get Port admin/oper status",
/* 15 */ "Port admin enable",
/* 16 */ "Port admin disable",
/* 17 */ "Port current status",
/* 18 */ "Configure PVC",
/* 19 */ "Set port profile",
	NULL
};

/*!
 * Contains the number of arguments for each command.
 * For command 10:
 * arg1 = profile name.
 * arg2 = Atuc Channel Conf Fast Max Tx Rate(0...10000)---------[2048]
 * arg3 = Atuc Channel Conf InterLeave Max Tx Rate(0...10000)---[2048]
 * arg4 = Atuc Channel Conf InterLeave Max Delay(4 16 or 64)------[16]
 * arg5 = Atur Channel Conf Fast Max Tx Rate(0...1000)-----------[512]
 * arg6 = Atur Channel Conf InterLeave Max Tx Rate(0...1000)-----[512]
 * arg7 = Atur Channel Conf InterLeave Max Delay(4 16 or 64)------[16]
 */
static int MDM_DEVICE_CMD_DSLAM_ZTE_8426_ARGSN[] =
{
/* 0  */	0,
/* 1  */	0,
/* 2  */	0,
/* 3  */ 0,
/* 4  */ 0,
/* 5  */ 0,
/* 6  */ 0,
/* 7  */ 0,
/* 8  */ 0,
/* 9  */ 0,
/* 10 */ 7,
/* 11 */ 1,
/* 12 */ 1,
/* 13 */ 0,
/* 14 */ 0,
/* 15 */ 1,
/* 16 */ 1,
/* 17 */ 1,
/* 18 */ 3,
/* 19 */ 4
};

/*!
 * Corresponding processing routines post exec.
 */
const mdm_device_dslam_zte_8426_post_exec_t 
MDM_DEVICE_CMD_DSLAM_ZTE_8426_PROCESS[] =
{
/* 0  */ dslam_zte_8426_soft_version,
/* 1  */ dslam_zte_8426_show_internet,
/* 2  */ dslam_zte_8426_get_macs,
/* 3  */ dslam_zte_8426_show_slots,
/* 4  */ dslam_zte_8426_get_routes,
/* 5  */ dslam_zte_8426_get_all_pvc,
/* 6  */ dslam_zte_8426_nop,
/* 7  */ dslam_zte_8426_get_service_profiles,
/* 8  */ dslam_zte_8426_show_ports,
/* 9  */ dslam_zte_8426_reboot,
/* 10 */ dslam_zte_8426_create_service_profile,
/* 11 */ dslam_zte_8426_delete_service_profile,
/* 12 */ dslam_zte_8426_get_service_profile,
/* 13 */ dslam_zte_8426_get_alarms,
/* 14 */ dslam_zte_8426_get_adminoper_status,
/* 15 */ dslam_zte_8426_nop,
/* 16 */ dslam_zte_8426_nop,
/* 17 */ dslam_zte_8426_get_current_status,
/* 18 */ dslam_zte_8426_nop,
/* 19 */ dslam_zte_8426_nop
};

/*!
 * Corresponding command strings from #mdm_device_cmd_dslam_zte_8426_t
 */
const char *MDM_DEVICE_CMD_DSLAM_ZTE_8426_STR[] =
{
/* 0  */	"version",
/* 1  */	"net\nshowip\na\nhome",
/* 2  */	"net\nshowmac\na\nhome",
/* 3  */ "version",
/* 4  */	"net\nshowroute\nhome",
/* 5  */ "atm\nshowconf\n1\na\nhome",
/* 6  */ "",
/* 7  */ "dsl\nshowprf\n1\ne\nhome",
/* 8  */ "dsl\nshowstatus\na\nhome",
/* 9  */ "sys\nconfig\nreset\ny",
/* 10 */ "dsl\nconfig\naddprf\n1\n%%ARG%%\n%%ARG%%\n%%ARG%%\n%%ARG%%\n%%ARG%%\n%%ARG%%\n%%ARG%%\na\nexit\nhome",
/* 11 */ "dsl\nconfig\ndelprf\n1\n%%ARG%%\ny\nexit\nhome",
/* 12 */ "dsl\nshowprf\n1\n%%ARG%%\nhome",
/* 13 */ "dsl\nshowalarm\nhome",
/* 14 */ "dsl\nshowinfo\na\nhome",
/* 15 */ "dsl\nconfig\nenable\n%%ARG%%\nexit\nhome",
/* 16 */ "dsl\nconfig\ndisable\n%%ARG%%\nexit\nhome",
/* 17 */ "dsl\nconfig\ndetailshow\n1\ny\nshowstatus\n%%ARG%%\ndetailshow\n2\nexit\nhome",
/* 18 */ "atm\nconfig\nsetpvc\n1\n%%ARG%%\n%%ARG%%\n%%ARG%%\nexit\nhome", // VPI:VCI
/* 19 */ "dsl\nconfig\nlinecfg\n%%ARG%%\n%%ARG%%\n%%ARG%%\n%%ARG%%\nexit\nhome"
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
mdm_device_dslam_zte_8426_open(
	mdm_device_descriptor_t *d, mdm_connection_type_t ct,
	mdm_operation_result_t *status
)
{
	mdm_driver_telnet_options_t telnet_options;
	mdm_driver_serial_options_t serial_options;
	char *host_port_token = NULL;
	mdm_device_dslam_zte_8426_options_t *options;
	char buffer[4096];
	int bufflen;
	mdm_connection_options_t *connopts;
	
	options = (mdm_device_dslam_zte_8426_options_t *)d->options;
	switch(ct)
	{
		case MDM_CONNECTION_SERIAL:
			mdm_connection_new(&d->connection, MDM_CONNECTION_SERIAL, status);
			if(status->status == MDM_OP_ERROR)
				return;

			sprintf(serial_options.port, "%s", options->target);
			serial_options.speed = MDM_DRIVER_SERIAL_9600;
			serial_options.bits = MDM_DRIVER_SERIAL_8BITS;
			serial_options.parity = MDM_DRIVER_SERIAL_PNONE;
			serial_options.stop_bits = MDM_DRIVER_SERIAL_SBITS_1;
			serial_options.to_recv = options->to_recv;
			serial_options.echo_cancel = 0;
			connopts = (mdm_connection_options_t)&serial_options;
			break;
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
			telnet_options.echo_cancel = 0;
			telnet_options.to_recv = options->to_recv;
			telnet_options.to_connect = options->to_connect;
			telnet_options.send_eol = 1;
#if MDM_DEBUG_MESSAGES > 0
			MDM_LOGDEBUG(
				"Using %s:%s", telnet_options.host, telnet_options.service
			);
#endif
			connopts = (mdm_connection_options_t)&telnet_options;
			break;
		default:
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "Connection type not supported.");
			return;
	}
	/* Finally, open the connection to the host. */
	mdm_connection_open(&d->connection, connopts, status);
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
			MDM_LOGDEBUG("Waiting login: Got |%s|", buffer);
#endif
			if(strstr(buffer, "Sorry, session limit reached.") != NULL) {
				status->status = MDM_OP_ERROR;
				sprintf(status->status_message, "Session limit reached.");
				return;
			}
			if(strstr(buffer, ">>") != NULL) {
				mdm_connection_send(&d->connection, "home", 4, status);
				do
				{
					mdm_connection_recv(&d->connection, buffer, &bufflen, status);
					if(status->status == MDM_OP_ERROR)
					{
						return;
					}
				} while(bufflen > 0);
				mdm_connection_send(&d->connection, "logout", 6, status);
				mdm_connection_recv(&d->connection, buffer, &bufflen, status);
				if(status->status == MDM_OP_ERROR)
				{
					return;
				}
				do
				{
					mdm_connection_recv(&d->connection, buffer, &bufflen, status);
					if(status->status == MDM_OP_ERROR)
					{
						return;
					}
				} while(bufflen > 0);
				continue;
			} else if(strstr(buffer, "You are logout") != NULL) {
				do
				{
					mdm_connection_recv(&d->connection, buffer, &bufflen, status);
					if(status->status == MDM_OP_ERROR)
					{
						return;
					}
				} while(bufflen > 0);
				break;
			} else if(strstr(buffer, "word:") != NULL) {
				mdm_connection_send(&d->connection, "\n\n\n\n", 1, status);
				mdm_connection_recv(&d->connection, buffer, &bufflen, status);
				if(status->status == MDM_OP_ERROR)
				{
					return;
				}
				do
				{
					mdm_connection_recv(&d->connection, buffer, &bufflen, status);
					if(status->status == MDM_OP_ERROR)
					{
						return;
					}
				} while(bufflen > 0);
			} else if(strstr(buffer, "Sorry, login failed...") != NULL) {
				mdm_connection_send(&d->connection, "\n", 1, status);
				mdm_connection_recv(&d->connection, buffer, &bufflen, status);
				if(status->status == MDM_OP_ERROR)
				{
					return;
				}
				do
				{
					mdm_connection_recv(&d->connection, buffer, &bufflen, status);
					if(status->status == MDM_OP_ERROR)
					{
						return;
					}
				} while(bufflen > 0);
			}
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
			MDM_LOGDEBUG("Waiting password Got |%s|", buffer);
#endif
			if(strstr(buffer, "No such user") != NULL)
			{
				status->status = MDM_OP_ERROR;
				sprintf(status->status_message, "Login incorrect!");
				return;
			}
		}
	} while(strstr(buffer, "word:") == NULL);
	mdm_connection_send(
		&d->connection, options->password, strlen(options->password), status
	);
	if(status->status == MDM_OP_ERROR)
	{
		return;
	}

	/* Wait for login result. */
	memset(buffer, 0, bufflen);
	do
	{
		bufflen = sizeof(buffer) - strlen(buffer);
		mdm_connection_recv(
			&d->connection, buffer + strlen(buffer), &bufflen, status
		);
		if(status->status == MDM_OP_ERROR)
		{
			return;
		}
		if(bufflen > 0)
		{
#if MDM_DEBUG_MESSAGES > 0
			MDM_LOGDEBUG("Got |%s|", buffer);
#endif
			if(strstr(buffer, "Sorry, login failed...") != NULL) {
				status->status = MDM_OP_ERROR;
				sprintf(status->status_message, "Login incorrect!");
				return;
			} else if(strstr(buffer, "Invalid Password...") != NULL) {
				status->status = MDM_OP_ERROR;
				sprintf(status->status_message, "Login incorrect!");
				return;
			}
		}
	} while(strstr(buffer, ">>") == NULL);
	return;
}

/*!
 * Will try to prepare a command for exec.
 * \param d Device descriptor.
 * \param c Command, one of #mdm_device_cmd_dslam_zte_8426_t
 * \param args Command arguments, normally a csv string.
 * \param status Where to store the result.
 */
void
mdm_device_dslam_zte_8426_prepare_exec(
	mdm_device_descriptor_t *d, mdm_device_cmd_t c,
	const char *args, mdm_operation_result_t *status
)
{
	const char *cmd = MDM_DEVICE_CMD_DSLAM_ZTE_8426_STR[c];
	int cmdlen = strlen(cmd);
	int cmdargsn = MDM_DEVICE_CMD_DSLAM_ZTE_8426_ARGSN[c];
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
		(strlen(DSLAM_ZTE_8426_CMD_ARG) * cmdargsn) -
		argstoken
	;
	precmdlen = 
		cmdlen + argslen -
		(strlen(DSLAM_ZTE_8426_CMD_ARG) * cmdargsn) -
		argstoken
	;
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG(
		"clen: %d, alen: %d, ctoklen: %d, atoklen: %d",
		cmdlen, argslen, strlen(DSLAM_ZTE_8426_CMD_ARG) * cmdargsn, argstoken
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
		cmdp2 = strstr(cmdp1, DSLAM_ZTE_8426_CMD_ARG);

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
		cmdp1 = cmdp2 + strlen(DSLAM_ZTE_8426_CMD_ARG);
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
mdm_device_dslam_zte_8426_close(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
/*
	char buffer[32];
	int bufflen = sizeof(buffer);
*/
	if(d->connection.type == MDM_CONNECTION_SERIAL)
	{
		mdm_connection_send(&d->connection, "logout", 6, status);
/*
		do
		{
			mdm_connection_recv(&d->connection, buffer, &bufflen, status);
		} while(bufflen > 0);
*/
	}

	return;
}

/*!
 * Will search for known error strings in response buffer.
 * \param d Device descriptor.
 * \param status Where to store the result error code.
 * \return void
 */
void
mdm_device_dslam_zte_8426_check_error(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	const char *error;

	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif

/*	if(strstr(*buffer, "Profile not exist!") != NULL) {
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Invalid Profile");
		return;
	} else*/ if(strstr(d->exec_buffer, "Argument error!") != NULL) {
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error in arguments");
	} else if(strstr(d->exec_buffer, "This Profile already exist!") != NULL) {
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Profile already exists");
	} else if(strstr(d->exec_buffer, "Bad commond") != NULL) {
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Command syntax error?");
	} else if((error = strstr(d->exec_buffer, "ERROR: ")) != NULL) {
		status->status = MDM_OP_ERROR;
		snprintf(
			status->status_message, strrchr(error, '\n') - error, "%s", error
		);
	}

	/* Done. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
	return;
}

/*!
 * Will exec a command. It will strip the prompt from
 * the final line and the issued command from the first n lines. See 
 * #mdm_device_dslam_zte_8426_data_t.promptlen
 * \param d Device descriptor.
 * \param status Where to store the result error code.
 */
void
mdm_device_dslam_zte_8426_exec(
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

	/* Done. */
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
		if(strstr(d->exec_buffer, "System will be restart.....") != NULL) {
			return;
		} else {
			d->exec_buffer_len += tempbufferlen;
			strncat(d->exec_buffer, tempbuffer, tempbufferlen);
			prompt = strstr(d->exec_buffer, "HOME>>");
			if(prompt != NULL)
			{
				lastlinebeforeprompt = strrchr(d->exec_buffer, '\n');
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
		linestoskip--;
	} while(linestoskip > 0);
	if(foundcmd != NULL)
	{
#if MDM_DEBUG_MESSAGES > 0
		MDM_LOGDEBUG("Stripping cmd.");
#endif
		d->exec_buffer_len = strlen(foundcmd);
		snprintf(d->exec_buffer, d->exec_buffer_len, "%s", foundcmd);
	}

	/* Done. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
}

/*!
 * Called by #mdm_device_new to initialize a device dslam zte 8426.
 * \param d Device.
 * \param status Where to store the result.
 */
void
mdm_device_dslam_zte_8426_new(
	mdm_device_t *d, mdm_operation_result_t *status
)
{
	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif

	d->open = mdm_device_dslam_zte_8426_open;
	d->exec = mdm_device_dslam_zte_8426_exec;
	d->prepare_exec = mdm_device_dslam_zte_8426_prepare_exec;
	d->post_exec = mdm_device_dslam_zte_8426_post_exec;
	d->close = mdm_device_dslam_zte_8426_close;
	d->check_error = mdm_device_dslam_zte_8426_check_error;

	/* Done. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
	return;
}

/*!
 * Called by #mdm_device_free to deinitialize a device dslam zte 8426.
 * \param d Device.
 */
void
mdm_device_dslam_zte_8426_free(mdm_device_t *d)
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
 * This will process the output from #mdm_device_dslam_zte_8426_exec.
 * Dont call this directly, use #mdm_device_exec.
 * \param d Device descriptor.
 * \param c Device command used to get response.
 * \param status Result of the operation.
 */
void
mdm_device_dslam_zte_8426_post_exec(
	mdm_device_descriptor_t * d, 
	mdm_device_cmd_t c, mdm_operation_result_t *status

)
{
	if(MDM_DEVICE_CMD_DSLAM_ZTE_8426_PROCESS[c] == NULL)
	{
		d->exec_buffer_post_len = d->exec_buffer_len;
		memcpy(d->exec_buffer_post, d->exec_buffer, d->exec_buffer_len);
	} else {
		MDM_DEVICE_CMD_DSLAM_ZTE_8426_PROCESS[c](d, status);
	}
}
/*******************************************************************************
 * CODE ENDS.
 ******************************************************************************/
