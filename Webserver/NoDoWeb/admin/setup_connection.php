<?php 

require_once('../connections/tc.php'); 
require_once('../include/auth.php');
require_once('../include/settings.php');

$page_title = "Setup: Communication";	

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
    curl_setopt($ch, CURLOPT_HEADER, 1);
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



if (isset($_POST['save'])) 
{  
 
 // get form data, making sure it is valid 
 $nodo_ip = mysql_real_escape_string(htmlspecialchars($_POST['nodo_ip'])); 
 $nodo_port = mysql_real_escape_string(htmlspecialchars($_POST['nodo_port']));  
 $send_method = mysql_real_escape_string(htmlspecialchars($_POST['send_method']));
 $nodo_password = mysql_real_escape_string(htmlspecialchars($_POST['nodo_password']));
 

/************************************************************************************************
Generate NoDo ID													
*************************************************************************************************/ 
//Only generate ID when ID in database is empty
if ($nodo_id == "")  { 
   
   
	//ID Lenght
	$unique_ref_length = 8;


	$unique_ref_found = false;

	
	$possible_chars = "1234567890ABCDEFGHIJKLMNPQRSTUVW";

	//Until we find a unique id
	while (!$unique_ref_found) {

	
		$unique_ref = "";

		
		$i = 0;

	
		while ($i < $unique_ref_length) {

			//Get random character from $possible_chars
			$char = substr($possible_chars, mt_rand(0, strlen($possible_chars)-1), 1);

			$unique_ref .= $char;

			$i++;

		}

	
		//ID generated. Check if ID exists in Database
		mysql_select_db($database_tc, $tc);
		$result = mysql_query("SELECT 'nodo_id' FROM NODO_tbl_users WHERE nodo_id='$unique_ref'") or die(mysql_error());  
		$row = mysql_fetch_array($result);
		
		if (mysql_num_rows($result)==0) {

			//We have a unique ID
			$unique_ref_found = true;

		}
	
	
	}

$nodo_id = $unique_ref;
 }  
   
 else{ 
   
  $nodo_id = mysql_real_escape_string(htmlspecialchars($_POST['nodo_id'])); 
  
}
/************************************************************************************************
END Generate NoDo ID													
*************************************************************************************************/ 
   
/************************************************************************************************
Check connection & Nodo config													
*************************************************************************************************/ 
if (isset($_POST['auto_config'])){
	 //Check connection on default NODO and split headers
     $headers = explode( "\n", HTTPRequest("http://$nodo_ip/?event=userevent%20255,255&password=Nodo&id=00000000") );
     }
	else
	{
	//Check connection and split headers
     $headers = explode( "\n", HTTPRequest("http://$nodo_ip/?event=userevent%20255,255&password=$nodo_password&id=$nodo_id") );
      }   
     
    

	
	//Default response
	$response = "not_ok";
	
	//We have connection with NoDo server
	if (strpos($headers[0], 'HTTP/1.1 200 Ok') !== false && strpos($headers[3], 'Nodo/3') !== false) {
		
		$response = "ok";
		
		if (isset($_POST['auto_config'])){
		
			//Configure NoDo
			HTTPRequest("http://$nodo_ip/?event=HTTPHost%20nodo2.powerkite.nl/events.php&password=Nodo&id=00000000");
			HTTPRequest("http://$nodo_ip/?event=Filewrite%20waconfig&password=Nodo&id=00000000");
			HTTPRequest("http://$nodo_ip/?event=ok&password=Nodo&id=00000000");
			HTTPRequest("http://$nodo_ip/?event=Filewrite&password=Nodo&id=00000000");
			HTTPRequest("http://$nodo_ip/?event=OutputIp%20HTTP;id%20$nodo_id;password%20$nodo_password&password=Nodo&id=00000000");
			
			
			
			if (strpos(HTTPRequest("http://$nodo_ip/?id=$nodo_id&password=$nodo_password&file=waconfig"), 'ok') !== false) {
			
				HTTPRequest("http://$nodo_ip/?event=Fileerase waconfig&password=$nodo_password&id=$nodo_id");
				HTTPRequest("http://$nodo_ip/?event=reboot&password=$nodo_password&id=$nodo_id");				
				$response = "ok_config";	
			
			}
			else{
				
				$response = "error_config";
			
			}
						
		}
	}
	
	if (strpos($headers[0], 'HTTP/1.1 403 Forbidden') !== false && strpos($headers[3], 'Nodo/3') !== false) {
		
		if (isset($_POST['auto_config'])){
		
		$response = "forbidden_config";
		}
		else{
		$response = "forbidden";
		}
		
		
	}
  
 // save the data to the database 
 mysql_select_db($database_tc, $tc);
 mysql_query("UPDATE NODO_tbl_users SET nodo_ip='$nodo_ip', nodo_port='$nodo_port', send_method='$send_method', nodo_password='$nodo_password', nodo_id='$nodo_id' WHERE id='$userId'") or die(mysql_error());   
 // once saved, redirect back to the view page 
 header("Location: setup_connection.php/?response=$response#saved");   
 }
 
else 
{
mysql_select_db($database_tc, $tc);
$result = mysql_query("SELECT * FROM nodo_tbl_users WHERE id='$userId'") or die(mysql_error());  
$row = mysql_fetch_array($result);
}
/************************************************************************************************
END Check connection & Nodo config													
*************************************************************************************************/ 	

?>




<!DOCTYPE html> 
<html> 
 
<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1"> 
	<title><?php echo $title; ?></title> 
	<?php require_once('../include/jquery_mobile.php'); ?>
</head> 
 
<body> 

<!-- Start of main page: -->

<div data-role="page" pageid="main" >
 
	<?php require_once('../include/header_admin.php'); ?>
 
	<div data-role="content">	

	<form action="setup_connection.php" data-ajax="false" method="post"> 
	
	 
		<input type="hidden" name="send_method" id="send_method" value="2" />

		<!--		
		Apop overbodig in webapp?
		
		<label for="sendmethod" class="select">Send NoDo commands through:</label>
		    <select name="send_method" id="send_method" data-placeholder="true" data-native-menu="false">
				<option value="1" <?php if ($row['send_method'] == 1) {echo 'selected="selected"';}?>>Network Event client (APOP)</option>
				<option value="2" <?php if ($row['send_method'] == 2 || $row['send_method'] == 0 ) {echo 'selected="selected"';}?>>HTTP</option>
		    </select> 
	    -->
	
	<br>
	
		<label for="name">NoDo ip/host: (x.x.x.x)</label>
		<input type="text" name="nodo_ip" id="nodo_ip" value="<?php echo $row['nodo_ip']?>"  />
	
	<br>   
      
		<label for="name">TCP port:</label>
		<input type="text" name="nodo_port" id="nodo_port" value="<?php echo $row['nodo_port']?>"  />

			
	<br>
      
		<label for="name">NoDo password:</label>
		<input type="password" name="nodo_password" id="nodo_password" value="<?php echo $row['nodo_password']?>"  />
		
	<br>
	   	   	
		<!-- <input type="checkbox" name="checkbox-1" id="checkbox-0" class="custom" />
		<label for="checkbox-0">Generate NoDo ID</label>
		 -->
		 <input type="checkbox" name="auto_config" id="auto_config_2" class="custom" />
		<label for="auto_config_2">Auto configure default NoDo</label>
	 
	 <br>  
      
		<label for="name">Nodo ID: <b><?php echo $row['nodo_id']?></b></label>
		<input type="hidden" name="nodo_id" id="nodo_id" value="<?php echo $row['nodo_id']?>"  />
    
	<br><br>
        
		<input type="submit" name="save" value="Save & check connection" >

		
	
	</form> 

	

	
	</div><!-- /content -->
	
	<?php include('../include/footer_admin.php'); ?>
	
</div><!-- /main page -->

<!-- Start of saved dialog: -->
<div data-role="dialog" id="saved">

	<div data-role="header">
		<h1><?php echo $page_title;?></h1>
	</div><!-- /header -->

	<div data-role="content">	
		<h2>Settings saved.</h2>
			
			<h2>
			<?php
			switch ($_GET['response'])
			{
			case 'not_ok':
			echo "Connection failed!";
			break;
			case 'ok':
			echo "Connection succeeded!";
			break;
			case 'ok_config':
			echo "Connection succeeded!<br>NoDo configured ok";
			break;
			case 'error_config':
			echo "Connection succeeded!<br>Something went wrong configuring Nodo";
			break;
			case 'forbidden':
			echo "Authentication failed!";
			break;
			case 'forbidden_config':
			echo "Cannot configure NoDo!<BR>Make sure your Nodo has default settings.";
			break;
			default:
			echo "Connection failed!";
			}	
			?>	
			</h2>
		<p><a href="#main" data-rel="back" data-role="button" data-inline="true" data-icon="back">Ok</a></p>	
	</div><!-- /content -->
	
	
</div><!-- /dialog saved -->

 
</body>
</html>