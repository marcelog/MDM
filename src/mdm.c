/*!
 * \file mdm.c Backend mdm functions to control library behavior.
 *
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#include	<mdm.h>
#include	<syslog.h>

/*******************************************************************************
 * CODE STARTS.
 ******************************************************************************/

/*!
 * Call this at the start of your program to initialize connections and configs.
 * \param loglevel Priority loglevel. For #mdm_config_t.loglevel
 * \param logsyslog Enable or disable syslog. For #mdm_config_t.logsyslog
 * \param logfile Enable or disable file logging. For #mdm_config_t.logfile
 * \param fulldebug Enable or disable full debugging. For
 * #mdm_config_t.fulldebug
 * \param logpath File path for file logging. For #mdm_config_t.logpath
 */
void
mdm_init(
	mdm_log_priority_t loglevel,
	int logsyslog,
	int logfile,
	int fulldebug,
	const char *logpath
)
{
	if(logsyslog)
	{
		openlog("MDM", LOG_CONS | LOG_NDELAY | LOG_PID, LOG_USER);
	}
	
	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif

	mdm_connection_init();
	mdm_config_set_loglevel(loglevel);
	mdm_config_set_logsyslog(logsyslog);
	mdm_config_set_logfile(logfile);
	mdm_config_set_fulldebug(fulldebug);
	mdm_config_set_logpath(logpath);

	/* Done. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif
	return;
}

/*!
 * Call this at the end of your program to cleanup connections and configs.
 */
void
mdm_cleanup(void)
{
	/* Start. */
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Start.");
#endif

	mdm_connection_cleanup();
	mdm_config_cleanup();

#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif

	if(mdm_config_get_logsyslog())
	{
		closelog();
	}

	/* Done. */
	return;
}

#ifndef	__FreeBSD__
/* fgetln.c    - a replacement for FreeBSD's fgetln(3) in Linux */
/*-
 * Copyright (c) 2003 Nick Leuta
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include <stdio.h>
#include <stdlib.h>

/* This implementation of fgetln(3) returns pointer to data, which may be
 * modified by later calls. Unlike original FreeBSD implementation, the
 * allocated (with help of malloc()/realloc()) memory isn't automatically
 * free()'d when the stream is fclose()'d. The returned pointer may be free()'d
 * by the application at any time, but in some cases this function itself frees
 * the memory.
 */

/* Emulation of FreeBSD's style of the implementation of streams */
char *__fgetln_int_buf = NULL;
/* Size of the allocated memory */
size_t __fgetln_int_len = 0;

char *
fgetln(FILE *fp, size_t *lenp)
{
    /* SKYNICK: Implementation note: "== -1" isn't equal to "< 0"... May be
     * getline(3) page will have more notes/examples in future... */
    if(((*lenp) = getline (&__fgetln_int_buf, &__fgetln_int_len, fp)) == -1) {
	if (__fgetln_int_buf != NULL) {
	    free(__fgetln_int_buf);
	}
	__fgetln_int_buf = NULL;
	__fgetln_int_len = 0;
	(*lenp) = 0;
    }

    return __fgetln_int_buf;
}
#endif
/*******************************************************************************
 * CODE ENDS.
 ******************************************************************************/
