<?php

//Lees setup waarden uit de database
mysql_select_db($database_tc, $tc);
$RS_setup = mysql_query("SELECT * FROM nodo_tbl_users WHERE id='$userId'") or die(mysql_error());  
$row_RSsetup = mysql_fetch_array($RS_setup);

if($row_RSsetup['webapp_theme'] == "") {  
	$theme = "a";
} 
else { 
	$theme = $row_RSsetup['webapp_theme'];
}

if($row_RSsetup['webapp_theme_header'] == "") {  
	$theme_header = "a";
} 
else { 
	$theme_header = $row_RSsetup['webapp_theme_header'];
}

if($row_RSsetup['webapp_title'] == "") {  
	$title = "NoDo Webapp";
} 
else { 
	$title = $row_RSsetup['webapp_title'];
}

$send_method = $row_RSsetup['send_method'];
$nodo_ip = $row_RSsetup['nodo_ip'];
$nodo_port = $row_RSsetup['nodo_port'];
$nodo_password = $row_RSsetup['nodo_password'];
$nodo_id = $row_RSsetup['nodo_id'];
?>