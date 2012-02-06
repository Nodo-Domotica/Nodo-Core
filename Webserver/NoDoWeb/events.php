<?php require_once('connections/tc.php'); 
 


 // Login gegevens controleren
 
 
 //checken of id=[nodoID] als url parameter is meegegeven.
 if (isset($_GET['id'])){ 
	
		$id = mysql_real_escape_string(htmlspecialchars($_GET['id'])); 
	}
	
	else 
	
	{
	
		$id = "";
		
	}
//checken of pwd=[wachtwoord] als url parameter is meegegeven.
if (isset($_GET['password'])){ 
	
		$password = mysql_real_escape_string(htmlspecialchars($_GET['password'])); 
	}
	
	else 
	
	{
		$password = "";
	
	}
	


mysql_select_db($database_tc, $tc);
$result = mysql_query("SELECT * FROM nodo_tbl_users WHERE nodo_id='$id' AND nodo_password='$password'") or die(mysql_error());  
$row = mysql_fetch_array($result);
$userId = $row['id']; 
 
 
 //Als de logingegevens correct zijn dan event= behandelen
 if($userId > 0) {
 
			 if (isset($_GET['event'])){
			 
			 

			 //Event in nodo_tbl_event_log opslaan
			 
			 $eventraw = $_GET['event'];
			 $unit = $_GET['unit'];
			 mysql_select_db($database_tc, $tc);
			 mysql_query("INSERT INTO nodo_tbl_event_log (user_id, nodo_unit_nr, event) VALUES ('$userId','$unit','$eventraw')") or die(mysql_error());
			 
			 
			 
			$event = str_replace(" ",",",$eventraw); 

			$cmd_array = explode(',', $event);

			$cmd = strtolower($cmd_array[0]);
			$par1 = strtolower($cmd_array[1]);
			$par2 = strtolower($cmd_array[2]);
			//tijdelijke workaround omdat wiredanalog waarden met een spatie voor par2 worden verstuurd  (WiredAnalog 8, 99.9 ipv WiredAnalog 8,99.9)
			$par3 = strtolower($cmd_array[3]);
			
			echo "cmd: ";
			echo $cmd;
			echo "<br>";
			echo "PAR1: ";
			echo $par1;
			echo "<br>";
			echo "PAR2: ";
			echo $par2;
			echo "<br>";
			echo "PAR3: ";
			echo $par3;
			
			
			switch ($cmd) {
				
				
				case "wiredanalog" :
								
					// Waarde opslaan in sensor log
					mysql_select_db($database_tc, $tc);
					mysql_query("INSERT INTO nodo_tbl_sensor_data (port, data, nodo_unit_nr, user_id) VALUES ('$par1', '$par3','$unit','$userId')") or die(mysql_error()); 

					// Sensor updaten
					mysql_select_db($database_tc, $tc);
					mysql_query("UPDATE nodo_tbl_sensor SET data='$par3' WHERE port='$par1' AND nodo_unit_nr='$unit' AND user_id='$userId'") or die(mysql_error());
						
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
					
					// save the data to the database 
					mysql_select_db($database_tc, $tc);
					mysql_query("UPDATE nodo_tbl_devices SET status='$status' WHERE address='$address' AND homecode='$homecode' AND user_id='$userId'") or die(mysql_error());   
								
				break;

				case "newkaku" :
													
					switch ($par2) {

						case "on" :
							$status = 1;
							// save the data to the database 
							mysql_select_db($database_tc, $tc);
							mysql_query("UPDATE nodo_tbl_devices SET status='$status' WHERE address='$par1' AND homecode='0' AND user_id='$userId'") or die(mysql_error()); 
						break;
						
						case "off" :
							$status = 0;
							// save the data to the database 
							mysql_select_db($database_tc, $tc);
							mysql_query("UPDATE nodo_tbl_devices SET status='$status' WHERE address='$par1' AND homecode='0' AND user_id='$userId'") or die(mysql_error()); 
						break;
						
						case 0 :
							$status = 0;
							// save the data to the database 
							mysql_select_db($database_tc, $tc);
							mysql_query("UPDATE nodo_tbl_devices SET status='$status' WHERE address='$par1' AND homecode='0' AND user_id='$userId'") or die(mysql_error()); 
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
							// save the data to the database 
							mysql_select_db($database_tc, $tc);
							mysql_query("UPDATE nodo_tbl_devices SET status='$status', dim_value='$par2' WHERE address='$par1' AND homecode='0' AND user_id='$userId'") or die(mysql_error()); 
						break;
					}
					
					  
					
				break;

				default :

				break;

				}


			}
 
}
else
{

	echo "Please authenticate...";

}
  
 ?>