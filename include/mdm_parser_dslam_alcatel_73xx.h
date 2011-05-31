/*!
 * \file mdm_parser_dslam_alcatel_73xx.h Parser for dslams alcatel 73xx.
 *
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#ifndef	_MDM_PARSER_DSLAM_ALCATEL_73xx_H_
#define	_MDM_PARSER_DSLAM_ALCATEL_73xx_H_

#include	<mdm_operation.h>
#include	<mdm_device.h>

extern void dslam_alcatel_73xx_uptime(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_soft_version(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_show_slots(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_show_internet(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_show_slot_port(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_show_spectrum_profiles(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_show_service_profiles(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_show_ports(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_get_routes(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_get_port_description(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_port_profile(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_port_data_near_end(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_port_data_far_end(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_get_all_pvc(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_get_bridge(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_get_backup_status(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_get_databases(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_get_slot_vlans(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_load_ports(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_get_alarm_types(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_get_alarm_count(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_get_eth_list(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_get_eth_statistics(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_load_ports_all(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_configure_timezone(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_get_ntp(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_create_service_profile(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_delete_service_profile(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_nop(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void
dslam_alcatel_73xx_ramodeup_service_profile(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_ramodedown_service_profile(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_pbitrateup_service_profile(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_pbitratedown_service_profile(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void
dslam_alcatel_73xx_mdelayup_service_profile(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void
dslam_alcatel_73xx_mdelaydown_service_profile(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void
dslam_alcatel_73xx_maxbitrateup_service_profile(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void
dslam_alcatel_73xx_maxbitratedown_service_profile(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_minbitrateup_service_profile(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_minbitratedown_service_profile(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);

extern void dslam_alcatel_73xx_get_pvc_mac(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_get_cpu_info(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_get_mem_info(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_get_misc_shub_info(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_get_shub_version(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_get_shub_ip(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_alcatel_73xx_get_syslog_destinations(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
);
extern void dslam_alcatel_73xx_get_syslog_routes(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
);

#endif
