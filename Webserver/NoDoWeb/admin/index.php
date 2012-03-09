<?php 

require_once('../connections/tc.php'); 
require_once('../include/auth.php');
require_once('../include/user_settings.php');
$page_title = "Setup"; 

?>

<!DOCTYPE html> 
<html> 

<head>
	
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1"> 
	<title>Setup</title> 
	<?php require_once('../include/jquery_mobile.php'); ?>
	
</head> 

<body> 

<div data-role="page">

	<?php require_once('../include/header_admin.php'); ?>

	<div data-role="content">	
	  <p><a href="setup_connection.php" data-role="button" data-ajax="false">Communication</a></p>
	  <p><a href="devices.php" data-role="button" data-ajax="false">Devices</a></p>
	  <p><a href="activities.php" data-role="button" data-ajax="false">Activities</a></p>
	  <p><a href="values.php" data-role="button" data-ajax="false">Values</a></p>
	  <p><a href="scripts.php" data-role="button" data-ajax="false">Scripts</a></p>
	  <p><a href="notifications.php" data-role="button" data-ajax="false">Notifications</a></p>
	  <p><a href="personal.php" data-role="button" data-ajax="false">Personal</a></p>
	</div><!-- /content -->
	
	<?php require_once('../include/footer_admin.php'); ?>
	
</div><!-- /page -->

</body>
</html>