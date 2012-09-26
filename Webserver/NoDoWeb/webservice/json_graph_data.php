<?php 
require_once('../connections/db_connection.php');
require_once('../include/auth.php');
require_once('../include/user_settings.php');

//uitzoeken waarom GMT+1 niet werkt!!??
date_default_timezone_set('Etc/GMT');

$sensor_id=$_GET['sensor_id'];
$hours=$_GET['hours'];
$bars=$_GET['bars'];
$date1=$_GET['date1'];
$date2=$_GET['date2'];
$filter=$_GET['filter']; //1 day totals, 2 line graph

if ($bars == 1) {

	if ($filter == 1) {

		$query_RSsensor_value_data = "SELECT DATE_FORMAT(timestamp , '%Y-%m-%d') as timestamp , ROUND(SUM(data),2) as data FROM nodo_tbl_sensor_data WHERE sensor_id='$sensor_id' AND timestamp BETWEEN '$date1' AND date_add( '$date2' , interval 1 day)  GROUP BY date(timestamp)";
	
	}
	
	else {
	
		$query_RSsensor_value_data = "SELECT DATE_FORMAT(timestamp , '%Y-%m-%d') as timestamp , ROUND(SUM(data),2) as data FROM nodo_tbl_sensor_data WHERE sensor_id='$sensor_id' AND timestamp >= NOW() - INTERVAL $hours HOUR GROUP BY date(timestamp)";
	
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
  
while($row = mysql_fetch_assoc($result))  
{  
 
$dataset1[] = array((strtotime($row['timestamp'])*1000),$row['data']);  
}  

echo json_encode($dataset1); 



?> 
