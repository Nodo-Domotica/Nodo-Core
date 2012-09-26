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

mysql_select_db($database, $db);
$query = "SELECT * FROM nodo_tbl_users WHERE id='$userId'";
$recordset = mysql_query($query, $db) or die(mysql_error());
$row = mysql_fetch_array($recordset);
//Controleren of de Nodo maximaal 3 minuten geleden een connectie met de Web App heeft gehad.
if (strtotime($row['cookie_update']) >  (strtotime("now")-120)) {$online = "1";} else {$online = "0";}
        
             $rowsarray[] = array(
            "busy" 			=> $row['busy'],
			"online" 	=> $online);
        
 
$json = json_encode($rowsarray);

echo '{"nodostate":'. $json .'}'; 
 



?>