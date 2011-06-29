/*!
 * \file mdm_driver_rawtcp.c This is a raw driver for tcp connections.
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<stdio.h>
#include	<string.h>
#include	<unistd.h>
#include	<netinet/in.h>
#include	<netinet/tcp.h>
#include	<errno.h>
#include	<poll.h>
#include	<netdb.h>
#include	<fcntl.h>
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
mdm_driver_rawtcp_open(
	mdm_connection_descriptor_t *d, mdm_operation_result_t *status
)
{
	mdm_driver_rawtcp_data_t *data;
	mdm_driver_rawtcp_options_t *options;
	long socket_arg;
	int error = 0;
	socklen_t lon = sizeof(int);
	int socket_error;
	struct pollfd pollinfo;

	data = (mdm_driver_rawtcp_data_t *)d->data;
	options = (mdm_driver_rawtcp_options_t *)d->options;

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
	}
	socket_error = connect(data->s, data->res->ai_addr, data->res->ai_addrlen);
	if(socket_error < 0 && errno == EINPROGRESS)
	{
#if MDM_DEBUG_MESSAGES > 0
		MDM_LOGDEBUG("Waiting %d millis.", data->tv_connect.tv_usec / 1000);
#endif
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
		socket_arg = 1;
	    if (setsockopt(data->s, IPPROTO_TCP, TCP_NODELAY, &socket_arg, sizeof(long)) == -1) {
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "Error setting nodelay: %s", strerror(errno));
			close(data->s);
			data->s = -1;
	    }
	    socket_arg = 1048576;
	    if (setsockopt(data->s, SOL_SOCKET, SO_SNDBUF, &socket_arg, sizeof(long)) == -1) {
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "Error setting sndbuf: %s", strerror(errno));
			close(data->s);
			data->s = -1;
	    }
	} else if(socket_error < 0) {
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error connecting: %s", strerror(errno));
		close(data->s);
		data->s = -1;
		return;
	}
	return;
}

/*!
 * Closes the connection.
 * \param d Connection descriptor, containing all the necessary data not
 * portable across drivers to handle this particular connection.
 * \param status The result of this operation will be stored here.
 */
void
mdm_driver_rawtcp_close(
	mdm_connection_descriptor_t *d, mdm_operation_result_t *status
)
{
	mdm_driver_rawtcp_data_t *data;

	data = (mdm_driver_rawtcp_data_t *)d->data;
	if(data->s > -1)
	{
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
mdm_driver_rawtcp_setup(
	mdm_connection_descriptor_t *d, 
	mdm_connection_options_t *opts, mdm_operation_result_t *status
)
{
	mdm_driver_rawtcp_options_t *options;
	mdm_driver_rawtcp_data_t *data;

	data = (mdm_driver_rawtcp_data_t *)d->data;
	options = (mdm_driver_rawtcp_options_t *)opts;

	memcpy(
		(void *)d->options, (void *)options, sizeof(mdm_driver_rawtcp_options_t)
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
 * Will send data through the connection.
 * \param d Connection descriptor, containing all the necessary data not
 * portable across drivers to handle this particular connection.
 * \param buffer Buffer to send.
 * \param bufflen Length of the buffer.
 * \param status The result of this operation will be stored here.
 */
void
mdm_driver_rawtcp_send(
	mdm_connection_descriptor_t *d, const char *buffer,
	int bufflen, mdm_operation_result_t *status
)
{
	mdm_driver_rawtcp_data_t *data;
	int i;
	int sent = 0;
	data = (mdm_driver_rawtcp_data_t *)d->data;
	while(sent != bufflen)
	{
		i = send(data->s, buffer + sent, bufflen - sent, 0);
		if (i < 1) {
			if (errno == 35) {
				usleep(100000);
				continue; // try again
			}
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "%d:%s", errno, strerror(errno));
			MDM_LOGDEBUG("%s", status->status_message);
			break;
		}
		sent += i;
		MDM_LOGDEBUG("Sent: %d/%d", sent, bufflen);
	}
	return;
}

/*!
 * Will receive data through the connection. On non-blocking mode, will
 * return 0 if no data is available.
 * \param d Connection descriptor, containing all the necessary data not
 * portable across drivers to handle this particular connection.
 * \param buffer Buffer to put data.
 * \param bufflen Maximum length of data to receive.
 * \param status The result of this operation will be stored here.
 */
void
mdm_driver_rawtcp_recv(
	mdm_connection_descriptor_t *d, void *buffer,
	int *bufflen, mdm_operation_result_t *status
)
{
	mdm_driver_rawtcp_data_t *data;
	mdm_driver_rawtcp_options_t *options;
	struct pollfd pollinfo;

	data = (mdm_driver_rawtcp_data_t *)d->data;
	options = d->options;
	
	pollinfo.fd = data->s;
	pollinfo.events = POLLIN | POLLRDNORM | POLLRDBAND | POLLPRI;
	pollinfo.revents = POLLIN | POLLRDNORM | POLLRDBAND | POLLPRI;

	switch(poll(
		&pollinfo, 1, options->to_recv >= 0 ? data->tv_sr.tv_usec / 1000 : -1
	))
	{
		case 0:
			*bufflen = 0;
			break;
		case -1:
			*bufflen = -1;
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "%s", strerror(errno));
			return;
		default:
			*bufflen = recv(data->s, buffer, *bufflen, 0);
			if(*bufflen < 0)
			{
				status->status = MDM_OP_ERROR;
				sprintf(status->status_message, "%s", strerror(errno));
				return;
			} else if(*bufflen == 0) {
				status->status = MDM_OP_ERROR;
				sprintf(status->status_message, "eof?");
				return;
			}
			break;
	}
	return;
}

/*!
 * Called by #mdm_connection_new to initialize a raw tcp connection.
 * \param c Connection.
 * \param status Where to store the result.
 */
void
mdm_driver_rawtcp_new(mdm_connection_t *c, mdm_operation_result_t *status)
{
	mdm_driver_rawtcp_data_t *rawtcp_data;

	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif

	c->open = mdm_driver_rawtcp_open;
	c->close = mdm_driver_rawtcp_close;
	c->setup = mdm_driver_rawtcp_setup;
	c->send = mdm_driver_rawtcp_send;
	c->recv = mdm_driver_rawtcp_recv;
	rawtcp_data = (mdm_driver_rawtcp_data_t *)c->descriptor.data;
	rawtcp_data->s = -1;

	/* Done. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
	return;
}

/*!
 * Called by #mdm_connection_free to deinitialize a raw tcp connection.
 * \param c Connection.
 */
void
mdm_driver_rawtcp_free(mdm_connection_t *c)
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
/*******************************************************************************
 * CODE ENDS.
 ******************************************************************************/
