/*!
 * \file mdm_operation.h Contains types, definitions, and constants regarding
 * actions and operations to be used along mdm.
 *
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#ifndef	_MDM_OPERATION_H_
#define	_MDM_OPERATION_H_

/*!
 * Used to identify operation results. See #mdm_operation_result_t
 */
typedef enum
{
	MDM_OP_OK, /*!< Last operation ok. */
	MDM_OP_ERROR /*!< Last operation failed. */
} mdm_operation_status_t;

/*!
 * This is a generic way to handle operation results across different drivers.
 */
typedef struct
{
	mdm_operation_status_t status; /*!< Last operation result. */
	char status_message[4096]; /*!< Will hold the string description of the
		last operation error. */
} mdm_operation_result_t;

#endif
