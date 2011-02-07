/*!
 * \file mdm.h Main include file. Use this one for everything.
 * \todo Make a good makefile
 * \todo We should have some kind of mdm initialization routine based on
 * a config file to specify loglevel, log destination, etc.
 * \todo It would be nice to unify all mdm_test_driver_* into one and unique
 * test where you can select interactively or by command line the driver to
 * test.
 * \todo Static vs. shared? maybe static is better...
 * \todo Support binary commands, we only support text commands now.
 * http://www.faqs.org/docs/Linux-HOWTO/Program-Library-HOWTO.html
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#ifndef	_MDM_H_
#define	_MDM_H_

#define	MDM_VERSION_MAJOR	0 /*!< Major version number. */
#define	MDM_VERSION_MINOR	1 /*!< Minor version number. */
#define	MDM_VERSION_MICRO	1 /*!< Micro version number. */

#define	MDM_DEBUG_MESSAGES 1 /*!< Set to 1 to include debug messages in
	compilation. */

#include	<mdm_operation.h>
#include	<mdm_config.h>
#include	<mdm_log.h>
#include	<mdm_mm.h>
#include	<mdm_connection.h>
#include	<mdm_driver_dummy.h>
#include	<mdm_driver_rawtcp.h>
#include	<mdm_driver_rawtcp_server.h>
#include	<mdm_driver_telnet.h>
#include	<mdm_driver_serial.h>
#include	<mdm_driver_ssh2.h>
#include	<mdm_device.h>
#include	<mdm_device_dslam_alcatel_73xx.h>
#include	<mdm_device_dslam_zte_8426.h>
#include	<mdm_device_dslam_zte_9xxx.h>
#include	<mdm_device_dslam_huawei_ma5600.h>


extern void mdm_init(mdm_log_priority_t, int, int, int, const char *);
extern void mdm_cleanup(void);

#ifndef	__FreeBSD__
extern char *fgetln(FILE *fp, size_t *lenp);
#endif

#endif
