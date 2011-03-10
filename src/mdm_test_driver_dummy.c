/*!
 * \file mdm_test_driver_dummy.c A test for the dummy driver.
 *
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
/*******************************************************************************
 * Include Files.
 ******************************************************************************/
#include	<stdlib.h>
#include	<mdm.h>

/*******************************************************************************
 * Constants.
 ******************************************************************************/
#define	LOGLEVEL	MDM_LOG_DEBUG
#define	LOGSYSLOG	1
#define	LOGFILENAME	1
#define	LOGFULL		1
#define	LOGPATH		"/tmp/mdm_test_driver_dummy.log"

/*******************************************************************************
 * CODE STARTS.
 ******************************************************************************/

/*******************************************************************************
 * Main Entry Point.
 ******************************************************************************/
int
main(int argc, char *argv[])
{
	int retcode = EXIT_SUCCESS;
	mdm_connection_t c;
	mdm_driver_dummy_options_t options;
	mdm_operation_result_t status;

	/* Global MDM init. */
	mdm_init(LOGLEVEL, LOGSYSLOG, LOGFILENAME, LOGFULL, LOGPATH);
	mdm_connection_new(&c, MDM_CONNECTION_DUMMY, &status);
	mdm_connection_open(&c, (mdm_connection_options_t)&options, &status);
	mdm_connection_close(&c, &status);
	mdm_connection_free(&c);

	/* Global MDM cleanup. */
	mdm_cleanup();
	
	/* Quit. */
	exit(retcode);
}
/*******************************************************************************
 * CODE ENDS.
 ******************************************************************************/
