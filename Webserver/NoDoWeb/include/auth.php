<?php 

//SQL injection prevention
//$_POST = array_map("mysql_real_escape_string", $_POST);
$_GET = array_map("mysql_real_escape_string", $_GET);
$_REQUEST = array_map("mysql_real_escape_string", $_REQUEST);


session_start();
if(!isset($_SESSION['userId'])) {
        //die('You need to be logged in!!!');
	header("Location: login.php");
} else {
    $userId=$_SESSION['userId'];
}
?>