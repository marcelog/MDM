/*!
 * \file mdm_config.h Function and types definition regarding the library
 * configuration for logging, etc. There is currently only one instance of
 * the configuration structure, residing in mdm_config.c and accessed by
 * the functions mdm_config_[sg]et_*
 *
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#ifndef	_MDM_CONFIG_H_
#define	_MDM_CONFIG_H_

#include	<stdio.h>
#include	<mdm_log.h>

/*!
 * This will be our library configuration. The implementation may suffer if you
 * choose to use static library or shared library, so you've been warned.
 */
typedef struct
{
	mdm_log_priority_t loglevel; /*!< Adjust this to be more or less verbose.
		Will log eveything <= than this value. See #mdm_config_set_loglevel. */
	int fulldebug; /*!< Really annoying debug messages!. */
	int logsyslog; /*!< If 1, will log to syslog. */
	int logfile; /*!< If 1, will log to #logpath. */
	char logpath[FILENAME_MAX]; /*!< Filename to use when logging to file. */	
} mdm_config_t;

extern void mdm_config_set_loglevel(mdm_log_priority_t);
extern mdm_log_priority_t mdm_config_get_loglevel(void);

extern void mdm_config_set_fulldebug(int);
extern int mdm_config_get_fulldebug(void);

extern void mdm_config_set_logsyslog(int);
extern int mdm_config_get_logsyslog(void);

extern void mdm_config_set_logfile(int);
extern int mdm_config_get_logfile(void);

extern void mdm_config_set_logpath(const char *);
extern const char *mdm_config_get_logpath(void);

extern void mdm_config_cleanup(void);

#endif
