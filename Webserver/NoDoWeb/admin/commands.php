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
require_once('../include/webapp_settings.php');


$page_title = "Setup: Commands";

/************************************************************************************************
HTTPRequest function do not output http headers													
*************************************************************************************************/
function HTTPRequest($Url){


    
    if (!function_exists('curl_init')){
        die('Sorry cURL is not installed!');
    }
 
    global $nodo_port;
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, $Url);
    curl_setopt($ch, CURLOPT_USERAGENT, "Nodo WebApp");
    curl_setopt($ch, CURLOPT_HEADER, 0);
	curl_setopt($ch, CURLOPT_PORT, $nodo_port);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_TIMEOUT, 5);
 
    $output = curl_exec($ch);
    curl_close($ch);
	
	return $output;
	
	}
/************************************************************************************************
END HTTPRequest function														
*************************************************************************************************/	

?>

<!DOCTYPE html> 
<html> 
 
<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1"> 
	<title><?php echo $title ?></title> 
	<?php require_once('../include/jquery_mobile.php'); ?>
	
		
</head> 
 
<body> 

<!-- Start of main page: -->

<div data-role="page" data-theme="<?php echo $theme?>">
 
	<?php require_once('../include/header_admin.php'); ?>
 
	<div data-role="content">	
	
	
	
	<form action="commands.php" id="command_form" data-ajax="false" method="post"> 
	
	 
				
	<br \>
		<label for="select-choice-0" class="select">Predefined commands:</label>
		<select name="def_command" id="def_command" data-native-menu="false">
		    <option value="">Select command</option>
			<option value="">Do nothing</option>
		    <option value="1">Sync Nodo clock</option>
			<option value="2">SimulateDay 1</option>
			<option value="3">SimulateDay 7</option>
			<option value="4">UserEvent 1,0</option>
			<option value="5">UserEvent 2,0</option>
			<option value="6">UserEvent 3,0</option>
			<option value="7">UserEvent 4,0</option>
			<option value="8">UserEvent 5,0</option>
			<option value="9">UserEvent 6,0</option>
			<option value="10">UserEvent 7,0</option>
			<option value="11">UserEvent 8,0</option>
			<option value="12">UserEvent 9,0</option>
			<option value="13">UserEvent 10,0</option>
			
			
		 </select>	
		<br \>
		<label for="name">Nodo command: (Example: userevent 1;sendkaku a1,on)</label>
		<input type="text" name="command" id="command" value=""  />
	
	    <br \>
        
		<input type="submit" name="submit" value="Execute" >

			
	</form> 
	<br \>
	<div data-role="collapsible" data-collapsed="false" data-content-theme="<?php echo $theme?>">
	<h4>Result:</h4>		
	<div id="result_div" style="font-family:monospace">
	</div>
	</div>
	
			
	</div><!-- /content -->
	
	<?php require_once('../include/footer_admin.php'); ?>
	
</div><!-- /main page -->
<script>
$(document).ready(function() {
     $('#command_form').submit(function() {
      			
	var command = $('#command').val();
	var defcommand = $('#def_command').val();
	
	Get_Command_Result(command,defcommand);
		  
    return false;
	});
});	 
	 
		  
</script>
<script src="js/commands.js"></script>
</body>
</html>





