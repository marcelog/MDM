/*!
 * \file mdm_driver_ssh2.c This is a telnet driver. Uses libssh2. Only supports
 * password authentication, no keys.
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<string.h>
#include	<errno.h>
#include	<netdb.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<mdm.h>

/*******************************************************************************
 * CODE STARTS.
 ******************************************************************************/

/*!
 * Opens the connection.
 * \param d Connection descriptor, containing all the necessary data not
 * portable across drivers to handle this particular connection.
 * \param status The result of this operation will be stored here.
 */
void
mdm_driver_ssh2_open(
	mdm_connection_descriptor_t *d, mdm_operation_result_t *status
)
{
	mdm_driver_ssh2_data_t *data;
	mdm_driver_ssh2_options_t *options;
	int error = 0;
	socklen_t lon = sizeof(int);
	int socket_error;
	struct pollfd pollinfo;
	long socket_arg;
	
	data = (mdm_driver_ssh2_data_t *)d->data;
	options = (mdm_driver_ssh2_options_t *)d->options;

	/* Open socket. */
	data->s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(data->s == -1)
	{
		status->status = MDM_OP_ERROR;
		sprintf(
			status->status_message,
			"Could not allocate socket: %s", strerror(errno)
		);
		return;
	}
	/* Try to resolve hostname. */
#if MDM_DEBUG_MESSAGES > 0
		MDM_LOGDEBUG("Trying %s:%s.", options->host, options->service);
#endif
	error = getaddrinfo(options->host, options->service, NULL, &data->res);
	if(error)
	{
		status->status = MDM_OP_ERROR;
		sprintf(
			status->status_message,
			"Error resolving: %s", gai_strerror(error)
		);
		close(data->s);
		data->s = -1;
		return;
	}
	if(options->to_connect > 0)
	{
		/* Set non blocking for connect. */
		socket_arg = fcntl(data->s, F_GETFL, NULL);
		socket_arg |= O_NONBLOCK;
		fcntl(data->s, F_SETFL, socket_arg);
		pollinfo.fd = data->s;
		pollinfo.events = POLLIN | POLLRDNORM | POLLRDBAND | POLLPRI | POLLWRNORM;
		pollinfo.revents = pollinfo.events;
#if MDM_DEBUG_MESSAGES > 0
		MDM_LOGDEBUG("Waiting %d millis.", data->tv_connect.tv_usec / 1000);
#endif
	}
	socket_error = connect(data->s, data->res->ai_addr, data->res->ai_addrlen);
	if(socket_error < 0 && errno == EINPROGRESS)
	{
		switch(poll(&pollinfo, 1, data->tv_connect.tv_usec / 1000))
		{
			case -1:
				status->status = MDM_OP_ERROR;
				sprintf(status->status_message, "%s", strerror(errno));
				close(data->s);
				data->s = -1;
				return;
			case 0:
				status->status = MDM_OP_ERROR;
				sprintf(status->status_message, "Connection timeout.");
				close(data->s);
				data->s = -1;
				return;
			default:
				getsockopt(
					data->s, SOL_SOCKET, SO_ERROR, (void*)(&socket_error), &lon
				);
				if(socket_error)
				{
					status->status = MDM_OP_ERROR;
					sprintf(
						status->status_message,
						"Error connecting: %s", strerror(socket_error)
					);
					close(data->s);
					data->s = -1;
				}
				break;
		}
		/* Set to blocking mode again. */
		socket_arg = fcntl(data->s, F_GETFL, NULL);
		socket_arg &= (~O_NONBLOCK);
		fcntl(data->s, F_SETFL, socket_arg);
	} else if(socket_error < 0) {
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error connecting: %s", strerror(errno));
		close(data->s);
		data->s = -1;
		return;
	}
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Starting SSH2 negotiation.");
#endif
	/* Create a session instance and start it up. This will trade welcome
	 * banners, exchange keys, and setup crypto, compression, and MAC layers
	 */
	data->session = libssh2_session_init();
	if(libssh2_session_startup(data->session, data->s))
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Could not establish ssh connection");
		close(data->s);
		data->s = -1;
		return;
	}
	/* At this point we havn't authenticated. The first thing to do is check
	 * the hostkey's fingerprint against our known hosts Your app may have it
	 * hard coded, may go to a file, may present it to the user, that's your
	 * call.
	 */
	data->fingerprint = libssh2_hostkey_hash(
		data->session, LIBSSH2_HOSTKEY_HASH_SHA1
	);

	/* check what authentication methods are available */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("%s", libssh2_userauth_list(
		data->session, options->username, strlen(options->username)
	));
#endif

	/* We could authenticate via password */
	if(libssh2_userauth_password(
			data->session, options->username, options->password)
	)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Authentication by password failed");
		return;
	}
	/* Request a shell */
	if(!(data->channel = libssh2_channel_open_session(data->session)))
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Unable to open a session");
		return;
	}

	/* Some environment variables may be set,
	 * It's up to the server which ones it'll allow though
	 */
	/*libssh2_channel_setenv(channel, "FOO", "bar");*/

	/* Request a terminal with 'vanilla' terminal emulation
	 * See /etc/termcap for more options
	 */
	if(libssh2_channel_request_pty(data->channel, "vt100"))
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Failed requesting pty");
		return;
	}

	/* Open a SHELL on that pty */
	if(libssh2_channel_shell(data->channel))
	{
		status->status = MDM_OP_ERROR;
		sprintf(
			status->status_message, "Unable to request shell on allocated pty"
		);
		return;
	}
	/* At this point the shell can be interacted with using
	 * libssh2_channel_read()
	 * libssh2_channel_read_stderr()
	 * libssh2_channel_write()
	 * libssh2_channel_write_stderr()
	 *
	 * Blocking mode may be (en|dis)abled with: libssh2_channel_set_blocking()
	 * If the server send EOF, libssh2_channel_eof() will return non-0
	 * To send EOF to the server use: libssh2_channel_send_eof()
	 * A channel can be closed with: libssh2_channel_close()
	 * A channel can be freed with: libssh2_channel_free()
	 */
	libssh2_channel_set_blocking(data->channel, options->to_recv < 0);
	return;
}

/*!
 * Closes the connection.
 * \param d Connection descriptor, containing all the necessary data not
 * portable across drivers to handle this particular connection.
 * \param status The result of this operation will be stored here.
 */
void
mdm_driver_ssh2_close(
	mdm_connection_descriptor_t *d, mdm_operation_result_t *status
)
{
	mdm_driver_ssh2_data_t *data;
	
	data = (mdm_driver_ssh2_data_t *)d->data;
	if(data->s > -1)
	{
		if(data->channel)
		{
			libssh2_channel_close(data->channel);
			libssh2_channel_free(data->channel);
			data->channel = NULL;
		}
		libssh2_session_disconnect(data->session, "Bye Bye.");
		libssh2_session_free(data->session);
		close(data->s);
		data->s = -1;
	}
	if(data->res != NULL)
	{
		freeaddrinfo(data->res);
		data->res = NULL;
	}
	return;
}

/*!
 * Will setup the connection.
 * \param d Connection descriptor, containing all the necessary data not
 * portable across drivers to handle this particular connection.
 * \param opts Connection options.
 * \param status The result of this operation will be stored here.
 */
void
mdm_driver_ssh2_setup(
	mdm_connection_descriptor_t *d, 
	mdm_connection_options_t *opts, mdm_operation_result_t *status
)
{
	mdm_driver_ssh2_data_t *data;
	mdm_driver_ssh2_options_t *options;

	data = (mdm_driver_ssh2_data_t *)d->data;
	options = (mdm_driver_ssh2_options_t *)opts;

	memcpy(
		(void *)d->options, (void *)options, sizeof(mdm_driver_ssh2_options_t)
	);

	/* Set timeval struct for timeout for send/receive. */
	data->tv_sr.tv_sec = 0;
	if(options->to_recv < 0)
	{
		data->tv_sr.tv_usec = 0;
	} else {
		data->tv_sr.tv_usec = options->to_recv * 1000;
	}
	/* Set timeval struct for timeout for connect. */
	data->tv_connect.tv_sec = 0;
	if(options->to_connect < 1)
	{
		data->tv_connect.tv_usec = 0;
	} else {
		data->tv_connect.tv_usec = options->to_connect * 1000;
	}
	return;
}

/*!
 * Will send data through the connection. Will send \\r\\n after every 
 * buffer, so dont send it yourself.
 * \param d Connection descriptor, containing all the necessary data not
 * portable across drivers to handle this particular connection.
 * \param buffer Buffer to send.
 * \param bufflen Length of the buffer.
 * \param status The result of this operation will be stored here.
 */
void
mdm_driver_ssh2_send(
	mdm_connection_descriptor_t *d, const char *buffer,
	int bufflen, mdm_operation_result_t *status
)
{
	mdm_driver_ssh2_data_t *data;
	mdm_driver_ssh2_options_t *options;
#if MDM_DEBUG_MESSAGES > 0
	int i;
#endif

	data = (mdm_driver_ssh2_data_t *)d->data;
	options = (mdm_driver_ssh2_options_t *)d->options;

	memcpy(data->sbuffer, buffer, bufflen);
	data->sbuffer[bufflen] = '\r';
	data->sbuffer[bufflen + 1] = 0;
	data->sbufferlen = bufflen + 1;

	switch(libssh2_channel_write(data->channel, buffer, bufflen))
	{
		case LIBSSH2_ERROR_EAGAIN:
			break;
		default:
			break;
	}
	switch(libssh2_channel_write(data->channel, "\r", 1))
	{
		case LIBSSH2_ERROR_EAGAIN:
			break;
		default:
			break;
	}
#if MDM_DEBUG_MESSAGES > 0
	if(mdm_config_get_fulldebug())
	{
		for(i = 0; i < bufflen; i++)
		{
			MDM_LOGDEBUG(
				"|%c|%d|",
				buffer[i] <= 31 ? '.' : buffer[i], buffer[i]
			);
		}
	}
#endif
	return;
}

/*!
 * Will receive data through the connection.
 * \param d Connection descriptor, containing all the necessary data not
 * portable across drivers to handle this particular connection.
 * \param buffer Buffer to put data.
 * \param bufflen Maximum length of data to receive.
 * \param status The result of this operation will be stored here.
 */
void
mdm_driver_ssh2_recv(
	mdm_connection_descriptor_t *d, void *buffer,
	int *bufflen, mdm_operation_result_t *status
)
{
	mdm_driver_ssh2_data_t *data;
	mdm_driver_ssh2_options_t *options;
	int i;
	int datastart;
	char *buffpointer;
	double timetook;
	struct timeval time_start;
	struct timeval time_end;
	int oribufflen;
	
	gettimeofday(&time_start, NULL);
	data = (mdm_driver_ssh2_data_t *)d->data;
	options = (mdm_driver_ssh2_options_t *)d->options;
	oribufflen = *bufflen;
	
	do
	{
		*bufflen = libssh2_channel_read(data->channel, buffer, oribufflen);
		if(*bufflen != LIBSSH2_ERROR_EAGAIN) {
			break;
		} else {
			gettimeofday(&time_end, NULL);
			timetook = 
				((double)time_end.tv_sec - (double)time_start.tv_sec) + 
				(
					((double)time_end.tv_usec / (double)1000000) -
					((double)time_start.tv_usec / (double)1000000)
				)
			;
			if((timetook * (double)1000) > (double)options->to_recv) {
				*bufflen = 0;
				return;
			}
		}
		usleep(10000);
	} while(1);
	if(*bufflen < 0)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error in ssh read: %d", *bufflen);
		return;
	}
	switch(*bufflen)
	{
		case 0:
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "eof?");
			return;
		default:

			/* Check for echoed data. */
			if(
					options->echo_cancel &&
					data->sbufferlen &&
					strncmp((char *)buffer, data->sbuffer, data->sbufferlen) == 0
			) {
				/* If we only got echoed data, return. */
				if(*bufflen == data->sbufferlen)
				{
					*bufflen = 0;
					return;
				}
				/* If we got more data, strip the echoed data. */
				i = *bufflen - data->sbufferlen;
				datastart = data->sbufferlen;
			} else {
				i = *bufflen;
				datastart = 0;
			}
			buffpointer = (char *)buffer;
			memcpy(buffer, buffpointer + datastart, i);
			buffpointer[i] = 0;
#if MDM_DEBUG_MESSAGES > 0
			if(mdm_config_get_fulldebug())
			{
				for(i = 0; i < *bufflen; i++)
				{
					MDM_LOGDEBUG(
						"|%c|%d|",
						((char *)buffer)[i] <= 31
						? '.' : ((char *)buffer)[i], ((char *)buffer)[i]
					);
				}
			}
#endif
			break;
	}
	return;
}

/*!
 * Called by #mdm_connection_new to initialize a ssh2 connection.
 * \param c Connection.
 * \param status Where to store the result.
 */
void
mdm_driver_ssh2_new(mdm_connection_t *c, mdm_operation_result_t *status)
{
	mdm_driver_ssh2_data_t *ssh2_data;

	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif

	c->open = mdm_driver_ssh2_open;
	c->close = mdm_driver_ssh2_close;
	c->setup = mdm_driver_ssh2_setup;
	c->send = mdm_driver_ssh2_send;
	c->recv = mdm_driver_ssh2_recv;
	ssh2_data = (mdm_driver_ssh2_data_t *)c->descriptor.data;
	ssh2_data->s = -1;

	/* Done. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
	return;
}

/*!
 * Called by #mdm_connection_free to deinitialize a ssh2 connection.
 * \param c Connection.
 */
void
mdm_driver_ssh2_free(mdm_connection_t *c)
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
 * CODE ENDS.
 ******************************************************************************/
