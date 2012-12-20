<?php
$db_hostname = "localhost";
$database = "nododom_webappdb";
$db_username = "nododom_webadbu";
$db_password = "xxxx";
$db = mysql_pconnect($db_hostname, $db_username, $db_password) or trigger_error(mysql_error(),E_USER_ERROR); 
?>