<?php 
require_once('connections/tc.php'); 
require_once('include/auth.php'); 
require_once('include/settings.php'); 

$page_title = "Values";

//lees sensoren uit
mysql_select_db($database_tc, $tc);
$query_RSsensor = "SELECT * FROM nodo_tbl_sensor WHERE user_id='$userId'";
$RSsensor = mysql_query($query_RSsensor, $tc) or die(mysql_error());

//lees maximaal 24 uur aan value data uit
mysql_select_db($database_tc, $tc);
$query_RSvalue_data = "SELECT * FROM nodo_tbl_sensor_data WHERE user_id='$userId' AND timestamp >= SYSDATE() - INTERVAL 1 DAY";
$RSvalue_data = mysql_query($query_RSvalue_data, $tc) or die(mysql_error());

//Tijdzone nog uitzoeken...
date_default_timezone_set('Etc/GMT');

?>



<!DOCTYPE html> 
<html> 

<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1"> 
	<title><?php echo $title ?></title> 
	<?php require_once('include/jquery_mobile.php'); ?>
	 <!--[if lte IE 8]><script language="javascript" type="text/javascript" src="../excanvas.min.js"></script><![endif]-->
    <script language="javascript" type="text/javascript" src="js/flot/jquery.js"></script>
    <script language="javascript" type="text/javascript" src="js/flot/jquery.flot.js"></script>
	<script language="javascript" type="text/javascript" src="js/flot/jquery.flot.resize.js"></script>
	
</head> 

<body> 

<div data-role="page" data-theme="<?php echo $theme?>">

<?php require_once('include/header.php'); ?>
<?php require_once('include/send_event.php'); ?>

	<div data-role="content">	

	
	<?php while($row_RSsensor = mysql_fetch_array($RSsensor)) {
		
		
		echo "<div data-role=\"collapsible\" data-iconshadow=\"true\" data-content-theme=\"c\">";
		//Prefix
		echo "<h3>".$row_RSsensor['sensor_prefix']." ";
		
		
		//Value suffix
		if ($row_RSsensor['display'] == 1){	echo $row_RSsensor['data']." ".$row_RSsensor['sensor_suffix']."</h3>";}
		
		//State suffix
		if ($row_RSsensor['display'] == 2){
				if ($row_RSsensor['data'] <=0){	echo $row_RSsensor['sensor_suffix_false']."</h3>";}
				if ($row_RSsensor['data'] > 0){	echo $row_RSsensor['sensor_suffix_true']."</h3>"; }
		}
				
				//Controleren of er log data bestaat. Zoniet dan hoeven we niet terug naar het eerste record.
				if ( mysql_num_rows ( $RSvalue_data ) > 0 ) {

					mysql_data_seek($RSvalue_data, 0);
				
				}
 
/**********************************************************************************************************************************************************************************************
Grafiek: Values
**********************************************************************************************************************************************************************************************/				

			
		//Alleen een grafiek weergeven als het een output vanuit de nodo richting webapp betreft
		if ($row_RSsensor['input_output'] == 2){
		?>
			<br>
			<div id="<?php echo $row_RSsensor['id']; ?>" style="width:100%;height:300px;"></div>
			<br>
			<script type="text/javascript">
										
					var value_data = [
					
					//Grafiek
					<?php while($row_RSvalue_data = mysql_fetch_array($RSvalue_data)){
						
						
						if ($row_RSsensor['sensor_type'] == $row_RSvalue_data['type'] && $row_RSsensor['nodo_unit_nr'] == $row_RSvalue_data['nodo_unit_nr'] && $row_RSsensor['par1'] == $row_RSvalue_data['par1'])
						{	
													
							echo "[".(strtotime($row_RSvalue_data['timestamp'])*1000).",".$row_RSvalue_data['data']."],";
													
						}
						
					}
					echo "];";						
					?>
											
					
					$.plot($("#<?php echo $row_RSsensor['id']; ?>"),[{data: value_data, label: "<?php echo $row_RSsensor['sensor_suffix']; ?>"}], { xaxis: { mode: "time"} });



				</script>

		<?php }
/**********************************************************************************************************************************************************************************************
/Grafiek: Values
**********************************************************************************************************************************************************************************************/				

/**********************************************************************************************************************************************************************************************
Input control
**********************************************************************************************************************************************************************************************/									
			//+- Buttons
			if ($row_RSsensor['input_control'] == 1){
			?>
				<a href="javascript:send_event(&quot;variableinc <?php echo $row_RSsensor['par1'];?>,<?php echo $row_RSsensor['input_step'];?>&quot;)" data-role="button" data-inline="true">+</a>
				<a href="javascript:send_event(&quot;variabledec <?php echo $row_RSsensor['par1'];?>,<?php echo $row_RSsensor['input_step'];?>&quot;)" data-role="button" data-inline="true">-</a>		
			<?php
			}
			
			//Slider
			if ($row_RSsensor['input_control'] == 2){
			?>
			
			<script>
				var t<?php echo $row_RSsensor['id']; ?>;
				function update_distance_timer_<?php echo $row_RSsensor['id']; ?>()	{
					clearTimeout(t<?php echo $row_RSsensor['id']; ?>);
					t<?php echo $row_RSsensor['id']; ?>=setTimeout("update_distance_<?php echo $row_RSsensor['id']; ?>()",200);
					}
				function update_distance_<?php echo $row_RSsensor['id']; ?>() {
					var val<?php echo $row_RSsensor['id']; ?> = $('#distSlider<?php echo $row_RSsensor['id']; ?>').val();
					//alert(val);
					send_event('variableset <?php echo $row_RSsensor['par1']; ?>,' + val<?php echo $row_RSsensor['id'];?> + ';VariableEvent,' + <?php echo $row_RSsensor['par1'];?> )
					}
			</script>
		
		
			<label  id="distSlider-label" for="distSlider">Set value: </label>
			<input  name="distSlider" id="distSlider<?php echo $row_RSsensor['id']; ?>" value="<?php echo $row_RSsensor['data'];?>" min="<?php echo $row_RSsensor['input_min_val'];?>" max="<?php echo $row_RSsensor['input_max_val'];?>" step="<?php echo $row_RSsensor['input_step'];?>" data-type="range" onChange='update_distance_timer_<?php echo $row_RSsensor['id']; ?>()'>
		
		<?php }
		
		echo "</div>";	
	}
	
/**********************************************************************************************************************************************************************************************
/Input control
**********************************************************************************************************************************************************************************************/		
?>

	

</div><!-- /content -->
<?php require_once('include/footer.php'); ?>
	
</div><!-- /page -->

</body>
</html>