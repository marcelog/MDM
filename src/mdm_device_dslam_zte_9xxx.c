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
/* 19 */ "Get Routes",
/* 20 */ "Get full port information",
/* 21 */ "Get physical port information",
/* 22 */ "Get Network information",
/* 23 */ "Get channel information",
/* 24 */ "Get fans information",
/* 25 */ "Configure NTP Server",
/* 26 */ "Enable NTP",
/* 27 */ "Disable NTP",
/* 28 */ "Copy configuration to startup",
/* 29 */ "Configure card type",
/* 30 */ "Reset card",
/* 31 */ "Switch SCB",
/* 32 */ "Reboot Slave SCB",
/* 33 */ "Reboot",
/* 34 */ "Add VLAN",
/* 35 */ "Remove VLAN",
/* 36 */ "Port uplink configure tag vlan",
/* 37 */ "Port uplink configure untag vlan",
/* 38 */ "Port admin down",
/* 39 */ "Port admin up",
/* 40 */ "Configure port profile",
/* 41 */ "Configure port pvc",
/* 42 */ "Enable PVC",
/* 43 */ "Disable PVC",
/* 44 */ "Configure PVC vlan",
/* 45 */ "Backup to FTP",
/* 46 */ "Download from FTP",
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
/* 18 */ 1,
/* 19 */ 0,
/* 20 */ 1,
/* 21 */ 1,
/* 22 */ 0,
/* 23 */ 1,
/* 24 */ 0,
/* 25 */ 1,
/* 26 */ 0,
/* 27 */ 0,
/* 28 */ 0,
/* 29 */ 2,
/* 30 */ 1,
/* 31 */ 0,
/* 32 */ 0,
/* 33 */ 0,
/* 34 */ 1,
/* 35 */ 1,
/* 36 */ 2,
/* 37 */ 2,
/* 38 */ 1,
/* 39 */ 1,
/* 40 */ 2,
/* 41 */ 4,
/* 42 */ 2,
/* 43 */ 2,
/* 44 */ 3,
/* 45 */ 2,
/* 46 */ 2
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
/* 18 */ dslam_zte_9xxx_get_port_mac,
/* 19 */ dslam_zte_9xxx_get_routes,
/* 20 */ dslam_zte_9xxx_get_port_full,
/* 21 */ dslam_zte_9xxx_get_physical_port,
/* 22 */ dslam_zte_9xxx_get_network_info,
/* 23 */ dslam_zte_9xxx_get_channel_info,
/* 24 */ dslam_zte_9xxx_get_fans_info,
/* 25 */ dslam_zte_9xxx_nop,
/* 26 */ dslam_zte_9xxx_nop,
/* 27 */ dslam_zte_9xxx_nop,
/* 28 */ dslam_zte_9xxx_nop,
/* 29 */ dslam_zte_9xxx_nop,
/* 30 */ dslam_zte_9xxx_nop,
/* 31 */ dslam_zte_9xxx_nop,
/* 32 */ dslam_zte_9xxx_nop,
/* 33 */ dslam_zte_9xxx_nop,
/* 34 */ dslam_zte_9xxx_nop,
/* 35 */ dslam_zte_9xxx_nop,
/* 36 */ dslam_zte_9xxx_nop,
/* 37 */ dslam_zte_9xxx_nop,
/* 38 */ dslam_zte_9xxx_nop,
/* 39 */ dslam_zte_9xxx_nop,
/* 40 */ dslam_zte_9xxx_nop,
/* 41 */ dslam_zte_9xxx_nop,
/* 42 */ dslam_zte_9xxx_nop,
/* 43 */ dslam_zte_9xxx_nop,
/* 44 */ dslam_zte_9xxx_nop,
/* 45 */ dslam_zte_9xxx_nop,
/* 46 */ dslam_zte_9xxx_nop
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
/* 19 */ "show ip route",
/* 20 */ "show interface %%ARG%% full",
/* 21 */ "show adsl physical %%ARG%%",
/* 22 */ "show ip subnet",
/* 23 */ "show adsl channel %%ARG%%",
/* 24 */ "show fans",
/* 25 */ "configure ip ntp server %%ARG%%",
/* 26 */ "configure ip ntp enable",
/* 27 */ "configure ip ntp disable",
/* 28 */ "copy running-config startup-config",
/* 29 */ "configure card slot %%ARG%% cardtype %%ARG%%",
/* 30 */ "reset card %%ARG%%\r\ny",
/* 31 */ "switch scb\r\ny",
/* 32 */ "reboot slaveSCB\r\ny",
/* 33 */ "reboot\r\ny",
/* 34 */ "configure add-vlan %%ARG%%",
/* 35 */ "configure no add-vlan %%ARG%%",
/* 36 */ "configure vlan %%ARG%% %%ARG%% tag", // vlan port
/* 37 */ "configure vlan %%ARG%% %%ARG%% untag", // vlan port
/* 38 */ "configure interface dsl-mpvc %%ARG%%\r\nshutdown\r\nexit",
/* 39 */ "configure interface dsl-mpvc %%ARG%%\r\nno shutdown\r\nexit",
/* 40 */ "configure interface dsl-mpvc %%ARG%%\r\nadsl profile %%ARG%%\r\nexit",
/* 41 */ "configure interface dsl-mpvc %%ARG%%\r\natm pvc %%ARG%%:%%ARG%% pvc%%ARG%%\r\nexit",
/* 42 */ "configure interface dsl-mpvc %%ARG%%\r\natm status enable PVC%%ARG%%\r\nexit",
/* 43 */ "configure interface dsl-mpvc %%ARG%%\r\natm status disable PVC%%ARG%%\r\nexit",
/* 44 */ "configure interface dsl-mpvc %%ARG%%\r\npvid %%ARG%% PVC%%ARG%%\r\nexit",
/* 45 */ "copy startup-config network\r\n%%ARG%%\r\n%%ARG%%", // ip, filename
/* 46 */ "copy network startup-config\r\n%%ARG%%\r\n%%ARG%%",
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
			if(strstr(buffer, "Error: ") != NULL) {
				status->status = MDM_OP_ERROR;
				sprintf(status->status_message, "%s", buffer);
				return;
		}
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
        snprintf(data->prompt, data->promptlen, "%s", buffer);
	} else {
		data->promptlen = strlen(buffer) - (strrchr(buffer, '\n') - buffer);
        snprintf(data->prompt, data->promptlen, "%s", strrchr(buffer, '\n') + 1);
	}
#if MDM_DEBUG_MESSAGES > 0
    MDM_LOGDEBUG("Login successfull: promptlen: %s %d", data->prompt, data->promptlen);
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
    } else if((error = strstr(d->exec_buffer, "unsuccessfully")) != NULL) {
        status->status = MDM_OP_ERROR;
        snprintf(
            status->status_message, d->exec_buffer_len, "%s", d->exec_buffer
        );
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
    mdm_device_dslam_zte_9xxx_data_t *data =
        (mdm_device_dslam_zte_9xxx_data_t *)d->data;
	/* We have to skip the issued command. */
	float linestoskip = ceilf((float)(d->exec_buffer_cmd_len) / (float)(80));
	char pagebuffer[64];
	int pagebufferlen = 0;
	char *tmp1;
	char *pagingstr = "Press any key to continue (Q to quit)";
	int paginglen = strlen(pagingstr) + 1;
	
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
		strncat(d->exec_buffer, tempbuffer, tempbufferlen);
		d->exec_buffer_len += tempbufferlen;
        prompt = strstr(d->exec_buffer, data->prompt);
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
		tmp1 = strstr(d->exec_buffer, pagingstr);
		if(tmp1 != NULL)
		{
#if MDM_DEBUG_MESSAGES > 0
			MDM_LOGDEBUG("Paging");
#endif
			d->exec_buffer_len -= paginglen;
			sleep(1);
			d->connection.send(&d->connection.descriptor, "", 0, status);
			if(status->status == MDM_OP_ERROR)
			{
				return;
			}
			pagebufferlen = 1;//paginglen;
			do
			{
				MDM_LOGDEBUG("Reading paging");
				mdm_connection_recv(
					&d->connection, pagebuffer, &pagebufferlen, status
				);
				if(status->status == MDM_OP_ERROR)
				{
					return;
				}
				MDM_LOGDEBUG("Reading paging %c", *pagebuffer);
				if (pagebufferlen == 0) {
					break;
				}
			} while(*pagebuffer == 32 || *pagebuffer == 13 || *pagebuffer == 10);
			d->exec_buffer[d->exec_buffer_len] = *pagebuffer;
			d->exec_buffer_len++;
			d->exec_buffer[d->exec_buffer_len] = 0;
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
		d->exec_buffer_len = strlen(foundcmd) + 1;
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
