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

$sensor_id = $_GET['id'];

mysql_select_db($database, $db);

$RSevent_log = mysql_query("SELECT data,timestamp FROM nodo_tbl_sensor_data WHERE user_id='$userId' AND sensor_id='$sensor_id'") or die(mysql_error());
    header("Content-Type: text/csv; charset=utf-8");
    header("Content-Disposition:attachment;filename=values.csv");
    print "Value, Timestamp\n";
	while($row = mysql_fetch_row($RSevent_log)) {
	print '"' . stripslashes(implode('","',$row)) . "\"\n";
    }
    exit;


?>