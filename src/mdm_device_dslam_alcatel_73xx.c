/*!
 * \file mdm_device_dslam_alcatel_73xx.c DSLAM ALCATEL 73xx implementation.
 * \bug What about range commands like [1...48] ?
 * \bug What about alarms when reading outputs from commands? the workaround is
 * to use some commands to setup the terminal session in alcatel, to disable
 * alarm reporting ;) this is done in #mdm_device_dslam_alcatel_73xx_open
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#include	<stdio.h>
#include	<string.h>
#include	<math.h>
#include	<stdarg.h>
#include	<errno.h>
#include	<mdm_device_dslam_alcatel_73xx.h>
#include	<mdm_parser_dslam_alcatel_73xx.h>
#include	<mdm.h>

#define	DSLAM_ALCATEL_73xx_CMD_ARG	"%%ARG%%" /*!< Use this string to identify
	where arguments go inside the command. */

/*!
 * Corresponding command names from #mdm_device_cmd_dslam_alcatel_73xx_t
 */
const char *MDM_DEVICE_CMDNAME_DSLAM_ALCATEL_73xx_STR[] =
{
/* 0  */	"Uptime",
/* 1  */	"Software Version",
/* 2  */	"Show Slots",
/* 3  */	"Show Slot Port",
/* 4  */	"Show Spectrum Profile Detail",
/* 5  */	"Show Service Profile",
/* 6  */	"Show All Ports",
/* 7  */	"Get Network information",
/* 8  */	"Get Routes",
/* 9  */	"Show Port Description",
/* 10 */	"Show Port Profile",
/* 11 */	"Show Port Data Near End",
/* 12 */	"Show Port Data Far End",
/* 13 */	"Show Port ATM PVC",
/* 14 */	"Info VLAN",
/* 15 */	"Show Backup Status",
/* 16 */	"Show Download Backup DB2",
/* 17 */	"Show Port ATM PVC2", /*!< Use like 1/1/1 */
/* 18 */	"Load Port Detail 2", /*!< Use like 1/1/1 */
/* 19 */	"Load Port Detail 1",
/* 20 */	"Syslog Get 1",
/* 21 */	"Syslog Get 2",
/* 22 */	"ATM Show PVC Traffic 1",
/* 23 */	"ATM Show PVC Traffic 2",
/* 24 */	"ATM Show PVC Traffic 3",
/* 25 */	"Alarm Get 2",
/* 26 */	"Alarm Get",
/* 27 */	"Get Ports Traffic",
/* 28 */	"Get eth Ports",
/* 29 */	"Alarm Get Count",
/* 30 */ "NOP",
/* 31 */ "Configure timezone",
/* 32 */ "Get NTP",
/* 33 */ "Create Service Profile",
/* 34 */ "Delete Service Profile",
/* 35 */ "Configure Service Profile ra-mode-up",
/* 36 */ "Configure Service Profile ra-mode-down",
/* 37 */ "Configure Service Profile plan-bitrate-up",
/* 38 */ "Configure Service Profile plan-bitrate-down",
/* 39 */ "Configure Service Profile max-delay-up",
/* 40 */ "Configure Service Profile max-delay-down",
/* 41 */ "Configure Service Profile max-bitrate-up",
/* 42 */ "Configure Service Profile max-bitrate-down",
/* 43 */ "Configure Service Profile min-bitrate-up",
/* 44 */ "Configure Service Profile min-bitrate-down",
/* 45 */ "Reboot hot",
/* 46 */ "Reboot all",
/* 47 */ "Reboot shub",
/* 48 */ "Port enable",
/* 49 */ "Port disable",
/* 50 */ "Configure System Security Filetransfer",
/* 51 */ "Configure System Security Filetransfer host/user/password",
/* 52 */ "Upload active database",
/* 53 */ "Download database",
/* 54 */ "Alarm clear",
	NULL
};

/*!
 * Contains the number of arguments for each command.
 * For command 31:
 * arg1 = timezone in minutes!
 */
static int MDM_DEVICE_CMD_DSLAM_ALCATEL_73xx_ARGSN[] =
{
/* 0  */	0,
/* 1  */	0,
/* 2  */	0,
/* 3  */	1,
/* 4  */	0,
/* 5  */	0,
/* 6  */	0,
/* 7  */	0,
/* 8  */	0,
/* 9  */	1,
/* 10 */	1,
/* 11 */	1,
/* 12 */	1,
/* 13 */	0,
/* 14 */	1,
/* 15 */	0,
/* 16 */	0,
/* 17 */	1,
/* 18 */	1,
/* 19 */	0,
/* 20 */	0,
/* 21 */	0,
/* 22 */	3,
/* 23 */	2,
/* 24 */	1,
/* 25 */	1,
/* 26 */	1,
/* 27 */	1,
/* 28 */	0,
/* 29 */	1,
/* 30 */ 0,
/* 31 */ 1,
/* 32 */ 0,
/* 33 */ 2,
/* 34 */ 1,
/* 35 */ 2,
/* 36 */ 2,
/* 37 */ 2,
/* 38 */ 2,
/* 39 */ 2,
/* 40 */ 2,
/* 41 */ 2,
/* 42 */ 2,
/* 43 */ 2,
/* 44 */ 2,
/* 45 */ 0,
/* 46 */ 0,
/* 47 */ 0,
/* 48 */ 1,
/* 49 */ 1,
/* 50 */ 1,
/* 51 */ 3,
/* 52 */ 2,
/* 53 */ 2,
/* 54 */ 1
};

/*!
 * Corresponding processing routines post exec.
 */
static const mdm_device_dslam_alcatel_73xx_post_exec_t
MDM_DEVICE_CMD_DSLAM_ALCATEL_73xx_PROCESS[] = {
/* 0  */ dslam_alcatel_73xx_uptime,
/* 1  */ dslam_alcatel_73xx_soft_version,
/* 2  */ dslam_alcatel_73xx_show_slots,
/* 3  */ dslam_alcatel_73xx_show_slot_port,
/* 4  */ dslam_alcatel_73xx_show_spectrum_profiles,
/* 5  */ dslam_alcatel_73xx_show_service_profiles,
/* 6  */ dslam_alcatel_73xx_show_ports,
/* 7  */ dslam_alcatel_73xx_show_internet,
/* 8  */ dslam_alcatel_73xx_get_routes,
/* 9  */ dslam_alcatel_73xx_get_port_description,
/* 10 */ dslam_alcatel_73xx_port_profile,
/* 11 */ dslam_alcatel_73xx_port_data_near_end,
/* 12 */ dslam_alcatel_73xx_port_data_far_end,
/* 13 */ dslam_alcatel_73xx_get_all_pvc,
/* 14 */ dslam_alcatel_73xx_get_bridge,
/* 15 */ dslam_alcatel_73xx_get_backup_status,
/* 16 */ dslam_alcatel_73xx_get_databases,
/* 17 */ dslam_alcatel_73xx_get_slot_vlans,
/* 18 */ dslam_alcatel_73xx_load_ports,
/* 19 */ dslam_alcatel_73xx_load_ports_all,
/* 20 */ NULL,
/* 21 */ NULL,
/* 22 */ NULL,
/* 23 */ NULL,
/* 24 */ NULL,
/* 25 */ NULL,
/* 26 */ NULL,
/* 27 */ dslam_alcatel_73xx_get_eth_statistics,
/* 28 */ dslam_alcatel_73xx_get_eth_list,
/* 29 */ dslam_alcatel_73xx_get_alarm_count,
/* 30 */ dslam_alcatel_73xx_nop,
/* 31 */ dslam_alcatel_73xx_configure_timezone,
/* 32 */ dslam_alcatel_73xx_get_ntp,
/* 33 */ dslam_alcatel_73xx_create_service_profile,
/* 34 */ dslam_alcatel_73xx_delete_service_profile,
/* 35 */ dslam_alcatel_73xx_ramodeup_service_profile,
/* 36 */ dslam_alcatel_73xx_ramodedown_service_profile,
/* 37 */ dslam_alcatel_73xx_pbitrateup_service_profile,
/* 38 */ dslam_alcatel_73xx_pbitratedown_service_profile,
/* 39 */ dslam_alcatel_73xx_mdelayup_service_profile,
/* 40 */ dslam_alcatel_73xx_mdelaydown_service_profile,
/* 41 */ dslam_alcatel_73xx_maxbitrateup_service_profile,
/* 42 */ dslam_alcatel_73xx_maxbitratedown_service_profile,
/* 43 */ dslam_alcatel_73xx_minbitrateup_service_profile,
/* 44 */ dslam_alcatel_73xx_minbitratedown_service_profile,
/* 45 */ dslam_alcatel_73xx_nop,
/* 46 */ dslam_alcatel_73xx_nop,
/* 47 */ dslam_alcatel_73xx_nop,
/* 48 */ dslam_alcatel_73xx_nop,
/* 49 */ dslam_alcatel_73xx_nop,
/* 50 */ dslam_alcatel_73xx_nop,
/* 51 */ dslam_alcatel_73xx_nop,
/* 52 */ dslam_alcatel_73xx_nop,
/* 53 */ dslam_alcatel_73xx_nop,
/* 54 */ dslam_alcatel_73xx_nop
};

/*!
 * Corresponding command strings from #mdm_device_cmd_dslam_alcatel_73xx_t
 */
const char *MDM_DEVICE_CMD_DSLAM_ALCATEL_73xx_STR[] =
{
/* 0  */	"show transport system-parameters",
/* 1  */	"show software-mngt oswp detail",
/* 1  */	/*"show software-mngt oswp detail | match exact:\"act-status : active                              commit-status : committed\" before | match exact:name",*/
/* 2  */	"show equipment slot",
/* 2  */	/*"show equipment slot | match skip exact:\"---\" | match skip exact:\"slot count\" | match skip exact:\"nt-\" | match skip exact:\"not-installed\"",*/
/* 3  */	"show xdsl operational-data line %%ARG%% detail",
/* 4  */	"info configure xdsl spectrum-profile detail flat |  match exact:name",
/* 5  */	"info configure xdsl service-profile flat | match skip exact:active",
/* 6  */	"show xdsl operational-data line",
/* 7  */	"info configure system management",
/* 8  */	"info configure system management",
/* 9  */	"info configure interface port xdsl-line:%%ARG%%",
/* 10 */	"info configure xdsl line %%ARG%%",
/* 11 */	"show xdsl operational-data near-end line %%ARG%% detail",
/* 12 */	"show xdsl operational-data far-end line %%ARG%% detail",
/* 13 */	"info configure atm",
/* 14 */	"info configure bridge port %%ARG%%", /*!< Use like 1/1/1/1:1:33 */
/* 15 */	"show software-mngt upload-download",
/* 16 */	"show software-mngt database",
/* 17 */	"info configure interface flat | match exact:\"vlan-port:%%ARG%%/\"", /*!< Use like 1/1/1 */
/* 18 */	"info configure interface flat | match exact:\"xdsl-line:%%ARG%%/\"", /*!< Use like 1/1/1 */
/* 19 */	"info configure xdsl line flat",
/* 20 */	"info configure system syslog destination mdm | match exact:mdm",
/* 21 */	"info configure system syslog flat | match exact:\"route mdm\"",
/* 22 */	"show vlan port-curr-day-stats %%ARG%%:%%ARG%%:%%ARG%% detail | match exact:byte",
/* 23 */	"show vlan port-curr-day-stats detail | match before exact:%%ARG%%/ | match after exact:%%ARG%%/",
/* 24 */	"show equipment slot | match after exact:\"%%ARG%% \"",
/* 25 */	"show alarm log %%ARG%% detail | match skip exact:== | match skip exact:--",
/* 26 */	"show alarm delta-log %%ARG%% | match skip exact:----- | match skip exact:===== | match skip exact:index | match skip exact:count",
/* 27 */	"show interface shub statistics %%ARG%% detail",
/* 28 */	"show interface shub port",
/* 29 */	"show alarm delta-log %%ARG%% | match exact count",
/* 30 */ "show transport system-parameters",
/* 31 */ "configure system sntp timezone-offset %%ARG%%",
/* 32 */ "info configure system sntp",
/* 33 */ "configure xdsl service-profile %%ARG%% name %%ARG%%",
/* 34 */ "configure xdsl no service-profile %%ARG%%",
/* 35 */ "configure xdsl service-profile %%ARG%% ra-mode-up %%ARG%%",
/* 36 */ "configure xdsl service-profile %%ARG%% ra-mode-down %%ARG%%",
/* 37 */ "configure xdsl service-profile %%ARG%% plan-bitrate-up %%ARG%%",
/* 38 */ "configure xdsl service-profile %%ARG%% plan-bitrate-down %%ARG%%",
/* 39 */ "configure xdsl service-profile %%ARG%% max-delay-up %%ARG%%",
/* 40 */ "configure xdsl service-profile %%ARG%% max-delay-down %%ARG%%",
/* 41 */ "configure xdsl service-profile %%ARG%% max-bitrate-up %%ARG%%",
/* 42 */ "configure xdsl service-profile %%ARG%% max-bitrate-down %%ARG%%",
/* 43 */ "configure xdsl service-profile %%ARG%% min-bitrate-up %%ARG%%",
/* 44 */ "configure xdsl service-profile %%ARG%% min-bitrate-down %%ARG%%",
/* 45 */ "admin equipment reboot-isam hot-restart",
/* 46 */ "admin equipment reboot-isam with-self-test",
/* 47 */ "admin equipment reboot-shub applicationreset",
/* 48 */ "configure interface port xdsl-line:%%ARG%% admin-up\r\nexit\r\nexit\r\nexit",
/* 49 */ "configure interface port xdsl-line:%%ARG%% no admin-up\r\nexit\r\nexit\r\nexit",
/* 50 */ "configure system security filetransfer protocol %%ARG%%\r\nexit\r\nexit\r\nexit\r\nexit",
/* 51 */ "configure system security filetransfer server %%ARG%% user-name %%ARG%% password %%ARG%%\r\nexit\r\nexit\r\nexit\r\nexit",
/* 52 */ "admin software-mngt database upload actual-active:%%ARG%%:%%ARG%%\r\nexit\r\nexit\r\nexit\r\nexit",
/* 53 */ "admin software-mngt database download %%ARG%%:%%ARG%%\r\nexit\r\nexit\r\nexit\r\nexit",
/* 54 */ "admin alarm delta-log clr-%%ARG%%\r\nexit\r\nexit\r\nexit\r\nexit",
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
 * #mdm_device_dslam_alcatel_73xx_data_t.promptlen
 * \param d Device descriptor.
 * \param ct One of #mdm_connection_type_t
 * \param status Where to store the result.
 */ 
void
mdm_device_dslam_alcatel_73xx_open(
	mdm_device_descriptor_t *d, mdm_connection_type_t ct,
	mdm_operation_result_t *status
)
{
	mdm_driver_telnet_options_t telnet_options;
	mdm_driver_ssh2_options_t ssh2_options;
	char *host_port_token = NULL;
	mdm_device_dslam_alcatel_73xx_options_t *options;
	mdm_connection_options_t *conn_options;
	int i;
	char *envcmds[] = {
		"environment terminal-timeout timeout:5",
		"environment print no-more",
		"environment inhibit-alarms"
	};
	int envcmdslen[] = { 38, 25, 26};
	char buffer[4096];
	int bufflen;
	mdm_device_dslam_alcatel_73xx_data_t *data =
		(mdm_device_dslam_alcatel_73xx_data_t *)d->data;

	options = (mdm_device_dslam_alcatel_73xx_options_t *)d->options;
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

	/* Wait for login. */
	if(ct != MDM_CONNECTION_SSH2)
	{
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
		} while(strstr(buffer, "password:") == NULL);
		mdm_connection_send(
			&d->connection, options->password, strlen(options->password), status
		);
		if(status->status == MDM_OP_ERROR)
		{
			return;
		}
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
	} while(strstr(buffer, ">#") == NULL);
	
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
	/* Set environment properties. */
	for(i = 0; i < 3; i++)
	{
		d->exec_buffer_cmd_len = envcmdslen[i];
		snprintf(
			d->exec_buffer_cmd, d->exec_buffer_cmd_len + 1, "%s", envcmds[i]
		);
		mdm_device_dslam_alcatel_73xx_exec(d, status);
	}
#if MDM_DEBUG_MESSAGES > 0
		MDM_LOGDEBUG("Login successfull: promptlen: %d", data->promptlen);
#endif		
	return;
}

/*!
 * Will try to prepare a command for exec.
 * \param d Device descriptor.
 * \param c Command, one of #mdm_device_cmd_dslam_alcatel_73xx_t
 * \param args Command arguments, normally a csv string.
 * \param status Where to store the result.
 */
void
mdm_device_dslam_alcatel_73xx_prepare_exec(
	mdm_device_descriptor_t *d, mdm_device_cmd_t c,
	const char *args, mdm_operation_result_t *status
)
{
	const char *cmd = MDM_DEVICE_CMD_DSLAM_ALCATEL_73xx_STR[c];
	int cmdlen = strlen(cmd);
	int cmdargsn = MDM_DEVICE_CMD_DSLAM_ALCATEL_73xx_ARGSN[c];
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
		(strlen(DSLAM_ALCATEL_73xx_CMD_ARG) * cmdargsn) -
		argstoken
	;
	precmdlen = 
		cmdlen + argslen -
		(strlen(DSLAM_ALCATEL_73xx_CMD_ARG) * cmdargsn) -
		argstoken
	;
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG(
		"clen: %d, alen: %d, ctoklen: %d, atoklen: %d",
		cmdlen, argslen, strlen(DSLAM_ALCATEL_73xx_CMD_ARG) * cmdargsn, argstoken
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
		cmdp2 = strstr(cmdp1, DSLAM_ALCATEL_73xx_CMD_ARG);

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
		cmdp1 = cmdp2 + strlen(DSLAM_ALCATEL_73xx_CMD_ARG);
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
mdm_device_dslam_alcatel_73xx_close(
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

/*******************************************************************************
 * Internal functions.
 ******************************************************************************/

/*!
 * Will search for known error strings in response buffer.
 * \param d Device descriptor.
 * \param status Where to store the result error code.
 * \return void
 */
void
mdm_device_dslam_alcatel_73xx_check_error(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	const char *error;
	
	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif

	if(strstr(d->exec_buffer, "invalid token") != NULL) {
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Command syntax error?");
	} else if((error = strstr(d->exec_buffer, "Error : ")) != NULL) {
		status->status = MDM_OP_ERROR;
		snprintf(
			status->status_message, strrchr(error, '\n') - error, "%s", error
		);
	} else if((
			error = strstr(d->exec_buffer, "command is not complete"
		)) != NULL) {
		status->status = MDM_OP_ERROR;
		snprintf(
			status->status_message, strrchr(error, '\n') - error, "%s", error
		);
	}

	/* Done. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
}

/*!
 * Will exec a command. It will strip the prompt from
 * the finalline and the issued command from the first n lines. See 
 * #mdm_device_dslam_alcatel_73xx_data_t.promptlen need to avoid
 * \\27 [1Dx
 * \param d Device descriptor.
 * \param status Where to store the result error code.
 * \return Result from executed command.
 */
void
mdm_device_dslam_alcatel_73xx_exec(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	char tempbuffer[256];
	int tempbufferlen;
	char *lastlinebeforeprompt = NULL;
	char *foundcmd = NULL;
	char *prompt = NULL;
	char *promptlast = NULL;
	mdm_device_dslam_alcatel_73xx_data_t *data =
		(mdm_device_dslam_alcatel_73xx_data_t *)d->data;
	/* We have to skip the issued command *plus* the length of the prompt ;) */
	float linestoskip = ceilf((float)(
		d->exec_buffer_cmd_len + data->promptlen) / (float)(80)
	);
	int i, j;
		
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
		if(
			(tempbufferlen <= 0) &&
			(tempbufferlen != MDM_TELNET_DRIVER_ONLY_TELNET_DATA)
		) {
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "Receive timeout.");
			return;
		}
#if MDM_DEBUG_MESSAGES > 0
		MDM_LOGDEBUG("Got %s", tempbuffer);
#endif
		d->exec_buffer_len += tempbufferlen;
		strncat(d->exec_buffer, tempbuffer, tempbufferlen);
		promptlast = NULL;
		prompt = d->exec_buffer;
		while((prompt = strstr(prompt, ">")) != NULL)
		{
			prompt++;
			promptlast = prompt;
		}
		if((promptlast != NULL) && strchr(promptlast, 13) == NULL)
		{
			lastlinebeforeprompt = strrchr(d->exec_buffer, 13);
			*lastlinebeforeprompt = 0;
			break;
		}
	} while(1);

#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Stripping spinner");
#endif
	/* Strip spinner. */
	for(i = 0, j = 0; i < d->exec_buffer_len; i++)
	{
		if((d->exec_buffer)[i] == 27)
		{
			i += 3;
			j--;
			continue;
		}
		d->exec_buffer_post[j] = (d->exec_buffer)[i];
		j++;
	}
	memcpy(d->exec_buffer, d->exec_buffer_post, j);
	d->exec_buffer_len = j;
	/*
	 * Strip command from response: We end up skipping the length of the command
	 * plus the length of the prompt / 80 columns. Each line has \r\n because
	 * of the telnet protocol, so we skip that too.
	 */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG(
		"Skipping %f lines from cmdlen: %d.", linestoskip, d->exec_buffer_cmd_len
	);
#endif
	/* Allocate space for command stripping. */
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
 * Called by #mdm_device_new to initialize a device dslam alcatel 73xx.
 * \param d Device.
 * \param status Where to store the result.
 */
void
mdm_device_dslam_alcatel_73xx_new(
	mdm_device_t *d, mdm_operation_result_t *status
)
{
	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif

	d->open = mdm_device_dslam_alcatel_73xx_open;
	d->exec = mdm_device_dslam_alcatel_73xx_exec;
	d->prepare_exec = mdm_device_dslam_alcatel_73xx_prepare_exec;
	d->post_exec = mdm_device_dslam_alcatel_73xx_post_exec;
	d->close = mdm_device_dslam_alcatel_73xx_close;
	d->check_error = mdm_device_dslam_alcatel_73xx_check_error;

	/* Done. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
	return;
}

/*!
 * Called by #mdm_device_free to deinitialize a device dslam alcatel 73xx.
 * \param d Device.
 */
void
mdm_device_dslam_alcatel_73xx_free(mdm_device_t *d)
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
 * This will process the output from #mdm_device_dslam_alcatel_73xx_exec.
 * Dont call this directly, use #mdm_device_exec.
 * \param d Device descriptor.
 * \param c Device command used to get response.
 * \param status Result of the operation.
 */
void
mdm_device_dslam_alcatel_73xx_post_exec(
	mdm_device_descriptor_t * d, 
	mdm_device_cmd_t c, mdm_operation_result_t *status

)
{
	if(MDM_DEVICE_CMD_DSLAM_ALCATEL_73xx_PROCESS[c] == NULL)
	{
		d->exec_buffer_post_len = d->exec_buffer_len;
		memcpy(d->exec_buffer_post, d->exec_buffer, d->exec_buffer_len);
	} else {
		MDM_DEVICE_CMD_DSLAM_ALCATEL_73xx_PROCESS[c](d, status);
	}
}
/*******************************************************************************
 * CODE ENDS.
 ******************************************************************************/
