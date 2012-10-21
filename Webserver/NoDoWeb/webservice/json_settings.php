<?php
require_once('../connections/db_connection.php');
require_once('../include/auth.php');

mysql_select_db($database, $db);
$query = "SELECT * FROM nodo_tbl_users WHERE id='$userId'";
$recordset = mysql_query($query, $db) or die(mysql_error());
$row = mysql_fetch_array($recordset);


        
             $rowsarray[] = array(
            "theme" 			=> $row['webapp_theme'],
			"themeheader" 		=> $row['webapp_theme_header'],
			"usergroup"			=> $row['user_group'],
			"nodoid"			=> $row['nodo_id'],
			"nodoip"			=> $row['nodo_ip'],
			"nodoport"			=> $row['nodo_port']);
			
			

 
$json = json_encode($rowsarray);

echo '{"settings":'. $json .'}'; 
 



?>