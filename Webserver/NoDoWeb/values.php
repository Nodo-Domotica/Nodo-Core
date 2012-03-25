<?php 

require_once('connections/tc.php'); 
require_once('include/auth.php'); 
require_once('include/user_settings.php'); 

$page_title = "Values";

//Tijdzone
date_default_timezone_set('Etc/GMT');

//Grafiek lijnkleur instellen
switch ($theme) {
	
		case "a":
		$graph_line_color = "black";
		break;
		case "b":
		$graph_line_color = "#2065E6"; //blauw
		break;
		case "c":
		$graph_line_color = "black";
		break;
		case "d":
		$graph_line_color = "black";
		break;
		case "e":
		$graph_line_color = "#FFDF0F"; //geel
		break;
}
			

//lees sensoren uit
mysql_select_db($database_tc, $tc);
$query_RSsensor = "SELECT * FROM nodo_tbl_sensor WHERE user_id='$userId' ORDER BY sort_order ASC";
$RSsensor = mysql_query($query_RSsensor, $tc) or die(mysql_error());


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
	
	<!-- NoDoWebapp client side java -->
	<script src="js/get_values.js"></script>
	<!-- /NoDoWebapp client side java -->
	
</head> 

<body> 

<div data-role="page" data-theme="<?php echo $theme?>">

<?php require_once('include/header.php'); ?>
<?php require_once('include/send_event.php'); ?>

	<div data-role="content">	

	
	<?php while($row_RSsensor = mysql_fetch_array($RSsensor)) {
		
			$sensor_id = $row_RSsensor['id'];
			
			//Maximaal aantal uur weergegeven in de grafiek
			if ($row_RSsensor['graph_hours'] == 0) {
				
				$graph_hours = "24";
			}
			else {
				
				$graph_hours = $row_RSsensor['graph_hours'];
			}
			
			
			
			//Lijn grafiek
			if ($row_RSsensor['graph_type'] == 1) {
			
				//X-as minimum ticksize
				switch ($row_RSsensor['graph_min_ticksize']) {
		
					case "1":
					$graph_min_ticksize = "1, \"minute\""; 
					break;
					case "2":
					$graph_min_ticksize = "1, \"hour\"";
					break;
					case "3":
					$graph_min_ticksize = "1, \"day\"";
					break;
					case "4":
					$graph_min_ticksize = "1, \"week\""; 
					break;
					case "5":
					$graph_min_ticksize = "1, \"month\""; 
					break;
				}
								
				$query_RSsensor_value_data = "SELECT * FROM nodo_tbl_sensor_data WHERE user_id='$userId' AND sensor_id='$sensor_id' AND timestamp >= SYSDATE() - INTERVAL $graph_hours HOUR";
				//Data ophalen gebaseerd op keuze ticksize
				$RSsensor_value_data = mysql_query($query_RSsensor_value_data, $tc) or die(mysql_error());	
				
				//Leeg maken omdat er anders onterecht een staaf grafiek weergegeven word indien de voorgaande grafiek een staafgrafiek was
				$graph_bars="";
								
			}
			
				
			//Staaf grafiek
			if ($row_RSsensor['graph_type'] == 2) {
			
			
				switch ($row_RSsensor['graph_min_ticksize']) {
		
					case "1":
					$graph_min_ticksize = "1, \"minute\""; 
					$graph_bar_width = 30000;
					$query_RSsensor_value_data = "SELECT sensor_id,DATE_FORMAT(timestamp , '%Y-%m-%d %H:%i:00') as timestamp , ROUND(SUM(data),2) as data FROM nodo_tbl_sensor_data WHERE user_id='$userId' AND sensor_id='$sensor_id' AND timestamp >= SYSDATE() - INTERVAL $graph_hours HOUR GROUP BY minute(timestamp)";
					break;
					case "2":
					$graph_min_ticksize = "1, \"hour\"";
					$graph_bar_width = 1800000;
					$query_RSsensor_value_data = "SELECT sensor_id,DATE_FORMAT(timestamp , '%Y-%m-%d %H:00') as timestamp , ROUND(SUM(data),2) as data FROM nodo_tbl_sensor_data WHERE user_id='$userId' AND sensor_id='$sensor_id' AND timestamp >= SYSDATE() - INTERVAL $graph_hours HOUR GROUP BY hour(timestamp)";
					break;
					case "3":
					$graph_min_ticksize = "1, \"day\"";
					$graph_bar_width = 43200000;
					$query_RSsensor_value_data = "SELECT sensor_id,DATE_FORMAT(timestamp , '%Y-%m-%d') as timestamp , ROUND(SUM(data),2) as data FROM nodo_tbl_sensor_data WHERE user_id='$userId' AND sensor_id='$sensor_id' AND timestamp >= SYSDATE() - INTERVAL $graph_hours HOUR GROUP BY date(timestamp)";
					break;
					case "4": //Weken zijn lastiger uit te rekenen. Uitzoeken of dit uberhaubt wenselijk is.
					$graph_min_ticksize = "1, \"week\""; 
					$graph_bar_width = 302400000;
					$query_RSsensor_value_data = "SELECT sensor_id,DATE_FORMAT(timestamp , '%Y-%m-%d') as timestamp , ROUND(SUM(data),2) as data FROM nodo_tbl_sensor_data WHERE user_id='$userId' AND sensor_id='$sensor_id' AND timestamp >= SYSDATE() - INTERVAL $graph_hours HOUR GROUP BY week(timestamp)";
					break;
					case "5":
					$graph_min_ticksize = "1, \"month\""; 
					$graph_bar_width = 1314000000;
					$query_RSsensor_value_data = "SELECT sensor_id,DATE_FORMAT(timestamp , '%Y-%m') as timestamp , ROUND(SUM(data),2) as data FROM nodo_tbl_sensor_data WHERE user_id='$userId' AND sensor_id='$sensor_id' AND timestamp >= SYSDATE() - INTERVAL $graph_hours HOUR GROUP BY month(timestamp)";
					break;
				}
				
				
				$graph_bars="bars: {show: true, barWidth:".$graph_bar_width.", align: \"center\"},";
				
				//Totalen ophalen gebaseerd op keuze ticksize
				$RSsensor_value_data = mysql_query($query_RSsensor_value_data, $tc) or die(mysql_error());	
				
				
						
			}			
			
								
			echo "<div data-role=\"collapsible\" data-content-theme=\"c\">";
			
			//Input or Ouput
			if ($row_RSsensor['input_output'] == 1) {echo "<h3>Out: ";}
			if ($row_RSsensor['input_output'] == 2) {echo "<h3>In: ";}
			
			
			//Prefix
			echo $row_RSsensor['sensor_prefix']." ";
			
			
			//Value suffix
			if ($row_RSsensor['display'] == 1){	echo "<span id='value_".$row_RSsensor['id']."'><img src=\"media/loading.gif\" WIDTH=\"15\"></span>"." ".$row_RSsensor['sensor_suffix']."</h3>";}
			
			//State suffix
			if ($row_RSsensor['display'] == 2){
					
					echo "<span id='value_".$row_RSsensor['id']."'><img src=\"media/loading.gif\" WIDTH=\"15\"></span></h3>";
	}
				
			
 
/**********************************************************************************************************************************************************************************************
Grafiek: Values
**********************************************************************************************************************************************************************************************/				

			
		//Alleen een grafiek weergeven als het een output vanuit de nodo richting webapp betreft en het een waarde betreft.
		if ($row_RSsensor['input_output'] == 2 && $row_RSsensor['display'] == 1){
		?>
			<br>
			<div id="<?php echo $row_RSsensor['id']; ?>" style="width:100%;height:300px;"></div>
			<br>
			<script type="text/javascript">
										
					var value_data = [
					
					//Grafiek
					<?php 
						
						//lees maximaal X uur aan value data uit
						mysql_select_db($database_tc, $tc);
						
											
												
						while($row_RSsensor_value_data = mysql_fetch_array($RSsensor_value_data))	{
													
							echo "[".(strtotime($row_RSsensor_value_data['timestamp'])*1000).",".$row_RSsensor_value_data['data']."],";
													
						}
						
					
							echo "];";						
					?>
											
					
					$.plot($("#<?php echo $row_RSsensor['id']; ?>"),[{data: value_data, <?php echo $graph_bars;?> label: "<?php echo $row_RSsensor['sensor_suffix']; ?>",color: "<?php echo $graph_line_color; ?>"}], { xaxis: { mode: "time",minTickSize: [<?php echo $graph_min_ticksize;?>]}});


					

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
				<a href="javascript:send_event(&quot;variableinc <?php echo $row_RSsensor['par1'];?>,<?php echo $row_RSsensor['input_step'];?>&quot;)" data-role="button">+</a>
				<a href="javascript:send_event(&quot;variabledec <?php echo $row_RSsensor['par1'];?>,<?php echo $row_RSsensor['input_step'];?>&quot;)" data-role="button">-</a>		
			<?php
			}
			
			//Slider
			if ($row_RSsensor['input_control'] == 2){
			?>
			
			<script>
				var t<?php echo $row_RSsensor['id']; ?>;
				function update_distance_timer_<?php echo $row_RSsensor['id']; ?>()	{
					clearTimeout(t<?php echo $row_RSsensor['id']; ?>);
					t<?php echo $row_RSsensor['id']; ?>=setTimeout("update_distance_<?php echo $row_RSsensor['id']; ?>()",1000);
					}
				function update_distance_<?php echo $row_RSsensor['id']; ?>() {
					var val<?php echo $row_RSsensor['id']; ?> = $('#distSlider<?php echo $row_RSsensor['id']; ?>').val();
					//alert(val);
					send_event('variableset <?php echo $row_RSsensor['par1']; ?>,' + val<?php echo $row_RSsensor['id'];?>)
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
<script>
//Eerste maal de functie Get_Value opstarten zodat de loop gaat lopen welke de waarde elke x seconde ververst. 
Get_Values();
</script>
	

</div><!-- /content -->
<?php require_once('include/footer.php'); ?>
	
</div><!-- /page -->

</body>
</html>