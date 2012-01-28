<?php
# FileName="Connection_php_mysql.htm"
# Type="MYSQL"
# HTTP="true"
$hostname_tc = "localhost";
$database_tc = "nodo";
$username_tc = "nodo";
$password_tc = "123nodo";
$tc = mysql_pconnect($hostname_tc, $username_tc, $password_tc) or trigger_error(mysql_error(),E_USER_ERROR); 
?>