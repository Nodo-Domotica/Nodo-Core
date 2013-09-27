<?php
require_once('../connections/db_connection.php');
require_once('../include/auth.php');

mysql_select_db($database, $db);
$query = "SELECT * FROM nodo_tbl_sensor WHERE user_id='$userId' ORDER BY sort_order ASC";
$recordset = mysql_query($query, $db) or die(mysql_error());

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



        while($row = mysql_fetch_assoc($recordset))
        {
            
			
			$formula = $row['formula'];
			if ($formula != '') {
				
				$formula = str_ireplace("%value%",$row['data'],$formula);
				$data = round(evalmath($formula),$row['round']);
			}
			else {
				$data = $row['data'];
			}

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
			"data" 			=> $data);
        }
 
$json = json_encode($rowsarray);

echo '{"values":'. $json .'}'; 
 



?>