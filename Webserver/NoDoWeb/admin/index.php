<?php 

require_once('../connections/tc.php'); 
require_once('../include/auth.php');
require_once('../include/settings.php');
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
	  <p><a href="sensor.php" data-role="button" data-ajax="false">Sensors</a></p>
   	  <p><a href="setup_nodoweb.php" data-role="button" data-ajax="false">Look & Feel</a></p>
	</div><!-- /content -->
	
	<?php require_once('../include/footer_admin.php'); ?>
	
</div><!-- /page -->

</body>
</html>