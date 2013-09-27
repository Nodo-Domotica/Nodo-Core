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
		
	Get_Command_Result(command);
		  
    return false;
	});
});	 
	 
		  
</script>
<script src="js/setup_commands.js"></script>
</body>
</html>





