<?php
/**
 * This class provides access to MDM Server for DSLAM ZTE 9000 series.
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
 * This class provides access to MDM Server for DSLAM ZTE 9000 series.
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
class MDMZTE9xxx extends MDMDevice
{
    /** DSLAM ZTE 9xxx family. */
    public static $MDM_DEVICE_DSLAM_ZTE_9xxx = 2;

    /***************** SUPPORTED COMMANDS FOR ZTE 9xxx *****************/
    public static $MDM_DSLAM_ZTE_9xxx_CMD_NOP = 0;
    public static $MDM_DSLAM_ZTE_9xxx_CMD_SHOW_SLOTS = 1;
    public static $MDM_DSLAM_ZTE_9xxx_CMD_SHOW_RUNNING_CONFIG = 2;
    public static $MDM_DSLAM_ZTE_9xxx_CMD_SHOW_SERVICE_PROFILE = 3;
    public static $MDM_DSLAM_ZTE_9xxx_CMD_SOFT_VERSION = 4;
    public static $MDM_DSLAM_ZTE_9xxx_CMD_SHOW_PORT_ATM_PVC = 5;
    public static $MDM_DSLAM_ZTE_9xxx_CMD_SHOW_UPTIME = 6;
    public static $MDM_DSLAM_ZTE_9xxx_CMD_LOAD_PORTS_SLOT = 7;
    public static $MDM_DSLAM_ZTE_9xxx_CMD_SCB_DETAIL = 8;
    public static $MDM_DSLAM_ZTE_9xxx_CMD_SLOT_DETAIL = 9;
    public static $MDM_DSLAM_ZTE_9xxx_CMD_PORT_DETAIL = 10;
    public static $MDM_DSLAM_ZTE_9xxx_CMD_SET_TIMEZONE = 11;
    public static $MDM_DSLAM_ZTE_9xxx_CMD_GET_NTP = 12;
    public static $MDM_DSLAM_ZTE_9xxx_CMD_GET_SERVICE_PROFILE = 13;
    public static $MDM_DSLAM_ZTE_9xxx_CMD_CREATE_SERVICE_PROFILE = 14;
    public static $MDM_DSLAM_ZTE_9xxx_CMD_CONFIGURE_SERVICE_PROFILE = 15;
    public static $MDM_DSLAM_ZTE_9xxx_CMD_DELETE_SERVICE_PROFILE = 16;
    public static $MDM_DSLAM_ZTE_9xxx_CMD_GET_SLOT_MAC = 17;
    public static $MDM_DSLAM_ZTE_9xxx_CMD_GET_PORT_MAC = 18;

    /**
     * Return service profile info.
     *
     * @throws Exception on error.
     * @return array With service profile info.
     */
    public function getServiceProfile()
    {
        return $this->getXMLAsArray(
            'zte_9xxx_profile',
            array(
                'AtucChanConfFastMinTxRate',
                'AtucChanConfInterMinTxRate',
                'AtucChanConfFastMaxTxRate',
                'AtucChanConfInterMaxTxRate',
                'AtucChanConfMaxInterDelay',
                'AturChanConfFastMinTxRate',
                'AturChanConfInterMinTxRate',
                'AturChanConfFastMaxTxRate',
                'AturChanConfInterMaxTxRate',
                'AturChanConfMaxInterDelay',
                'ConfProfileLineType'
            )
        );
    }

    /**
     * Return NTP info.
     *
     * @throws Exception on error.
     * @return arrayofarrays With NTP info.
     */
    public function getNTP()
    {
        return $this->getXMLAsArray(
            'zte_9xxx_ntp',
            array(
                'status', 'interval', 'timezone', 'server',
                'stratum', 'clockstatus'
            )
        );
    }

    /**
     * Return running config.
     *
     * @throws Exception on error.
     * @return arrayofarrays With software versions.
     */
    public function getRunningConfig()
    {
        return $this->getXMLAsArray('zte_9xxx_running_config', array('config'));
    }

    /**
     * Return service profiles for DSLAMs.
     *
     * @throws Exception on error.
     * @return arrayofarrays With spectrum profiles.
     */
    public function getServiceProfiles()
    {
        return $this->getXMLChildsAsArray(
            'zte_9xxx_serviceprofilelist', 'serviceprofile', array('name')
        );
    }

    /**
     * Return software information for DSLAMs.
     *
     * @throws Exception on error.
     * @return arrayofarrays With software versions.
     */
    public function getSoftwareVersions()
    {
        return $this->getXMLChildsAsArray(
            'zte_9xxx_soft_version', 'soft',
            array('id', 'name', 'state', 'size')
        );
    }

    /**
     * Return boot software information for DSLAMs.
     *
     * @throws Exception on error.
     * @return arrayofarrays With software versions.
     */
    public function getSoftwareBootVersion()
    {
        return $this->getXMLAsArray(
            'zte_9xxx_soft_version', array('bootversion')
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
            'zte_9xxx_pvclist', 'pvc',
            array(
                'slot', 'port', 'id', 'vpi', 'vci', 'status'
            )
        );
    }

    /**
     * Return information for Uptime command in DSLAMs.
     *
     * @throws Exception on error.
     * @return array With uptime values.
     */
    public function getUptime()
    {
        return $this->getXMLAsArray(
            'zte_9xxx_uptime',
            array('months', 'days', 'hours', 'minutes', 'seconds')
        );
    }

    /**
     * Return full information for ports in DSLAMs.
     *
     * @throws Exception on error.
     * @return arrayofarrays With ports.
     */
    public function getPortListFull()
    {
        return $this->getXMLChildsAsArray(
            'zte_9xxx_portlistfull', 'port',
            array(
                'slot', 'port', 'admin-status', 'link-status', 'up', 'down',
                'service-profile'
            )
        );
    }

    /**
     * Return information for scb in DSLAMs.
     *
     * @throws Exception on error.
     * @return array With slot.
     */
    public function getSCBDetail()
    {
        return $this->getXMLAsArray(
            'zte_9xxx_scb', array(
                'name', 'status', 'minicard-type', 'switch-devices',
                'position', 'sync', 'swap-permit', 'cpu', 'memory'
            )
        );
    }
    
    /**
     * Return information for a slot in DSLAMs.
     *
     * @throws Exception on error.
     * @return array With slot.
     */
    public function getSlotDetail()
    {
        return $this->getXMLAsArray(
            'zte_9xxx_slot', array(
                'configured-type', 'run-type', 'card-hardver', 'card-status',
                'switch-devices', 'link-master-scb', 'loopcheck-interval',
                'cpu'
            )
        );
    }

    /**
     * Return profile information for ports in DSLAMs.
     *
     * @throws Exception on error.
     * @return array With port profile information.
     */
    public function getPortProfile()
    {
        return $this->getXMLAsArray(
            'zte_9xxx_portprofile',
            array(
                'serviceprofile', 'alarmprofile', 'pmconfpmsf', 'pmstate',
                'transatucconfig', 'transatucactual'
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
            'zte_9xxx_slotlist', 'slot',
            array('id', 'type', 'state', 'cardtype',
                'softversion', 'portnum', 'porttype'
            )
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
            $host, $port, MDMZTE9xxx::$MDM_DEVICE_DSLAM_ZTE_9xxx, $connection, 
            $connection_target, $connection_username, $connection_password,
            $schemafile
        );
    }
}
?>
