<?php 
require_once('../connections/db_connection.php');
require_once('../include/auth.php');
require_once('../include/user_settings.php');

//uitzoeken waarom GMT+1 niet werkt!!??
date_default_timezone_set('Etc/GMT');

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
	 
	// sanitize input
	$equation = preg_replace("/[^0-9+\-.*\/()%]/","",$equation);
	 
	// convert percentages to decimal
	$equation = preg_replace("/([+-])([0-9]{1})(%)/","*(1\$1.0\$2)",$equation);
	$equation = preg_replace("/([+-])([0-9]+)(%)/","*(1\$1.\$2)",$equation);
	$equation = preg_replace("/([0-9]+)(%)/",".\$1",$equation);
	 
	if ( $equation != "" ) {
		$result = @eval("return " . $equation . ";" );
	}
	 
	if ($result === false) {
		return "0"; //Unable to calculate equation
	}
	 
	return $result;
}

$sensor_id=$_GET['sensor_id'];
$hours=$_GET['hours'];
$bars=$_GET['bars'];
$date1=$_GET['date1'];
$date2=$_GET['date2'];
$filter=$_GET['filter']; //1 day totals, 2 line graph

mysql_select_db($database, $db);
$query_values = mysql_query("SELECT * FROM nodo_tbl_sensor WHERE id='$sensor_id'") or die(mysql_error()); 
$row_values = mysql_fetch_array($query_values);

$formula1 = $row_values['formula'];



if ($bars == 1) {

	if ($filter == 1) {

		$query_RSsensor_value_data = "SELECT DATE_FORMAT(timestamp , '%Y-%m-%d') as timestamp , SUM(data) as data FROM nodo_tbl_sensor_data WHERE sensor_id='$sensor_id' AND timestamp BETWEEN '$date1' AND date_add( '$date2' , interval 1 day)  GROUP BY date(timestamp)";
	
	}
	
	else {
	
		$query_RSsensor_value_data = "SELECT DATE_FORMAT(timestamp , '%Y-%m-%d') as timestamp , SUM(data) as data FROM nodo_tbl_sensor_data WHERE sensor_id='$sensor_id' AND timestamp >= NOW() - INTERVAL $hours HOUR GROUP BY date(timestamp)";
	
	}

}

else {

	if ($filter == 2) {

		$query_RSsensor_value_data = "SELECT data,timestamp FROM nodo_tbl_sensor_data WHERE sensor_id='$sensor_id' AND timestamp BETWEEN '$date1' AND '$date2'";
}
	
	else {
	
		$query_RSsensor_value_data = "SELECT data,timestamp FROM nodo_tbl_sensor_data WHERE sensor_id='$sensor_id' AND timestamp >= NOW() - INTERVAL $hours HOUR";
		
	}
}

$result = mysql_query($query_RSsensor_value_data) or die("Could not query ".mysql_error());  
  
while($row = mysql_fetch_assoc($result)) {  
	
	if ($row_values['formula'] != '') {
					
					$formula = str_ireplace("%value%",$row['data'],$row_values['formula']);
					$data = round(evalmath($formula),$row_values['round']);
				}
				else {
					$data = $row['data'];
				}
			
				$value = $data;

	$dataset1[] = array((strtotime($row['timestamp'])*1000),$value);  
}  



echo json_encode($dataset1); 



?> 
