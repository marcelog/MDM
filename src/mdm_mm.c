/*!
 * \file mdm_mm.c Memory management functions.
 *
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#include	<stdio.h>
#include	<string.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<pthread.h>
#include	<mdm.h>
#include	<mdm_mm.h>

/*******************************************************************************
 * CODE STARTS.
 ******************************************************************************/

#if MDM_DEBUG_MESSAGES > 0
/*!
 * This will allocate a chunk of memory and zero it.
 * \param size Total amount of memory to allocate.
 * \param function Caller function.
 * \param line Caller line.
 * \return Will return NULL on error or if size < 1. Otherwise, it will
 * return a pointer to a newly allocated and zero'ed memory buffer.
 */
void *
mdm_malloc(size_t size, const char *function, int line)
#else
/*!
 * This will allocate a chunk of memory and zero it.
 * \param size Total amount of memory to allocate.
 * \return Will return NULL on error or if size < 1. Otherwise, it will
 * return a pointer to a newly allocated and zero'ed memory buffer.
 */
void *
mdm_malloc(size_t size)
#endif
{
	void *p = NULL;

#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Allocating %ld bytes.", size);
#endif

	p = malloc(size);
	if(p != NULL)
	{
		memset(p, 0, size);
#if MDM_DEBUG_MESSAGES > 0
		MDM_LOGDEBUG("Allocated %p from %s:%d", p, function, line);
#endif
	}
	
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Done.");
#endif

	return p;
}

/*!
 * This will try to realloc more memory for the given buffer.
 * \param buffer Buffer to resize.
 * \param size New buffer size.
 * \return New buffer with new size.
 */
void *
mdm_mrealloc(void *buffer, size_t size)
{
	return realloc(buffer, size);
}


#if MDM_DEBUG_MESSAGES > 0
/*!
 * This will free the memory allocated by #mdm_malloc. Relates to ticket #24.
 *
 * Please use #MDM_MFREE macro instead of calling #mdm_mfree directly.
 * This is needed because we are going to free *any* kind of pointer and will
 * try to check (before calling free()) that this pointer is not null. See
 * #mdm_mfree doc. So we need a generic pointer-to-pointer to make this work,
 * breaking strict aliasing rules.
 *
 * \param buffer Pointer to memory buffer pointer.
 * \param function Caller function.
 * \param line Caller line.
 */
void
mdm_mfree(void **buffer, const char *function, int line)
#else
/*!
 * This will free the memory allocated by #mdm_malloc. Relates to ticket #24.
 *
 * Please use #MDM_MFREE macro instead of calling #mdm_mfree directly.
 * This is needed because we are going to free *any* kind of pointer and will
 * try to check (before calling free()) that this pointer is not null. See
 * #mdm_mfree doc. So we need a generic pointer-to-pointer to make this work,
 * breaking strict aliasing rules.
 *
 * \param buffer Pointer to memory buffer pointer.
 */
void
mdm_mfree(void **buffer)
#endif
{
	if(*buffer == NULL)
	{
		/*MDM_LOGWARN("Double free from %s:%d", function, line);*/
		return;
	}
#if MDM_DEBUG_MESSAGES > 0
	MDM_LOGDEBUG("Freeing %p from %s:%d", *buffer, function, line);
#endif
	free(*buffer);
	*buffer = NULL;
	return;
}
/*******************************************************************************
 * CODE ENDS.
 ******************************************************************************/
