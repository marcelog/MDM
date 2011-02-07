/*!
 * \file mdm_driver_rawtcp_server.c This is a raw driver for tcp servers.
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#include	<stdio.h>
#include	<string.h>
#include	<unistd.h>
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
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
mdm_driver_rawtcp_server_open(
	mdm_connection_descriptor_t *d, mdm_operation_result_t *status
)
{
	mdm_driver_rawtcp_server_data_t *data;
	mdm_driver_rawtcp_server_options_t *options;
	socklen_t sockopt_len;
	long sockopt_val;
	int error = 0;
	struct linger lingering;
	
	data = (mdm_driver_rawtcp_server_data_t *)d->data;
	options = (mdm_driver_rawtcp_server_options_t *)d->options;

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
	/* Set lingering. */
	lingering.l_onoff = 1;
	lingering.l_linger = 50000;
	if(setsockopt(
		data->s, SOL_SOCKET, SO_LINGER, &lingering, sizeof(lingering)
	) == -1)
	{
		status->status = MDM_OP_ERROR;
		sprintf(
			status->status_message,
			"Error setting lingering: %s", strerror(errno)
		);
		close(data->s);
		data->s = -1;
		return;
	}

	/* Try to resolve hostname. */
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
	sockopt_val = 1;
	sockopt_len = sizeof(sockopt_val);
	if(options->reuse > 0)
	{
		if(setsockopt(
			data->s, SOL_SOCKET, SO_REUSEADDR, &sockopt_val, sockopt_len
		) == -1)
		{
			status->status = MDM_OP_ERROR;
			sprintf(
				status->status_message,
				"Error setting reuseaddr: %s", strerror(errno)
			);
			close(data->s);
			data->s = -1;
			return;
		}
	}
	if(bind(data->s, data->res->ai_addr, data->res->ai_addrlen) < 0)
	{
			status->status = MDM_OP_ERROR;
			sprintf(
				status->status_message,
				"Error binding: %s", strerror(errno)
			);
			close(data->s);
			data->s = -1;
			return;
	}
	if(listen(data->s, options->backlog))
	{
			status->status = MDM_OP_ERROR;
			sprintf(
				status->status_message,
				"Error listening: %s", strerror(errno)
			);
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
mdm_driver_rawtcp_server_close(
	mdm_connection_descriptor_t *d, mdm_operation_result_t *status
)
{
	mdm_driver_rawtcp_server_data_t *data;

	data = (mdm_driver_rawtcp_server_data_t *)d->data;
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
mdm_driver_rawtcp_server_setup(
	mdm_connection_descriptor_t *d, 
	mdm_connection_options_t *opts, mdm_operation_result_t *status
)
{
	mdm_driver_rawtcp_server_options_t *options;
	mdm_driver_rawtcp_server_data_t *data;

	data = (mdm_driver_rawtcp_server_data_t *)d->data;
	options = (mdm_driver_rawtcp_server_options_t *)opts;

	memcpy(
		(void *)d->options, (void *)options,
		sizeof(mdm_driver_rawtcp_server_options_t)
	);

	/* Set timeval struct for timeout. */
	data->tv.tv_sec = 0;
	if(options->to_accept == 0)
	{
		data->tv.tv_usec = 0;
	} else if(options->to_accept > 0) {
		data->tv.tv_usec = options->to_accept * 1000;
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
mdm_driver_rawtcp_server_send(
	mdm_connection_descriptor_t *d, const char *buffer,
	int bufflen, mdm_operation_result_t *status
)
{
	return;
}

/*!
 * Will receive a new client.On non-blocking mode, will return 0 on bufflen
 * if no connection is available. buffer param will be filled with a new
 * #mdm_connection_t of type raw tcp.
 * \param d Connection descriptor, containing all the necessary data not
 * portable across drivers to handle this particular connection.
 * \param buffer Buffer to put data, should be a #mdm_connection_t.
 * \param bufflen Maximum length of data to receive, should be a
 * sizeof(#mdm_connection_t)
 * \param status The result of this operation will be stored here.
 */
void
mdm_driver_rawtcp_server_recv(
	mdm_connection_descriptor_t *d, void *buffer,
	int *bufflen, mdm_operation_result_t *status
)
{
	mdm_driver_rawtcp_server_options_t *options;
	mdm_driver_rawtcp_server_data_t *data;
	int socket_error;
	mdm_connection_t client;
	struct sockaddr_in client_inaddr;
	long socket_arg;
	struct pollfd pollinfo;
	socklen_t lon = sizeof(int);
	socklen_t client_inaddr_len = sizeof(struct sockaddr_in);
	mdm_driver_rawtcp_data_t *client_data;
	mdm_driver_rawtcp_options_t *client_options;
	
	data = (mdm_driver_rawtcp_server_data_t *)d->data;
	options = (mdm_driver_rawtcp_server_options_t *)d->options;

	/* Set non blocking for accept. */
	socket_arg = fcntl(data->s, F_GETFL, NULL);
	socket_arg |= O_NONBLOCK;
	fcntl(data->s, F_SETFL, socket_arg);
	pollinfo.fd = data->s;
	pollinfo.events = POLLIN | POLLRDNORM | POLLRDBAND | POLLPRI | POLLWRNORM;
	pollinfo.revents = pollinfo.events;

	socket_error = accept(
		data->s, (struct sockaddr *)&client_inaddr, &client_inaddr_len
	);
	if(socket_error < 0 && errno == EWOULDBLOCK)
	{
#if MDM_DEBUG_MESSAGES > 0
		MDM_LOGDEBUG(
			"Waiting %d millis.",
			options->to_accept >= 0 ? data->tv.tv_usec / 1000 : -1
		);
#endif
		/* We 'emulate' blocking accept by polling with -1 */
		switch(poll(
			&pollinfo, 1, options->to_accept >= 0 ? data->tv.tv_usec / 1000 : -1
		))
		{
			case -1:
				status->status = MDM_OP_ERROR;
				sprintf(status->status_message, "%s", strerror(errno));
				close(data->s);
				data->s = -1;
				return;
			case 0:
#if MDM_DEBUG_MESSAGES > 0
				MDM_LOGDEBUG("Accept timeout.");
				*bufflen = 0;
#endif
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
						"Error accepting 1: %s", strerror(errno)
					);
					close(data->s);
					data->s = -1;
					return;
				}
				socket_error = accept(
					data->s, (struct sockaddr *)&client_inaddr, &client_inaddr_len
				);
				break;
		}
		/* Set to blocking mode again. */
		socket_arg = fcntl(data->s, F_GETFL, NULL);
		socket_arg &= (~O_NONBLOCK);
		fcntl(data->s, F_SETFL, socket_arg);
	} else if(socket_error < 0) {
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error accepting 2: %s", strerror(errno));
		close(data->s);
		data->s = -1;
		return;
	}
	/* New connection. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Accepting new connection: %d.", socket_error);
#endif
	/* Allocate a connection of type raw tcp. */
	mdm_connection_new(&client, MDM_CONNECTION_RAW_TCP, status);
	if(status->status == MDM_OP_ERROR)
	{
		close(socket_error);
		return;
	}
	/* Setup connection information. */
	client_data = (mdm_driver_rawtcp_data_t *)client.descriptor.data;
	client_options = (mdm_driver_rawtcp_options_t *)client.descriptor.options;

	/* Write internal data. */
	client_data->s = socket_error;
	client_data->tv_connect.tv_usec = 0;
	client_data->tv_connect.tv_sec = 0;
	client_data->res = NULL;
	
	/* Set timeval struct for timeout for send/receive. */
	client_options->to_connect = -1;
	client_options->to_recv = options->clients_to_recv;
	client_data->tv_sr.tv_sec = 0;
	if(client_options->to_recv < 0)
	{
		client_data->tv_sr.tv_usec = 0;
	} else {
		client_data->tv_sr.tv_usec = client_options->to_recv * 1000;
	}
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG(
		"New connection: %s:%d.",
		inet_ntoa(client_inaddr.sin_addr), ntohs(client_inaddr.sin_port)		
	);
#endif
	/* Fill client information. */
	sprintf(client_options->host, "%s", inet_ntoa(client_inaddr.sin_addr));
	sprintf(client_options->service, "%d", ntohs(client_inaddr.sin_port));
	
	/* Copy connection to buffer. */
	memcpy(buffer, &client, *bufflen);
	
	/* Done!. */
	return;
}

/*!
 * Called by #mdm_connection_new to initialize a raw tcp server.
 * \param c Connection.
 * \param status Where to store the result.
 */
void
mdm_driver_rawtcp_server_new(
	mdm_connection_t *c, mdm_operation_result_t *status
)
{
	mdm_driver_rawtcp_server_data_t *rawtcp_server_data;

	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif

	c->open = mdm_driver_rawtcp_server_open;
	c->close = mdm_driver_rawtcp_server_close;
	c->setup = mdm_driver_rawtcp_server_setup;
	c->send = mdm_driver_rawtcp_server_send;
	c->recv = mdm_driver_rawtcp_server_recv;
	rawtcp_server_data = (mdm_driver_rawtcp_server_data_t *)c->descriptor.data;
	rawtcp_server_data->s = -1;

	/* Done. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
	return;
}

/*!
 * Called by #mdm_connection_free to deinitialize a raw tcp server.
 * \param c Connection.
 */
void
mdm_driver_rawtcp_server_free(mdm_connection_t *c)
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
