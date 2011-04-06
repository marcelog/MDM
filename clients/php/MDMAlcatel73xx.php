<?php
/**
 * This class provides access to MDM Server for DSLAM Alcatel 73xx.
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
 * This class provides access to MDM Server for DSLAM Alcatel 73xx.
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
class MDMAlcatel73xx extends MDMDevice
{
    /** DSLAM Alcatel 73xx family. */
    public static $MDM_DEVICE_DSLAM_ALCATEL_73xx = 0;

    /***************** SUPPORTED COMMANDS FOR ALCATEL 73xx *****************/
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_UPTIME = 0;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_SOFT_VERSION = 1;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_SHOW_SLOTS = 2;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_SHOW_SLOT_PORT = 3;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_SHOW_SPECTRUM_PROFILE_DETAIL = 4;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_SHOW_SERVICE_PROFILE = 5;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_SHOW_ALL_PORTS = 6;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_GET_IP = 7;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_GET_ROUTES = 8;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_SHOW_PORT_DESCRIPTION = 9;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_SHOW_PORT_PROFILE = 10;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_SHOW_PORT_DATA_NEAR_END = 11;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_SHOW_PORT_DATA_FAR_END = 12;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_SHOW_PORT_ATM_PVC = 13;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_SHOW_PORT_INFO_VLAN = 14;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_SHOW_BACKUP_STATUS = 15;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_SHOW_DOWNLOAD_BACKUP_DB2 = 16;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_SHOW_PORT_ATM_PVC2 = 17;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_LOAD_PORTS_SLOT = 18;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_LOAD_PORTS_ALL = 19;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_SYSLOG_GET_1 = 20;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_SYSLOG_GET_2 = 21;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_SHOW_ATM_PVC_TRAFFIC_1 = 22;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_SHOW_ATM_PVC_TRAFFIC_2 = 23;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_SHOW_ATM_PVC_TRAFFIC_3 = 24;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_ALARM_GET_2 = 25;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_ALARM_GET = 26;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_PORTS_GET_TRAFFIC = 27;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_PORTS_GET = 28;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_ALARM_GET_COUNT = 29;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_NOP = 30;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_SET_TIMEZONE = 31;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_GET_NTP = 32;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_CREATE_SERVICE_PROFILE = 33;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_DELETE_SERVICE_PROFILE = 34;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_CFG_RAMODEUP_SERVICE_PROFILE = 35;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_CFG_RAMODEDOWN_SERVICE_PROFILE
        = 36;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_CFG_PBITRATEUP_SERVICE_PROFILE
        = 37;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_CFG_PBITRATEDOWN_SERVICE_PROFILE
        = 38;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_CFG_MDELAYUP_SERVICE_PROFILE = 39;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_CFG_MDELAYDOWN_SERVICE_PROFILE
        = 40;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_CFG_MAXBITRATEUP_SERVICE_PROFILE
        = 41;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_CFG_MAXBITRATEDOWN_SERVICE_PROFILE
        = 42;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_CFG_MINBITRATEUP_SERVICE_PROFILE
        = 43;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_CFG_MINBITRATEDOWN_SERVICE_PROFILE
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_REBOOT_HOT = 45;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_REBOOT_ALL = 46;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_REBOOT_SHUB = 47;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_PORT_ENABLE = 48;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_PORT_DISABLE = 49;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_FILETRANSFER = 50;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_FILETRANSFER_HOST_USER_PASS = 51;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_DATABASE_UPLOAD_ACTIVE = 52;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_DATABASE_DOWNLOAD_DATABASE = 53;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_ALARM_CLEAR = 54;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_CFG_PVC = 55;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_DEL_PVC = 56;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_REMOVE_PVC_FROM_PORT = 57;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_CFG_VLAN_SHUB_RESIDENTIAL_BRIDGE = 58;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_CFG_VLAN_ID_RESIDENTIAL_BRIDGE = 59;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_CFG_VLAN_EGRESS_PORT = 60;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_CFG_VLAN_ID_CROSS_CONNECT = 61;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_CFG_PORT_BRIDGE = 62;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_CFG_PORT_BRIDGE_VLAN_ID = 63;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_CFG_PORT_BRIDGE_PV_ID = 64;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_CFG_SYSLOG_NO_DESTINATION = 65;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_CFG_SYSLOG_DESTINATION = 66;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_CFG_SYSLOG_NOROUTE = 67;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_CFG_SYSLOG_ROUTE = 68;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_CFG_DATABASE_DEFAULT = 69;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_CFG_DATABASE_RESTORE = 70;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_SET_TIMEZONE_SERVER = 71;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_SET_TIMEZONE_SERVER_ENABLE = 72;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_SET_TIMEZONE_SERVER_DISABLE = 73;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_GET_PVC_MAC = 74;
    public static $MDM_DSLAM_ALCATEL_73xx_CMD_CHANGE_ISADMIN_PASSWORD = 75;



    /**
     * Return NTP info.
     *
     * @throws Exception on error.
     * @return arrayofarrays With NTP info.
     */
    public function getNTP()
    {
        return $this->getXMLAsArray(
            'alcatel_73xx_ntp', array('server', 'status', 'timezone')
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
            'alcatel_73xx_uptime',
            array('months', 'days', 'hours', 'minutes', 'seconds')
        );
    }

    /**
     * Return information for show slot port command in DSLAMs.
     *
     * @throws Exception on error.
     * @return array With slot port configuration.
     */
    public function getSlotPort()
    {
        return $this->getXMLAsArray(
            'alcatel_73xx_datalineinfo',
            array(
                "if-index", "adm-state", "opr-state-tx-rate-ds", "tx-rate-us",
                "tx-rate-ds", "max-tx-rate-us", "max-tx-rate-ds", "inp-up",
                "inp-dn", "interl-us", "interl-ds", "cur-op-mode",
                "rinit-1d", "actual-tps-tc-mode"
            )
        );
    }

    /**
     * Return information for backup status.
     *
     * @throws Exception on error.
     * @return array With slot port configuration.
     */
    public function getBackupStatus()
    {
        return $this->getXMLAsArray(
            'alcatel_73xx_backupstatus',
            array(
                "disk-space", "free-space", "download-progress",
                "download-error", "upload-progress", "upload-error",
                "auto-activate-error"
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
            'alcatel_73xx_pvclist', 'pvc', array('slot', 'port', 'vpi', 'vci')
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
            'alcatel_73xx_internetlist', 'internet',
            array('id', 'ip', 'netmask')
        );
    }

    /**
     * Return alarm types for DSLAMs.
     *
     * @throws Exception on error.
     * @return arrayofarrays With alarm types.
     */
    public function getAlarmTypes()
    {
        return $this->getXMLChildsAsArray(
            'alcatel_73xx_alarmtypelist', 'alarmtype', array('name')
        );
    }

    /**
     * Return databases (firmware) for DSLAMs.
     *
     * @throws Exception on error.
     * @return arrayofarrays With databases.
     */
    public function getFirmwares()
    {
        return $this->getXMLChildsAsArray(
            'alcatel_73xx_databaselist', 'database', array(
                'container', 'number', 'name', 'status', 'version', 'error'
            )
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
            'alcatel_73xx_routelist', 'route', array(
                'destination', 'netmask', 'gateway', 'interface'
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
            'alcatel_73xx_slotlist', 'slot',
            array(
                'id', 'type', 'enabled', 'error-status',
                'availability', 'restrt-cnt'
            )
        );
    }

    /**
     * Return information for eths in DSLAMs.
     *
     * @throws Exception on error.
     * @return arrayofarrays With eths.
     */
    public function getEths()
    {
        return $this->getXMLChildsAsArray(
            'alcatel_73xx_ethlist', 'eth',
            array(
                'id', 'admin-status', 'oper-status', 'speed',
                'type', 'duplex'
            )
        );
    }

    /**
     * Return information for ports in DSLAMs.
     *
     * @throws Exception on error.
     * @return arrayofarrays With ports.
     */
    public function getPortList()
    {
        return $this->getXMLChildsAsArray(
            'alcatel_73xx_portstatistics', 'port',
            array(
                'id', 'adm-state', 'opr-state-tx-rate-ds', 'tx-rate-us',
                'tx-rate-ds', 'cur-op-mode'
            )
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
            'alcatel_73xx_portlistfull', 'port',
            array(
                'slot', 'port', 'service-profile', 'spectrum-profile',
                'admin-status', 'transfer-mode'
            )
        );
    }

    /**
     * Return ethernet statistics in DSLAMs.
     *
     * @throws Exception on error.
     * @return arrayofarrays With statistics.
     */
    public function getEthStatistics()
    {
        return $this->getXMLChildsAsArray(
            'alcatel_73xx_ethstatistics', 'eth',
            array(
                "id", "in-octets", "in-ucast-pkts",
                "in-addr-ucast-pkts", "in-discard-pkts", "in-err-pkts",
                "pkts-unknown-proto", "out-octets", "out-ucast-pkts", 
                "out-addr-ucast-pkts", "out-discard-pkts", "out-err-pkts"
            )
        );
    }

    /**
     * Return VLANs in DSLAMs.
     *
     * @throws Exception on error.
     * @return arrayofarrays With VLANs.
     */
    public function getVlanList()
    {
        return $this->getXMLChildsAsArray(
            'alcatel_73xx_vlanlist', 'vlan',
            array(
                'slot', 'port', 'vpi', 'vci', 'vlan', 'admin-status',
                'level', 'value'
            )
        );
    }

    /**
     * Return information for port description in DSLAMs.
     *
     * @throws Exception on error.
     * @return arrayofarrays With ports.
     */
    public function getPortDescriptions()
    {
        return $this->getXMLChildsAsArray(
            'alcatel_73xx_portlist', 'port',
            array('slot', 'port', 'admin-status', 'description')
        );
    }

    /**
     * Return description for a port in DSLAMs.
     *
     * @throws Exception on error.
     * @return string Port Description.
     */
    public function getPortDescription()
    {
        return $this->getXMLAsArray(
            'alcatel_73xx_portdescription', array()
        );
    }

    /**
     * Return alarm count from DSLAMs.
     *
     * @throws Exception on error.
     * @return string Alarm count.
     */
    public function getAlarmCount()
    {
        return $this->getXMLAsArray(
            'alcatel_73xx_alarmcount', array()
        );
    }

    /**
     * Return operational data for a near end port in DSLAMs.
     *
     * @throws Exception on error.
     * @return array With operational data.
     */
    public function getOpDataNearEnd()
    {
        return $this->getXMLAsArray(
            'alcatel_73xx_portdatanearend', array(
                "rel-cap-occ-up", "noise-margin-up", "output-power-down",
                "sig-attenuation-up", "loop-attenuation-up", "actual-opmode",
                "xtu-c-opmode", "ansi-t1413", "etsi-dts", "g992-1-a",
                "g992-1-b", "g992-2-a", "g992-3-a", "g992-3-b", "g992-3-l1",
                "g992-3-l2", "g992-3-am", "g992-5-a", "g992-5-b",
                "ansi-t1.424", "etsi-ts", "itu-g993-1", "ieee-802.3ah",
                "g992-5-am", "g993-2-8a", "g993-2-8b", "g993-2-8c",
                "g993-2-8d", "g993-2-12a", "g993-2-12b", "g993-2-17a",
                "g993-2-30a", "actual-psd-down", "power-mgnt-state",
                "per-bnd-lp-att-up", "pr-bnd-sgn-att-up", "pr-bnd-nois-mg-up",
                "high-freq-up", "elect-length", "time-adv-corr",
                "actual-tps-tc-mode"
            )
        );
    }

    /**
     * Return operational data for a far end port in DSLAMs.
     *
     * @throws Exception on error.
     * @return array With operational data.
     */
    public function getOpDataFarEnd()
    {
        return $this->getXMLAsArray(
            'alcatel_73xx_portdatafarend', array(
                "rel-cap-occ-down", "noise-margin-down", "output-power-up",
                "sig-attenuation-down", "loop-attenuation-down", 
                "xtu-r-opmode", "ansi-t1413", "etsi-dts", "g992-1-a",
                "g992-1-b", "g992-2-a", "g992-3-a", "g992-3-b",
                "g992-3-l1", "g992-3-l2", "g992-3-am", "g992-5-a",
                "g992-5-b",	"ansi-t1.424", "etsi-ts", "itu-g993-1",
                "ieee-802.3ah", "g992-5-am", "g993-2-8a", "g993-2-8b",
                "g993-2-8c", "g993-2-8d", "g993-2-12a", "g993-2-12b",
                "g993-2-17a", "g993-2-30a", "actual-psd-up",
                "pr-bnd-lp-att-dn", "pr-bnd-sig-att-dn",
                "pr-bnd-nmgn-dn", "elect-length-est", "time-adv-prp",
                "high-freq-dn",
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
            'alcatel_73xx_portprofile',
            array('spectrumprofile', 'serviceprofile', 'status', 'mode')
        );
    }

    /**
     * Return vlan information in DSLAMs.
     *
     * @throws Exception on error.
     * @return array With vlan information.
     */
    public function getVlanInfo()
    {
        return $this->getXMLAsArray(
            'alcatel_73xx_vlan', array('vlanid', 'pvid')
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
            'alcatel_73xx_soft_version', 'soft',
            array(
                'index', 'name', 'availability', 'act-status',
                'commit-status', 'download-error', 'err-file-name', 
                'err-file-server', 'auto-activate-error'
            )
        );
    }

    /**
     * Return spectrum profiles for DSLAMs.
     *
     * @throws Exception on error.
     * @return arrayofarrays With spectrum profiles.
     */
    public function getSpectrumProfiles()
    {
        return $this->getXMLChildsAsArray(
            'alcatel_73xx_spectrumprofilelist', 'spectrumprofile',
            array('id', 'name', 'version', 'mode')
        );
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
            'alcatel_73xx_serviceprofilelist', 'serviceprofile',
            array(
                'id', 'name', 'version', 'ra-mode-down', 'ra-mode-up',
                'plan-bitrate-down', 'plan-bitrate-up',
                'max-delay-down', 'max-delay-up'
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
            $host, $port, MDMAlcatel73xx::$MDM_DEVICE_DSLAM_ALCATEL_73xx,
            $connection, $connection_target, $connection_username,
            $connection_password, $schemafile
        );
    }
}
?>
