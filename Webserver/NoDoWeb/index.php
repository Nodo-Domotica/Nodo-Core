<?php 


session_start();
if(!isset($_SESSION['userId'])) {
        //die('You need to be logged in!!!');
	header("Location: login.php");
} else {
    header("Location: devices.php");
}
?>