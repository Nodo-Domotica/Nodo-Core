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


require_once('connections/tc.php'); 


/*
Login gegevens controleren
Checken of id=[nodoID] als url parameter is meegegeven.
*/
if (isset($_GET['id'])){$id = mysql_real_escape_string($_GET['id']);} else {$id = "";}

//Checken of pwd=[wachtwoord] als url parameter is meegegeven.
if (isset($_GET['password'])){$password = mysql_real_escape_string($_GET['password']);} else {$password = "";}

	

mysql_select_db($database_tc, $tc);
$result = mysql_query("SELECT id,nodo_id,nodo_password FROM nodo_tbl_users WHERE nodo_id='$id' AND nodo_password='$password'") or die(mysql_error());  
$row = mysql_fetch_array($result);
$userId = $row['id']; 
 
 
//Als de logingegevens correct zijn dan event= behandelen
if($userId > 0) {

	 //Alle komende output bufferen
	ob_start();
	
	
	 	 	
	//Als de Nodo een file opvraagt dan zal onderstaande routine de file weergeven zodat de Nodo deze kan parsen
	if (isset($_GET['file'])){
	
		$file = $_GET['file'];
		
		$result = mysql_query("SELECT * FROM nodo_tbl_scripts WHERE file='$file' AND user_id='$userId'") or die(mysql_error());  
		$row = mysql_fetch_array($result);
		echo $row['script'];
		echo "\n";
	}
 
    //Grootte van de output opvragen
	$size = ob_get_length();

	//!!!!Zaken welke output voor de Nodo gegeneren moeten voor het sluiten van de connectie worden geplaatst!!!!!
	
	//Headers naar de client sturen zodat deze de verbinding verbreekt.
	header("Content-Length: $size");
	header('Connection: close');

	ob_end_flush();
	ob_flush();
	flush();	

	//Hier begint het achtergrond process, de connectie is op dit moment al verbroken
					 
				 
	 //Ontvangen events verwerken
	 if (isset($_GET['event'])){
			 
			 			 
			//Url parameters in variablen plaatsen
			$eventraw = $_GET['event'];
			$unit = $_GET['unit'];
			 
			//Spaties vervangen door , om makkelijk een array te kunnen maken
			$event = str_replace(" ",",",$eventraw); 
			
			//Commando en parameters uit event distileren	
			$cmd_array = explode(',', $event);
			$cmd = strtolower($cmd_array[0]);
			$par1 = strtolower($cmd_array[1]);
			$par2 = strtolower($cmd_array[2]);
			 
			 
			//Event in nodo_tbl_event_log opslaan
			mysql_query("INSERT INTO nodo_tbl_event_log (user_id, nodo_unit_nr, event) VALUES ('$userId','$unit','$eventraw')") or die(mysql_error());
			//Kijken of er meer dan 10000 events gelogd zijn voor de gebruiker dan wissen we het eerste record
			mysql_query("DELETE FROM nodo_tbl_event_log WHERE user_id=$userId AND id<(SELECT min(id) FROM(SELECT id FROM nodo_tbl_event_log WHERE user_id=$userId ORDER BY id DESC LIMIT 10000) AS myselect )") or die(mysql_error());
			 
			 
			//Kijken of we een notificatie moeten sturen			
			$RSnotify = mysql_query("SELECT * FROM nodo_tbl_notifications WHERE user_id='$userId' AND event='$eventraw'") or die(mysql_error());  
			
					
			while($row_RSnotify = mysql_fetch_array($RSnotify)) 
				{                                
			   
					 $to = $row_RSnotify['recipient'];
					 $subject = $row_RSnotify['subject'];
					 $message = $row_RSnotify['body'];
					 $from = "webapp@nodo-domotica.nl";
					 $headers = "From:" . $from;
					 mail($to,$subject,$message,$headers);
					 
				}

														
			switch ($cmd) {
				
				
				case "wiredanalog":
				case "variable":
					 
					 if ($cmd == "wiredanalog") {$type=1;}
					 if ($cmd == "variable") {$type=2;}
					

										
					//Sensor_id (id) opvragen uit values tabel 
					$RS_sensor = mysql_query("SELECT * FROM nodo_tbl_sensor WHERE user_id='$userId' AND par1='$par1' AND sensor_type='$type'") or die(mysql_error());  
					$row_RS_sensor = mysql_fetch_array($RS_sensor); //Eerste sensor ophalen volgende sensoren met dezelfde parameters worden niet behandeld.
					$sensor_id=$row_RS_sensor[id]; //$sensor_id gebruiken we om in de tabel sensor_data bij te houden welke data bij welke sensor hoort 
								
					//Alleen waarde opslaan als er een sensor is aangemaakt
					if ($sensor_id > 0) {
						mysql_query("INSERT INTO nodo_tbl_sensor_data (sensor_id, par1, data, nodo_unit_nr, type, user_id) VALUES ('$sensor_id','$par1','$par2','$unit','$type','$userId')") or die(mysql_error()); 
					}
					
					//Waarde updaten
					mysql_select_db($database_tc, $tc);
					mysql_query("UPDATE nodo_tbl_sensor SET data='$par2' WHERE par1='$par1' AND nodo_unit_nr='$unit' AND user_id='$userId' AND sensor_type='$type'") or die(mysql_error());
						
				break;

			
				case "kaku" :
				
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

				case "newkaku" :
													
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
					
						mysql_query("UPDATE nodo_tbl_devices SET status='1' WHERE user_event_on='$userevent' AND type='4' AND user_id='$userId'") or die(mysql_error()); 
						mysql_query("UPDATE nodo_tbl_devices SET status='0' WHERE user_event_off='$userevent' AND type='4' AND user_id='$userId'") or die(mysql_error()); 						
						
		
				}

		}
	
}


  
 ?>