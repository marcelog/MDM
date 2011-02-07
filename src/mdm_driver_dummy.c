/*!
 * \file mdm_driver_dummy.c This is a dummy driver for testing.
 * Will always return true.
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#include	<unistd.h>
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
mdm_driver_dummy_open(
	mdm_connection_descriptor_t *d, mdm_operation_result_t *status
)
{
	return;
}

/*!
 * Closes the connection.
 * \param d Connection descriptor, containing all the necessary data not
 * portable across drivers to handle this particular connection.
 * \param status The result of this operation will be stored here.
 */
void
mdm_driver_dummy_close(
	mdm_connection_descriptor_t *d, mdm_operation_result_t *status
)
{
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
mdm_driver_dummy_setup(
	mdm_connection_descriptor_t *d, 
	mdm_connection_options_t *opts, mdm_operation_result_t *status
)
{
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
mdm_driver_dummy_send(
	mdm_connection_descriptor_t *d, const char *buffer,
	int bufflen, mdm_operation_result_t *status
)
{
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
mdm_driver_dummy_recv(
	mdm_connection_descriptor_t *d, void *buffer,
	int *bufflen, mdm_operation_result_t *status
)
{
	return;
}

/*!
 * Called by #mdm_connection_new to initialize a dummy connection.
 * \param c Connection.
 * \param status Where to store the result.
 */
void
mdm_driver_dummy_new(mdm_connection_t *c, mdm_operation_result_t *status)
{
	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif

	c->open = mdm_driver_dummy_open;
	c->close = mdm_driver_dummy_close;
	c->setup = mdm_driver_dummy_setup;
	c->send = mdm_driver_dummy_send;
	c->recv = mdm_driver_dummy_recv;

	/* Done. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
	return;
}

/*!
 * Called by #mdm_connection_free to deinitialize a dummy connection.
 * \param c Connection.
 */
void
mdm_driver_dummy_free(mdm_connection_t *c)
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
