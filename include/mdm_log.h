/*!
 * \file mdm_log.h Logging constants, types, and definitions. Interface for
 * mdm_log.c
 *
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#ifndef	_MDM_LOG_H_
#define	_MDM_LOG_H_

/*******************************************************************************
 * Log Constants & Definitions.
 ******************************************************************************/

/*!
 * This will control the maximum allowed log buffer size. This means that
 * now log can be larger than this amount of bytes. 
 */
#define	MDM_LOG_MAX_SIZE	1024

/*!
 * The log priority relates to the log level. You can use #mdm_config_t.loglevel
 * to be more or less verbose, thus, only logging the messages with the
 * desired priority.
 */
typedef enum
{
	MDM_LOG_ERROR, /*!< Indicates an unrecoverable error. */
	MDM_LOG_WARN, /*!< Indicates a warning, or recoverable error, not fatal. */
	MDM_LOG_INFO, /*!< Information, normal logging. */
	MDM_LOG_PERF, /*!< Performance Information. */
	MDM_LOG_DEBUG /*!< Debug output. */
} mdm_log_priority_t;

/*! \internal
	GNU CPP permits you to completely omit the variable arguments in this way.
	In the above examples, the compiler would complain, though since the
	expansion of the macro still has the extra comma after the format string.
	
	To help solve this problem, CPP behaves specially for variable arguments
	used with the token paste operator, ##. If instead you write
	
	https://ascendbugs.cheme.cmu.edu/view.php?id=179
	The variadic macros defined in utilities/error.h cause compiler
	warnings using gcc with -std=c99. This is using gcc/mingw.
	Not sure of gcc/linux.
	
	There are 2 issues with the current implemention, ansi-wise.
	
	1. the macro definitions when __GNUC__ is defined use a gnu
	extension which is non-ansi and results in a compiler warning in every
	source file (since all include ascConfig.h, which includes error.h):
	
	\#ifdef __GNUC__
	\# define ERROR_REPORTER_DEBUG(MSG,args...) error_reporter
	(ASC_PROG_NOTE,__FILE__,__LINE__,"%s: " MSG, __func__, \#\#args)
	\# define CONSOLE_DEBUG(MSG,args...)
	fprintf(stderr,"%s:%d (%s): " MSG "\n", __FILE__,__LINE__,__func__, \#\#args)
	
	../base/generic/utilities/error.h:59:39:
	        warning: ISO C does not permit named variadic macros
	../base/generic/utilities/error.h:60:32:
	        warning: ISO C does not permit named variadic macros
	
	2. In ansi, variadic macros require that at least 1 of the variable
	arguments be actually specified when the macro is used. In the
	current definition of the macros, a compiler warning results when
	the macro is used with only a single argument. For example:
	
	instantiate.c:5359:
	  CONSOLE_DEBUG("ENTERED ExecuteBlackBoxExt\n");
	
	..\\base\\generic\\compiler\\instantiate.c:5359:47:
	warning: ISO C99 requires rest arguments to be used
	
	
	One solution to both these problems is to elminate the named argument (fmt)
*/

/*! \def MDM_LOG(...)
	This will end up calling #mdm_log with priority #MDM_LOG_INFO
*/
#define MDM_LOG(...) \
	mdm_log( \
		MDM_LOG_INFO, __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__ \
	)

/*! \def MDM_LOGERROR(...)
	This will end up calling #mdm_log with priority #MDM_LOG_ERROR
*/
#define MDM_LOGERROR(...) \
	mdm_log( \
		MDM_LOG_ERROR, __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__ \
	)

/*! \def MDM_LOGWARN(...)
	This will end up calling #mdm_log with priority #MDM_LOG_WARN
*/
#define MDM_LOGWARN(...) \
	mdm_log( \
		MDM_LOG_WARN, __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__ \
	)

/*! \def MDM_LOGDEBUG(...)
	This will end up calling #mdm_log with priority #MDM_LOG_DEBUG
*/
#define MDM_LOGDEBUG(...) \
	mdm_log( \
		MDM_LOG_DEBUG, __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__ \
	)

/*! \def MDM_LOGDEBUG(...)
	This will end up calling #mdm_log with priority #MDM_LOG_DEBUG
*/
#define MDM_LOGPERF(...) \
	mdm_log( \
		MDM_LOG_PERF, __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__ \
	)

extern void mdm_log(
	mdm_log_priority_t , const char *, char *, int, const char *, ...
);
extern const char *mdm_log_p2string(mdm_log_priority_t);
extern int mdm_log_isvalid_priority(mdm_log_priority_t);

#endif
