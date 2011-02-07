/*!
 * \file mdm_log.c Logging functions.
 *
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<string.h>
#include	<libgen.h>
#include	<stdarg.h>
#include	<syslog.h>
#include	<time.h>
#include	<stdarg.h>
#include	<syslog.h>
#include	<pthread.h>
#include	<mdm.h>

/*!
 * This will hold the strings for each logging priority.
 */
static const char *MDM_LOG_PRIORITY_STRING[] = {
	"ERROR", "WARN", "INFO", "PERFORMANCE", "DEBUG"
};

/*! To avoid race conditions when accessing log file. */
static pthread_mutex_t file_log_mutex = PTHREAD_MUTEX_INITIALIZER;
static FILE *stream = NULL;

/*******************************************************************************
 * CODE STARTS.
 ******************************************************************************/

/*!
 * \param p Log priority to check.
 * \return 1 (true) if priority is valid. 0 (false) otherwise. A valid 
 * priority is contained between the higher and lower values of
 * #mdm_log_priority_t, which are #MDM_LOG_DEBUG and #MDM_LOG_ERROR respectively.
 */
int
mdm_log_isvalid_priority(mdm_log_priority_t p)
{
	return (p <= MDM_LOG_DEBUG && p >= MDM_LOG_ERROR);
}

/*!
 * Returns the associated string (human readable form) of a log priority.
 * \return NULL if p is not valid. Otherwise, a const char * with the string
 * associated with this priority.
 */
const char *
mdm_log_p2string(mdm_log_priority_t p)
{
	return mdm_log_isvalid_priority(p) ? MDM_LOG_PRIORITY_STRING[p] : NULL;
}

/*!
 * This is the main logging function. You should not need to call this one
 * directly. Use the predefined macros in mdm_log.h: 
 * #MDM_LOG, #MDM_LOGERROR, #MDM_LOGDEBUG, #MDM_LOGWARN.
 * Only messages with priority <= #mdm_config_t.loglevel will be logged.
 *
 * \param p Log priority.
 * \param function Name of the funcion calling (i.e: __FUNCTION__).
 * \param file Name of the file calling (i.e: __FILE__).
 * \param line Number of line of the file calling (i.e: __LINE__).
 * \param fmt printf format
 * \param ... Variable arguments follow.
 * \return void
 */
void
mdm_log(
	mdm_log_priority_t p, const char *function,
	char *file, int line, const char *fmt, ...
)
{
	va_list ap;
	int buffer_log_len = MDM_LOG_MAX_SIZE - 1;
	char *buffer_log = NULL;
	char buffer_time[32];
	int buffer_msg_len = buffer_log_len - sizeof(buffer_time);
	char *buffer_msg = NULL;
	time_t current_time;
	struct tm current_localtime;
	int syslog_prio = 0;
	if(
		mdm_log_isvalid_priority(p) &&
		p <= mdm_config_get_loglevel() &&
		(mdm_config_get_logsyslog() || mdm_config_get_logfile())
	)
	{
		buffer_log = (char *)malloc(buffer_log_len);
		if(buffer_log == NULL)
			return;
		buffer_msg = (char *)malloc(buffer_msg_len);
		if(buffer_msg == NULL)
		{
			free(buffer_log);
			return;
		}
		memset(buffer_log, 0, buffer_log_len);
		memset(buffer_msg, 0, buffer_msg_len);
		/* Do variadic arguments, construct logmessage */
		va_start(ap, fmt);
		vsnprintf(buffer_msg, buffer_msg_len, fmt, ap);
		va_end(ap);

		/* Log it! */
		if(mdm_config_get_logsyslog())
		{
			switch(p)
			{
				case MDM_LOG_ERROR:
					syslog_prio = LOG_ERR;
					break;
				case MDM_LOG_WARN:
					syslog_prio = LOG_WARNING;
					break;
				case MDM_LOG_PERF:
				case MDM_LOG_INFO:
					syslog_prio = LOG_INFO;
					break;
				case MDM_LOG_DEBUG:
					syslog_prio = LOG_DEBUG;
					break;
				default:
					syslog_prio = LOG_NOTICE;
					break;
			}
			syslog(
				syslog_prio, "%s - %s:%d:%s - %s",
				mdm_log_p2string(p), basename(file), line, function, buffer_msg
			);
		}

		if(mdm_config_get_logfile())
		{
			pthread_mutex_lock(&file_log_mutex);
			if(stream == NULL)
			{
				stream = fopen(mdm_config_get_logpath(), "a");
			}
			if(stream != NULL)
			{
				/* Get current time() and format it in a string, logtime */
				time(&current_time);
				localtime_r(&current_time, &current_localtime);
				strftime(
					buffer_time, sizeof(buffer_time),
					"%m/%d/%y - %H:%M:%S", &current_localtime
				);
			
				/* Prepare full string: logtime - loginfo - logmessage */
				snprintf(
					buffer_log, buffer_msg_len, "%s - %s - %s:%d:%s - %s\n",
					buffer_time, mdm_log_p2string(p), file, line,
					function, buffer_msg
				);
				if(fwrite(buffer_log, strlen(buffer_log), 1, stream) <= 0) {
					fclose(stream);
					stream = NULL;
				} else {
					fflush(stream);
				}
			}
			pthread_mutex_unlock(&file_log_mutex);
		}
		free(buffer_msg);
		free(buffer_log);
	}
	return;
}
/*******************************************************************************
 * CODE ENDS.
 ******************************************************************************/
