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


mysql_select_db($database, $db);
$query_RSvalue = "SELECT id,user_id,display,data,formula,round,sensor_suffix_false,sensor_suffix_true,graph_type,graph_hours,graph_min_ticksize FROM nodo_tbl_sensor WHERE user_id='$userId'";
$RSvalue = mysql_query($query_RSvalue, $db) or die(mysql_error());
$row_RSvalue = mysql_fetch_assoc($RSvalue);

/**
* Evaluates a math equation and returns the result, sanitizing
* input for security purposes.  Note, this function does not
* support math functions (sin, cos, etc)
*
* @param string the math equation to evaluate (ex:  100 * 24)
* @return number
*/
function evalmath($equation) {

	$result = 0;
	 
	// sanitize imput
	$equation = preg_replace("/[^0-9+\-.*\/()%]/","",$equation);
	 
	// convert percentages to decimal
	$equation = preg_replace("/([+-])([0-9]{1})(%)/","*(1\$1.0\$2)",$equation);
	$equation = preg_replace("/([+-])([0-9]+)(%)/","*(1\$1.\$2)",$equation);
	$equation = preg_replace("/([0-9]+)(%)/",".\$1",$equation);
	 
	if ( $equation != "" ) {
		$result = @eval("return " . $equation . ";" );
	}
	 
	if ($result === false) {
		return "Error in formula!"; //Unable to calculate equation
	}
	 
	return $result;
}



if ($row_RSvalue != NULL){
do {  
   
   $formula = $row_RSvalue['formula'];
   
   
   
   if ($row_RSvalue['display'] == 1){
   
		//Lijn grafiek dus laatste meting weergeven
		if ($row_RSvalue['graph_type'] <=1 ){
		
			
			if ($formula != '') {
				
				$formula = str_ireplace("%value%",$row_RSvalue['data'],$formula);
				$data = round(evalmath($formula),$row_RSvalue['round']);
			}
			else {
				$data = $row_RSvalue['data'];
			}
		
			$value = $data;
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
		$query_RSsensor_value_data = "SELECT DATE_FORMAT(timestamp , '%Y-%m-%d') as timestamp , SUM(data) as data FROM nodo_tbl_sensor_data WHERE sensor_id='$sensor_id' AND timestamp >= NOW() - INTERVAL $graph_hours HOUR GROUP BY date(timestamp) ORDER BY timestamp DESC LIMIT 1;";
		$RSsensor_value_data = mysql_query($query_RSsensor_value_data, $db) or die(mysql_error()); 
		$row_RSsensor_value_data = mysql_fetch_assoc($RSsensor_value_data);
			
		if ($formula != '') {
				
				$formula = str_ireplace("%value%",$row_RSsensor_value_data['data'],$formula);
				$data = round(evalmath($formula),$row_RSvalue['round']);
			}
			else {
				$data = $row_RSsensor_value_data['data'];
			}
		
			$value = $data;	
		
			
		}
			
   
   }
   
   if ($row_RSvalue['display'] == 2){
	
		if ($row_RSvalue['data'] <= 0){
		
			$value = $row_RSvalue['sensor_suffix_false'];
		}
		
		if ($row_RSvalue['data'] > 0){
		
			$value = $row_RSvalue['sensor_suffix_true'];
		}
		
		
   }
   
	  $rows[] = array(
            "id" => $row_RSvalue['id'],
			"value" => $value);
	
 } while ($row_RSvalue = mysql_fetch_assoc($RSvalue)); }

mysql_free_result($RSvalue);

$json = json_encode($rows);


 echo $json;



?>
