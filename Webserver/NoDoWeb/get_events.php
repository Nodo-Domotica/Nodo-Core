<?php 

require_once('connections/tc.php'); 
require_once('include/auth.php');
require_once('include/user_settings.php');

          
		   
mysql_select_db($database_tc, $tc);

$RSevent_log = mysql_query("SELECT * FROM (SELECT * FROM nodo_tbl_event_log WHERE user_id='$userId' ORDER BY id DESC LIMIT 25)x ORDER BY id") or die(mysql_error());
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
		while($row = mysql_fetch_array($RSevent_log)) 
		{                                
?>		 
		<tr>	
		<td width="50"><?php echo $row['nodo_unit_nr'];?></td> <td width="200"><?php echo $row['event'];?></td><td><?php echo $row['timestamp'];?></td>		
		</tr>
<?php		
		}         
		?>
</tbody>
	</table>
