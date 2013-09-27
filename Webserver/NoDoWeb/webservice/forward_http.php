<?php
/***********************************************************************************************************************
"Nodo Web App" Copyright © 2012 Martin de Graaf

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*************************************************************************************************************************/

require_once('../connections/db_connection.php'); 
require_once('../include/auth.php');
require_once('../include/user_settings.php');

//Alleen events versturen indien de Nodo online is
if ($heartbeat == "lost") {die('No connection to Nodo!!!'); }

//Stuur event via HTTP naar Nodo
function send_event($event)
	{
		global $nodo_ip, $nodo_port, $key;
				
		$url = "http://$nodo_ip:$nodo_port/?event=$event&key=$key";
				
		$ch = curl_init();
		$timeout = 0;
		curl_setopt($ch,CURLOPT_URL,$url);
		curl_setopt($ch,CURLOPT_RETURNTRANSFER,0);
		curl_setopt($ch,CURLOPT_CONNECTTIMEOUT,5);
		curl_setopt($ch,CURLOPT_TIMEOUT,2);	
		$data = curl_exec($ch);
		curl_close($ch);
		return $data;
	}

	
//Door gebruiker ingevoerd commando bij activiteiten
$user_cmd = str_replace ( ' ', '%20',$_GET["event"]);


$action = $_GET["action"];
$device_id = $_GET["device_id"];
$dim = $_GET["dim"];


//Events voor het schakelen van devices genereren
if ($action != NULL) { 

	mysql_select_db($database, $db);
	$query_RSdevices = "SELECT * FROM nodo_tbl_devices WHERE user_id='$userId' AND id='$device_id'";
	$RSdevices = mysql_query($query_RSdevices, $db) or die(mysql_error());
	$row_RSdevices = mysql_fetch_assoc($RSdevices);
	if ($row_RSdevices['status'] == 1) {$status=1;} else {$status=0;}
	$type = $row_RSdevices['type'];
	
	$homecode = $row_RSdevices['homecode'];
	$address = $row_RSdevices['address'];
	
	$kaku = "SendKaku%20$homecode$address,";
	$kaku_on = $kaku . "on";
	$kaku_off = $kaku . "off";
	
	$newkaku = "SendNewKaku%20$address,";
	$newkaku_on = $newkaku . "on";
	$newkaku_off = $newkaku . "off";
	$newkaku_dim = $newkaku . $dim;
	
	$wired_out_1 = "WiredOut%20$address,";
	$wired_out_on = $wired_out_1 . "on";
	$wired_out_off = $wired_out_1 . "off";
	
	$send_user_event = "SendUserEvent%20";
	$send_user_event_on = $send_user_event . $row_RSdevices['user_event_on'];
	$send_user_event_off = $send_user_event . $row_RSdevices['user_event_off'];
	
	$user_event = "UserEvent%20";
	$user_event_on = $user_event . $row_RSdevices['user_event_on'];
	$user_event_off = $user_event . $row_RSdevices['user_event_off'];
	
	$rawsignal_send = "Rawsignalsend%20";
	$rawsignal_send_on = $rawsignal_send . $row_RSdevices['rawsignal_on'];
	$rawsignal_send_off = $rawsignal_send . $row_RSdevices['rawsignal_off'];

	
	
		
			switch ($type) {
			
				case 1: //KAKU
		
					switch ($action) {
						
						case "toggle":
				
							if ($status == 1) {
								send_event($kaku_off);
							}
							elseif ($status == 0) {
								send_event($kaku_on);
							}
						break;
						
						case "on":
								send_event($kaku_on);
						break;
						
						case "off":
								send_event($kaku_off);
						break;
					}
				break;
				
				case 2: //NewKAKU
				
					switch ($action) {
							
						case "toggle":
									
							if ($status == 1) {
								send_event($newkaku_off);
							}
							elseif ($status == 0) {
								send_event($newkaku_on);
							}
						break;
						
						case "on":
							send_event($newkaku_on);
						break;
						
						case "off":
							send_event($newkaku_off);
						break;
						
						case "dim":
			
							$address = $row_RSdevices['address'];
							send_event($newkaku_dim);
								
						break;	
					}	
							
							
				break;

				case 3: //WiredOut
					
									
					switch ($action) {
							
						case "toggle":
									
							if ($status == 1) {
							send_event($wired_out_off);
							}
							elseif ($status == 0) {
								send_event($wired_out_on);
							}
						break;
						
						case "on":
							send_event($wired_out_on);
						break;
						
						case "off":
							send_event($wired_out_off);
						break;
					}
					
					
				break;

				case 4: //SendUserEvent
					
					
					
					switch ($action) {
							
						case "toggle":
							if ($status == 1) {
							send_event($send_user_event_off);
							}
							elseif ($status == 0) {
							send_event($send_user_event_on);
							}
						break;
						
						case "on":
							send_event($send_user_event_on);
						break;
						
						case "off":
							send_event($send_user_event_off);
						break;
					}
									
				break;
				
				case 5: //UserEvent
										
					switch ($action) {
							
						case "toggle":
							if ($status == 1) {
							send_event($user_event_off);
							}
							elseif ($status == 0) {
							send_event($user_event_on);
							}
						break;
						
						case "on":
							send_event($user_event_on);
						break;
						
						case "off":
							send_event($user_event_off);
						break;
					}
									
				break;
				
				case 6: //Rawsignalsend
										
					switch ($action) {
							
						case "toggle":
							if ($status == 1) {
							send_event($rawsignal_send_off);
							}
							elseif ($status == 0) {
							send_event($rawsignal_send_on);
							}
						break;
						
						case "on":
							send_event($rawsignal_send_on);
						break;
						
						case "off":
							send_event($rawsignal_send_off);
						break;
					}
									
				break;
					
			}
		
}


//Event doorsturen zoals opgegeven door gebruiker
if ($user_cmd != NULL) { 

	send_event($user_cmd);
	
}

echo $data;

?>