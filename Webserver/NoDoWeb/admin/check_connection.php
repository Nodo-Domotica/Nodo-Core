<?php

     // Initialize cURL
     $ch = curl_init();
     
     // Set some options
     curl_setopt ( $ch, CURLOPT_URL, ("http://nodo.powerkite.nl/?event=newkaku%201,on&id=D4KSHLJJ&password=xxxx"));
     curl_setopt ( $ch, CURLOPT_HEADER, true);
     curl_setopt ( $ch, CURLOPT_RETURNTRANSFER, true);
     curl_setopt ( $ch, CURLOPT_PORT, '80' );
     curl_setopt ( $ch, CURLOPT_TIMEOUT, 10);
     
     // Set user agent
     $agent = 'NoDo WebApp';
     curl_setopt ( $ch, CURLOPT_USERAGENT, $agent);
     
     // Perform HTTP request
     $response = curl_exec($ch);
     
     // Split headers
     $headers = explode( "\n", $response );
     
         
     
     //echo $headers[0];

	 
	 
	if (strpos($headers[0], 'HTTP/1.1 500') !== false) {
	echo "Geen connectie";
	}
	
	if (strpos($headers[0], 'HTTP/1.1 200 Ok') !== false) {
	echo "Connectie in orde";
	}
	
	if (strpos($headers[0], 'HTTP/1.1 403 Forbidden') !== false) {
	echo "We hebben connectie maar het ID of wachtwoord is onjuist";
	}
	
 
 ?>