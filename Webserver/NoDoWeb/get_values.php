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
require_once('include/auth.php');


mysql_select_db($database, $db);
$query_RSvalue = "SELECT id,user_id,display,data,sensor_suffix_false,sensor_suffix_true,graph_type,graph_hours,graph_min_ticksize FROM nodo_tbl_sensor WHERE user_id='$userId'";
$RSvalue = mysql_query($query_RSvalue, $db) or die(mysql_error());
$row_RSvalue = mysql_fetch_assoc($RSvalue);

?>


<?php if ($row_RSvalue != NULL){
do { ?>document.getElementById('value_<?php echo $row_RSvalue['id'];?>').innerHTML = <?php 
   
   if ($row_RSvalue['display'] == 1){
   
		//Lijn grafiek dus laatste meting weergeven
		if ($row_RSvalue['graph_type'] <=1 ){
		
			echo "'".$row_RSvalue['data']."';";
		}
		
		if ($row_RSvalue['graph_type'] == 2){
		
			$sensor_id = $row_RSvalue['id'];
		
			if ($row_RSvalue['graph_hours'] == 0) {
					
					$graph_hours = "24";
				}
				else {
					
					$graph_hours = $row_RSvalue['graph_hours'];
				}
		
		//Staaf grafiek, totaal meting per dag weergeven
		$query_RSsensor_value_data = "SELECT DATE_FORMAT(timestamp , '%Y-%m-%d') as timestamp , ROUND(SUM(data),2) as data FROM nodo_tbl_sensor_data WHERE sensor_id='$sensor_id' AND timestamp >= SYSDATE() - INTERVAL $graph_hours HOUR GROUP BY date(timestamp) ORDER BY timestamp DESC LIMIT 1;";
		$RSsensor_value_data = mysql_query($query_RSsensor_value_data, $db) or die(mysql_error()); 
		$row_RSsensor_value_data = mysql_fetch_assoc($RSsensor_value_data);
		  
		echo "'".$row_RSsensor_value_data['data']."';";
		
		}
			
   
   }
   
   if ($row_RSvalue['display'] == 2){
	
		if ($row_RSvalue['data'] <= 0){
		
			echo "'".$row_RSvalue['sensor_suffix_false']."';";
		}
		
		if ($row_RSvalue['data'] > 0){
		
			echo "'".$row_RSvalue['sensor_suffix_true']."';";
		}
		
		
   }
   
	
 } while ($row_RSvalue = mysql_fetch_assoc($RSvalue)); }

mysql_free_result($RSvalue);

?>
