<?php 

require_once('../connections/tc.php'); 
require_once('../include/auth.php');
require_once('../include/settings.php');

$page_title = "Setup: Sensors";	  



// check if the form has been submitted. If it has, process the form and save it to the database if 

if (isset($_POST['submit'])) 
{  
 
 // get form data, making sure it is valid 
 $name = mysql_real_escape_string(htmlspecialchars($_POST['name'])); 
 $unit = mysql_real_escape_string(htmlspecialchars($_POST['unit'])); 
 $par1 = mysql_real_escape_string(htmlspecialchars($_POST['par1'])); 
 $type = mysql_real_escape_string(htmlspecialchars($_POST['type']));
 $display = mysql_real_escape_string(htmlspecialchars($_POST['display']));
  
 if ($_POST['display'] == 1) {
	$suffix = mysql_real_escape_string(htmlspecialchars($_POST['suffix']));
	}
	
 else {
	$suffix = "";
 }
 
 if ($_POST['display'] == 2) {
 
	 $suffix_true = mysql_real_escape_string(htmlspecialchars($_POST['suffix_true']));
	 $suffix_false = mysql_real_escape_string(htmlspecialchars($_POST['suffix_false']));
	 }
 
 else {
 
	 $suffix_true = "";
	 $suffix_false = "";
 }
 
 
  
 // save the data to the database 
 mysql_select_db($database_tc, $tc);
 
   
 mysql_query("INSERT INTO nodo_tbl_sensor (sensor_type, display, sensor_name, sensor_suffix, sensor_suffix_true, sensor_suffix_false, user_id, nodo_unit_nr,par1) 
 VALUES 
 ('$type','$display','$name','$suffix','$suffix_true','$suffix_false','$userId','$unit','$par1')");
 // once saved, redirect back to the view page 
 header("Location: sensor.php#saved");    }
 
else 
{
mysql_select_db($database_tc, $tc);
$result = mysql_query("SELECT * FROM nodo_tbl_sensor WHERE user_id='$userId'") or die(mysql_error());  
}?>




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

<div data-role="page" pageid="main">
 
	<?php require_once('../include/header_admin.php'); ?>
 
	<div data-role="content">	

		
	<div data-role="collapsible">
			<h3>Add</h3>

			<form action="sensor.php" data-ajax="false" method="post"> 
	
	 
				
	<br>
		<label for="select-choice-0" class="select" >Sensor type:</label>
		    <select name="type" id="type" data-native-menu="false" >
				<option value="1" selected="selected">WiredAnalog</option>
				<option value="2">Variable</option>
			</select>	
		<br>
		<label for="select-choice-1" class="select" >Display:</label>
		    <select name="display" id="display" data-native-menu="false" >
				<option value="1" selected="selected">Value</option>
				<option value="2">State</option>
			</select>	
		<br>
		<div id="label_value_div">
		<label for="name">Name: (Example: Temperature outside:)</label>
		</div>
		<div id="label_state_div">
		<label for="name">Name: (Example: Door:)</label>
		</div>
		<input type="text" name="name" id="name" value=""  />
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
		
		<label for="select-choice-2" class="select" >Nodo unit:</label>
		    <select name="unit" id="unit" data-native-menu="false" >
				<option value="1">1</option>
				<option value="2">2</option>
				<option value="3">3</option>
				<option value="4">4</option>
				<option value="5">5</option>
				<option value="6">6</option>
				<option value="7">7</option>
				<option value="8">8</option>
				<option value="9">9</option>
				<option value="10">10</option>
				<option value="11">11</option>
				<option value="12">12</option>
				<option value="13">13</option>
				<option value="14">14</option>
				<option value="15">15</option>
			</select>
		<br>
		
		<div id="label_wiredanalog_div">
		<label for="name">WiredAnalog port (1 until 15)</label>
		</div>
		<div id="label_variable_div">
		<label for="name">Variable (1 until 15)</label>
		</div>
		<input type="text" name="par1" id="par1" value=""  />
		<br>
	    <br>
        
		<input type="submit" name="submit" value="Add" >

		
	
	</form> 
			
	</div>		
			
	<div data-role="collapsible" data-collapsed="false">
	<h3>Edit</h3>
	<?php

						   
	echo '<ul data-role="listview" data-split-icon="delete" data-split-theme="a">';

	echo '<br>';   
	// loop through results of database query, displaying them in the table        
	while($row = mysql_fetch_array($result)) 
	{                                
			   
	echo '<li><a href="sensor_edit.php?id=' . $row['id'] . '" title=Edit data-ajax="false">'. $row['sensor_name'] .'</a>';                
	echo '<a href="sensor_delete_confirm.php?id=' . $row['id'] . '" data-rel="dialog">Delete</a></li>';
	
	}         
	?>
	
	</div>
	
	</div><!-- /content -->
	
	<?php require_once('../include/footer_admin.php'); ?>
	
</div><!-- /main page -->

<!-- Start of saved page: -->
<div data-role="dialog" id="saved">

	<div data-role="header">
		<h1><?php echo $page_title?></h1>
	</div><!-- /header -->

	<div data-role="content">	
		<h2>Settings saved.</h2>
				
		<p><a href="sensor.php" data-role="button" data-inline="true" data-icon="back">Ok</a></p>	
	
	
	</div><!-- /content -->
	
	
</div><!-- /page saved -->
 <script type="text/javascript">		

$(document).ready(function() {

$('#label_variable_div').hide();   
$('#state_div').hide();
$('#label_state_div').hide(); 
	
});

$('#display').change(function() 
{

if ($(this).attr('value')==1) {   

$('#state_div').hide(); 
$('#label_state_div').hide();  
$('#value_div').show(); 
$('#label_value_div').show();      

}
   
if ($(this).attr('value')==2) {   

$('#state_div').show(); 
$('#label_state_div').show();
$('#label_value_div').hide();   
$('#value_div').hide(); 

}


   
});




$('#type').change(function() 
{

if ($(this).attr('value')==1) {   

$('#label_wiredanalog_div').show();  
$('#label_variable_div').hide();   

}
   //alert('Value change to ' + $(this).attr('value'));
if ($(this).attr('value')==2) {   

$('#label_wiredanalog_div').hide();  
$('#label_variable_div').show();   

}


   
});
</script>
</body>
</html>

