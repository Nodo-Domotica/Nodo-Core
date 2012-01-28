
<?php

require_once('../connections/tc.php');
require_once('../include/auth.php');  


if (!$tc)
  {
  die('Could not connect: ' . mysql_error());
  }

$naam = mysql_real_escape_string(htmlspecialchars($_POST['naam'])); 
$address = mysql_real_escape_string(htmlspecialchars($_POST['address']));  
$type = mysql_real_escape_string(htmlspecialchars($_POST['type']));
 
  //Als de schakelaar een newkaku schakelaar is dan is homecode 0
 if ($type == 2) {
	$homecode = 0; 
 }
 else {
 	$homecode = mysql_real_escape_string(htmlspecialchars($_POST['homecode']));
 }
 
 $dim = mysql_real_escape_string(htmlspecialchars($_POST['dim']));  
  
mysql_select_db($database_tc, $tc);



$sql="INSERT INTO nodo_tbl_devices(naam, type, dim, homecode, address, user_id)
VALUES
('$naam','$type','$dim','$homecode','$address','$userId')";
//('test','test','2','1','1','1')";


if (!mysql_query($sql,$tc))
  {
  die('Error: ' . mysql_error());
  }
$addGoTo = "switch_add_confirm.php";
  if (isset($_SERVER['QUERY_STRING'])) {
    $addGoTo .= (strpos($addGoTo, '?')) ? "&" : "?";
    $addGoTo .= $_SERVER['QUERY_STRING'];
  }
  header(sprintf("Location: %s", $addGoTo));
mysql_close($tc)
?> 
