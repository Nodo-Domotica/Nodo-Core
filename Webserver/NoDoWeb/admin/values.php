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

require_once('../connections/db_connection.php'); 
require_once('../include/auth.php');
require_once('../include/user_settings.php');

$page_title = "Setup: Values";	  


/*************************************************************************************************************************************************************
POST gegevens wegschrijven naar de database
**************************************************************************************************************************************************************/
if (isset($_POST['submit'])) 
{  
 

 $prefix= mysql_real_escape_string(htmlspecialchars($_POST['prefix'])); 
 $unit = mysql_real_escape_string(htmlspecialchars($_POST['unit'])); 
 $par1 = mysql_real_escape_string(htmlspecialchars($_POST['par1'])); 
 $type = mysql_real_escape_string(htmlspecialchars($_POST['type']));
 $display = mysql_real_escape_string(htmlspecialchars($_POST['display']));
 $collapsed = mysql_real_escape_string(htmlspecialchars($_POST['collapsed']));
 $input_output = mysql_real_escape_string(htmlspecialchars($_POST['input_output']));
 $input_control = mysql_real_escape_string(htmlspecialchars($_POST['input_control']));
 $input_slider_min = mysql_real_escape_string(htmlspecialchars($_POST['input_slider_min_val']));
 $input_slider_max = mysql_real_escape_string(htmlspecialchars($_POST['input_slider_max_val']));
 $input_step = mysql_real_escape_string(htmlspecialchars($_POST['input_step_val']));
 $input_step = str_replace(",", ".", $input_step); //Als een gebruiker 0,5 invoerd slaan we de data op als 0.5
 $graph_hours = mysql_real_escape_string(htmlspecialchars($_POST['graph_hours']));
 $graph_min_ticksize = mysql_real_escape_string(htmlspecialchars($_POST['graph_min_ticksize']));
 $graph_type = mysql_real_escape_string(htmlspecialchars($_POST['graph_type']));


  
//1=wiredin 2=variable
//Als het wiredin betreft of een variable met output geselecteerd gaat het om een output gaan dus zetten we alle input velden op 0
if ($_POST['type'] == 1 || $_POST['type'] == 2 && $_POST['input_output'] == 2 ) {
	$input_output = "2"; // wiredanalog is output vanuit de nodo richting webapp
	$input_control = "0";
	$input_slider_min = "0";
	$input_slider_max = "0";
	$input_step = "0";
		
	}

	else {
	
	//Een grafiek gebruiken we niet bij een output dus eventuele postdata leegmaken.
	$graph_hours = "";
	$graph_min_ticksize = "";
	$graph_type = "";
	
	
	
	}

 
 if ($_POST['display'] == 1) {
	$suffix = mysql_real_escape_string(htmlspecialchars($_POST['suffix']));
	}
	
 else {
	$suffix = "";
 }
 
 if ($_POST['display'] == 2) {

	// ' vervangen door &#8217; omdat we anders met het get_value.js script in de knoei komen als iemand ' invoerd 
	 $suffix_true = str_replace("'","&#8217;",mysql_real_escape_string(htmlspecialchars($_POST['suffix_true'])));
	 $suffix_false = str_replace("'","&#8217;",mysql_real_escape_string(htmlspecialchars($_POST['suffix_false'])));
	 }
 
 else {
 
	 $suffix_true = "";
	 $suffix_false = "";
 }
 
 
  
 // save the data to the database 
 mysql_select_db($database, $db);
 
 //Totaal aantal records bepalen 
$RSValues = mysql_query("SELECT id FROM nodo_tbl_sensor WHERE user_id='$userId'") or die(mysql_error()); 
$RSValues_rows = mysql_num_rows($RSValues);
//aantal records met 1 verhogen zodat we deze waarde in het sorteerveld kunnen gebruiken
$sort_order = $RSValues_rows + 1;
 
   
 mysql_query("INSERT INTO nodo_tbl_sensor (sensor_type, display, collapsed, input_output, input_control, input_step, input_min_val, input_max_val, sensor_prefix, sensor_suffix, sensor_suffix_true, sensor_suffix_false, user_id, nodo_unit_nr,par1,graph_hours,graph_min_ticksize,graph_type,sort_order) 
 VALUES 
 ('$type','$display','$collapsed','$input_output','$input_control','$input_step','$input_slider_min','$input_slider_max','$prefix','$suffix','$suffix_true','$suffix_false','$userId','$unit','$par1','$graph_hours','$graph_min_ticksize','$graph_type','$sort_order')");
 // once saved, redirect back to the view page 
 header("Location: values.php#saved");    }
 
else 
{

}
//Records sorteren
 if (isset($_GET['sort'])) {
	
	$sensor_id = $_GET['id'];
	$sort = $_GET['sort'];
	$sort_order = $_GET['sort_order'];
	$prev_record = $_GET['sort_order'] - 1;
	$next_record = $_GET['sort_order'] + 1;
	
	if ($sort == "up") {
	

	 mysql_query("UPDATE nodo_tbl_sensor SET sort_order=sort_order +1 WHERE user_id='$userId' AND sort_order='$prev_record'") or die(mysql_error()); 	
	 mysql_query("UPDATE nodo_tbl_sensor SET sort_order=sort_order -1 WHERE user_id='$userId' AND sort_order='$sort_order' AND id='$sensor_id'") or die(mysql_error()); 
	
	}
	if ($sort == "down") {

	 mysql_query("UPDATE nodo_tbl_sensor SET sort_order=sort_order -1 WHERE user_id='$userId' AND sort_order='$next_record'") or die(mysql_error()); 	
	 mysql_query("UPDATE nodo_tbl_sensor SET sort_order=sort_order +1 WHERE user_id='$userId' AND sort_order='$sort_order' AND id='$sensor_id'") or die(mysql_error()); 
	
	}
header("Location: values.php?id=$sensor_id"); 
}
?>





<!DOCTYPE html> 
<html> 
 
<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1"> 
	<title><?php echo $title ?></title> 
	<?php require_once('../include/jquery_mobile.php'); ?>
</head> 
 
<body> 

<!-- Start of main page: -->

<div data-role="page" pageid="main" data-theme="<?php echo $theme?>">
 
	<?php require_once('../include/header_admin.php'); ?>
 
	<div data-role="content">	
			
	<div data-role="collapsible" data-content-theme="<?php echo $theme?>">
		<h3>Add</h3>

		<form action="values.php" data-ajax="false" method="post"> 
					
		<br>
	
		<label for="select-choice-0" class="select" >Input type:</label>
		<select name="type" id="type" data-native-menu="false" >
			<option value="1" selected="selected">WiredIn</option>
			<option value="2">Variable</option>
		</select>	
		
		<br>
		
		<label for="select-choice-1" class="select" >Display:</label>
		<select name="display" id="display" data-native-menu="false" >
			<option value="1" selected="selected">Value</option>
			<option value="2">State</option>
		</select>				
		
		<br>
		<label for="select-choice-2" class="select" >Expand on values page:</label>
		<select name="collapsed" id="collapsed" data-placeholder="true" data-native-menu="false">
			<option value="0">No</option>
			<option value="1">Yes</option>
		</select>
		<br>
		
		<div id="input_output_div">
			<label for="select-choice-3" class="select" >In or output:</label>
		    <select name="input_output" id="input_output" data-native-menu="false" >
				<option value="1" selected="selected">Input</option>
				<option value="2">Output</option>
			</select>
		
		<br>	
		
			<div id="input_div">
				<div id="input_control_div">
					<label for="select-choice-4" class="select" >Input control:</label>
					<select name="input_control" id="input_control" data-native-menu="false" >
						<option value="1" selected="selected">+/- Buttons</option>
						<option value="2">Slider</option>
					</select>
					<br>	
				</div>
				
				<div id="slider_min_max_div">
					<label for="input_slider_min_val">Minimum value:</label>
					<input type="text" name="input_slider_min_val" id="input_slider_min_val" value=""  />
					<br>
					<label for="input_slider_max_val">Maximum value:</label>
					<input type="text" name="input_slider_max_val" id="input_slider_max_val" value=""  />
					<br>
				</div>
				
				<div id="input_step_div">
					<label for="input_step_val">Step: (Example: 0.5)</label>
					<input type="text" name="input_step_val" id="input_step_val" value=""  />
					<br>
				</div>
			
			</div>
			
		</div>
		
		<label for="prefix">Prefix: (Example: Temperature outside:, Door:)</label>
		<input type="text" name="prefix" id="prefix" value=""  />
		
		<br>
		
		<div id="value_div">
			<label for="suffix">Suffix: (Example: &deg;C, M&sup3;)</label>
			<input type="text" name="suffix" id="suffix" value=""  />
			<br>
		</div>
		
		<div id="state_div">
			<label for="suffix_false">Suffix: >0 (Example: Open)</label>
			<input type="text" name="suffix_true" id="suffix_true" value=""  />
			<br>
			<label for="suffix_true">Suffix: <=0 (Example: Closed)</label>
			<input type="text" name="suffix_false" id="suffix_false" value=""  />
			<br>
		</div>
		
		<label for="unit" >Nodo unit: (1...15)</label>
		<input type="text" maxLength="2" name="unit" id="unit" value=""  />
		<br>
		<div id="label_wiredanalog_div">
			<label for="name">WiredIn port: (1...8)</label>
		</div>
		<div id="label_variable_div">
			<label for="name">Variable: (1...15)</label>
		</div>
		<input type="text" maxLength="2" name="par1" id="par1" value=""  />
		<br>
		<div id="graph_div">
			<label for="select-choice-5" class="select" >Graph: type:</label>
			<select name="graph_type" id="graph_type" data-native-menu="false" >
				<option value="1" selected="selected">Line</option>
				<option value="2">Bar (totals per day)</option>
			</select>
			<br>
			
							
				<label for="graph_hours">Graph: maximum hours to show:</label>
				<input type="text" MaxLength="5" name="graph_hours" id="graph_hours" value="24"  />
				<br>
			<div id="graph_ticksize_div">
				<label for="select-choice-7" class="select" >Graph: minimum tick size x-axis:</label>
				<select name="graph_min_ticksize" id="graph_min_ticksize" data-native-menu="false" >
					<option value="1" selected="selected">Minutes</option>
					<option value="2">Hours</option>
					<option value="3">Days</option>
					<!-- <option value="4">Weeks</option> tijdelijk uit omdat weken lastiger uit te rekenen zijn-->
					<option value="5">Months</option>
				</select>
				<br>
			</div>
		</div>
		<br>	
			
	           
		<input type="submit" name="submit" value="Save" >

		
	
	</form> 
			
	</div>		
			
	<div data-role="collapsible" data-collapsed="false" data-content-theme="<?php echo $theme?>">
	<h3>Edit</h3>
	<?php

						   
	//echo '<ul data-role="listview" data-split-icon="delete" data-split-theme="$theme" data-inset="true">';
	mysql_select_db($database, $db);
	$result = mysql_query("SELECT * FROM nodo_tbl_sensor WHERE user_id='$userId' ORDER BY sort_order ASC") or die(mysql_error());  
	$rows = mysql_num_rows($result);
			
			// loop through results of database query, displaying them in the table        
			$i=0;
			while($row = mysql_fetch_array($result)) 
			
				
				{                                
				 
				$i++;
				?>          
				
				
		  
				<div data-role="collapsible" data-collapsed="<?php if ($_GET['id'] == $row['id']) {echo "false";} else {echo "true";} ?>" data-content-theme="<?php echo $theme;?>">
				<h3><?php echo $row['sensor_prefix']; ?></h3>
					
				

				<?php if ($i > 1) { ?>
				<a href="values.php?sort=up&sort_order=<?php echo $row['sort_order']; ?>&id=<?php echo $row['id']; ?>" data-role="button"  data-icon="arrow-u" data-ajax="false">Move up</a>
				<?php } ?>
				
				<?php if ($i != $rows) { ?>
				<a href="values.php?sort=down&sort_order=<?php echo $row['sort_order']; ?>&id=<?php echo $row['id']; ?>" data-role="button" data-icon="arrow-d"  data-ajax="false">Move down</a>
				<?php } ?>
				
				<a href="values_edit.php?id=<?php echo $row['id']; ?>" data-role="button" data-icon="gear" data-ajax="false">Edit</a>
				<a href="values_delete_confirm.php?id=<?php echo $row['id']; ?>" data-role="button" data-icon="delete" data-rel="dialog">Delete</a>
				<a href="export_csv_values.php?id=<?php echo $row['id']; ?>" data-role="button" data-ajax="false">Export to csv</a>
				</div>
			
				<?php
				}         
	?>
	
	</div>
	
	</div><!-- /content -->
	
	<?php require_once('../include/footer_admin.php'); ?>
	
</div><!-- /main page -->

<!-- Start of saved page: -->
<div data-role="dialog" id="saved" data-theme="<?php echo $theme?>">

	<div data-role="header" data-theme="<?php echo $theme_header?>">
		<h1><?php echo $page_title?></h1>
	</div><!-- /header -->

	<div data-role="content">	
		<h2>Setting saved.</h2>
				
		<p><a href="values.php" data-role="button" data-inline="true" data-icon="back">Ok</a></p>	
	
	
	</div><!-- /content -->
	
	
</div><!-- /page saved -->
 <script type="text/javascript">		

$(document).ready(function() {

	//Als het document geladen word dan verbergen we onderstaanden divs

	$('#state_div').hide();
	$('#label_variable_div').hide(); 
	$('#input_output_div').hide();
	$('#slider_min_max_div').hide(); 
	
});

$('#display').change(function() {

	//Value
	if ($(this).attr('value')==1) {   

		$('#state_div').hide(); 
		$('#value_div').show(); 
		$('#graph_div').show(); 

	}

	//State   
	if ($(this).attr('value')==2) {   

		$('#state_div').show(); 
		$('#value_div').hide(); 
		$('#graph_div').hide(); 

 

	}
   
});


$('#type').change(function() {

	//WiredIn
	if ($(this).attr('value')==1) {   

		$('#label_wiredanalog_div').show();  
		$('#label_variable_div').hide();   
		$('#input_output_div').hide(); 
		$('#graph_div').show(); 

	}
  
	//Variable
	if ($(this).attr('value')==2) {   

		$('#label_wiredanalog_div').hide();  
		$('#label_variable_div').show(); 
		$('#input_output_div').show();  
		$('#graph_div').hide(); 
	 
	}
});


$('#input_output').change(function() {

	//Input
	if ($(this).attr('value')==1) {   
		
		$('#input_div').show();
		$('#graph_div').hide();  

	}

	//Output  
	if ($(this).attr('value')==2) {   

		$('#input_div').hide(); 
		$('#graph_div').show();    

	}
   
});

$('#input_control').change(function() {

	//+/-Buttons
	if ($(this).attr('value')==1) {   
	
		$('#slider_min_max_div').hide(); 

	}

	//Slider 
	if ($(this).attr('value')==2) {   

		$('#slider_min_max_div').show();   

	}
});

$('#graph_type').change(function() {

	//Line
	if ($(this).attr('value')==1) {   
	
		$('#graph_ticksize_div').show(); 

	}

	//Bars 
	if ($(this).attr('value')==2) {   

		$('#graph_ticksize_div').hide();   

	}
});

</script>
</body>
</html>

