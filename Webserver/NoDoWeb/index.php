<?php 
require_once('connections/tc.php'); 



session_start();
if(!isset($_SESSION['userId'])) {
        //die('You need to be logged in!!!');
	header("Location: login.php");
} 

else {

require_once('include/auth.php');
require_once('include/user_settings.php');

$default_page = $row_RSsetup['default_page'];

	switch ($default_page) {

		case 1:
		header("Location: devices.php");
		break;
		case 2:
		header("Location: activities.php");
		break;
		case 3:
		header("Location: values.php");
		break;
		default:
		header("Location: devices.php");
		break;

	}

}
?>