/*!
 * \file mdm_parser_dslam_zte_8426.c Parser for dslams zte 8426.
 *
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#ifndef	_MDM_PARSER_DSLAM_ZTE_8426_H_
#define	_MDM_PARSER_DSLAM_ZTE_8426_H_

#include	<mdm_operation.h>
#include	<mdm_device.h>

extern void dslam_zte_8426_soft_version(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_zte_8426_show_internet(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_zte_8426_show_slots(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_zte_8426_get_routes(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_zte_8426_get_all_pvc(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_zte_8426_get_macs(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_zte_8426_get_service_profiles(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_zte_8426_show_ports(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_zte_8426_create_service_profile(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_zte_8426_delete_service_profile(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_zte_8426_nop(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_zte_8426_reboot(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_zte_8426_get_service_profile(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);

extern void dslam_zte_8426_get_alarms(
       mdm_device_descriptor_t *, mdm_operation_result_t *
);

#endif
