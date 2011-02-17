/*!
 * \file mdm_driver_serial.c This is a serial driver interface. No hardware or
 * software control supported, all connections will be made with hardware and
 * software control off.
 * setup().
 * \author Marcelo Gornstein <marcelog@gmail.com>
 * see http://www.it.freebsd.org/pub/linux/distributions/slackware/slackware-3.6/docs/mini/Serial-Port-Programming
 */
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<errno.h>
#include	<poll.h>
#include	<mdm.h>

/*
    No parity (8N1):

     options.c_cflag &= ~PARENB;
     options.c_cflag &= ~CSTOPB;
     options.c_cflag &= ~CSIZE;
     options.c_cflag |= CS8;

    Even parity (7E1):

     options.c_cflag |= PARENB;
     options.c_cflag &= ~PARODD;
     options.c_cflag &= ~CSTOPB;
     options.c_cflag &= ~CSIZE;
     options.c_cflag |= CS7;

    Odd parity (7O1):

     options.c_cflag |= PARENB;
     options.c_cflag |= PARODD;
     options.c_cflag &= ~CSTOPB;
     options.c_cflag &= ~CSIZE;
     options.c_cflag |= CS7;

    Mark parity is simulated by using 2 stop bits (7M1):

     options.c_cflag &= ~PARENB;
     options.c_cflag |= CSTOPB;
     options.c_cflag &= ~CSIZE;
     options.c_cflag |= CS7;

    Space parity is setup the same as no parity (7S1):

     options.c_cflag &= ~PARENB;
     options.c_cflag &= ~CSTOPB;
     options.c_cflag &= ~CSIZE;
     options.c_cflag |= CS8;

 12. POSIX control mode flags

 The following table lists the possible control modes for c_cflag.

   Constant     Description
   ________________________

   CBAUD        Bit mask for baud rate
   B0           0 baud (drop DTR)
   B50          50 baud
   B75          75 baud
   B110         110 baud
   B134         134.5 baud
   B150         150 baud
   B200         200 baud
   B300         300 baud
   B600         600 baud
   B1200        1200 baud
   B1800        1800 baud
   B2400        2400 baud
   B4800        4800 baud
   B9600        9600 baud
   B19200       19200 baud
   B38400       38400 baud
   EXTA         External rate clock
   EXTB         External rate clock
   CSIZE        Bit mask for data bits
   CS5          5 data bits
   CS6          6 data bits
   CS7          7 data bits
   CS8          8 data bits
   CSTOPB       2 stop bits (1 otherwise)
   CREAD        Enable receiver
   PARENB       Enable parity bit
   PARODD       Use odd parity instead of even
   HUPCL        Hangup (drop DTR) on last close
   CLOCAL       Local line - do not change 'owner' of port
   LOBLK        Block job control output
   CRTSCTS      Enable hardware flow control (not supported on all platforms)

 13. POSIX tcsetattr Constants

   Constant   Description
   ______________________

   TCSANOW    Make changes now without waiting for data to complete
   TCSADRAIN  Wait until everything has been transmitted
   TCSAFLUSH  Flush input and output buffers and make the change

 14. POSIX Local Mode Constants
   Constant   Description
   ______________________

   ISIG       Enable SIGINTR, SIGSUSP, SIGDSUSP, and SIGQUIT signals
   ICANON     Enable canonical input (else raw)
   XCASE      Map uppercase \lowercase (obselete)
   ECHO       Enable echoing of input characters
   ECHOE      Echo erase character as BS-SP-BS
   ECHOK      Echo NL after kill character
   ECHONL     Echo NL
   NOFLSH     Disable flushing of input buffers after interrupt
              or quit characters
   IEXTEN     Enable extended functions
   ECHOCTL    Echo control characters as ^char and delete as ~?
   ECHOPRT    Echo erased character as character erased
   ECHOKE     BS-SP-BS entire line on line kill
   FLUSHO     Output being flushed
   PENDIN     Retype pending input at next read or input char
   TOSTOP     Send SIGTTOU for background output

 15. POSIX Input Mode Constants

   Constant   Description
   ______________________

   INPCK      Enable parity check
   IGNPAR     Ignore parity errors
   PARMRK     Mark parity errors
   ISTRIP     Strip parity bits
   IXON       Enable software flow control (outgoing)
   IXOFF      Enable software flow control (incoming)
   IXANY      Allow any character to start flow again
   IGNBRK     Ignore break condition
   BRKINT     Send a SIGINT when a break condition is detected
   INLCR      Map NL to CR
   IGNCR      Ignore CR
   ICRNL      Map CR to NL
   IUCLC      Map uppercase to lowercase
   IMAXBEL    Echo BEL on input line too long

 16. POSIX Output Mode Constants

   Constant   Description
   ______________________

   OPOST      Postprocess output (not set = raw output)
   OLCUC      Map lowercase to uppercase
   ONLCR      Map NL to CR-NL
   OCRNL      Map CR to NL
   NOCR       No CR output at column 0
   ONLRET     NL performs CR function
   OFILL      Use fill characters for delay
   OFDEL      Fill character is DEL
   NLDLY      Mask for delay time needed between lines
   NL0        No delay for NLs
   NL1        Delay further output after newline for 100 milliseconds
   CRDLY      Mask for delay time needed to return carriage to left column
   CR0        No delay for CRs
   CR1        Delay after CRs depending on current column position
   CR2        Delay 100 milliseconds after sending CRs
   CR3        Delay 150 milliseconds after sending CRs
   TABDLY     Mask for delay time needed after TABs
   TAB0       No delay for TABs
   TAB1       Delay after TABs depending on current column position
   TAB2       Delay 100 milliseconds after sending TABs
   TAB3       Expand TAB characters to spaces
   BSDLY      Mask for delay time needed after BSs
   BS0        No delay for BSs
   BS1        Delay 50 milliseconds after sending BSs
   VTDLY      Mask for delay time needed after VTs
   VT0        No delay for VTs
   VT1        Delay 2 seconds after sending VTs
   FFDLY      Mask for delay time needed after FFs
   FF0        No delay for FFs
   FF1        Delay 2 seconds after sending FFs

 17. POSIX Control Character Constants

   Constant   Description          Key
   ______________________________________

   VINTR      Interrupt            CTRL-C
   VQUIT      Quit                 CTRL-Z
   VERASE     Erase                Backspace (BS)
   VKILL      Kill-line            CTRL-U
   VEOF       End-of-file          CTRL-D
   VEOL       End-of-line          Carriage return (CR)
   VEOL2      Second end-of-line   Line feed (LF)
   VMIN       Minimum number of characters to read
   VTIME      Time to wait for data (tenths of seconds)
*/

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
mdm_driver_serial_open(
	mdm_connection_descriptor_t *d, mdm_operation_result_t *status
)
{
	mdm_driver_serial_data_t *data;
	mdm_driver_serial_options_t *options;

	data = (mdm_driver_serial_data_t *)d->data;
	options = (mdm_driver_serial_options_t *)d->options;
	
	data->fd = open(options->port, O_RDWR | O_NOCTTY | O_NDELAY);
	if(data->fd < 0)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error open()ing: %s", strerror(errno));
		return;
	}
	fcntl(data->fd, F_SETFL, FNDELAY);
	/* Save old termios options. */
	tcgetattr(data->fd, &data->old_termios_options);
	tcgetattr(data->fd, &data->new_termios_options);

	switch(options->speed)
	{
		case MDM_DRIVER_SERIAL_300:
			cfsetispeed(&data->new_termios_options, B300);
			cfsetospeed(&data->new_termios_options, B300);
			break;
		case MDM_DRIVER_SERIAL_1200:
			cfsetispeed(&data->new_termios_options, B1200);
			cfsetospeed(&data->new_termios_options, B1200);
			break;
		case MDM_DRIVER_SERIAL_2400:
			cfsetispeed(&data->new_termios_options, B2400);
			cfsetospeed(&data->new_termios_options, B2400);
			break;
		case MDM_DRIVER_SERIAL_4800:
			cfsetispeed(&data->new_termios_options, B4800);
			cfsetospeed(&data->new_termios_options, B4800);
			break;
		case MDM_DRIVER_SERIAL_9600:
			cfsetispeed(&data->new_termios_options, B9600);
			cfsetospeed(&data->new_termios_options, B9600);
			break;
		case MDM_DRIVER_SERIAL_19200:
			cfsetispeed(&data->new_termios_options, B19200);
			cfsetospeed(&data->new_termios_options, B19200);
			break;
		case MDM_DRIVER_SERIAL_38400:
			cfsetispeed(&data->new_termios_options, B38400);
			cfsetospeed(&data->new_termios_options, B38400);
			break;
		case MDM_DRIVER_SERIAL_57600:
			cfsetispeed(&data->new_termios_options, B57600);
			cfsetospeed(&data->new_termios_options, B57600);
			break;
		case MDM_DRIVER_SERIAL_115200:
			cfsetispeed(&data->new_termios_options, B115200);
			cfsetospeed(&data->new_termios_options, B115200);
			break;
		default:
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "Invalid speed.");
			return;
	}

	switch(options->bits)
	{
		case MDM_DRIVER_SERIAL_8BITS:
			data->new_termios_options.c_cflag |= CS8;
			break;
		case MDM_DRIVER_SERIAL_7BITS:
			data->new_termios_options.c_cflag |= CS7;
			break;
		default:
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "Invalid bits.");
			return;
	}
	
	switch(options->stop_bits)
	{
		case MDM_DRIVER_SERIAL_SBITS_1:
			data->new_termios_options.c_cflag &= ~CSTOPB;
			break;
		case MDM_DRIVER_SERIAL_SBITS_2:
			data->new_termios_options.c_cflag |= CSTOPB;
			break;
		default:
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "Invalid stop bits.");
			return;
	}

	switch(options->parity)
	{
		case MDM_DRIVER_SERIAL_PNONE:
			data->new_termios_options.c_cflag &= ~PARENB;
			data->new_termios_options.c_cflag &= ~CSIZE;
			break;
		case MDM_DRIVER_SERIAL_PODD:
			data->new_termios_options.c_cflag |= PARENB;
			data->new_termios_options.c_cflag &= ~CSIZE;
			data->new_termios_options.c_cflag |= PARODD;
			break;
		case MDM_DRIVER_SERIAL_PEVEN:
			data->new_termios_options.c_cflag |= PARENB;
			data->new_termios_options.c_cflag &= ~CSIZE;
			data->new_termios_options.c_cflag &= ~PARODD;
			break;
		default:
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "Invalid parity.");
			return;
	}
	/* Disable hardware flow control */
	data->new_termios_options.c_cflag &= ~CRTSCTS;

	/* Enable the receiver and set local mode */
	/*	data->new_termios_options.c_cflag |= (CLOCAL | CREAD);*/

	/*
	 * Canonical input means that all incoming characters are placed in a buffer
	 * which may be edited by the user, until a carriage return or line feed
	 * (CR or LF) are received. Typically, you would use
	 *
	 * data->new_termios_options.c_cflag |= ~(ICANON | ECHO | ECHOE);
	 *
	 * Raw input is unprocessed, so they may be used as they are read.
	 * Our device sent raw data.
	 *
	 * data->new_termios_options.c_cflag &= ~(ICANON | ECHO | ISIG);
	 * Whether you use canonical or raw input, make sure you never enable input
	 * echo when connected to a computer/device which is echoing characters to
	 * you. Refer to section 14 for local mode constants.
	 */

	/* Set terminal to raw mode. */
	 cfmakeraw(&data->new_termios_options);

	/* Set parameters. */
	tcsetattr(data->fd, TCSANOW, &data->new_termios_options);
	write(data->fd, "\0\r", 2);
	return;
}

/*!
 * Closes the connection.
 * \param d Connection descriptor, containing all the necessary data not
 * portable across drivers to handle this particular connection.
 * \param status The result of this operation will be stored here.
 */
void
mdm_driver_serial_close(
	mdm_connection_descriptor_t *d, mdm_operation_result_t *status
)
{
	mdm_driver_serial_data_t *data;
	
	data = (mdm_driver_serial_data_t *)d->data;
	if(data->fd > -1)
	{
		/* Restablish old termios options. */
		tcsetattr(data->fd, TCSANOW, &data->old_termios_options);
		close(data->fd);
		data->fd = -1;
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
mdm_driver_serial_setup(
	mdm_connection_descriptor_t *d, 
	mdm_connection_options_t *opts, mdm_operation_result_t *status
)
{
	mdm_driver_serial_data_t *data;
	mdm_driver_serial_options_t *options;

	data = (mdm_driver_serial_data_t *)d->data;
	options = (mdm_driver_serial_options_t *)d->options;
	memcpy(
		(void *)d->options, (void *)opts, sizeof(mdm_driver_serial_options_t)
	);
	/* Set timeval struct for timeout. */
	data->tv.tv_usec = 0;
	if(options->to_recv < 0)
	{
		data->tv.tv_usec = 0;
	} else {
		data->tv.tv_usec = options->to_recv * 1000;
	}
	return;
}

/*!
 * Will send data through the connection. Will send \\n after every 
 * buffer, so dont send it yourself.
 * \param d Connection descriptor, containing all the necessary data not
 * portable across drivers to handle this particular connection.
 * \param buffer Buffer to send.
 * \param bufflen Length of the buffer.
 * \param status The result of this operation will be stored here.
 */
void
mdm_driver_serial_send(
	mdm_connection_descriptor_t *d, const char *buffer,
	int bufflen, mdm_operation_result_t *status
)
{
	mdm_driver_serial_data_t *data;
	mdm_driver_serial_options_t *options;

#if MDM_DEBUG_MESSAGES > 0
	int i;
	MDM_LOGDEBUG("Start.");
#endif
	data = (mdm_driver_serial_data_t *)d->data;
	options = (mdm_driver_serial_options_t *)d->options;
	
	memcpy(data->sbuffer, buffer, bufflen);
	data->sbuffer[bufflen] = '\n';
	data->sbuffer[bufflen + 1] = 0;
	data->sbufferlen = bufflen + 1;

	if(write(data->fd, buffer, bufflen) != bufflen)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "%s", strerror(errno));
		return;
	}
	if(write(data->fd, "\n", 1) != 1)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "%s", strerror(errno));
		return;
	}
#if MDM_DEBUG_MESSAGES > 0
	if(mdm_config_get_fulldebug())
	{
		for(i = 0; i < bufflen; i++)
		{
			MDM_LOGDEBUG(
				"|%c|%d|",
				buffer[i] <= 31 ?
					'.' :
					buffer[i], buffer[i]
			);
		}
	}
#endif
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
mdm_driver_serial_recv(
	mdm_connection_descriptor_t *d, void *buffer,
	int *bufflen, mdm_operation_result_t *status
)
{
	mdm_driver_serial_data_t *data;
	mdm_driver_serial_options_t *options;

	struct pollfd pollinfo;
	int i;
	int datastart;
	char *buffpointer;

	data = (mdm_driver_serial_data_t *)d->data;
	options = (mdm_driver_serial_options_t *)d->options;

	pollinfo.fd = data->fd;
	pollinfo.events = POLLIN | POLLRDNORM | POLLRDBAND | POLLPRI;
	pollinfo.revents = POLLIN | POLLRDNORM | POLLRDBAND | POLLPRI;

	switch(poll(
		&pollinfo, 1, options->to_recv >= 0 ? data->tv.tv_usec / 1000 : -1
	))
	{
		case 0:
			*bufflen = 0;
			break;
		case -1:
			*bufflen = -1;
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "%s", strerror(errno));
			break;
		default:
			*bufflen = read(data->fd, buffer, *bufflen);
			if(*bufflen < 1)
				return;
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
						((char *)buffer)[i] <= 31 ?
							'.' :
							((char *)buffer)[i], ((char *)buffer)[i]
					);
				}
			}
			break;
#endif
	}
	return;
}

/*!
 * Called by #mdm_connection_new to initialize a serial connection.
 * \param c Connection.
 * \param status Where to store the result.
 */
void
mdm_driver_serial_new(mdm_connection_t *c, mdm_operation_result_t *status)
{
	mdm_driver_serial_data_t *serial_data;

	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif

	c->open = mdm_driver_serial_open;
	c->close = mdm_driver_serial_close;
	c->setup = mdm_driver_serial_setup;
	c->send = mdm_driver_serial_send;
	c->recv = mdm_driver_serial_recv;
	serial_data = (mdm_driver_serial_data_t *)c->descriptor.data;
	serial_data->fd = -1;

	/* Done. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
	return;
}

/*!
 * Called by #mdm_connection_free to deinitialize a serial connection.
 * \param c Connection.
 */
void
mdm_driver_serial_free(mdm_connection_t *c)
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
