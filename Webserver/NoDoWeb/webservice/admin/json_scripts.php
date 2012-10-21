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
    curl_setopt($ch, CURLOPT_CONNECTTIMEOUT,5);
	curl_setopt($ch, CURLOPT_TIMEOUT, 20);
 
    $output = curl_exec($ch);
    curl_close($ch);
	
	return $output;
	
	}
/************************************************************************************************
END HTTPRequest function														
*************************************************************************************************/


/************************************************************************************************
Filelist															
*************************************************************************************************/
if (isset($_GET['files'])) {
	if ($heartbeat == "ok" && $busy == 0) {

		HTTPRequest("http://$nodo_ip/?event=fileerase%20Filelst;Filelog%20Filelst;filelist;filelog&key=$key");
		$files = explode("\n", trim(HTTPRequest("http://$nodo_ip/?file=filelst&key=$key")));
		$total_files = count($files);
			
		if (isset($files)){  

			for($i=0;$i<$total_files;$i++){
		
			$files[$i] = trim($files[$i]);
					
			//Remove !********************************** lines 
			$pos = strpos($files[$i],"!*");
			
				if($pos === false || $pos > 0) {
					
					if ($files[$i] != "" && $files[$i] != "FILELST" && $files[$i] != "EVENTLST" && $files[$i]!= "STATUS" && $files[$i]!= "LOG" && $files[$i]!= "TRACE"){
					
						$rowsarray[] = array(
						"file" 		=> $files[$i]);
					
					}

				}
			}
		}

	$json = json_encode($rowsarray);
	
	}

echo '{"files":'. $json .'}'; 


	
}

/************************************************************************************************
Eventlist script read															
*************************************************************************************************/
if (isset($_POST['read'])) 
{  

	
	if ($heartbeat == "ok" && $busy == 0) {



		if ($_POST['scriptfile'] == "EVENTLST") {
		
			$file = "EVENTLST"; 

			//Read eventlist on nodo
				
			HTTPRequest("http://$nodo_ip/?event=EventListfile%20$file&key=$key");
			
		}
		else {
		
			
			$file = $_POST['scriptfile'];

		}
		
		

		//Read file from Nodo to array
		$script =  trim(HTTPRequest("http://$nodo_ip/?file=$file&key=$key"));
		
					
	}


	 if (isset($script)){  

						
				echo $script;
	 
			
		}
		
		
	}
 

/************************************************************************************************
END script Read															
*************************************************************************************************/


/************************************************************************************************
Script write or execute														
*************************************************************************************************/
if (isset($_POST['write'])) 
{  

 
 // get form data, making sure it is valid 
 $file = mysql_real_escape_string(htmlspecialchars($_POST['scriptfile'])); 
 $scriptpost = mysql_real_escape_string(htmlspecialchars($_POST['script']));
 
 
 
 mysql_select_db($database, $db);
 $result = mysql_query("SELECT * FROM nodo_tbl_scripts WHERE user_id='$userId' AND file = '$file'") or die(mysql_error());  
 
	 if (mysql_affected_rows()==0) {
		// update the data to the database 
		 mysql_query("INSERT INTO nodo_tbl_scripts (script, file, user_id) 
		 VALUES 
		 ('$scriptpost','$file','$userId')");
		 
		     
	 
	 }
 
 
	else {

		// save the data to the database 
		 mysql_select_db($database, $db);
		 mysql_query("UPDATE nodo_tbl_scripts SET script='$scriptpost' WHERE user_id='$userId' AND file='$file'") or die(mysql_error());   
		  
		 
		
	}
			
		
	
	if ($heartbeat == "ok" && $busy == 0) {	


		
		if ($_POST['checkbox']=='true'){
		
			
			//Execute script on Nodo
			HTTPRequest("http://$nodo_ip/?event=FileGetHTTP%20$file;FileExecute%20$file&key=$key");
			
		}
		
		else {
		
		//Save script on Nodo 
		HTTPRequest("http://$nodo_ip/?event=FileGetHTTP%20$file&key=$key");
		
		}
	
		
	}
	echo $file;
	echo $scriptpost;
}
/************************************************************************************************
END Script write													
*************************************************************************************************/

/************************************************************************************************
New File
*************************************************************************************************/
if (isset($_POST['new'])) {

	$file = $_POST["scriptfile"];

	HTTPRequest("http://$nodo_ip/?event=FileWrite%20$file&key=$key");
	HTTPRequest("http://$nodo_ip/?event=&key=$key");
	HTTPRequest("http://$nodo_ip/?event=FileWrite&key=$key");
	

}

/************************************************************************************************
Delete File
*************************************************************************************************/
if (isset($_POST['delete'])) {

	$file = $_POST["scriptfile"];

	HTTPRequest("http://$nodo_ip/?event=FileErase%20$file&key=$key");
	
	

}



?>

