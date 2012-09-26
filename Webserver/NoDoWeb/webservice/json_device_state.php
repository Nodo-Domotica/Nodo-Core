<?php
require_once('../connections/db_connection.php');
require_once('../include/auth.php');

mysql_select_db($database, $db);
if(isset($_GET['id'])) { 
	
	$id = $_GET['id'];
	$query_RSdevice = "SELECT id,user_id,naam,status,dim_value FROM nodo_tbl_devices WHERE user_id='$userId' and id='$id'";
	
}
else {
	
	$query_RSdevice = "SELECT id,user_id,naam,status,dim_value FROM nodo_tbl_devices WHERE user_id='$userId'";
}


$RSdevice = mysql_query($query_RSdevice, $db) or die(mysql_error());
//$row_RSdevice = mysql_fetch_assoc($RSdevice);




        while($row_RSdevice = mysql_fetch_array($RSdevice))
        {
             $rows[] = array(
            "id" => $row_RSdevice['id'],
			//"naam" => $row_RSdevice['naam'],
            "status" => $row_RSdevice['status'],
            "dimvalue" => $row_RSdevice['dim_value']);
        }
 
$json = json_encode($rows);


 echo $json;



?>