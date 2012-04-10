/*!
 * \file mdm_parser_dslam_huawei_ma5600.c Parsers for dslams huawei ma5600.
 *
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#ifndef	_MDM_PARSER_DSLAM_HUAWEI_MA5600_H_
#define	_MDM_PARSER_DSLAM_HUAWEI_MA5600_H_

#include	<mdm_operation.h>
#include	<mdm_device.h>

extern void dslam_huawei_ma5600_routes(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_huawei_ma5600_timezone(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_huawei_ma5600_cpu(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_huawei_ma5600_service_ports(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_huawei_ma5600_service_profile(
	mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_huawei_ma5600_alarms(
    mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_huawei_ma5600_boards(
    mdm_device_descriptor_t *, mdm_operation_result_t *
);
extern void dslam_huawei_ma5600_frames(
    mdm_device_descriptor_t *, mdm_operation_result_t *
);

#endif
