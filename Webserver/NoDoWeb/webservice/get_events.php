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
require_once('../include/user_settings.php');

          
		   
mysql_select_db($database, $db);

$RSevent_log = mysql_query("SELECT nodo_unit_nr,event,timestamp FROM (SELECT * FROM nodo_tbl_event_log WHERE user_id='$userId' AND event NOT LIKE 'Message%' ORDER BY id DESC LIMIT 100)x ORDER BY id") or die(mysql_error());
?>

<table>    
   
 <thead>     
 <tr>      
 <th scope="col" align="left">Unit</th>      
 <th scope="col" align="left">Event</th>      
 <th scope="col" align="left">Timestamp</th>
 </tr>    
 </thead>   
 <tbody>

<?php		
		while($row = mysql_fetch_assoc($RSevent_log)) 
		{                                
?>		 
		<tr>	
		<td width="50"><?php echo $row['nodo_unit_nr'];?></td> <td width="40%"><?php echo $row['event'];?></td><td <td><?php echo $row['timestamp'];?></td>		
		</tr>
<?php		
		}         
		?>
</tbody>
	</table>
