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

$page_title = "Activities";


mysql_select_db($database, $db);
$RSactivities = mysql_query("SELECT * FROM nodo_tbl_activities WHERE user_id='$userId' ORDER BY sort_order ASC") or die(mysql_error());

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

<?php require_once('include/send_event.php'); ?>
	
	<style type="text/css">	
				
    .ui-btn-icon-left_collapsible .ui-btn-inner {padding-left: 40px !important;}
	
	.ui-btn-collapsible {
		
		text-align: left !important;
		margin: 0 -8px !important;
		margin-top: 0px !important;
		margin-right: -8px !important;
		margin-bottom: 8px !important;
		margin-left: -8px !important;
		-moz-border-radius: 1em !important;
		-webkit-border-radius: 1em !important;
		border-radius: 1em !important;
				
	}
	
	
	</style>

<?php require_once('include/header.php'); ?>

	<div data-role="content">	
<?php
		
          
		   
		  
		//Lees activiteiten uit
		while($row = mysql_fetch_array($RSactivities)) 
		{                                
	           
		echo "<a href=\"javascript:send_event(&quot;" . $row['events'] . "&quot;)\" data-role=\"button\" data-icon\"star\" class=\"ui-btn-collapsible ui-btn-icon-left_collapsible\"  data-icon=\"star\" >" . $row['name'] . "</a>";
		
		
		}         
?>
		

	</div><!-- /content -->
	
	<?php require_once('include/footer.php'); ?>
	
</div><!-- /page -->

</body>
</html>