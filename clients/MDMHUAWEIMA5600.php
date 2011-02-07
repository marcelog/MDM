<?php
/**
 * This class provides access to MDM Server for DSLAM HUAWEI MA5600
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
 * This class provides access to MDM Server for DSLAM HUAWEI MA5600
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
class MDMHUAWEIMA5600 extends MDMDevice
{
    /** DSLAM HUAWEI MA5600. */
    public static $MDM_DEVICE_DSLAM_HUAWEI_MA5600 = 3;

    /***************** SUPPORTED COMMANDS FOR ZTE 8426 *****************/
    public static $MDM_DSLAM_HUAWEI_MA5600_CMD_NOP = 0;
    public static $MDM_DSLAM_HUAWEI_MA5600_CMD_UPTIME = 1;

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
            $host, $port, MDMHUAWEIMA5600::$MDM_DEVICE_DSLAM_HUAWEI_MA5600,
            $connection, $connection_target, $connection_username,
            $connection_password, $schemafile
        );
    }
}
?>
