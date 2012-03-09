<?php 

require_once('connections/tc.php'); 
require_once('include/auth.php');
require_once('include/user_settings.php');

$page_title = "Setup: Nodo settings";	


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
    curl_setopt($ch, CURLOPT_TIMEOUT, 10);
 
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

		$file="STATUS";
		HTTPRequest("http://$nodo_ip/?id=$nodo_id&password=$nodo_password&event=FileErase%20$file");
		HTTPRequest("http://$nodo_ip/?id=$nodo_id&password=$nodo_password&event=FileLog%20$file");
		HTTPRequest("http://$nodo_ip/?id=$nodo_id&password=$nodo_password&event=Status%20All");
		HTTPRequest("http://$nodo_ip/?id=$nodo_id&password=$nodo_password&event=FileLog");
		
			

		//Read file from Nodo to array
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