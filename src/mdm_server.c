/*!
 * \file mdm_server.c MDM Server. Needs 1 command line argument, the directory
 * where to find the xml config files and xsd schemas.
 *
 * \todo finish devices in use... too many fuck ups points :P
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
/*******************************************************************************
 * Include Files.
 ******************************************************************************/
#include	<stdio.h>
#include	<stdlib.h>
#include	<stdarg.h>
#include	<unistd.h>
#include	<string.h>
#include	<errno.h>
#include	<signal.h>
#include	<poll.h>
#include	<libxml/parser.h>
#include	<libxml/tree.h>
#include	<libxml/xmlschemas.h>
#include	<libxml/xpath.h>
#include	<libxml/xpathInternals.h>
#include	<pthread.h>
#include	<mdm.h>
#include	<pwd.h>
#include	<grp.h>

/*******************************************************************************
 * Constants.
 ******************************************************************************/
#define	RESPONSE_MAX_LEN		1048576	/*!< 1M max for xml responses. */
#define	PRERESPONSE_MAX_LEN	1048576	/*!< 1M max for parser responses. */

/*******************************************************************************
 * Types.
 ******************************************************************************/
/*! #client_handler uses this to get arguments from main and handle the
 * connections.
 */
typedef struct
{
	int shutdown; /*!< Flag for server to know then thread ended. */
	int initialized; /*!< Flag for server to know if thread was initialized. */
	int id; /*!< Thread id. */
	int working; /*!< Flag for server to know if this thread is now busy. */
	char buffer[4096]; /*!< Receive buffer. */
	int bufferlen; /*!< Receive buffer len. */
	pthread_cond_t wakeup; /*!< Condition to wait for wake up. */
	pthread_mutex_t wakeup_mtx; /*!< Mutex associated with condition wake up. */
	mdm_connection_t *client; /*!< Client given by server. */
	mdm_driver_rawtcp_options_t *client_options; /*!< Pointer to
		client options. */
	mdm_driver_rawtcp_data_t *client_data; /*!< Pointer to client data. */
	mdm_operation_result_t status; /*!< Last operation result. */
} client_handler_argument_t;

/*!
 * #client_handler uses this to handle commands from clients.
 */
typedef struct
{
	xmlDocPtr xml_request; /*!< XML Doc from read buffer. */
	xmlXPathContextPtr xpathCtx; /*!< For Xpath expressions. */
	mdm_device_type_t device_requested; /*!< Device requested. */
	mdm_device_cmd_t command_requested; /*!< Command requested. */
	mdm_device_cmd_t connection_requested; /*!< Connection requested. */
	mdm_device_t *device; /*!< Will store the device in use. */
	mdm_device_options_t device_options; /*!< Options for opening device. */
	int to_recv; /*!< Read timeout for device. */
	int to_connect; /*!< Connect timeout for device. */
	char *response; /*!< Final XML Response. */
	int responselen; /*!< Final XML Response Length. */
} client_handler_command_t;

/*! For devices in use. */
typedef struct
{
	char target[FILENAME_MAX]; /*!< Connection target. */
	int initialized; /*!< If this device is initialized (connected). */
	pthread_mutex_t mutex; /*!< Mutex to prevent race conditions. */
	mdm_device_t *device; /*!< Device created with #mdm_device_new */
	int in_use; /*!< Currently in use by other thread. */
} devices_in_use_t;

/*******************************************************************************
 * Global variables.
 ******************************************************************************/
static pthread_mutex_t libxml2_mutex; /*!< Exclusive access to libxml2 when
	validating requests against schemas. */
static int server_started = 0; /*!< For libxml2 error handler. */
static int running = 1; /*!< Changed by #signal_handler to 0 on SIGINT, SIGTERM
	and SIGQUIT */
static devices_in_use_t *devices_in_use = NULL; /*!< For devices in use. */
static pthread_mutex_t devices_in_use_mtx; /*!< To access #devices_in_use */
static int devices_in_use_n; /*!< Counter for devices in use. */

/* Global MDM configuration options for #mdm_init */
static int logsyslog = 0; /*!< Overriden by #readconfig. */
static int logfull = 0; /*!< Overriden by #readconfig. */
static int loglevel = MDM_LOG_ERROR; /*!< Overriden by #readconfig. */
static const char *logpath; /*!< Overriden by #readconfig. */
static int logfile = 0; /*!< Overriden by #readconfig. */

/* Configuration options for server connection. */
static const char *listen_address = NULL; /*!< Overriden by #readconfig. */
static int listen_port = 0; /*!< Overriden by #readconfig. */
static int listen_reuse = 0; /*!< Overriden by #readconfig. */
static int listen_backlog = 0; /*!< Overriden by #readconfig. */
static int listen_toaccept = 0; /*!< Overriden by #readconfig. */
static const char *pidfile = NULL; /*!< Overriden by #readconfig. */
static const char *uid = NULL; /*!< Overriden by #readconfig. */
static const char *gid = NULL; /*!< Overriden by #readconfig. */
static int maxclients = 0; /*!< Overriden by #readconfig. */
static int clients_block = 0; /*!< Overriden by #readconfig. */
static int clients_to_recv = 0; /*!< Overriden by #readconfig. */
static int devices_maxopen = 0; /*!< Overriden by #readconfig. */
static int keepalive_interval = 0; /*!< Overriden by #readconfig. */
static mdm_connection_t server_connection; /*!< Our server connection. */

/* Threads variables. */
static pthread_t *threads = NULL; /*!< Our thread army.*/
static client_handler_argument_t *threads_args = NULL; /*!< Threads params. */
static pthread_t server_thread; /*!< Server pthread id. */
static pthread_t keepalive_thread; /*!< Sends NOP to keep connections open. */
static int keepalive_thread_init = 0; /*!< To know if keep alive
	thread is alive. */
static pthread_mutex_t keepalive_fakeMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t keepalive_fakeCond = PTHREAD_COND_INITIALIZER;

/* For request & response validation. */
static xmlSchemaParserCtxtPtr request_parser_ctxt = NULL; /*!< Needed to
	validate requests against request schema. */
static xmlDocPtr request_schema_doc = NULL; /*!< Needed to
	validate requests against request schema. */
static xmlSchemaPtr request_schema = NULL; /*!< Needed to
	validate requests against request schema. */
static xmlSchemaValidCtxtPtr request_valid_ctxt = NULL; /*!< Needed to
	validate requests against request schema. */
	
/*******************************************************************************
 * Forward Declarations.
 ******************************************************************************/
static void signal_handler(int sig);
static int readconfig(const char *file, const char *schema);
static void readconfig_error_handler(
	xmlParserCtxtPtr ctx, const char *msg, ...
);
/*static void *keepalive(void *args);*/
static void *client_handler(void *args);
static char *response_form(
	int code, const char *msg, const char *data, int datalen,
	const char *originalresponse, int originalresponselen
);
static int read_schemas(const char *);
static int pidfile_write(const char *);
static void pidfile_cleanup(const char *);
static int privileges_switch(const char *user, const char *group);
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
	int i = 0;
	
	switch(sig)
	{
		case SIGINT:
		case SIGTERM:
		case SIGQUIT:
			/* If we're a worker kill the server and quit. */
			if(!pthread_equal(pthread_self(), server_thread))
			{
				if(running)
					pthread_kill(server_thread, SIGINT);
				running = 0;
				return;
			}
			MDM_LOG("Got signal: %d", sig);
			/* Send SIGINT to all threads. */
			if(threads != NULL && threads_args != NULL)
			{
				for(i = 0; i < maxclients; i++)
				{
					if(threads_args[i].initialized == 1 && !threads_args[i].shutdown)
					{
						pthread_kill(threads[i], SIGINT);
					}
				}
			}
			break;
		default:
			MDM_LOGWARN("Unhandled signal: %d", sig);
			break;
	}
}

/*!
 * libxml2 error handler used to report errors by libxml2 during xml parsing
 * and schema validation.
 */
static void
readconfig_error_handler(xmlParserCtxtPtr ctx, const char *msg, ...)
{
	char string[1024];
	va_list arg_ptr;
	va_start(arg_ptr, msg);
	vsnprintf(string, sizeof(string), msg, arg_ptr);
	va_end(arg_ptr);
	if(!server_started)
		fprintf(stderr, "File error: %s\n", string);
	else
		MDM_LOGERROR("File error: %s\n", string);
	return;
}

/*!
 * Used to get the value of a xpath expression.
 * \param xpathCtx libxml2 xpath context, previously initialized by
 * #readconfig.
 * \param xpath xpath expression to evaluate.
 * \return NULL if none found, otherwise a cstring with the value.
 */
static const char *
get_xpath_value(xmlXPathContextPtr xpathCtx, const char *xpath)
{
	const char *ret = NULL;
	xmlXPathObjectPtr xpathObj;

	xpathObj = xmlXPathEvalExpression((unsigned const char *)xpath, xpathCtx);
	if(xpathObj == NULL)
	{
		xmlXPathFreeContext(xpathCtx); 
		return NULL;
	}
	if(xpathObj->nodesetval->nodeNr > 0)
	{
		if(xpathObj->nodesetval->nodeTab[0]->type == XML_ELEMENT_NODE)
		{
			if(!xpathObj->nodesetval->nodeTab[0]->ns)
			{
				ret =
					(const char *)xmlNodeGetContent(
						xpathObj->nodesetval->nodeTab[0]
					);
			}
		}
	}
	xmlXPathFreeObject(xpathObj);
	return ret;
}

/*!
 * Tries to parse and validate an xml config file against a xsd schema. On
 * success will fill the global config variables. Will also install a libxml2
 * error handler to report any errors: #readconfig_error_handler.
 * \param filename xml config file path.
 * \param schema_filename xsd file to validate filename.
 * \return -1 on error, 0 on success.
 */
static int
readconfig(const char *filename, const char *schema_filename)
{
	xmlSchemaParserCtxtPtr parser_ctxt = NULL;
	xmlDocPtr schema_doc = NULL;
	xmlDocPtr doc = NULL;
	xmlSchemaPtr schema = NULL;
	xmlSchemaValidCtxtPtr valid_ctxt = NULL;
	xmlXPathContextPtr xpathCtx = NULL;
	const char *loglevelstring = NULL;
	int retcode = -1;
	
	/* Init error handler. */
	xmlGenericErrorFunc handler = (xmlGenericErrorFunc)readconfig_error_handler;
	initGenericErrorDefaultFunc(&handler);

	/* Read config file first. */
	doc = xmlReadFile(filename, NULL, 0);
	if(doc == NULL)
	{
		fprintf(stderr, "Failed to parse %s\n", filename);
		goto readconfig_done;
	}

	/* Read schema now. */
	schema_doc = xmlReadFile(schema_filename, NULL, XML_PARSE_NONET);
	if(schema_doc == NULL)
	{
		/* the schema cannot be loaded or is not well-formed */
		fprintf(stderr, "Cannot open: %s\n", schema_filename);
		goto readconfig_done;
	}

	parser_ctxt = xmlSchemaNewDocParserCtxt(schema_doc);
	if(parser_ctxt == NULL)
	{
		/* unable to create a parser context for the schema */
		fprintf(stderr, "Cannot read: %s\n", schema_filename);
		goto readconfig_done;
	}

	schema = xmlSchemaParse(parser_ctxt);
	if(schema == NULL)
	{
		/* the schema itself is not valid */
		fprintf(stderr, "Invalid schema: %s\n", schema_filename);
		goto readconfig_done;
	}

	valid_ctxt = xmlSchemaNewValidCtxt(schema);
	if(valid_ctxt == NULL)
	{
		/* unable to create a validation context for the schema */
		fprintf(stderr, "Cant create validation context.\n");
		goto readconfig_done;
	}

	/* Validate config. */
	if(xmlSchemaValidateDoc(valid_ctxt, doc))
	{
		fprintf(stderr, "Not a valid config file. Check schema.\n");
		goto readconfig_done;
	}

	/* Create xpath evaluation context */
   xpathCtx = xmlXPathNewContext(doc);
	if(xpathCtx == NULL)
	{
		fprintf(stderr, "Error: unable to create new XPath context\n");
		goto readconfig_done;
	}
	/* do register namespace */
	if(xmlXPathRegisterNs(
	    xpathCtx, (unsigned const char *)"mdm",
	    (unsigned const char *)"http://www.mg.com.ar/MDM"
    ) != 0) {
        fprintf(stderr, "Error: unable to register NS.\n");
        goto readconfig_done;
    }

    /* Evaluate xpath expression */
	logpath = get_xpath_value(xpathCtx, "/mdm_server_config/logging/logpath");
	if(strcasecmp(
		get_xpath_value(xpathCtx, "/mdm_server_config/logging/logsyslog"),
		"true"
	) == 0)
		logsyslog = 1;
	if(strcasecmp(
		get_xpath_value(xpathCtx, "/mdm_server_config/logging/logfile"),
		"true"
	) == 0)
		logfile = 1;
	if(strcasecmp(
		get_xpath_value(xpathCtx, "/mdm_server_config/logging/logfull"),
		"true"
	) == 0)
		logfull = 1;
	loglevelstring =
		get_xpath_value(xpathCtx, "/mdm_server_config/logging/loglevel")
	;
	if(strcasecmp(
		get_xpath_value(xpathCtx, "/mdm_server_config/listen/reuse"),
		"true"
	) == 0)
		listen_reuse = 1;
	if(strcasecmp(loglevelstring, "warn") == 0)
		loglevel = MDM_LOG_ERROR;
	else if(strcasecmp(loglevelstring, "info") == 0)
		loglevel = MDM_LOG_INFO;
	else if(strcasecmp(loglevelstring, "debug") == 0)
		loglevel = MDM_LOG_DEBUG;
	else if(strcasecmp(loglevelstring, "perf") == 0)
		loglevel = MDM_LOG_PERF;

	/* Get listen configuration. */
	listen_address = get_xpath_value(
		xpathCtx, "/mdm_server_config/listen/address"
	);
	listen_port = strtol(get_xpath_value(
		xpathCtx, "/mdm_server_config/listen/port"
	), NULL, 10);

	listen_backlog = strtol(get_xpath_value(
		xpathCtx, "/mdm_server_config/listen/backlog"
	), NULL, 10);

	listen_toaccept = strtol(get_xpath_value(
		xpathCtx, "/mdm_server_config/listen/accept_timeout"
	), NULL, 10);

	maxclients = strtol(get_xpath_value(
		xpathCtx, "/mdm_server_config/listen/maxclients"
	), NULL, 10);

	clients_to_recv = strtol(get_xpath_value(
		xpathCtx, "/mdm_server_config/clients/recv_timeout"
	), NULL, 10);

	devices_maxopen = strtol(get_xpath_value(
		xpathCtx, "/mdm_server_config/devices/maxopen"
	), NULL, 10);

	keepalive_interval = strtol(get_xpath_value(
		xpathCtx, "/mdm_server_config/devices/keepalive"
	), NULL, 10);
	pidfile = get_xpath_value(xpathCtx, "/mdm_server_config/pid");
	uid = get_xpath_value(xpathCtx, "/mdm_server_config/user");
	gid = get_xpath_value(xpathCtx, "/mdm_server_config/group");

	/* Success! */
	retcode = 0;

	/* Done. */
readconfig_done:
	if(xpathCtx != NULL)
		xmlXPathFreeContext(xpathCtx);

	if(valid_ctxt != NULL)
		xmlSchemaFreeValidCtxt(valid_ctxt);

	if(schema != NULL)
		xmlSchemaFree(schema);

	if(parser_ctxt != NULL)
		xmlSchemaFreeParserCtxt(parser_ctxt);

	if(schema_doc != NULL)
		xmlFreeDoc(schema_doc);
	
	if(doc != NULL)
		xmlFreeDoc(doc);

	if(retcode != 0)
	{
		fprintf(stderr, "Done xml cleanup.\n");
	}
	return retcode;
}

/*!
 * This is the thread that will send NOP to devices to keep the connection
 * open. I.e: Alcatel DSLAM will close the connection because of inactivity.
 */
static void *
keepalive(void *args)
{
	sigset_t my_signals;
	struct timespec timeToWait;
	struct timeval now;
	int i;
	mdm_device_cmd_t cmd;
	mdm_operation_result_t status;
	
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif
	/* Dont catch any signals. */
	sigfillset(&my_signals);
	/*sigdelset(&my_signals, SIGINT);*/
	pthread_sigmask(SIG_BLOCK, &my_signals, NULL);

	/* Main Loop. */
	while(running)
	{
		gettimeofday(&now, NULL);
		timeToWait.tv_sec = now.tv_sec + keepalive_interval;
		timeToWait.tv_nsec = now.tv_usec * 1000;
		pthread_mutex_lock(&keepalive_fakeMutex);

#if MDM_DEBUG_MESSAGES > 0
		MDM_LOGDEBUG("Sleeping.");
#endif

		pthread_cond_timedwait(
			&keepalive_fakeCond, &keepalive_fakeMutex, &timeToWait
		);
		if(!running)
			break;
		pthread_mutex_unlock(&keepalive_fakeMutex);
#if MDM_DEBUG_MESSAGES > 0
		MDM_LOGDEBUG("Woke up.");
#endif

		/* Try to find already in use device. */
		if(devices_maxopen > 0)
		{
			/*pthread_mutex_lock(&devices_in_use_mtx);*/
			for(i = 0; i < devices_maxopen; i++)
			{
				cmd = -1;
				if(!running)
					break;
				if(devices_in_use[i].in_use == 1)
					continue;
				pthread_mutex_lock(&devices_in_use[i].mutex);
				devices_in_use[i].in_use = 1;
				if(devices_in_use[i].initialized == 1)
				{
					MDM_LOGDEBUG("Sending NOP %d: %s", i, devices_in_use[i].target);
					switch(devices_in_use[i].device->type)
					{
						case MDM_DEVICE_DSLAM_ALCATEL_73xx:
							cmd = MDM_DSLAM_ALCATEL_73xx_CMD_NOP;
							break;
						case MDM_DEVICE_DSLAM_ZTE_9xxx:
							cmd = MDM_DSLAM_ZTE_9xxx_CMD_NOP;
							break;
						case MDM_DEVICE_DSLAM_ZTE_8426:
							cmd = MDM_DSLAM_ZTE_8426_CMD_NOP;
							break;
						case MDM_DEVICE_DSLAM_SIEMENS_HIX5300:
							cmd = MDM_DSLAM_SIEMENS_HIX5300_CMD_NOP;
							break;
						default:
							break;
					}
					mdm_device_exec(devices_in_use[i].device, cmd, NULL, &status);
					if(status.status == MDM_OP_ERROR)
					{
						MDM_LOGWARN("Connection down %s", devices_in_use[i].target);
						mdm_device_close(devices_in_use[i].device, &status);
						devices_in_use[i].initialized = 0;
						devices_in_use_n--;
						mdm_device_free(devices_in_use[i].device);
						MDM_MFREE(&devices_in_use[i].device);
					}
					MDM_LOGDEBUG("Sent NOP: %d: %s", i, devices_in_use[i].target);
				}
				devices_in_use[i].in_use = 0;
				pthread_mutex_unlock(&devices_in_use[i].mutex);
			}
			/*pthread_mutex_unlock(&devices_in_use_mtx);*/
		}
	}
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
	return NULL;
}

/*!
 * Function for handling clients. Each one of these is a thread.
 * \param args Thread arguments.
 * \return NULL
 */
static void *
client_handler(void *args)
{
	sigset_t my_signals;
	struct timeval time_start;
	struct timeval time_end;
	client_handler_argument_t *thargs = (client_handler_argument_t *)args;
	client_handler_command_t command;
	mdm_device_dslam_alcatel_73xx_options_t dslam_alcatel_73xx_options;
	mdm_device_dslam_zte_8426_options_t dslam_zte_8426_options;
	mdm_device_dslam_zte_9xxx_options_t dslam_zte_9xxx_options;
    mdm_device_dslam_huawei_ma5600_options_t dslam_huawei_ma5600_options;
    mdm_device_dslam_siemens_hix5300_options_t dslam_siemens_hix5300_options;
	mdm_device_t *device;
	const char *tempbuffer;
	const char *connection_target;
	int i;
	int using_in_use;
	int device_options_len;
	
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("%d: Started.", thargs->id);
#endif

	/* Init error handler. */
	xmlGenericErrorFunc handler = (xmlGenericErrorFunc)readconfig_error_handler;
	initGenericErrorDefaultFunc(&handler);

	/* Dont catch any signals. */
	sigfillset(&my_signals);
	sigdelset(&my_signals, SIGINT);
	pthread_sigmask(SIG_BLOCK, &my_signals, NULL);

	/* Main Loop. */
	while(running)
	{
#if MDM_DEBUG_MESSAGES > 0
		MDM_LOGDEBUG("%d: Sleeping.", thargs->id);
#endif

		/* Wait for work or shutdown. */
		device = NULL;
		command.device = NULL;
		thargs->client = NULL;
		command.xml_request = NULL;
		command.xpathCtx = NULL;
		command.response = NULL;
		command.responselen = 0;
		using_in_use = -1;
		device_options_len = 0;
		pthread_mutex_lock(&thargs->wakeup_mtx);
		thargs->working = 0;
		pthread_cond_wait(&thargs->wakeup, &thargs->wakeup_mtx);
		pthread_mutex_unlock(&thargs->wakeup_mtx);

#if MDM_DEBUG_MESSAGES > 0
		MDM_LOGDEBUG("%d: Woke up.", thargs->id);
#endif

		thargs->working = 1;
		if(!running)
			break;

		/* Got work!. */
		thargs->client_options = thargs->client->descriptor.options;
		thargs->client_data = thargs->client->descriptor.data;
#if MDM_DEBUG_MESSAGES > 0
		MDM_LOGDEBUG(
			"%d: Got connection from: %s:%s:%p\n",
			thargs->id,
			thargs->client_options->host, thargs->client_options->service,
			thargs->client
		);
#endif

		/* Get command. */
		thargs->bufferlen = sizeof(thargs->buffer);
		memset(thargs->buffer, 0, thargs->bufferlen);
		mdm_connection_recv(
			thargs->client, &thargs->buffer, &thargs->bufferlen, &thargs->status
		);

		/* Error reading command? */
		if(thargs->status.status == MDM_OP_ERROR)
		{
			MDM_LOGERROR(
				"%d: Error receiving: %s\n",
				thargs->id, &thargs->status.status_message
			);
			goto work_done;
		}

		/* Control timeout. */
		if(thargs->bufferlen == 0)
		{
			MDM_LOGERROR("%d: Timeout on client\n", thargs->id);
			goto work_done;
		}
#if MDM_DEBUG_MESSAGES > 0
		MDM_LOGDEBUG(
			"%d: Read: |%.*s|", thargs->id, thargs->bufferlen, thargs->buffer
		);
#endif

		/* Parse XML.
		 * The document being in memory, it have no base per RFC 2396,
		 * and the "noname.xml" argument will serve as its base.
		 */
		if(pthread_mutex_lock(&libxml2_mutex))
		{
			MDM_LOGERROR("%d: Failed to lock libxml2 mutex.", thargs->id);
			goto work_done;
		}
		command.xml_request = xmlReadMemory(
			thargs->buffer, thargs->bufferlen, "noname.xml", NULL, 0
		);
		if(command.xml_request == NULL)
		{
			MDM_LOGERROR("%d: Failed to parse request.", thargs->id);
			pthread_mutex_unlock(&libxml2_mutex);
			goto work_done;
		}

#if MDM_DEBUG_MESSAGES > 0
		MDM_LOGDEBUG("%d: XML OK.", thargs->id);
#endif

		/* Validate request. */
		if(xmlSchemaValidateDoc(request_valid_ctxt, command.xml_request))
		{
			MDM_LOGERROR("%d: Not a valid request. Check schema.", thargs->id);
			pthread_mutex_unlock(&libxml2_mutex);
			goto work_done;
		}

		/* Unlock libxml2 mutex for other threads. */
		if(pthread_mutex_unlock(&libxml2_mutex))
		{
			MDM_LOGERROR("%d: Failed to unlock libxml2 mutex.", thargs->id);
			goto work_done;
		}

#if MDM_DEBUG_MESSAGES > 0
		MDM_LOGDEBUG("%d: SCHEMA OK.", thargs->id);
#endif
		gettimeofday(&time_start, NULL);

		/* Create xpath evaluation context */
		command.xpathCtx = xmlXPathNewContext(command.xml_request);
		if(command.xpathCtx == NULL)
		{
			MDM_LOGERROR(
				"%d: Error: unable to create new XPath context", thargs->id
			);
			goto work_done;
		}
		/* do register namespace */
		if(xmlXPathRegisterNs(
			command.xpathCtx, (unsigned const char *)"mdm",
			(unsigned const char *)"http://www.mg.com.ar/MDM-Protocol"
		) != 0) {
			MDM_LOGERROR(
				"%d: Error: unable to register NS.", thargs->id
			);
			goto work_done;
		}

		/* Validate device. */
		command.device_requested = strtol(
			get_xpath_value(command.xpathCtx, "/mdm_request/device"), NULL, 10
		);
		if(!mdm_device_isvalid_type(command.device_requested))
		{
			command.response = response_form(
				-1, "Invalid device requested", NULL, 0, NULL, 0
			);
			goto work_done;
		}

		/* Validate device command. */
		command.command_requested = strtol(
			get_xpath_value(command.xpathCtx, "/mdm_request/command/type"),
			NULL, 10
		);
		if(!mdm_devicecmd_isvalid(
			command.device_requested, command.command_requested
		))
		{
			command.response = response_form(
				-1, "Invalid device command requested", NULL, 0, NULL, 0
			);
			goto work_done;
		}

		/* Validate connection. */
		command.connection_requested = strtol(
			get_xpath_value(command.xpathCtx, "/mdm_request/connection/type"),
			NULL, 10
		);
		if(!mdm_connection_isvalid_type(command.connection_requested))
		{
			command.response = response_form(
				-1, "Invalid connection type requested", NULL, 0, NULL, 0
			);
			goto work_done;
		}

		/* Start real work. */
		device = (mdm_device_t *)MDM_MALLOC(sizeof(mdm_device_t));
		if(device == NULL)
		{
			MDM_LOGERROR("Could not allocate memory for new device.");
			goto work_done;
		}
		mdm_device_new(
			device, command.device_requested, &thargs->status
		);
		command.device = device;
		if(thargs->status.status == MDM_OP_ERROR)
		{
			command.response = response_form(
				-1, thargs->status.status_message, NULL, 0, NULL, 0
			);
			goto work_done;
		}

		/* Fill device and connection options. */
		switch(command.device_requested)
		{
			case MDM_DEVICE_DSLAM_ALCATEL_73xx:
				device_options_len = sizeof(
					mdm_device_dslam_alcatel_73xx_options_t
				);

				/* Get connection target. */
				tempbuffer = get_xpath_value(
					command.xpathCtx, "/mdm_request/connection/target"
				);
				if(tempbuffer == NULL)
				{
					command.response = response_form(
						-1, "Need target", NULL, 0, NULL, 0
					);
					goto work_done;
				}
				snprintf(
					dslam_alcatel_73xx_options.target,
					sizeof(dslam_alcatel_73xx_options.target), "%s", tempbuffer
				);
				connection_target = dslam_alcatel_73xx_options.target;

				/* Get connection username. */
				tempbuffer = get_xpath_value(
					command.xpathCtx, "/mdm_request/connection/credential/username"
				);
				if(tempbuffer == NULL)
				{
					command.response = response_form(
						-1, "Need username", NULL, 0, NULL, 0
					);
					goto work_done;
				}
				snprintf(
					dslam_alcatel_73xx_options.username,
					sizeof(dslam_alcatel_73xx_options.username), "%s", tempbuffer
				);

				/* Get connection password. */
				tempbuffer = get_xpath_value(
					command.xpathCtx, "/mdm_request/connection/credential/password"
				);
				if(tempbuffer == NULL)
				{
					command.response = response_form(
						-1, "Need password", NULL, 0, NULL, 0
					);
					goto work_done;
				}
				snprintf(
					dslam_alcatel_73xx_options.password,
					sizeof(dslam_alcatel_73xx_options.password),
					"%s", tempbuffer
				);

				/* Get connection timeout. */
				tempbuffer = get_xpath_value(
					command.xpathCtx, "/mdm_request/connection/connect_timeout"
				);
				if(tempbuffer == NULL)
				{
					command.response = response_form(
					    -1, "Need connection timeout", NULL, 0, NULL, 0
					);
					goto work_done;
				}
				dslam_alcatel_73xx_options.to_connect =
					strtol(tempbuffer, NULL, 10)
				;

				/* Get connection read timeout. */
				tempbuffer = get_xpath_value(
					command.xpathCtx, "/mdm_request/connection/recv_timeout"
				);
				if(tempbuffer == NULL)
				{
					command.response = response_form(
					    -1, "Need connection read timeout", NULL, 0, NULL, 0
					);
					goto work_done;
				}
				dslam_alcatel_73xx_options.to_recv =
					strtol(tempbuffer, NULL, 10)
				;

				/* Done with this. */
				command.device_options =
					(mdm_device_options_t)&dslam_alcatel_73xx_options
				;
				break;
				
			case MDM_DEVICE_DSLAM_ZTE_8426:
				device_options_len = sizeof(mdm_device_dslam_zte_8426_options_t);
				/* Get connection target. */
				tempbuffer = get_xpath_value(
					command.xpathCtx, "/mdm_request/connection/target"
				);
				if(tempbuffer == NULL)
				{
					command.response = response_form(
						-1, "Need target", NULL, 0, NULL, 0
					);
					goto work_done;
				}
				snprintf(
					dslam_zte_8426_options.target,
					sizeof(dslam_zte_8426_options.target), "%s", tempbuffer
				);
				connection_target = dslam_zte_8426_options.target;

				/* Get connection username. */
				tempbuffer = get_xpath_value(
					command.xpathCtx, "/mdm_request/connection/credential/username"
				);
				if(tempbuffer == NULL)
				{
					command.response = response_form(
						-1, "Need username", NULL, 0, NULL, 0
					);
					goto work_done;
				}
				snprintf(
					dslam_zte_8426_options.username,
					sizeof(dslam_zte_8426_options.username), "%s", tempbuffer
				);

				/* Get connection password. */
				tempbuffer = get_xpath_value(
					command.xpathCtx, "/mdm_request/connection/credential/password"
				);
				if(tempbuffer == NULL)
				{
					command.response = response_form(
						-1, "Need password", NULL, 0, NULL, 0
					);
					goto work_done;
				}
				snprintf(
					dslam_zte_8426_options.password,
					sizeof(dslam_zte_8426_options.password),
					"%s", tempbuffer
				);
				/* Get connection timeout. */
				tempbuffer = get_xpath_value(
					command.xpathCtx, "/mdm_request/connection/connect_timeout"
				);
				if(tempbuffer == NULL)
				{
					command.response = response_form(
					    -1, "Need connection timeout", NULL, 0, NULL, 0
					);
					goto work_done;
				}
				dslam_zte_8426_options.to_connect =
					strtol(tempbuffer, NULL, 10)
				;

				/* Get connection read timeout. */
				tempbuffer = get_xpath_value(
					command.xpathCtx, "/mdm_request/connection/recv_timeout"
				);
				if(tempbuffer == NULL)
				{
					command.response = response_form(
					    -1, "Need connection read timeout", NULL, 0, NULL, 0
					);
					goto work_done;
				}
				dslam_zte_8426_options.to_recv = strtol(tempbuffer, NULL, 10);

				/* Done with this. */
				command.device_options =
					(mdm_device_options_t)&dslam_zte_8426_options
				;
				break;
			case MDM_DEVICE_DSLAM_ZTE_9xxx:
				device_options_len = sizeof(mdm_device_dslam_zte_9xxx_options_t);
				/* Get connection target. */
				tempbuffer = get_xpath_value(
					command.xpathCtx, "/mdm_request/connection/target"
				);
				if(tempbuffer == NULL)
				{
					command.response = response_form(
						-1, "Need target", NULL, 0, NULL, 0
					);
					goto work_done;
				}
				snprintf(
					dslam_zte_9xxx_options.target,
					sizeof(dslam_zte_8426_options.target), "%s", tempbuffer
				);
				connection_target = dslam_zte_9xxx_options.target;

				/* Get connection username. */
				tempbuffer = get_xpath_value(
					command.xpathCtx, "/mdm_request/connection/credential/username"
				);
				if(tempbuffer == NULL)
				{
					command.response = response_form(
						-1, "Need username", NULL, 0, NULL, 0
					);
					goto work_done;
				}
				snprintf(
					dslam_zte_9xxx_options.username,
					sizeof(dslam_zte_9xxx_options.username), "%s", tempbuffer
				);

				/* Get connection password. */
				tempbuffer = get_xpath_value(
					command.xpathCtx, "/mdm_request/connection/credential/password"
				);
				if(tempbuffer == NULL)
				{
					command.response = response_form(
						-1, "Need password", NULL, 0, NULL, 0
					);
					goto work_done;
				}
				snprintf(
					dslam_zte_9xxx_options.password,
					sizeof(dslam_zte_9xxx_options.password),
					"%s", tempbuffer
				);
				/* Get connection timeout. */
				tempbuffer = get_xpath_value(
					command.xpathCtx, "/mdm_request/connection/connect_timeout"
				);
				if(tempbuffer == NULL)
				{
					command.response = response_form(
					    -1, "Need connection timeout", NULL, 0, NULL, 0
					);
					goto work_done;
				}
				dslam_zte_9xxx_options.to_connect =
					strtol(tempbuffer, NULL, 10)
				;

				/* Get connection read timeout. */
				tempbuffer = get_xpath_value(
					command.xpathCtx, "/mdm_request/connection/recv_timeout"
				);
				if(tempbuffer == NULL)
				{
					command.response = response_form(
					    -1, "Need connection read timeout", NULL, 0, NULL, 0
					);
					goto work_done;
				}
				dslam_zte_9xxx_options.to_recv = strtol(tempbuffer, NULL, 10);

				/* Done with this. */
				command.device_options =
					(mdm_device_options_t)&dslam_zte_9xxx_options
				;
				break;
        case MDM_DEVICE_DSLAM_HUAWEI_MA5600:
                device_options_len = sizeof(mdm_device_dslam_huawei_ma5600_options_t);
                /* Get connection target. */
                tempbuffer = get_xpath_value(
                        command.xpathCtx, "/mdm_request/connection/target"
                );
                if(tempbuffer == NULL)
                {
                        command.response = response_form(
                                -1, "Need target", NULL, 0, NULL, 0
                        );
                        goto work_done;
                }
                snprintf(
                        dslam_huawei_ma5600_options.target,
                        sizeof(dslam_huawei_ma5600_options.target), "%s", tempbuffer
                );
                connection_target = dslam_huawei_ma5600_options.target;

                /* Get connection username. */
                tempbuffer = get_xpath_value(
                        command.xpathCtx, "/mdm_request/connection/credential/username"
                );
                if(tempbuffer == NULL)
                {
                        command.response = response_form(
                                -1, "Need username", NULL, 0, NULL, 0
                        );
                        goto work_done;
                }
                snprintf(
                        dslam_huawei_ma5600_options.username,
                        sizeof(dslam_huawei_ma5600_options.username), "%s", tempbuffer
                );

                /* Get connection password. */
                tempbuffer = get_xpath_value(
                        command.xpathCtx, "/mdm_request/connection/credential/password"
                );
                if(tempbuffer == NULL)
                {
                        command.response = response_form(
                                -1, "Need password", NULL, 0, NULL, 0
                        );
                        goto work_done;
                }
                snprintf(
                        dslam_huawei_ma5600_options.password,
                        sizeof(dslam_huawei_ma5600_options.password),
                        "%s", tempbuffer
                );
                /* Get connection timeout. */
                tempbuffer = get_xpath_value(
                        command.xpathCtx, "/mdm_request/connection/connect_timeout"
                );
                if(tempbuffer == NULL)
                {
                        command.response = response_form(
                            -1, "Need connection timeout", NULL, 0, NULL, 0
                        );
                        goto work_done;
                }
                dslam_huawei_ma5600_options.to_connect =
                        strtol(tempbuffer, NULL, 10)
                ;

                /* Get connection read timeout. */
                tempbuffer = get_xpath_value(
                        command.xpathCtx, "/mdm_request/connection/recv_timeout"
                );
                if(tempbuffer == NULL)
                {
                        command.response = response_form(
                            -1, "Need connection read timeout", NULL, 0, NULL, 0
                        );
                        goto work_done;
                }
                dslam_huawei_ma5600_options.to_recv = strtol(tempbuffer, NULL, 10);

                /* Done with this. */
                command.device_options =
                        (mdm_device_options_t)&dslam_huawei_ma5600_options
                ;
                break;
 
        case MDM_DEVICE_DSLAM_SIEMENS_HIX5300:
                device_options_len = sizeof(mdm_device_dslam_siemens_hix5300_options_t);
                /* Get connection target. */
                tempbuffer = get_xpath_value(
                        command.xpathCtx, "/mdm_request/connection/target"
                );
                if(tempbuffer == NULL)
                {
                        command.response = response_form(
                                -1, "Need target", NULL, 0, NULL, 0
                        );
                        goto work_done;
                }
                snprintf(
                        dslam_siemens_hix5300_options.target,
                        sizeof(dslam_siemens_hix5300_options.target), "%s", tempbuffer
                );
                connection_target = dslam_siemens_hix5300_options.target;

                /* Get connection username. */
                tempbuffer = get_xpath_value(
                        command.xpathCtx, "/mdm_request/connection/credential/username"
                );
                if(tempbuffer == NULL)
                {
                        command.response = response_form(
                                -1, "Need username", NULL, 0, NULL, 0
                        );
                        goto work_done;
                }
                snprintf(
                        dslam_siemens_hix5300_options.username,
                        sizeof(dslam_siemens_hix5300_options.username), "%s", tempbuffer
                );

                /* Get connection password. */
                tempbuffer = get_xpath_value(
                        command.xpathCtx, "/mdm_request/connection/credential/password"
                );
                if(tempbuffer == NULL)
                {
                        command.response = response_form(
                                -1, "Need password", NULL, 0, NULL, 0
                        );
                        goto work_done;
                }
                snprintf(
                        dslam_siemens_hix5300_options.password,
                        sizeof(dslam_siemens_hix5300_options.password),
                        "%s", tempbuffer
                );
                /* Get connection enable password. */
                tempbuffer = get_xpath_value(
                        command.xpathCtx, "/mdm_request/connection/credential/enable"
                );
                if(tempbuffer == NULL)
                {
                        command.response = response_form(
                                -1, "Need enable password", NULL, 0, NULL, 0
                        );
                        goto work_done;
                }
                snprintf(
                        dslam_siemens_hix5300_options.enable,
                        sizeof(dslam_siemens_hix5300_options.enable),
                        "%s", tempbuffer
                );
                /* Get connection timeout. */
                tempbuffer = get_xpath_value(
                        command.xpathCtx, "/mdm_request/connection/connect_timeout"
                );
                if(tempbuffer == NULL)
                {
                        command.response = response_form(
                            -1, "Need connection timeout", NULL, 0, NULL, 0
                        );
                        goto work_done;
                }
                dslam_siemens_hix5300_options.to_connect =
                        strtol(tempbuffer, NULL, 10)
                ;

                /* Get connection read timeout. */
                tempbuffer = get_xpath_value(
                        command.xpathCtx, "/mdm_request/connection/recv_timeout"
                );
                if(tempbuffer == NULL)
                {
                        command.response = response_form(
                            -1, "Need connection read timeout", NULL, 0, NULL, 0
                        );
                        goto work_done;
                }
                dslam_siemens_hix5300_options.to_recv = strtol(tempbuffer, NULL, 10);

                /* Done with this. */
                command.device_options =
                        (mdm_device_options_t)&dslam_siemens_hix5300_options
                ;
                break;
			default:
				command.response = response_form(
					-1, "Should not get here: invalid device requested",
					NULL, 0, NULL, 0
				);
				goto work_done;
		}

		/* Try to find already in use device. */
		if(devices_maxopen > 0)
		{
			pthread_mutex_lock(&devices_in_use_mtx);
			for(i = 0; i < devices_maxopen; i++)
			{
#if MDM_DEBUG_MESSAGES > 0
				MDM_LOGDEBUG(
					"Looking up device in use %d |%s|%s|%d|%d|", i,
					devices_in_use[i].target, connection_target,
					devices_in_use[i].in_use, devices_in_use[i].initialized
				);
#endif
				if(devices_in_use[i].initialized == 0)
				{
					if(using_in_use == -1 && (devices_in_use[i].in_use == 0))
					{
						pthread_mutex_lock(&devices_in_use[i].mutex);
						using_in_use = i;
						devices_in_use[i].in_use = 1;
					}
					continue;
				} else if(
					strcmp(devices_in_use[i].target, connection_target
					) == 0)
				{
#if MDM_DEBUG_MESSAGES > 0
					MDM_LOGDEBUG("Found! device in use %d %s", i, connection_target);
#endif
					pthread_mutex_lock(&devices_in_use[i].mutex);
					devices_in_use[i].in_use = 1;
					if(using_in_use != -1)
					{
						devices_in_use[using_in_use].in_use = 0;
						pthread_mutex_unlock(&devices_in_use[using_in_use].mutex);
					}
					using_in_use = i;
					command.device = devices_in_use[i].device;
					mdm_device_free(device);
					MDM_MFREE(&device);
					break;
				}
			}
			if(i == devices_maxopen && using_in_use >= 0)
			{
#if MDM_DEBUG_MESSAGES > 0
				MDM_LOGDEBUG(
					"Allocating new device in use %d %s",
					using_in_use, connection_target
				);
#endif
				/* Open device. */
				mdm_device_open(
					command.device, command.connection_requested,
					command.device_options,
					device_options_len,
					&thargs->status
				);
				if(thargs->status.status == MDM_OP_ERROR)
				{
					mdm_device_free(device);
					MDM_MFREE(&device);
					devices_in_use[using_in_use].in_use = 0;
					pthread_mutex_unlock(&devices_in_use[using_in_use].mutex);
					pthread_mutex_unlock(&devices_in_use_mtx);
					command.response = response_form(
						-1, thargs->status.status_message, NULL, 0, NULL, 0
					);
					goto work_done;
				}
				sprintf(
					devices_in_use[using_in_use].target, "%s", connection_target
				);
				devices_in_use[using_in_use].device = command.device;
				devices_in_use[using_in_use].initialized = 1;
				devices_in_use_n++;
			}
			pthread_mutex_unlock(&devices_in_use_mtx);
		} else {
			/* Open device. */
			mdm_device_open(
				command.device, command.connection_requested,
				command.device_options,
				device_options_len,
				&thargs->status
			);
			if(thargs->status.status == MDM_OP_ERROR)
			{
				mdm_device_free(device);
				MDM_MFREE(&device);
				command.device = NULL;
				command.response = response_form(
					-1, thargs->status.status_message, NULL, 0, NULL, 0
				);
				goto work_done;
			}
		}

		/* Exec command! Wiiiiiiiii! */
		mdm_device_exec(
			command.device, command.command_requested,
			get_xpath_value(
				command.xpathCtx, "/mdm_request/command/arguments"
			), &thargs->status
		);
		if(thargs->status.status == MDM_OP_ERROR)
		{
			command.response = response_form(
				-1, thargs->status.status_message, NULL, 0,
				mdm_device_get_buffer(command.device),
				mdm_device_get_buffer_len(command.device)
			);
			command.responselen = strlen(command.response);
		} else {
			/* Form final command response. */
			command.response = response_form(
				0, "OK",
				mdm_device_get_buffer_post(command.device),
				mdm_device_get_buffer_post_len(command.device), 
				mdm_device_get_buffer(command.device),
				mdm_device_get_buffer_len(command.device)
			);
		}

		/* Device cleanup. */
		if(using_in_use == -1)
		{
			mdm_device_close(command.device, &thargs->status);
			mdm_device_free(command.device);
			MDM_MFREE(&command.device);
		} else {
			devices_in_use[using_in_use].in_use = 0;
			pthread_mutex_unlock(&devices_in_use[using_in_use].mutex);
		}
		
		/* Work done. */
	gettimeofday(&time_end, NULL);
	MDM_LOGPERF(
		"Work done in: %g secs.",
		thargs->id,
		((double)time_end.tv_sec - (double)time_start.tv_sec) + 
		(
			((double)time_end.tv_usec / (double)1000000) -
			((double)time_start.tv_usec / (double)1000000)
		)
	);
work_done:
		/* Responses buffer cleanup. */
		if(command.response != NULL)
		{
			mdm_connection_send(
				thargs->client, command.response, strlen(command.response),
				&thargs->status
			);
			
			MDM_MFREE(&command.response);
		}
		
		/* MDM Connection Cleanup. */
		mdm_connection_close(thargs->client, &thargs->status);
		mdm_connection_free(thargs->client);
		MDM_MFREE(&thargs->client);

		/* XML Cleanup. */
		if(command.xpathCtx != NULL)
			xmlXPathFreeContext(command.xpathCtx);
		if(command.xml_request != NULL)
			xmlFreeDoc(command.xml_request);
	}
	/* Done. */
	thargs->shutdown = 1;
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("%d: Done.", thargs->id);
#endif
	return NULL;
}

/*!
 * This will return the XML response.
 * \param code Result status, generally -1 or 0.
 * \param msg Result message.
 * \param originalresponse Original result, may be null.
 * \param originalresponselen Original result length. May be 0.
 * \param data Result data, may be null.
 * \param datalen Result data length. May be 0.
 * \return XML response or NULL on error. The caller must #mdm_mfree the
 * result.
 */
static char *
response_form(
	int code, const char *msg, const char *data, int datalen,
	const char *originalresponse, int originalresponselen
)
{
	char *response;
	char resultcode[8];
	
	response = (char *)MDM_MALLOC(RESPONSE_MAX_LEN);
	if(response == NULL)
	{
		MDM_LOGERROR("Could not allocate memory for response.");
		return NULL;
	}
	sprintf(response, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	strcat(response, "<mdm_response xmlns:mdm=\"http://www.mg.com.ar/MDM-Protocol\"\n");
	strcat(response, "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n");
	strcat(response, "xsi:schemaLocation=\"http://www.mg.com.ar/MDM-Protocol mdm_response.xsd\">\n");
	snprintf(resultcode, sizeof(resultcode), "%d", code);
	strcat(response, "<version>0.1.1</version>\n");
	strcat(response, "<originalresponse><![CDATA[\n");
	if((originalresponse != NULL) && (originalresponselen > 0))
	{
		strncat(response, originalresponse, RESPONSE_MAX_LEN - strlen(response));
	}
	strcat(response, "]]></originalresponse>\n");
	strcat(response, "<result>");
	strcat(response, resultcode);
	strcat(response, "</result>\n");
	strcat(response, "<message><![CDATA[");
	strcat(response, msg);
	strcat(response, "]]></message>\n");
	if((data != NULL) && (datalen > 1))
	{
		strcat(response, "<data>\n");
		strncat(response, data, RESPONSE_MAX_LEN - strlen(response));
		strcat(response, "</data>\n");
	}
	strcat(response, "</mdm_response>\n");

	return response;
}

/*!
 * Will try to create validation schemas for requests and responses.
 * \param srcdir directory containing needed schemas.
 * \return -1 on error, 0 on success.
 */
static int
read_schemas(const char *srcdir)
{
	char request_schema_filename[FILENAME_MAX];
	
	/* Form filename. */
	sprintf(request_schema_filename, "%s/mdm_request.xsd", srcdir);
	
	/* Read schema now. */
	request_schema_doc = xmlReadFile(
		request_schema_filename, NULL, XML_PARSE_NONET
	);
	if(request_schema_doc == NULL)
	{
		/* the schema cannot be loaded or is not well-formed */
		fprintf(stderr, "Cannot open: %s\n", request_schema_filename);
		return -1;
	}

	request_parser_ctxt = xmlSchemaNewDocParserCtxt(request_schema_doc);
	if(request_parser_ctxt == NULL)
	{
		/* unable to create a parser context for the schema */
		fprintf(stderr, "Cannot read: %s\n", request_schema_filename);
		return -1;
	}

	request_schema = xmlSchemaParse(request_parser_ctxt);
	if(request_schema == NULL)
	{
		/* the schema itself is not valid */
		fprintf(stderr, "Invalid schema: %s\n", request_schema_filename);
		return -1;
	}

	request_valid_ctxt = xmlSchemaNewValidCtxt(request_schema);
	if(request_valid_ctxt == NULL)
	{
		/* unable to create a validation context for the schema */
		fprintf(stderr, "Cant create validation context.\n");
		return -1;
	}
	return 0;
}
/*!
 * Writes the current pid to a pid file.
 * \param filename PID Filename.
 * \return -1 on error, 0 on success.
 */
static int
pidfile_write(const char *filename)
{
	pid_t mypid;
	char mypidstr[8];
	int mypidstrlen;
	FILE *pidfd;
	int retcode = 0;

	if (filename == NULL) {
		MDM_LOGERROR("NULL pidfile");
		return -1;
	}

	/* Get pid, format it to text. */
	mypid = getpid();
	snprintf(mypidstr, sizeof(mypidstr), "%d", mypid);
	mypidstrlen = strlen(mypidstr);

	MDM_LOG("Saving pid in %s", filename);
	pidfd = fopen(filename, "w");
	if (pidfd == NULL) {
		MDM_LOGERROR("Could not open pidfile: %s: %s", filename, strerror(errno));
		return -1;
	}
	if (fwrite(mypidstr, sizeof(char), mypidstrlen, pidfd) != mypidstrlen) {
		MDM_LOGERROR("Could not write pidfile: %s", strerror(errno));
		retcode = -1;
	}
	fclose(pidfd);
	return retcode;
}

/*!
 * Cleanups (deletes) pid file.
 * \param filename PID filename.
 * \return void
 */
static void
pidfile_cleanup(const char *filename)
{
	if (filename == NULL) {
		return;
	}
	unlink(filename);
}

/*!
 * Switches privileges to the given user (name) and group (name).
 * \param user User name (not uid).
 * \param group Group name (not gid).
 * \return -1 on error, 0 on success.
 */
static int
privileges_switch(const char *user, const char *group)
{
	struct passwd *uidpw;
	struct group *gidgr;

	uidpw = getpwnam(user);
	if (uidpw == NULL) {
		MDM_LOGERROR("Error getting user info for: %s: %s", user, strerror(errno));
		return -1;
	}

	gidgr = getgrnam(group);
	if (gidgr == NULL) {
		MDM_LOGERROR("Error getting group info for: %s: %s", group, strerror(errno));
		return -1;
	}
	if (setgid(gidgr->gr_gid) != 0 || setegid(gidgr->gr_gid) != 0) {
		MDM_LOGERROR("Could not switch to group: %s: %s", group, strerror(errno));
		return -1;
	}
	if (setuid(uidpw->pw_uid) != 0 || seteuid(uidpw->pw_uid)) {
		MDM_LOGERROR("Could not switch to user: %s: %s", user, strerror(errno));
		return -1;
	}
	return 0;
}

/*!
 * Main Entry point.
 */
int
main(int argc, char *argv[])
{
	int retcode = EXIT_SUCCESS;
	char cffile[FILENAME_MAX];
	char cfschema[FILENAME_MAX];
	mdm_operation_result_t status;
	mdm_driver_rawtcp_server_options_t options;
	mdm_connection_t *client = NULL;
	int bufflen;
	int i;
	int thid;
	FILE *pidfd;
	char stalepidstr[8];
	
	/* Get own thread id. */
	server_thread = pthread_self();
	
	/*
	 * this initialize the library and check potential ABI mismatches
	 * between the version it was compiled for and the actual shared
	 * library used.
	 */
	xmlInitParser();
	LIBXML_TEST_VERSION

	/* Check arguments. */
	if(argc != 2)
	{
		fprintf(stderr, "Use: %s <config directory>\n", argv[0]);
		retcode = EXIT_FAILURE;
		goto main_done;
	}

	/* Init libxml2 mutex to protect threads. */
	if(pthread_mutex_init(&libxml2_mutex, NULL))
	{
		fprintf(stderr, "Could not initialize libxml2 mutex.", argv[0]);
		retcode = EXIT_FAILURE;
		goto main_done;
	}

	/* Form config filename and schema for validation. */
	snprintf(cffile, sizeof(cffile), "%s/mdm_server_conf.xml", argv[1]);
	snprintf(cfschema, sizeof(cffile), "%s/mdm_server_conf.xsd", argv[1]);
	
	/* Read config file and config values. */
	if(readconfig(cffile, cfschema) == -1)
	{
		retcode = EXIT_FAILURE;
		goto main_done_no_mdm_cleanup;
	}

	/* Read request & response schemas. */
	if(read_schemas(argv[1]) == -1)
	{
		retcode = EXIT_FAILURE;
		goto main_done_no_mdm_cleanup;
	}

	/* Setup signal handler. */
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGQUIT, signal_handler);

	/* Init MDM. */
	mdm_init(loglevel, logsyslog, logfile, logfull, logpath);
	
	/* Flag libxml2 error handler to start using mdm_logerror. */
	server_started = 1;

	/* Drop privileges. */
	if (privileges_switch(uid, gid) != 0) {
		retcode = EXIT_FAILURE;
		goto main_done;
	}

	/* Check for already existant pidfile. */
	pidfd = fopen(pidfile, "r");
	if (pidfd == NULL) {
		if (pidfile_write(pidfile) != 0) {
			retcode = EXIT_FAILURE;
			goto main_done;
		}
	} else {
		memset(stalepidstr, 0, sizeof(stalepidstr));
		if (fread(stalepidstr, 1, sizeof(stalepidstr), pidfd) == 0) {
			if (pidfile_write(pidfile) != 0) {
				retcode = EXIT_FAILURE;
				goto main_done;
			}
		} else {
			retcode = EXIT_FAILURE;
			MDM_LOGERROR("Already running with pid: %s or stale pidfile %s.", stalepidstr, pidfile);
			pidfile = NULL;
			goto main_done;
		}
	}
	
	/* Allocate memory for threads args. */
	threads_args = (client_handler_argument_t *)MDM_MALLOC(
		maxclients * sizeof(client_handler_argument_t)
	);
	if(threads_args == NULL)
	{
		MDM_LOGERROR("Error allocating memory for threads args.");
		goto main_done;
	}

	/* Allocate memory for threads. */
	threads = (pthread_t *)MDM_MALLOC(maxclients * sizeof(pthread_t));
	if(threads == NULL)
	{
		MDM_LOGERROR("Error allocating memory for threads.");
		goto main_done;
	}

	/* Get Raw TCP Server Connection.*/
	mdm_connection_new(
		&server_connection, MDM_CONNECTION_RAW_TCP_SERVER, &status
	);
	if(status.status == MDM_OP_ERROR)
	{
		fprintf(stderr, "Error creating connection: %s\n", status.status_message);
		retcode = 254;
		goto main_done;
	}

	/* Initialize memory for devices open. */
	if(devices_maxopen > 0)
	{
		devices_in_use = (devices_in_use_t *)MDM_MALLOC(
			devices_maxopen * sizeof(devices_in_use_t)
		);
		if(devices_in_use == NULL)
		{
			MDM_LOGERROR("Error allocating memory for devices in use.");
			goto main_done;
		}

		/* Initialize mutex for devices open. */
		if(pthread_mutex_init(&devices_in_use_mtx, NULL))
		{
			MDM_LOGERROR("Error creating mtx for devices in use.");
			goto main_done;
		}

		/* Initialize all mutexes. */
		for(i = 0; i < devices_maxopen; i++)
		{
			if(pthread_mutex_init(&devices_in_use[i].mutex, NULL))
			{
				MDM_LOGERROR("Error creating mtx for device in use %d.", i);
				goto main_done;
			}
		}
		
		/* Launch keepalive thread. */
		if(pthread_create(&keepalive_thread, NULL, keepalive, NULL))
		{
			MDM_LOGERROR("Error spawning keep alive thread.");
			goto main_done;
		}
		keepalive_thread_init = 1;
	}

	/* Start threads! */
	for(i = 0; i < maxclients; i++)
	{
		thid = i + 1;
		if(pthread_cond_init(&threads_args[i].wakeup, NULL))
		{
			MDM_LOGERROR("Error creating wakeup condition for thread: %d.", thid);
			goto main_done;
		}
		if(pthread_mutex_init(&threads_args[i].wakeup_mtx, NULL))
		{
			MDM_LOGERROR("Error creating wakeup mtx for thread: %d.", thid);
			goto main_done;
		}
		threads_args[i].id = thid;
		threads_args[i].initialized = 1;
		if(pthread_create(
			&threads[i], NULL, client_handler, (void *)&threads_args[i]
		))
		{
			MDM_LOGERROR("Error spawning thread: %d.", thid);
			threads_args[i].initialized = 0;
			goto main_done;
		}
	}
	
	/* Copy options from command line. */
	snprintf(options.host, sizeof(options.host), "%s", listen_address);
	snprintf(options.service, sizeof(options.service), "%d", listen_port);
	options.reuse = listen_reuse;
	options.backlog = listen_backlog;
	options.to_accept = listen_toaccept;
	options.clients_to_recv = clients_to_recv;
	options.clients_block = clients_block;

	/* Start Server. */	
	MDM_LOG("Server Start on %s:%d.", listen_address, listen_port);
	mdm_connection_open(
		&server_connection, (mdm_connection_options_t)&options, &status
	);
	if(status.status == MDM_OP_ERROR)
	{
		fprintf(stderr, "Error opening: %s\n", status.status_message);
		retcode = 254;
		goto main_done;
	}

	/* Main Loop. */
	while(running)
	{
#if MDM_DEBUG_MESSAGES > 0
		MDM_LOGDEBUG("Allocating new client.");
#endif
		bufflen = sizeof(mdm_connection_t);
		client = MDM_MALLOC(sizeof(mdm_connection_t));
		if(client == NULL)
		{
			MDM_LOGERROR("Not enough memory for client?");
			break;
		}
		mdm_connection_recv(&server_connection, client, &bufflen, &status);
		if(status.status == MDM_OP_ERROR)
		{
			fprintf(stderr, "Error accepting: %s\n", status.status_message);
			retcode = 254;
			goto main_done;
		}
		if(bufflen > 0)
		{
			/* Search for a free worker. */
			for(i = 0; i < maxclients; i++)
			{
				if(threads_args[i].initialized && !threads_args[i].working)
				{
					/* Found one. Copy client data and signal to get it to work. */
#if MDM_DEBUG_MESSAGES > 0
					MDM_LOGDEBUG("To thread: %d.", threads_args[i].id);
#endif
					threads_args[i].client = client;
					threads_args[i].working = 1;
					pthread_cond_signal(&threads_args[i].wakeup);
					break;
				}
			}
			if(i == maxclients)
			{
				MDM_LOGWARN("No more clients available. Increase maxclients.");
				mdm_connection_close(client, &status);
				mdm_connection_free(client);
				MDM_MFREE(&client);
			}
		}
	}

	/* Main Loop done. */
	MDM_LOG("Server Stop.");

	/* Done. */
main_done:
	MDM_LOG("Shutting down...");
	/* Cleanup server connection. */
	mdm_connection_close(&server_connection, &status);
	mdm_connection_free(&server_connection);

	/* Kill our threads... sniff. */
	if((threads != NULL) && (threads_args != NULL))
	{
		for(i = 0; i < maxclients; i++)
		{
			if(threads_args[i].initialized == 1)
			{
#if MDM_DEBUG_MESSAGES > 0
				MDM_LOGDEBUG("Joining on thread: %d", i + 1);
#endif
				/* Wait for thread to finish work. */
				/*
				 This should not be necessary because #signal_handler already
				 sends SIGINT's to all threads so they will get EINTR when
				 sleeping on the condition wake up.
				*/
				if(!threads_args[i].shutdown)
					pthread_cond_signal(&threads_args[i].wakeup);
				pthread_join(threads[i], NULL);
				pthread_mutex_destroy(&threads_args[i].wakeup_mtx);
				pthread_cond_destroy(&threads_args[i].wakeup);
			}
		}
	}
	/* Free memory used by our threads. */
	MDM_MFREE(&threads_args);
	MDM_MFREE(&threads);

	/* Devices in use cleanup. */
	if(devices_in_use != NULL)
	{
		pthread_mutex_destroy(&devices_in_use_mtx);

		/* Cleanup all mutexes. */
		if(keepalive_thread_init)
		{
			pthread_cond_signal(&keepalive_fakeCond);
#if MDM_DEBUG_MESSAGES > 0
			MDM_LOGDEBUG("Joining on keep alive thread.");
#endif
			pthread_join(keepalive_thread, NULL);
		}
		for(i = 0; i < devices_maxopen; i++)
		{
			if(devices_in_use[i].initialized)
			{
				mdm_device_close(devices_in_use[i].device, &status);
				mdm_device_free(devices_in_use[i].device);
				MDM_MFREE(&devices_in_use[i].device);
			}
			pthread_mutex_destroy(&devices_in_use[i].mutex);
		}
		MDM_MFREE(&devices_in_use);
	}

	/* Cleanup MDM. */
	mdm_cleanup();

main_done_no_mdm_cleanup:
	/* Cleanup schemas for requests & responses. */
	if(request_valid_ctxt != NULL)
		xmlSchemaFreeValidCtxt(request_valid_ctxt);

	if(request_schema != NULL)
		xmlSchemaFree(request_schema);

	if(request_parser_ctxt != NULL)
		xmlSchemaFreeParserCtxt(request_parser_ctxt);

	if(request_schema_doc != NULL)
		xmlFreeDoc(request_schema_doc);
		
	/* Cleanup pid file. */
	pidfile_cleanup(pidfile);

	/* Cleanup function for the XML library. */
	xmlCleanupParser();
	
	/* Cleanup libxml2 mutex. */
	pthread_mutex_destroy(&libxml2_mutex);
	
	/* Cleanup client. */
	MDM_MFREE(&client);
	exit(retcode);
}
/*******************************************************************************
 * CODE ENDS.
 ******************************************************************************/
