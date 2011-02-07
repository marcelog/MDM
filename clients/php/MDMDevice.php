<?php
/**
 * This class provides access to MDM Server.
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
 * This class provides access to MDM Server.
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
abstract class MDMDevice
{
    /***************** SUPPORTED CONNECTIONS *****************/
    /** Telnet protocol connection. */
    public static $MDM_CONNECTION_TELNET = 0;
    /** SSH1 protocol connection. */
    public static $MDM_CONNECTION_SSH1 = 1;
    /** SSH2 protocol connection. */
    public static $MDM_CONNECTION_SSH2 = 2;
    /** HTTP protocol connection. Uses CURL. */
    public static $MDM_CONNECTION_HTTP = 3;
    /** HTTP over SSL protocol connection. Uses CURL. */
    public static $MDM_CONNECTION_HTTPS = 4;
    /** Raw TCP connection. */
    public static $MDM_CONNECTION_RAW_TCP = 5;
    /** Raw TCP SERVER connection. */
    public static $MDM_CONNECTION_RAW_TCP_SERVER = 6;
    /** Serial RS-232 protocol connection. */
    public static $MDM_CONNECTION_SERIAL = 7;
    /** SNMPv1 protocol driver. */
    public static $MDM_CONNECTION_SNMP1 = 8;
    /** SNMPv2 protocol driver. */
    public static $MDM_CONNECTION_SNMP2 = 9;
    /** SNMPv3 protocol driver. */
    public static $MDM_CONNECTION_SNMP3 = 10;

    /***************** INTERNAL VARIABLES *****************/
    /** Will hold the mdm server host. */
    private $_host = false;

    /** Will hold the mdm server port. */
    private $_port = false;

    /** Will hold the device requested. */
    private $_device = false;

    /** Will hold the connection type requested. */
    private $_connection = false;

    /** Will hold the target for the connection, like ip:port of a dslam. */
    private $_connection_target = false;

    /** Will hold the username to use in the target device. */
    private $_connection_username = false;

    /** Will hold the password to use in the target device. */
    private $_connection_password = false;

    /** Will hold the schema filename to validate respones from server. */
    private $_schemafile = false;

    /** Will hold the plain text response from server. */
    private $_response = false;

    /** Will hold the xml document generated from the server response. */
    private $_responsexmldoc = false;

    /** Holds connection names in human readable form. */
    private static $_connectionNames = array(
        'Telnet', 'SSH1', 'SSH2', 'HTTP', 'HTTPS', 'Raw TCP', 'Raw TCP server',
        'Serial', 'SNMPv1', 'SNMPv2', 'SNMPv3'
    );

    /**
     * Returns response in plain text.
     *
     * @return string Response from server.
     */
    public function getRawResponse()
    {
        return $this->_response;
    }
    /**
     * Returns the connection name for a given connection id.
     *
     * @param int $connID Connection ID.
     *
     * @return string Connection name or false if invalid.
     */
    public static function getConnectionName($connID = -1)
    {
        if ($connID < MDMDevice::$MDM_CONNECTION_TELNET
            || $connID > MDMDevice::$MDM_CONNECTION_SNMP3
        ) {
            return false;
        }
        return MDMDevice::$_connectionNames[$connID];
    }
    
    /**
     * Returns the last socket error. Dont call this function twice in a row, it
     * will loose the error.
     *
     * @param socket $socket Socket. Can be null (or non existant). If this is
     * the case, it will use socket_last_error() to get the error. Otherwise,
     * it will
     * try to use socket_get_option() with SO_ERROR.
     *
     * @throws Exception if cant get socket error.
     * @return int The last socket error.
     */
    protected function getSocketErrorNumber($socket = null)
    {
        $ret = false;
        if ($socket === null) {
            $ret = socket_last_error();
        } else {
            $ret = socket_get_option($socket, SOL_SOCKET, SO_ERROR);
            if ($ret === false) {
                throw new Exception('Could not get socket error.');
            }
        }
        return $ret;
    }

    /**
     * Returns the string representation of the socket error via
     * socket_strerror().
     *
     * @param socket $socket Socket. Can be null. If this is the case
     * it will return the string representation of $error instead of a
     * particular socket.
     * @param int    $error  Error number, can be inexistant or used
     * with socket = null to specify the return of a particular error
     * string needed.
     *
     * @see MDMDevice::getSocketErrorNumber()
     * @throws Exception on error.
     * @return string Socket error string.
     */
    protected function getSocketErrorString($socket = null, $error = false)
    {
        if ($socket != null) {
            $error = $this->getSocketErrorNumber($socket);
        }
        $errMsg = socket_strerror($error);
        if ($errMsg === false) {
            throw new Exception('Could not get socket error string.');
        }
        return '(' . $error . '): ' . $errMsg;
    }

    /**
     * Executes the given command in the device. Be *warned* you may need
     * high times for recv_to and or devread_to depending on the device you
     * are interacting with.
     *
     * @param int $connection_to Connection timeout to server, in milliseconds.
     * @param int $recv_to       Receive timeout from server, in milliseconds.
     * @param int $command       Command.
     * @param int $devconn_to    Timeout when connecting to the device from
     * server.
     * @param int $devread_to    Timeout when reading from the device from
     * server.
     * @param csv $arguments     Command arguments, like 'a,b,c'
     *
     * @throws Exception on error.
     * @return void
     */
    public function exec(
        $connection_to, $recv_to, $command, $devconn_to,
        $devread_to, $arguments = null
    ) {
        /* Validate connection timeout. */
        if (!is_int($connection_to) || $connection_to < 1) {
            throw new Exception('Connection timeout should be a integer > 0');
        }

        /* Validate receive timeout. */
        if (!is_int($recv_to) || $recv_to < 1) {
            throw new Exception('Receive timeout should be a integer > 0');
        }

        /* Validate device connection timeout. */
        if (!is_int($devconn_to) || $devconn_to < 1) {
            throw new Exception(
                'Device connection timeout should be a integer > 0'
            );
        }

        /* Validate device read timeout. */
        if (!is_int($devread_to) || $devread_to < 1) {
            throw new Exception('Device read timeout should be a integer > 0');
        }

        /* Create socket. */
        $socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
        if ($socket === false) {
            throw new Exception(
                'Error opening socket: ' . $this->getSocketErrorString()
            );
        }

        /* Set non blocking. */
        if (socket_set_nonblock($socket) === false) {
            throw new Exception(
                'Error setting non blocking mode: ' .
                $this->getSocketErrorString($socket)
            );
        }

        /* Connect socket. */
        $totaltime = 0;
        $connection_to_tmp = $connection_to / 2; /* XXX WTF ??? XXX */
        do {
            if (@socket_connect($socket, $this->_host, $this->_port) === true) {
                break;
            }
            $readfd = array($socket);
            $writefd = array($socket);
            $exfd = array($socket);
            /*
             * If wasnt successfull, select() in the socket until timeout or
             * event occurs.
             */
            $r = @socket_select($readfd, $writefd, $exfd, 0, 0);
            if ($r === false) {
                throw new Exception(
                    'Error select()ing socket: ' .
                    $this->getSocketErrorString($socket)
                );
            }
            if ($r > 0) {
                $error = $this->getSocketErrorNumber();
                socket_clear_error($socket);
                if ($error != SOCKET_EINPROGRESS) {
                    if ($error === SOCKET_EALREADY || $error === 56) {
                        break;
                    } else {
                        throw new Exception(
                            'Error connecting socket (1): ' .
                            $this->getSocketErrorString(null, $error)
                        );
                    }
                }
            }
            usleep(1000);
            $totaltime += 1;
        } while ($totaltime < $connection_to_tmp);

        if ($totaltime >= $connection_to_tmp) {
            throw new Exception('Timeout Connecting.');
        }
        /* Form command. */
        $commandstr = '<?xml version="1.0" encoding="UTF-8"?>' . "\n";
        $commandstr .= '<mdm_request ';
        $commandstr .= 'xmlns:mdm="http://www.mg.com.ar/MDM-Protocol" ';
        $commandstr .= "\n";
        $commandstr .= 'xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" ';
        $commandstr .= "\n";
        $commandstr .= 'xsi:schemaLocation="';
        $commandstr .= 'http://www.mg.com.ar/MDM mdm_request.xsd">' . "\n";
        $commandstr .= '<version>0.1.1</version>' . "\n" ;
        $commandstr .= '<device>' . $this->_device . '</device>' . "\n" ;
        $commandstr .= '<connection>' . "\n" ;
        $commandstr .= '<target>' . $this->_connection_target . '</target>';
        $commandstr .= "\n" ;
        $commandstr .= '<type>' . $this->_connection . '</type>' . "\n" ;
        $commandstr .= '<connect_timeout>';
        $commandstr .= $devconn_to . '</connect_timeout>' . "\n";
        $commandstr .= '<recv_timeout>';
        $commandstr .= $devread_to . '</recv_timeout>' . "\n";
        $commandstr .= '<credential>' . "\n" ;
        $commandstr .= '<username>';
        $commandstr .= $this->_connection_username;
        $commandstr .= '</username>' . "\n" ;
        $commandstr .= '<password>';
        $commandstr .= $this->_connection_password;
        $commandstr .= '</password>' . "\n" ;
        $commandstr .= '</credential>' . "\n" ;
        $commandstr .= '</connection>' . "\n" ;
        $commandstr .= '<command>' . "\n" ;
        $commandstr .= '<type>' . $command . '</type>' . "\n" ;
        if (is_string($arguments) && !empty($arguments)) {
            $commandstr .= '<arguments><![CDATA[' . $arguments;
            $commandstr .= ']]></arguments>' . "\n" ;
        }
        $commandstr .= '</command>' . "\n" ;
        $commandstr .= '</mdm_request>' . "\n" ;

        /* Send command. */
        $bufflen = strlen($commandstr);
        if (socket_write($socket, $commandstr, $bufflen) < $bufflen) {
            throw new Exception(
                'Error sending command: ' .
                $this->getSocketErrorString($socket)
            );
        }
        /* Get response. */
        /* This is blocking
        $responsestr = '';
        $responsestrlen = 1048576;
        socket_set_block($socket);
        $responsestr .= socket_read($socket, $responsestrlen);
        if ($responsestr === false) {
            throw new Exception(
                'Error reading response: ' .
                $this->getSocketErrorString($socket)
            );
        }
        */
        $this->_response = '';
        $responsestrlen = 1048576;
        $to = 0;
        $recv_to_tmp = $recv_to / 2; /* XXX WTF ?? XXX */
        do {
            $buffer = '';
            $readfd = array($socket);
            $writefd = array($socket);
            $exfd = array($socket);

            /*
             * If wasnt successfull, select() in the socket until timeout or
             * event occurs.
             */
            $r = @socket_select($readfd, $write, $exfd, 0, 0);
            if ($r === false) {
                throw new Exception(
                    'Error select()ing socket: ' .
                    $this->getSocketErrorString($socket)
                );
            }
            if (count($exfd) > 0) {
                throw new Exception(
                    'Error reading command 1: ' .
                    $this->getSocketErrorString($socket)
                );
            }
            if (count($readfd) > 0) {
                $r = @socket_read($socket, $responsestrlen);
                if ($r === false) {
                    throw new Exception(
                        'Error reading command 2: ' .
                        $this->getSocketErrorString($socket)
                    );
                } else {
                    $this->_response .= $r;
                }
            }
            usleep(1000);
            $to += 1;
            if ($to >= $recv_to_tmp) {
                throw new Exception('Read timeout from server.');
            }
        } while (strstr($this->_response, '</mdm_response>') === false);
        /* Done. */
        socket_close($socket);

        /*var_dump($this->_response); */
        /* Validate response. */
        $this->_responsexmldoc = new DOMDocument(); 
        if (!$this->_responsexmldoc->loadXML($this->_response)) {
            throw new Exception(
                'Error loading xml: ' . print_r(libxml_get_last_error(), true) .
                ' : ' . print_r($this->_response, true)
            );
        }
        if (!$this->_responsexmldoc->schemaValidate($this->_schemafile)) {
            throw new Exception(
                'Error validating xml: ' .
                print_r(libxml_get_last_error(), true)
            );
        }

        /* Work with a simplexml object from now on. */
        $this->_responsexmldoc = new SimpleXMLElement($this->_response);
    }

    /**
     * Get response code, 0 means success. Any other value means an error
     * happened while executing the command.
     *
     * @throws Exception on error.
     * @return int Response code.
     */
    public function getResponseCode()
    {
        if (!isset($this->_responsexmldoc->result)) {
            throw new Exception('Response code isn\'t set');
        }
        return (int)$this->_responsexmldoc->result;
    }

    /**
     * Get response message. This is useful when getResponseCode() returns
     * a value different from 0.
     *
     * @throws Exception on error.
     * @return string Response message.
     */
    public function getResponseMessage()
    {
        if (!isset($this->_responsexmldoc->message)) {
            throw new Exception('Response message isn\'t set');
        }
        return (string)$this->_responsexmldoc->message;
    }

    /**
     * Will return all nodes descending from root as an array. if $nodes is 
     * an empty array, it will return the content of $root
     *
     * @param string $root  Root node, like uptime. May be a string and may also
     * be a SimpleXMLElement.
     * @param array  $nodes Array of strings, like ('years', 'months')
     *
     * @return array Associative, with results key = value.
     */
    protected function getXMLAsArray($root, $nodes)
    {
        $ret = array();
        $cnt = 0;
        if (is_string($root)) {
            if (count($nodes) > 0) {
                foreach ($nodes as $node) {
                    if (!isset($this->_responsexmldoc->data->$root->$node)) {
                        continue;//throw new Exception($node . ' missing');
                    }
                    $cnt = count($this->_responsexmldoc->data->$root->$node);
                    foreach ($this->_responsexmldoc->data->$root->$node as $n) {
                        if ($cnt > 1) {
                            $ret[$node][] = (string)$n;
                        } else {
                            $ret[$node] = (string)$n;
                        }
                    }
                }
            } else {
                $ret[$root] = (string)$this->_responsexmldoc->data->$root;
            }
        } else { /* Assume we got a SimpleXMLElement instead! */
            foreach ($nodes as $node) {
                /*if (!isset($root->$node)) {
                    throw new Exception($node . ' missing');
                }*/
                $cnt = count($root->$node);
                foreach ($root->$node as $n) {
                    if ($cnt > 1) {
                        $ret[$node][] = (string)$n;
                    } else {
                        $ret[$node] = (string)$n;
                    }
                }
            }
        }
        return $ret;
    }

    /**
     * Will return all nodes descending from root as an array of arrays.
     *
     * @param string $root   Root node, like internetlist.
     * @param string $childs Root node for childs, like internet.
     * @param array  $nodes  Array of strings, like ('id', 'ip', 'netmask')
     *
     * @throws Exception     On error.
     * @return arrayofarrays Associative, with results key = value.
     */
    protected function getXMLChildsAsArray($root, $childs, $nodes)
    {
        $ret = array();
        if (isset($this->_responsexmldoc->data->$root->$childs)) {
            foreach ($this->_responsexmldoc->data->$root->$childs as $child) {
                $ret[] = $this->getXMLAsArray($child, $nodes);
            }
        }
        return $ret;
    }

    /**
     * Constructor. Forms a device, identified by a connection_target, with
     * credentials connection_username and connection_password
     *
     * @param string $host                MDM Server ip address, like 1.1.1.1
     * @param int    $port                MDM Server port, like 34000
     * @param int    $device              MDM_DEVICE_DSLAM_ALCATEL_73xx or the
     * like.
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
        $host, $port, $device, $connection, $connection_target,
        $connection_username, $connection_password, $schemafile
    ) {
        /* Validate MDM Server Host. */
        if (!is_string($host)) {
            throw new Exception(
                'Host should be a valid ip address. i.e: 1.1.1.1'
            );
        }
        $this->_host = $host;

        /* Validate MDM Server Port. */
        if (!is_int($port) || $port < 1) {
            throw new Exception('Port should be an integer > 0');
        }
        $this->_port = $port;

        /* Validate Device. */
        if (!is_int($device) || $device < 0 || $device > 3) {
            throw new Exception('Device is not valid');
        }
        $this->_device = $device;

        /* Validate Connection Type. */
        if (!is_int($connection) || $connection < 0 || $connection > 10) {
            throw new Exception('Connection is not valid');
        }
        $this->_connection = $connection;

        /* Validate Connection Target. */
        if (!is_string($connection_target) || empty($connection_target)) {
            throw new Exception(
                'connection_target should be a string. ' .
                'i.e: 1.1.1.1:23 or /dev/cuad0'
            );
        }
        $this->_connection_target = $connection_target;

        /* Validate Connection Username. */
        if (!is_string($connection_username) || empty($connection_username)) {
            throw new Exception('Username should be a string, like root');
        }
         $this->_connection_username = $connection_username;

        /* Validate Connection Password. */
        if (!is_string($connection_password) || empty($connection_password)) {
            throw new Exception('Password should be a string, like root');
        }
         $this->_connection_password = $connection_password;

        /* Validate schema. */
        if (!is_string($schemafile)
            || empty($schemafile)
            || !file_exists($schemafile)
            || !is_file($schemafile)
        ) {
            throw new Exception(
                'Schemafile should be an absolute path to an existant file ' .
                'like /etc/schema.xsd'
            );
        }
        $this->_schemafile = $schemafile;
    }
}
