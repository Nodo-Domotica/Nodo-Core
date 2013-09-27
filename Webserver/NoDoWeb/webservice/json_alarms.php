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
 


if (isset($_GET['alarm'])) {

	$nr = $_GET['alarm'];

	
			 $httpresponse=str_replace("<br>","",(HTTPRequest("http://$nodo_ip/?event=status%20alarmset%20".$nr."&key=$key")));
			 
					 
			 $Alarm=get_phrase_after_string($httpresponse,"AlarmSet ".$nr.",");
			 
			 sscanf($Alarm,"%[^,],%[^,],%s",$state,$time,$day);
			
			 $hour = substr($time,0,2); 
			 $minute = substr($time,3);
			 
			 
				
		 
	$rowsarray[] = array(
		"state" 		=> $state,
		"hour"			=> $hour,
		"minute"		=> $minute,
		"day"			=> $day);
		 
		$json = json_encode($rowsarray);

		echo '{"alarm":'. $json .'}'; 
}

if (isset($_GET['alarms'])) {

	

	
			 $httpresponse=str_replace("<br>","",(HTTPRequest("http://$nodo_ip/?event=status%20alarmset%20&key=$key")));
			 
			$alarm1 = get_phrase_after_string($httpresponse,"AlarmSet 1,");
			$alarm2 = get_phrase_after_string($httpresponse,"AlarmSet 2,");
			$alarm3 = get_phrase_after_string($httpresponse,"AlarmSet 3,");
			$alarm4 = get_phrase_after_string($httpresponse,"AlarmSet 4,");	
			$alarm5 = get_phrase_after_string($httpresponse,"AlarmSet 5,");
			$alarm6 = get_phrase_after_string($httpresponse,"AlarmSet 6,");
			$alarm7 = get_phrase_after_string($httpresponse,"AlarmSet 7,");
			$alarm8 = get_phrase_after_string($httpresponse,"AlarmSet 8,");				
			
			sscanf($alarm1,"%[^,],%[^,],%s",$state1,$time1,$day1);
			sscanf($alarm2,"%[^,],%[^,],%s",$state2,$time2,$day2);
			sscanf($alarm3,"%[^,],%[^,],%s",$state3,$time3,$day3);
			sscanf($alarm4,"%[^,],%[^,],%s",$state4,$time4,$day4);
			sscanf($alarm5,"%[^,],%[^,],%s",$state5,$time5,$day5);
			sscanf($alarm6,"%[^,],%[^,],%s",$state6,$time6,$day6);
			sscanf($alarm7,"%[^,],%[^,],%s",$state7,$time7,$day7);
			sscanf($alarm8,"%[^,],%[^,],%s",$state8,$time8,$day8);
			
			$hour1 = substr($time1,0,2); 
			$minute1 = substr($time1,3);
			
			$hour2 = substr($time2,0,2); 
			$minute2 = substr($time2,3);
			
			$hour3 = substr($time3,0,2); 
			$minute3 = substr($time3,3);
			
			$hour4 = substr($time4,0,2); 
			$minute4 = substr($time4,3);
			
			$hour5 = substr($time5,0,2); 
			$minute5 = substr($time5,3);
			
			$hour6 = substr($time6,0,2); 
			$minute6 = substr($time6,3);
			
			$hour7 = substr($time7,0,2); 
			$minute7 = substr($time7,3);
			
			$hour8 = substr($time8,0,2); 
			$minute8 = substr($time8,3);
					 
			 

			 
		$Alarms = array(
        array('alarmnr' => '1','state' => $state1,'hour' => $hour1,'minute' => $minute1,'day' => $day1),
        array('alarmnr' => '2','state' => $state2,'hour' => $hour2,'minute' => $minute2,'day' => $day2),
		array('alarmnr' => '3','state' => $state3,'hour' => $hour3,'minute' => $minute3,'day' => $day3),
		array('alarmnr' => '4','state' => $state4,'hour' => $hour4,'minute' => $minute4,'day' => $day4),
		array('alarmnr' => '5','state' => $state5,'hour' => $hour5,'minute' => $minute5,'day' => $day5),
        array('alarmnr' => '6','state' => $state6,'hour' => $hour6,'minute' => $minute6,'day' => $day6),
		array('alarmnr' => '7','state' => $state7,'hour' => $hour7,'minute' => $minute7,'day' => $day7),
		array('alarmnr' => '8','state' => $state8,'hour' => $hour8,'minute' => $minute8,'day' => $day8));
		
			 
	
		$json = json_encode($Alarms);

		echo '{"alarms":'. $json .'}'; 
}
