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

	
//Door gebruiker ingevoerd event bij activiteiten
$event = str_replace ( ' ', '%20',$_GET["event"]);


$action = $_GET["action"];
$device_id = $_GET["device_id"];
$dim = $_GET["dim"];


//Events voor het schakelen van devices genereren
if ($action != NULL) { 

	mysql_select_db($database_tc, $tc);
	$query_RSdevices = "SELECT * FROM nodo_tbl_devices WHERE user_id='$userId' AND id='$device_id' ORDER BY sort_order ASC";
	$RSdevices = mysql_query($query_RSdevices, $tc) or die(mysql_error());
	$row_RSdevices = mysql_fetch_assoc($RSdevices);
	if ($row_RSdevices['status'] == 1) {$status=1;} else {$status=0;}
	$type = $row_RSdevices['type'];
	
		
			switch ($type) {
			
				case 1: //KAKU
		
					$homecode = $row_RSdevices['homecode'];
					$address = $row_RSdevices['address'];
					
					switch ($action) {
						
						case "toggle":
				
							if ($status == 1) {
								get_data("http://$nodo_ip:$nodo_port/?event=SendKaku%20$homecode$address,off&password=$nodo_password&id=$nodo_id");
							}
							elseif ($status == 0) {
								get_data("http://$nodo_ip:$nodo_port/?event=SendKaku%20$homecode$address,on&password=$nodo_password&id=$nodo_id");
							}
						break;
						
						case "on":
								get_data("http://$nodo_ip:$nodo_port/?event=SendKaku%20$homecode$address,on&password=$nodo_password&id=$nodo_id");
						break;
						
						case "off":
								get_data("http://$nodo_ip:$nodo_port/?event=SendKaku%20$homecode$address,off&password=$nodo_password&id=$nodo_id");
						break;
					}
				break;
				
				case 2: //NewKAKU
				
					$address = $row_RSdevices['address'];
					
					switch ($action) {
							
						case "toggle":
									
							if ($status == 1) {
								get_data("http://$nodo_ip:$nodo_port/?event=SendNewKaku%20$address,off&password=$nodo_password&id=$nodo_id");
							}
							elseif ($status == 0) {
								get_data("http://$nodo_ip:$nodo_port/?event=SendNewKaku%20$address,on&password=$nodo_password&id=$nodo_id");
							}
						break;
						
						case "on":
							get_data("http://$nodo_ip:$nodo_port/?event=SendNewKaku%20$address,on&password=$nodo_password&id=$nodo_id");
						break;
						
						case "off":
							get_data("http://$nodo_ip:$nodo_port/?event=SendNewKaku%20$address,off&password=$nodo_password&id=$nodo_id");
						break;
						
						case "dim":
			
							$address = $row_RSdevices['address'];
							get_data("http://$nodo_ip:$nodo_port/?event=SendNewKaku%20$address,$dim&password=$nodo_password&id=$nodo_id");
								
						break;	
					}	
							
							
				break;

				case 3: //WiredIn
					
					$address = $row_RSdevices['address'];
					
					switch ($action) {
							
						case "toggle":
									
							if ($status == 1) {
							get_data("http://$nodo_ip:$nodo_port/?event=WiredOut%20$address,off;Sendstatus%20WiredOut%20$address&password=$nodo_password&id=$nodo_id");
							}
							elseif ($status == 0) {
								get_data("http://$nodo_ip:$nodo_port/?event=WiredOut%20$address,on;Sendstatus%20WiredOut%20$address&password=$nodo_password&id=$nodo_id");
							}
						break;
						
						case "on":
							get_data("http://$nodo_ip:$nodo_port/?event=WiredOut%20$address,on;Sendstatus%20WiredOut%20$address&password=$nodo_password&id=$nodo_id");
						break;
						
						case "off":
							get_data("http://$nodo_ip:$nodo_port/?event=WiredOut%20$address,off;Sendstatus%20WiredOut%20$address&password=$nodo_password&id=$nodo_id");
						break;
					}
					
					
				break;

				case 4: //UserEvent
					
					$user_event_on = $row_RSdevices['user_event_on'];
					$user_event_off = $row_RSdevices['user_event_off'];
					
					switch ($action) {
							
						case "toggle":
							if ($status == 1) {
							get_data("http://$nodo_ip:$nodo_port/?event=SendUserEvent%20$user_event_off&password=$nodo_password&id=$nodo_id");
							}
							elseif ($status == 0) {
							get_data("http://$nodo_ip:$nodo_port/?event=SendUserEvent%20$user_event_on&password=$nodo_password&id=$nodo_id");
							}
						break;
						
						case "on":
							get_data("http://$nodo_ip:$nodo_port/?event=SendUserEvent%20$user_event_on&password=$nodo_password&id=$nodo_id");
						break;
						
						case "off":
							get_data("http://$nodo_ip:$nodo_port/?event=SendUserEvent%20$user_event_off&password=$nodo_password&id=$nodo_id");
						break;
					}
									
				break;
					
			}
		
}


//Event doorsturen zoals opgegeven door gebruiker
if ($event != NULL) { 

	get_data("http://$nodo_ip:$nodo_port/?event=$event&password=$nodo_password&id=$nodo_id");
	
}

?>