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

$page_title = "Setup"; 

?>

<!DOCTYPE html> 
<html> 

<head>
	
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1"> 
	<title>Setup</title> 
	<?php require_once('../include/jquery_mobile.php'); ?>
	
</head> 

<body> 

<div data-role="page" data-theme="<?php echo $theme?>">

	<?php require_once('../include/header_admin.php'); ?>

	<div data-role="content">

	  <p><a href="setup_connection.php" data-role="button" data-ajax="false">Communication</a></p>
	  <p><a href="devices.php" data-role="button" data-ajax="false">Devices</a></p>
	  <p><a href="activities.php" data-role="button" data-ajax="false">Activities</a></p>
	  <p><a href="values.php" data-role="button" data-ajax="false">Values</a></p>
	  <p><a href="scripts.php" data-role="button" data-ajax="false">Scripts</a></p>
	  <p><a href="commands.php" data-role="button" data-ajax="false">Commands</a></p>
	  <p><a href="notifications.php" data-role="button" data-ajax="false">Notifications</a></p>
	  <p><a href="personal.php" data-role="button" data-ajax="false">Personal</a></p>
	<?php
	if ($user_group == "admin") {
	?>
	
	<p><a href="../webapp_admin/user_list.php" data-role="button" data-ajax="false">User list*</a></p>
	<p>*Only visible for Nodo Web App administrators.</p>
	
	<?php
	}
	?>	
	
	
	</div><!-- /content -->
	
	<?php require_once('../include/footer_admin.php'); ?>
	
</div><!-- /page -->

</body>
</html>