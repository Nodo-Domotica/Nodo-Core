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


require_once('../connections/tc.php'); 
require_once('../include/auth.php');
require_once('../include/user_settings.php');


$page_title = "Setup: Scripts";	


/************************************************************************************************
HTTPRequest function (do not output http headers)													
*************************************************************************************************/
function HTTPRequest($Url){


    
    if (!function_exists('curl_init')){
        die('Sorry cURL is not installed!');
    }
 
    global $nodo_port;
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, $Url);
    curl_setopt($ch, CURLOPT_USERAGENT, "NoDoWeb");
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



/************************************************************************************************
Eventlist script read															
*************************************************************************************************/
if (isset($_POST['Read'])) 
{  


	if ($_POST['select-script-1'] == "EVENTLST") {
	
		$file = "EVENTLST"; 

		//Write eventlist on nodo
			
		HTTPRequest("http://$nodo_ip/?id=$nodo_id&password=$nodo_password&event=EventListfile%20$file");
		
	}
	else {
	
		
		$file = $_POST['select-script-1'];

	}

	//Read file from Nodo to array
	$script = explode("\n", HTTPRequest("http://$nodo_ip/?id=$nodo_id&password=$nodo_password&file=$file"));
			
	$total_script_lines = count($script);
	
    
 
 
}
/************************************************************************************************
END script Read															
*************************************************************************************************/


/************************************************************************************************
Script write or execute														
*************************************************************************************************/
if (isset($_POST['Write'])) 
{  

 
 // get form data, making sure it is valid 
 $file = mysql_real_escape_string(htmlspecialchars($_POST['select-script-1'])); 
 $scriptpost = mysql_real_escape_string(htmlspecialchars($_POST['script']));
 
 
 
 mysql_select_db($database_tc, $tc);
 $result = mysql_query("SELECT * FROM nodo_tbl_scripts WHERE user_id='$userId' AND file = '$file'") or die(mysql_error());  
 
	 if (mysql_affected_rows()==0) {
		// update the data to the database 
		 mysql_query("INSERT INTO nodo_tbl_scripts (script, file, user_id) 
		 VALUES 
		 ('$scriptpost','$file','$userId')");
		 
		     
	 
	 }
 
 
	else {

		// save the data to the database 
		 mysql_select_db($database_tc, $tc);
		 mysql_query("UPDATE nodo_tbl_scripts SET script='$scriptpost' WHERE user_id='$userId' AND file='$file'") or die(mysql_error());   
		  
		 
		
	}
			
		
	//Save script on Nodo 
	HTTPRequest("http://$nodo_ip/?id=$nodo_id&password=$nodo_password&event=FileGetHTTP%20$file");
	
	
	if (ISSET($_POST["checkbox-2"])){
	
		//Execute script on Nodo
		HTTPRequest("http://$nodo_ip/?id=$nodo_id&password=$nodo_password&event=FileExecute%20$file");
		
	}
	
	
	
	header("Location: scripts.php?file=$file#saved");
	
	
}
/************************************************************************************************
END Script write													
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

<div data-role="page" pageid="main" data-theme="<?php echo $theme?>">
 
	<?php require_once('../include/header_admin.php'); ?>
 
	<div data-role="content">	

	<form action="scripts.php" data-ajax="false" method="post"> 
	
	 
				
	<br>
	
		<label for="select-script" class="select">Choose script:</label>
		<select name="select-script-1" id="select-script-1" data-native-menu="false">
		<option value="EVENTLST"<?php if (isset($_POST['select-script-1']) || isset($_GET['file']) ) {if ($_POST['select-script-1'] == "EVENTLST" || $_GET['file'] == "EVENTLST") {echo 'Selected="Selected"';}}?>>Eventlist</option>
		<option value="1"<?php if (isset($_POST['select-script-1']) || isset($_GET['file']) ) {if ($_POST['select-script-1'] == "1" || $_GET['file'] == "1") {echo 'Selected="Selected"';}}?>>Script 1</option>
		<option value="2"<?php if (isset($_POST['select-script-1']) || isset($_GET['file']) ) {if ($_POST['select-script-1'] == "2" || $_GET['file'] == "2") {echo 'Selected="Selected"';}}?>>Script 2</option>
		<option value="3"<?php if (isset($_POST['select-script-1']) || isset($_GET['file']) ) {if ($_POST['select-script-1'] == "3" || $_GET['file'] == "3") {echo 'Selected="Selected"';}}?>>Script 3</option>
		<option value="4"<?php if (isset($_POST['select-script-1']) || isset($_GET['file']) ) {if ($_POST['select-script-1'] == "4" || $_GET['file'] == "4") {echo 'Selected="Selected"';}}?>>Script 4</option>
		<option value="5"<?php if (isset($_POST['select-script-1']) || isset($_GET['file']) ) {if ($_POST['select-script-1'] == "5" || $_GET['file'] == "5") {echo 'Selected="Selected"';}}?>>Script 5</option>
		</select>
		
		
		<input type="submit" name="Read" value="Read" >
		

		
		
		
		<label for="script">Script:</label>
		<textarea name="script" id="script"><?php 
		
	

if (isset($script)){  

	for($i=0;$i<$total_script_lines;$i++){
		
			
			//<br /> aan het einde van de regels verwijderen
			$script[$i] = str_replace("<br />","",$script[$i]);
			
			echo $script[$i];
 
		
	}
	
	
}

?>
</textarea>
<br>		
		
    	
		<input type="checkbox" name="checkbox-2" id="checkbox-2" class="custom" checked="yes"/>
		<label for="checkbox-2">Execute after write</label>
		
		<br>
        
		<input type="submit" name="Write" value="Write" > 
			

	
	</form> 
	
	
	
		
	
	</div><!-- /content -->
	
	<?php require_once('../include/footer_admin.php'); ?>
	
</div><!-- /main page -->

<!-- Start of saved page: -->
<div data-role="dialog" id="saved" data-theme="<?php echo $theme?>">

	<div data-role="header" data-theme="<?php echo $theme_header?>">
		<h1><?php echo $page_title?></h1>
	</div><!-- /header -->

	<div data-role="content">	
		<h2>Script sent to Nodo.</h2>
				
		<p><a href="scripts.php<?php if (isset($_GET['file'])) {echo "?file=" . $_GET['file'];} ?>" data-role="button" data-inline="true" data-icon="back">Ok</a></p>	
	
	
	</div><!-- /content -->
	
	
</div><!-- /page saved -->
 <script>

</script>
</body>
</html>
