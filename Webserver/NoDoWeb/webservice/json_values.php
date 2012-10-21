<?php
require_once('../connections/db_connection.php');
require_once('../include/auth.php');

mysql_select_db($database, $db);
$query = "SELECT * FROM nodo_tbl_sensor WHERE user_id='$userId' ORDER BY sort_order ASC";
$recordset = mysql_query($query, $db) or die(mysql_error());


        while($row = mysql_fetch_array($recordset))
        {
             $rowsarray[] = array(
            "id" 			=> $row['id'],
			"prefix" 		=> $row['sensor_prefix'],
			"suffix" 		=> $row['sensor_suffix'],
			"suffixtrue" 	=> $row['sensor_suffix_true'],
			"suffixfalse" 	=> $row['sensor_suffix_false'],
			"display"		=> $row['display'],
			"collapsed"		=> $row['collapsed'],
			"type"			=> $row['graph_type'],
			"linecolor"		=> $row['graph_line_color'],
			"inputoutput"	=> $row['input_output'],
			"hours" 		=> $row['graph_hours'],
			"inputcontrol" 	=> $row['input_control'],
			"inputstep"		=> $row['input_step'],
			"inputminval"	=> $row['input_min_val'],
			"inputmaxval"	=> $row['input_max_val'],
			"par1" 			=> $row['par1'],
			"data" 			=> $row['data']);
        }
 
$json = json_encode($rowsarray);

echo '{"values":'. $json .'}'; 
 



?>