/*!
 * \file mdm_driver_telnet.c This is a telnet driver. Uses libtelnet.
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<string.h>
#include	<errno.h>
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<netdb.h>
#include	<poll.h>
#include	<fcntl.h>
#include	<mdm.h>

/*******************************************************************************
 * CODE STARTS.
 ******************************************************************************/

/*!
 * Standard telnet options we use.
 */
static const telnet_telopt_t telopts[] = {
	{ TELNET_TELOPT_ECHO,				TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_TTYPE,				TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_COMPRESS2,			TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_MSSP,				TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_BINARY,				TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_RCP,					TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_SGA,					TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_NAMS,				TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_STATUS,				TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_TM,					TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_RCTE,				TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_NAOL,				TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_NAOP,				TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_NAOCRD,				TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_NAOHTS,				TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_NAOHTD,				TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_NAOFFD,				TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_NAOVTS,				TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_NAOVTD,				TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_NAOLFD,				TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_XASCII,				TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_LOGOUT,				TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_BM,					TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_DET,					TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_SUPDUP,				TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_SUPDUPOUTPUT,		TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_SNDLOC,				TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_EOR,					TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_TUID,				TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_OUTMRK,				TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_TTYLOC,				TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_3270REGIME,		TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_X3PAD,				TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_NAWS,				TELNET_WILL, TELNET_DONT },
	{ TELNET_TELOPT_TSPEED,				TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_LFLOW,				TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_LINEMODE,			TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_XDISPLOC,			TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_ENVIRON,			TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_AUTHENTICATION,	TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_NEW_ENVIRON,		TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_ENCRYPT,			TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_ZMP,					TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_EXOPL,				TELNET_WONT, TELNET_DONT },
	{ TELNET_TELOPT_MCCP2,				TELNET_WONT, TELNET_DONT },
	{ -1, 0, 0 }
};


/*!
 * Called by #_event_handler to actually send the data.
 * \param sock Socket descriptor.
 * \param buffer Buffer to send.
 * \param size Buffer size.
 */
static void
_send(int sock, const char *buffer, size_t size)
{
	int rs;

	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif

	/* send data */
	while(size > 0)
	{
		if((rs = send(sock, buffer, size, 0)) == -1) {
			MDM_LOGERROR("send() failed: %s\n", strerror(errno));
			break;
		} else if (rs == 0) {
			MDM_LOGERROR("send() unexpectedly returned 0\n");
			break;
		}
		/* update pointer and size to see if we've got more to send */
		buffer += rs;
		size -= rs;
	}

	/* Done. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
}

/*!
 * This one is called automatically by libtelnet when any events arrive.
 * \param telnet libtelnet struct, from #mdm_driver_telnet_data_t.telnet.
 * \param ev libtelnet event.
 * \param user_data A #mdm_connection_descriptor_t.
 */
static void
_event_handler(telnet_t *telnet, telnet_event_t *ev, void *user_data)
{
	mdm_connection_descriptor_t *d = (mdm_connection_descriptor_t *)user_data;
	mdm_driver_telnet_data_t *data;
	mdm_driver_telnet_options_t *options;
	int i;
	int datastart;
	char *traversed_buffer;
	
	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif

	data = (mdm_driver_telnet_data_t *)d->data;
	options = (mdm_driver_telnet_options_t *)d->options;
	
	switch (ev->type)
	{
		/* Data received */
		case TELNET_EV_DATA:
#if MDM_DEBUG_MESSAGES > 0
			MDM_LOGDEBUG("TELNET_EV_DATA (%d) |%s|", ev->size, ev->buffer);
			if(mdm_config_get_fulldebug())
			{
				for(i = 0; i < ev->size; i++)
				{
					MDM_LOGDEBUG(
						"|%c|%d|",
						ev->buffer[i] <= 31 ? '.' : ev->buffer[i], ev->buffer[i]
					);
				}
			}
#endif

			/* Check for echoed data. */
			if(
					options->echo_cancel &&
					data->sbufferlen &&
					strncmp(ev->buffer, data->sbuffer, data->sbufferlen) == 0
			) {
				/* If we only got echoed data, return. */
				if(ev->size == data->sbufferlen)
					break;
				/* If we got more data, strip the echoed data + \13\10. */
				i = ev->size - data->sbufferlen - 2;
				datastart = data->sbufferlen + 2;
				/* Try to take out empty lines from echoed data. */
#if MDM_DEBUG_MESSAGES > 0
				MDM_LOGDEBUG("Got echoed data: %d %d", i, datastart);
#endif
			} else {
				i = ev->size;
				datastart = 0;
			}
			/* Copy arrived data to buffer in connection descriptor. */
			memcpy(data->buffer, ev->buffer + datastart, i);
			data->bufflen = i;
			data->received = 1;
	
			/* Convert all \13\0 into \13\10 */
			traversed_buffer = strchr(data->buffer, 0);
			while(
				traversed_buffer != NULL &&
				traversed_buffer < (data->buffer + data->bufflen)
			)
			{
				*traversed_buffer = 10;
				traversed_buffer++;
				traversed_buffer = strchr(traversed_buffer, 0);
			}
			data->buffer[data->bufflen] = 0;
			
			/* No more echoed data. */
			data->sbufferlen = 0;
			break;
		/* data must be sent */
		case TELNET_EV_SEND:
#if MDM_DEBUG_MESSAGES > 0
			MDM_LOGDEBUG("TELNET_EV_SEND");
			/* Debug output. */
			if(mdm_config_get_fulldebug())
			{
				for(i = 0; i < data->sbufferlen; i++)
				{
					MDM_LOGDEBUG(
						"|%c|%d|",
						data->sbuffer[i] <= 31
							? '.' : data->sbuffer[i], data->sbuffer[i]
					);
				}
			}
#endif
			_send(data->s, ev->buffer, ev->size);
			break;
		/* request to enable remote feature (or receipt) */
		case TELNET_EV_WILL:
			/* we'll agree to turn off our echo if server wants us to stop */
#if MDM_DEBUG_MESSAGES > 0
			MDM_LOGDEBUG("TELNET_EV_WILL");
#endif
			break;
		/* notification of disabling remote feature (or receipt) */
		case TELNET_EV_WONT:
#if MDM_DEBUG_MESSAGES > 0
			MDM_LOGDEBUG("TELNET_EV_WONT");
#endif
			break;
		/* request to enable local feature (or receipt) */
		case TELNET_EV_DO:
			if(
				ev->telopt == TELNET_TELOPT_NAWS
			)
			{
				char buffer[5] = { 0, 80, 0, (char)254, (char)255};
				telnet_subnegotiation(telnet, TELNET_TELOPT_NAWS, buffer, 5);
			}
#if MDM_DEBUG_MESSAGES > 0
			MDM_LOGDEBUG("TELNET_EV_DO");
#endif
			break;
		/* demand to disable local feature (or receipt) */
		case TELNET_EV_DONT:
#if MDM_DEBUG_MESSAGES > 0
			MDM_LOGDEBUG("TELNET_EV_DONT");
#endif
			break;
		/* respond to particular subnegotiations */
		case TELNET_EV_SUBNEGOTIATION:
			/* if they just asked for our terminal type, response with it */
			/* respond with our terminal type */
#if MDM_DEBUG_MESSAGES > 0
			MDM_LOGDEBUG("TELNET_EV_SUBNEGOTIATION");
#endif
			if(
				ev->telopt == TELNET_TELOPT_TTYPE &&
				ev->argc >= 1 && ev->argv[0][0] == TELNET_TTYPE_SEND
			)
			{
				telnet_format_sb(
					telnet, TELNET_TELOPT_TTYPE, 1, TELNET_TTYPE_IS, "vt100"
				);
			}
			break;
		/* error */
		case TELNET_EV_ERROR:
			MDM_LOGERROR("libtelnet error: %s", ev->buffer);
		default:
			/* ignore */
			break;
	}
	/* Done. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
	return;
}

/*!
 * Opens the connection.
 * \param d Connection descriptor, containing all the necessary data not
 * portable across drivers to handle this particular connection.
 * \param status The result of this operation will be stored here.
 */
void
mdm_driver_telnet_open(
	mdm_connection_descriptor_t *d, mdm_operation_result_t *status
)
{
	mdm_driver_telnet_data_t *data;
	mdm_driver_telnet_options_t *options;
	int error = 0;
	socklen_t lon = sizeof(int);
	int socket_error;
	struct pollfd pollinfo;
	long socket_arg;
	
	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif

	data = (mdm_driver_telnet_data_t *)d->data;
	options = (mdm_driver_telnet_options_t *)d->options;

#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Opening %s:%s", options->host, options->service);
#endif
	
	/* Open socket. */
	data->s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(data->s == -1)
	{
		status->status = MDM_OP_ERROR;
		sprintf(
			status->status_message,
			"Could not allocate socket: %s", strerror(errno)
		);
		goto mdm_driver_telnet_open_done;
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
		goto mdm_driver_telnet_open_done;
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
	/* initialize telnet box */
	data->telnet = telnet_init(telopts, _event_handler, 0, d);
	if(data->telnet == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error initializing telnet.");
		close(data->s);
		data->s = -1;
		goto mdm_driver_telnet_open_done;
	}

	/* Done. */
mdm_driver_telnet_open_done:
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
	return;
}

/*!
 * Closes the connection.
 * \param d Connection descriptor, containing all the necessary data not
 * portable across drivers to handle this particular connection.
 * \param status The result of this operation will be stored here.
 */
void
mdm_driver_telnet_close(
	mdm_connection_descriptor_t *d, mdm_operation_result_t *status
)
{
	mdm_driver_telnet_data_t *data;

	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif

	data = (mdm_driver_telnet_data_t *)d->data;
	if(data->s > -1)
	{
		close(data->s);
		data->s = -1;
	}
	if(data->telnet != NULL)
	{
		telnet_free(data->telnet);
		data->telnet = NULL;
	}
	if(data->res != NULL)
	{
		freeaddrinfo(data->res);
		data->res = NULL;
	}

	/* Done. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
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
mdm_driver_telnet_setup(
	mdm_connection_descriptor_t *d, 
	mdm_connection_options_t *opts, mdm_operation_result_t *status
)
{
	mdm_driver_telnet_options_t *options;
	mdm_driver_telnet_data_t *data;

	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif

	data = (mdm_driver_telnet_data_t *)d->data;
	options = (mdm_driver_telnet_options_t *)opts;

	memcpy(
		(void *)d->options, (void *)options, sizeof(mdm_driver_telnet_options_t)
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
	/* Done. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
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
mdm_driver_telnet_send(
	mdm_connection_descriptor_t *d, const char *buffer,
	int bufflen, mdm_operation_result_t *status
)
{
	mdm_driver_telnet_data_t *data;
	mdm_driver_telnet_options_t *options;
	
	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	int i;
	MDM_LOGDEBUG("Start.");
#endif

	data = (mdm_driver_telnet_data_t *)d->data;
	options = (mdm_driver_telnet_options_t *)d->options;
	
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Sending: |%s|", buffer);
#endif

	memcpy(data->sbuffer, buffer, bufflen);
	/*
	 * This seems to be the best option by trial & error. Most implementors
	 * dont deal well with \r\n or \n\0.
	 */	
	data->sbuffer[bufflen] = 13;
	data->sbuffer[bufflen + 1] = 10;
	data->sbufferlen = bufflen;
#if MDM_DEBUG_MESSAGES > 0
	if(mdm_config_get_fulldebug())
	{
		for(i = 0; i < data->sbufferlen; i++)
		{
			MDM_LOGDEBUG(
				"|%c|%d|",
				data->sbuffer[i] <= 31 ? '.' : data->sbuffer[i], data->sbuffer[i]
			);
		}
	}
#endif

	telnet_send(
		data->telnet, data->sbuffer,
		data->sbufferlen + (options->send_eol ? 2 : 0)
	);

	/* Done. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
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
mdm_driver_telnet_recv(
	mdm_connection_descriptor_t *d, void *buffer,
	int *bufflen, mdm_operation_result_t *status
)
{
	mdm_driver_telnet_data_t *data;
	mdm_driver_telnet_options_t *options;
	struct pollfd pollinfo;

	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	int i;
	MDM_LOGDEBUG("Start.");
#endif

	data = (mdm_driver_telnet_data_t *)d->data;
	options = (mdm_driver_telnet_options_t *)d->options;
	
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
			goto mdm_driver_telnet_recv_done;
		default:
			*bufflen = recv(data->s, buffer, *bufflen, 0);
			if(*bufflen < 0)
			{
				status->status = MDM_OP_ERROR;
				sprintf(status->status_message, "%s", strerror(errno));
				goto mdm_driver_telnet_recv_done;
			} else if(*bufflen == 0) {
				status->status = MDM_OP_ERROR;
				sprintf(status->status_message, "eof?");
				goto mdm_driver_telnet_recv_done;
			} else {
#if MDM_DEBUG_MESSAGES > 0
				if(mdm_config_get_fulldebug())
				{
					for(i = 0; i < data->sbufferlen; i++)
					{
						MDM_LOGDEBUG(
							"|%c|%d|",
							data->sbuffer[i] <= 31 ?
								'.' : data->sbuffer[i], data->sbuffer[i]
						);
					}
				}
#endif
				data->received = 0;
				telnet_recv(data->telnet, buffer, *bufflen);
				if(data->received)
				{
					memcpy(buffer, data->buffer, data->bufflen + 1);
					*bufflen = data->bufflen;
				} else {
					*bufflen = MDM_TELNET_DRIVER_ONLY_TELNET_DATA;
				}
			}
			break;
	}
	/* Done. */
mdm_driver_telnet_recv_done:
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
	return;
}

/*!
 * Called by #mdm_connection_new to initialize a telnet connection.
 * \param c Connection.
 * \param status Where to store the result.
 */
void
mdm_driver_telnet_new(mdm_connection_t *c, mdm_operation_result_t *status)
{
	mdm_driver_telnet_data_t *telnet_data;

	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif

	c->open = mdm_driver_telnet_open;
	c->close = mdm_driver_telnet_close;
	c->setup = mdm_driver_telnet_setup;
	c->send = mdm_driver_telnet_send;
	c->recv = mdm_driver_telnet_recv;
	telnet_data = (mdm_driver_telnet_data_t *)c->descriptor.data;
	telnet_data->s = -1;

	/* Done. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
	return;
}

/*!
 * Called by #mdm_connection_free to deinitialize a telnet connection.
 * \param c Connection.
 */
void
mdm_driver_telnet_free(mdm_connection_t *c)
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
