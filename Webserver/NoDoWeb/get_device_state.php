<?php 
//Haal de schakelaar status op
require_once('connections/tc.php');
require_once('include/auth.php');
?>
<?php 
mysql_select_db($database_tc, $tc);
$query_RSdevice = "SELECT * FROM nodo_tbl_devices WHERE user_id='$userId'";
$RSdevice = mysql_query($query_RSdevice, $tc) or die(mysql_error());
$row_RSdevice = mysql_fetch_assoc($RSdevice);
$totalRows_RSdevice = mysql_num_rows($RSdevice);
?>
<?php if ($row_RSdevice != NULL){
do { ?>document.getElementById('switch_<?php echo $row_RSdevice['id'];?>').innerHTML = <?php 
   if ($row_RSdevice['status'] == 0){ echo "'<IMG SRC=\"media/off.png\" WIDTH=20 ALIGN=right>';"; } 
		else {echo "'<IMG SRC=\"media/on.png\" WIDTH=20 ALIGN=right>';";}
	?>
<?php } while ($row_RSdevice = mysql_fetch_assoc($RSdevice)); }?>
<?php
mysql_free_result($RSdevice);

?>
