<?php

require_once('connections/tc.php'); 
require_once('include/auth.php');
require_once('include/user_settings.php');

/* Stuur parameters naar HTTP server bijvoorbeeld ?event=sendkaku a1,on&password=[password]&id=[id] */
function get_data($url)
	{
		$ch = curl_init();
		$timeout = 0;
		curl_setopt($ch,CURLOPT_URL,$url);
		curl_setopt($ch,CURLOPT_RETURNTRANSFER,0);
		curl_setopt($ch,CURLOPT_CONNECTTIMEOUT,10);
		$data = curl_exec($ch);
		curl_close($ch);
		return $data;
	}

$event = str_replace ( ' ', '%20',$_GET["event"]);

if ($event != NULL) { 

	get_data("http://$nodo_ip:$nodo_port/?event=$event&password=$nodo_password&id=$nodo_id");

}

?>