<?php 
//Haal de schakelaar status op
require_once('connections/tc.php');
require_once('include/auth.php');
?>
<?php 
mysql_select_db($database_tc, $tc);
$query_RSvalue = "SELECT * FROM nodo_tbl_sensor WHERE user_id='$userId'";
$RSvalue = mysql_query($query_RSvalue, $tc) or die(mysql_error());
$row_RSvalue = mysql_fetch_assoc($RSvalue);
//$totalRows_RSvalue = mysql_num_rows($RSvalue);
?>
<?php if ($row_RSvalue != NULL){
do { ?>document.getElementById('value_<?php echo $row_RSvalue['id'];?>').innerHTML = <?php 
   
   if ($row_RSvalue['display'] == 1){
   
		echo "'".$row_RSvalue['data']."';";
   
   }
   
   if ($row_RSvalue['display'] == 2){
	
		if ($row_RSvalue['data'] <= 0){
		
			echo "'".$row_RSvalue['sensor_suffix_false']."';";
		}
		
		if ($row_RSvalue['data'] > 0){
		
			echo "'".$row_RSvalue['sensor_suffix_true']."';";
		}
		
		
   }
   
   

  
   
 
   
 	
	
	?>
<?php } while ($row_RSvalue = mysql_fetch_assoc($RSvalue)); }?>
<?php
mysql_free_result($RSvalue);

?>
