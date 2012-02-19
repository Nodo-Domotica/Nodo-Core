<?php 
require_once('connections/tc.php');
require_once('include/auth.php');
require_once('include/settings.php'); 



$page_title = "Information";
 



?>


<!DOCTYPE html> 
<html> 

<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1"> 
	<title><?php echo $title ?></title> 
	<?php require_once('include/jquery_mobile.php'); ?>
	<?php require_once('include/send_event.php'); ?>
</head> 

<body> 

<div data-role="page" data-theme="<?php echo $theme?>">

<?php require_once('include/header.php'); ?>

	<div data-role="content">	
		<?php
		
          
		   
		  

mysql_select_db($database_tc, $tc);

$RSevent_log = mysql_query("SELECT * FROM (SELECT * FROM nodo_tbl_event_log WHERE user_id='$userId' ORDER BY id DESC LIMIT 25)x ORDER BY id") or die(mysql_error());
?>

<div data-role="collapsible" data-collapsed="true">
	<h3>Web App details</h3>
<b>Web App version: </b><?php echo $WEBAPP_VERSION;?><br>
<b>Nodo ID: </b><?php echo $nodo_id;?><br>
<b>Connected to Nodo: </b><?php echo $nodo_ip.":".$nodo_port;?><br>

</div>

	<div data-role="collapsible" data-collapsed="true">
	<h3>Eventlog</h3>


 <h3>Last 25 events</h3>	
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
	<br>
	
	<a href="export_csv.php" data-role="button" data-inline="true" data-ajax="false">Export all events to csv</a>
	</div>	

	
	</div><!-- /content -->
	
	<?php require_once('include/footer.php'); ?>
	
</div><!-- /page -->

</body>
</html>