<?php 
//Haal de schakelaar status op
require_once('connections/tc.php');
require_once('include/auth.php');
?>
<?php 
mysql_select_db($database_tc, $tc);
$query_RSdevice = "SELECT id,user_id,status,dim_value FROM nodo_tbl_devices WHERE user_id='$userId'";
$RSdevice = mysql_query($query_RSdevice, $tc) or die(mysql_error());
$row_RSdevice = mysql_fetch_assoc($RSdevice);
$totalRows_RSdevice = mysql_num_rows($RSdevice);
?>
<?php if ($row_RSdevice != NULL){
do { ?>document.getElementById('switch_<?php echo $row_RSdevice['id'];?>').innerHTML = <?php 
   
   switch ($row_RSdevice['status'])
   
   {
   
	case 0:
		echo "'<IMG SRC=\"media/off.png\" WIDTH=20 ALIGN=right alt=\"UIT\">';";
	break;
   
	case 1:
	
		switch ($row_RSdevice['dim_value'])
   
		{
			//Als er geen dimwaarde bestaat dan gaan we er van uit dat er niet gedimd wordt.
			case 0:
				echo "'<IMG SRC=\"media/on.png\" WIDTH=20 ALIGN=right>';";
				break;
			   
			//Status icoon afhankelijk van het dim-niveau
			case 1:
				echo "'<IMG SRC=\"media/on_dim1.png\" WIDTH=20 ALIGN=right>';";
				break;
			case 2:
				echo "'<IMG SRC=\"media/on_dim2.png\" WIDTH=20 ALIGN=right>';";
				break;
			case 3:
				echo "'<IMG SRC=\"media/on_dim3.png\" WIDTH=20 ALIGN=right>';";
				break;
			case 4:
				echo "'<IMG SRC=\"media/on_dim4.png\" WIDTH=20 ALIGN=right>';";
				break;
			case 5:
				echo "'<IMG SRC=\"media/on_dim5.png\" WIDTH=20 ALIGN=right>';";
				break;
			case 6:
				echo "'<IMG SRC=\"media/on_dim6.png\" WIDTH=20 ALIGN=right>';";
				break;
			case 7:
				echo "'<IMG SRC=\"media/on_dim7.png\" WIDTH=20 ALIGN=right>';";
				break;
			case 8:
				echo "'<IMG SRC=\"media/on_dim8.png\" WIDTH=20 ALIGN=right>';";
				break;
			case 9:
				echo "'<IMG SRC=\"media/on_dim9.png\" WIDTH=20 ALIGN=right>';";
				break;
			case 10:
				echo "'<IMG SRC=\"media/on_dim10.png\" WIDTH=20 ALIGN=right>';";
				break;
			case 11:
				echo "'<IMG SRC=\"media/on_dim11.png\" WIDTH=20 ALIGN=right>';";
				break;
			case 12:
				echo "'<IMG SRC=\"media/on_dim12.png\" WIDTH=20 ALIGN=right>';";
				break;
			case 13:
				echo "'<IMG SRC=\"media/on_dim13.png\" WIDTH=20 ALIGN=right>';";
				break;
			case 14:
				echo "'<IMG SRC=\"media/on_dim14.png\" WIDTH=20 ALIGN=right>';";
				break;
			case 15:
				echo "'<IMG SRC=\"media/on_dim15.png\" WIDTH=20 ALIGN=right>';";
				break;
			case 16:
				echo "'<IMG SRC=\"media/on_dim16.png\" WIDTH=20 ALIGN=right>';";
				break;
		}
	
   }
 
   
 	
	
	?>
<?php } while ($row_RSdevice = mysql_fetch_assoc($RSdevice)); }?>
<?php
mysql_free_result($RSdevice);

?>
