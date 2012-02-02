<?php

require_once('connections/tc.php'); 
require_once('include/auth.php'); 


mysql_select_db($database_tc, $tc); 
$result = mysql_query("SELECT * FROM nodo_tbl_users WHERE id='$userId'") or die(mysql_error());  
$rowUsers = mysql_fetch_array($result);

/* Stuur parameters naar HTTP server bijvoorbeeld ?event=sendkaku a1,on&password=[password]&id=[id] */
function get_data($url)
	{
		$ch = curl_init();
		$timeout = 0;
		curl_setopt($ch,CURLOPT_URL,$url);
		curl_setopt($ch,CURLOPT_RETURNTRANSFER,0);
		curl_setopt($ch,CURLOPT_CONNECTTIMEOUT,$timeout);
		$data = curl_exec($ch);
		curl_close($ch);
		return $data;
	}

$id_1 = str_replace ( ' ', '%20',$_GET["event"]);
$nodo_ip = $rowUsers['nodo_ip'];
$nodo_port = $rowUsers['nodo_port'];
$nodo_id = $rowUsers['nodo_id'];
$nodo_password = $rowUsers['nodo_password'];

if ($id_1 != NULL) { 

get_data("http://$nodo_ip:$nodo_port/?event=$id_1&password=$nodo_password&id=$nodo_id");
}

?>