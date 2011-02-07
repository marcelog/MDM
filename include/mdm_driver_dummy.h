/*!
 * \file mdm_driver_dummy.h This is a dummy driver interface for testing.
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#ifndef	_MDM_DRIVER_DUMMY_H_
#define	_MDM_DRIVER_DUMMY_H_

#include	<mdm_connection.h>

/*!
 * This is the dummy options we use. Just an example of what you could do.
 */
typedef struct
{
	char option[128]; /*!< Simulated option. */
} mdm_driver_dummy_options_t;

extern void mdm_driver_dummy_open(
	mdm_connection_descriptor_t *, mdm_operation_result_t *
);
extern void mdm_driver_dummy_close(
	mdm_connection_descriptor_t *, mdm_operation_result_t *
);
extern void mdm_driver_dummy_setup(
	mdm_connection_descriptor_t *, mdm_connection_options_t *,
	mdm_operation_result_t *
);
extern void mdm_driver_dummy_send(
	mdm_connection_descriptor_t *, const char *,
	int, mdm_operation_result_t *
);

extern void mdm_driver_dummy_recv(
	mdm_connection_descriptor_t *, void *,
	int *, mdm_operation_result_t *
);

extern void mdm_driver_dummy_new(mdm_connection_t *, mdm_operation_result_t *);
extern void mdm_driver_dummy_free(mdm_connection_t *);

#endif
