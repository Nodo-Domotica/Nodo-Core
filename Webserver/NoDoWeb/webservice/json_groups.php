<?php
require_once('../connections/db_connection.php');
require_once('../include/auth.php');

$grouptype = $_GET['grouptype'];

mysql_select_db($database, $db);

if (isset($grouptype)) {
	if ($grouptype == 'devices') {$query = "SELECT * FROM nodo_tbl_groups WHERE user_id='$userId' And devices='1'";}
	if ($grouptype == 'activities') {$query = "SELECT * FROM nodo_tbl_groups WHERE user_id='$userId' And activities='1'";}
	if ($grouptype == 'values') {$query = "SELECT * FROM nodo_tbl_groups WHERE user_id='$userId' And values='1'";}

	$recordset = mysql_query($query, $db) or die(mysql_error());


			while($row = mysql_fetch_array($recordset))
			{
				 $rowsarray[] = array(
				"id" 				=> $row['id'],
				"name" 				=> $row['name'],
				"devices" 			=> $row['devices'],
				"devices_default" 	=> $row['devices_default'],
				"activities"		=> $row['activities'],		
				"values" 			=> $row['values']);
			}
	 
	$json = json_encode($rowsarray);


	echo '{"groups":'. $json .'}'; 
}




?>