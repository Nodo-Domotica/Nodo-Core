<?php 

require_once('../connections/tc.php'); 
require_once('../include/auth.php');
require_once('../include/user_settings.php');

$page_title = "Setup: Notifications";	  



// check if the form has been submitted. If it has, process the form and save it to the database if 

if (isset($_POST['submit'])) 
{  
 
 // get form data, making sure it is valid 
 $name = mysql_real_escape_string(htmlspecialchars($_POST['name'])); 
 $event = mysql_real_escape_string(htmlspecialchars($_POST['event'])); 
 $recipient = mysql_real_escape_string(htmlspecialchars($_POST['recipient']));
 $subject = mysql_real_escape_string(htmlspecialchars($_POST['subject'])); 
 $body = mysql_real_escape_string(htmlspecialchars($_POST['body']));  
 
 // ongewenste spaties uit de invoer verwijderen
 $event_lenght = strlen($event);
 $pos1 = strpos($event, " ");
 $cmd = trim(substr($event, 0, $pos1));
 $event = $cmd . " " .  str_replace(" ","",trim(substr($event, $pos1,  $event_lenght)));

   
 // save the data to the database 
 mysql_select_db($database_tc, $tc);
 
   
 mysql_query("INSERT INTO nodo_tbl_notifications (name, event, recipient, subject, body, user_id) 
 VALUES 
 ('$name','$event','$recipient','$subject','$body','$userId')");
 // once saved, redirect back to the view page 
 header("Location: notifications.php#saved");    }
 
else 
{
mysql_select_db($database_tc, $tc);
$result = mysql_query("SELECT * FROM nodo_tbl_notifications WHERE user_id='$userId'") or die(mysql_error());  
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

<div data-role="page" pageid="main" data-theme="<?php echo $theme?>">
 
	<?php require_once('../include/header_admin.php'); ?>
 
	<div data-role="content">	

	<div data-role="collapsible" data-content-theme="<?php echo $theme?>">
			<h3>Add</h3>
	<form action="notifications.php" data-ajax="false" method="post"> 
	
	 
				
	<br>
	<label for="name">Name: </label>
		<input type="text" name="name" id="name" value=""  />
		<br>
		<label for="name">Event: (Example: userevent 100,0 | newkaku 1,on)</label>
		<input type="text" name="event" id="event" value=""  />
		<br>
		<label for="name">Recipient:</label>
		<input type="text" name="recipient" id="recipient" value=""  />
		<br>
		<label for="name">Subject:</label>
		<input type="text" name="subject" id="subject" value=""  />
		<br>
		<label for="body">Body:</label>
		<textarea name="body" id="body"></textarea>
		<br>		
	
	    <br>
        
		<input type="submit" name="submit" value="Save" >

		
	
	</form> 
	</div>
	
	
		<div data-role="collapsible" data-collapsed="false" data-content-theme="<?php echo $theme?>">
			<h3>Edit</h3>
			<?php
			 
								   
			echo '<ul data-role="listview" data-split-icon="delete" data-split-theme="$theme" data-inset="true">';
	  
			//echo '<br>';   
			// loop through results of database query, displaying them in the table        
			while($row = mysql_fetch_array($result)) 
			{                                
					   
			echo '<li><a href="notifications_edit.php?id=' . $row['id'] . '" title=Edit data-ajax="false">'. $row['name'] .'</a>';                
			echo '<a href="notifications_delete_confirm.php?id=' . $row['id'] . '" data-rel="dialog">Delete</a></li>';
			
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
				
		<p><a href="notifications.php" data-role="button" data-inline="true" data-icon="back">Ok</a></p>	
	
	
	</div><!-- /content -->
	
	
</div><!-- /page saved -->
 
</body>
</html>

