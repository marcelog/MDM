/*!
 * \file mdm_parser_dslam_zte_9xxx.c Parsers for dslams zte 9000 series.
 *
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#ifndef	_MDM_PARSER_DSLAM_ZTE_9xxx_H_
#define	_MDM_PARSER_DSLAM_ZTE_9xxx_H_

#include	<mdm_operation.h>
#include	<mdm_device.h>

extern void dslam_zte_9xxx_show_slots(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_zte_9xxx_get_service_profiles(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_zte_9xxx_soft_version(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_zte_9xxx_port_pvc(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_zte_9xxx_uptime(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_zte_9xxx_show_slot_ports(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_zte_9xxx_show_scb(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_zte_9xxx_show_slot(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_zte_9xxx_show_port(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_zte_9xxx_show_running_config(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_zte_9xxx_configure_timezone(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_zte_9xxx_get_ntp(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_zte_9xxx_get_service_profile(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_zte_9xxx_create_service_profile(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_zte_9xxx_delete_service_profile(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_zte_9xxx_configure_service_profile(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_zte_9xxx_nop(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);

extern void dslam_zte_9xxx_get_port_mac(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_zte_9xxx_get_routes(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
#endif
