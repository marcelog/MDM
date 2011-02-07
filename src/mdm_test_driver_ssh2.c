/*!
 * \file mdm_test_driver_ssh2.c A test for the ssh2 driver. This program
 * will poll for a complete line (non blocking) from stdin and then send
 * it through the connection. It will also read (non blocking also) from the
 * connection and print any characters received, much like a telnet client
 * does. Has 6 arguments: host, port, connection timeout, read timeout user,
 * and pass.\n
 * You may also use a service name from /etc/services instead of a port number,
 * i.e: 22 vs. ssh or 80 vs. http, etc. For connection timeout, use 0 if you
 * dont want to set any. Any number > 0 will be the connection timeout in
 * milliseconds.
 *
 * \todo Better terminal handling (termios, ioctl, etc).
 * \todo Send and receive OOB data, like ^C
 * \todo Send and receive special telnet characters and handle them in the
 * terminal, like ^L.
 * \todo Make vim work ;)
 * \todo Add escape character, like real telnet client.
 * \todo Add support for different authentication schemes, like public key
 * and keyboard interactive.
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
/*******************************************************************************
 * Include Files.
 ******************************************************************************/
#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<string.h>
#include	<signal.h>
#include	<poll.h>
#include	<errno.h>
#include	<mdm.h>

/*******************************************************************************
 * Constants.
 ******************************************************************************/
#define	LOGLEVEL		MDM_LOG_DEBUG
#define	LOGSYSLOG	1
#define	LOGFILE		1
#define	LOGFULL		1
#define	LOGPATH		"/tmp/mdm_test_driver_ssh2.log"

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
 * Main Entry Point.
 */
int
main(int argc, char *argv[])
{
	int retcode = EXIT_SUCCESS;
	mdm_connection_t c;
	mdm_driver_ssh2_options_t options;
	mdm_operation_result_t status;
	struct pollfd pollinfo;
	char linebuffer[4096];
	int bufflen;
	size_t stdinbuffer_len;
	char *stdinbuffer;
	
	/* Check command line arguments. */
	if(argc < 7)
	{
		fprintf(
			stderr,
			"Use: %s <host> <port> <conntimeout> <readtimeout> <user> <pass>\n",
			argv[0]
		);
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "Start.\n");

	/* Global MDM init. */
	mdm_init(LOGLEVEL, LOGSYSLOG, LOGFILE, LOGFULL, LOGPATH);

	/* Allocate a connection of type raw tcp. */
	mdm_connection_new(&c, MDM_CONNECTION_SSH2, &status);
	if(status.status == MDM_OP_ERROR)
	{
		fprintf(stderr, "Error creating connection: %s\n", status.status_message);
		retcode = 254;
		goto done;
	}

	/* Copy options from command line. */
	snprintf(options.host, sizeof(options.host), "%s", argv[1]);
	snprintf(options.service, sizeof(options.service), "%s", argv[2]);
	snprintf(options.username, sizeof(options.username), "%s", argv[5]);
	snprintf(options.password, sizeof(options.password), "%s", argv[6]);
	options.echo_cancel = 1;
	options.to_connect = strtol(argv[3], NULL, 10); /* In millis. */	
	options.to_recv = strtol(argv[4], NULL, 10); /* In millis. */	

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
				/*
					This will only work on unixes. On windows boxes, we should check
					for \r\n
				*/
				if(stdinbuffer[stdinbuffer_len - 1] == 13)
					stdinbuffer_len--;
				if(stdinbuffer[stdinbuffer_len - 1] == 10)
					stdinbuffer_len--;
				memcpy(linebuffer, stdinbuffer, stdinbuffer_len);
				mdm_connection_send(&c, linebuffer, stdinbuffer_len, &status);
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
