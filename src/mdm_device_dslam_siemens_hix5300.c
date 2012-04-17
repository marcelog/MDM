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
/* 2  */ "Show version",
/* 3  */ "Show NTP client",
/* 4  */ "Show alarm list",
/* 5  */ "Show upgrade",
/* 6  */ "Show upgrade list",
/* 7  */ "Show syslog",
/* 8  */ "Show fan status",
/* 9  */ "Show slots",
/* 10 */ "Show uptime",
/* 11 */ "Show memory",
/* 12 */ "Show PVID",
/* 13 */ "Show ATM VC",
/* 14 */ "Show Port Detail",
/* 15 */ "Show Port ATUC",
/* 16 */ "Show Port ATUR",
/* 17 */ "Show Port line table",
/* 18 */ "Show Port line config",
/* 19 */ "Show profiles",
/* 20 */ "Show arp",
/* 21 */ "Show net interfaces",
/* 22 */ "Show CPU load",
/* 23 */ "Configure timezone",
/* 24 */ "Configure syslog",
/* 25 */ "Unconfigure syslog",
/* 26 */ "Reset all",
/* 27 */ "Reset card",
/* 28 */ "Configure NTP Server",
/* 29 */ "Show adsl lre",
/* 30 */ "Ports config",
/* 31 */ "Physical port info",
/* 32 */ "Unconfigure NTP Server",
/* 33 */ "Configure NTP interval",
/* 34 */ "Show mac in ports",
/* 35 */ "Create line profile",
/* 36 */ "Delete line profile",
/* 37 */ "Set line profile to fast",
/* 38 */ "Set line profile to interleaved",
/* 39 */ "Set line profile standard",
/* 40 */ "Set atuc fast max tx rate",
/* 41 */ "Set atuc fast min tx rate",
/* 42 */ "Set atur fast max tx rate",
/* 43 */ "Set atur fast min tx rate",
/* 44 */ "Set atuc interleaved max tx rate",
/* 45 */ "Set atuc interleaved min tx rate",
/* 46 */ "Set atur interleaved max tx rate",
/* 47 */ "Set atur interleaved min tx rate",
/* 48 */ "Set max tx rate",
/* 49 */ "Set min tx rate",
/* 50 */ "Set port line profile",
/* 51 */ "Add vc to port",
/* 52 */ "Remove vc from port",
/* 53 */ "Configure vcc in port",
/* 54 */ "Enable port",
/* 55 */ "Disable port",
/* 56 */ "Set port description",
/* 57 */ "Get port description",
/* 58 */ "Show port 0",
/* 59 */ "Create VLAN",
/* 60 */ "Delete VLAN",
	NULL
};

/*!
 * Contains the number of arguments for each command.
 */
static int MDM_DEVICE_CMD_DSLAM_SIEMENS_HIX5300_ARGSN[] =
{
/* 0  */ 0,
/* 1  */ 0,
/* 2  */ 0,
/* 3  */ 0,
/* 4  */ 0,
/* 5  */ 0,
/* 6  */ 0,
/* 7  */ 0,
/* 8  */ 0,
/* 9  */ 0,
/* 10 */ 0,
/* 11 */ 0,
/* 12 */ 0,
/* 13 */ 1,
/* 14 */ 1,
/* 15 */ 1,
/* 16 */ 1,
/* 17 */ 1,
/* 18 */ 1,
/* 19 */ 0,
/* 20 */ 0,
/* 21 */ 0,
/* 22 */ 0,
/* 23 */ 1,
/* 24 */ 2,
/* 25 */ 2,
/* 26 */ 0,
/* 27 */ 1,
/* 28 */ 1,
/* 29 */ 1,
/* 30 */ 0,
/* 31 */ 1,
/* 32 */ 1,
/* 33 */ 1,
/* 34 */ 1,
/* 35 */ 1,
/* 36 */ 1,
/* 37 */ 1,
/* 38 */ 1,
/* 39 */ 2,
/* 40 */ 2,
/* 41 */ 2,
/* 42 */ 2,
/* 43 */ 2,
/* 44 */ 2,
/* 45 */ 2,
/* 46 */ 2,
/* 47 */ 2,
/* 48 */ 2,
/* 49 */ 2,
/* 50 */ 2,
/* 51 */ 3,
/* 52 */ 3,
/* 53 */ 4,
/* 54 */ 1,
/* 55 */ 1,
/* 56 */ 2,
/* 57 */ 1,
/* 58 */ 0,
/* 59 */ 1,
/* 60 */ 1
};

/*!
 * Corresponding processing routines post exec.
 */
const mdm_device_dslam_siemens_hix5300_post_exec_t
MDM_DEVICE_CMD_DSLAM_SIEMENS_HIX5300_PROCESS[] =
{
/* 0  */ dslam_siemens_hix5300_nop,
/* 1  */ dslam_siemens_hix5300_nop,
/* 2  */ dslam_siemens_hix5300_get_system_version,
/* 3  */ dslam_siemens_hix5300_get_ntp,
/* 4  */ dslam_siemens_hix5300_get_alarms,
/* 5  */ dslam_siemens_hix5300_get_soft_versions,
/* 6  */ dslam_siemens_hix5300_nop,
/* 7  */ dslam_siemens_hix5300_get_syslog,
/* 8  */ dslam_siemens_hix5300_get_fans,
/* 9  */ dslam_siemens_hix5300_get_slots,
/* 10 */ dslam_siemens_hix5300_get_uptime,
/* 11 */ dslam_siemens_hix5300_get_memory_info,
/* 12 */ dslam_siemens_hix5300_nop,
/* 13 */ dslam_siemens_hix5300_nop,
/* 14 */ dslam_siemens_hix5300_get_port_detail,
/* 15 */ dslam_siemens_hix5300_nop,
/* 16 */ dslam_siemens_hix5300_nop,
/* 17 */ dslam_siemens_hix5300_get_physical_port_info,
/* 18 */ dslam_siemens_hix5300_get_line_config,
/* 19 */ dslam_siemens_hix5300_get_service_profile,
/* 20 */ dslam_siemens_hix5300_nop,
/* 21 */ dslam_siemens_hix5300_get_network_interfaces,
/* 22 */ dslam_siemens_hix5300_nop,
/* 23 */ dslam_siemens_hix5300_nop,
/* 24 */ dslam_siemens_hix5300_nop,
/* 25 */ dslam_siemens_hix5300_nop,
/* 26 */ dslam_siemens_hix5300_nop,
/* 27 */ dslam_siemens_hix5300_nop,
/* 28 */ dslam_siemens_hix5300_nop,
/* 29 */ dslam_siemens_hix5300_get_slot_ports,
/* 30 */ dslam_siemens_hix5300_get_all_ports_config,
/* 31 */ dslam_siemens_hix5300_get_physical_port_info,
/* 32 */ dslam_siemens_hix5300_nop,
/* 33 */ dslam_siemens_hix5300_nop,
/* 34 */ dslam_siemens_hix5300_get_mac_ports,
/* 35 */ dslam_siemens_hix5300_nop,
/* 36 */ dslam_siemens_hix5300_nop,
/* 37 */ dslam_siemens_hix5300_nop,
/* 38 */ dslam_siemens_hix5300_nop,
/* 39 */ dslam_siemens_hix5300_nop,
/* 40 */ dslam_siemens_hix5300_nop,
/* 41 */ dslam_siemens_hix5300_nop,
/* 42 */ dslam_siemens_hix5300_nop,
/* 43 */ dslam_siemens_hix5300_nop,
/* 44 */ dslam_siemens_hix5300_nop,
/* 45 */ dslam_siemens_hix5300_nop,
/* 46 */ dslam_siemens_hix5300_nop,
/* 47 */ dslam_siemens_hix5300_nop,
/* 48 */ dslam_siemens_hix5300_nop,
/* 49 */ dslam_siemens_hix5300_nop,
/* 50 */ dslam_siemens_hix5300_nop,
/* 51 */ dslam_siemens_hix5300_nop,
/* 52 */ dslam_siemens_hix5300_nop,
/* 53 */ dslam_siemens_hix5300_nop,
/* 54 */ dslam_siemens_hix5300_nop,
/* 55 */ dslam_siemens_hix5300_nop,
/* 56 */ dslam_siemens_hix5300_nop,
/* 57 */ dslam_siemens_hix5300_get_port_description,
/* 58 */ dslam_siemens_hix5300_nop,
/* 59 */ dslam_siemens_hix5300_nop,
/* 60 */ dslam_siemens_hix5300_nop
};

/*!
 * Corresponding command strings from #mdm_device_cmd_dslam_siemens_hix5300_t
 */
const char *MDM_DEVICE_CMD_DSLAM_SIEMENS_HIX5300_STR[] =
{
/* 0  */	"",
/* 1  */	"show running-config",
/* 2  */    "show system-version",
/* 3  */    "show ntp-client",
/* 4  */    "show alarm-list",
/* 5  */    "show upgrade",
/* 6  */    "show upgrade table",
/* 7  */    "show syslog",
/* 8  */    "show status fan",
/* 9  */    "show slot all flow-ctrl",
/* 10 */    "show uptime",
/* 11 */    "show memory",
/* 12 */    "show pvid",
/* 13 */    "show lre %%ARG%% atm vc",
/* 14 */    "show lre %%ARG%% adsl line-config-detail",
/* 15 */    "show lre %%ARG%% adsl physical atuc",
/* 16 */    "show lre %%ARG%% adsl physical atur",
/* 17 */    "show lre %%ARG%% adsl line-table",
/* 18 */    "show lre %%ARG%% adsl line-config",
/* 19 */    "show adsl line-config-profile",
/* 20 */    "show arp",
/* 21 */    "show ip interface brief",
/* 22 */    "show cpuload",
/* 23 */    "configure terminal\ntime-zone %%ARG%%\nexit",
/* 24 */    "configure terminal\nsyslog output %%ARG%% remote %%ARG%%\nexit",
/* 25 */    "configure terminal\nno syslog output %%ARG%% remote %%ARG%%\nexit",
/* 26 */    "configure terminal\nreset all\nexit",
/* 27 */    "configure terminal\nreset card %%ARG%%\nexit",
/* 28 */    "configure terminal\nntp-client server %%ARG%%\nexit",
/* 29 */    "show lre s%%ARG%% adsl",
/* 30 */    "show running-config | include lre",
/* 31 */    "show lre %%ARG%% adsl line-table",
/* 32 */    "configure terminal\nno ntp-client server %%ARG%%\nexit",
/* 33 */    "configure terminal\nntp-client interval %%ARG%%\nexit",
/* 34 */    "show mac port %%ARG%%",
/* 35 */    "configure terminal\nbridge\nadsl add line-config-profile %%ARG%%\nexit\nexit",
/* 36 */    "configure terminal\nbridge\nadsl delete line-config-profile %%ARG%%\nexit\nexit",
/* 37 */    "configure terminal\nbridge\nadsl line-config-profile %%ARG%% comm line-type fast\nexit\nexit",
/* 38 */    "configure terminal\nbridge\nadsl line-config-profile %%ARG%% comm line-type interleaved\nexit\nexit",
/* 39 */    "configure terminal\nbridge\nadsl line-config-profile %%ARG%% gs standard %%ARG%%\nexit\nexit",
/* 40 */    "configure terminal\nbridge\nadsl line-config-profile %%ARG%% atuc fast max-tx-rate %%ARG%%\nexit\nexit",
/* 41 */    "configure terminal\nbridge\nadsl line-config-profile %%ARG%% atuc fast min-tx-rate %%ARG%%\nexit\nexit",
/* 42 */    "configure terminal\nbridge\nadsl line-config-profile %%ARG%% atur fast max-tx-rate %%ARG%%\nexit\nexit",
/* 43 */    "configure terminal\nbridge\nadsl line-config-profile %%ARG%% atur fast min-tx-rate %%ARG%%\nexit\nexit",
/* 44 */    "configure terminal\nbridge\nadsl line-config-profile %%ARG%% atuc interleaved max-tx-rate %%ARG%%\nexit\nexit",
/* 45 */    "configure terminal\nbridge\nadsl line-config-profile %%ARG%% atuc interleaved min-tx-rate %%ARG%%\nexit\nexit",
/* 46 */    "configure terminal\nbridge\nadsl line-config-profile %%ARG%% atur interleaved max-tx-rate %%ARG%%\nexit\nexit",
/* 47 */    "configure terminal\nbridge\nadsl line-config-profile %%ARG%% atur interleaved min-tx-rate %%ARG%%\nexit\nexit",
/* 48 */    "configure terminal\nbridge\nadsl line-config-profile %%ARG%% atur max-tx-rate %%ARG%%\nexit\nexit",
/* 49 */    "configure terminal\nbridge\nadsl line-config-profile %%ARG%% atur min-tx-rate %%ARG%%\nexit\nexit",
/* 50 */    "configure terminal\nbridge\nlre %%ARG%% adsl line-config profile %%ARG%%\nexit\nexit",
/* 51 */    "configure terminal\nbridge\nlre %%ARG%% atm vc create vpi %%ARG%% vci %%ARG%%\nexit\nexit",
/* 52 */    "configure terminal\nbridge\nlre %%ARG%% atm vc delete vpi %%ARG%% vci %%ARG%%\nexit\nexit",
/* 53 */    "configure terminal\nbridge\nlre %%ARG%% atm vcc %ARG%% vpi %%ARG%% vci %%ARG%%\nexit\nexit",
/* 54 */    "configure terminal\nbridge\nport lre %%ARG%% enable\nexit\nexit",
/* 55 */    "configure terminal\nbridge\nport lre %%ARG%% disable\nexit\nexit",
/* 56 */    "configure terminal\nbridge\nport lre %%ARG%% description %%ARG%%\nexit\nexit",
/* 57 */    "show port %%ARG%% description",
/* 58 */    "show port s0",
/* 59 */    "configure terminal\nbridge\nvlan create %%ARG%%\nexit\nexit",
/* 60 */    "configure terminal\nbridge\nno vlan %%ARG%%\nexit\nexit",
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
	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif
    if((error = strstr(d->exec_buffer, "%")) != NULL) {
	    status->status = MDM_OP_ERROR;
		snprintf(
            status->status_message, sizeof(status->status_message), "%s", error
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
    int keepreading = 1;
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
        prompt = d->exec_buffer;
        while((prompt = strstr(prompt, data->prompt)) != NULL) {
            MDM_LOGDEBUG("Candidate prompt found");
			prompt += data->promptlen;
			if (*prompt == '#' || *prompt == '>') {
				lastlinebeforeprompt = strrchr(d->exec_buffer, 13);
				*lastlinebeforeprompt = 0;
                keepreading = 0;
				break;
			}
		}
	} while(keepreading > 0);
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
		snprintf(d->exec_buffer, d->exec_buffer_len + 1, "%s", foundcmd);
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
