<?php
/**
 * Example for ZTE 9000 series.
 *
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
require_once 'MDMZTE9xxx.php';

/* Initialize. */
/*
error_reporting(0);
ini_set('display_errors', false);
*/

/** This is where our schema file is. */
define('SCHEMA', '/etc/mdm/mdm_response_zte_9xxx.xsd');

/** Set this to true if you want to use the server or the cli binaries. */
define('USE_SERVER', true);

/** Execute these commands. */
$cmds = array(
    MDMZTE9xxx::$MDM_DSLAM_ZTE_9xxx_CMD_NOP,
    MDMZTE9xxx::$MDM_DSLAM_ZTE_9xxx_CMD_SHOW_SLOTS,
    /*MDMZTE9xxx::$MDM_DSLAM_ZTE_9xxx_CMD_SHOW_RUNNING_CONFIG,*/
    MDMZTE9xxx::$MDM_DSLAM_ZTE_9xxx_CMD_SHOW_SERVICE_PROFILE,
    MDMZTE9xxx::$MDM_DSLAM_ZTE_9xxx_CMD_SOFT_VERSION,
    MDMZTE9xxx::$MDM_DSLAM_ZTE_9xxx_CMD_SHOW_PORT_ATM_PVC,
    MDMZTE9xxx::$MDM_DSLAM_ZTE_9xxx_CMD_SHOW_UPTIME,
    MDMZTE9xxx::$MDM_DSLAM_ZTE_9xxx_CMD_LOAD_PORTS_SLOT,
    MDMZTE9xxx::$MDM_DSLAM_ZTE_9xxx_CMD_SCB_DETAIL,
    MDMZTE9xxx::$MDM_DSLAM_ZTE_9xxx_CMD_SLOT_DETAIL,
    MDMZTE9xxx::$MDM_DSLAM_ZTE_9xxx_CMD_PORT_DETAIL,
    MDMZTE9xxx::$MDM_DSLAM_ZTE_9xxx_CMD_SET_TIMEZONE,
    MDMZTE9xxx::$MDM_DSLAM_ZTE_9xxx_CMD_GET_NTP,
    MDMZTE9xxx::$MDM_DSLAM_ZTE_9xxx_CMD_CREATE_SERVICE_PROFILE,
    MDMZTE9xxx::$MDM_DSLAM_ZTE_9xxx_CMD_CONFIGURE_SERVICE_PROFILE,
    MDMZTE9xxx::$MDM_DSLAM_ZTE_9xxx_CMD_DELETE_SERVICE_PROFILE,
    MDMZTE9xxx::$MDM_DSLAM_ZTE_9xxx_CMD_GET_SERVICE_PROFILE,
//    MDMZTE9xxx::$MDM_DSLAM_ZTE_9xxx_CMD_GET_SLOT_MAC,
//    MDMZTE9xxx::$MDM_DSLAM_ZTE_9xxx_CMD_GET_PORT_MAC
);

/** With these arguments. */
$args = array(
    null,
    null,
    /*null,*/
    null,
    null,
    '1/6',
    null,
    '1',
    null,
    '1',
    '1/6',
    '-3',
    null,
    'profiletest',
    'profiletest,32,33,34,35,36,42,43,44,45,46,3',
    'profiletest',
    'MAXXYABIRU'
);

/** With these connections. */
$conns = array(
    MDMDevice::$MDM_CONNECTION_TELNET
);

/** In these targets. */
$tgts = array('127.0.0.1:666');

/** Descriptions for commands. */
$descs = array(
    'NOP',
    'Show Slots',
    /*'Show Running Config',*/
    'Show Service Profiles',
    'Show Software Versions',
    'Show PVC',
    'Show Uptime',
    'Load Ports',
    'Show SCB',
    'Show Slot Detail',
    'Show Port Profile',
    'Configure Timezone',
    'Get NTP',
    'Create Profile',
    'Configure Profile',
    'Delete Profile',
    'Get Profile'
);

/** Methods to get responses for each command. */
$responses = array(
    null,
    array('getSlotList'),
    /*array('getRunningConfig'),*/
    array('getServiceProfiles'),
    array('getSoftwareVersions','getSoftwareBootVersion'),
    array('getPVC'),
    array('getUptime'),
    array('getPortListFull'),
    array('getSCBDetail'),
    array('getSlotDetail'),
    array('getPortProfile'),
    null,
    array('getNTP'),
    null,
    null,
    null,
    array('getServiceProfile')
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
    echo date('m/d/Y - H:i:s - ', time()) . 'ZTE9xxx - ' . $msg . "\n";
    return;
}

try
{
    /* Instantiate devices. */
    $connsCount = count($conns);
    $devices = array();
    for ($i = 0; $i < $connsCount; $i++) {
        $devices[] = new MDMZTE9xxx(
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
                    (int)$argv[3], (int)$argv[4], $cmds[$i], 1000, 3000,
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
                    foreach ($responses[$i] as $method) {
                        myLog(print_r($device->$method(), true));
                    }
                }
            } else {
                $output = array();
                $ret = 0;
                $cmd = '../../bin/mdm_test_device_dslam_zte_9xxx ' .
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
