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

require_once('connections/db_connection.php');
require_once('include/auth.php');
require_once('include/user_settings.php'); 
require_once('include/webapp_settings.php'); 

$page_title = "Information";
?>


<!DOCTYPE html> 
<html> 

<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1"> 
	<title><?php echo $title ?></title> 
	<?php require_once('include/jquery_mobile.php'); ?>
</head> 

<body> 

<div data-role="page" data-theme="<?php echo $theme?>">

<script src="js/get_nodo_status.js"></script>
<script src="js/get_events.js"></script>

<?php require_once('include/header.php'); ?>

	<div data-role="content">	
		<?php
		
          
mysql_select_db($database, $db);

$RSevent_log = mysql_query("SELECT * FROM (SELECT * FROM nodo_tbl_event_log WHERE user_id='$userId' ORDER BY id DESC LIMIT 25)x ORDER BY id") or die(mysql_error());
?>

<div data-role="collapsible" data-collapsed="false" data-content-theme="<?php echo $theme?>">
<h3>Web App details</h3>
<b>Web App version: </b><?php echo $WEBAPP_VERSION;?><br>
<b>Nodo ID: </b><?php echo $nodo_id;?><br>
<b>Nodo IP-address: </b><?php echo $nodo_ip.":".$nodo_port;?><br>
<b>Connection status: </b> <?php echo $heartbeat;?><br>
<b>Cookies received: </b> <?php echo $row_RSsetup['cookie_count'];?><br>
<b>Last Cookie received: </b> <?php echo $cookie_update;?><br>



</div>

	<div data-role="collapsible" data-collapsed="true" data-content-theme="<?php echo $theme?>">
	<h3>Eventlog</h3>


 <h3>Last 25 events</h3>	


    <div id="events_div">
	</div>


 	<br>
	<!-- <a href="javascript:Get_Nodo_Events();" data-role="button" data-ajax="false" data-inline="true" >Refresh events</a> -->
	<a href="export_csv.php" data-role="button" data-inline="true" data-ajax="false">Export all events to csv</a>
	</div>	
	
	<div data-role="collapsible" data-collapsed="true" data-content-theme="<?php echo $theme?>">
	<h3>Nodo status</h3>
	<div id="status_div" style="font-family:monospace">
	
	</div>
	
	<a href="javascript:Get_Nodo_Status();" data-role="button" data-ajax="false" data-inline="true" >Refresh status</a>
	

	</div>
	</div><!-- /content -->
	
	<?php require_once('include/footer.php'); ?>
<script type="text/javascript">
//Events bij openen weergeven
Get_Nodo_Events(); 

</script>
	
</div><!-- /page -->



</body>
</html>