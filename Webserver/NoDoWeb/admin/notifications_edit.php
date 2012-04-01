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

require_once('../connections/tc.php'); 
require_once('../include/auth.php'); 
require_once('../include/user_settings.php'); 

$page_title="Setup: Edit Notification";



 // check if the form has been submitted. If it has, process the form and save it to the database if 
 
 if (isset($_POST['submit'])) 
 {  
 // confirm that the 'id' value is a valid integer before getting the form data 
 if (is_numeric($_POST['id'])) 
 { 
 // get form data, making sure it is valid 
 $id = $_POST['id']; 
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
 mysql_query("UPDATE nodo_tbl_notifications SET name='$name', event='$event', recipient='$recipient', subject='$subject', body='$body'  WHERE id='$id' AND user_id='$userId'") or die(mysql_error());   
 // once saved, redirect back to the view page 
 header("Location: notifications.php#saved");  
 } 
 } 
 else {
 mysql_select_db($database_tc, $tc);
 $id = $_GET['id']; $result = mysql_query("SELECT * FROM nodo_tbl_notifications WHERE id=$id AND user_id='$userId'") or die(mysql_error());  
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

	<form action="notifications_edit.php" data-ajax="false" method="post"> 
	
	 
		
		<input type="hidden" name="id" id="id" value="<?php echo $row['id'] ;?>"  />
	
	<br>
	<br>
	<label for="name">Name: </label>
		<input type="text" name="name" id="name" value="<?php echo $row['name'] ;?>"  />
		<br>
		<label for="name">Event: (Example: userevent 100,0 | newkaku 1,on)</label>
		<input type="text" name="event" id="event" value="<?php echo $row['event'] ;?>"  />
		<br>
		<label for="name">Recipient:</label>
		<input type="text" name="recipient" id="recipient" value="<?php echo $row['recipient'] ;?>"  />
		<br>
		<label for="name">Subject:</label>
		<input type="text" name="subject" id="subject" value="<?php echo $row['subject'] ;?>"  />
		<br>
		<label for="body">Body:</label>
		<textarea name="body" id="body"><?php echo $row['body'] ;?></textarea>
		<br>		
	
	    <br>
         <a href="notifications.php" data-role="button" data-ajax="false">Cancel</a> 
		<input type="submit" name="submit" value="Save" >

		
	
	</form> 

		
	</div><!-- /content -->
	
	<?php require_once('../include/footer_admin.php'); ?>
	
</div><!-- /page -->

 
</body>
</html>

