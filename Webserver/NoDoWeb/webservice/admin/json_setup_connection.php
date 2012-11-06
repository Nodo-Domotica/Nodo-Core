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

require_once('../../connections/db_connection.php'); 
require_once('../../include/auth.php');
require_once('../../include/user_settings.php');
require_once('../../include/webapp_settings.php');

$page_title = "Setup: Communication";

$http_status; //Variable voor de HTTP status code
$build;


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

//HTTPRequest function output http headers
function HTTPRequest($Url){

	global $http_status;
    
    if (!function_exists('curl_init')){
        die('Sorry cURL is not installed!');
    }
 
    global $nodo_port;
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, $Url);
    curl_setopt($ch, CURLOPT_USERAGENT, "Nodo WebApp");
    curl_setopt($ch, CURLOPT_HEADER, 1);
	curl_setopt($ch, CURLOPT_PORT, $nodo_port);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_CONNECTTIMEOUT,5);
	curl_setopt($ch, CURLOPT_TIMEOUT, 30);
    $output = curl_exec($ch);
	$http_status = curl_getinfo($ch, CURLINFO_HTTP_CODE);
	curl_close($ch);
	
	return $output;
	
}

//Functie welke de HTTP headers in een array plaats. Een bepaalde header is op te vragen via bijvoorbeeld $headers=http_parse_headers(HTTPRequest("http://$nodo_ip/?event=status%20NodoIp")); $headers['Server'];
if (!function_exists('http_parse_headers')) {
	function http_parse_headers($header) {
		$retVal = array();
		$fields = explode("\r\n", preg_replace('/\x0D\x0A[\x09\x20]+/', ' ', $header));
		foreach ($fields as $field) {
			if (preg_match('/([^:]+): (.+)/m', $field, $match)) {
				$match[1] = preg_replace('/(?<=^|[\x09\x20\x2D])./e', 'strtoupper("\0")', strtolower(trim($match[1])));
				if (isset($retVal[$match[1]])) {
					$retVal[$match[1]] = array($retVal[$match[1]], $match[2]);
				} else {
					$retVal[$match[1]] = trim($match[2]);
				}
			}
		}
		return $retVal;
	}
}

	
function get_phrase_after_string($haystack,$needle)
//  voorbeelden:
//	$Build=get_phrase_after_string($script,'Build ');
//	$OutputHTTP=get_phrase_after_string($script,'Output HTTP,');
//	$HTTPHost=get_phrase_after_string($script,'HTTPHost ');

        {
                //length of needle
                $len = strlen($needle);
                
                //matches $needle until hits a \n or \r
                if(preg_match("#$needle([^\r\n]+)#i", $haystack, $match))
                {
                        //length of matched text
                        $rsp = strlen($match[0]);
                        
                        //determine what to remove
                        $back = $rsp - $len;
                        
                        return trim(substr($match[0],- $back));
                }
        }



if (isset($_POST['save'])) {  
 
 
 $nodo_ip = mysql_real_escape_string(htmlspecialchars($_POST['nodo_ip'])); 
 $nodo_port = mysql_real_escape_string(htmlspecialchars($_POST['nodo_port']));  
 $send_method = mysql_real_escape_string(htmlspecialchars($_POST['send_method']));
 $nodo_password = mysql_real_escape_string(htmlspecialchars($_POST['nodo_password']));
 

	//Connectie controleren.. optie om de Nodo automatisch te configureren
	if (isset($_POST['auto_config'])){
		 
		 global $build;
		 
		 //Connectie controleren van een standaard Nodo
		 $httpresponse=HTTPRequest("http://$nodo_ip/?event=status%20NodoIp");
		 $headers=http_parse_headers($httpresponse);
		 
		 $build=trim(substr(strrchr($headers['Server'], "="), 1));
		 $build=(int)$build;
			 
	}

	else {
		//Connectie controleren
		 $key = md5($cookie.":".$nodo_password); //key opnieuw genereren omdat we het wachtwoord nog niet in de DB hebben opgeslagen
		 HTTPRequest("http://$nodo_ip/?event=Filelog%20concheck;status%20HTTPHost;status%20Output%20HTTP;Filelog&key=$key");
		 $httpresponse=HTTPRequest("http://$nodo_ip/?file=concheck&key=$key");
		 HTTPRequest("http://$nodo_ip/?event=FileErase%20concheck&key=$key");
		 
		 $headers=http_parse_headers($httpresponse);
		 
		 $HTTPHost=get_phrase_after_string($httpresponse,'HTTPHost ');
		 $OutputHTTP=get_phrase_after_string($httpresponse,'Output HTTP,');
		 
		 
	}   
     
   
	
	//Controle of een verbinding met een Nodo hebben
	if ($http_status == '200' && strpos($headers['Server'], 'Nodo/') !== false) {
		
		//Controle of de configuratie van de Nodo correct is
		if ($HTTPHost == "www.nodo-domotica.nl/webapp/nodo.php" && $OutputHTTP== "On" ) { $response = "1"; } else { $response = "2"; }
		
		
		if (isset($_POST['auto_config'])){	
		
			//Configureer de Nodo
			HTTPRequest("http://$nodo_ip/?event=eventlistwrite;WildCard%20RF,All;SendEvent%20HTTP");
			HTTPRequest("http://$nodo_ip/?event=eventlistwrite;WildCard%20IR,All;SendEvent%20HTTP");
			HTTPRequest("http://$nodo_ip/?event=eventlistwrite;WildCard%20Variables,All;SendEvent%20HTTP");
			HTTPRequest("http://$nodo_ip/?event=eventlistwrite;WildCard%20Wired,All;SendEvent%20HTTP");
			HTTPRequest("http://$nodo_ip/?event=eventlistwrite;EventListWrite;Wildcard%20ALL,UserEvent;SendEvent%20HTTP");
			HTTPRequest("http://$nodo_ip/?event=HTTPHost%20$WEBAPP_HOST/nodo.php");
			HTTPRequest("http://$nodo_ip/?event=Filewrite%20waconfig");
			HTTPRequest("http://$nodo_ip/?event=ok");
			HTTPRequest("http://$nodo_ip/?event=Filewrite");
			
			//Clock gelijk zetten
			$year_par1 = substr(date("Y"), 0, 2);
			$year_par2 = substr(date("Y"), 2, 2);
				
			$date_par1 = date("j");
			$date_par2 = date("n");
				
			$time_par1 = date("G");
			$time_par2 = 1*date("i"); //*1 zorgt ervoor dat de voorloop nul wegvalt.
								
			$dow_par1 = date("w")+1; // php zondag = 0 Nodo gaat uit van 1
			
			HTTPRequest("http://$nodo_ip/?event=FileLog%20$file");
			HTTPRequest("http://$nodo_ip/?event=ClockSetYear%20$year_par1,$year_par2");
			HTTPRequest("http://$nodo_ip/?event=ClockSetDate%20$date_par1,$date_par2");
			HTTPRequest("http://$nodo_ip/?event=ClockSetTime%20$time_par1,$time_par2");
			HTTPRequest("http://$nodo_ip/?event=ClockSetDow%20$dow_par1");
						
			if (strpos(HTTPRequest("http://$nodo_ip/?file=waconfig"), 'ok') !== false) {
				
				global $build;
				
								
				if ($build >= 443 && $build < 455) { // vanaf build 443
				
					HTTPRequest("http://$nodo_ip/?event=id%20$nodo_id;password%20$nodo_password;fileerase%20waconfig;Output%20HTTP,on;reboot"); 
				
				}
					
				elseif 	($build >= 455) { //vanaf build 455
					
					HTTPRequest("http://$nodo_ip/?event=id%20$nodo_id;password%20$nodo_password;fileerase%20waconfig;Output%20HTTP,on;SettingsSave;reboot");
					
				}
				
				else {
					HTTPRequest("http://$nodo_ip/?event=id%20$nodo_id;password%20$nodo_password;fileerase%20waconfig;OutputIp%20HTTP,on;reboot"); //oude manier
				}
				
				
						
				$response = "6";	
			
			}
			else{
				
				$response = "7";
			
			}
						
		}
	}
else {
 
	$response = "3";
}
	
if ($http_status == '403' && strpos($headers['Server'], 'Nodo/') !== false) {
		
		if (isset($_POST['auto_config'])){
		
			$response = "5";
		}
		else{
			$response = "4";
		}
		
}
  
 //Gegevens opslaan in de database
 mysql_select_db($database, $db);
 mysql_query("UPDATE nodo_tbl_users SET nodo_ip='$nodo_ip', nodo_port='$nodo_port', send_method='$send_method', nodo_password='$nodo_password' WHERE id='$userId'") or die(mysql_error());   
 
    
 
 

	$rowsarray[] = array(
	
	"response"		=> $response);
	 
	 
	$json = json_encode($rowsarray);

	echo '{"connection":'. $json .'}'; 

 
 }
 
else 
{

mysql_select_db($database, $db);
$result = mysql_query("SELECT * FROM nodo_tbl_users WHERE id='$userId'") or die(mysql_error());  
$row = mysql_fetch_array($result);


//Indien er al een IP-adres in de database bekend is gebruiken we deze.
if ($row['nodo_ip'] != "") { 
	$nodo_ip = $row['nodo_ip'];
	$nodo_ip_message = "";
}
else 
//Indien er geen IP-adres of hostname is ingevoerd dan vullen we het IP-adres van de client in
{
	$nodo_ip = get_ip_address();
	$nodo_ip_message = "Your IP-address is automatically filled. <br \>Make sure that your Nodo can be reached on this IP-address.<br \><br \>";
}

$rowsarray[] = array(
"nodoid" 		=> $row['nodo_id'],
"nodoip" 		=> $nodo_ip,
"nodoipmsg" 	=> $nodo_ip_message,
"nodoport" 		=> $row['nodo_port'],
"nodopassword"	=> $row['nodo_password']);
 
 
$json = json_encode($rowsarray);

echo '{"connection":'. $json .'}'; 

}

?>
