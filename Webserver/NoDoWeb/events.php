<?php require_once('/Connections/tc.php'); 
 


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
$result = mysql_query("SELECT * FROM NODO_tbl_users WHERE nodo_id='$id' AND nodo_password='$password'") or die(mysql_error());  
$row = mysql_fetch_array($result);
$userId = $row['id']; 
 
 
 //Als de logingegevens correct zijn dan event= behandelen
 if($userId > 0) {
 
			 if (isset($_GET['event'])){

			 //Event in nodo_tbl_event_log opslaan
			 
			 $eventraw = $_GET['event'];
			 mysql_select_db($database_tc, $tc);
			 mysql_query("INSERT INTO nodo_tbl_event_log (user_id, event, timestamp) VALUES ('$userId','$eventraw',now() )") or die(mysql_error());
			 
			 
			 
			$event = str_replace(" ",",",$_GET['event']); 

			$cmd_array = explode(',', $event);

			$cmd = strtolower($cmd_array[0]);
			$par1 = strtolower($cmd_array[1]);
			$par2 = strtolower($cmd_array[2]);

			switch ($cmd) {

				case "wiredanalog" :
								
					// save the data to the database 
					mysql_select_db($database_tc, $tc);
					mysql_query("INSERT INTO nodo_tbl_sensor_data (port, data, user_id) VALUES ('$par1', '$par2','$userId' )") or die(mysql_error());   
						
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
					echo $homecode;
					// save the data to the database 
					mysql_select_db($database_tc, $tc);
					mysql_query("UPDATE nodo_tbl_devices SET status='$status' WHERE address='$address' AND homecode='$homecode' AND user_id='$userId'") or die(mysql_error());   
					
				
				
				break;

				case "newkaku" :
													
					switch ($par2) {

						case "on" :
							$status = 1;
						break;
						
						case "off" :
							$status = 0;
						break;
						
						case 0 :
							$status = 0;
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
						break;
					}
					$address = $par1;
					// save the data to the database 
					mysql_select_db($database_tc, $tc);
					mysql_query("UPDATE nodo_tbl_devices SET status='$status' WHERE address='$address' AND homecode='0' AND user_id='$userId'") or die(mysql_error());   
					
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