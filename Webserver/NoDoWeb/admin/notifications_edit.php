<?php 

require_once('../connections/tc.php'); 
require_once('../include/auth.php'); 
require_once('../include/settings.php'); 

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
 $userevent = mysql_real_escape_string(htmlspecialchars($_POST['userevent'])); 
 $recipient = mysql_real_escape_string(htmlspecialchars($_POST['recipient']));
 $subject = mysql_real_escape_string(htmlspecialchars($_POST['subject'])); 
 $body = mysql_real_escape_string(htmlspecialchars($_POST['body']));  
   
 

 
  
 // save the data to the database 
 mysql_select_db($database_tc, $tc);
 mysql_query("UPDATE NODO_tbl_notifications SET name='$name', userevent='$userevent', recipient='$recipient', subject='$subject', body='$body'  WHERE id='$id' AND user_id='$userId'") or die(mysql_error());   
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

 
<div data-role="page">
 
	<?php require_once('../include/header_admin.php'); ?>
 
	<div data-role="content">	

	<form action="notifications_edit.php" data-ajax="false" method="post"> 
	
	 
		
		<input type="hidden" name="id" id="id" value="<?php echo $row['id'] ;?>"  />
	
	<br>
	<br>
	<label for="name">Name: </label>
		<input type="text" name="name" id="name" value="<?php echo $row['name'] ;?>"  />
		<br>
		<label for="name">Userevent: (Example: 100,0 | 100,100)</label>
		<input type="text" name="userevent" id="userevent" value="<?php echo $row['userevent'] ;?>"  />
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
        
		<input type="submit" name="submit" value="Save" >

		
	
	</form> 

		
	</div><!-- /content -->
	
	<?php require_once('../include/footer_admin.php'); ?>
	
</div><!-- /page -->

 
</body>
</html>

