<?php
/**
 * Example for ZTE 8426.
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
require_once 'MDMZTE8426.php';

/* Initialize. */
/*
error_reporting(0);
ini_set('display_errors', false);
*/

/** This is where our schema file is. */
define('SCHEMA', '/etc/mdm/mdm_response_zte_8426.xsd');

/** Set this to true if you want to use the server or the cli binaries. */
define('USE_SERVER', true);

/** Execute these commands. */
$cmds = array(
    MDMZTE8426::$MDM_DSLAM_ZTE_8426_CMD_SOFT_VERSION,
    MDMZTE8426::$MDM_DSLAM_ZTE_8426_CMD_GET_NETWORK_INFO,
    MDMZTE8426::$MDM_DSLAM_ZTE_8426_CMD_GET_MAC,
    MDMZTE8426::$MDM_DSLAM_ZTE_8426_CMD_SHOW_SLOTS,
    MDMZTE8426::$MDM_DSLAM_ZTE_8426_CMD_GET_ROUTES,
    MDMZTE8426::$MDM_DSLAM_ZTE_8426_CMD_SHOW_PORT_ATM_PVC,
    MDMZTE8426::$MDM_DSLAM_ZTE_8426_CMD_NOP,
    MDMZTE8426::$MDM_DSLAM_ZTE_8426_CMD_SHOW_SERVICE_PROFILES,
    MDMZTE8426::$MDM_DSLAM_ZTE_8426_CMD_SHOW_SLOT_PORTS,
    /*MDMZTE8426::$MDM_DSLAM_ZTE_8426_CMD_REBOOT,*/
    MDMZTE8426::$MDM_DSLAM_ZTE_8426_CMD_GET_SERVICE_PROFILE,
    MDMZTE8426::$MDM_DSLAM_ZTE_8426_CMD_CREATE_PROFILE,
    MDMZTE8426::$MDM_DSLAM_ZTE_8426_CMD_DELETE_PROFILE
);

/** With these arguments. */
$args = array(
    null,
    null,
    null,
    null,
    null,
    null,
    null,
    null,
    null,
    /*null,*/
    '1',
    'profiletest,1024,1024,4,512,512,4',
    '5'
);

/** With these connections. */
$conns = array(
    MDMDevice::$MDM_CONNECTION_TELNET, MDMDevice::$MDM_CONNECTION_SERIAL
);

/** In these targets. */
$tgts = array('127.0.0.1:23', '/dev/cuad0');

/** Descriptions for commands. */
$descs = array(
    'Show Software Version',
    'Show Network Information',
    'Show MAC Information',
    'Show Slots',
    'Show Routes Information',
    'Show ATM PVC',
    'NOP',
    'Show Service Profiles',
    'Show Ports',
    /*'Reboot',*/
    'Get Service Profile',
    'Create Service Profile',
    'Delete Service Profile'
);

/** Methods to get responses for each command. */
$responses = array(
    'getSoftwareVersion',
    'getInternetList',
    'getMACList',
    'getSlotList',
    'getInternetRoutes',
    'getPVC',
    null,
    'getServiceProfilesList',
    'getSlotPorts',
    /*null*/
    'getServiceProfile',
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
    echo date('m/d/Y - H:i:s - ', time()) . 'ZTE8426 - ' . $msg . "\n";
    return;
}

try
{
    /* Instantiate devices. */
    $connsCount = count($conns) - 1;
    $devices = array();
    for ($i = 0; $i < $connsCount; $i++) {
        $devices[] = new MDMZTE8426(
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
                    myLog(print_r($device->$responses[$i](), true));
                }
            } else {
                $output = array();
                $ret = 0;
                $cmd = '../../bin/mdm_test_device_dslam_zte_8426 ' .
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
