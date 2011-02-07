/*!
 * \file mdm_config.c Functions for configuring mdm. There is currently only
 * one instance of the configuration structure, residing here and accessed by
 * the functions mdm_config_[sg]et_*
 *
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#include	<stdio.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<string.h>
#include	<mdm_mm.h>
#include	<mdm_config.h>

static mdm_config_t mdm_config; /*!< Only one configuration per
	instance. Watch out for shared or static linking! */

/*******************************************************************************
 * CODE STARTS.
 ******************************************************************************/

/*!
 * Used to set loglevel. If loglevel is < 0, will set 0. Will change
 * #mdm_config_t.loglevel.
 * \param loglevel Log level based on priorities.
 */
void
mdm_config_set_loglevel(mdm_log_priority_t loglevel)
{
	mdm_config.loglevel = loglevel;
	return;
}

/*!
 * Used to get current loglevel from #mdm_config_t.loglevel.
 * \return Current log priority.
 */
mdm_log_priority_t
mdm_config_get_loglevel(void)
{
	return mdm_config.loglevel;
}

/*!
 * Used to set fulldebug. If fulldebug is < 0, will set 0. Full debug will make
 * a lot of output besides normal debugging information. See
 * #mdm_config_t.fulldebug
 * \param fulldebug Set to 1 if full debug is desired. 0 otherwise.
 */
void
mdm_config_set_fulldebug(int fulldebug)
{
	mdm_config.fulldebug = (fulldebug > 0);
	return;
}

/*!
 * Used to get fulldebug from #mdm_config_t.fulldebug
 */
int
mdm_config_get_fulldebug(void)
{
	return mdm_config.fulldebug;
}

/*!
 * Used to set #mdm_config_t.logsyslog, triggering on or off the logging to 
 * syslog.
 * \param logsyslog 1 to enable logging to syslog, 0 to disable.
 */
void
mdm_config_set_logsyslog(int logsyslog)
{
	mdm_config.logsyslog = (logsyslog > 0);
	return;
}

/*!
 * Used to get #mdm_config_t.logsyslog.
 * \return 0 if not logging to syslog.
 */
int
mdm_config_get_logsyslog(void)
{
	return mdm_config.logsyslog;
}

/*!
 * Used to set #mdm_config_t.logfile, triggering on or off the logging to 
 * #mdm_config_t.logpath. If logfile < 0, will set 0.
 * \param logfile 1 to enable logging to filepath, 0 to disable.
 */
void
mdm_config_set_logfile(int logfile)
{
	mdm_config.logfile = (logfile > 0);
	return;
}

/*!
 * Used to get #mdm_config_t.logfile, meaning if currently logging to
 * #mdm_config_t.logpath.
 * \return Current log priority. Could be NULL.
 */
int
mdm_config_get_logfile(void)
{
	return mdm_config.logfile;
}

/*!
 * Used to set #mdm_config_t.logpath. To trigger file
 * logging see #mdm_config_get_logfile and #mdm_config_set_logfile.
 * \param logpath Filename to use when logging to file.
 */
void
mdm_config_set_logpath(const char *logpath)
{
	int logpathlen;
	logpathlen = strlen(logpath);
	memcpy(mdm_config.logpath, logpath, logpathlen);
	mdm_config.logpath[logpathlen] = 0;
	return;
}

/*!
 * Used to get current logpath from #mdm_config_t.logpath.
 * \return Current log filename in use. Could be NULL.
 */
const char *
mdm_config_get_logpath(void)
{
	return mdm_config.logpath;
}

/*!
 * Used to cleanup configuration information when shutting down. Do not
 * call this directly, #mdm_cleanup will do this.
 */
void
mdm_config_cleanup(void)
{
	return;
}

/*******************************************************************************
 * CODE ENDS.
 ******************************************************************************/
