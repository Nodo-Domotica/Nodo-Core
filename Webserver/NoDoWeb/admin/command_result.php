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
    curl_setopt($ch, CURLOPT_USERAGENT, "Nodo Web App");
    curl_setopt($ch, CURLOPT_HEADER, 0);
	curl_setopt($ch, CURLOPT_PORT, $nodo_port);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_TIMEOUT, 60);
 
    $output = curl_exec($ch);
    curl_close($ch);
	
	return $output;
	
	}
/************************************************************************************************
END HTTPRequest function														
*************************************************************************************************/



/************************************************************************************************
Status  read															
*************************************************************************************************/
$file="cmdres";

		if ($_GET['defcommand'] != "") {
		
			switch ($_GET['defcommand']) {
				
				
				case "1": //sync Nodo clock
				
				$year_par1 = substr(date("Y"), 0, 2);
				$year_par2 = substr(date("Y"), 2, 2);
				
				$date_par1 = date("d");
				$date_par2 = date("m");
				
				$time_par1 = date("H");
				$time_par2 = date("i");
				
				$dow_par1 = date("w")+1; // php zondag = 0 Nodo gaat uit van 1
				
				
				HTTPRequest("http://$nodo_ip/?event=FileLog%20$file&id=$nodo_id&password=$nodo_password");
				HTTPRequest("http://$nodo_ip/?event=ClockSetYear%20$year_par1,$year_par2&id=$nodo_id&password=$nodo_password");
				HTTPRequest("http://$nodo_ip/?event=ClockSetDate%20$date_par1,$date_par2&id=$nodo_id&password=$nodo_password");
				HTTPRequest("http://$nodo_ip/?event=ClockSetTime%20$time_par1,$time_par2&id=$nodo_id&password=$nodo_password");
				HTTPRequest("http://$nodo_ip/?event=ClockSetDow%20$dow_par1&id=$nodo_id&password=$nodo_password");
				HTTPRequest("http://$nodo_ip/?event=FileLog&id=$nodo_id&password=$nodo_password");
				break;
				
				case "2": //simlate day 1
				HTTPRequest("http://$nodo_ip/?event=FileLog%20$file&id=$nodo_id&password=$nodo_password");
				HTTPRequest("http://$nodo_ip/?event=SimulateDay%201&id=$nodo_id&password=$nodo_password");
				HTTPRequest("http://$nodo_ip/?event=FileLog&id=$nodo_id&password=$nodo_password");
				break;
				
				case "3": //simlate day 7
				HTTPRequest("http://$nodo_ip/?event=FileLog%20$file&id=$nodo_id&password=$nodo_password");
				HTTPRequest("http://$nodo_ip/?event=SimulateDay%207&id=$nodo_id&password=$nodo_password");
				HTTPRequest("http://$nodo_ip/?event=FileLog&id=$nodo_id&password=$nodo_password");
				break;
				
				case "4": //userevent 1,0
				HTTPRequest("http://$nodo_ip/?event=FileLog%20$file&id=$nodo_id&password=$nodo_password");
				HTTPRequest("http://$nodo_ip/?event=UserEvent%201,0&id=$nodo_id&password=$nodo_password");
				HTTPRequest("http://$nodo_ip/?event=FileLog&id=$nodo_id&password=$nodo_password");
				break;
				
				case "5": //userevent 2,0
				HTTPRequest("http://$nodo_ip/?event=FileLog%20$file&id=$nodo_id&password=$nodo_password");
				HTTPRequest("http://$nodo_ip/?event=UserEvent%202,0&id=$nodo_id&password=$nodo_password");
				HTTPRequest("http://$nodo_ip/?event=FileLog&id=$nodo_id&password=$nodo_password");
				break;
				
				case "6": //userevent 3,0
				HTTPRequest("http://$nodo_ip/?event=FileLog%20$file&id=$nodo_id&password=$nodo_password");
				HTTPRequest("http://$nodo_ip/?event=UserEvent%203,0&id=$nodo_id&password=$nodo_password");
				HTTPRequest("http://$nodo_ip/?event=FileLog&id=$nodo_id&password=$nodo_password");
				break;
				
				case "7": //userevent 4,0
				HTTPRequest("http://$nodo_ip/?event=FileLog%20$file&id=$nodo_id&password=$nodo_password");
				HTTPRequest("http://$nodo_ip/?event=UserEvent%204,0&id=$nodo_id&password=$nodo_password");
				HTTPRequest("http://$nodo_ip/?event=FileLog&id=$nodo_id&password=$nodo_password");
				break;
				
				case "8": //userevent 5,0
				HTTPRequest("http://$nodo_ip/?event=FileLog%20$file&id=$nodo_id&password=$nodo_password");
				HTTPRequest("http://$nodo_ip/?event=UserEvent%205,0&id=$nodo_id&password=$nodo_password");
				HTTPRequest("http://$nodo_ip/?event=FileLog&id=$nodo_id&password=$nodo_password");
				break;
				
				case "9": //userevent 6,0
				HTTPRequest("http://$nodo_ip/?event=FileLog%20$file&id=$nodo_id&password=$nodo_password");
				HTTPRequest("http://$nodo_ip/?event=UserEvent%206,0&id=$nodo_id&password=$nodo_password");
				HTTPRequest("http://$nodo_ip/?event=FileLog&id=$nodo_id&password=$nodo_password");
				break;
				
				case "10": //userevent 7,0
				HTTPRequest("http://$nodo_ip/?event=FileLog%20$file&id=$nodo_id&password=$nodo_password");
				HTTPRequest("http://$nodo_ip/?event=UserEvent%207,0&id=$nodo_id&password=$nodo_password");
				HTTPRequest("http://$nodo_ip/?event=FileLog&id=$nodo_id&password=$nodo_password");
				break;
				
				case "11": //userevent 8,0
				HTTPRequest("http://$nodo_ip/?event=FileLog%20$file&id=$nodo_id&password=$nodo_password");
				HTTPRequest("http://$nodo_ip/?event=UserEvent%208,0&id=$nodo_id&password=$nodo_password");
				HTTPRequest("http://$nodo_ip/?event=FileLog&id=$nodo_id&password=$nodo_password");
				break;
				
				case "12": //userevent 9,0
				HTTPRequest("http://$nodo_ip/?event=FileLog%20$file&id=$nodo_id&password=$nodo_password");
				HTTPRequest("http://$nodo_ip/?event=UserEvent%209,0&id=$nodo_id&password=$nodo_password");
				HTTPRequest("http://$nodo_ip/?event=FileLog&id=$nodo_id&password=$nodo_password");
				break;
				
				case "13": //userevent 10,0
				HTTPRequest("http://$nodo_ip/?event=FileLog%20$file&id=$nodo_id&password=$nodo_password");
				HTTPRequest("http://$nodo_ip/?event=UserEvent%2010,0&id=$nodo_id&password=$nodo_password");
				HTTPRequest("http://$nodo_ip/?event=FileLog&id=$nodo_id&password=$nodo_password");
				break;
		
			}
		
		}
		
		if ($_GET['command'] != "") {
		
			
			$command = str_replace(" ","%20",$_GET['command']);
			
			HTTPRequest("http://$nodo_ip/?event=FileLog%20$file&id=$nodo_id&password=$nodo_password");
			HTTPRequest("http://$nodo_ip/?event=$command&id=$nodo_id&password=$nodo_password");
			HTTPRequest("http://$nodo_ip/?event=FileLog&id=$nodo_id&password=$nodo_password");

		}
		
		//Read file from Nodo to array
		$scriptraw = explode("\n", HTTPRequest("http://$nodo_ip/?id=$nodo_id&password=$nodo_password&file=$file"));
		HTTPRequest("http://$nodo_ip/?event=FileErase%20$file&id=$nodo_id&password=$nodo_password");
		
			
		//count total lines in $scriptraw
		$total_script_lines_raw = count($scriptraw);
		
		$x=0;
		
		for($i=0;$i<$total_script_lines_raw;$i++){
			
			//Remove !********************************** start en stop lines 
			//$pos = strpos($scriptraw[$i],"!*****");
			
			//if($pos === false || $pos > 0) {
				$script[$x] = $scriptraw[$i];
				$x++;
			//}
			

		}
		
		$total_script_lines = count($script);
		
		

/************************************************************************************************
END status Read															
*************************************************************************************************/
?>	
<?php
if (isset($script)){  

	for($i=0;$i<$total_script_lines;$i++){
		
			
			//<br /> aan het einde van de regels verwijderen
			//$script[$i] = str_replace("<br />","",$script[$i]);
			
			echo $script[$i];
		
	}
	
}


?>