<?php

     // Initialize cURL
     $ch = curl_init();
     
     // Set some options
     curl_setopt ( $ch, CURLOPT_URL, ("http://nodo.powerkite.nl?id=9HQQCC0P&password=nodo123&event=userevent%20255,255"));
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
     
         
     
     echo $headers[0];
	 echo $headers[1];
	 echo $headers[2];
	 echo $headers[3];
	 echo $headers[4];

	 
	 
	if (strpos($headers[0], 'HTTP/1.1 500') !== false) {
	echo "Geen connectie";
	}
	
	elseif (strpos($headers[0], 'HTTP/1.1 200 Ok') !== false && strpos($headers[3], 'Nodo/3') !== false ) {
	echo "Connectie in orde";
	}
	
	elseif (strpos($headers[0], 'HTTP/1.1 403 Forbidden') !== false && strpos($headers[3], 'Nodo/3') !== false ) {
	echo "We hebben connectie maar het ID of wachtwoord is onjuist";
	}
	else {
	echo "Geen connectie";
	}
 
 ?>