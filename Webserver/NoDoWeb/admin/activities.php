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

$page_title = "Setup: Activities";	  



// check if the form has been submitted. If it has, process the form and save it to the database if 

if (isset($_POST['submit'])) 
{  
 
 // get form data, making sure it is valid 
 $name = mysql_real_escape_string(htmlspecialchars($_POST['name'])); 
 $events = mysql_real_escape_string(htmlspecialchars($_POST['events']));  
 
 
 
  
 // save the data to the database 
 mysql_select_db($database, $db);
 
 //Totaal aantal records bepalen 
$RSActivities = mysql_query("SELECT id FROM nodo_tbl_activities WHERE user_id='$userId'") or die(mysql_error()); 
$RSActivities_rows = mysql_num_rows($RSActivities);
//aantal records met 1 verhogen zodat we deze waarde in het sorteerveld kunnen gebruiken
$sort_order = $RSActivities_rows + 1; 
 
   
 mysql_query("INSERT INTO nodo_tbl_activities (name, events, user_id, sort_order) 
 VALUES 
 ('$name','$events','$userId','$sort_order')");
 // once saved, redirect back to the view page 
 header("Location: activities.php#saved");    }
 


 //Records sorteren
 if (isset($_GET['sort'])) {
	
		if (is_numeric($_GET['id']) && is_numeric($_GET['sort_order'])){
		
		$device_id = $_GET['id'];
		$sort = mysql_real_escape_string($_GET['sort']);
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

	<div data-role="collapsible"  data-inset="false" data-collapsed-icon="plus" data-expanded-icon="minus" data-iconpos="right">
			<h3>Add activitie:</h3>
	<form action="activities.php" data-ajax="false" method="post"> 
		
		<label for="name">Name: </label>
		<input type="text" name="name" id="name" value=""  />
		<br \>
		<label for="name">Event(s): (Example: userevent 1;sendkaku a1,on)</label>
		<input type="text" name="events" id="events" value=""  />
	
	    <br \>
        
		<input type="submit" name="submit" value="Save" >
    </form> 
	</div>
	
	
		<div data-role="collapsible" data-collapsed="false" data-inset="false" data-iconpos="right" >
			<h3>Edit activities:</h3>
			<?php
			 
								   
			
			mysql_select_db($database, $db);
			$result = mysql_query("SELECT * FROM nodo_tbl_activities WHERE user_id='$userId' ORDER BY sort_order ASC") or die(mysql_error());  
			$rows = mysql_num_rows($result);
			
			// loop through results of database query, displaying them in the table        
			$i=0;
			while($row = mysql_fetch_array($result)) 
			
				
				{                                
				 
				$i++;
				?>          
				
				
		  
				<div data-role="collapsible" data-collapsed="<?php if ($_GET['id'] == $row['id']) {echo "false";} else {echo "true";} ?>"  data-iconpos="right" data-content-theme="<?php echo $theme;?>">
				<h3><?php echo $row['name']; ?></h3>
				<?php if ($i > 1) { ?>
				<a href="activities.php?sort=up&sort_order=<?php echo $row['sort_order']; ?>&id=<?php echo $row['id']; ?>" data-role="button"  data-icon="arrow-u" data-ajax="false">Move up</a>
				<?php } ?>
				<?php if ($i != $rows) { ?>
				<a href="activities.php?sort=down&sort_order=<?php echo $row['sort_order']; ?>&id=<?php echo $row['id']; ?>" data-role="button" data-icon="arrow-d"  data-ajax="false">Move down</a>
				<?php } ?>
				<a href="activities_edit.php?id=<?php echo $row['id']; ?>" data-role="button" data-icon="gear" data-ajax="false">Edit</a>
				<a href="activities_delete_confirm.php?id=<?php echo $row['id']; ?>" data-role="button"  data-icon="delete" data-rel="dialog">Delete</a>
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
		<h2>Settings saved.</h2>
				
		<p><a href="activities.php" data-role="button" data-inline="true" data-icon="back">Ok</a></p>	
	
	
	</div><!-- /content -->
	
	
</div><!-- /page saved -->
 
</body>
</html>

