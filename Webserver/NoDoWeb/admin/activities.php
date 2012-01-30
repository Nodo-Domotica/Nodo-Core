<?php 

require_once('../connections/tc.php'); 
require_once('../include/auth.php');
require_once('../include/settings.php');

$page_title = "Setup: Activiteiten";	  



// check if the form has been submitted. If it has, process the form and save it to the database if 

if (isset($_POST['submit'])) 
{  
 
 // get form data, making sure it is valid 
 $name = mysql_real_escape_string(htmlspecialchars($_POST['name'])); 
 $events = mysql_real_escape_string(htmlspecialchars($_POST['events']));  
 
 
 
  
 // save the data to the database 
 mysql_select_db($database_tc, $tc);
 
   
 mysql_query("INSERT INTO nodo_tbl_activities (name, events, user_id) 
 VALUES 
 ('$name','$events','$userId')");
 // once saved, redirect back to the view page 
 header("Location: activities.php#saved");    }
 
else 
{
mysql_select_db($database_tc, $tc);
$result = mysql_query("SELECT * FROM nodo_tbl_activities WHERE user_id='$userId'") or die(mysql_error());  
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
			<h3>Toevoegen</h3>
	<form action="activities.php" data-ajax="false" method="post"> 
	
	 
				
	<br>
	
		<label for="name">Naam: </label>
		<input type="text" name="name" id="name" value=""  />
		<br>
		<label for="name">Event(s): (voorbeeld: userevent 100,100)</label>
		<input type="text" name="events" id="events" value=""  />
	
	    <br>
        
		<input type="submit" name="submit" value="Toevoegen" >

		
	
	</form> 
	</div>
	
	
		<div data-role="collapsible" data-collapsed="false">
			<h3>Aanpassen</h3>
			<?php
			 
								   
			echo '<ul data-role="listview" data-split-icon="delete" data-split-theme="a">';
	  
			echo '<br>';   
			// loop through results of database query, displaying them in the table        
			while($row = mysql_fetch_array($result)) 
			{                                
					   
			echo '<li><a href="activities_edit.php?id=' . $row['id'] . '" title=Aanpassen data-ajax="false">'. $row['name'] .'</a>';                
			echo '<a href="activities_delete_confirm.php?id=' . $row['id'] . '" data-rel="dialog">Verwijderen</a></li>';
			
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
		<h2> De wijzigingen zijn opgeslagen.</h2>
				
		<p><a href="activities.php" data-role="button" data-inline="true" data-icon="back">Ok</a></p>	
	
	
	</div><!-- /content -->
	
	
</div><!-- /page saved -->
 
</body>
</html>

