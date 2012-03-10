<?php

require_once('connections/tc.php'); 
require_once('include/webapp_settings.php');

$confirm_code=$_GET['passkey'];

mysql_select_db($database_tc, $tc);
$result = mysql_query("SELECT id,confirm_code FROM nodo_tbl_users WHERE confirm_code='$confirm_code' ") or die(mysql_error());  
$row = mysql_fetch_array($result);
$id = $row['id'];

	if($id > 0) {
	
		//Gegevens in de database opslaan
		//mysql_query("UPDATE nodo_tbl_activities SET name='$name', events='$events' WHERE id='$id' AND user_id='$userId'") or die(mysql_error());   
		mysql_select_db($database_tc, $tc);
		mysql_query("UPDATE nodo_tbl_users SET active='1',confirm_code='' WHERE id='$id'") or die(mysql_error());
		$message = "You account is confirmed<br>";
		$message.= "Click <a href=\"http://$WEBAPP_HOST/login.php\">here</a> to login";
    }
	else {
		
		$message = "Confirmation ID not found";
	}

?>






<!DOCTYPE html> 
<html>

<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1"> 
	<title>Nodo Web App</title> 
	<?php require_once('include/jquery_mobile.php'); ?>
</head> 

<body> 

<div data-role="page">

	<div data-role="header">
		<h1>Nodo Web App</h1>
	</div><!-- /header -->

	<div data-role="content">	
		<?php echo $message; ?>
				
		
	</div><!-- /content -->
	
	<div data-role="footer">
		<h4></h4>
	</div><!-- /footer -->
	
</div><!-- /page -->

</body>
</html>