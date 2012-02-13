<?php 

require_once('../connections/tc.php'); 
require_once('../include/auth.php');
require_once('../include/settings.php');

$page_title = "Setup: Scripts";	


/************************************************************************************************
HTTPRequest function														
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
    curl_setopt($ch, CURLOPT_TIMEOUT, 10);
 
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


	if ($_POST['select-script-1'] == 0) {
	
		$file = "wasevent"; 

		//Write eventlist on nodo
			
		HTTPRequest("http://$nodo_ip/?id=$nodo_id&password=$nodo_password&event=EventListfile%20$file");
		
	}
	else {
	
		$scriptnr = $_POST['select-script-1'];
		$file = "was00$scriptnr";

	}

	//Read eventlist to array
	$scriptraw = explode("\n", HTTPRequest("http://$nodo_ip/?id=$nodo_id&password=$nodo_password&file=$file"));
		
	//count total lines in $scriptraw
	$total_script_lines_raw = count($scriptraw);
	
	$x=0;
	
	for($i=0;$i<$total_script_lines_raw;$i++){
		
		//Remove !********************************** start en stop lines 
		$pos = strpos($scriptraw[$i],"!*****");
        
		if($pos === false || $pos > 0) {
			$script[$x] = $scriptraw[$i];
			$x++;
		}
	}
	
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


if ($_POST['select-script-1'] == 0) {
		
		$scriptnr = $_POST['select-script-1'];
		$file = "wasevent"; 
					
		
	}
	else {
	
		$scriptnr = $_POST['select-script-1'];
		$file = "was00$scriptnr";

	}
 
 
 // get form data, making sure it is valid 
 $script_id = mysql_real_escape_string(htmlspecialchars($_POST['select-script-1'])); 
 $scriptpost = mysql_real_escape_string(htmlspecialchars($_POST['script']));   

 
 
 mysql_select_db($database_tc, $tc);
 $result = mysql_query("SELECT * FROM nodo_tbl_scripts WHERE user_id='$userId' AND script_id = '$script_id'") or die(mysql_error());  
 
	 if (mysql_affected_rows()==0) {
	 
		 mysql_query("INSERT INTO nodo_tbl_scripts (script, script_id, user_id) 
		 VALUES 
		 ('$scriptpost','$script_id','$userId')");
		 // once saved, redirect back to the view page 
		     
	 
	 }
 
 
	else {

		// save the data to the database 
		 mysql_select_db($database_tc, $tc);
		 mysql_query("UPDATE NODO_tbl_scripts SET script='$scriptpost' WHERE user_id='$userId' AND script_id='$script_id'") or die(mysql_error());   
		 // once saved, redirect back to the view page 
		 
		
	}
	
	
	
		
	//delete current file on Nodo
	//HTTPRequest("http://$nodo_ip/?id=$nodo_id&password=$nodo_password&event=FileErase%20$file");	
		
	//Save script on Nodo 
	HTTPRequest("http://$nodo_ip/?id=$nodo_id&password=$nodo_password&event=FileGetHTTP%20$scriptnr");
	
	
	if (ISSET($_POST["checkbox-2"])){
	
		//Execute script on Nodo
		HTTPRequest("http://$nodo_ip/?id=$nodo_id&password=$nodo_password&event=FileExecute%20$file");
		
		}
	
	
	
	//header("Location: scripts.php#saved");
	
	
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

<div data-role="page" pageid="main">
 
	<?php require_once('../include/header_admin.php'); ?>
 
	<div data-role="content">	

	<form action="scripts.php" data-ajax="false" method="post"> 
	
	 
				
	<br>
	
		<label for="select-script" class="select">Choose script:</label>
		<select name="select-script-1" id="select-script-1">
		<option value="0" <?php if (isset($_POST['select-script-1'])) {if ($_POST['select-script-1'] == 0) {echo 'Selected="Selected"';}}?>>Eventlist</option>
		<option value="1"<?php if (isset($_POST['select-script-1'])) {if ($_POST['select-script-1'] == 1) {echo 'Selected="Selected"';}}?>>Script 1</option>
		<option value="2"<?php if (isset($_POST['select-script-1'])) {if ($_POST['select-script-1'] == 2) {echo 'Selected="Selected"';}}?>>Script 2</option>
		<option value="3"<?php if (isset($_POST['select-script-1'])) {if ($_POST['select-script-1'] == 3) {echo 'Selected="Selected"';}}?>>Script 3</option>
		<option value="4"<?php if (isset($_POST['select-script-1'])) {if ($_POST['select-script-1'] == 4) {echo 'Selected="Selected"';}}?>>Script 4</option>
		<option value="5"<?php if (isset($_POST['select-script-1'])) {if ($_POST['select-script-1'] == 5) {echo 'Selected="Selected"';}}?>>Script 5</option>
		</select>
		
		
		<input type="submit" name="Read" value="Read" >
		

		
		
		
		<label for="script">Script:</label>
		<textarea name="script" id="script"><?php 
		
		

if (isset($script)){  

	for($i=0;$i<$total_script_lines;$i++){
		//Alleen als de regel met EventList begint behandelen we de regel.
		//if(strstr($script[$i],"EventList ")){ 
			
			//<br /> aan het einde van de regels verwijderen
			$script[$i] = str_replace("<br />","",$script[$i]);
			//Eventlist [nr]; aan het begin van de regels verwijderen
			//$script[$i] = substr($script[$i],strpos($script[$i],";")+2);
			echo $script[$i];
 
		//}
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
<div data-role="dialog" id="saved">

	<div data-role="header">
		<h1><?php echo $page_title?></h1>
	</div><!-- /header -->

	<div data-role="content">	
		<h2>Script saved.</h2>
				
		<p><a href="scripts.php" data-role="button" data-inline="true" data-icon="back">Ok</a></p>	
	
	
	</div><!-- /content -->
	
	
</div><!-- /page saved -->
 
</body>
</html>
