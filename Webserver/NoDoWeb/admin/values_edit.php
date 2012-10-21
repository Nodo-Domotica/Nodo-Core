<?php 
/***********************************************************************************************************************
"Nodo Web App" Copyright � 2012 Martin de Graaf

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

$page_title="Setup: Edit values";


 // check if the form has been submitted. If it has, process the form and save it to the database if 
 
 if (isset($_POST['submit'])) 
 {  
 // confirm that the 'id' value is a valid integer before getting the form data 
 if (is_numeric($_POST['id'])) 
 { 
 
 $id = mysql_real_escape_string(htmlspecialchars($_POST['id']));  
 $prefix = mysql_real_escape_string(htmlspecialchars($_POST['prefix'])); 
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
 $graph_line_color = mysql_real_escape_string(htmlspecialchars($_POST['graph_line_color']));
 
  
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
	$graph_line_color ="";
	
	
	
	}

 
 if ($_POST['display'] == 1) {
	$suffix = mysql_real_escape_string(htmlspecialchars($_POST['suffix']));
	}
	
 else {
	$suffix = "";
 }
 
 if ($_POST['display'] == 2) {
 
	 // ' vervangen door &#8217; omdat we anders met het get_value.js script in de knoei komen als iemand ' gebruikt 
	 $suffix_true = str_replace("'","&#8217;",mysql_real_escape_string(htmlspecialchars($_POST['suffix_true'])));
	 $suffix_false = str_replace("'","&#8217;",mysql_real_escape_string(htmlspecialchars($_POST['suffix_false'])));
	 }
 
 else {
 
	 $suffix_true = "";
	 $suffix_false = "";
 }
 
	
 // save the data to the database 
 mysql_select_db($database, $db);
 mysql_query("UPDATE nodo_tbl_sensor SET sensor_type='$type',display='$display',collapsed='$collapsed',input_output='$input_output',input_control='$input_control',input_step='$input_step'
 ,input_min_val='$input_slider_min',input_max_val='$input_slider_max',sensor_prefix='$prefix', sensor_suffix='$suffix', sensor_suffix_true='$suffix_true'
 ,sensor_suffix_false='$suffix_false', nodo_unit_nr='$unit',par1='$par1',graph_hours='$graph_hours',graph_min_ticksize='$graph_min_ticksize',graph_type='$graph_type',graph_line_color='$graph_line_color'
 WHERE id='$id' AND user_id='$userId'") or die(mysql_error());   
 // once saved, redirect back to the view page 
 header("Location: values.php#saved");  
 } 
 } 
 else {
 mysql_select_db($database, $db);
 $id = $_GET['id']; $result = mysql_query("SELECT * FROM nodo_tbl_sensor WHERE id=$id AND user_id='$userId'") or die(mysql_error());  
 $row = mysql_fetch_array($result);  
 
  }?>

 <!DOCTYPE html> 
<html> 
 
<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1"> 
	<title><?php echo $title;?></title> 
	<?php require_once('../include/jquery_mobile.php'); ?>
</head> 
 
<body> 

 
<div data-role="page" data-theme="<?php echo $theme?>">
 
	<?php require_once('../include/header_admin.php'); ?>
 
	<div data-role="content">	
	
	
<form action="values_edit.php" data-ajax="false" method="post"> 
	
	 <input type="hidden" name="id" id="id" value="<?php echo $row['id'] ;?>"  />
				
	<br \>
	
	
	
		<form action="values.php" data-ajax="false" method="post"> 
					
		<br \>
	
		<label for="select-choice-0" class="select" >Input type:</label>
		<select name="type" id="type" data-native-menu="false" >
			<option value="1" <?php if ($row['sensor_type'] == 1) {echo 'selected="selected"';}?>>WiredIn</option>
			<option value="2" <?php if ($row['sensor_type'] == 2) {echo 'selected="selected"';}?>>Variable</option>
		</select>	
		
		<br \>
		
		<label for="select-choice-1" class="select" >Display:</label>
		<select name="display" id="display" data-native-menu="false" >
			<option value="1" <?php if ($row['display'] == 1) {echo 'selected="selected"';}?>>Value</option>
			<option value="2" <?php if ($row['display'] == 2) {echo 'selected="selected"';}?>>State</option>
		</select>				
		
		<br \>
		
		<label for="select-choice-2" class="select" >Expand on values page:</label>
		<select name="collapsed" id="collapsed" data-placeholder="true" data-native-menu="false">
			<option value="0"<?php if ($row['collapsed'] == 0) {echo 'selected="selected"';}?>>No</option>
			<option value="1"<?php if ($row['collapsed'] == 1) {echo 'selected="selected"';}?>>Yes</option>
		</select>
		<br \>
		
		<div id="input_output_div">
			<label for="select-choice-3" class="select" >In or output:</label>
		    <select name="input_output" id="input_output" data-native-menu="false" >
				<option value="1" <?php if ($row['input_output'] == 1) {echo 'selected="selected"';}?>>Input</option>
				<option value="2" <?php if ($row['input_output'] == 2) {echo 'selected="selected"';}?>>Output</option>
			</select>
		
		<br \>	
		
			<div id="input_div">
				<div id="input_control_div">
					<label for="select-choice-4" class="select" >Input control:</label>
					<select name="input_control" id="input_control" data-native-menu="false" >
						<option value="1" <?php if ($row['input_control'] == 1) {echo 'selected="selected"';}?>>+/- Buttons</option>
						<option value="2" <?php if ($row['input_control'] == 2) {echo 'selected="selected"';}?>>Slider</option>
					</select>
					<br \>	
				</div>
				
				<div id="slider_min_max_div">
					<label for="input_slider_min_val">Minimum value:</label>
					<input type="text" name="input_slider_min_val" id="input_slider_min_val" value="<?php echo $row['input_min_val'] ;?>"  />
					<br \>
					<label for="input_slider_max_val">Maximum value:</label>
					<input type="text" name="input_slider_max_val" id="input_slider_max_val" value="<?php echo $row['input_max_val'] ;?>"  />
					<br \>
				</div>
				
				<div id="input_step_div">
					<label for="input_step_val">Step: (Example: 0.5)</label>
					<input type="text" name="input_step_val" id="input_step_val" value="<?php echo $row['input_step'] ;?>"  />
					<br \>
				</div>
			
			</div>
			
		</div>
		
		<label for="prefix">Prefix: (Example: Temperature outside:, Door:)</label>
		<input type="text" name="prefix" id="prefix" value="<?php echo $row['sensor_prefix'] ;?>"  />
		
		<br \>
		
		<div id="value_div">
			<label for="suffix">Suffix: (Example: &deg;C, M&sup3;)</label>
			<input type="text" name="suffix" id="suffix" value="<?php echo $row['sensor_suffix'] ;?>"  />
			<br \>
		</div>
		
		<div id="state_div">
			<label for="suffix_false">Suffix: >0 (Example: Open)</label>
			<input type="text" name="suffix_true" id="suffix_true" value="<?php echo $row['sensor_suffix_true'] ;?>"  />
			<br \>
			<label for="suffix_true">Suffix: <=0 (Example: Closed)</label>
			<input type="text" name="suffix_false" id="suffix_false" value="<?php echo $row['sensor_suffix_false'] ;?>"  />
			<br \>
		</div>
		
		<label for="unit" >Nodo unit: (1...15)</label>
		<input type="text" maxLength="2" name="unit" id="unit" value="<?php echo $row['nodo_unit_nr'] ;?>"  />
		<br \>
		<div id="label_wiredanalog_div">
			<label for="name">WiredIn port: (1...8)</label>
		</div>
		<div id="label_variable_div">
			<label for="name">Variable: (1...15)</label>
		</div>
		<input type="text" maxLength="2" name="par1" id="par1" value="<?php echo $row['par1'] ;?>"  />
		<br \>
		<div id="graph_div">
			<label for="select-choice-5" class="select" >Graph: type:</label>
			<select name="graph_type" id="graph_type" data-native-menu="false" >
				<option value="1" <?php if ($row['graph_type'] == 1) {echo 'selected="selected"';}?>>Line</option>
				<option value="2" <?php if ($row['graph_type'] == 2) {echo 'selected="selected"';}?>>Bar (day totals)</option>
			</select>
			<br \>
			
			
			<label for="graph_hours">Graph: maximum hours to show:</label>
			<input type="text" maxLength="5" name="graph_hours" id="graph_hours" value="<?php echo $row['graph_hours'] ;?>"  />
			<br \>
			<label for="select-choice-6" class="select" >Graph line color:</label>
			<select name="graph_line_color" id="graph_line_color" data-native-menu="true" >
				<option value=""<?php if ($row['graph_line_color'] == "") {echo 'selected="selected"';}?>>Default</option>
				<option value="#000000"<?php if ($row['graph_line_color'] == "#000000") {echo 'selected="selected"';}?>>Black</option>
				<option value="#0000FF"<?php if ($row['graph_line_color'] == "#0000FF") {echo 'selected="selected"';}?>>Blue</option>
				<option value="#A52A2A"<?php if ($row['graph_line_color'] == "#A52A2A") {echo 'selected="selected"';}?>>Brown</option>
				<option value="#00FFFF"<?php if ($row['graph_line_color'] == "#00FFFF") {echo 'selected="selected"';}?>>Cyan</option>
				<option value="#00008B"<?php if ($row['graph_line_color'] == "#00008B") {echo 'selected="selected"';}?>>Dark Blue</option>
				<option value="#006400"<?php if ($row['graph_line_color'] == "#006400") {echo 'selected="selected"';}?>>Green</option>
				<option value="#FF00FF"<?php if ($row['graph_line_color'] == "#FF00FF") {echo 'selected="selected"';}?>>Magenta</option>
				<option value="#FFA500"<?php if ($row['graph_line_color'] == "#FFA500") {echo 'selected="selected"';}?>>Orange</option>
				<option value="#FFC0CB"<?php if ($row['graph_line_color'] == "#FFC0CB") {echo 'selected="selected"';}?>>Pink</option>
				<option value="#800080"<?php if ($row['graph_line_color'] == "#800080") {echo 'selected="selected"';}?>>Purple</option>
				<option value="#FF0000"<?php if ($row['graph_line_color'] == "#FF0000") {echo 'selected="selected"';}?>>Red</option>
				<option value="#FFFF00"<?php if ($row['graph_line_color'] == "#FFFF00") {echo 'selected="selected"';}?>>Yellow</option>
				<option value="#FFFFFF"<?php if ($row['graph_line_color'] == "#FFFFFF") {echo 'selected="selected"';}?>>White</option>
			</select>
			<br \>
			
			<div id="graph_ticksize_div">
			<label for="select-choice-7" class="select" >Graph: minimum tick size x-axis:</label>
			<select name="graph_min_ticksize" id="graph_min_ticksize" data-native-menu="false" >
				<option value="1"<?php if ($row['graph_min_ticksize'] == 1) {echo 'selected="selected"';}?>>Minutes</option>
				<option value="2"<?php if ($row['graph_min_ticksize'] == 2) {echo 'selected="selected"';}?>>Hours</option>
				<option value="3"<?php if ($row['graph_min_ticksize'] == 3) {echo 'selected="selected"';}?>>Days</option>
			<!--	<option value="4"<?php if ($row['graph_min_ticksize'] == 4) {echo 'selected="selected"';}?>>Weeks</option> -->
				<option value="5"<?php if ($row['graph_min_ticksize'] == 5) {echo 'selected="selected"';}?>>Months</option>
			</select>
			<br \>
			</div>
		</div>
		<br \>	
			
	    <a href="values.php" data-role="button" data-ajax="false">Cancel</a>       
		<input type="submit" name="submit" value="Save" >

		
	
	
	
	</form> 
			
				
	</div><!-- /content -->
	
	<?php require_once('../include/footer_admin.php'); ?>
	
</div><!-- /page -->

<script type="text/javascript">	

<?php 

if ($row['sensor_type'] == 1) {

	echo "$('#label_wiredanalog_div').show();";
	echo "$('#label_variable_div').hide();";
	echo "$('#input_output_div').hide();";
	echo "$('#graph_div').show();";
 
}

 if ($row['sensor_type'] == 2) {

	echo "$('#label_wiredanalog_div').hide();";
	echo "$('#label_variable_div').show();";
	echo "$('#graph_div').hide();";
	echo "$('#input_output_div').show();";
	
		
	
}


if ($row['input_control'] == 1) { 
	
	echo "$('#slider_min_max_div').hide();";
	
}

if ($row['input_control'] == 2) { 
	
	echo "$('#slider_min_max_div').show();";
	
}	

if ($row['display'] == 1) { 

	echo "$('#state_div').hide();";
	echo "$('#value_div').show();";
	echo "$('#graph_div').show();";

}

if ($row['display'] == 2) { 

	echo "$('#state_div').show();";
	echo "$('#value_div').hide();";
	echo "$('#graph_div').hide();";

}

if ($row['input_output'] == 1) { 
	
	echo "$('#input_output_div').show();";
	echo "$('#input_div').show();";
	echo "$('#graph_div').hide();";
}
if ($row['input_output'] == 2) { 
	
	
	echo "$('#input_output_div').show();";
	echo "$('#input_div').hide();";
	echo "$('#graph_div').show();";
}

if ($row['input_output'] == 2 && $row['display'] == 2 ) { 
	
	
	echo "$('#state_div').show();";
	echo "$('#value_div').hide();";
	echo "$('#graph_div').hide();";
}

if ($row['graph_type'] == 1) { 
	
	
	echo "$('#graph_ticksize_div').show();";
}

if ($row['graph_type'] == 2) { 
	
	
	echo "$('#graph_ticksize_div').hide();";
}	

?>


$('#type').change(function() 
{
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

$('#display').change(function() 
{

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

$('#input_output').change(function() 
{

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
$('#input_control').change(function() 
{

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

