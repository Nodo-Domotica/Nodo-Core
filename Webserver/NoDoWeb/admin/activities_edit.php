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

$page_title="Setup: Edit activity";



 // check if the form has been submitted. If it has, process the form and save it to the database if 
 
 if (isset($_POST['submit'])) 
 {  
 // confirm that the 'id' value is a valid integer before getting the form data 
 if (is_numeric($_POST['id'])) 
 { 
 // get form data, making sure it is valid 
 $id = $_POST['id']; 
 $name = mysql_real_escape_string(htmlspecialchars($_POST['name'])); 
 $events = mysql_real_escape_string(htmlspecialchars($_POST['events']));  
 

 
  
 // save the data to the database 
 mysql_select_db($database, $db);
 mysql_query("UPDATE nodo_tbl_activities SET name='$name', events='$events' WHERE id='$id' AND user_id='$userId'") or die(mysql_error());   
 // once saved, redirect back to the view page 
 header("Location: activities.php#saved");  
 } 
 } 
 else {
 mysql_select_db($database, $db);
 $id = $_GET['id']; $result = mysql_query("SELECT * FROM nodo_tbl_activities WHERE id=$id AND user_id='$userId'") or die(mysql_error());  
 $row = mysql_fetch_array($result);  
 
  }
  
  //Records sorteren
 if (isset($_GET['sort'])) {
	
	$device_id = $_GET['id'];
	$sort = $_GET['sort'];
	$sort_order = $_GET['sort_order'];
	$prev_record = $_GET['sort_order'] - 1;
	$next_record = $_GET['sort_order'] + 1;
	
	if ($sort == "up") {
	

	 mysql_query("UPDATE nodo_tbl_activities SET sort_order=sort_order +1 WHERE user_id='$userId' AND sort_order='$prev_record'") or die(mysql_error()); 	
	 mysql_query("UPDATE nodo_tbl_activities SET sort_order=sort_order -1 WHERE user_id='$userId' AND sort_order='$sort_order' AND id='$device_id'") or die(mysql_error()); 
	
	}
	if ($sort == "down") {

	 mysql_query("UPDATE nodo_tbl_activities SET sort_order=sort_order -1 WHERE user_id='$userId' AND sort_order='$next_record'") or die(mysql_error()); 	
	 mysql_query("UPDATE nodo_tbl_activities SET sort_order=sort_order +1 WHERE user_id='$userId' AND sort_order='$sort_order' AND id='$device_id'") or die(mysql_error()); 
	
	}
header("Location: activities.php?id=$device_id"); 
}

 ?>


 <!DOCTYPE html> 
<html> 
 
<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1"> 
	<title><?php echo $title;?></title> 
	<?php require_once('../include/jquery_mobile.php'); ?>
</head> 
 
<body> 

 
<div data-role="page">
 
	<?php require_once('../include/header_admin.php'); ?>
 
	<div data-role="content" data-theme="<?php echo $theme?>">	

	<form action="activities_edit.php" data-ajax="false" method="post"> 
	
	 
		
		<input type="hidden" name="id" id="id" value="<?php echo $row['id'] ;?>"  />
	
	<br \>
	
		<br \>
	
		<label for="name">Name: </label>
		<input type="text" name="name" id="name" value="<?php echo $row['name'] ;?>"  />
		<br \>
		<label for="name">Event(s): (example: userevent 100,100;sendkaku a1,on)</label>
		<input type="text" name="events" id="events" value="<?php echo $row['events'] ;?>"  />
	
	   <br \>
         <a href="activities.php" data-role="button" data-ajax="false">Cancel</a> 
		<input type="submit" name="submit" value="Save" >

		
	
	</form> 

		
	</div><!-- /content -->
	
	<?php require_once('../include/footer_admin.php'); ?>
	
</div><!-- /page -->


 
</body>
</html>

