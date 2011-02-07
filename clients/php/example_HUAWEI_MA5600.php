<?php
/**
 * Example for HUAWEI MA5600.
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
require_once 'MDMHUAWEIMA5600.php';

/* Initialize. */
/*
error_reporting(0);
ini_set('display_errors', false);
*/

/** This is where our schema file is. */
define('SCHEMA', '/etc/mdm/mdm_response_huawei_ma5600.xsd');

/** Set this to true if you want to use the server or the cli binaries. */
define('USE_SERVER', false);

/** Execute these commands. */
$cmds = array(
    MDMHUAWEIMA5600::$MDM_DSLAM_HUAWEI_MA5600_CMD_NOP,
    MDMHUAWEIMA5600::$MDM_DSLAM_HUAWEI_MA5600_CMD_UPTIME,
);

/** With these arguments. */
$args = array(
    null,
    null
);

/** With these connections. */
$conns = array(MDMDevice::$MDM_CONNECTION_TELNET);

/** In these targets. */
$tgts = array('127.0.0.1:23');

/** Descriptions for commands. */
$descs = array(
    'NOP',
    'Uptime'
);

/** Methods to get responses for each command. */
$responses = array(
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
    echo date('m/d/Y - H:i:s - ', time()) . 'HUAWEIMA5600 - ' . $msg . "\n";
    return;
}

try
{
    /* Instantiate devices. */
    $connsCount = count($conns);
    $devices = array();
    for ($i = 0; $i < $connsCount; $i++) {
        $devices[] = new MDMHUAWEIMA5600(
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
                    (int)$argv[3], (int)$argv[4], $cmds[$i], 300000, 300000,
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
                $cmd = '../../bin/mdm_test_device_dslam_huawei_ma5600 ' .
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
