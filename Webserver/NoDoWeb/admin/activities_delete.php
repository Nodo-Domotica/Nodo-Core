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

require_once('../connections/tc.php');
require_once('../include/auth.php'); 


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
  
  
  
  
  $deleteSQL = ("DELETE FROM nodo_tbl_activities WHERE id='$id' AND user_id='$userId'");

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

