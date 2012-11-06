<?php
require_once('../connections/db_connection.php');
require_once('../include/auth.php');

mysql_select_db($database, $db);
$query = "SELECT * FROM nodo_tbl_devices WHERE user_id='$userId' ORDER BY sort_order ASC";
$recordset = mysql_query($query, $db) or die(mysql_error());


        while($row = mysql_fetch_array($recordset))
        {
             $rowsarray[] = array(
            "id" 		=> $row['id'],
			"naam" 		=> $row['naam'],
			"labelon" 	=> $row['label_on'],
			"labeloff" 	=> $row['label_off'],		
            "status" 	=> $row['status'],
			"type" 		=> $row['type'],
			"dim" 		=> $row['dim'],
            "dimvalue" 	=> $row['dim_value'],
			"group" 	=> $row['group_id'],
			"toggle"	=> $row['toggle']);
        }
 
$json = json_encode($rowsarray);


echo '{"devices":'. $json .'}'; 
 



?>