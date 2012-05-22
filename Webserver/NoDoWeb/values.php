<?php

/***********************************************************************************************************************
"Nodo Web App" Copyright © 2012 Martin de Graaf

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*************************************************************************************************************************/

require_once('connections/db_connection.php');
require_once('include/auth.php');
require_once('include/user_settings.php');

$page_title = "Values";


//Grafiek lijnkleur instellen
switch ($theme) {
	
		case "a":
		$graph_line_color = "#808080"; //grijs
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
mysql_select_db($database, $db);
$query_RSsensor = "SELECT * FROM nodo_tbl_sensor WHERE user_id='$userId' ORDER BY sort_order ASC";
$RSsensor = mysql_query($query_RSsensor, $db) or die(mysql_error());


?>


<!DOCTYPE html> 
<html> 

<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1"> 
	<title><?php echo $title ?></title> 
	<?php require_once('include/jquery_mobile.php'); ?>
	
	
</head> 

<body> 

<div data-role="page" data-theme="<?php echo $theme?>">

 <!--[if lte IE 8]><script language="javascript" type="text/javascript" src="../excanvas.min.js"></script><![endif]-->
    <script language="javascript" type="text/javascript" src="js/flot/jquery.js"></script>
    <script language="javascript" type="text/javascript" src="js/flot/jquery.flot.js"></script>
	<script language="javascript" type="text/javascript" src="js/flot/jquery.flot.resize.js"></script>
	
	<!-- NoDoWebapp client side java -->
	<script src="js/get_values.js"></script>
	<!-- /NoDoWebapp client side java -->

<?php require_once('include/header.php'); ?>
<?php require_once('include/send_event.php'); ?>

<script>

function test(){
alert ('test');
}

function Get_Graph_data(hours,sensor_id,label,bars,ticksize,date1,date2,filter)
 {  
     
			var options = {
						xaxis: { mode: "time",minTickSize: [1,ticksize]}
					};
			
		
		element = document.getElementById(sensor_id);
					
		element.innerHTML = '<h4><img src="media/loading.gif"/> Please wait, loading graph...</h4>'; 

					
		$.getJSON("include/get_graph_data.php?hours=" + hours +"&sensor_id="+sensor_id +"&bars="+bars +"&date1="+date1 +"&date2="+date2+"&filter="+filter, function(graph_data) {
       //succes - data loaded, now use plot:
       
	   if (graph_data != null) {
	   
	   var plotarea = $("#" + sensor_id);
       
       if (bars != 1)
		   {
		   
		   
		   
		   $.plot(plotarea , [
					 { label: label, data: graph_data, color: "<?php echo $graph_line_color; ?>"  }
					 ],options );
		   }
		else
		   {
		   
		   
		   
		   $.plot(plotarea , [
					 { label: label, data: graph_data, color: "<?php echo $graph_line_color; ?>" ,bars: {show: true, barWidth:43200000, align: "center"} }
					 ],options );
		   }
		   
		}
		
		else {
		
		
			element.innerHTML = '<h4>No data available......</h4>'; 
			
		}
	   
	
    });
}

function Historic_bar_day_totals(id,label,ticksize)

{
	day1 = $("#select-choice-day1-"+id).val();
	day2 = $("#select-choice-day2-"+id).val();
	month1 = $("#select-choice-month1-"+id).val();
	month2 = $("#select-choice-month2-"+id).val();
	year1 = $("#select-choice-year1-"+id).val();
	year2 = $("#select-choice-year2-"+id).val();

	date1 = year1 + "-" + month1 + "-" + day1;
	date2 = year2 + "-" + month2 + "-" + day2;

	Get_Graph_data(0,id,label,1,ticksize,date1,date2,1);
	//alert (date1);
}

function Historic_line(id,label,ticksize)

{
	hour1 = $("#select-choice-hour1-"+id).val();
	hour2 = $("#select-choice-hour2-"+id).val();
	day1 = $("#select-choice-day1-"+id).val();
	day2 = $("#select-choice-day2-"+id).val();
	month1 = $("#select-choice-month1-"+id).val();
	month2 = $("#select-choice-month2-"+id).val();
	year1 = $("#select-choice-year1-"+id).val();
	year2 = $("#select-choice-year2-"+id).val();

	date1 = year1 + "-" + month1 + "-" + day1 + " " + hour1 + ":00";
	date2 = year2 + "-" + month2 + "-" + day2 + " " + hour2 + ":00";

	Get_Graph_data(0,id,label,0,ticksize,date1,date2,2);
	//alert (date1);
}



</script>

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
					$graph_min_ticksize = "minute"; 
					break;
					case "2":
					$graph_min_ticksize = "hour";
					break;
					case "3":
					$graph_min_ticksize = "day";
					break;
					case "4":
					$graph_min_ticksize = "week"; 
					break;
					case "5":
					$graph_min_ticksize = "month"; 
					break;
				}
								
				//$query_RSsensor_value_data = "SELECT data,timestamp FROM nodo_tbl_sensor_data WHERE sensor_id='$sensor_id' AND timestamp >= SYSDATE() - INTERVAL $graph_hours HOUR";
				//Data ophalen gebaseerd op keuze ticksize
				//$RSsensor_value_data = mysql_query($query_RSsensor_value_data, $db) or die(mysql_error());	
				
				//Leeg maken omdat er anders onterecht een staaf grafiek weergegeven word indien de voorgaande grafiek een staafgrafiek was
				$graph_bars="0";
								
			}
			
				
			//Staaf grafiek
			if ($row_RSsensor['graph_type'] == 2) {
			
				$graph_min_ticksize = "day";
					$graph_bar_width = 43200000;
					//$query_RSsensor_value_data = "SELECT DATE_FORMAT(timestamp , '%Y-%m-%d') as timestamp , ROUND(SUM(data),2) as data FROM nodo_tbl_sensor_data WHERE sensor_id='$sensor_id' AND timestamp >= SYSDATE() - INTERVAL $graph_hours HOUR GROUP BY date(timestamp)";
			
				$graph_bars="1";
				
				//Totalen ophalen gebaseerd op keuze ticksize
				//$RSsensor_value_data = mysql_query($query_RSsensor_value_data, $db) or die(mysql_error());	
				
				
						
			}			
			
								
			echo "<div data-role=\"collapsible\" id=\"collapsible" . $row_RSsensor['id'] . "\" data-content-theme=\"" . $theme ."\">";
			
			//Input or Ouput
			if ($row_RSsensor['input_output'] == 1) {echo "<h3> In: ";}
			if ($row_RSsensor['input_output'] == 2) {echo "<h3 onclick=\"Get_Graph_data(".$graph_hours. "," .$row_RSsensor['id']. ",'" . $row_RSsensor['sensor_suffix']."',".$graph_bars.",'".$graph_min_ticksize. "')\">Out: ";}
			
			
			//Prefix
			echo $row_RSsensor['sensor_prefix']." ";
			
			
			//Value suffix
			if ($row_RSsensor['display'] == 1){	echo "<span id='value_".$row_RSsensor['id']."'><img src=\"media/loading.gif\" WIDTH=\"15\"></span>"." ".$row_RSsensor['sensor_suffix']."</h3>";}
			
			//State suffix
			if ($row_RSsensor['display'] == 2){
					
					echo "<span id='value_".$row_RSsensor['id']."'><img src=\"media/loading.gif\" WIDTH=\"15\"></span></h3>";
	}
				
			
 

		//Grafiek: Values
			
		//Alleen een grafiek weergeven als het een output vanuit de nodo richting webapp betreft en het een waarde betreft.
		if ($row_RSsensor['input_output'] == 2 && $row_RSsensor['display'] == 1){
		?>
			<br>
			<div id="<?php echo $row_RSsensor['id']; ?>" style="width:100%;height:300px;"></div>
			<br>
			<script type="text/javascript">
			//Get_Graph_data(<?php echo $graph_hours; ?>,<?php echo $row_RSsensor['id']; ?>,'<?php echo $row_RSsensor['sensor_suffix']; ?>',<?php echo $graph_bars; ?>,'<?php echo $graph_min_ticksize; ?>')
			</script>
			
				
			
			<?php if ($graph_bars != 1) { ?>	
				<div data-role="collapsible" data-content-theme="<?php echo $theme; ?>" data-mini="true">
			<h3>Historic data</h3>

			<fieldset data-role="controlgroup" data-type="horizontal" data-mini="true">
				<legend>Show data from:</legend>

				<label for="select-choice-month1-<?php echo $row_RSsensor['id']; ?>">Month</label>
					<select name="select-choice-month1-<?php echo $row_RSsensor['id']; ?>" id="select-choice-month1-<?php echo $row_RSsensor['id']; ?>">
						<option>Month</option>
						<option value="1">January</option>
						<option value="2">February</option>
						<option value="3">March</option>
						<option value="4">April</option>
						<option value="5">May</option>
						<option value="6">June</option>
						<option value="7">July</option>
						<option value="8">August</option>
						<option value="9">September</option>
						<option value="10">October</option>
						<option value="11">November</option>
						<option value="12">December</option>
						
					</select>

				<label for="select-choice-day1-<?php echo $row_RSsensor['id']; ?>">Day</label>
					<select name="select-choice-day1-<?php echo $row_RSsensor['id']; ?>" id="select-choice-day1-<?php echo $row_RSsensor['id']; ?>">
						<option>Day</option>
						<option value="01">1</option>
						<option value="02">2</option>
						<option value="03">3</option>
						<option value="04">4</option>
						<option value="05">5</option>
						<option value="06">6</option>
						<option value="07">7</option>
						<option value="08">8</option>
						<option value="09">9</option>
						<option value="10">10</option>
						<option value="11">11</option>
						<option value="12">12</option>
						<option value="13">13</option>
						<option value="14">14</option>
						<option value="15">15</option>
						<option value="16">16</option>
						<option value="17">17</option>
						<option value="18">18</option>
						<option value="19">19</option>
						<option value="20">20</option>
						<option value="21">21</option>
						<option value="22">22</option>
						<option value="23">23</option>
						<option value="24">24</option>
						<option value="25">25</option>
						<option value="26">26</option>
						<option value="27">27</option>
						<option value="28">28</option>
						<option value="29">29</option>
						<option value="30">30</option>
						<option value="31">31</option>
														
					</select>
					
					<label for="select-choice-hour1-<?php echo $row_RSsensor['id']; ?>">Hour</label>
					<select name="select-choice-hour1-<?php echo $row_RSsensor['id']; ?>" id="select-choice-hour1-<?php echo $row_RSsensor['id']; ?>">
						<option>Hour</option>
						<?php
																		
						for ( $x = 0; $x <= 23; $x ++) { 
						
						echo "<option value=\"$x\">$x:00</option>";
						
						}?>
																				
					</select>

				<label for="select-choice-year1-<?php echo $row_RSsensor['id']; ?>">Year</label>
					<select name="select-choice-year1-<?php echo $row_RSsensor['id']; ?>" id="select-choice-year1-<?php echo $row_RSsensor['id']; ?>">
						<option value="2012">2012</option>
						
					</select>
			</fieldset>
			
			<fieldset data-role="controlgroup" data-type="horizontal" data-mini="true" >
				<legend>To:</legend>

				<label for="select-choice-month1-<?php echo $row_RSsensor['id']; ?>">Month</label>
					<select name="select-choice-month1-<?php echo $row_RSsensor['id']; ?>" id="select-choice-month2-<?php echo $row_RSsensor['id']; ?>" >
						<option>Month</option>
						<option value="1">January</option>
						<option value="2">February</option>
						<option value="3">March</option>
						<option value="4">April</option>
						<option value="5">May</option>
						<option value="6">June</option>
						<option value="7">July</option>
						<option value="8">August</option>
						<option value="9">September</option>
						<option value="10">October</option>
						<option value="11">November</option>
						<option value="12">December</option>
					</select>

				<label for="select-choice-day2-<?php echo $row_RSsensor['id']; ?>">Day</label>
					<select name="select-choice-day2-<?php echo $row_RSsensor['id']; ?>" id="select-choice-day2-<?php echo $row_RSsensor['id']; ?>">
						<option>Day</option>
						<option value="01">1</option>
						<option value="02">2</option>
						<option value="03">3</option>
						<option value="04">4</option>
						<option value="05">5</option>
						<option value="06">6</option>
						<option value="07">7</option>
						<option value="08">8</option>
						<option value="09">9</option>
						<option value="10">10</option>
						<option value="11">11</option>
						<option value="12">12</option>
						<option value="13">13</option>
						<option value="14">14</option>
						<option value="15">15</option>
						<option value="16">16</option>
						<option value="17">17</option>
						<option value="18">18</option>
						<option value="19">19</option>
						<option value="20">20</option>
						<option value="21">21</option>
						<option value="22">22</option>
						<option value="23">23</option>
						<option value="24">24</option>
						<option value="25">25</option>
						<option value="26">26</option>
						<option value="27">27</option>
						<option value="28">28</option>
						<option value="29">29</option>
						<option value="30">30</option>
						<option value="31">31</option>
						
					</select>
					
					<label for="select-choice-hour2-<?php echo $row_RSsensor['id']; ?>">Hour</label>
					<select name="select-choice-hour2-<?php echo $row_RSsensor['id']; ?>" id="select-choice-hour2-<?php echo $row_RSsensor['id']; ?>">
						<option>Hour</option>
						<?php
																		
						for ( $x = 0; $x <= 23; $x ++) { 
						
						echo "<option value=\"$x\">$x:00</option>";
						
						}?>
																				
					</select>

				<label for="select-choice-year2-<?php echo $row_RSsensor['id']; ?>">Year</label>
					<select name="select-choice-year2-<?php echo $row_RSsensor['id']; ?>" id="select-choice-year2-<?php echo $row_RSsensor['id']; ?>">
						<option value="2012">2012</option>
						
					</select>
			</fieldset>
			<a href="javascript:Historic_line(<?php echo $row_RSsensor['id']; ?>,'<?php echo $row_RSsensor['sensor_suffix']; ?>','<?php echo $graph_min_ticksize; ?>')" data-role="button" data-inline="true" data-mini="true">Get data</a>
			</div> 
				

			<?php } ?>
			<?php if ($graph_bars == 1) { ?>
			
			<div data-role="collapsible" data-content-theme="<?php echo $theme; ?>" data-mini="true">
			<h3>Historic data</h3>

			<fieldset data-role="controlgroup" data-type="horizontal" data-mini="true">
				<legend>Show data from:</legend>

				<label for="select-choice-month1-<?php echo $row_RSsensor['id']; ?>">Month</label>
					<select name="select-choice-month1-<?php echo $row_RSsensor['id']; ?>" id="select-choice-month1-<?php echo $row_RSsensor['id']; ?>">
						<option>Month</option>
						<option value="1">January</option>
						<option value="2">February</option>
						<option value="3">March</option>
						<option value="4">April</option>
						<option value="5">May</option>
						<option value="6">June</option>
						<option value="7">July</option>
						<option value="8">August</option>
						<option value="9">September</option>
						<option value="10">October</option>
						<option value="11">November</option>
						<option value="12">December</option>
						
					</select>

				<label for="select-choice-day1-<?php echo $row_RSsensor['id']; ?>">Day</label>
					<select name="select-choice-day1-<?php echo $row_RSsensor['id']; ?>" id="select-choice-day1-<?php echo $row_RSsensor['id']; ?>">
						<option>Day</option>
						<option value="01">1</option>
						<option value="02">2</option>
						<option value="03">3</option>
						<option value="04">4</option>
						<option value="05">5</option>
						<option value="06">6</option>
						<option value="07">7</option>
						<option value="08">8</option>
						<option value="09">9</option>
						<option value="10">10</option>
						<option value="11">11</option>
						<option value="12">12</option>
						<option value="13">13</option>
						<option value="14">14</option>
						<option value="15">15</option>
						<option value="16">16</option>
						<option value="17">17</option>
						<option value="18">18</option>
						<option value="19">19</option>
						<option value="20">20</option>
						<option value="21">21</option>
						<option value="22">22</option>
						<option value="23">23</option>
						<option value="24">24</option>
						<option value="25">25</option>
						<option value="26">26</option>
						<option value="27">27</option>
						<option value="28">28</option>
						<option value="29">29</option>
						<option value="30">30</option>
						<option value="31">31</option>
						
						
						
					</select>

				<label for="select-choice-year1-<?php echo $row_RSsensor['id']; ?>">Year</label>
					<select name="select-choice-year1-<?php echo $row_RSsensor['id']; ?>" id="select-choice-year1-<?php echo $row_RSsensor['id']; ?>">
						<option value="2012">2012</option>
						
					</select>
			</fieldset>
			
			<fieldset data-role="controlgroup" data-type="horizontal" data-mini="true" >
				<legend>To:</legend>

				<label for="select-choice-month2-<?php echo $row_RSsensor['id']; ?>">Month</label>
					<select name="select-choice-month2-<?php echo $row_RSsensor['id']; ?>" id="select-choice-month2-<?php echo $row_RSsensor['id']; ?>" >
						<option>Month</option>
						<option value="1">January</option>
						<option value="2">February</option>
						<option value="3">March</option>
						<option value="4">April</option>
						<option value="5">May</option>
						<option value="6">June</option>
						<option value="7">July</option>
						<option value="8">August</option>
						<option value="9">September</option>
						<option value="10">October</option>
						<option value="11">November</option>
						<option value="12">December</option>
					</select>

				<label for="select-choice-day2-<?php echo $row_RSsensor['id']; ?>">Day</label>
					<select name="select-choice-day2-<?php echo $row_RSsensor['id']; ?>" id="select-choice-day2-<?php echo $row_RSsensor['id']; ?>">
						<option>Day</option>
						<option value="01">1</option>
						<option value="02">2</option>
						<option value="03">3</option>
						<option value="04">4</option>
						<option value="05">5</option>
						<option value="06">6</option>
						<option value="07">7</option>
						<option value="08">8</option>
						<option value="09">9</option>
						<option value="10">10</option>
						<option value="11">11</option>
						<option value="12">12</option>
						<option value="13">13</option>
						<option value="14">14</option>
						<option value="15">15</option>
						<option value="16">16</option>
						<option value="17">17</option>
						<option value="18">18</option>
						<option value="19">19</option>
						<option value="20">20</option>
						<option value="21">21</option>
						<option value="22">22</option>
						<option value="23">23</option>
						<option value="24">24</option>
						<option value="25">25</option>
						<option value="26">26</option>
						<option value="27">27</option>
						<option value="28">28</option>
						<option value="29">29</option>
						<option value="30">30</option>
						<option value="31">31</option>
						
					</select>

				<label for="select-choice-year2-<?php echo $row_RSsensor['id']; ?>">Year</label>
					<select name="select-choice-year2-<?php echo $row_RSsensor['id']; ?>" id="select-choice-year2-<?php echo $row_RSsensor['id']; ?>">
						<option value="2012">2012</option>
						
					</select>
			</fieldset>
			<a href="javascript:Historic_bar_day_totals(<?php echo $row_RSsensor['id']; ?>,'<?php echo $row_RSsensor['sensor_suffix']; ?>','<?php echo $graph_min_ticksize; ?>')" data-role="button" data-inline="true" data-mini="true">Get data</a>
			</div> 


			
			<?php } ?>
				
		<?php }
				


			//Input control
									
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
		
			<?php 
			
			if ($row_RSsensor['input_min_val'] == 0 || $row_RSsensor['input_max_val'] == 1) {
			
			?>
			
			<select name="distSlider" id="distSlider<?php echo $row_RSsensor['id']; ?>" data-role="slider" onChange='update_distance_timer_<?php echo $row_RSsensor['id']; ?>()'>
			<option value="0"<?php if ($row_RSsensor['data'] == 0) {echo 'selected="selected"';}?>><?php echo $row_RSsensor['sensor_suffix_false']; ?></option>
			<option value="1"<?php if ($row_RSsensor['data'] == 1) {echo 'selected="selected"';}?>><?php echo $row_RSsensor['sensor_suffix_true']; ?></option>
			</select> 
			
			<?php }
			
			else
			
			{
			
			?>
			
			
			
			<label  id="distSlider-label" for="distSlider">Set value: </label>
			<input  name="distSlider" id="distSlider<?php echo $row_RSsensor['id']; ?>" value="<?php echo $row_RSsensor['data'];?>" min="<?php echo $row_RSsensor['input_min_val'];?>" max="<?php echo $row_RSsensor['input_max_val'];?>" step="<?php echo $row_RSsensor['input_step'];?>" data-type="range" onChange='update_distance_timer_<?php echo $row_RSsensor['id']; ?>()'>
		
		<?php }
		
		}
		
		echo "</div>";	
		?>
		

<?php
		
	}
	
	
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