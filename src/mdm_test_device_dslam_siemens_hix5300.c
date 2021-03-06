/*!
 * \file mdm_test_device_dslam_siemens_hix5300.c A test for dslams siemens
 * hix 5300.
 *
 * # ./mdm_test_device_dslam_siemens_hix5300 list
 *
 * Will show a list of available commands (by name and command to be run).
 *
 * # ./mdm_test_device_dslam_siemens_hix5300
 * contype connection_timeout read_timeout target user pass enable cmd cmdargs
 *
 * Where:
 *
 * contype is the id of the connection to be used (i.e: telnet is 0). Use
 * mdm_connection_types_list to get them all.
 *
 * connection_timeout is in milliseconds the max time to wait for a connection
 * to the device to succeed.
 *
 * read_timeout is in milliseconds the max time to wait for data from the
 * device (per read).
 *
 * target is host:port (for telnet and ssh).
 *
 * user is the username to use.
 *
 * pass is the password to use.
 *
 * enable password.
 *
 * cmd is the id of the command to run.
 *
 * cmdargs is a csv with the arguments for the command.
 *
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
/*******************************************************************************
 * Include Files.
 ******************************************************************************/
#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<string.h>
#include	<errno.h>
#include	<signal.h>
#include	<poll.h>
#include	<mdm.h>

/*******************************************************************************
 * Constants.
 ******************************************************************************/
#define	LOGLEVEL	MDM_LOG_DEBUG
#define	LOGSYSLOG	1
#define	LOGFILENAME	1
#define	LOGFULL		1
#define	LOGPATH		"/tmp/mdm_test_device_dslam_siemens_hix5300.log"

/*******************************************************************************
 * Global variables.
 ******************************************************************************/

/*******************************************************************************
 * Forward Declarations.
 ******************************************************************************/
int main(int argc, char *argv[]);

/*******************************************************************************
 * CODE STARTS.
 ******************************************************************************/

/*!
 * Main entry point.
 */
int
main(int argc, char *argv[])
{
	int retcode = EXIT_SUCCESS;
	mdm_device_t device;
	const char *cmd_name;
	int i = 0;
	mdm_operation_result_t status;
	mdm_device_dslam_siemens_hix5300_options_t options;
	int cmd;

	/* Check command line arguments. */
	if(argc < 2)
	{
		fprintf(stderr, "Use: %s <list | cmd args...>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* Global MDM init. */
	mdm_init(LOGLEVEL, LOGSYSLOG, LOGFILENAME, LOGFULL, LOGPATH);
	mdm_device_new(&device, MDM_DEVICE_DSLAM_SIEMENS_HIX5300, &status);
	if(status.status == MDM_OP_ERROR)
	{
		fprintf(stderr, "Error: %s\n", status.status_message);
		retcode = 254;
		goto done;
	}

	/* See what we got in the arguments. */
	if(strcmp(argv[1], "list") == 0)
	{
		fprintf(stdout, "Start.\n");
		while((
			cmd_name = mdm_devicecmd_2stringname(MDM_DEVICE_DSLAM_SIEMENS_HIX5300, i
		)) != NULL)
		{
			fprintf(
				stdout, "(%.2d) - !!%s!!%s!!\n",
				i, cmd_name,
				mdm_devicecmd_2string(MDM_DEVICE_DSLAM_SIEMENS_HIX5300, i)
			);
			i++;
		}
		goto done;
	} else {
		if(argc < 9)
		{
			fprintf(
				stderr,
				"Use: %s contype conn_to read_to target user pass enable cmd cmdargs\n",
				argv[0]
			);
			retcode = 254;
			goto done;
		}
		fprintf(stdout, "Start.\n");
		cmd = strtol(argv[8], NULL, 10);
		if(!mdm_devicecmd_isvalid(MDM_DEVICE_DSLAM_SIEMENS_HIX5300, cmd))
		{
			fprintf(stderr, "Invalid command.\n");
			retcode = 254;
			goto done;
		}
		snprintf(options.target, sizeof(options.target), "%s", argv[4]);
		snprintf(options.username, sizeof(options.username), "%s", argv[5]);
		snprintf(options.password, sizeof(options.password), "%s", argv[6]);
		snprintf(options.enable, sizeof(options.password), "%s", argv[7]);
		options.to_connect = strtol(argv[2], NULL, 10);
		options.to_recv = strtol(argv[3], NULL, 10);

		mdm_device_open(
			&device, strtol(argv[1], NULL, 10),
			(mdm_device_options_t)&options,
			sizeof(mdm_device_dslam_siemens_hix5300_options_t),
			&status
		);
		if(status.status == MDM_OP_ERROR)
		{
			fprintf(stderr, "Error: %s\n", status.status_message);
			retcode = 254;
			goto done;
		}
		mdm_device_exec(&device, cmd, argc == 10 ? argv[9] : NULL, &status);
		if(status.status == MDM_OP_ERROR)
		{
			fprintf(stderr, "App Error: %s\n", status.status_message);
			retcode = 254;
		}
		if(mdm_device_get_buffer_post_len(&device) > 0)
		{
			fprintf(
				stdout, "%.*s\n",
				mdm_device_get_buffer_post_len(&device),
				mdm_device_get_buffer_post(&device)
			);
		}
		if(mdm_device_get_buffer_len(&device) > 0)
		{
			fprintf(
				stdout, "Original Response:|%.*s|\n",
				mdm_device_get_buffer_len(&device),
				mdm_device_get_buffer(&device)
			);
		}
		mdm_device_close(&device, &status);
	}
done:
	mdm_device_free(&device);
	/* Global MDM cleanup. */
	mdm_cleanup();
	fprintf(stdout, "Done.\n");
	exit(retcode);
}
/*******************************************************************************
 * CODE ENDS.
 ******************************************************************************/
