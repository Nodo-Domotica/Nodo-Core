<?php
$db_hostname = "mysql.proxsys.net";
$database = "powerkite";
$db_username = "powerkite";
$db_password = "123power";
$db = mysql_pconnect($db_hostname, $db_username, $db_password) or trigger_error(mysql_error(),E_USER_ERROR); 
?>