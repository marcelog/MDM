/*!
 * \file mdm_parser_dslam_siemens_hix5300.c Parsers for dslams siemens hix5300.
 *
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<string.h>
#include	<libxml/parser.h>
#include	<libxml/tree.h>
#include	<mdm_parser_dslam_siemens_hix5300.h>
#include	<mdm.h>

/*******************************************************************************
 * CODE STARTS.
 ******************************************************************************/
/**
 * Dummy.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_siemens_hix5300_nop(
    mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
    d->exec_buffer_post_len = 0;
    return;
}
/*******************************************************************************
 * CODE ENDS.
 ******************************************************************************/
