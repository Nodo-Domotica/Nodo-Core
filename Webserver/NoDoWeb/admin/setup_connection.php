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

$page_title = "Setup: Communication";



/************************************************************************************************
HTTPRequest function output http headers
*************************************************************************************************/
function HTTPRequest($Url){
    
    if (!function_exists('curl_init')){
        die('Sorry cURL is not installed!');
    }
 
    global $nodo_port;
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, $Url);
    curl_setopt($ch, CURLOPT_USERAGENT, "Nodo Web App");
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
		mysql_select_db($database, $db);
		$result = mysql_query("SELECT 'nodo_id' FROM nodo_tbl_users WHERE nodo_id='$unique_ref'") or die(mysql_error());  
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
	 //Check connection on default NODO and get headers
     $headers = (HTTPRequest("http://$nodo_ip/?event=status%20NodoIp") );
     }
	else
	{
	//Check connection and get headers
	
	 $key = md5($cookie.":".$nodo_password); //key opnieuw genereren omdat we het wachtwoord nog niet in de DB hebben opgelagen
     
	 $headers = (HTTPRequest("http://$nodo_ip/?event=status%20NodoIp&key=$key") );
      }   
     
    

	
	//Default response
	$response = "not_ok";
	
	//We have connection with NoDo server
	if (strpos($headers, 'HTTP/1.1 200 Ok') !== false && strpos($headers, 'Nodo/') !== false) {
		
		$response = "ok";
		
		if (isset($_POST['auto_config'])){
		
			
			//Configure NoDo
			HTTPRequest("http://$nodo_ip/?event=eventlistwrite;WildCard%20RF,All;SendEvent%20HTTP");
			HTTPRequest("http://$nodo_ip/?event=eventlistwrite;WildCard%20IR,All;SendEvent%20HTTP");
			HTTPRequest("http://$nodo_ip/?event=eventlistwrite;WildCard%20Variables,All;SendEvent%20HTTP");
			HTTPRequest("http://$nodo_ip/?event=eventlistwrite;WildCard%20Wired,All;SendEvent%20HTTP");
			HTTPRequest("http://$nodo_ip/?event=eventlistwrite;EventListWrite;Wildcard%20ALL,UserEvent;SendEvent%20HTTP");
			HTTPRequest("http://$nodo_ip/?event=HTTPHost%20$WEBAPP_HOST/nodo.php");
			HTTPRequest("http://$nodo_ip/?event=Filewrite%20waconfig");
			HTTPRequest("http://$nodo_ip/?event=ok");
			HTTPRequest("http://$nodo_ip/?event=Filewrite");
			
			//Sync clock
			$year_par1 = substr(date("Y"), 0, 2);
			$year_par2 = substr(date("Y"), 2, 2);
			$date_par1 = date("d");
			$date_par2 = date("m");
			$time_par1 = date("H");
			$time_par2 = date("i");
			$dow_par1 = date("w")+1; // php zondag = 0 Nodo gaat uit van 1
			
			HTTPRequest("http://$nodo_ip/?event=FileLog%20$file");
			HTTPRequest("http://$nodo_ip/?event=ClockSetYear%20$year_par1,$year_par2");
			HTTPRequest("http://$nodo_ip/?event=ClockSetDate%20$date_par1,$date_par2");
			HTTPRequest("http://$nodo_ip/?event=ClockSetTime%20$time_par1,$time_par2");
			HTTPRequest("http://$nodo_ip/?event=ClockSetDow%20$dow_par1");
			
			
		    
		
			
			if (strpos(HTTPRequest("http://$nodo_ip/?file=waconfig"), 'ok') !== false) {
				HTTPRequest("http://$nodo_ip/?event=id%20$nodo_id;password%20$nodo_password;fileerase%20waconfig;OutputIp%20HTTP,on;reboot");
						
				$response = "ok_config";	
			
			}
			else{
				
				$response = "error_config";
			
			}
						
		}
	}
	
	if (strpos($headers, 'HTTP/1.1 403 Forbidden') !== false && strpos($headers, 'Nodo/') !== false) {
		
		if (isset($_POST['auto_config'])){
		
		$response = "forbidden_config";
		}
		else{
		$response = "forbidden";
		}
		
		
	}
  
 // save the data to the database 
 mysql_select_db($database, $db);
 mysql_query("UPDATE nodo_tbl_users SET nodo_ip='$nodo_ip', nodo_port='$nodo_port', send_method='$send_method', nodo_password='$nodo_password', nodo_id='$nodo_id' WHERE id='$userId'") or die(mysql_error());   
 // once saved, redirect back to the view page 
 header("Location: ?response=$response#saved");   
 }
 
else 
{

mysql_select_db($database, $db);
$result = mysql_query("SELECT * FROM nodo_tbl_users WHERE id='$userId'") or die(mysql_error());  
$row = mysql_fetch_array($result);

//Functie om het ip-adres van de client te achterhalen
function get_ip_address() {
    foreach (array('HTTP_CLIENT_IP', 'HTTP_X_FORWARDED_FOR', 'HTTP_X_FORWARDED', 'HTTP_X_CLUSTER_CLIENT_IP', 'HTTP_FORWARDED_FOR', 'HTTP_FORWARDED', 'REMOTE_ADDR') as $key) {
        if (array_key_exists($key, $_SERVER) === true) {
            foreach (explode(',', $_SERVER[$key]) as $ip) {
                if (filter_var($ip, FILTER_VALIDATE_IP) !== false) {
                    return $ip;
                }
            }
        }
    }
}

//Indien er al een IP-adres in de database bekend is gebruiken we deze.
if ($row['nodo_ip'] != "") { 
	$nodo_ip = $row['nodo_ip'];
	$nodo_ip_message = "";
}
else 
//Indien er geen IP-adres of hostname is ingevoerd dan vullen we het IP-adres van de client in
{
	$nodo_ip = get_ip_address();
	$nodo_ip_message = "Your IP address is automatically filled. <br>Make sure that your Nodo can be reached on this IP address.<br><br>";
}

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

<div data-role="page" pageid="main" data-theme="<?php echo $theme?>">
 
	<?php require_once('../include/header_admin.php'); ?>
 
	<div data-role="content">	

	<form action="setup_connection.php" data-ajax="false" id="form_connection" method="post" > 
	
	 
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
	
		<label for="name"><?php echo $nodo_ip_message; ?>Nodo IP address/hostname: (x.x.x.x)</label>
		<input type="text" name="nodo_ip" id="nodo_ip" value="<?php echo $nodo_ip?>"  />
	
	<br>   
      
		<label for="name">TCP port: (HTTP)</label>
		    <select name="nodo_port" id="nodo_port" data-placeholder="true" data-native-menu="false">
				<option value="8080" <?php if ($row['nodo_port'] == 8080) {echo 'selected="selected"';}?>>8080</option>
				<option value="80" <?php if ($row['nodo_port'] == 80) {echo 'selected="selected"';}?>>80</option>
		    </select> 

			
	<br>
      
		<label for="name">Nodo password:</label>
		<input type="password" name="nodo_password" id="nodo_password" value="<?php echo $row['nodo_password']?>"  />
		In case of auto configuration your Nodo password will be changed to this password!<br>
		
	<br>
	   	   	
		<!-- <input type="checkbox" name="checkbox-1" id="checkbox-0" class="custom" />
		<label for="checkbox-0">Generate NoDo ID</label>
		 -->
		 <input type="checkbox" name="auto_config" id="auto_config_2" class="custom" />
		<label for="auto_config_2">Auto configure your default Nodo</label>
	 
	 <br>  
      
		<label for="name">Nodo ID: <b><?php echo $row['nodo_id']?></b></label>
		<input type="hidden" name="nodo_id" id="nodo_id" value="<?php echo $row['nodo_id']?>"  />
    
	<br><br>
        

 
		
		
		<input type="submit" name="save" value="Save & check connection" onclick="show_loading();" >
		
			
	</form> 
	
	<div align="center" id="check_connection_div" style="display: none">
	<h4><img src="../media/loading.gif"/> Please wait, checking connection....</h4>
	</div>

<script type="text/javascript">
 
function show_loading() {

	$('#form_connection').hide();
	$('#check_connection_div').show();
	//IE bug!?
	document.getElementById('check_connection_div').innerHTML = document.getElementById('check_connection_div').innerHTML;
 
}
</script>
	
	
	</div><!-- /content -->
	
	<?php include('../include/footer_admin.php'); ?>
	
</div><!-- /main page -->

<!-- Start of saved dialog: -->
<div data-role="dialog" id="saved" data-theme="<?php echo $theme?>">

	<div data-role="header" data-theme="<?php echo $theme_header?>">
		<h1><?php echo $page_title;?></h1>
	</div><!-- /header -->

	<div data-role="content">	
		<h2>Settings saved.</h2>
			
			<h2>
			<?php
			
		if (isset($_GET['response'])) {	
			switch ($_GET['response'])
			{
			case 'not_ok':
			echo "Connection failed!";
			break;
			case 'ok':
			echo "Connection succeeded!";
			break;
			case 'ok_config':
			echo "Connection succeeded!<br>Nodo configured ok";
			break;
			case 'error_config':
			echo "Connection succeeded!<br>Error: Cannot verify configuration.";
			break;
			case 'forbidden':
			echo "Authentication failed!";
			break;
			case 'forbidden_config':
			echo "Cannot configure your Nodo!<BR>Make sure your Nodo has default login settings.";
			break;
			default:
			echo "Connection failed!";
			}
		}	
			?>	
			</h2>
		<p><a href="setup_connection.php" data-role="button" data-inline="true" data-icon="back" data-ajax="false">Ok</a></p>	
	</div><!-- /content -->
	
	
</div><!-- /dialog saved -->

 
</body>
</html>