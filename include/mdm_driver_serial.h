/*!
 * \file mdm_driver_serial.h This is a serial driver interface. No hardware or
 * software control supported, all connections will be made with hardware and
 * software control off.
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#ifndef	_MDM_DRIVER_SERIAL_H_
#define	_MDM_DRIVER_SERIAL_H_

#include	<stdio.h>
#include	<termios.h>
#include	<mdm_connection.h>

typedef enum
{
	MDM_DRIVER_SERIAL_7BITS,
	MDM_DRIVER_SERIAL_8BITS
} mdm_serial_driver_bits_t;

typedef enum
{
	MDM_DRIVER_SERIAL_PEVEN,
	MDM_DRIVER_SERIAL_PODD,
	MDM_DRIVER_SERIAL_PNONE
} mdm_serial_driver_parity_t;

typedef enum
{
	MDM_DRIVER_SERIAL_SBITS_1,
	MDM_DRIVER_SERIAL_SBITS_2
} mdm_serial_driver_stop_bits_t;

typedef enum
{
	MDM_DRIVER_SERIAL_300,
	MDM_DRIVER_SERIAL_1200,
	MDM_DRIVER_SERIAL_2400,
	MDM_DRIVER_SERIAL_4800,
	MDM_DRIVER_SERIAL_9600,
	MDM_DRIVER_SERIAL_19200,
	MDM_DRIVER_SERIAL_38400,
	MDM_DRIVER_SERIAL_57600,
	MDM_DRIVER_SERIAL_115200
} mdm_serial_driver_speed_t;

/*!
 * This are the options we use.
 */
typedef struct
{
	char port[FILENAME_MAX]; /*!< Serial port filename. */
	mdm_serial_driver_speed_t speed; /*< Port speed. */
	mdm_serial_driver_bits_t bits; /*!< 7 or 8 bits. */
	mdm_serial_driver_parity_t parity; /*< Odd, Even, or None. */
	mdm_serial_driver_stop_bits_t stop_bits; /*< 1 or 2. */
	int to_recv; /*!< If blocking >= 0, wait at most this amount of milliseconds
		before timeout. If -1, will block. */
	int echo_cancel; /*!< Turn on echo cancellation. */
} mdm_driver_serial_options_t;

/*!
 * Internal driver data.
 */
typedef struct
{
	int fd; /*!< File descriptor. */
	struct termios old_termios_options; /*!< Saved termios for port at open(). */
	struct termios new_termios_options; /*!< Tweaked termios options
		after open(). */
	struct timeval tv; /*!< Used to control polling. */
	char sbuffer[65535]; /*!< Internal buffer to store last transmission so
		we can check against the reply and avoid bad use of echo. */
	int sbufferlen; /*!< Length of last transmission. */
} mdm_driver_serial_data_t;

extern void mdm_driver_serial_open(
	mdm_connection_descriptor_t *, mdm_operation_result_t *
);
extern void mdm_driver_serial_close(
	mdm_connection_descriptor_t *, mdm_operation_result_t *
);
extern void mdm_driver_serial_setup(
	mdm_connection_descriptor_t *, mdm_connection_options_t *,
	mdm_operation_result_t *
);
extern void mdm_driver_serial_send(
	mdm_connection_descriptor_t *, const char *,
	int, mdm_operation_result_t *
);

extern void mdm_driver_serial_recv(
	mdm_connection_descriptor_t *, void *,
	int *, mdm_operation_result_t *
);

extern void mdm_driver_serial_new(mdm_connection_t *, mdm_operation_result_t *);
extern void mdm_driver_serial_free(mdm_connection_t *);

#endif
