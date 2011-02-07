/*!
 * \file mdm_mm.h Memory management functions, constants, types,
 * and definitions.
 *
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#ifndef	_MDM_MM_H_
#define	_MDM_MM_H_

#include	<mdm.h>
#include	<unistd.h>
extern void *mdm_mrealloc(void *, size_t);

/*!
 * Please use this macro instead of calling #mdm_mfree directly.
 * Relates to ticket #24.
 * This macro needs -fno-strict-aliasing in cflags because it will try to
 * cast any pointer-to-pointer to void **, breaking strict aliasing rules. 
 * Please also see #mdm_mfree doc.
 */
#if MDM_DEBUG_MESSAGES > 0
extern void mdm_mfree(void **, const char *, int);
#define	MDM_MFREE(x)	mdm_mfree((void **)x, __FUNCTION__, __LINE__)
#else
extern void mdm_mfree(void **);
#define	MDM_MFREE(x)	mdm_mfree((void **)x)
#endif

/*!
 * Please use this macro instead of calling #mdm_malloc directly, it will
 * help debugging.
 */
#if MDM_DEBUG_MESSAGES > 0
extern void *mdm_malloc(size_t, const char *, int);
#define	MDM_MALLOC(x)	mdm_malloc(x, __FUNCTION__, __LINE__)
#else
extern void *mdm_malloc(size_t);
#define	MDM_MALLOC(x)	mdm_malloc(x)
#endif

#endif
