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
 $suffix = mysql_real_escape_string(htmlspecialchars($_POST['suffix']));
 $unit = mysql_real_escape_string(htmlspecialchars($_POST['unit'])); 
 $port = mysql_real_escape_string(htmlspecialchars($_POST['port'])); 
 
 
 
  
 // save the data to the database 
 mysql_select_db($database_tc, $tc);
 
   
 mysql_query("INSERT INTO nodo_tbl_sensor (sensor_name, sensor_suffix, user_id, nodo_unit_nr,port) 
 VALUES 
 ('$name','$suffix','$userId','$unit','$port')");
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
	
		<label for="name">Name: (Example: Temperature outside:)</label>
		<input type="text" name="name" id="name" value=""  />
		<br>
		<label for="select-choice-0" class="select" >Suffix:</label>
		    <select name="suffix" id="suffix" data-native-menu="false" >
				<option value="0" data-placeholder="true">Select suffix:</option>
				<option value="%">%</option>
				<option value="&deg;C">&deg;C</option>
				<option value=" ">None</option>
			</select>
		<br>
		<label for="select-choice-1" class="select" >NoDo unit:</label>
		    <select name="unit" id="unit" data-native-menu="false" >
				<option value="0" data-placeholder="true">Select unit nr:</option>
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
		<label for="select-choice-2" class="select" >Wiredanalog port:</label>
		    <select name="port" id="port" data-native-menu="false" >
				<option value="0" data-placeholder="true">Select port:</option>
				<option value="1">1</option>
				<option value="2">2</option>
				<option value="3">3</option>
				<option value="4">4</option>
				<option value="5">5</option>
				<option value="6">6</option>
				<option value="7">7</option>
				<option value="8">8</option>
			</select>
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
 
</body>
</html>

