<?php 

require_once('../connections/tc.php'); 
require_once('../include/auth.php');
require_once('../include/settings.php');

$page_title = "Setup: Metingen";	  



// check if the form has been submitted. If it has, process the form and save it to the database if 

if (isset($_POST['submit'])) 
{  
 
 // get form data, making sure it is valid 
 $name = mysql_real_escape_string(htmlspecialchars($_POST['name'])); 
 $events = mysql_real_escape_string(htmlspecialchars($_POST['events']));  
 
 
 
  
 // save the data to the database 
 mysql_select_db($database_tc, $tc);
 
   
 mysql_query("INSERT INTO nodo_tbl_xxxx (name, events, user_id) 
 VALUES 
 ('$name','$events','$userId')");
 // once saved, redirect back to the view page 
 header("Location: activities.php#saved");    }
 
else 
{
//mysql_select_db($database_tc, $tc);
//$result = mysql_query("SELECT * FROM nodo_tbl_xxx WHERE user_id='$userId'") or die(mysql_error());  
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

	<h1>Uitwerken!</h1>
	
	<div data-role="collapsible">
			<h3>Toevoegen</h3>

			<form action="measurements.php" data-ajax="false" method="post"> 
	
	 
				
	<br>
	
		<label for="name">Naam: </label>
		<input type="text" name="name" id="name" value=""  />
		<br>
		<label for="select-choice-0" class="select" >Nodo unit:</label>
		    <select name="unit" id="unit" data-native-menu="false" >
				<option value="0" data-placeholder="true">Kies unit nr:....</option>
				<option value="0">0</option>
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
		<label for="select-choice-0" class="select" >Wiredanalog port:</label>
		    <select name="port" id="port" data-native-menu="false" >
				<option value="0" data-placeholder="true">Kies port:....</option>
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
        
		<input type="submit" name="submit" value="Toevoegen" >

		
	
	</form> 
			
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
		<h2> De wijzigingen zijn opgeslagen.</h2>
				
		<p><a href="measurements" data-role="button" data-inline="true" data-icon="back">Ok</a></p>	
	
	
	</div><!-- /content -->
	
	
</div><!-- /page saved -->
 
</body>
</html>

