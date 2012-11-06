<?php
require_once('../../connections/db_connection.php');
require_once('../../include/auth.php');


mysql_select_db($database, $db);

if (isset($_GET['readgroups'])) { //1 = alle groupen opvragen 2 = ייn group opvragen via group id
	
	if (is_numeric($_GET['id'])) {$id=$_GET['id'];}
	
	if ($_GET['readgroups'] == 1) {	$query = "SELECT * FROM nodo_tbl_groups WHERE user_id='$userId'";}
	if ($_GET['readgroups'] == 2) {	$query = "SELECT * FROM nodo_tbl_groups WHERE user_id='$userId' AND id='$id'";}
	
	

	$recordset = mysql_query($query, $db) or die(mysql_error());


			while($row = mysql_fetch_array($recordset))
			{
				 $rowsarray[] = array(
				"id" 					=> $row['id'],
				"name" 					=> $row['name'],
				"devices" 				=> $row['devices'],
				"devices_default" 		=> $row['devices_default'],
				"activities"			=> $row['activities'],
				"activities_default"	=> $row['activities_default'],				
				"values" 				=> $row['values']);
			}
	 
	$json = json_encode($rowsarray);


	echo '{"groups":'. $json .'}'; 

}

if (isset($_POST['deletegroup']) && is_numeric($_POST['id'])) { //Groep verwijderen

	$id = $_POST['id'];
	$query = "DELETE FROM nodo_tbl_groups WHERE user_id='$userId' AND id='$id'";
	mysql_query($query, $db) or die(mysql_error());
	
	$query = "UPDATE nodo_tbl_devices SET group_id='0' WHERE group_id='$id' AND user_id='$userId'"; //indien deze group aan een device zat gekoppeld zetten we hem om 0
	mysql_query($query, $db) or die(mysql_error());
}

if (isset($_POST['addgroup'])) {

	$name = mysql_real_escape_string(htmlspecialchars($_POST['group_name_add']));
	if ($_POST['checkbox_devices_add'] == true ) { $devices = 1; } else { $devices = 0; }
	
	if ($_POST['checkbox_devices_default_add'] == true ) { 
	
		$devices_default = 1; 
		
		$query = "UPDATE nodo_tbl_groups SET devices_default='0' WHERE user_id='$userId'";
		mysql_query($query, $db) or die(mysql_error());
	}
	
	else { 
	
		$devices_default = 0; 
	}
	
	if ($_POST['checkbox_activities_default_add'] == true ) { 
		
		$activities_default = 1; 
		$query = "UPDATE nodo_tbl_groups SET activities_default='0' WHERE user_id='$userId'";
		mysql_query($query, $db) or die(mysql_error());
	
	} 
	
	else { 
		$activities_default = 0; 
	}
	
	mysql_query("INSERT INTO nodo_tbl_groups(user_id,name,devices,devices_default,activities,activities_default)
VALUES
('$userId','$name','$devices','$devices_default','$activities','$activities_default')") or die(mysql_error());

echo '1';

}

if (isset($_POST['editgroup']) && is_numeric($_POST['group_id_edit'])) {

	$id = $_POST['group_id_edit'];
	$name = mysql_real_escape_string(htmlspecialchars($_POST['group_name_edit']));
	if ($_POST['checkbox_devices_edit'] == true ) { $devices = 1; } else { $devices = 0; }
	if ($_POST['checkbox_activities_edit'] == true ) { $activities = 1; } else { $activities = 0; }
	
	if ($_POST['checkbox_devices_default_edit'] == true ) { 
	
		$devices_default = 1; 
		
		$query = "UPDATE nodo_tbl_groups SET devices_default='0' WHERE user_id='$userId'";
		mysql_query($query, $db) or die(mysql_error());
	}
	
	else { 
	
		$devices_default = 0; 
	}
	
	if ($_POST['checkbox_activities_default_edit'] == true ) { 
		
		$activities_default = 1; 
		$query = "UPDATE nodo_tbl_groups SET activities_default='0' WHERE user_id='$userId'";
		mysql_query($query, $db) or die(mysql_error());
	
	} 
	
	else { 
		$activities_default = 0; 
	}
	
	
	
	$query = "UPDATE nodo_tbl_groups SET name='$name', devices='$devices', devices_default='$devices_default', activities='$activities', activities_default='$activities_default' WHERE id='$id' AND user_id='$userId'";
	mysql_query($query, $db) or die(mysql_error());
	
echo '1';

}


?>