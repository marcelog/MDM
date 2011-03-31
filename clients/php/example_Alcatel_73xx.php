<?php
/**
 * Example for Alcatel 73xx.
 * argv[0] == program name
 * argv[1] == server ip
 * argv[2] == server port
 * argv[3] == connection timeout
 * argv[4] == recv timeout
 *
 * PHP Version 5
 *
 * @category MDMDevice
 * @package  Examples
 * @author   Marcelo Gornstein <marcelog@gmail.com>
 * @license  http://www.noneyet.ar/ Apache License 2.0
 * @version  SVN: $Id$
 * @link     http://www.noneyet.ar/
 *
 * Copyright 2011 Marcelo Gornstein <marcelog@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
require_once 'MDMAlcatel73xx.php';

/* Initialize. */
/*
error_reporting(0);
ini_set('display_errors', false);
*/

/** This is where our schema file is. */
define('SCHEMA', '/etc/mdm/mdm_response_alcatel_73xx.xsd');

/** Set this to true if you want to use the server or the cli binaries. */
define('USE_SERVER', true);

/** Execute these commands. */
$cmds = array(
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_UPTIME,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_SOFT_VERSION,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_SHOW_SLOTS,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_SHOW_SLOT_PORT,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_SHOW_SPECTRUM_PROFILE_DETAIL,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_SHOW_SERVICE_PROFILE,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_SHOW_ALL_PORTS,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_GET_IP,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_GET_ROUTES,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_SHOW_PORT_DESCRIPTION,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_SHOW_PORT_PROFILE,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_SHOW_PORT_DATA_NEAR_END,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_SHOW_PORT_DATA_FAR_END,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_SHOW_PORT_ATM_PVC,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_SHOW_PORT_INFO_VLAN,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_SHOW_BACKUP_STATUS,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_SHOW_DOWNLOAD_BACKUP_DB2,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_SHOW_PORT_ATM_PVC2,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_LOAD_PORTS_SLOT,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_LOAD_PORTS_ALL,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_PORTS_GET_TRAFFIC,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_PORTS_GET,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_ALARM_GET_COUNT,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_SET_TIMEZONE,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_GET_NTP,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_CREATE_SERVICE_PROFILE,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_CFG_RAMODEUP_SERVICE_PROFILE,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_CFG_RAMODEDOWN_SERVICE_PROFILE,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_CFG_PBITRATEUP_SERVICE_PROFILE,
    MDMAlcatel73xx::
        $MDM_DSLAM_ALCATEL_73xx_CMD_CFG_PBITRATEDOWN_SERVICE_PROFILE,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_CFG_MDELAYUP_SERVICE_PROFILE,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_CFG_MDELAYDOWN_SERVICE_PROFILE,
    MDMAlcatel73xx::
        $MDM_DSLAM_ALCATEL_73xx_CMD_CFG_MAXBITRATEUP_SERVICE_PROFILE,
    MDMAlcatel73xx::
        $MDM_DSLAM_ALCATEL_73xx_CMD_CFG_MAXBITRATEDOWN_SERVICE_PROFILE,
    MDMAlcatel73xx::
        $MDM_DSLAM_ALCATEL_73xx_CMD_CFG_MINBITRATEUP_SERVICE_PROFILE,
    MDMAlcatel73xx::
        $MDM_DSLAM_ALCATEL_73xx_CMD_CFG_MINBITRATEDOWN_SERVICE_PROFILE,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_DELETE_SERVICE_PROFILE
/*
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_PORT_ENABLE,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_PORT_DISABLE,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_REBOOT_HOT,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_REBOOT_ALL,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_REBOOT_SHUB,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_SYSLOG_GET_1,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_SYSLOG_GET_2,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_SHOW_ATM_PVC_TRAFFIC_1,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_SHOW_ATM_PVC_TRAFFIC_2,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_SHOW_ATM_PVC_TRAFFIC_3,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_ALARM_GET_2,
    MDMAlcatel73xx::$MDM_DSLAM_ALCATEL_73xx_CMD_ALARM_GET,
*/
);

/** With these arguments. */
$args = array(
    null,
    null,
    null,
    '1/1/1/1',
    null,
    null,
    null,
    null,
    null,
    '1/1/1/1',
    '1/1/1/1',
    '1/1/1/1',
    '1/1/1/1',
    null,
    '1/1/1/1:1:33',
    null,
    null,
    '1/1/1',
    '1/1/1',
    null,
    '3',
    null,
    'critical',
    '-180',
    null,
    '88,profiletest',
    '88,operator-ctrld',
    '88,operator-ctrld',
    '88,1024',
    '88,1024',
    '88,1',
    '88,2',
    '88,256',
    '88,256',
    '88,512',
    '88,512',
    '88'
);

/** With these connections. */
$conns = array(
    MDMDevice::$MDM_CONNECTION_TELNET, MDMDevice::$MDM_CONNECTION_SSH2
    /*MDMDevice::$MDM_CONNECTION_TELNET, MDMDevice::$MDM_CONNECTION_SSH2*/
);

/** In these targets. */
$tgts = array(
    '127.0.0.1:23', '127.0.0.1:22'
);

/** Descriptions for commands. */
$descs = array(
    'Show Uptime',
    'Show Software Version',
    'Show Slots',
    'Show Slot Port',
    'Show Spectrum Profile',
    'Show Service Profile',
    'Show Ports',
    'Show Network Information',
    'Show Routes Information',
    'Show Port Description',
    'Show Port Profile',
    'Show Near End',
    'Show Far End',
    'Show ATM PVC',
    'Show VLAN',
    'Show Backup Status',
    'Show Download Backup DB2',
    'Show Port ATM PVC',
    'Load Ports 2',
    'Load Ports 1',
    'Show Port Traffic',
    'Show Eth Ports',
    'Get Alarm count',
    'Configure timezone',
    'Get NTP',
    'Create Service Profile',
    'Configure Service Profile ra-mode-up',
    'Configure Service Profile ra-mode-down',
    'Configure Service Profile plan-bitrate-up',
    'Configure Service Profile plan-bitrate-down',
    'Configure Service Profile max-delay-up',
    'Configure Service Profile max-delay-down',
    'Configure Service Profile max-bitrate-up',
    'Configure Service Profile max-bitrate-down',
    'Configure Service Profile min-bitrate-up',
    'Configure Service Profile min-bitrate-down',
    'Delete Service Profile'
);

/** Methods to get responses for each command. */
$responses = array(
    'getUptime',
    'getSoftwareVersions',
    'getSlotList',
    'getSlotPort',
    'getSpectrumProfiles',
    'getServiceProfiles',
    'getPortList',
    'getInternetList',
    'getInternetRoutes',
    'getPortDescription',
    'getPortProfile',
    'getOpDataNearEnd',
    'getOpDataFarEnd',
    'getPVC',
    'getVlanInfo',
    'getBackupStatus',
    'getFirmwares',
    'getVlanList',
    'getPortDescriptions',
    'getPortListFull',
    'getEthStatistics',
    'getEths',
    'getAlarmCount',
    null,
    'getNTP',
    null,
    null,
    null,
    null,
    null,
    null,
    null,
    null,
    null,
    null,
    null,
    null,
    null,
    null,
    null,
    null,
    null
);

/**
 * This will log messages to console with a date/time format.
 *
 * @param string $msg Message to be logged.
 *
 * @return void.
 */
function myLog($msg)
{
    echo date('m/d/Y - H:i:s - ', time()) . 'Alcatel73xxx - ' . $msg . "\n";
    return;
}

$device = null;
try
{
    /* Instantiate devices. */
    $connsCount = count($conns);
    $devices = array();
    for ($i = 0; $i < $connsCount; $i++) {
        $devices[] = new MDMAlcatel73xx(
            $argv[1], (int)$argv[2], $conns[$i], $tgts[$i],
            'user', 'pass', SCHEMA
        );
    }

    /* Do. */
    myLog('Start.');
    $cmdCount = count($cmds);
    for ($j = 0; $j < $connsCount; $j++) {
        for ($i = 0; $i < $cmdCount; $i++) {
            myLog(
                $descs[$i] . ' ' . ($args[$i] != null ? $args[$i] : '') . ' ' .
                '(' . intval($i) . ') ' .
                MDMDevice::getConnectionName($conns[$j])
            );
            $device = $devices[$j];
            if (USE_SERVER) {
                $device->exec(
                    (int)$argv[3], (int)$argv[4], $cmds[$i], 100000, 300000,
                    $args[$i]
                );
                $resultCode = $device->getResponseCode();
                $resultMsg = $device->getResponseMessage();
                myLog($device->getRawResponse());
                if ($resultCode != 0) {
                    throw new Exception(
                        'Command failed: ' . $resultCode . ': ' . $resultMsg
                    );
                }
                if ($responses[$i] != null) {
                    myLog(print_r($device->$responses[$i](), true));
                }
            } else {
                $output = array();
                $ret = 0;
                $cmd = '../../bin/mdm_test_device_dslam_alcatel_73xx ' .
                    intval($conns[$j]) . ' ' .
                    (int)$argv[3] . ' ' . (int)$argv[4] . ' ' . $tgts[$j] . 
                    ' user pass ' . intval($cmds[$i]) . ' ' .
                    ($args[$i] != null ? $args[$i] : '')
                ;
                myLog($cmd);
                exec($cmd, $output, $ret);
                if ($ret != 0) {
                    throw new Exception('Command returned error.');
                }
                myLog(print_r($output, true));
            }
        }
    }
} catch(Exception $e) {
    myLog('Error: ' . $e->getMessage());
}

myLog('Done.');

?>
