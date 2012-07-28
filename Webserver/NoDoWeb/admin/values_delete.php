<?php
/***********************************************************************************************************************
"Nodo Web App" Copyright © 2012 Martin de Graaf

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*************************************************************************************************************************/


require_once('../connections/db_connection.php');
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
  
   mysql_select_db($database, $db); 
  //Alle records groter dan het te verwijderen record bepalen
  $result = mysql_query("SELECT * FROM nodo_tbl_sensor WHERE user_id='$userId' AND sort_order > (SELECT sort_order FROM nodo_tbl_sensor WHERE user_id='$userId' AND id='$id')") or die(mysql_error());
  
  //Van alle records groter dan het verwijderde record het sorteer veld met één verlagen   
  while($row = mysql_fetch_array($result)) {
  
  $id = $row['id'];
  $sort_number = $row['sort_order'] - 1;
  
  // save the data to the database 
 
 mysql_query("UPDATE nodo_tbl_sensor SET sort_order='$sort_number' WHERE id='$id' AND user_id='$userId'") or die(mysql_error());   
 
 
 
 
 
 }
  
  
  
  
  
  
  
  
  $delete_sensor_SQL = sprintf("DELETE FROM nodo_tbl_sensor WHERE id=%s AND user_id='$userId'", 
                       GetSQLValueString($_GET['id'], "int"));
 

  $delete_sensor_data_SQL = sprintf("DELETE FROM nodo_tbl_sensor_data WHERE sensor_id=%s", 
                       GetSQLValueString($_GET['id'], "int"));
					   
					   

  mysql_select_db($database, $db);
  $Result1 = mysql_query($delete_sensor_SQL, $db) or die(mysql_error());
  $Result2 = mysql_query($delete_sensor_data_SQL, $db) or die(mysql_error());

  $deleteGoTo = "values.php";
  if (isset($_SERVER['QUERY_STRING'])) {
    $deleteGoTo .= (strpos($deleteGoTo, '?')) ? "&" : "?";
    $deleteGoTo .= $_SERVER['QUERY_STRING'];
  }
  header(sprintf("Location: %s", $deleteGoTo));
}
?>
