/*!
 * \file mdm_test_driver_rawtcp.c A test for the raw tcp driver. This program
 * will poll for a complete line (non blocking) from stdin and then send
 * it through the connection. It will also read (non blocking also) from the
 * connection and print any characters received, much like a telnet client
 * does (remember this is *not* a telnet driver, just a raw tcp one, so you
 * may use it to test something like http or raw tcp connections).
 * Has 3 arguments: host, port, and connection timeout. You may also use a
 * service name from /etc/services instead of a port number, i.e: 23 vs. telnet
 * or 80 vs. http, etc. For connection timeout, use 0 if you dont want to set
 * any. Any number > 0 will be the connection timeout in milliseconds.
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
#define	LOGPATH		"/tmp/mdm_test_driver_rawtcp.log"

/*******************************************************************************
 * Global variables.
 ******************************************************************************/
static int running = 1; /*!< Changed by #signal_handler to 0 on SIGINT, SIGTERM
	and SIGQUIT */

/*******************************************************************************
 * Forward Declarations.
 ******************************************************************************/
static void signal_handler(int sig);
int main(int argc, char *argv[]);

/*******************************************************************************
 * CODE STARTS.
 ******************************************************************************/
/*!
 * This is our signal handler. Will catch signals SIGINT, SIGTERM, and
 * SIGQUIT and change #running accordingly to affect main loop.
 */
static void
signal_handler(int sig)
{
	switch(sig)
	{
		case SIGINT:
		case SIGTERM:
		case SIGQUIT:
			fprintf(stderr, "Got signal: %d\n", sig);
			running = 0;
			break;
		default:
			fprintf(stderr, "Unhandled signal: %d\n", sig);
			break;
	}
}
/*!
 * Main entry point.
 */
int
main(int argc, char *argv[])
{
	int retcode = EXIT_SUCCESS;
	mdm_connection_t c;
	mdm_driver_rawtcp_options_t options;
	mdm_operation_result_t status;
	struct pollfd pollinfo;
	char linebuffer[4096];
	int bufflen;
	size_t stdinbuffer_len;
	char *stdinbuffer;
	
	/* Check command line arguments. */
	if(argc < 4)
	{
		fprintf(stderr, "Use: %s <host> <port> <connect timeout>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "Start.\n");

	/* Global MDM init. */
	mdm_init(LOGLEVEL, LOGSYSLOG, LOGFILENAME, LOGFULL, LOGPATH);

	/* Allocate a connection of type raw tcp. */
	mdm_connection_new(&c, MDM_CONNECTION_RAW_TCP, &status);
	if(status.status == MDM_OP_ERROR)
	{
		fprintf(stderr, "Error creating connection: %s\n", status.status_message);
		retcode = 254;
		goto done;
	}

	/* Copy options from command line. */
	snprintf(options.host, sizeof(options.host), "%s", argv[1]);
	snprintf(options.service, sizeof(options.service), "%s", argv[2]);
	options.to_recv = 10;
	options.to_connect = strtol(argv[3], NULL, 10); /* In millis. */	
	
	/* Setup signal handler. */
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGQUIT, signal_handler);

	/* Finally, open the connection to the host. */
	mdm_connection_open(&c, (mdm_connection_options_t)&options, &status);
	if(status.status == MDM_OP_ERROR)
	{
		fprintf(stderr, "Error opening: %s\n", status.status_message);
		retcode = 254;
		goto done;
	}
	fprintf(stdout, "Connected.\n");
	/* Main loop. Read lines and send/recv data. */
	while(running)
	{
		bufflen = sizeof(linebuffer);
		memset(linebuffer, 0, bufflen);
		mdm_connection_recv(&c, linebuffer, &bufflen, &status);
		if(status.status == MDM_OP_ERROR)
		{
			fprintf(stderr, "Error receiving: %s\n", status.status_message);
			retcode = 254;
			goto done;
		}
		if(bufflen > 0)
		{
			fprintf(stdout, "%s", linebuffer);
			fflush(stdout);
		}
		usleep(10);

		/* Setup poll info. */
		pollinfo.fd = 0;
		pollinfo.events = POLLIN | POLLRDNORM | POLLRDBAND | POLLPRI;
		pollinfo.revents = POLLIN | POLLRDNORM | POLLRDBAND | POLLPRI;
		switch(poll(&pollinfo, 1, 0))
		{
			case 0:
				break;
			case -1:
				running = 0;
				fprintf(stderr, "poll() error on stdin: %s\n", strerror(errno));
				break;
			default:
				stdinbuffer = fgetln(stdin, &stdinbuffer_len);
				memcpy(linebuffer, stdinbuffer, stdinbuffer_len - 1);
				memcpy(linebuffer + stdinbuffer_len - 1, "\r\n", 3);
				mdm_connection_send(&c, linebuffer, strlen(linebuffer), &status);
				if(status.status == MDM_OP_ERROR)
				{
					fprintf(stderr, "Error sending: %s\n", status.status_message);
					retcode = 254;
					goto done;
				}
		}
	}
done:
	/* Cleanup connection. */
	mdm_connection_close(&c, &status);
	mdm_connection_free(&c);

	/* Global MDM cleanup. */
	mdm_cleanup();
	fprintf(stdout, "Done.\n");
	exit(retcode);
}
/*******************************************************************************
 * CODE ENDS.
 ******************************************************************************/
