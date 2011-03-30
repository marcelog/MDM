<?php
/**
 * This class provides access to MDM Server for DSLAM ZTE 8426.
 *
 * PHP Version 5
 *
 * @category MDM
 * @package  Driver
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

/**
 * Include files.
 */
require_once 'MDMDevice.php';

/**
 * This class provides access to MDM Server for DSLAM ZTE 8426.
 *
 * PHP Version 5
 *
 * @category MDM
 * @package  Driver
 * @author   Marcelo Gornstein <marcelog@gmail.com>
 * @license  http://www.noneyet.ar/ Apache License 2.0
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
class MDMZTE8426 extends MDMDevice
{
    /** DSLAM ZTE 8426 family. */
    public static $MDM_DEVICE_DSLAM_ZTE_8426 = 1;

    /***************** SUPPORTED COMMANDS FOR ZTE 8426 *****************/
    public static $MDM_DSLAM_ZTE_8426_CMD_SOFT_VERSION = 0;
    public static $MDM_DSLAM_ZTE_8426_CMD_GET_NETWORK_INFO = 1;
    public static $MDM_DSLAM_ZTE_8426_CMD_GET_MAC = 2;
    public static $MDM_DSLAM_ZTE_8426_CMD_SHOW_SLOTS = 3;
    public static $MDM_DSLAM_ZTE_8426_CMD_GET_ROUTES = 4;
    public static $MDM_DSLAM_ZTE_8426_CMD_SHOW_PORT_ATM_PVC = 5;
    public static $MDM_DSLAM_ZTE_8426_CMD_NOP = 6;
    public static $MDM_DSLAM_ZTE_8426_CMD_SHOW_SERVICE_PROFILES = 7;
    public static $MDM_DSLAM_ZTE_8426_CMD_SHOW_SLOT_PORTS = 8;
    public static $MDM_DSLAM_ZTE_8426_CMD_REBOOT = 9;
    public static $MDM_DSLAM_ZTE_8426_CMD_CREATE_PROFILE = 10;
    public static $MDM_DSLAM_ZTE_8426_CMD_DELETE_PROFILE = 11;
    public static $MDM_DSLAM_ZTE_8426_CMD_GET_SERVICE_PROFILE = 12;
    public static $MDM_DSLAM_ZTE_8426_CMD_GET_ALARMS = 13;

    /**
     * Return information for alarms.
     *
     * @throws Exception on error.
     * @return array with alarms
     */
    public function getAlarms()
    {
        return $this->getXMLAsArray('zte_8426_alarm', array('date', 'card', 'line', 'type'));
    }
    /**
     * Return information for service profile.
     *
     * @throws Exception on error.
     * @return array with service profile info.
     */
    public function getServiceProfile()
    {
        return $this->getXMLAsArray(
            'zte_8426_serviceprofile', array(
                'AtucConfRateMode',
                'AtucConfTargetSnrMgn',
                'AtucConfMaxSnrMgn',
                'AtucConfMinSnrMgn',
                'AtucChanConfFastMaxTxRate',
                'AtucChanConfInterleaveMaxTxRate',
                'AtucChanConfMaxInterleaveDelay',
                'AturConfTargetSnrMgn',
                'AturConfMaxSnrMgn',
                'AturConfMinSnrMgn',
                'AturChanConfFastMaxTxRate',
                'AturChanConfInterleaveMaxTxRate',
                'AturChanConfMaxInterleaveDelay',
                'adslLineConfProfileRowStatus'
            )
        );
    }

    /**
     * Return information for PVC from DSLAMs.
     *
     * @throws Exception on error.
     * @return arrayofarrays With pvc's.
     */
    public function getPVC()
    {
        return $this->getXMLChildsAsArray(
            'zte_8426_pvclist', 'pvc', array('slot', 'port', 'vpi', 'vci')
        );
    }

    /**
     * Return information for Internet Ports in DSLAMs.
     *
     * @throws Exception on error.
     * @return arrayofarrays With internet interfaces.
     */
    public function getInternetList()
    {
        return $this->getXMLChildsAsArray(
            'zte_8426_internetlist', 'internet', array('id', 'ip', 'netmask')
        );
    }

    /**
     * Return Service Profile list.
     *
     * @throws Exception on error.
     * @return arrayofarrays With service profiles.
     */
    public function getServiceProfilesList()
    {
        return $this->getXMLChildsAsArray(
            'zte_8426_serviceprofile_list', 'serviceprofile',
            array('id', 'name')
        );
    }

    /**
     * Return mac information for Ethernet Ports in DSLAMs.
     *
     * @throws Exception on error.
     * @return arrayofarrays With mac addresses.
     */
    public function getMACList()
    {
        return $this->getXMLChildsAsArray(
            'zte_8426_maclist', 'mac', array('id', 'mac')
        );
    }

    /**
     * Return information for Internet Routes in DSLAMs.
     *
     * @throws Exception on error.
     * @return arrayofarrays With internet routes.
     */
    public function getInternetRoutes()
    {
        return $this->getXMLChildsAsArray(
            'zte_8426_routelist', 'route', array(
                'destination', 'netmask', 'gateway', 'interface'
            )
        );
    }

    /**
     * Return information for slot ports in DSLAMs.
     *
     * @throws Exception on error.
     * @return arrayofarrays With slot ports.
     */
    public function getSlotPorts()
    {
        return $this->getXMLChildsAsArray(
            'zte_8426_portlist', 'port', array(
                'id', 'type', 'coding', 'serviceprofile',
                'ratedownload', 'rateupload'
            )
        );
    }

    /**
     * Return information for slots in DSLAMs.
     *
     * @throws Exception on error.
     * @return arrayofarrays With slots.
     */
    public function getSlotList()
    {
        return $this->getXMLChildsAsArray(
            'zte_8426_slotlist', 'slot',
            array('id', 'type', 'enabled', 'availability')
        );
    }

    /**
     * Return software information for DSLAMs.
     *
     * @throws Exception on error.
     * @return arrayofarrays With software versions.
     */
    public function getSoftwareVersion()
    {
        return $this->getXMLChildsAsArray(
            'zte_8426_soft_version', 'soft', array('name')
        );
    }

    /**
     * Constructor. Forms a device, identified by a connection_target, with
     * credentials connection_username and connection_password
     *
     * @param string $host                MDM Server ip address, like 1.1.1.1
     * @param int    $port                MDM Server port, like 34000
     * @param string $connection          Connection type, like
     * MDM_CONNECTION_TELNET
     * @param string $connection_target   Something like 1.1.1.1:23 or
     * /dev/cuad0
     * @param string $connection_username Username to log into device
     * @param string $connection_password Password to log into device
     * @param string $schemafile          Absolute path to schema file used to
     * validate responses.
     *
     * @throws Exception on invalid arguments.
     *
     */
    public function __construct(
        $host, $port, $connection, $connection_target,
        $connection_username, $connection_password, $schemafile
    ) {
        parent::__construct(
            $host, $port, MDMZTE8426::$MDM_DEVICE_DSLAM_ZTE_8426, $connection, 
            $connection_target, $connection_username, $connection_password,
            $schemafile
        );
    }
}
?>
