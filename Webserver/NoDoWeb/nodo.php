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

require_once('connections/db_connection.php'); 

//HTTPRequest function do not output http headers
function HTTPRequest($Url){

	   
    if (!function_exists('curl_init')){
        die('Sorry cURL is not installed!');
    }
 
    global $nodo_port;
    $ch = curl_init();
	curl_setopt($ch,CURLOPT_URL,$Url);
	curl_setopt($ch,CURLOPT_RETURNTRANSFER,1);
	curl_setopt($ch,CURLOPT_CONNECTTIMEOUT,2);
	curl_setopt($ch,CURLOPT_TIMEOUT,1);	
	curl_exec($ch);
	curl_close($ch);
	
	
}

//Zomer winter tijd check
if (date('I') == 0) {$DLS='No';} else {$DLS='Yes';}

$date = date("d-m-Y");
$time = date("H:i");
$dow= date("w")+1; // php zondag = 0 Nodo gaat uit van 1

header("Nodo-Date: Day=".$dow."; Date=".$date."; Time=".$time."; DLS=".$DLS);

/*
Login gegevens controleren
Checken of id=[nodoID] als url parameter is meegegeven.
*/
if (isset($_GET['id'])){

	$id = mysql_real_escape_string($_GET['id']);

	//User id uit de database halen doormede van NodoId	
	mysql_select_db($database, $db);
	$result = mysql_query("SELECT id,nodo_id,nodo_ip,nodo_port,nodo_password,cookie,cookie_count FROM nodo_tbl_users WHERE nodo_id='$id'") or die(mysql_error());  
	$row = mysql_fetch_array($result);
	$userId = $row['id']; 
	
	$nodo_ip = $row['nodo_ip'];
	$nodo_port = $row['nodo_port'];
	$cookie_counter = $row['cookie_count'] + 1;
	$cookie_webapp = $row['cookie'];
	$nodo_password = $row['nodo_password']; 

	$key_webapp = md5($cookie_webapp.":".$nodo_password);
	$key_nodo = $_GET['key'];

	if ($key_webapp == $key_nodo) {$key_match = 1;}


	//Nieuwe cookie en header informatie in de database opslaan
	if (isset($_GET['cookie'])){
		
		$cookie = mysql_real_escape_string($_GET['cookie']);
		$build = $_SERVER['HTTP_USER_AGENT'];
			
		mysql_query("UPDATE nodo_tbl_users SET cookie='$cookie', cookie_update=NOW(), cookie_count='$cookie_counter', 
					nodo_build='$build' WHERE id='$userId'") or die(mysql_error());
		
		header('Cookie: $cookie');
	}
}

//$key_match = 1; //LET OP!!!! alleen voor debug doeleingen

 
//Als de key correct is dan event= behandelen
if($userId > 0 && $key_match == 1) {

	//Alle komende output bufferen
	ob_end_clean();
	header("Connection: close");
	header("Content-Encoding: none");
	ignore_user_abort(); // optional
	ob_start();
		
		 	 	
	//Als de Nodo een file opvraagt dan zal onderstaande routine de file weergeven zodat de Nodo deze kan parsen
	if (isset($_GET['file'])){
	
		$file = mysql_real_escape_string($_GET['file']);
		
		$result = mysql_query("SELECT script FROM nodo_tbl_scripts WHERE file='$file' AND user_id='$userId'") or die(mysql_error());  
		$row = mysql_fetch_array($result);
		echo $row['script'];
		echo "\n";
	}
	
	
	//Vanaf hier wordt de verbinding verbroken. Output na deze regel komt nooit bij de client aan.
	$size = ob_get_length();
	header("Content-Length: $size");
	ob_end_flush(); 
	flush(); 

	 
     //Ontvangen event verwerken
	if (isset($_GET['event'])){
					 			 
		//Url parameters in variablen plaatsen
		$eventraw = mysql_real_escape_string($_GET['event']);
		$unit = mysql_real_escape_string($_GET['unit']);
		 
		//Commando en parameters uit event distileren
		//Format CMD PAR1,PAR2
		sscanf($eventraw,"%s %[^,],%s",$cmd,$par1,$par2);
		
		//Commando en parameters opschonen	
		$cmd = trim(strtolower($cmd));
		$par1 = trim(strtolower($par1));
		$par2 = trim(strtolower($par2));
					 
		//Event in nodo_tbl_event_log opslaan
		mysql_query("INSERT INTO nodo_tbl_event_log (user_id, nodo_unit_nr, event) VALUES ('$userId','$unit','$eventraw')") or die(mysql_error());
					 
		//Controleren of we een notificatie moeten sturen	
		$RSnotify = mysql_query("SELECT recipient, subject, body FROM nodo_tbl_notifications WHERE user_id='$userId' AND event='$eventraw'") or die(mysql_error());  
							
		while($row_RSnotify = mysql_fetch_array($RSnotify)) {                                
		   
			 $to = $row_RSnotify['recipient'];
			 $subject = $row_RSnotify['subject'];
			 $message = $row_RSnotify['body'];
			 $from = "webapp@nodo-domotica.nl";
			 $headers = "From:" . $from;
			 mail($to,$subject,$message,$headers);
		}
													
		//Controleren of de gebruiker een device of value heeft aangemaakt welke met het event overeenkomt
		switch ($cmd) {
			
			//Values
			case "wiredanalog":
			case "variable":
			case "variableset":
			case "pulsetime":
			case "pulsecount":
				 
				 if ($cmd == "wiredanalog") {$type=1;}
				 if ($cmd == "variable" || $cmd == "variableset" ) {$type=2;}
				 if ($cmd == "pulsetime") {$type=3;}
				 if ($cmd == "pulsecount") {$type=4;}

									
				//Sensor_id (id) opvragen uit values tabel 
				if ($type == 1 || $type == 2) { //wiredanalog, variable
					$RS_sensor = mysql_query("SELECT id FROM nodo_tbl_sensor WHERE user_id='$userId' AND par1='$par1' AND sensor_type='$type' AND nodo_unit_nr='$unit'") or die(mysql_error());  
				}
				
				if ($type == 3 || $type == 4) { //pulsetime, pulsecount
					$RS_sensor = mysql_query("SELECT id FROM nodo_tbl_sensor WHERE user_id='$userId' AND sensor_type='$type' AND nodo_unit_nr='$unit'") or die(mysql_error());  
				}
									
				while($row_RS_sensor = mysql_fetch_array($RS_sensor))  {
					 
					$sensor_id = $row_RS_sensor[id]; //$sensor_id gebruiken we om in de tabel sensor_data bij te houden welke data bij welke sensor hoort 
							
					//Alleen waarde opslaan als er een sensor is aangemaakt
					if ($sensor_id > 0) {
						if ($type == 1 || $type == 2) { //wiredanalog, variable
							mysql_query("INSERT INTO nodo_tbl_sensor_data (sensor_id, data) VALUES ('$sensor_id','$par2')") or die(mysql_error()); 
							mysql_query("UPDATE nodo_tbl_sensor SET data='$par2' WHERE par1='$par1' AND nodo_unit_nr='$unit' AND user_id='$userId' AND sensor_type='$type'") or die(mysql_error());
						}
						if ($type == 3 || $type == 4) { //pulsetime, pulsecount
							mysql_query("INSERT INTO nodo_tbl_sensor_data (sensor_id, data) VALUES ('$sensor_id','$par1')") or die(mysql_error()); 
							mysql_query("UPDATE nodo_tbl_sensor SET data='$par1' WHERE nodo_unit_nr='$unit' AND user_id='$userId' AND sensor_type='$type'") or die(mysql_error());
						}
					}
				}
			break;
			
			case "kaku" : // voor 517
			case "sendkaku" : // na 517
			case "kakusend" : // >=3.6
			
				switch ($par2) {

					case "on" :
						$status = 1;
					break;
					
					case "off" :
						$status = 0;
					break;
				}
				
				$homecode = substr($par1, 0,1);
				$address =  substr($par1, 1);
				
				mysql_query("UPDATE nodo_tbl_devices SET status='$status' WHERE address='$address' AND homecode='$homecode' AND type='1' AND user_id='$userId'") or die(mysql_error());   
							
			break;

			case "newkaku" : // voor 517
			case "sendnewkaku" : // na 517
			case "newkakusend" : // >=3.6									
				switch ($par2) {

					case "on" :
						$status = 1;
						mysql_query("UPDATE nodo_tbl_devices SET status='$status' WHERE address='$par1' AND type='2' AND user_id='$userId'") or die(mysql_error()); 
					break;
					
					case "off" :
						$status = 0;
						mysql_query("UPDATE nodo_tbl_devices SET status='$status' WHERE address='$par1' AND type='2' AND user_id='$userId'") or die(mysql_error()); 
					break;
					
					case 0 :
						$status = 0;
						mysql_query("UPDATE nodo_tbl_devices SET status='$status' WHERE address='$par1' AND type='2' AND user_id='$userId'") or die(mysql_error()); 
					break;
					
					case 1:
					case 2:
					case 3:
					case 4:
					case 5:
					case 6:
					case 7:
					case 8:
					case 9:
					case 10:
					case 11:
					case 12:
					case 13:
					case 14:
					case 15:
					case 16:
						$status = 1;
						mysql_query("UPDATE nodo_tbl_devices SET status='$status', dim_value='$par2' WHERE address='$par1' AND type='2' AND user_id='$userId'") or die(mysql_error()); 
					break;
				}
							
			break;
			
			case "wiredout" :
			
				switch ($par2) {

					case "on" :
						$status = 1;
					break;
					
					case "off" :
						$status = 0;
					break;
				}
				
				$address =  $par1;
				
				mysql_query("UPDATE nodo_tbl_devices SET status='$status' WHERE address='$address' AND type='3' AND user_id='$userId'") or die(mysql_error());   
							
			break;
			
			case "userevent" :
			
					$userevent = $par1.",".$par2;
				
					mysql_query("UPDATE nodo_tbl_devices SET 
								status = CASE
									WHEN user_event_on='$userevent' THEN '1'
									WHEN user_event_off='$userevent' THEN '0'
									ELSE status
								END
								WHERE user_id='$userId'") or die(mysql_error()); 
			break;
				
			
			}
	}
}



?>