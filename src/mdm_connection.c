/*!
 * \file mdm_connection.c This file contains all the necessary code to implement
 * generic connections. All connection drivers will use these functions. You 
 * should use these functions which will provide you with a layer on top of
 * the connection drivers layer.
 *
 * \author Marcelo Gornstein <marcelog@gmail.com>
 * 
 */
#include	<time.h>
#include	<libssh2/libssh2.h>
#include	<unistd.h>
#include	<mdm.h>

/*!
 * This holds the strings associated to each of the supported connection
 * drivers.
 */
static const char *MDM_CONNECTION_TYPE_STRING[] = {
	"TELNET",  /*!< Telnet protocol connection. */                  
	"SSH1",    /*!< SSH1 protocol connection. */                    
	"SSH2",    /*!< SSH2 protocol connection. */                    
	"HTTP",    /*!< HTTP protocol connection. Uses CURL. */         
	"HTTPS",   /*!< HTTP over SSL protocol connection. Uses CURL. */
	"TCP_RAW", /*!< Raw TCP connection. */
	"TCP_RAW_SERVER", /*!< Raw TCP SERVER connection. */
	"SERIAL",  /*!< Serial RS-232 protocol connection. */           
	"SNMP1",   /*!< SNMPv1 protocol driver. */                      
	"SNMP2",   /*!< SNMPv2 protocol driver. */                      
	"SNMP3",   /*!< SNMPv3 protocol driver. */                      
	"DUMMY"    /*!< For testing, dummy driver, does nothing. */     
};

/*!
 * Checks for a valid connection type. Valid connections types are defined in
 * #mdm_connection_type_t and the corresponding strings are in 
 * #MDM_CONNECTION_TYPE_STRING. A valid connection type is contained between
 * values #MDM_CONNECTION_TELNET and #MDM_CONNECTION_DUMMY (lower and higher
 * values of the enum #mdm_connection_type_t).
 *
 * \param c Connection type to test.
 * \return 1 (true) if connection type is valid. 0 (false) otherwise.
 */
int
mdm_connection_isvalid_type(mdm_connection_type_t c)
{
	return (c >= MDM_CONNECTION_TELNET && c <= MDM_CONNECTION_DUMMY);
}

/*!
 * \param c Connection type.
 * \return NULL if c is an invalid connection type. Otherwise, a const char *
 * pointing to one of the elements of #MDM_CONNECTION_TYPE_STRING indicating
 * the connection type in human readable form.
 */
const char *
mdm_connection_2string(mdm_connection_type_t c)
{
	return mdm_connection_isvalid_type(c) ? MDM_CONNECTION_TYPE_STRING[c] : NULL;
}

/*!
 * Used to allocate a new generic connection.
 *
 * \param c Target connection struct.
 * \param t Connection type, used to select adequate driver and initial internal
 * setup.
 * \param status The result of the operation will be stored here.
 */
void
mdm_connection_new(
	mdm_connection_t *c, mdm_connection_type_t t, mdm_operation_result_t *status
)
{
	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif

	/* Init. */
	if(c == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Connection is null.");
		goto mdm_connection_new_done;
	}
	c->descriptor.data = NULL;
	c->descriptor.options = NULL;
	status->status = MDM_OP_OK;
	
	/* Check connection type. */
	if(!mdm_connection_isvalid_type(t))
	{
		status->status = MDM_OP_ERROR;
		sprintf(
			status->status_message, "Connection type is invalid: %d, aborting.", t
		);
		goto mdm_connection_new_done;
	}
	/* Ok, proceed. */
	c->type = t;
	
	/* Allocate data & descriptor. */
	c->descriptor.data = (mdm_driver_connection_data_t *)
		MDM_MALLOC(MDM_CONNECTION_MAX_DATA_SIZE)
	;
	if(c->descriptor.data == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error allocating memory for data");
		goto mdm_connection_new_done;
	}
	c->descriptor.options = (mdm_connection_options_t *)
		MDM_MALLOC(MDM_CONNECTION_MAX_OPTIONS_SIZE)
	;
	if(c->descriptor.options == NULL)
	{
		MDM_MFREE(&c->descriptor.data);
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error allocating memory for options");
		goto mdm_connection_new_done;
	}
	switch(t)
	{
		case MDM_CONNECTION_TELNET:
			mdm_driver_telnet_new(c, status);
			break;
		case MDM_CONNECTION_SSH1:
			break;
		case MDM_CONNECTION_SSH2:
			mdm_driver_ssh2_new(c, status);
			break;
		case MDM_CONNECTION_HTTP:
			break;
		case MDM_CONNECTION_HTTPS:
			break;
		case MDM_CONNECTION_RAW_TCP:
			mdm_driver_rawtcp_new(c, status);
			break;
		case MDM_CONNECTION_RAW_TCP_SERVER:
			mdm_driver_rawtcp_server_new(c, status);
			break;
		case MDM_CONNECTION_SERIAL:
			mdm_driver_serial_new(c, status);
			break;
		case MDM_CONNECTION_SNMP1:
			break;
		case MDM_CONNECTION_SNMP2:
			break;
		case MDM_CONNECTION_SNMP3:
			break;
		case MDM_CONNECTION_DUMMY:
			mdm_driver_dummy_new(c, status);
			break;
		default:
			status->status = MDM_OP_ERROR;
			sprintf(
				status->status_message,
				"Connection type is invalid: %d, aborting.", t
			);
			return;
	}

	/* Done. */
mdm_connection_new_done:
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
	return;
}
/*!
 * This is called to open the connection. Will internally call 
 * #mdm_connection_t.open with the #mdm_connection_descriptor_t of c.
 * \param c Connection to open, allocated with #mdm_connection_new.
 * \param options Connection options.
 * \param status The result of the operation will be stored here.
 */
void
mdm_connection_open(
	mdm_connection_t *c, mdm_connection_options_t *options,
	mdm_operation_result_t *status
)
{
	struct timeval time_start;
	struct timeval time_end;

	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif

	gettimeofday(&time_start, NULL);
	if((c != NULL) && (c->open != NULL)) {
		status->status = MDM_OP_OK;
		c->setup(&c->descriptor, options, status);
		if(status->status == MDM_OP_OK)
		{
			c->open(&c->descriptor, status);
		}
	} else {
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Connection or open is null.");
	}
	
	/* Done. */
	gettimeofday(&time_end, NULL);
	MDM_LOGPERF(
		"Connection Open took: %g secs.",
		((double)time_end.tv_sec - (double)time_start.tv_sec) + 
		(
			((double)time_end.tv_usec / (double)1000000) -
			((double)time_start.tv_usec / (double)1000000)
		)
	);
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
	return;
}

/*!
 * This is called to open the connection. Will internally call 
 * #mdm_connection_t.close with the #mdm_connection_descriptor_t of c.
 * \param c Connection to close, allocated with #mdm_connection_new.
 * \param status The result of the operation will be stored here.
 */
void
mdm_connection_close(
	mdm_connection_t *c, mdm_operation_result_t *status
)
{
	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif

	if((c != NULL) && (c->close != NULL)) {
		status->status = MDM_OP_OK;
		c->close(&c->descriptor, status);
	} else {
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Connection or close is null.");
	}
	/* Done. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
	return;
}

/*!
 * This will free the connection allocated by #mdm_connection_new
 * \param c Connection allocated by #mdm_connection_new
 */
void
mdm_connection_free(mdm_connection_t *c)
{
	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif

	if(c == NULL)
	{
		MDM_LOGWARN("Connection is null.");
		goto mdm_connection_free_done;
	}
	
	/* Check connection type. */
	if(!mdm_connection_isvalid_type(c->type))
	{
		MDM_LOGERROR("Connection type is invalid: %d, aborting.", c->type);
		goto mdm_connection_free_done;
	}

	switch(c->type)
	{
		case MDM_CONNECTION_TELNET:
			mdm_driver_telnet_free(c);
			break;
		case MDM_CONNECTION_SSH1:
			break;
		case MDM_CONNECTION_SSH2:
			mdm_driver_ssh2_free(c);
			break;
		case MDM_CONNECTION_HTTP:
			break;
		case MDM_CONNECTION_HTTPS:
			break;
		case MDM_CONNECTION_RAW_TCP:
			mdm_driver_rawtcp_free(c);
			break;
		case MDM_CONNECTION_RAW_TCP_SERVER:
			mdm_driver_rawtcp_server_free(c);
			break;
		case MDM_CONNECTION_SERIAL:
			mdm_driver_serial_free(c);
			break;
		case MDM_CONNECTION_SNMP1:
			break;
		case MDM_CONNECTION_SNMP2:
			break;
		case MDM_CONNECTION_SNMP3:
			break;
		case MDM_CONNECTION_DUMMY:
			mdm_driver_dummy_free(c);
			break;
		default:
			MDM_LOGERROR("Connection type is invalid: %d, aborting.", c->type);
			return;
	}
	MDM_MFREE(&c->descriptor.data);
	MDM_MFREE(&c->descriptor.options);

	/* Done. */
mdm_connection_free_done:
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
	return;
}

/*!
 * This is called to open the connection. Will internally call 
 * #mdm_connection_t.setup with the #mdm_connection_descriptor_t of c.
 * \param c Connection to setup, allocated with #mdm_connection_new.
 * \param options Options to setup, specific for each driver.
 * \param status The result of the operation will be stored here.
 * \return 0 on success.
 */
void
mdm_connection_setup(
	mdm_connection_t *c, mdm_connection_options_t *options,
	mdm_operation_result_t *status
)
{
	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif

	if((c != NULL) && (c->setup != NULL))
	{
		status->status = MDM_OP_OK;
		c->setup(&c->descriptor, options, status);
	} else {
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Connection or setup is null.");
	}
	/* Done. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
	return;
}

/*!
 * This is called to send data through the connection. Will internally call 
 * #mdm_connection_t.send with the #mdm_connection_descriptor_t of c.
 * \param c Connection to use, allocated with #mdm_connection_new.
 * \param buffer Buffer to send.
 * \param bufflen Length of the buffer.
 * \param status The result of the operation will be stored here.
 * \return 0 on success.
 */
void
mdm_connection_send(
	mdm_connection_t *c, const char *buffer,
	int bufflen, mdm_operation_result_t *status
)
{
	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif

	if((c != NULL) && (c->send != NULL))
	{
		status->status = MDM_OP_OK;
		c->send(&c->descriptor, buffer, bufflen, status);
	} else {
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Connection or send is null.");
	}

	/* Done. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
	return;
}
/*!
 * This is called to read data through the connection. Will internally call 
 * #mdm_connection_t.recv with the #mdm_connection_descriptor_t of c.
 * \param c Connection to use, allocated with #mdm_connection_new.
 * \param buffer Buffer to put the received data.
 * \param bufflen Max allowed length (Buffer length).
 * \param status The result of the operation will be stored here.
 * \return 0 on success.
 */
void
mdm_connection_recv(
	mdm_connection_t *c, void *buffer,
	int *bufflen, mdm_operation_result_t *status
)
{
	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	if(c->type != MDM_CONNECTION_SERIAL)
		MDM_LOGDEBUG("Start.");
#endif

	if((c != NULL) && (c->recv != NULL))
	{
		status->status = MDM_OP_OK;
		c->recv(&c->descriptor, buffer, bufflen, status);
	} else {
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Connection or recv is null.");
	}

	/* Done. */
#if MDM_DEBUG_MESSAGES > 0
	if(c->type != MDM_CONNECTION_SERIAL)
		MDM_LOGDEBUG("Done.");
#endif
	return;
}

/*!
 * Inits libssh2 with libssh2_init(0). Do not call this directly,
 * #mdm_init will do this.
 */
void
mdm_connection_init(void)
{
	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif

	libssh2_init(0);

	/* Done. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
	return;
}

/*!
 * Will deinitialize libssh2 with libssh2_exit. Do not call this
 * directly, #mdm_cleanup will do this.
 */
void
mdm_connection_cleanup(void)
{
	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif

	libssh2_exit();

	/* Done. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
	return;
}
/*******************************************************************************
 * CODE ENDS.
 ******************************************************************************/
