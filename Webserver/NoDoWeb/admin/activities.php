<?php require_once('../connections/tc.php'); 
require_once('../include/auth.php'); 
// check if the form has been submitted. If it has, process the form and save it to the database if 

if (isset($_POST['submit'])) 
{  
 
 // get form data, making sure it is valid 
 $host = mysql_real_escape_string(htmlspecialchars($_POST['host'])); 
 $port = mysql_real_escape_string(htmlspecialchars($_POST['port']));  
 $howto_send_command = mysql_real_escape_string(htmlspecialchars($_POST['howto_send_command']));
 $password = mysql_real_escape_string(htmlspecialchars($_POST['password']));
 
 
  
 // save the data to the database 
 mysql_select_db($database_tc, $tc);
 mysql_query("UPDATE nodo_tbl_activities SET host='$host', port='$port', howto_send_command='$howto_send_command', password='$password' WHERE user_id='$userId'") or die(mysql_error());   
 // once saved, redirect back to the view page 
 header("Location: setup_connection.php#saved");   }
 
else 
{
mysql_select_db($database_tc, $tc);
$result = mysql_query("SELECT * FROM NODO_tbl_setup WHERE user_id='$userId'") or die(mysql_error());  
$row = mysql_fetch_array($result);
}?>




<!DOCTYPE html> 
<html> 
 
<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1"> 
	<title>Setup activiteiten</title> 
	<link rel="stylesheet" href="http://code.jquery.com/mobile/1.0rc2/jquery.mobile-1.0rc2.min.css" />
	<script src="http://code.jquery.com/jquery-1.6.4.min.js"></script>
	<script src="http://code.jquery.com/mobile/1.0rc2/jquery.mobile-1.0rc2.min.js"></script>
</head> 
 
<body> 

<!-- Start of main page: -->

<div data-role="page" pageid="main">
 
	<div data-role="header">
		<h1>Setup activiteiten</h1>
		<a href="/admin/index.php" data-icon="gear" class="ui-btn-right" data-ajax="false">Setup</a>
		<a href="/index.php" data-icon="home" class="ui-btn-left" data-ajax="false" data-iconpos="notext">Home</a>
	
	</div><!-- /header -->
 
	<div data-role="content">	

	!!!!!Nog uitwerken!!!!!!!
	
	<form action="activities.php" data-ajax="false" method="post"> 
	
	 
				
	<br>
	
		<label for="name">Naam: </label>
		<input type="text" name="name" id="name" value=""  />
		<br>
		<label for="name">Event(s): (voorbeeld: sendkaku a1,on;sendkaku a2,on)</label>
		<input type="text" name="event" id="event" value=""  />
	
	   
        
		<input type="submit" name="submit" value="Opslaan" disabled="disabled">

		
	
	</form> 

	

	
	</div><!-- /content -->
	
	<div data-role="footer">
		<h4></h4>
	</div><!-- /footer -->
	
</div><!-- /main page -->

<!-- Start of saved page: -->
<div data-role="page" id="saved">

	<div data-role="header">
		<h1>Communicatie</h1>
	</div><!-- /header -->

	<div data-role="content">	
		<h2> De wijzigingen zijn opgeslagen.</h2>
				
		<p><a href="#main" data-rel="back" data-role="button" data-inline="true" data-icon="back">Ok</a></p>	
	</div><!-- /content -->
	
	<div data-role="footer">
		<h4></h4>
	</div><!-- /footer -->
</div><!-- /page saved -->
 
</body>
</html>

