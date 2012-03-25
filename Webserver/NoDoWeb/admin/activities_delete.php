<?php 

require_once('../connections/tc.php');
require_once('../include/auth.php'); 

if (!function_exists("GetSQLValueString")) {
function GetSQLValueString($theValue, $theType, $theDefinedValue = "", $theNotDefinedValue = "") 
{
  $theValue = get_magic_quotes_gpc() ? stripslashes($theValue) : $theValue;

  $theValue = function_exists("mysql_real_escape_string") ? mysql_real_escape_string($theValue) : mysql_escape_string($theValue);

  switch ($theType) {
    case "text":
      $theValue = ($theValue != "") ? "'" . $theValue . "'" : "NULL";
      break;    
    case "long":
    case "int":
      $theValue = ($theValue != "") ? intval($theValue) : "NULL";
      break;
    case "double":
      $theValue = ($theValue != "") ? "'" . doubleval($theValue) . "'" : "NULL";
      break;
    case "date":
      $theValue = ($theValue != "") ? "'" . $theValue . "'" : "NULL";
      break;
    case "defined":
      $theValue = ($theValue != "") ? $theDefinedValue : $theNotDefinedValue;
      break;
  }
  return $theValue;
}
}

if ((isset($_GET['id'])) && ($_GET['id'] != "")) {
  
  $id = $_GET['id'];
  
  //Alle records groter dan het te verwijderen record bepalen
  $result = mysql_query("SELECT * FROM nodo_tbl_activities WHERE user_id='$userId' AND sort_order > (SELECT sort_order FROM nodo_tbl_activities WHERE user_id='$userId' AND id='$id')") or die(mysql_error());
  
  //Van alle records groter dan het verwijderde record het sorteer veld met één verlagen   
  while($row = mysql_fetch_array($result)) {
  
  $id = $row['id'];
  $sort_number = $row['sort_order'] - 1;
  
  // save the data to the database 
 mysql_select_db($database_tc, $tc);
 mysql_query("UPDATE nodo_tbl_activities SET sort_order='$sort_number' WHERE id='$id' AND user_id='$userId'") or die(mysql_error());   
 
 }
  
  
  
  
  $deleteSQL = sprintf("DELETE FROM nodo_tbl_activities WHERE id=%s AND user_id='$userId'",
                       GetSQLValueString($_GET['id'], "int"));

  mysql_select_db($database_tc, $tc);
  $Result1 = mysql_query($deleteSQL, $tc) or die(mysql_error());

  $deleteGoTo = "activities.php";
  if (isset($_SERVER['QUERY_STRING'])) {
    $deleteGoTo .= (strpos($deleteGoTo, '?')) ? "&" : "?";
    $deleteGoTo .= $_SERVER['QUERY_STRING'];
  }
  header(sprintf("Location: %s", $deleteGoTo));
}
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>Untitled Document</title>
</head>

<body>
</body>
</html>
